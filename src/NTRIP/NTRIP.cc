/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "NTRIP.h"
#include "NTRIPSettings.h"
#include "NmeaMessage.h"
#include "PositionManager.h"
#include "QGCApplication.h"
#include "QGCLoggingCategory.h"
#include "QGCToolbox.h"
#include "SettingsManager.h"

QGC_LOGGING_CATEGORY(NTRIPLog, "NTRIP")

NTRIP::NTRIP(QGCApplication *app, QGCToolbox *toolbox)
    : QGCTool(app, toolbox) {}

void NTRIP::setToolbox(QGCToolbox *toolbox) {
  QGCTool::setToolbox(toolbox);
}

void NTRIP::_initLink() {
    if (_tcpLink || _rtcmMavlink) return; // Already initialized

    NTRIPSettings *settings = qgcApp()->toolbox()->settingsManager()->ntripSettings();
    _rtcmMavlink = new RTCMMavlink(*_toolbox);

    _tcpLink = new NTRIPTCPLink(
        settings->ntripServerHostAddress()->rawValue().toString(),
        settings->ntripServerPort()->rawValue().toInt(),
        settings->ntripUsername()->rawValue().toString(),
        settings->ntripPassword()->rawValue().toString(),
        settings->ntripMountpoint()->rawValue().toString(),
        settings->ntripWhitelist()->rawValue().toString(),
        settings->ntripEnableVRS()->rawValue().toBool()
    );

    connect(_tcpLink, &NTRIPTCPLink::error, this, &NTRIP::_tcpError, Qt::QueuedConnection);
    connect(_tcpLink, &NTRIPTCPLink::RTCMDataUpdate, _rtcmMavlink, &RTCMMavlink::RTCMDataUpdate);
    connect(_tcpLink, &NTRIPTCPLink::enabledChanged, this, &NTRIP::enabledChanged);
    connect(_tcpLink, &NTRIPTCPLink::connectionStatusChanged, this, &NTRIP::connectionStatusChanged);

    _tcpLink->start();  // Start thread after creation

}

void NTRIP::_tcpError(const QString errorMsg) {
  //qgcApp()->showAppMessage(tr("NTRIP Server Error: %1").arg(errorMsg));
  qWarning() << "NTRIP connection error:" << errorMsg;
}

//void NTRIP::setEnabled(bool en) {
//    if (en) {
//        if (!_tcpLink) {
//            _initLink();  // Only init when enabling
//            //_tcpLink->start();  // Start thread immediately after creation
//        }
//        _tcpLink->setEnabled(true);
//        emit enabledChanged();
//    } else {
//        if (_tcpLink) {
//            _tcpLink->setEnabled(false);
//            // Don't delete here — let thread quit safely
//            _tcpLink->quit();
//            _tcpLink->wait();
//            delete _tcpLink;
//            _tcpLink = nullptr;
//        }
//        if (_rtcmMavlink) {
//            delete _rtcmMavlink;
//            _rtcmMavlink = nullptr;
//        }
//    }
//}

void NTRIP::setEnabled(bool en) {
    if (!_tcpLink) {
        _initLink();  // Only init when enabling
    }
    _tcpLink->setEnabled(en);
    emit enabledChanged();
}

bool NTRIP::enabled() const {
    return _tcpLink && _tcpLink->enabled();
}

int NTRIP::connectionStatus() const {
    return _tcpLink ? static_cast<int>(_tcpLink->connectionStatus()) : 0;
}

bool NTRIP::masterEnable() const {
    NTRIPSettings* settings = qgcApp()->toolbox()->settingsManager()->ntripSettings();
    return settings->ntripServerConnectEnabled()->rawValue().toBool();
}

//void NTRIP::applySettings() {
//    // Fully shutdown and restart using new parameters
//    if (_tcpLink) {
//        _tcpLink->setEnabled(false);
//        _tcpLink->quit();
//        _tcpLink->wait();
//        delete _tcpLink;
//        _tcpLink = nullptr;
//    }

//    if (_rtcmMavlink) {
//        delete _rtcmMavlink;
//        _rtcmMavlink = nullptr;
//    }

//    // Re-init link using new parameters
//    _initLink();
//}

// ===================== NTRIPTCPLink =========================

NTRIPTCPLink::NTRIPTCPLink(const QString &hostAddress, int port,
                           const QString &username, const QString &password,
                           const QString &mountpoint, const QString &whitelist,
                           const bool &enableVRS)
    : QThread(), _hostAddress(hostAddress), _port(port), _username(username),
      _password(password), _mountpoint(mountpoint), _isVRSEnable(enableVRS),
      _toolbox(qgcApp()->toolbox()) {


  for (const auto &msg : whitelist.split(',')) {
    int msg_int = msg.toInt();
    if (msg_int) {
      _whitelist.insert(msg_int);
    }
  }
  //qCDebug(NTRIPLog) << "whitelist: " << _whitelist;
  if (!_rtcm_parsing) {
    _rtcm_parsing = new RTCMParsing();
  }
  _rtcm_parsing->reset();
  _state = NTRIPState::uninitialised;
  _connectionStatus = NTRIPStatus::Off;

  moveToThread(this);
  start();

}

//NTRIPTCPLink::~NTRIPTCPLink(void) {
//  qCDebug(NTRIPLog) << "NTRIP Thread stopped";
//  if (_socket) {
//    if (_isVRSEnable) {
//      _vrsSendTimer->stop();
//      QObject::disconnect(_vrsSendTimer, &QTimer::timeout, this,
//                          &NTRIPTCPLink::_sendNmeaGga);
//      delete _vrsSendTimer;
//      _vrsSendTimer = nullptr;
//    }
//    QObject::disconnect(_socket, &QTcpSocket::readyRead, this,
//                        &NTRIPTCPLink::_readBytes);
//    _socket->disconnectFromHost();
//    _socket->deleteLater();
//    _socket = nullptr;

//    // Delete Rtcm Parsing instance
//    delete (_rtcm_parsing);
//    _rtcm_parsing = nullptr;
//  }
//  quit();
//  wait();
//} // end of old ~NTRIPTCPLink

//NTRIPTCPLink::~NTRIPTCPLink() {
//  if (_vrsSendTimer) delete _vrsSendTimer;
//  if (_watchdogTimer) delete _watchdogTimer;
//  if (_reconnectTimer) delete _reconnectTimer;
//  if (_socket) {
//    _socket->disconnectFromHost();
//    delete _socket;
//  }
//  delete _rtcm_parsing;
//}


NTRIPTCPLink::~NTRIPTCPLink() {
    if (isRunning()) {
        // Stop everything inside the thread first
        QMetaObject::invokeMethod(this, "_stopNTRIP", Qt::BlockingQueuedConnection);
        quit();
        wait();
    }

    // Now safely delete these (because now we're guaranteed to be back in the correct thread context)
    delete _watchdogTimer;
    delete _reconnectTimer;
    delete _vrsSendTimer;
    delete _rtcm_parsing;
}

void NTRIPTCPLink::run(void) {
  qCDebug(NTRIPLog) << "NTRIP Thread started";
  // Create timers *after* thread starts
  _watchdogTimer = new QTimer();
  _watchdogTimer->setInterval(_watchdogTimeoutMSecs);
  _watchdogTimer->setSingleShot(true);
  connect(_watchdogTimer, &QTimer::timeout, this, &NTRIPTCPLink::_onWatchdogTimeout);

  _reconnectTimer = new QTimer();
  _reconnectTimer->setInterval(10000);
  _reconnectTimer->setSingleShot(true);
  connect(_reconnectTimer, &QTimer::timeout, this, &NTRIPTCPLink::_retryNow);
  //connect(_reconnectTimer, &QTimer::timeout, this, &NTRIPTCPLink::_retryConnection);

  // Init VRS Timer
  if (_isVRSEnable) {
    _vrsSendTimer = new QTimer();
    _vrsSendTimer->setInterval(_vrsSendRateMSecs);
    _vrsSendTimer->setSingleShot(false);
    QObject::connect(_vrsSendTimer, &QTimer::timeout, this, &NTRIPTCPLink::_sendNmeaGga);
  }

  exec();
} // end of run

void NTRIPTCPLink::setEnabled(bool en) {
    if (_enabled == en) return;

    _enabled = en;

    emit enabledChanged();

    if (_enabled) {
        QMetaObject::invokeMethod(this, "_startNTRIP", Qt::QueuedConnection);
    } else {
        QMetaObject::invokeMethod(this, "_stopNTRIP", Qt::BlockingQueuedConnection);
    }
}

void NTRIPTCPLink::_startNTRIP() {
    _retryCount = 0;
    _setConnectionStatus(NTRIPStatus::Connecting);
    _hardwareConnect();
    if (_isVRSEnable && _vrsSendTimer) {
        _vrsSendTimer->start();
    }
}

void NTRIPTCPLink::_stopNTRIP() {
    //qCDebug(NTRIPLog) << "Stopping NTRIP connection";

    if (_watchdogTimer) {
        //qCDebug(NTRIPLog) << "Before stopping Watchdog";
        if (_watchdogTimer->isActive()) {
            //qCDebug(NTRIPLog) << "Watchdog is active, stopping";
            _watchdogTimer->stop();
        } else {
            //qCDebug(NTRIPLog) << "Watchdog is not active";
        }
        //qCDebug(NTRIPLog) << "After stopping Watchdog";
    } else {
        //qCDebug(NTRIPLog) << "Watchdog timer nullptr";
    }

    if (_reconnectTimer) {
        //qCDebug(NTRIPLog) << "Before stopping Reconnect";
        if (_reconnectTimer->isActive()) {
            //qCDebug(NTRIPLog) << "Reconnect is active, stopping";
            _reconnectTimer->stop();
        } else {
            //qCDebug(NTRIPLog) << "Reconnect is not active";
        }
        //qCDebug(NTRIPLog) << "After stopping Reconnect";
    } else {
        //qCDebug(NTRIPLog) << "Reconnect timer nullptr";
    }

    if (_isVRSEnable) {
        if (_vrsSendTimer) {
            //qCDebug(NTRIPLog) << "Before stopping VRS";
            if (_vrsSendTimer->isActive()) {
                //qCDebug(NTRIPLog) << "VRS is active, stopping";
                _vrsSendTimer->stop();
            } else {
                //qCDebug(NTRIPLog) << "VRS is not active";
            }
            //qCDebug(NTRIPLog) << "After stopping VRS";
        } else {
            //qCDebug(NTRIPLog) << "VRS timer nullptr";
        }
    }
    else
    {
        //qCDebug(NTRIPLog) << "VRS not enabled";
    }

    // Socket code as before
    if (_socket) {
        _socket->blockSignals(true);
        //qCDebug(NTRIPLog) << "Disconnect 1a";
        QObject::disconnect(_socket, nullptr, nullptr, nullptr);
        //qCDebug(NTRIPLog) << "Disconnect 1";
        _socket->disconnectFromHost();
        //qCDebug(NTRIPLog) << "Disconnect 2";
        _socket->deleteLater();
        //qCDebug(NTRIPLog) << "Disconnect 3";
        _socket = nullptr;
        //qCDebug(NTRIPLog) << "Disconnect 4";
    }

    _retryCount = 0;
    _setConnectionStatus(NTRIPStatus::Off);
    //qCDebug(NTRIPLog) << "Disconnect 5";
}


void NTRIPTCPLink::_hardwareConnect() {
  qCDebug(NTRIPLog) << "Connecting to NTRIP Server: " << _hostAddress << ":" << _port;

  if (_socket) {
      delete _socket;
      _socket = nullptr;
  }

  _socket = new QTcpSocket();


  QObject::connect(_socket, &QTcpSocket::readyRead, this, &NTRIPTCPLink::_readBytes);
  QObject::connect(_socket, &QTcpSocket::disconnected, this, &NTRIPTCPLink::_onDisconnected);
  connect(_socket, &QTcpSocket::errorOccurred, this, &NTRIPTCPLink::_onSocketError);

  _socket->connectToHost(_hostAddress, static_cast<quint16>(_port));

  _setConnectionStatus(NTRIPStatus::Connecting);

  // Give the socket a second to connect to the other side otherwise error out
  if (!_socket->waitForConnected(2000)) {
      emit error("Ntrip connection attempt timed out");
      _scheduleRetry();
      return;
  }

  // If mountpoint is specified, send an http get request for data
  if (!_mountpoint.isEmpty()) {
    qCDebug(NTRIPLog) << "Sending HTTP request, using mount point: " << _mountpoint;
    QString digest = QString(_username + ":" + _password).toUtf8().toBase64();
    QString auth = QString("Authorization: Basic %1\r\n").arg(digest);
    QString query;
    if (_ntripForceV1) {
      query = "GET /%1 HTTP/1.0\r\n"
              "User-Agent: NTRIP QGroundControl\r\n"
              "%2"
              "Connection: close\r\n\r\n";
    } else {
      query = "GET /%1 HTTP/1.1\r\n"
              "User-Agent: NTRIP QGroundControl\r\n"
              "Ntrip-Version: Ntrip/2.0\r\n"
              "%2"
              "Connection: close\r\n\r\n";
    }

    _socket->write(query.arg(_mountpoint).arg(auth).toUtf8());
    _state = NTRIPState::waiting_for_http_response;
  } else {
    // If no mountpoint is set, assume we will just get data from the tcp stream
    _state = NTRIPState::waiting_for_rtcm_header;
  }


    _watchdogTimer->start();
    // TODO - figure out if this is good here
    if (_isVRSEnable && _vrsSendTimer) _vrsSendTimer->start();

    _retryCount = 0;

    qCDebug(NTRIPLog) << "NTRIP Socket connected";
} // end of hardwareConnect

//void NTRIPTCPLink::_retryConnection() {
//  if (_retryCount < _maxRetries && _enabled) {
//    _retryCount++;
//    emit error("Ntrip connection failed, retrying.");
//    _setConnectionStatus(NTRIPStatus::Retrying);
//    _reconnectTimer->start();
//    connect(_reconnectTimer, &QTimer::timeout, this, &NTRIPTCPLink::_hardwareConnect, Qt::UniqueConnection);
//  } else {
//    _setConnectionStatus(NTRIPStatus::TimedOut);
//    emit error("Ntrip connection timed out after max retries.");
//  }
//} // end of retryConnection

//void NTRIPTCPLink::_scheduleRetry() {
//    if (_retryCount < _maxRetries && _enabled) {
//        _retryCount++;
//        emit error("Ntrip connection failed, retrying.");
//        _setConnectionStatus(NTRIPStatus::Retrying);
//        if (!_reconnectTimer->isActive())
//            _reconnectTimer->start();
//    } else {
//        _setConnectionStatus(NTRIPStatus::TimedOut);
//        emit error("Ntrip connection timed out after max retries.");
//    }
//}

void NTRIPTCPLink::_retryNow() {
    _isRetryScheduled = false;
    _hardwareConnect();
}

void NTRIPTCPLink::_scheduleRetry() {
    if (_isRetryScheduled) {
        qCDebug(NTRIPLog) << "Retry already scheduled, skipping duplicate.";
        return;
    }

    if (!_enabled) {
        qCDebug(NTRIPLog) << "Not retrying because NTRIP is disabled.";
        return;
    }

    if (_retryCount >= _maxRetries) {
        _setConnectionStatus(NTRIPStatus::TimedOut);
        emit error("Ntrip connection timed out after max retries.");
        return;
    }

    _isRetryScheduled = true;
    _retryCount++;
    emit error("Ntrip connection failed, retrying.");
    _setConnectionStatus(NTRIPStatus::Retrying);
    _reconnectTimer->start(6000);  // retry after 1 second
}

void NTRIPTCPLink::_onDisconnected() {
    emit error("NTRIP socket disconnected.");
    _setConnectionStatus(NTRIPStatus::Off);
}

void NTRIPTCPLink::_onSocketError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError)
    emit error(QString("Socket error occurred: %1").arg(_socket->errorString()));
    _setConnectionStatus(NTRIPStatus::Connecting);
    _scheduleRetry();
}

void NTRIPTCPLink::_onWatchdogTimeout() {
    emit error("NTRIP data timeout. No data received for 15 seconds.");
    if (_socket) {
        _socket->disconnectFromHost();
    }
    _scheduleRetry();
}

void NTRIPTCPLink::_readBytes(void) {
  //QMutexLocker locker(&_lock);
  //qCDebug(NTRIPLog) << "Reading bytes";
  if (!_socket) {
    return;
  }
  if (_state == NTRIPState::waiting_for_http_response) {
    QString line = _socket->readLine();
    //qCDebug(NTRIPLog) << "Server responded with " << line;
    if (line.contains("200")) {
      //qCDebug(NTRIPLog) << "Server responded with " << line;
      if (line.contains("SOURCETABLE")) {
        //qCDebug(NTRIPLog) << "Server responded with SOURCETABLE, not supported";
        emit error("NTRIP Server responded with SOURCETABLE. Bad mountpoint?");
        _state = NTRIPState::uninitialised;
      } else {
        _state = NTRIPState::waiting_for_rtcm_header;
      }
    } else if (line.contains("401")) {
      qCWarning(NTRIPLog) << "Server responded with " << line;
      emit error("NTRIP Server responded with 401 Unauthorized");
      _state = NTRIPState::uninitialised;
    } else {
      qCWarning(NTRIPLog) << "Server responded with " << line;
      // TODO: Handle failure. Reconnect?
      // Just move into parsing mode and hope for now.
      _state = NTRIPState::waiting_for_rtcm_header;
    }
  }

  if (_state == NTRIPState::uninitialised) {
    //qCDebug(NTRIPLog) << "NTRIP State is uninitialised. Discarding bytes";
    _socket->readAll();
    return;
  }

  QByteArray bytes = _socket->readAll();
  _parse(bytes);
}

void NTRIPTCPLink::_parse(const QByteArray &buffer) {
    _setConnectionStatus(NTRIPStatus::Connected);
    _retryCount = 0;
  //qCDebug(NTRIPLog) << "Parsing " << buffer.size() << " bytes";
  //qCDebug(NTRIPLog) << "Buffer: " << QString(buffer);
  for (const uint8_t byte : buffer) {
    if (_state == NTRIPState::waiting_for_rtcm_header) {
      if (byte != RTCM3_PREAMBLE && byte != RTCM2_PREAMBLE) {
        //qCDebug(NTRIPLog) << "NOT RTCM 2/3 preamble, ignoring byte " << byte;
        continue;
      }
      _state = NTRIPState::accumulating_rtcm_packet;
    }

    if (_rtcm_parsing->addByte(byte)) {
      _state = NTRIPState::waiting_for_rtcm_header;
      QByteArray message((char *)_rtcm_parsing->message(),
                         static_cast<int>(_rtcm_parsing->messageLength()));
      uint16_t id = _rtcm_parsing->messageId();
      uint8_t version = _rtcm_parsing->rtcmVersion();
      qCDebug(NTRIPLog) << "RTCM version " << version;
      qCDebug(NTRIPLog) << "RTCM message ID " << id;
      qCDebug(NTRIPLog) << "RTCM message size " << message.size();

      if (version == 2) {
        //qCWarning(NTRIPLog) << "RTCM 2 not supported";
        emit error("Server sent RTCM 2 message. Not supported!");
        continue;
      } else if (version != 3) {
        //qCWarning(NTRIPLog) << "Unknown RTCM version " << version;
        emit error("Server sent unknown RTCM version");
        continue;
      }

      if (_watchdogTimer) {
          _watchdogTimer->start();  // Reset watchdog
      }


      if (_whitelist.empty() || _whitelist.contains(id)) {
        //qCDebug(NTRIPLog) << "Sending message ID [" << id << "] of size "
        //                  << message.length();
        emit RTCMDataUpdate(message);
      } else {
        //qCDebug(NTRIPLog) << "Ignoring " << id;
      }
      _rtcm_parsing->reset();
    }
  }
}

void NTRIPTCPLink::_sendNmeaGga() {
  //qCDebug(NTRIPLog) << "Sending NMEA GGA";

  if (!_toolbox->multiVehicleManager()->activeVehicleAvailable()) {
    qCDebug(NTRIPLog) << "No active vehicle";
    return;
  }
  qCDebug(NTRIPLog) << "Active vehicle found. Using vehicle position";

  if (_toolbox->multiVehicleManager()->vehicles()->count() > 1) {
    // TODO(bzd): Consider how to handle multiple vehicles - best option would be
    // send separate RTCM messages for each vehicle or at least
    // calculate the average position of all vehicles and use one RTCM for all
    qCDebug(NTRIPLog) << "More than one vehicle found. Using active vehicle for correction";
  }

  Vehicle *vehicle = _toolbox->multiVehicleManager()->activeVehicle();
  NmeaMessage nmeaMessage(vehicle->coordinate());
  // Write NMEA message
  if (_socket) {
    const QString &gga = nmeaMessage.getGGA();
    _socket->write(gga.toUtf8());
    qCDebug(NTRIPLog) << "NMEA GGA Message : " << gga;
  }
}

void NTRIPTCPLink::_setConnectionStatus(NTRIPStatus newStatus) {
    if (_connectionStatus != newStatus) {
        _connectionStatus = newStatus;
        emit connectionStatusChanged();
    }
}
