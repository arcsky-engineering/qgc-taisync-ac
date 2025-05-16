/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QThread>
#include <QTcpSocket>
#include <QGeoCoordinate>
#include <QUrl>

#include "Drivers/src/rtcm.h"
#include "RTCM/RTCMMavlink.h"

Q_DECLARE_LOGGING_CATEGORY(NTRIPLog)

class NTRIPSettings;

class NTRIPTCPLink : public QThread
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(NTRIPStatus connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
public:
    NTRIPTCPLink(const QString& hostAddress,
                 int port,
                 const QString& username,
                 const QString& password,
                 const QString& mountpoint,
                 const QString& whitelist,
                 const bool&    enableVRS);
    ~NTRIPTCPLink();

//    enum class NTRIPStatus {
//        Disconnected,
//        Connecting,
//        Connected,
//        Retrying,
//        TimedOut
//    };
//    Q_ENUM(NTRIPStatus)

    enum class NTRIPStatus {
        Off,
        Connecting,
        Connected,
        Retrying,
        TimedOut
    };
    Q_ENUM(NTRIPStatus)

    bool enabled() const {return _enabled;}
    void setEnabled(bool en);

    NTRIPStatus connectionStatus() const { return _connectionStatus; }

signals:
    void error(const QString errorMsg);
    void RTCMDataUpdate(QByteArray message);
    void connectionStatusChanged();
    void enabledChanged();

protected:
    void run() final;

private slots:
    void _readBytes();

private:
    enum class NTRIPState {
        uninitialised,
        waiting_for_http_response,
        waiting_for_rtcm_header,
        accumulating_rtcm_packet,
    };

    void _hardwareConnect(void);
    void _parse(const QByteArray &buffer);

    void _setConnectionStatus(NTRIPStatus newStatus);

    void _startNTRIP();
    void _stopNTRIP();
    void _retryConnection();

    QTcpSocket*     _socket =   nullptr;

    QString         _hostAddress;
    int             _port;
    QString         _username;
    QString         _password;
    QString         _mountpoint;
    QSet<int>       _whitelist;
    bool            _isVRSEnable;
    int             _vrsSendRateMSecs = 3000;
    bool            _ntripForceV1 = false;

    QTimer* _reconnectTimer = nullptr;
    int _retryCount = 0;
    const int _maxRetries = 5; // max retry attempts

    bool            _enabled = false;


    NTRIPStatus _connectionStatus = NTRIPStatus::Off;
    // QUrl
    QUrl            _ntripURL;

    // Send NMEA
    void _sendNmeaGga();

    // VRS Timer
    QTimer*          _vrsSendTimer;
    // this is perfectly fine to send VRS data every 30 seconds

    RTCMParsing *_rtcm_parsing{nullptr};
    NTRIPState _state;

    QGCToolbox*  _toolbox = nullptr;
};

class NTRIP : public QGCTool {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool masterEnable READ masterEnable NOTIFY masterEnableChanged)
public:
    NTRIP(QGCApplication* app, QGCToolbox* toolbox);

    // QGCTool overrides
    void setToolbox(QGCToolbox* toolbox) final;

    void _initLink();

    bool enabled() const;

    void setEnabled(bool en);

    int connectionStatus() const; // casted enum

    bool masterEnable() const;

signals:
    void enabledChanged();
    void connectionStatusChanged();
    void masterEnableChanged();

public slots:
    void _tcpError          (const QString errorMsg);

private slots:

private:
    NTRIPTCPLink*                    _tcpLink = nullptr;
    RTCMMavlink*                     _rtcmMavlink = nullptr;
};
