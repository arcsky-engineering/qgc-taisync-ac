#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QElapsedTimer>

#include "rtcm.h"
#include "RTCMMavlink.h"
#include "NmeaMessage.h"

Q_DECLARE_LOGGING_CATEGORY(NTRIPLog)

class NTRIPSettings;

class NTRIPTCPLink : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(NTRIPStatus connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

   public:
    enum class NTRIPStatus {
        Off,
        Connecting,
        Connected,
        Retrying,
        TimedOut
    };
    Q_ENUM(NTRIPStatus)

    explicit NTRIPTCPLink(const QString& hostAddress,
                          int port,
                          const QString& username,
                          const QString& password,
                          const QString& mountpoint,
                          const QString& whitelist,
                          const bool& enableVRS,
                          const bool& forceNtripV1,
                          QObject* parent = nullptr);
    ~NTRIPTCPLink();

    bool enabled() const { return _enabled; }
    void setEnabled(bool en);
    NTRIPStatus connectionStatus() const { return _connectionStatus; }

   signals:
    void error(const QString& errorMsg);
    void RTCMDataUpdate(QByteArray message);
    void connectionStatusChanged();
    void enabledChanged();

   private slots:
    void _socketConnected();
    void _socketDisconnected();
    void _socketError(QAbstractSocket::SocketError err);
    void _readBytes();
    void _retryConnection();
    void _sendNmeaGga();

   private:
    void _initSocket();
    void _teardownSocket();
    void _resetStreamState();
    void _setConnectionStatus(NTRIPStatus newStatus);
    void _processHandshake();
    void _handshakeComplete(const QByteArray& leftover);
    void _handshakeFailed(const QString& statusLine);
    void _processData(const QByteArray& data);
    bool _dechunk(const QByteArray& data, QByteArray& payload);
    void _parseRtcm(const QByteArray& payload);
    void _handleDisconnect(const QString& reason);

    QString _hostAddress;
    int _port;
    QString _username;
    QString _password;
    QString _mountpoint;
    QSet<int> _whitelist;
    bool _isVRSEnable = false;
    bool _forceNtripV1 = false;
    int _vrsSendRateMSecs = 3000;

    int _rapidRetryCount = 0;
    const int _maxRapidRetries = 10;                // Max consecutive rapid failures before timeout
    const int _minRetryDelayMS = 2000;              // Retry delay doubles after each failure...
    const int _maxRetryDelayMS = 30000;             // ...up to this limit
    const int _stableConnectionThresholdMS = 5000;  // Connection must be stable this long to not count as a rapid failure
    const int _backoffResetThresholdMS = 30000;     // Connection must be stable this long to reset the retry delay
    int _retryDelayMS = _minRetryDelayMS;
    qint64 _connectedTimestamp = 0;                 // 0 = never reached Connected this attempt

    QTcpSocket* _socket = nullptr;
    RTCMParsing* _rtcm_parsing = nullptr;
    QTimer* _reconnectTimer = nullptr;
    QTimer* _vrsSendTimer = nullptr;

    bool _enabled = false;
    NTRIPStatus _connectionStatus = NTRIPStatus::Off;

    // Handshake: caster response buffered here until the header is complete
    QByteArray _headerBuffer;
    const int _maxHeaderBytes = 8192;
    QTimer* _handshakeTimer = nullptr;
    const int _handshakeTimeoutMS = 10000;          // TCP connect + caster response

    // Chunked transfer decoding (HTTP/1.1 casters that send Transfer-Encoding: chunked)
    bool _chunked = false;
    QByteArray _chunkBuffer;
    int _chunkRemaining = -1;                       // -1 = expecting size line, 0 = expecting CRLF, >0 = data bytes left

    // Data heartbeat, restarted on every received byte once Connected
    QTimer* _heartbeatTimer = nullptr;
    const int _heartbeatTimeoutMS = 10000;

    // Receive statistics, logged every _rxStatsIntervalMS while Connected
    QElapsedTimer _rxStatsTimer;
    const int _rxStatsIntervalMS = 10000;
    qint64 _rxStatsBytes = 0;
    QMap<int, int> _rxStatsMsgCounts;
    qint64 _rxTotalBytes = 0;
};

//---------------------------------------------------------------

class NTRIP : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool masterEnable READ masterEnable NOTIFY masterEnableChanged)

   public:
    explicit NTRIP(NTRIPSettings* settings, QObject* parent = nullptr);
    ~NTRIP();

    static NTRIP* instance();

    bool enabled() const;
    void setEnabled(bool en);
    int connectionStatus() const;
    bool masterEnable() const;

   signals:
    void enabledChanged();
    void connectionStatusChanged();
    void masterEnableChanged();

   private slots:
    void _tcpError(const QString& errorMsg);

   private:
    void _initLink();
    void _stopLink();

    NTRIPSettings* _settings = nullptr;
    NTRIPTCPLink* _tcpLink = nullptr;
    RTCMMavlink* _rtcmMavlink = nullptr;
};
