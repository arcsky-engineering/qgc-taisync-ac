/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(TelemetryLogManagerLog)

class QmlObjectListModel;

/// Represents a single telemetry log file (.tlog) on disk.
class TelemetryLogEntry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString   name         READ name         CONSTANT)
    Q_PROPERTY(QString   filePath     READ filePath     CONSTANT)
    Q_PROPERTY(quint64   size         READ size         CONSTANT)
    Q_PROPERTY(QString   sizeStr      READ sizeStr      CONSTANT)
    Q_PROPERTY(QDateTime lastModified READ lastModified  CONSTANT)
    Q_PROPERTY(bool      selected     READ selected     WRITE setSelected NOTIFY selectedChanged)

public:
    explicit TelemetryLogEntry(const QString &filePath, quint64 size, const QDateTime &lastModified, QObject *parent = nullptr);

    QString   name()         const { return _name; }
    QString   filePath()     const { return _filePath; }
    quint64   size()         const { return _size; }
    QString   sizeStr()      const;
    QDateTime lastModified() const { return _lastModified; }
    bool      selected()     const { return _selected; }
    void      setSelected(bool selected);

signals:
    void selectedChanged();

private:
    QString   _name;
    QString   _filePath;
    quint64   _size;
    QDateTime _lastModified;
    bool      _selected = false;
};

/// Controller for managing saved telemetry log files (.tlog).
/// Instantiated from QML in TelemetrySettings — reads the telemetry save
/// directory and provides list/select/delete operations.
class TelemetryLogManager : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("QmlObjectListModel.h")

    Q_PROPERTY(QmlObjectListModel *logFiles      READ logFiles      NOTIFY logFilesChanged)
    Q_PROPERTY(int                 selectedCount READ selectedCount NOTIFY selectedCountChanged)
    Q_PROPERTY(int                 totalCount    READ totalCount    NOTIFY logFilesChanged)
    Q_PROPERTY(QString             totalSizeStr  READ totalSizeStr  NOTIFY logFilesChanged)
    Q_PROPERTY(bool                isCapturing   READ isCapturing   NOTIFY isCapturingChanged)

public:
    explicit TelemetryLogManager(QObject *parent = nullptr);
    ~TelemetryLogManager();

    QmlObjectListModel *logFiles() { return _logFiles; }
    int     selectedCount() const { return _selectedCount; }
    int     totalCount()    const;
    QString totalSizeStr()  const;
    bool    isCapturing()   const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteSelected();
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void selectNone();
    Q_INVOKABLE void startCapture();
    Q_INVOKABLE void stopAndSaveCapture();

signals:
    void logFilesChanged();
    void selectedCountChanged();
    void isCapturingChanged();

private:
    void _updateSelectedCount();

    QmlObjectListModel *_logFiles = nullptr;
    int _selectedCount = 0;
};
