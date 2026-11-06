/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AirDataSyncManager.h"

#include "AirDataSettings.h"
#include "Fact.h"
#include "AppSettings.h"
#include "MAVLinkProtocol.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"
#include "QmlObjectListModel.h"
#include "SettingsManager.h"
#include "Vehicle.h"

#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QMap>
#include <QtCore/qapplicationstatic.h>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkInformation>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

QGC_LOGGING_CATEGORY(AirDataSyncManagerLog, "qgc.settings.airdatasyncmanager")

Q_APPLICATION_STATIC(AirDataSyncManager, _airDataSyncManagerInstance);

namespace {

/// Digs the human readable message out of an AirData error body.
///
/// They are not consistent about the key: the upload endpoint answers with
/// {"Error":"Error #87, Invalid/missing appkey"} while other endpoints use a
/// lowercase "error". Checking only one of them throws away the only useful
/// part of a failure and leaves the user staring at "Internal Server Error".
QString airDataErrorMessage(const QJsonObject &object)
{
    for (const QString &key : {QStringLiteral("Error"), QStringLiteral("error"),
                               QStringLiteral("reason"), QStringLiteral("message")}) {
        const QString value = object.value(key).toString();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return QString();
}

/// Builds one text/plain part of the multipart upload body.
QHttpPart createFormPart(const QString &name, const QString &value)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"%1\"").arg(name));
    part.setBody(value.toUtf8());
    return part;
}

} // namespace

// ----------------------------------------------------------------------------
// AirDataSyncLedger
// ----------------------------------------------------------------------------

QString AirDataSyncLedger::_key(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

void AirDataSyncLedger::load(const QString &telemetryDir)
{
    _entries.clear();
    _filePath = QDir(telemetryDir).absoluteFilePath(QStringLiteral(".airdata_sync.json"));

    QFile file(_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        // No ledger yet, or it is unreadable. Either way we start from empty and
        // at worst re-upload files AirData will dedupe on its side.
        return;
    }

    const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    const QJsonObject files = root.value(QStringLiteral("files")).toObject();

    for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
        const QJsonObject object = it.value().toObject();
        Entry entry;
        entry.size       = static_cast<quint64>(object.value(QStringLiteral("size")).toDouble());
        entry.uploaded   = object.value(QStringLiteral("uploaded")).toBool();
        entry.jobId      = object.value(QStringLiteral("jobId")).toString();
        entry.uploadedAt = QDateTime::fromString(object.value(QStringLiteral("uploadedAt")).toString(), Qt::ISODate);
        entry.attempts   = object.value(QStringLiteral("attempts")).toInt();
        entry.lastError  = object.value(QStringLiteral("lastError")).toString();
        entry.pilotId    = object.value(QStringLiteral("pilotId")).toString();
        _entries.insert(it.key(), entry);
    }

    qCDebug(AirDataSyncManagerLog) << "Ledger loaded" << _entries.count() << "entries from" << _filePath;
}

void AirDataSyncLedger::save() const
{
    if (_filePath.isEmpty()) {
        return;
    }

    QJsonObject files;
    for (auto it = _entries.constBegin(); it != _entries.constEnd(); ++it) {
        QJsonObject object;
        object.insert(QStringLiteral("size"), static_cast<double>(it.value().size));
        object.insert(QStringLiteral("uploaded"), it.value().uploaded);
        if (!it.value().jobId.isEmpty()) {
            object.insert(QStringLiteral("jobId"), it.value().jobId);
        }
        if (it.value().uploadedAt.isValid()) {
            object.insert(QStringLiteral("uploadedAt"), it.value().uploadedAt.toString(Qt::ISODate));
        }
        object.insert(QStringLiteral("attempts"), it.value().attempts);
        if (!it.value().lastError.isEmpty()) {
            object.insert(QStringLiteral("lastError"), it.value().lastError);
        }
        if (!it.value().pilotId.isEmpty()) {
            object.insert(QStringLiteral("pilotId"), it.value().pilotId);
        }
        files.insert(it.key(), object);
    }

    QJsonObject root;
    root.insert(QStringLiteral("version"), 1);
    root.insert(QStringLiteral("files"), files);

    // QSaveFile so a power cut mid-write cannot leave a truncated ledger behind.
    QSaveFile file(_filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(AirDataSyncManagerLog) << "Could not write sync ledger:" << file.errorString();
        return;
    }
    (void) file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        qCWarning(AirDataSyncManagerLog) << "Could not commit sync ledger:" << file.errorString();
    }
}

bool AirDataSyncLedger::isUploaded(const QString &filePath) const
{
    const auto it = _entries.constFind(_key(filePath));
    if (it == _entries.constEnd()) {
        return false;
    }

    // A name match with a different size means this is a different file that
    // happens to reuse the name, so treat it as not yet uploaded.
    const quint64 size = static_cast<quint64>(QFileInfo(filePath).size());
    return it.value().uploaded && (it.value().size == size);
}

AirDataSyncLedger::Entry AirDataSyncLedger::entry(const QString &filePath) const
{
    return _entries.value(_key(filePath));
}

void AirDataSyncLedger::recordPilot(const QString &filePath, const QString &pilotId)
{
    Entry entry = _entries.value(_key(filePath));
    entry.size    = static_cast<quint64>(QFileInfo(filePath).size());
    entry.pilotId = pilotId;
    _entries.insert(_key(filePath), entry);
    save();
}

void AirDataSyncLedger::recordSuccess(const QString &filePath, const QString &jobId)
{
    Entry entry = _entries.value(_key(filePath));
    entry.size       = static_cast<quint64>(QFileInfo(filePath).size());
    entry.uploaded   = true;
    entry.jobId      = jobId;
    entry.uploadedAt = QDateTime::currentDateTimeUtc();
    entry.lastError.clear();
    _entries.insert(_key(filePath), entry);
    save();
}

void AirDataSyncLedger::recordFailure(const QString &filePath, const QString &error)
{
    Entry entry = _entries.value(_key(filePath));
    entry.size      = static_cast<quint64>(QFileInfo(filePath).size());
    entry.uploaded  = false;
    entry.attempts += 1;
    entry.lastError = error;
    _entries.insert(_key(filePath), entry);
    save();
}

void AirDataSyncLedger::clearFailures()
{
    bool changed = false;
    for (auto it = _entries.begin(); it != _entries.end(); ++it) {
        if (!it.value().uploaded && (it.value().attempts > 0)) {
            it.value().attempts = 0;
            it.value().lastError.clear();
            changed = true;
        }
    }
    if (changed) {
        save();
    }
}

void AirDataSyncLedger::pruneMissing(const QStringList &existingFilePaths)
{
    QSet<QString> existing;
    for (const QString &filePath : existingFilePaths) {
        (void) existing.insert(_key(filePath));
    }

    bool changed = false;
    for (auto it = _entries.begin(); it != _entries.end(); ) {
        if (!existing.contains(it.key())) {
            it = _entries.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    if (changed) {
        save();
    }
}

// ----------------------------------------------------------------------------
// AirDataSyncManager
// ----------------------------------------------------------------------------

AirDataSyncManager::AirDataSyncManager(QObject *parent)
    : QObject(parent)
    , _networkManager(new QNetworkAccessManager(this))
{
}

AirDataSyncManager::~AirDataSyncManager()
{
}

AirDataSyncManager *AirDataSyncManager::instance()
{
    return _airDataSyncManagerInstance();
}

void AirDataSyncManager::init()
{
    _loadSavedTokens();

    // The telemetry directory does not exist until AppSettings has built it, so
    // the ledger is loaded here rather than in the constructor.
    const QString telemetryDir = SettingsManager::instance()->appSettings()->telemetrySavePath();
    if (!telemetryDir.isEmpty()) {
        _ledger.load(telemetryDir);
        _ledger.pruneMissing(_telemetryLogPaths());
        _ledgerLoaded = true;
    }

    // Each flight's log is saved on the arm to disarm transition, so this fires
    // once per flight with the vehicle already disarmed.
    (void) connect(MAVLinkProtocol::instance(), &MAVLinkProtocol::telemetryLogSaved,
                   this, &AirDataSyncManager::_onTelemetryLogSaved);

    // Arming pauses a transfer that is already running; the start-of-sync check
    // alone would let a long backlog carry on uploading through the next flight.
    MultiVehicleManager *const vehicleManager = MultiVehicleManager::instance();
    (void) connect(vehicleManager, &MultiVehicleManager::vehicleAdded, this, &AirDataSyncManager::_onVehicleAdded);
    // A link lost mid-flight never reports a disarm, so re-check when a vehicle goes away.
    (void) connect(vehicleManager, &MultiVehicleManager::vehicleRemoved, this, &AirDataSyncManager::_onArmStateMayHaveChanged);
    QmlObjectListModel *const vehicles = vehicleManager->vehicles();
    for (int i = 0; i < vehicles->count(); i++) {
        _onVehicleAdded(vehicles->value<Vehicle*>(i));
    }

    _periodicTimer = new QTimer(this);
    _periodicTimer->setInterval(kPeriodicIntervalMs);
    (void) connect(_periodicTimer, &QTimer::timeout, this, &AirDataSyncManager::_onPeriodicCheck);
    _periodicTimer->start();

    // Draining the backlog the moment the controller gets a network back is the
    // single most useful trigger, since these machines spend days offline.
    if (!QNetworkInformation::availableBackends().isEmpty() && QNetworkInformation::loadDefaultBackend()) {
        (void) connect(QNetworkInformation::instance(), &QNetworkInformation::reachabilityChanged,
                       this, &AirDataSyncManager::_onReachabilityChanged);
    } else {
        qCDebug(AirDataSyncManagerLog) << "No QNetworkInformation backend; falling back to the periodic check";
    }

    // Turning sync on, or fixing a token, should act immediately rather than
    // waiting out the ten minute timer.
    AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
    if (settings) {
        (void) connect(settings->airDataUploadToken(), &Fact::rawValueChanged, this, [this]() {
            emit savedTokensChanged();  // currentSavedTokenIndex follows the active token
            validateToken();
            _considerAutoSync();
        });
        (void) connect(settings->airDataSyncEnabled(), &Fact::rawValueChanged, this, [this]() {
            _considerAutoSync();
        });
        (void) connect(settings->airDataAutoUpload(), &Fact::rawValueChanged, this, [this, settings]() {
            if (settings->airDataAutoUpload()->rawValue().toBool()) {
                _considerAutoSync();
            } else {
                // Off means "I'll choose", so stop what auto sync already started.
                _dropAutomaticUploads();
            }
        });
    }

    // First sweep once links and settings have settled after boot.
    QTimer::singleShot(kStartupDelayMs, this, [this]() { _considerAutoSync(); });

    // Validating on startup turns a stale or mistyped token into a visible message
    // before the operator goes looking for flights that never arrived.
    validateToken();
}

QStringList AirDataSyncManager::_telemetryLogPaths()
{
    const QString telemetryDir = SettingsManager::instance()->appSettings()->telemetrySavePath();
    if (telemetryDir.isEmpty()) {
        return QStringList();
    }

    const QDir dir(telemetryDir);
    if (!dir.exists()) {
        return QStringList();
    }

    const QString filter = QStringLiteral("*.%1").arg(AppSettings::telemetryFileExtension);
    const QFileInfoList fileInfoList = dir.entryInfoList(QStringList(filter), QDir::Files, QDir::Time);

    QStringList paths;
    paths.reserve(fileInfoList.count());
    for (const QFileInfo &fileInfo : fileInfoList) {
        paths.append(fileInfo.absoluteFilePath());
    }
    return paths;
}

bool AirDataSyncManager::_autoSyncAllowed()
{
    AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
    if (!settings) {
        return false;
    }

    if (!settings->airDataSyncEnabled()->rawValue().toBool()) {
        _setHoldReason(QString());
        return false;
    }

    if (!settings->airDataAutoUpload()->rawValue().toBool()) {
        _setHoldReason(QString());
        return false;
    }

    // A saved pilot's backlog can still go up while no token is active, so only
    // hold when there is no token anywhere.
    if (_appKey().isEmpty() || (_userToken().isEmpty() && _savedTokens.isEmpty())) {
        _setHoldReason(tr("Waiting for an Auto Upload Token."));
        return false;
    }

    // Never compete with a flight for the uplink or the cpu.
    if (_anyVehicleArmed()) {
        _setHoldReason(tr("Waiting until the vehicle is disarmed."));
        return false;
    }

    QNetworkInformation *const networkInfo = QNetworkInformation::instance();
    if (networkInfo) {
        if (networkInfo->reachability() == QNetworkInformation::Reachability::Disconnected) {
            _setHoldReason(tr("Waiting for a network connection."));
            return false;
        }

        if (settings->airDataWifiOnly()->rawValue().toBool()) {
            // "Metered" is the direct expression of "this costs the operator data".
            // Where the platform cannot tell us, fall back to the transport, and if
            // it cannot tell us that either, allow the upload rather than stalling
            // sync forever on a controller whose backend reports nothing.
            if (networkInfo->supports(QNetworkInformation::Feature::Metered)) {
                if (networkInfo->isMetered()) {
                    _setHoldReason(tr("Waiting for an unmetered network."));
                    return false;
                }
            } else if (networkInfo->supports(QNetworkInformation::Feature::TransportMedium)) {
                const QNetworkInformation::TransportMedium medium = networkInfo->transportMedium();
                const bool unmetered = (medium == QNetworkInformation::TransportMedium::WiFi) ||
                                       (medium == QNetworkInformation::TransportMedium::Ethernet);
                if (!unmetered) {
                    _setHoldReason(tr("Waiting for Wi-Fi."));
                    return false;
                }
            }
        }
    }

    _setHoldReason(QString());
    return true;
}

void AirDataSyncManager::_considerAutoSync()
{
    if (_uploading || !_autoSyncAllowed()) {
        return;
    }

    _ensureLedgerLoaded();
    if (!_ledgerLoaded) {
        return;
    }

    QList<PendingUpload> pending;
    for (const QString &filePath : _telemetryLogPaths()) {
        if (_ledger.isUploaded(filePath)) {
            continue;
        }
        // Stop hammering a file that keeps failing. "Sync Now" clears this.
        if (_ledger.entry(filePath).attempts >= kMaxAttempts) {
            continue;
        }
        // Its pilot was forgotten. Sending it under whoever is active now is
        // exactly the misattribution pilot tracking exists to prevent.
        const QString token = _tokenForFile(filePath);
        if (token.isEmpty()) {
            continue;
        }
        pending.append({filePath, token, true});
    }

    if (pending.isEmpty()) {
        return;
    }

    qCDebug(AirDataSyncManagerLog) << "Auto sync queuing" << pending.count() << "logs";
    _enqueue(pending);
}

void AirDataSyncManager::_onTelemetryLogSaved(const QString &filePath)
{
    // Record who flew this now, while their token is still the active one. A
    // token AirData has already rejected is left off, so the log falls back to
    // whichever pilot is active once someone fixes it.
    const QString token = _userToken();
    if (!token.isEmpty() && (_tokenState != TokenInvalid)) {
        _ensureLedgerLoaded();
        if (_ledgerLoaded) {
            _ledger.recordPilot(filePath, _pilotId(token));
        }
        // A token typed in while offline never got validated, and so was never
        // saved. Save it now, or this log could not be matched back to it later.
        _rememberToken(token);
    }

    // Landing is usually followed by battery swaps and re-arming, so leave a gap
    // before starting to push bytes rather than uploading the instant we land.
    QTimer::singleShot(kAfterFlightDelayMs, this, [this]() { _considerAutoSync(); });
}

void AirDataSyncManager::_onVehicleAdded(Vehicle *vehicle)
{
    if (!vehicle) {
        return;
    }

    (void) connect(vehicle, &Vehicle::armedChanged, this, &AirDataSyncManager::_onArmStateMayHaveChanged);
    // A vehicle can show up already armed, e.g. after a link drop and reconnect in flight.
    _onArmStateMayHaveChanged();
}

bool AirDataSyncManager::_anyVehicleArmed()
{
    QmlObjectListModel *const vehicles = MultiVehicleManager::instance()->vehicles();
    for (int i = 0; i < vehicles->count(); i++) {
        const Vehicle *const vehicle = vehicles->value<Vehicle*>(i);
        if (vehicle && vehicle->armed()) {
            return true;
        }
    }
    return false;
}

void AirDataSyncManager::_onArmStateMayHaveChanged()
{
    const bool armed = _anyVehicleArmed();
    if (armed == _pausedForFlight) {
        return;
    }

    _pausedForFlight = armed;

    if (armed) {
        if (_uploading || !_queue.isEmpty()) {
            qCDebug(AirDataSyncManagerLog) << "Vehicle armed, pausing AirData uploads";
            _setHoldReason(tr("Paused while the vehicle is armed."));
        }
        // _finishCurrentFile sees the pause, puts the file back at the head of the
        // queue and stops there, without spending one of its attempts.
        if (_currentReply) {
            _currentReply->abort();
        }
        return;
    }

    qCDebug(AirDataSyncManagerLog) << "All vehicles disarmed, AirData uploads may resume";
    // Either the pause or the start-of-sync check may have put up an armed message.
    if ((_holdReason == tr("Paused while the vehicle is armed.")) ||
        (_holdReason == tr("Waiting until the vehicle is disarmed."))) {
        _setHoldReason(QString());
    }

    // Same gap as after a landing: leave room for a battery swap and re-arm.
    QTimer::singleShot(kAfterFlightDelayMs, this, [this]() {
        _startQueueIfIdle();
        _considerAutoSync();
    });
}

void AirDataSyncManager::_onPeriodicCheck()
{
    _considerAutoSync();
}

void AirDataSyncManager::_onReachabilityChanged()
{
    QNetworkInformation *const networkInfo = QNetworkInformation::instance();
    if (networkInfo && (networkInfo->reachability() == QNetworkInformation::Reachability::Online)) {
        qCDebug(AirDataSyncManagerLog) << "Network came back, checking for logs to sync";
        _considerAutoSync();
    }
}

void AirDataSyncManager::syncNow()
{
    _ensureLedgerLoaded();

    // The user asking explicitly is the signal that whatever was wrong may now be
    // fixed, so give previously exhausted files a fresh set of attempts.
    _ledger.clearFailures();

    // Unlike Upload Selected this sweeps everything, so logs whose pilot was
    // forgotten are still left alone rather than swept into the active account.
    QList<PendingUpload> pending;
    int orphaned = 0;
    for (const QString &filePath : _telemetryLogPaths()) {
        if (_ledger.isUploaded(filePath)) {
            continue;
        }
        const QString token = _tokenForFile(filePath);
        if (token.isEmpty()) {
            orphaned++;
            continue;
        }
        pending.append({filePath, token});
    }

    if (pending.isEmpty()) {
        _setLastError(QString());
        _setHoldReason(orphaned > 0
            ? tr("%n log(s) belong to a pilot who is no longer saved. Use Upload Selected to send them under the current pilot.", "", orphaned)
            : tr("Everything is already uploaded."));
        return;
    }

    _setHoldReason(QString());
    _enqueue(pending);
}

bool AirDataSyncManager::isUploaded(const QString &filePath) const
{
    return _ledger.isUploaded(filePath);
}

QString AirDataSyncManager::fileStatusText(const QString &filePath) const
{
    if (_ledger.isUploaded(filePath)) {
        return tr("Uploaded");
    }

    const AirDataSyncLedger::Entry entry = _ledger.entry(filePath);
    if (_tokenForFile(filePath).isEmpty() && !entry.pilotId.isEmpty()) {
        return tr("Pilot not saved");
    }
    if (entry.attempts > 0) {
        return (entry.attempts >= kMaxAttempts) ? tr("Failed") : tr("Retrying (%1)").arg(entry.attempts);
    }

    return QString();
}

QString AirDataSyncManager::_builtInAppKey()
{
    // Supplied at configure time via -DQGC_AIRDATA_APP_KEY so the key never
    // lands in the repo. Undefined for builds configured without one.
#ifdef QGC_AIRDATA_APP_KEY
    return QStringLiteral(QGC_AIRDATA_APP_KEY);
#else
    return QString();
#endif
}

QString AirDataSyncManager::_appKey() const
{
    // A key entered in settings wins, so a build can be pointed at a different
    // app key for testing without recompiling.
    AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
    if (settings) {
        const QString keyOverride = settings->airDataAppKey()->rawValue().toString().trimmed();
        if (!keyOverride.isEmpty()) {
            return keyOverride;
        }
    }

    return _builtInAppKey();
}

QString AirDataSyncManager::_userToken() const
{
    AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
    if (!settings) {
        return QString();
    }
    return settings->airDataUploadToken()->rawValue().toString().trimmed();
}

void AirDataSyncManager::_ensureLedgerLoaded()
{
    if (_ledgerLoaded) {
        return;
    }

    const QString telemetryDir = SettingsManager::instance()->appSettings()->telemetrySavePath();
    if (!telemetryDir.isEmpty()) {
        _ledger.load(telemetryDir);
        _ledgerLoaded = true;
    }
}

QString AirDataSyncManager::_pilotId(const QString &token)
{
    // 64 bits is plenty to tell a handful of pilots apart, and short enough to read in the ledger.
    return QString::fromLatin1(QCryptographicHash::hash(token.toUtf8(), QCryptographicHash::Sha256).toHex().left(16));
}

QString AirDataSyncManager::_tokenForPilotId(const QString &pilotId) const
{
    for (const SavedToken &saved : _savedTokens) {
        if (_pilotId(saved.token) == pilotId) {
            return saved.token;
        }
    }

    const QString active = _userToken();
    if (!active.isEmpty() && (_pilotId(active) == pilotId)) {
        return active;
    }

    return QString();
}

QString AirDataSyncManager::_tokenForFile(const QString &filePath) const
{
    const QString pilotId = _ledger.entry(filePath).pilotId;
    if (pilotId.isEmpty()) {
        return _userToken();
    }
    return _tokenForPilotId(pilotId);
}

void AirDataSyncManager::_loadSavedTokens()
{
    _savedTokens.clear();

    QSettings settings;
    settings.beginGroup(QString::fromLatin1(kSavedTokensGroup));
    const int count = settings.beginReadArray(QString::fromLatin1(kSavedTokensArray));
    for (int i = 0; i < count; i++) {
        settings.setArrayIndex(i);
        SavedToken saved;
        saved.token = settings.value(QStringLiteral("token")).toString().trimmed();
        saved.nickname = settings.value(QStringLiteral("nickname")).toString();
        if (!saved.token.isEmpty()) {
            _savedTokens.append(saved);
        }
    }
    settings.endArray();
    settings.endGroup();

    emit savedTokensChanged();
}

void AirDataSyncManager::_storeSavedTokens() const
{
    QSettings settings;
    settings.beginGroup(QString::fromLatin1(kSavedTokensGroup));
    settings.remove(QString());
    settings.beginWriteArray(QString::fromLatin1(kSavedTokensArray), _savedTokens.count());
    for (int i = 0; i < _savedTokens.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(QStringLiteral("token"), _savedTokens[i].token);
        settings.setValue(QStringLiteral("nickname"), _savedTokens[i].nickname);
    }
    settings.endArray();
    settings.endGroup();
}

void AirDataSyncManager::_rememberToken(const QString &token)
{
    if (token.isEmpty()) {
        return;
    }

    SavedToken saved{token, QString()};
    for (int i = 0; i < _savedTokens.count(); i++) {
        if (_savedTokens[i].token == token) {
            if (i == 0) {
                return;
            }
            saved = _savedTokens.takeAt(i);
            break;
        }
    }

    _savedTokens.prepend(saved);
    _storeSavedTokens();
    emit savedTokensChanged();
}

QVariantList AirDataSyncManager::savedTokens() const
{
    QVariantList list;
    for (const SavedToken &saved : _savedTokens) {
        // Never show a whole token in the picker: "HD...a1b2" is enough to tell
        // two unnamed entries apart.
        const QString masked = (saved.token.length() > 8)
            ? QStringLiteral("%1...%2").arg(saved.token.left(2), saved.token.right(4))
            : saved.token;

        QVariantMap entry;
        entry[QStringLiteral("token")] = saved.token;
        entry[QStringLiteral("nickname")] = saved.nickname;
        entry[QStringLiteral("label")] = saved.nickname.isEmpty()
            ? masked
            : QStringLiteral("%1 (%2)").arg(saved.nickname, masked);
        list.append(entry);
    }
    return list;
}

int AirDataSyncManager::currentSavedTokenIndex() const
{
    const QString token = _userToken();
    if (token.isEmpty()) {
        return -1;
    }
    for (int i = 0; i < _savedTokens.count(); i++) {
        if (_savedTokens[i].token == token) {
            return i;
        }
    }
    return -1;
}

void AirDataSyncManager::selectSavedToken(int index)
{
    if ((index < 0) || (index >= _savedTokens.count())) {
        return;
    }

    AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
    if (settings) {
        settings->airDataUploadToken()->setRawValue(_savedTokens[index].token);
    }
}

void AirDataSyncManager::removeSavedToken(int index)
{
    if ((index < 0) || (index >= _savedTokens.count())) {
        return;
    }

    const QString token = _savedTokens.takeAt(index).token;
    _storeSavedTokens();
    emit savedTokensChanged();

    // Left active, the token would be validated and saved straight back on the
    // next startup, so forgetting the current pilot signs them out as well.
    if (token == _userToken()) {
        AirDataSettings *const settings = SettingsManager::instance()->airDataSettings();
        if (settings) {
            settings->airDataUploadToken()->setRawValue(QString());
        }
    }
}

void AirDataSyncManager::setSavedTokenNickname(int index, const QString &nickname)
{
    if ((index < 0) || (index >= _savedTokens.count())) {
        return;
    }

    const QString trimmed = nickname.trimmed();
    if (_savedTokens[index].nickname == trimmed) {
        return;
    }

    _savedTokens[index].nickname = trimmed;
    _storeSavedTokens();
    emit savedTokensChanged();
}

QString AirDataSyncManager::_appName()
{
    // AirData asks for an app identifier along the lines of "my_app_android".
#if defined(Q_OS_ANDROID)
    return QStringLiteral("arcskycontrol_android");
#elif defined(Q_OS_IOS)
    return QStringLiteral("arcskycontrol_ios");
#elif defined(Q_OS_WIN)
    return QStringLiteral("arcskycontrol_windows");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("arcskycontrol_macos");
#else
    return QStringLiteral("arcskycontrol_linux");
#endif
}

QString AirDataSyncManager::_appVersion()
{
    const QString version = QCoreApplication::applicationVersion();
    return version.isEmpty() ? QStringLiteral("0.0.0") : version;
}

QString AirDataSyncManager::tokenStatusText() const
{
    switch (_tokenState) {
    case TokenChecking:
        return tr("Checking token...");
    case TokenValid:
        return tr("Token accepted by AirData.");
    case TokenInvalid:
        return _tokenDetail.isEmpty() ? tr("AirData rejected this token.") : _tokenDetail;
    case TokenError:
        return _tokenDetail.isEmpty() ? tr("Could not reach AirData.") : _tokenDetail;
    case TokenUnknown:
    default:
        return QString();
    }
}

bool AirDataSyncManager::_parseJsonReply(QNetworkReply *reply, QJsonObject &object, QString &error)
{
    const QByteArray body = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        // AirData returns a JSON body on some failures; prefer its wording to Qt's.
        const QJsonObject errorObject = QJsonDocument::fromJson(body).object();
        const QString reported = airDataErrorMessage(errorObject);
        error = reported.isEmpty() ? reply->errorString() : reported;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if ((parseError.error != QJsonParseError::NoError) || !document.isObject()) {
        error = tr("Unexpected response from AirData: %1").arg(QString::fromUtf8(body.left(200)));
        return false;
    }

    object = document.object();
    return true;
}

void AirDataSyncManager::validateToken()
{
    const QString appKey = _appKey();
    if (appKey.isEmpty()) {
        _setTokenState(TokenError, tr("No AirData app key in this build. Rebuild with QGC_AIRDATA_APP_KEY set, or enter one below."));
        return;
    }

    const QString userToken = _userToken();
    if (userToken.isEmpty()) {
        _setTokenState(TokenUnknown);
        return;
    }

    _setTokenState(TokenChecking);

    QUrl url(QString::fromLatin1(kAuthUrl));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("appkey"), appKey);
    query.addQueryItem(QStringLiteral("usertoken"), userToken);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setTransferTimeout(kTransferTimeoutMs);

    QNetworkReply *const reply = _networkManager->get(request);
    (void) connect(reply, &QNetworkReply::finished, this, [this, reply, userToken]() {
        reply->deleteLater();

        // Switching pilots while a check is in flight starts a second check. The
        // first answer is about a token that is no longer active, and must not
        // set the status or land in the saved list under the wrong pilot.
        if (userToken != _userToken()) {
            return;
        }

        QJsonObject object;
        QString error;
        if (!_parseJsonReply(reply, object, error)) {
            qCWarning(AirDataSyncManagerLog) << "Token validation failed:" << error;
            _setTokenState(TokenError, error);
            return;
        }

        if (object.value(QStringLiteral("status")).toString() == QLatin1String("success")) {
            qCDebug(AirDataSyncManagerLog) << "Token validated";
            _rememberToken(userToken);
            _setTokenState(TokenValid);
        } else {
            const QString reported = airDataErrorMessage(object);
            qCWarning(AirDataSyncManagerLog) << "Token rejected:" << reported;
            _setTokenState(TokenInvalid, reported);
        }
    });
}

void AirDataSyncManager::uploadFiles(const QStringList &filePaths)
{
    if (_appKey().isEmpty()) {
        _setLastError(tr("No AirData app key configured."));
        return;
    }

    _ensureLedgerLoaded();

    QList<PendingUpload> uploads;
    bool missingToken = false;
    for (const QString &filePath : filePaths) {
        QString token = _tokenForFile(filePath);
        if (token.isEmpty()) {
            token = _userToken();
        }
        if (token.isEmpty()) {
            missingToken = true;
            continue;
        }
        uploads.append({filePath, token});
    }

    if (missingToken) {
        _setLastError(tr("No AirData Auto Upload Token entered."));
    }

    _enqueue(uploads);
}

void AirDataSyncManager::_enqueue(const QList<PendingUpload> &uploads)
{
    if (uploads.isEmpty()) {
        return;
    }

    for (const PendingUpload &upload : uploads) {
        // Picking a file by hand that auto sync already queued makes it the user's,
        // so turning auto upload off no longer withdraws it.
        if (upload.filePath == _currentFilePath) {
            _currentAutomatic = _currentAutomatic && upload.automatic;
            continue;
        }
        const auto queued = std::find_if(_queue.begin(), _queue.end(), [&upload](const PendingUpload &entry) {
            return entry.filePath == upload.filePath;
        });
        if (queued == _queue.end()) {
            _queue.enqueue(upload);
        } else {
            queued->automatic = queued->automatic && upload.automatic;
        }
    }

    emit countsChanged();

    if (_pausedForFlight) {
        _setHoldReason(tr("Paused while the vehicle is armed."));
    }
    _startQueueIfIdle();
}

void AirDataSyncManager::_dropAutomaticUploads()
{
    const qsizetype before = _queue.count();
    _queue.removeIf([](const PendingUpload &upload) { return upload.automatic; });
    if (_queue.count() != before) {
        qCDebug(AirDataSyncManagerLog) << "Auto upload turned off, withdrew" << (before - _queue.count()) << "queued logs";
        emit countsChanged();
    }

    if (_uploading && _currentAutomatic && _currentReply) {
        _droppingCurrent = true;
        _currentReply->abort();
    }
}

void AirDataSyncManager::_startQueueIfIdle()
{
    if (_uploading || _pausedForFlight || _queue.isEmpty()) {
        return;
    }

    _cancelled = false;
    _startNextUpload();
}

void AirDataSyncManager::cancel()
{
    _cancelled = true;
    _queue.clear();
    emit countsChanged();

    if (_currentReply) {
        _currentReply->abort();
    }
}

void AirDataSyncManager::resetCounts()
{
    _succeededCount = 0;
    _failedCount = 0;
    emit countsChanged();
}

void AirDataSyncManager::_startNextUpload()
{
    if (_queue.isEmpty()) {
        _setUploading(false);
        _currentFilePath.clear();
        _currentFileName.clear();
        emit currentFileNameChanged();
        emit syncFinished(_succeededCount, _failedCount);
        return;
    }

    const PendingUpload next = _queue.dequeue();
    _currentFilePath = next.filePath;
    _currentToken = next.token;
    _currentAutomatic = next.automatic;
    _currentFileName = QFileInfo(_currentFilePath).fileName();
    emit currentFileNameChanged();
    emit countsChanged();

    _setProgress(0);
    _setUploading(true);
    emit fileUploadStarted(_currentFilePath);

    _requestUploadDestination(_currentFilePath);
}

void AirDataSyncManager::_requestUploadDestination(const QString &filePath)
{
    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        _finishCurrentFile(false, QString(), tr("File no longer exists."));
        return;
    }

    // AirData wants the bare filename here, with no folder component.
    QUrlQuery form;
    form.addQueryItem(QStringLiteral("appkey"), _appKey());
    form.addQueryItem(QStringLiteral("usertoken"), _currentToken);
    form.addQueryItem(QStringLiteral("appname"), _appName());
    form.addQueryItem(QStringLiteral("appversion"), _appVersion());
    form.addQueryItem(QStringLiteral("filename"), fileInfo.fileName());

    QNetworkRequest request{QUrl(QString::fromLatin1(kDestinationUrl))};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    request.setTransferTimeout(kTransferTimeoutMs);

    qCDebug(AirDataSyncManagerLog) << "Requesting upload destination for" << fileInfo.fileName();

    _currentReply = _networkManager->post(request, form.toString(QUrl::FullyEncoded).toUtf8());
    (void) connect(_currentReply, &QNetworkReply::finished, this, [this, filePath]() {
        QNetworkReply *const reply = _currentReply;
        _currentReply = nullptr;
        reply->deleteLater();

        if (_cancelled) {
            _finishCurrentFile(false, QString(), tr("Cancelled."));
            return;
        }

        // The vehicle armed, or auto upload was turned off, after this reply had
        // already finished, so the abort had nothing to stop. Don't go on to start
        // the transfer itself.
        if (_pausedForFlight || _droppingCurrent) {
            _finishCurrentFile(false, QString(), tr("Stopped before upload."));
            return;
        }

        QJsonObject object;
        QString error;
        if (!_parseJsonReply(reply, object, error)) {
            _finishCurrentFile(false, QString(), error);
            return;
        }

        const QString uploadUrl = object.value(QStringLiteral("upload_url")).toString();
        if (uploadUrl.isEmpty()) {
            _finishCurrentFile(false, QString(), tr("AirData did not return an upload destination."));
            return;
        }

        qCDebug(AirDataSyncManagerLog) << "Upload destination" << uploadUrl;
        _uploadToDestination(filePath, QUrl(uploadUrl));
    });
}

void AirDataSyncManager::_uploadToDestination(const QString &filePath, const QUrl &destination)
{
    const QFileInfo fileInfo(filePath);

    QFile *const file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        const QString error = file->errorString();
        delete file;
        _finishCurrentFile(false, QString(), tr("Could not open log file: %1").arg(error));
        return;
    }

    QHttpMultiPart *const multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(createFormPart(QStringLiteral("appkey"), _appKey()));
    multiPart->append(createFormPart(QStringLiteral("usertoken"), _currentToken));
    multiPart->append(createFormPart(QStringLiteral("appname"), _appName()));
    multiPart->append(createFormPart(QStringLiteral("appversion"), _appVersion()));

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QStringLiteral("form-data; name=\"file\"; filename=\"%1\"").arg(fileInfo.fileName()));
    filePart.setBodyDevice(file);
    multiPart->append(filePart);
    file->setParent(multiPart);

    QNetworkRequest request(destination);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    request.setTransferTimeout(kTransferTimeoutMs);

    qCDebug(AirDataSyncManagerLog) << "Uploading" << fileInfo.fileName() << fileInfo.size() << "bytes";

    _currentReply = _networkManager->post(request, multiPart);
    multiPart->setParent(_currentReply);

    (void) connect(_currentReply, &QNetworkReply::uploadProgress, this, [this, filePath](qint64 sent, qint64 total) {
        if (total > 0) {
            const qreal fileProgress = static_cast<qreal>(sent) / static_cast<qreal>(total);
            _setProgress(fileProgress);
            emit fileUploadProgress(filePath, fileProgress);
        }
    });

    (void) connect(_currentReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *const reply = _currentReply;
        _currentReply = nullptr;
        reply->deleteLater();

        QJsonObject object;
        QString error;
        if (!_parseJsonReply(reply, object, error)) {
            _finishCurrentFile(false, QString(), error);
            return;
        }

        if (object.value(QStringLiteral("status")).toString() != QLatin1String("success")) {
            const QString reported = airDataErrorMessage(object);
            _finishCurrentFile(false, QString(), reported.isEmpty() ? tr("AirData rejected the upload.") : reported);
            return;
        }

        // The telemetry server does not issue a pollable job for .tlog uploads: it
        // answers with the literal string "Not available for this file type" in the
        // job_id field. Only keep a value that actually looks like a job handle, so
        // the ledger does not record a sentence as an id, and so nothing downstream
        // is tempted to poll it. Real ids look like "job_GLgbIEkeGLYwBLeb".
        const QString reportedJobId = object.value(QStringLiteral("job_id")).toString();
        const QString jobId = reportedJobId.startsWith(QLatin1String("job_")) ? reportedJobId : QString();
        if (jobId.isEmpty() && !reportedJobId.isEmpty()) {
            qCDebug(AirDataSyncManagerLog) << "No pollable job id for" << _currentFileName << ":" << reportedJobId;
        }

        _finishCurrentFile(true, jobId, QString());
    });
}

void AirDataSyncManager::_finishCurrentFile(bool success, const QString &jobId, const QString &error)
{
    const QString filePath = _currentFilePath;

    // Aborted because a vehicle armed. That is not the file's fault, so it goes
    // back to the head of the queue without being counted against its attempts.
    // A transfer that managed to finish anyway falls through and is recorded.
    const bool pausedMidTransfer = !success && _pausedForFlight && !_cancelled;
    // Withdrawn because auto upload was turned off: likewise not a failure, but
    // not requeued either. Arming at the same moment does not bring it back.
    const bool droppedMidTransfer = !success && _droppingCurrent && !_cancelled;
    _droppingCurrent = false;
    if (droppedMidTransfer) {
        qCDebug(AirDataSyncManagerLog) << "Withdrew" << _currentFileName << "since auto upload was turned off";
    } else if (pausedMidTransfer) {
        qCDebug(AirDataSyncManagerLog) << "Paused" << _currentFileName << "for flight; will resume after disarm";
        _queue.prepend({filePath, _currentToken, _currentAutomatic});
        emit fileUploadFinished(filePath, false, QString(), tr("Paused while the vehicle is armed."));
    } else if (success) {
        _succeededCount++;
        _setProgress(1);
        _ledger.recordSuccess(filePath, jobId);
        qCDebug(AirDataSyncManagerLog) << "Uploaded" << _currentFileName << "job" << jobId;
    } else {
        _failedCount++;
        _setLastError(tr("%1: %2").arg(_currentFileName, error));
        // Recorded even when cancelled, so the attempt count reflects reality. The
        // user pressing Sync Now clears these counters again.
        _ledger.recordFailure(filePath, error);
        qCWarning(AirDataSyncManagerLog) << "Upload failed for" << _currentFileName << error;
    }

    emit countsChanged();
    if (droppedMidTransfer || !pausedMidTransfer) {
        // A withdrawn file has nothing in the ledger, so the list shows it as not uploaded.
        emit fileUploadFinished(filePath, success, jobId, error);
    }

    if (_pausedForFlight && !_cancelled) {
        // Keep the queue; _onArmStateMayHaveChanged restarts it after disarm.
        _setUploading(false);
        _currentFilePath.clear();
        _currentFileName.clear();
        emit currentFileNameChanged();
        return;
    }

    if (_cancelled) {
        _queue.clear();
        _setUploading(false);
        _currentFilePath.clear();
        _currentFileName.clear();
        emit currentFileNameChanged();
        emit countsChanged();
        emit syncFinished(_succeededCount, _failedCount);
        return;
    }

    _startNextUpload();
}

void AirDataSyncManager::_setTokenState(TokenState state, const QString &detail)
{
    if ((_tokenState != state) || (_tokenDetail != detail)) {
        _tokenState = state;
        _tokenDetail = detail;
        emit tokenStateChanged();
    }
}

void AirDataSyncManager::_setUploading(bool uploading)
{
    if (_uploading != uploading) {
        _uploading = uploading;
        emit uploadingChanged();
    }
}

void AirDataSyncManager::_setProgress(qreal progress)
{
    if (!qFuzzyCompare(_progress, progress)) {
        _progress = progress;
        emit progressChanged();
    }
}

void AirDataSyncManager::_setHoldReason(const QString &reason)
{
    if (_holdReason != reason) {
        _holdReason = reason;
        emit holdReasonChanged();
    }
}

void AirDataSyncManager::_setLastError(const QString &error)
{
    if (_lastError != error) {
        _lastError = error;
        emit lastErrorChanged();
    }
}
