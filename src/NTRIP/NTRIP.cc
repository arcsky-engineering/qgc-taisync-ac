#include "NTRIP.h"
#include "NTRIPSettings.h"
#include <QDateTime>
#include "SettingsManager.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"
#include "QGCApplication.h"
#include "Vehicle.h"

QGC_LOGGING_CATEGORY(NTRIPLog, "qgc.ntrip")

Q_APPLICATION_STATIC(NTRIP, _ntrip, SettingsManager::instance()->ntripSettings());

NTRIP* NTRIP::instance() { return _ntrip(); }

NTRIP::NTRIP(NTRIPSettings* settings, QObject* parent)
    : QObject(parent)
      , _settings(settings)
{
    connect(_settings->ntripServerConnectEnabled(), &Fact::rawValueChanged, this, [this](QVariant enabled) {
        if (!enabled.toBool() && _tcpLink) {
            _stopLink();
            emit enabledChanged();
        }
        emit masterEnableChanged();
    });
}


NTRIP::~NTRIP() {
    _stopLink();
}

void NTRIP::_initLink() {
    if (_tcpLink || _rtcmMavlink)
        return;

    _rtcmMavlink = new RTCMMavlink();

    _tcpLink = new NTRIPTCPLink(
        _settings->ntripServerHostAddress()->rawValue().toString(),
        _settings->ntripServerPort()->rawValue().toInt(),
        _settings->ntripUsername()->rawValue().toString(),
        _settings->ntripPassword()->rawValue().toString(),
        _settings->ntripMountpoint()->rawValue().toString(),
        _settings->ntripWhitelist()->rawValue().toString(),
        _settings->ntripEnableVRS()->rawValue().toBool(),
        _settings->ntripForceNtripV1()->rawValue().toBool(),
        this
        );

    connect(_tcpLink, &NTRIPTCPLink::error, this, &NTRIP::_tcpError, Qt::QueuedConnection);
    connect(_tcpLink, &NTRIPTCPLink::RTCMDataUpdate, _rtcmMavlink, &RTCMMavlink::RTCMDataUpdate);
    connect(_tcpLink, &NTRIPTCPLink::enabledChanged, this, &NTRIP::enabledChanged);
    connect(_tcpLink, &NTRIPTCPLink::connectionStatusChanged, this, &NTRIP::connectionStatusChanged);
}

void NTRIP::_stopLink() {
    if (_tcpLink) {
        _tcpLink->setEnabled(false);
        delete _tcpLink;
        _tcpLink = nullptr;
    }
    if (_rtcmMavlink) {
        delete _rtcmMavlink;
        _rtcmMavlink = nullptr;
    }
}

void NTRIP::_tcpError(const QString& errorMsg) {
    qCWarning(NTRIPLog) << "NTRIP connection error:" << errorMsg;
}

bool NTRIP::enabled() const { return _tcpLink && _tcpLink->enabled(); }
void NTRIP::setEnabled(bool en) {
    if (en) {
        _initLink();
        if (_tcpLink && !_tcpLink->enabled())
            _tcpLink->setEnabled(true);
    } else {
        _stopLink();
    }
}
int NTRIP::connectionStatus() const { return _tcpLink ? static_cast<int>(_tcpLink->connectionStatus()) : 0; }
bool NTRIP::masterEnable() const { return _settings && _settings->ntripServerConnectEnabled()->rawValue().toBool(); }

// =====================================================================
// NTRIPTCPLink Implementation

NTRIPTCPLink::NTRIPTCPLink(const QString &hostAddress, int port,
                           const QString &username, const QString &password,
                           const QString &mountpoint, const QString &whitelist,
                           const bool &enableVRS, const bool &forceNtripV1,
                           QObject* parent)
    : QObject(parent)
      , _hostAddress(hostAddress)
      , _port(port)
      , _username(username)
      , _password(password)
      , _mountpoint(mountpoint)
      , _isVRSEnable(enableVRS)
      , _forceNtripV1(forceNtripV1)
{
    _rtcm_parsing = new RTCMParsing();
    _rtcm_parsing->reset();

    for (const auto &msg : whitelist.split(',')) {
        bool ok;
        int msgInt = msg.toInt(&ok);
        if (ok) _whitelist.insert(msgInt);
    }

    _reconnectTimer = new QTimer(this);
    _reconnectTimer->setSingleShot(true);
    connect(_reconnectTimer, &QTimer::timeout, this, &NTRIPTCPLink::_retryConnection);

    if (_isVRSEnable) {
        _vrsSendTimer = new QTimer(this);
        _vrsSendTimer->setInterval(_vrsSendRateMSecs);
        connect(_vrsSendTimer, &QTimer::timeout, this, &NTRIPTCPLink::_sendNmeaGga);
    }

    _handshakeTimer = new QTimer(this);
    _handshakeTimer->setSingleShot(true);
    connect(_handshakeTimer, &QTimer::timeout, this, [this]() {
        if (_socket && _socket->state() == QAbstractSocket::ConnectedState) {
            _handleDisconnect(QString("handshake timeout (no complete caster response after %1 s, got %2 bytes)")
                                  .arg(_handshakeTimeoutMS / 1000).arg(_headerBuffer.size()));
        } else {
            _handleDisconnect(QString("handshake timeout (TCP connect to %1:%2 not completed after %3 s)")
                                  .arg(_hostAddress).arg(_port).arg(_handshakeTimeoutMS / 1000));
        }
    });

    _heartbeatTimer = new QTimer(this);
    _heartbeatTimer->setSingleShot(true);
    connect(_heartbeatTimer, &QTimer::timeout, this, [this]() {
        _handleDisconnect(QString("heartbeat timeout (no data received for %1 s)").arg(_heartbeatTimeoutMS / 1000));
    });
}

NTRIPTCPLink::~NTRIPTCPLink() {
    setEnabled(false);
    delete _rtcm_parsing;
}

// -------------------- Enable / Disable -------------------

void NTRIPTCPLink::setEnabled(bool en) {
    if (_enabled == en) return;
    _enabled = en;
    emit enabledChanged();

    if (_enabled) {
        _rapidRetryCount = 0;
        _retryDelayMS = _minRetryDelayMS;
        _initSocket();
    } else {
        _teardownSocket();
        if (_reconnectTimer) _reconnectTimer->stop();
        _setConnectionStatus(NTRIPStatus::Off);
    }
}

// -------------------- Socket Handling -------------------

void NTRIPTCPLink::_initSocket() {
    if (!_enabled) return;

    _resetStreamState();
    _setConnectionStatus(NTRIPStatus::Connecting);

    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::connected, this, &NTRIPTCPLink::_socketConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &NTRIPTCPLink::_socketDisconnected);
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &NTRIPTCPLink::_socketError);
    connect(_socket, &QTcpSocket::readyRead, this, &NTRIPTCPLink::_readBytes);

    qCDebug(NTRIPLog) << "Connecting to" << _hostAddress << ":" << _port << "mount" << _mountpoint;
    _handshakeTimer->start(_handshakeTimeoutMS);
    _socket->connectToHost(_hostAddress, static_cast<quint16>(_port));
}

// Stops all per-connection activity and releases the socket without triggering its signals
void NTRIPTCPLink::_teardownSocket() {
    if (_handshakeTimer) _handshakeTimer->stop();
    if (_heartbeatTimer) _heartbeatTimer->stop();
    if (_vrsSendTimer) _vrsSendTimer->stop();
    if (_socket) {
        _socket->disconnect(this);
        _socket->abort();
        _socket->deleteLater();
        _socket = nullptr;
    }
}

void NTRIPTCPLink::_resetStreamState() {
    _headerBuffer.clear();
    _chunked = false;
    _chunkBuffer.clear();
    _chunkRemaining = -1;
    _rtcm_parsing->reset();
    _connectedTimestamp = 0;
    _rxStatsBytes = 0;
    _rxStatsMsgCounts.clear();
    _rxTotalBytes = 0;
}

void NTRIPTCPLink::_socketConnected() {
    const QByteArray digest = QString(_username + ":" + _password).toUtf8().toBase64();
    const QByteArray mount = _mountpoint.toUtf8();
    QByteArray requestLine;
    QByteArray query;

    if (_forceNtripV1) {
        // NTRIP v1 casters (e.g. embedded base station casters) expect a minimal HTTP/1.0 request
        requestLine = "GET /" + mount + " HTTP/1.0";
        query = requestLine + "\r\n"
                "User-Agent: NTRIP QGroundControl\r\n"
                "Authorization: Basic " + digest + "\r\n"
                "\r\n";
    } else {
        requestLine = "GET /" + mount + " HTTP/1.1";
        query = requestLine + "\r\n"
                "Host: " + _hostAddress.toUtf8() + ":" + QByteArray::number(_port) + "\r\n"
                "User-Agent: NTRIP QGroundControl\r\n"
                "Ntrip-Version: Ntrip/2.0\r\n"
                "Authorization: Basic " + digest + "\r\n"
                "Connection: close\r\n"
                "\r\n";
    }

    qCDebug(NTRIPLog) << "TCP connected, sending NTRIP" << (_forceNtripV1 ? "v1" : "v2") << "request:" << requestLine;
    _socket->write(query);
}

void NTRIPTCPLink::_socketDisconnected() {
    _handleDisconnect(QString("remote host closed the connection"));
}

void NTRIPTCPLink::_socketError(QAbstractSocket::SocketError err) {
    if (err == QAbstractSocket::RemoteHostClosedError) {
        _handleDisconnect(QString("remote host closed the connection"));
    } else {
        _handleDisconnect(QString("socket error: %1").arg(_socket ? _socket->errorString() : QString("unknown")));
    }
}

// Common path for every connection loss that should be retried
void NTRIPTCPLink::_handleDisconnect(const QString& reason) {
    if (!_enabled || !_socket) return;   // already handled (errorOccurred and disconnected both fire)

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 connectedMS = _connectedTimestamp > 0 ? now - _connectedTimestamp : -1;

    if (connectedMS >= 0) {
        qCWarning(NTRIPLog) << "Disconnected:" << reason << "- was connected for" << connectedMS / 1000.0
                            << "s, received" << _rxTotalBytes << "bytes";
    } else {
        qCWarning(NTRIPLog) << "Connection failed before handshake completed:" << reason;
    }
    emit error(QString("NTRIP: %1").arg(reason));

    _teardownSocket();

    if (connectedMS >= _backoffResetThresholdMS) {
        _retryDelayMS = _minRetryDelayMS;
    }
    if (connectedMS >= _stableConnectionThresholdMS) {
        _rapidRetryCount = 0;
    } else {
        // Never reached Connected or died quickly - count toward timeout
        _rapidRetryCount++;
    }

    if (_rapidRetryCount >= _maxRapidRetries) {
        qCWarning(NTRIPLog) << "Giving up after" << _rapidRetryCount << "consecutive rapid failures";
        _setConnectionStatus(NTRIPStatus::TimedOut);
        return;
    }

    _setConnectionStatus(NTRIPStatus::Retrying);
    qCDebug(NTRIPLog) << "Retrying in" << _retryDelayMS / 1000 << "s (rapid failure" << _rapidRetryCount << "of" << _maxRapidRetries << ")";
    _reconnectTimer->start(_retryDelayMS);
    _retryDelayMS = qMin(_retryDelayMS * 2, _maxRetryDelayMS);
}

// -------------------- Read / Parse -------------------

void NTRIPTCPLink::_readBytes() {
    if (!_socket) return;

    if (_connectionStatus == NTRIPStatus::Connecting) {
        _headerBuffer += _socket->readAll();
        _processHandshake();
        return;
    }

    if (_connectionStatus == NTRIPStatus::Connected) {
        _processData(_socket->readAll());
    }
}

// Waits for a complete caster response header, then either moves to Connected or fails
void NTRIPTCPLink::_processHandshake() {
    // Raw RTCM over TCP (no NTRIP caster): data starts immediately with an RTCM3 preamble
    if (!_headerBuffer.isEmpty() && static_cast<uint8_t>(_headerBuffer.at(0)) == RTCM3_PREAMBLE) {
        qCDebug(NTRIPLog) << "Response starts with RTCM3 data, treating as raw RTCM stream";
        _handshakeComplete(_headerBuffer);
        return;
    }

    const int eol = _headerBuffer.indexOf('\n');
    if (eol < 0) {
        if (_headerBuffer.size() > _maxHeaderBytes) {
            _handshakeFailed(QString("no status line in first %1 bytes").arg(_headerBuffer.size()));
        }
        return;
    }

    const QByteArray statusLine = _headerBuffer.left(eol).trimmed();

    if (statusLine.startsWith("ICY 200")) {
        // NTRIP v1: no further header, RTCM may follow right after the status line
        qCDebug(NTRIPLog) << "Caster response:" << statusLine;
        QByteArray leftover = _headerBuffer.mid(eol + 1);
        if (leftover.startsWith("\r\n")) {
            leftover.remove(0, 2);
        } else if (leftover.startsWith('\n')) {
            leftover.remove(0, 1);
        }
        _handshakeComplete(leftover);
        return;
    }

    if (statusLine.startsWith("HTTP/1.0 200") || statusLine.startsWith("HTTP/1.1 200")) {
        const int headerEnd = _headerBuffer.indexOf("\r\n\r\n");
        if (headerEnd < 0) {
            if (_headerBuffer.size() > _maxHeaderBytes) {
                _handshakeFailed(QString("%1 (header not terminated)").arg(QString::fromLatin1(statusLine)));
            }
            return;
        }
        qCDebug(NTRIPLog) << "Caster response:" << statusLine;

        const QList<QByteArray> headerLines = _headerBuffer.mid(eol + 1, headerEnd - eol - 1).split('\n');
        for (const QByteArray& rawLine : headerLines) {
            const QByteArray line = rawLine.trimmed().toLower();
            if (line.startsWith("transfer-encoding:") && line.contains("chunked")) {
                _chunked = true;
            } else if (line.startsWith("content-type:") && line.contains("sourcetable")) {
                _handshakeFailed(QString("%1 (%2)").arg(QString::fromLatin1(statusLine), QString::fromLatin1(rawLine.trimmed())));
                return;
            }
        }
        qCDebug(NTRIPLog) << "Header complete," << headerLines.size() << "header lines, chunked:" << _chunked;
        _handshakeComplete(_headerBuffer.mid(headerEnd + 4));
        return;
    }

    _handshakeFailed(QString::fromLatin1(statusLine.left(200)));
}

void NTRIPTCPLink::_handshakeComplete(const QByteArray& leftover) {
    _headerBuffer.clear();
    _handshakeTimer->stop();
    _connectedTimestamp = QDateTime::currentMSecsSinceEpoch();
    _rxStatsTimer.start();
    _setConnectionStatus(NTRIPStatus::Connected);
    _heartbeatTimer->start(_heartbeatTimeoutMS);
    if (_isVRSEnable && _vrsSendTimer) _vrsSendTimer->start();

    if (!leftover.isEmpty()) {
        _processData(leftover);
    }
}

// Caster rejected the request; retrying will not help so the link is disabled
void NTRIPTCPLink::_handshakeFailed(const QString& statusLine) {
    const QString lower = statusLine.toLower();
    QString reason;
    if (lower.contains(" 401") || lower.contains("unauthorized") || lower.contains("bad password")) {
        reason = QString("caster rejected credentials, check username/password");
    } else if (lower.contains("sourcetable") || lower.contains(" 404") || lower.contains("bad mountpoint")) {
        reason = QString("mount point not found (\"%1\")").arg(_mountpoint);
    } else {
        reason = QString("caster rejected request");
    }

    qCWarning(NTRIPLog) << "Handshake failed:" << reason << "- caster said:" << statusLine;
    emit error(QString("NTRIP: %1 - caster said: %2").arg(reason, statusLine));
    setEnabled(false);
}

void NTRIPTCPLink::_processData(const QByteArray& data) {
    if (data.isEmpty()) return;

    _heartbeatTimer->start(_heartbeatTimeoutMS); // restart on every received byte
    _rxStatsBytes += data.size();
    _rxTotalBytes += data.size();

    if (_chunked) {
        QByteArray payload;
        if (!_dechunk(data, payload)) {
            _handleDisconnect(QString("malformed chunked transfer encoding"));
            return;
        }
        _parseRtcm(payload);
    } else {
        _parseRtcm(data);
    }

    if (_rxStatsTimer.elapsed() >= _rxStatsIntervalMS) {
        QStringList counts;
        for (auto it = _rxStatsMsgCounts.cbegin(); it != _rxStatsMsgCounts.cend(); ++it) {
            counts << QString("%1x%2").arg(it.key()).arg(it.value());
        }
        qCDebug(NTRIPLog) << "Receiving" << qRound(_rxStatsBytes * 1000.0 / _rxStatsTimer.elapsed())
                          << "bytes/s, RTCM:" << (counts.isEmpty() ? QString("none") : counts.join(", "));
        _rxStatsBytes = 0;
        _rxStatsMsgCounts.clear();
        _rxStatsTimer.restart();
    }
}

// Strips HTTP chunk framing (hex size line, data, CRLF). Returns false on malformed input.
bool NTRIPTCPLink::_dechunk(const QByteArray& data, QByteArray& payload) {
    _chunkBuffer += data;

    while (!_chunkBuffer.isEmpty()) {
        if (_chunkRemaining > 0) {
            const int n = qMin(_chunkRemaining, static_cast<int>(_chunkBuffer.size()));
            payload += _chunkBuffer.left(n);
            _chunkBuffer.remove(0, n);
            _chunkRemaining -= n;
        } else if (_chunkRemaining == 0) {
            if (_chunkBuffer.size() < 2) break;
            if (!_chunkBuffer.startsWith("\r\n")) return false;
            _chunkBuffer.remove(0, 2);
            _chunkRemaining = -1;
        } else {
            const int eol = _chunkBuffer.indexOf("\r\n");
            if (eol < 0) {
                return _chunkBuffer.size() <= 32;   // size line not complete yet
            }
            QByteArray sizeField = _chunkBuffer.left(eol);
            const int ext = sizeField.indexOf(';');
            if (ext >= 0) sizeField.truncate(ext);
            bool ok = false;
            const int size = sizeField.trimmed().toInt(&ok, 16);
            if (!ok || size < 0) return false;
            _chunkBuffer.remove(0, eol + 2);
            if (size == 0) {
                // Last chunk: caster is ending the stream and will close the connection
                qCDebug(NTRIPLog) << "Received last chunk from caster";
                _chunkBuffer.clear();
                break;
            }
            _chunkRemaining = size;
        }
    }
    return true;
}

void NTRIPTCPLink::_parseRtcm(const QByteArray& payload) {
    for (const char c : payload) {
        if (_rtcm_parsing->addByte(static_cast<uint8_t>(c))) {
            const uint8_t* buf = _rtcm_parsing->message();
            QByteArray msg(reinterpret_cast<const char*>(buf), _rtcm_parsing->messageLength());
            // RTCM3 message number is the first 12 bits of the payload
            // (RTCMParsing::messageId() shifts by 8 instead of 4, so compute it here)
            const int id = (buf[3] << 4) | (buf[4] >> 4);
            const uint8_t version = _rtcm_parsing->rtcmVersion();

            if (version == 3) {
                _rxStatsMsgCounts[id]++;
                if (_whitelist.empty() || _whitelist.contains(id)) {
                    emit RTCMDataUpdate(msg);
                }
            }
            _rtcm_parsing->reset();
        }
    }
}

// -------------------- Retry / GGA -------------------

void NTRIPTCPLink::_retryConnection() {
    if (!_enabled) return;
    _initSocket();
}

void NTRIPTCPLink::_sendNmeaGga() {
    auto vehicleMgr = MultiVehicleManager::instance();
    if (!vehicleMgr->activeVehicle()) return;
    Vehicle* vehicle = vehicleMgr->activeVehicle();
    NmeaMessage nmea(vehicle->coordinate());
    if (_socket) _socket->write(nmea.getGGA().toUtf8());
}

// -------------------- Status -------------------

void NTRIPTCPLink::_setConnectionStatus(NTRIPStatus newStatus) {
    if (_connectionStatus != newStatus) {
        _connectionStatus = newStatus;
        emit connectionStatusChanged();
    }
}
