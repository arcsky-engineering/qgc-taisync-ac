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
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/qnumeric.h>
#include <QtQmlIntegration/QtQmlIntegration>

class QNetworkAccessManager;

Q_DECLARE_LOGGING_CATEGORY(EncoderTempPollerLog)

/// Polls a Z3/ILX video encoder's CGI stats endpoint (plain HTTP on the LAN) for
/// CPU/FPGA/LENS temperatures and exposes them to QML.
///
/// This lives in C++ — rather than a QML XMLHttpRequest — specifically because the
/// app enables system-proxy configuration globally (QNetworkProxyFactory::setUse-
/// SystemConfiguration(true) in QGCApplication). QML's XMLHttpRequest gives no way
/// to bypass that proxy, so on a machine with a proxy or WPAD auto-detect enabled
/// (common on isolated payload networks with no route to a WPAD/proxy server) every
/// request failed at the transport layer with XHR status 0. The encoder is always a
/// direct device on the LAN, so this class forces QNetworkProxy::NoProxy — matching
/// how the raw LAN links (UDPLink, TaisyncInfo) already opt out of the system proxy.
class EncoderTempPoller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString url          READ url            WRITE setUrl            NOTIFY urlChanged)
    Q_PROPERTY(bool    active       READ active         WRITE setActive         NOTIFY activeChanged)
    Q_PROPERTY(int     intervalMsec READ intervalMsec   WRITE setIntervalMsec   NOTIFY intervalMsecChanged)
    Q_PROPERTY(qreal   cpuTemp      READ cpuTemp        NOTIFY dataChanged)
    Q_PROPERTY(qreal   fpgaTemp     READ fpgaTemp       NOTIFY dataChanged)
    Q_PROPERTY(qreal   lensTemp     READ lensTemp       NOTIFY dataChanged)
    Q_PROPERTY(bool    valid        READ valid          NOTIFY dataChanged)
    Q_PROPERTY(QString lastError    READ lastError      NOTIFY dataChanged)

public:
    explicit EncoderTempPoller(QObject *parent = nullptr);
    ~EncoderTempPoller() override;

    QString url() const { return _url; }
    void setUrl(const QString &url);

    bool active() const { return _active; }
    void setActive(bool active);

    int intervalMsec() const { return _timer.interval(); }
    void setIntervalMsec(int msec);

    qreal   cpuTemp()   const { return _cpuTemp; }
    qreal   fpgaTemp()  const { return _fpgaTemp; }
    qreal   lensTemp()  const { return _lensTemp; }
    bool    valid()     const { return _valid; }
    QString lastError() const { return _lastError; }

signals:
    void urlChanged();
    void activeChanged();
    void intervalMsecChanged();
    void dataChanged();

private slots:
    void _poll();

private:
    void _fetchStats();
    void _fail(const QString &why);
    void _publish(qreal cpu, qreal fpga, qreal lens);

    QNetworkAccessManager   *_nam       = nullptr;
    QTimer                  _timer;
    QString                 _url;
    bool                    _active     = false;
    bool                    _polling    = false;    ///< guards against overlapping requests if one hangs
    qreal                   _cpuTemp    = qQNaN();
    qreal                   _fpgaTemp   = qQNaN();
    qreal                   _lensTemp   = qQNaN();
    bool                    _valid      = false;
    QString                 _lastError;

    static constexpr int    kRequestTimeoutMs = 4000;   ///< per-request transfer timeout, < poll interval
};
