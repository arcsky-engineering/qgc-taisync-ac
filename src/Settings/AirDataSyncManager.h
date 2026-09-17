/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QStringList>

Q_DECLARE_LOGGING_CATEGORY(AirDataSyncManagerLog)

class QJsonObject;
class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

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
/// Files are uploaded strictly one at a time. A "success" response only means
/// AirData accepted the file into its processing queue -- confirming the flight
/// actually parsed requires polling the returned job id, which is not done here.
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

    bool    appKeyBuiltIn()     const { return !_builtInAppKey().isEmpty(); }
    int     tokenState()       const { return _tokenState; }
    QString tokenStatusText()  const;
    bool    uploading()        const { return _uploading; }
    QString currentFileName()  const { return _currentFileName; }
    qreal   progress()         const { return _progress; }
    int     queuedCount()      const { return _queue.count(); }
    int     succeededCount()   const { return _succeededCount; }
    int     failedCount()      const { return _failedCount; }
    QString lastError()        const { return _lastError; }

    /// Checks the configured Auto Upload Token against AirData.
    Q_INVOKABLE void validateToken();

    /// Queues the given .tlog paths for upload and starts the queue if idle.
    Q_INVOKABLE void uploadFiles(const QStringList &filePaths);

    /// Drops anything still queued and aborts the transfer in progress.
    Q_INVOKABLE void cancel();

    /// Clears the succeeded/failed tallies shown in the ui.
    Q_INVOKABLE void resetCounts();

signals:
    void tokenStateChanged();
    void uploadingChanged();
    void currentFileNameChanged();
    void progressChanged();
    void countsChanged();
    void lastErrorChanged();

    /// Emitted as each file leaves the queue. Phase 2 hangs the sync ledger off this.
    void fileUploadStarted(const QString &filePath);
    void fileUploadProgress(const QString &filePath, qreal progress);
    void fileUploadFinished(const QString &filePath, bool success, const QString &jobId, const QString &error);

    /// Emitted once the queue drains.
    void syncFinished(int succeeded, int failed);

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

    void _setTokenState(TokenState state, const QString &detail = QString());
    void _setUploading(bool uploading);
    void _setProgress(qreal progress);
    void _setLastError(const QString &error);

    /// Pulls the response body out of a reply and parses it as a JSON object.
    /// Returns false and fills @a error when the request failed or the body is not JSON.
    static bool _parseJsonReply(QNetworkReply *reply, QJsonObject &object, QString &error);

    QNetworkAccessManager *_networkManager = nullptr;
    QNetworkReply *_currentReply = nullptr;

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

    static constexpr const char *kDestinationUrl = "https://api.airdata.com/get_upload_destination";
    static constexpr const char *kAuthUrl        = "https://api.airdata.com/flight_upload_auth";

    /// AirData accepts files well over 100 MB; don't let Qt time out a slow link mid-transfer.
    static constexpr int kTransferTimeoutMs = 120 * 1000;
};
