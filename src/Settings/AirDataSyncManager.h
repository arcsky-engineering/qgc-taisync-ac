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

Q_DECLARE_LOGGING_CATEGORY(AirDataSyncManagerLog)

class QJsonObject;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QUrl;

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
    };

    /// Reads the ledger sitting in @a telemetryDir. A missing or unreadable
    /// file simply yields an empty ledger.
    void load(const QString &telemetryDir);
    void save() const;

    bool  isUploaded(const QString &filePath) const;
    Entry entry(const QString &filePath) const;

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

    /// Checks the configured Auto Upload Token against AirData.
    Q_INVOKABLE void validateToken();

    /// Queues the given .tlog paths for upload and starts the queue if idle.
    /// Bypasses the Wi-Fi-only and armed checks -- the user asked for this explicitly.
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

    QQueue<QString> _queue;
    QString _currentFilePath;
    QString _currentFileName;
    qreal   _progress = 0;
    bool    _uploading = false;
    bool    _cancelled = false;
    int _succeededCount = 0;
    int _failedCount = 0;

    TokenState _tokenState = TokenUnknown;
    QString    _tokenDetail;
    QString    _lastError;
    QString    _holdReason;

    static constexpr const char *kDestinationUrl = "https://api.airdata.com/get_upload_destination";
    static constexpr const char *kAuthUrl        = "https://api.airdata.com/flight_upload_auth";
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
