/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "EncoderTempPoller.h"
#include "QGCLoggingCategory.h"

#include <QtCore/QByteArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

QGC_LOGGING_CATEGORY(EncoderTempPollerLog, "EncoderTempPollerLog")

EncoderTempPoller::EncoderTempPoller(QObject *parent)
    : QObject(parent)
    , _nam(new QNetworkAccessManager(this))
{
    // The encoder is always a direct device on the LAN. The app turns on system
    // proxy config globally, so force NoProxy here — this is exactly what QML
    // XMLHttpRequest could not do, and why on-device polling failed with status 0.
    _nam->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));

    _timer.setInterval(5000);
    connect(&_timer, &QTimer::timeout, this, &EncoderTempPoller::_poll);
}

EncoderTempPoller::~EncoderTempPoller() = default;

void EncoderTempPoller::setUrl(const QString &url)
{
    if (_url == url) {
        return;
    }
    _url = url;
    emit urlChanged();
}

void EncoderTempPoller::setActive(bool active)
{
    if (_active == active) {
        return;
    }
    _active = active;
    emit activeChanged();

    if (_active) {
        _timer.start();
        _poll();            // poll immediately, matching the old Timer's triggeredOnStart
    } else {
        _timer.stop();
    }
}

void EncoderTempPoller::setIntervalMsec(int msec)
{
    if (msec <= 0 || _timer.interval() == msec) {
        return;
    }
    _timer.setInterval(msec);
    emit intervalMsecChanged();
}

// Step 1: POST to refresh the board's temperature readings.
void EncoderTempPoller::_poll()
{
    if (!_active || _polling) {
        return;             // inactive, or previous poll still in flight — skip this tick
    }
    if (_url.isEmpty()) {
        _fail(QStringLiteral("no encoder URL configured"));
        return;
    }

    _polling = true;

    QNetworkRequest request{QUrl(_url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    request.setTransferTimeout(kRequestTimeoutMs);

    QNetworkReply *reply = _nam->post(request, QByteArrayLiteral("action=TempStatus"));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        const int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((reply->error() != QNetworkReply::NoError) || (httpCode != 200)) {
            _polling = false;
            _fail(QStringLiteral("POST TempStatus failed: %1 (HTTP %2)").arg(reply->errorString()).arg(httpCode));
            return;
        }
        _fetchStats();
    });
}

// Step 2: GET the refreshed stats JSON and parse out the CPU/FPGA/LENS temps.
void EncoderTempPoller::_fetchStats()
{
    QNetworkRequest request{QUrl(_url + QStringLiteral("?ctrl=stats&chn=null"))};
    request.setTransferTimeout(kRequestTimeoutMs);

    QNetworkReply *reply = _nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        _polling = false;

        const int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((reply->error() != QNetworkReply::NoError) || (httpCode != 200)) {
            _fail(QStringLiteral("GET stats failed: %1 (HTTP %2)").arg(reply->errorString()).arg(httpCode));
            return;
        }

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
        if ((parseError.error != QJsonParseError::NoError) || !doc.isObject()) {
            _fail(QStringLiteral("stats JSON parse failed: %1").arg(parseError.errorString()));
            return;
        }

        // Format varies by firmware. Seen: "+CPU 53.2 +OK" (CPU only) and
        // " LENS 29 CPU 78.700 FPGA 89.8 OK". CPU is required; FPGA/LENS are optional.
        const QString str = doc.object().value(QStringLiteral("temp_status_str")).toString();
        static const QRegularExpression reCpu (QStringLiteral("CPU\\s+([0-9.]+)"));
        static const QRegularExpression reFpga(QStringLiteral("FPGA\\s+([0-9.]+)"));
        static const QRegularExpression reLens(QStringLiteral("LENS\\s+([0-9.]+)"));
        const QRegularExpressionMatch mCpu  = reCpu.match(str);
        const QRegularExpressionMatch mFpga = reFpga.match(str);
        const QRegularExpressionMatch mLens = reLens.match(str);

        if (!mCpu.hasMatch()) {
            _fail(QStringLiteral("could not parse CPU temp from temp_status_str: \"%1\"").arg(str));
            return;
        }

        _publish(mCpu.captured(1).toDouble(),
                 mFpga.hasMatch() ? mFpga.captured(1).toDouble() : qQNaN(),
                 mLens.hasMatch() ? mLens.captured(1).toDouble() : qQNaN());
    });
}

void EncoderTempPoller::_fail(const QString &why)
{
    _valid     = false;
    _lastError = why;
    qCDebug(EncoderTempPollerLog) << "[ILX_ENC_TEMP]" << why << "url" << _url;
    emit dataChanged();
}

void EncoderTempPoller::_publish(qreal cpu, qreal fpga, qreal lens)
{
    _cpuTemp   = cpu;
    _fpgaTemp  = fpga;
    _lensTemp  = lens;
    _valid     = true;
    _lastError.clear();
    qCDebug(EncoderTempPollerLog) << "[ILX_ENC_TEMP] CPU" << cpu << "FPGA" << fpga << "LENS" << lens;
    emit dataChanged();
}
