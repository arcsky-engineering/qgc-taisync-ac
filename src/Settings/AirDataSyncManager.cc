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
#include "QGCLoggingCategory.h"
#include "SettingsManager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/qapplicationstatic.h>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

QGC_LOGGING_CATEGORY(AirDataSyncManagerLog, "qgc.settings.airdatasyncmanager")

Q_APPLICATION_STATIC(AirDataSyncManager, _airDataSyncManagerInstance);

namespace {

/// Builds one text/plain part of the multipart upload body.
QHttpPart createFormPart(const QString &name, const QString &value)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"%1\"").arg(name));
    part.setBody(value.toUtf8());
    return part;
}

} // namespace

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
        const QString reported = errorObject.value(QStringLiteral("error")).toString();
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
    (void) connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        QJsonObject object;
        QString error;
        if (!_parseJsonReply(reply, object, error)) {
            qCWarning(AirDataSyncManagerLog) << "Token validation failed:" << error;
            _setTokenState(TokenError, error);
            return;
        }

        if (object.value(QStringLiteral("status")).toString() == QLatin1String("success")) {
            qCDebug(AirDataSyncManagerLog) << "Token validated";
            _setTokenState(TokenValid);
        } else {
            const QString reported = object.value(QStringLiteral("error")).toString();
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

    if (_userToken().isEmpty()) {
        _setLastError(tr("No AirData Auto Upload Token entered."));
        return;
    }

    for (const QString &filePath : filePaths) {
        if (!_queue.contains(filePath) && (filePath != _currentFilePath)) {
            _queue.enqueue(filePath);
        }
    }

    emit countsChanged();

    if (!_uploading) {
        _cancelled = false;
        _startNextUpload();
    }
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

    _currentFilePath = _queue.dequeue();
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
    form.addQueryItem(QStringLiteral("usertoken"), _userToken());
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
    multiPart->append(createFormPart(QStringLiteral("usertoken"), _userToken()));
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
            const QString reported = object.value(QStringLiteral("error")).toString();
            _finishCurrentFile(false, QString(), reported.isEmpty() ? tr("AirData rejected the upload.") : reported);
            return;
        }

        // A job id means AirData queued the file for processing. It has not
        // parsed the flight yet -- that needs a /flight_job poll, which is what
        // any delete-after-upload has to wait on.
        const QString jobId = object.value(QStringLiteral("job_id")).toString();
        _finishCurrentFile(true, jobId, QString());
    });
}

void AirDataSyncManager::_finishCurrentFile(bool success, const QString &jobId, const QString &error)
{
    const QString filePath = _currentFilePath;

    if (success) {
        _succeededCount++;
        _setProgress(1);
        qCDebug(AirDataSyncManagerLog) << "Uploaded" << _currentFileName << "job" << jobId;
    } else {
        _failedCount++;
        _setLastError(tr("%1: %2").arg(_currentFileName, error));
        qCWarning(AirDataSyncManagerLog) << "Upload failed for" << _currentFileName << error;
    }

    emit countsChanged();
    emit fileUploadFinished(filePath, success, jobId, error);

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

void AirDataSyncManager::_setLastError(const QString &error)
{
    if (_lastError != error) {
        _lastError = error;
        emit lastErrorChanged();
    }
}
