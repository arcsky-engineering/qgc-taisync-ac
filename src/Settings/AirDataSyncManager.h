/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QVariant>

Q_DECLARE_LOGGING_CATEGORY(AirDataSyncManagerLog)

class QJsonObject;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QUrl;
class Vehicle;

/// Remembers which telemetry logs have already gone to AirData.
///
/// This is a bandwidth optimisation, not a correctness mechanism. AirData
/// identifies uploads by content MD5 and runs a second duplicate check against
/// the account before a job reports Success, so re-sending a file it already
/// has reuses the existing flight rather than creating a second one. Losing
/// this ledger therefore costs one redundant upload pass and nothing else,
/// which is why it keys on name plus size and does not bother hashing.
class AirDataSyncLedger
{
public:
    struct Entry {
        quint64   size = 0;
        bool      uploaded = false;
        QString   jobId;
        QDateTime uploadedAt;
        int       attempts = 0;
        QString   lastError;
        /// Fingerprint of the token that was active when the log was saved, so the
        /// flight lands in that pilot's account however long it waits for a network.
        /// Empty for logs saved before pilots were tracked.
        QString   pilotId;
    };

    /// Reads the ledger sitting in @a telemetryDir. A missing or unreadable
    /// file simply yields an empty ledger.
    void load(const QString &telemetryDir);
    void save() const;

    bool  isUploaded(const QString &filePath) const;
    Entry entry(const QString &filePath) const;

    void recordPilot(const QString &filePath, const QString &pilotId);
    void recordSuccess(const QString &filePath, const QString &jobId);
    void recordFailure(const QString &filePath, const QString &error);

    /// Clears the attempt counters so previously exhausted files are retried.
    void clearFailures();

    /// Drops entries whose file is no longer on disk, so the ledger cannot grow forever.
    void pruneMissing(const QStringList &existingFilePaths);

private:
    static QString _key(const QString &filePath);

    QString _filePath;
    QHash<QString, Entry> _entries;
};

/// Uploads telemetry logs to AirData using their Flight Upload API.
///
/// Upload of a single file is a two step process, per AirData's docs:
///   1. POST /get_upload_destination to learn which server takes this file.
///      The destination can differ per file, so this is done for every file.
///   2. POST the file itself as multipart form data to that destination.
///
/// Both calls are authenticated with Arcsky's 8 character app key plus the
/// pilot's "HD..." Auto Upload Token. The app key is compiled in via the
/// QGC_AIRDATA_APP_KEY build variable and can be overridden from settings for
/// testing.
///
/// Files are uploaded strictly one at a time. AirData imposes no rate limit, but
/// a controller's uplink is the bottleneck and parallel transfers would only make
/// progress reporting and retry messier without finishing sooner.
///
/// A "success" response to the upload itself only means AirData accepted the file.
/// For most formats the response carries a job id that can be polled until it
/// reports Success, but the telemetry server does not issue one for .tlog uploads:
/// it returns the literal string "Not available for this file type" instead. So
/// there is currently no programmatic way to confirm a tlog actually parsed, and
/// nothing here should treat an accepted upload as proof the flight landed.
///
/// AirData have confirmed this is expected rather than a gap to work around: a tlog
/// goes through storage, conversion and import steps that nothing in their API
/// exposes, so HTTP 200 means the file reached their server and no more. They
/// explicitly advise against gating deletion of local copies on anything their API
/// returns for tlogs. Do not add an auto-delete that claims upload as its trigger.
class AirDataSyncManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool    appKeyBuiltIn    READ appKeyBuiltIn     CONSTANT)
    Q_PROPERTY(int     tokenState       READ tokenState        NOTIFY tokenStateChanged)
    Q_PROPERTY(QString tokenStatusText  READ tokenStatusText   NOTIFY tokenStateChanged)
    Q_PROPERTY(bool    uploading        READ uploading         NOTIFY uploadingChanged)
    Q_PROPERTY(QString currentFileName  READ currentFileName   NOTIFY currentFileNameChanged)
    Q_PROPERTY(qreal   progress         READ progress          NOTIFY progressChanged)
    Q_PROPERTY(int     queuedCount      READ queuedCount       NOTIFY countsChanged)
    Q_PROPERTY(int     succeededCount   READ succeededCount    NOTIFY countsChanged)
    Q_PROPERTY(int     failedCount      READ failedCount       NOTIFY countsChanged)
    Q_PROPERTY(QString lastError        READ lastError         NOTIFY lastErrorChanged)
    Q_PROPERTY(QString holdReason       READ holdReason        NOTIFY holdReasonChanged)
    Q_PROPERTY(QVariantList savedTokens READ savedTokens       NOTIFY savedTokensChanged)
    Q_PROPERTY(int     currentSavedTokenIndex READ currentSavedTokenIndex NOTIFY savedTokensChanged)

public:
    enum TokenState {
        TokenUnknown = 0,   ///< Not checked yet
        TokenChecking,      ///< Validation request in flight
        TokenValid,         ///< AirData accepted the token
        TokenInvalid,       ///< AirData rejected the token
        TokenError          ///< Could not reach AirData, or no app key in this build
    };
    Q_ENUM(TokenState)

    explicit AirDataSyncManager(QObject *parent = nullptr);
    ~AirDataSyncManager();

    static AirDataSyncManager *instance();

    /// Wires up the automatic triggers. Called once from QGCApplication after
    /// settings exist, since the constructor may run before SettingsManager::init().
    void init();

    bool    appKeyBuiltIn()    const { return !_builtInAppKey().isEmpty(); }
    int     tokenState()       const { return _tokenState; }
    QString tokenStatusText()  const;
    bool    uploading()        const { return _uploading; }
    QString currentFileName()  const { return _currentFileName; }
    qreal   progress()         const { return _progress; }
    int     queuedCount()      const { return _queue.count(); }
    int     succeededCount()   const { return _succeededCount; }
    int     failedCount()      const { return _failedCount; }
    QString lastError()        const { return _lastError; }

    /// Why an automatic sync is not running right now, empty when nothing is holding it.
    QString holdReason()       const { return _holdReason; }

    /// Tokens AirData has accepted on this controller, most recently used first,
    /// so pilots sharing a controller can switch without retyping. Each entry is a
    /// map of token, nickname and label (the nickname, or a masked token if none).
    QVariantList savedTokens() const;

    /// Index into savedTokens of the token currently configured, or -1.
    int currentSavedTokenIndex() const;

    /// Makes the saved token at @a index the active one. Validation and an
    /// automatic sync follow from the setting change, as if it had been typed.
    Q_INVOKABLE void selectSavedToken(int index);

    /// Drops the saved token at @a index, and clears the active token if it was that one.
    Q_INVOKABLE void removeSavedToken(int index);

    /// Sets an optional display name, e.g. the pilot's name, for the saved token at @a index.
    Q_INVOKABLE void setSavedTokenNickname(int index, const QString &nickname);

    /// Checks the configured Auto Upload Token against AirData.
    Q_INVOKABLE void validateToken();

    /// Queues the given .tlog paths for upload and starts the queue if idle.
    /// Bypasses the Wi-Fi-only check -- the user asked for this explicitly.
    /// Each log goes to the pilot who flew it; a log whose pilot has been forgotten
    /// goes to the active pilot, since picking it by hand is the user's say-so.
    /// Like everything else, it still waits while a vehicle is armed.
    Q_INVOKABLE void uploadFiles(const QStringList &filePaths);

    /// Scans the telemetry directory and queues everything the ledger has not
    /// recorded as uploaded. Resets exhausted attempt counters first, so a user
    /// who has just fixed their token gets a real retry.
    Q_INVOKABLE void syncNow();

    /// Drops anything still queued and aborts the transfer in progress.
    Q_INVOKABLE void cancel();

    /// Clears the succeeded/failed tallies shown in the ui.
    Q_INVOKABLE void resetCounts();

    /// True when the ledger records this file as already sent.
    Q_INVOKABLE bool isUploaded(const QString &filePath) const;

    /// Human readable per-file state for the log list: "Uploaded", "Failed (2)", or empty.
    Q_INVOKABLE QString fileStatusText(const QString &filePath) const;

signals:
    void tokenStateChanged();
    void uploadingChanged();
    void currentFileNameChanged();
    void progressChanged();
    void countsChanged();
    void lastErrorChanged();
    void holdReasonChanged();
    void savedTokensChanged();

    /// Emitted as each file moves through the queue, so the log list can track it.
    void fileUploadStarted(const QString &filePath);
    void fileUploadProgress(const QString &filePath, qreal progress);
    void fileUploadFinished(const QString &filePath, bool success, const QString &jobId, const QString &error);

    /// Emitted once the queue drains.
    void syncFinished(int succeeded, int failed);

private slots:
    void _onTelemetryLogSaved(const QString &filePath);
    void _onPeriodicCheck();
    void _onReachabilityChanged();
    void _onVehicleAdded(Vehicle *vehicle);
    void _onArmStateMayHaveChanged();

private:
    static QString _builtInAppKey();
    QString _appKey() const;
    QString _userToken() const;
    static QString _appName();
    static QString _appVersion();

    void _startNextUpload();
    void _requestUploadDestination(const QString &filePath);
    void _uploadToDestination(const QString &filePath, const QUrl &destination);
    void _finishCurrentFile(bool success, const QString &jobId, const QString &error);

    /// Queues everything not yet uploaded, subject to the automatic-sync conditions.
    void _considerAutoSync();

    /// Checks token, network and arm state. Returns false and sets holdReason when
    /// an automatic sync should not start right now.
    bool _autoSyncAllowed();

    /// Absolute paths of every .tlog in the telemetry save directory, newest first.
    static QStringList _telemetryLogPaths();

    struct SavedToken {
        QString token;
        QString nickname;
    };

    /// A queued file and the token it goes up under, fixed when it is queued so
    /// switching pilots mid-queue cannot redirect it.
    struct PendingUpload {
        QString filePath;
        QString token;
        /// Queued by auto sync rather than by the user, so turning auto upload off withdraws it.
        bool    automatic = false;
    };

    /// Withdraws everything auto sync queued, including the transfer in progress,
    /// leaving files the user picked by hand to carry on.
    void _dropAutomaticUploads();

    void _ensureLedgerLoaded();

    /// Any connected vehicle, not just the active one: a second vehicle in the air
    /// leans on the same uplink for RTK corrections.
    static bool _anyVehicleArmed();

    /// Starts the queue unless it is already running or paused for a flight.
    void _startQueueIfIdle();

    /// Short one-way fingerprint of a token. This is what the ledger stores, since
    /// the ledger sits in the telemetry folder, which may be on removable storage.
    static QString _pilotId(const QString &token);

    /// The saved or active token with this fingerprint, or empty if no longer known.
    QString _tokenForPilotId(const QString &pilotId) const;

    /// The token a log should be uploaded under: its pilot's if recorded, the
    /// active token for logs saved before pilots were tracked, and empty when its
    /// pilot has since been forgotten.
    QString _tokenForFile(const QString &filePath) const;

    void _enqueue(const QList<PendingUpload> &uploads);

    void _loadSavedTokens();
    void _storeSavedTokens() const;

    /// Adds or moves @a token to the front of the saved list. Called once AirData accepts it.
    void _rememberToken(const QString &token);

    void _setTokenState(TokenState state, const QString &detail = QString());
    void _setUploading(bool uploading);
    void _setProgress(qreal progress);
    void _setLastError(const QString &error);
    void _setHoldReason(const QString &reason);

    /// Pulls the response body out of a reply and parses it as a JSON object.
    /// Returns false and fills @a error when the request failed or the body is not JSON.
    static bool _parseJsonReply(QNetworkReply *reply, QJsonObject &object, QString &error);

    QNetworkAccessManager *_networkManager = nullptr;
    QNetworkReply *_currentReply = nullptr;
    QTimer *_periodicTimer = nullptr;

    AirDataSyncLedger _ledger;
    bool _ledgerLoaded = false;

    QQueue<PendingUpload> _queue;
    QString _currentFilePath;
    QString _currentToken;
    bool    _currentAutomatic = false;
    /// The transfer in progress was aborted because auto upload was turned off. It
    /// is neither a failure nor requeued; the next queued file follows.
    bool    _droppingCurrent = false;
    QString _currentFileName;
    qreal   _progress = 0;
    bool    _uploading = false;
    bool    _cancelled = false;

    /// Set while any vehicle is armed. A transfer in progress is aborted and put
    /// back at the head of the queue without counting as a failed attempt, and
    /// nothing starts again until every vehicle is disarmed. Uploads would
    /// otherwise compete with NTRIP corrections on the controller's uplink in flight.
    bool    _pausedForFlight = false;
    int _succeededCount = 0;
    int _failedCount = 0;

    TokenState _tokenState = TokenUnknown;
    QString    _tokenDetail;
    QString    _lastError;
    QString    _holdReason;

    QList<SavedToken> _savedTokens;

    static constexpr const char *kDestinationUrl = "https://api.airdata.com/get_upload_destination";
    static constexpr const char *kAuthUrl        = "https://api.airdata.com/flight_upload_auth";

    /// Saved tokens live in QSettings beside the active token rather than as a Fact,
    /// since a Fact holds one value and this is a list. Same plaintext storage as the token itself.
    static constexpr const char *kSavedTokensGroup = "AirDataSavedTokens";
    static constexpr const char *kSavedTokensArray = "tokens";

    /// AirData accepts files well over 100 MB; don't let Qt time out a slow link mid-transfer.
    static constexpr int kTransferTimeoutMs = 120 * 1000;

    /// Give the operator room to re-arm straight after landing before we start pushing bytes.
    static constexpr int kAfterFlightDelayMs = 30 * 1000;

    /// First sweep after boot, once links have settled.
    static constexpr int kStartupDelayMs = 30 * 1000;

    /// Catches files missed while offline, and retries earlier failures.
    static constexpr int kPeriodicIntervalMs = 10 * 60 * 1000;

    /// Automatic retries stop here. "Sync Now" clears the counters and tries again.
    static constexpr int kMaxAttempts = 5;
};
