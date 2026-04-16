/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "TelemetryLogManager.h"

#include "AppSettings.h"
#include "MAVLinkProtocol.h"
#include "QGCLoggingCategory.h"
#include "QmlObjectListModel.h"
#include "SettingsManager.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

QGC_LOGGING_CATEGORY(TelemetryLogManagerLog, "qgc.settings.telemetrylogmanager")

// ----------------------------------------------------------------------------
// TelemetryLogEntry
// ----------------------------------------------------------------------------

TelemetryLogEntry::TelemetryLogEntry(const QString &filePath, quint64 size, const QDateTime &lastModified, QObject *parent)
    : QObject(parent)
    , _name(QFileInfo(filePath).fileName())
    , _filePath(filePath)
    , _size(size)
    , _lastModified(lastModified)
{
}

QString TelemetryLogEntry::sizeStr() const
{
    if (_size < 1024) {
        return QStringLiteral("%1 B").arg(_size);
    } else if (_size < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(_size / 1024.0, 0, 'f', 1);
    } else {
        return QStringLiteral("%1 MB").arg(_size / (1024.0 * 1024.0), 0, 'f', 1);
    }
}

void TelemetryLogEntry::setSelected(bool selected)
{
    if (_selected != selected) {
        _selected = selected;
        emit selectedChanged();
    }
}

// ----------------------------------------------------------------------------
// TelemetryLogManager
// ----------------------------------------------------------------------------

TelemetryLogManager::TelemetryLogManager(QObject *parent)
    : QObject(parent)
    , _logFiles(new QmlObjectListModel(this))
{
    (void) connect(MAVLinkProtocol::instance(), &MAVLinkProtocol::manualCaptureActiveChanged,
                   this, &TelemetryLogManager::isCapturingChanged);
    refresh();
}

TelemetryLogManager::~TelemetryLogManager()
{
}

int TelemetryLogManager::totalCount() const
{
    return _logFiles->count();
}

QString TelemetryLogManager::totalSizeStr() const
{
    quint64 total = 0;
    for (int i = 0; i < _logFiles->count(); i++) {
        const auto *entry = qobject_cast<TelemetryLogEntry *>(_logFiles->get(i));
        if (entry) {
            total += entry->size();
        }
    }

    if (total < 1024) {
        return QStringLiteral("%1 B").arg(total);
    } else if (total < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(total / 1024.0, 0, 'f', 1);
    } else if (total < 1024ull * 1024 * 1024) {
        return QStringLiteral("%1 MB").arg(total / (1024.0 * 1024.0), 0, 'f', 1);
    } else {
        return QStringLiteral("%1 GB").arg(total / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
    }
}

void TelemetryLogManager::refresh()
{
    _logFiles->clearAndDeleteContents();

    const QString savePath = SettingsManager::instance()->appSettings()->telemetrySavePath();
    if (savePath.isEmpty()) {
        emit logFilesChanged();
        _updateSelectedCount();
        return;
    }

    const QDir saveDir(savePath);
    if (!saveDir.exists()) {
        emit logFilesChanged();
        _updateSelectedCount();
        return;
    }

    const QString filter = QStringLiteral("*.%1").arg(AppSettings::telemetryFileExtension);
    QFileInfoList fileInfoList = saveDir.entryInfoList(QStringList(filter), QDir::Files, QDir::Time);

    for (const QFileInfo &fileInfo : fileInfoList) {
        auto *entry = new TelemetryLogEntry(fileInfo.absoluteFilePath(), fileInfo.size(), fileInfo.lastModified(), _logFiles);
        (void) connect(entry, &TelemetryLogEntry::selectedChanged, this, &TelemetryLogManager::_updateSelectedCount);
        _logFiles->append(entry);
    }

    emit logFilesChanged();
    _updateSelectedCount();
}

void TelemetryLogManager::deleteSelected()
{
    for (int i = _logFiles->count() - 1; i >= 0; i--) {
        auto *entry = qobject_cast<TelemetryLogEntry *>(_logFiles->get(i));
        if (entry && entry->selected()) {
            const QString path = entry->filePath();
            if (QFile::remove(path)) {
                qCDebug(TelemetryLogManagerLog) << "Deleted telemetry log:" << path;
            } else {
                qCWarning(TelemetryLogManagerLog) << "Failed to delete telemetry log:" << path;
            }
            _logFiles->removeAt(i);
            entry->deleteLater();
        }
    }

    emit logFilesChanged();
    _updateSelectedCount();
}

void TelemetryLogManager::selectAll()
{
    for (int i = 0; i < _logFiles->count(); i++) {
        auto *entry = qobject_cast<TelemetryLogEntry *>(_logFiles->get(i));
        if (entry) {
            entry->setSelected(true);
        }
    }
}

void TelemetryLogManager::selectNone()
{
    for (int i = 0; i < _logFiles->count(); i++) {
        auto *entry = qobject_cast<TelemetryLogEntry *>(_logFiles->get(i));
        if (entry) {
            entry->setSelected(false);
        }
    }
}

bool TelemetryLogManager::isCapturing() const
{
    return MAVLinkProtocol::instance()->manualCaptureActive();
}

void TelemetryLogManager::startCapture()
{
    MAVLinkProtocol::instance()->startManualCapture();
}

void TelemetryLogManager::stopAndSaveCapture()
{
    MAVLinkProtocol::instance()->stopManualCapture();
    refresh();
}

void TelemetryLogManager::_updateSelectedCount()
{
    int count = 0;
    for (int i = 0; i < _logFiles->count(); i++) {
        const auto *entry = qobject_cast<TelemetryLogEntry *>(_logFiles->get(i));
        if (entry && entry->selected()) {
            count++;
        }
    }

    if (_selectedCount != count) {
        _selectedCount = count;
        emit selectedCountChanged();
    }
}
