/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoCoordinate>
#include <QtCore/QLoggingCategory>

#include "MAVLinkLib.h"

Q_DECLARE_LOGGING_CATEGORY(RemoteIDManagerLog)

class RemoteIDSettings;
class Vehicle;

// Supporting Open Drone ID protocol
class RemoteIDManager : public QObject
{
    Q_OBJECT

public:
    RemoteIDManager(Vehicle* vehicle);

    Q_PROPERTY(bool    available            READ available          NOTIFY availableChanged)             ///< true: the vehicle supports Mavlink Open Drone ID messages
    Q_PROPERTY(bool    armStatusGood        READ armStatusGood      NOTIFY armStatusGoodChanged)
    Q_PROPERTY(QString armStatusError       READ armStatusError     NOTIFY armStatusErrorChanged)
    Q_PROPERTY(bool    commsGood            READ commsGood          NOTIFY commsGoodChanged)
    Q_PROPERTY(bool    gcsGPSGood           READ gcsGPSGood         NOTIFY gcsGPSGoodChanged)
    Q_PROPERTY(bool    basicIDGood          READ basicIDGood        NOTIFY basicIDGoodChanged)
    Q_PROPERTY(bool    emergencyDeclared    READ emergencyDeclared  NOTIFY emergencyDeclaredChanged)
    Q_PROPERTY(bool    operatorIDGood       READ operatorIDGood     NOTIFY operatorIDGoodChanged)

    // Position currently being broadcast as the operator location. Source can
    // be either the GCS GPS or the drone (fallback). Tag is "G", "D", or "".
    Q_PROPERTY(double   broadcastLatitude       READ broadcastLatitude      NOTIFY broadcastPositionChanged)
    Q_PROPERTY(double   broadcastLongitude      READ broadcastLongitude     NOTIFY broadcastPositionChanged)
    Q_PROPERTY(double   broadcastAltitude       READ broadcastAltitude      NOTIFY broadcastPositionChanged)
    Q_PROPERTY(bool     broadcastPositionValid  READ broadcastPositionValid NOTIFY broadcastPositionChanged)
    Q_PROPERTY(QString  positionSourceTag       READ positionSourceTag      NOTIFY broadcastPositionChanged)

    Q_INVOKABLE void checkOperatorID(const QString& operatorID);
    Q_INVOKABLE void setOperatorID();

    // Declare emergency
    Q_INVOKABLE void setEmergency(bool declare);

    bool    available           (void) const { return _available; }
    bool    armStatusGood       (void) const { return _armStatusGood; }
    QString armStatusError      (void) const { return _armStatusError; }
    bool    commsGood           (void) const { return _commsGood; }
    bool    gcsGPSGood          (void) const { return _gcsGPSGood; }
    bool    basicIDGood         (void) const { return _basicIDGood; }
    bool    emergencyDeclared   (void) const { return _emergencyDeclared;}
    bool    operatorIDGood      (void) const { return _operatorIDGood; }

    double  broadcastLatitude       (void) const { return _broadcastPosition.latitude(); }
    double  broadcastLongitude      (void) const { return _broadcastPosition.longitude(); }
    double  broadcastAltitude       (void) const { return _broadcastPosition.altitude(); }
    bool    broadcastPositionValid  (void) const { return _broadcastPositionValid; }
    QString positionSourceTag       (void) const;

    void mavlinkMessageReceived (mavlink_message_t& message);

    enum LocationTypes {
        TAKEOFF,
        LiveGNSS,
        FIXED
    };

    enum Region {
        FAA,
        EU
    };

signals:
    void availableChanged();
    void armStatusGoodChanged();
    void armStatusErrorChanged();
    void commsGoodChanged();
    void gcsGPSGoodChanged();
    void basicIDGoodChanged();
    void emergencyDeclaredChanged();
    void operatorIDGoodChanged();
    void broadcastPositionChanged();

private slots:
    void _odidTimeout();
    void _sendMessages();
    void _updateLastGCSPositionInfo(QGeoPositionInfo update);
    void _checkGCSBasicID();

private:
    void _handleArmStatus(mavlink_message_t& message);

    // Self ID
    void        _sendSelfIDMsg ();
    const char* _getSelfIDDescription();

    // Operator ID
    void        _sendOperatorID ();

    // System
    void        _sendSystem();
    uint32_t    _timestamp2019();

    // Basic ID
    void        _sendBasicID();

    bool _isEUOperatorIDValid(const QString& operatorID) const;
    QChar _calculateLuhnMod36(const QString& input) const;

    Vehicle*            _vehicle;
    RemoteIDSettings*   _settings;

    // Flags ODID
    bool    _available = false;
    bool    _armStatusGood;
    QString _armStatusError;
    bool    _commsGood;
    bool    _gcsGPSGood;
    bool    _basicIDGood;
    bool    _GCSBasicIDValid;
    bool    _operatorIDGood;

    bool        _emergencyDeclared;
    QDateTime   _lastGeoPositionTimeStamp;
    int         _targetSystem;
    int         _targetComponent;

    // After emergency cleared, this makes sure the non emergency selfID message makes it to the vehicle
    bool        _enforceSendingSelfID;

    // Operator-location source state machine. We prefer the GCS (Android)
    // GPS, but it can take a long time to acquire. After the initial wait
    // expires we fall back to broadcasting the *drone's* position as the
    // operator location for testing. Once the GCS has provided a fix at any
    // point, we won't fall back to the drone again unless the drone has been
    // disarmed for a sustained period without a GCS fix. See _sendSystem().
    bool        _gcsEverGood                    = false;   // latched true on first valid GCS fix
    bool        _droneFallbackActive            = false;   // true while we're broadcasting the drone's coordinate
    QGeoCoordinate _lastGoodGcsPosition;                   // most recent good GCS fix (used while stale, post-first-fix)
    QDateTime   _droneFallbackEligibleStartTime;           // set while drone disarmed AND GCS stale post-first-fix; cleared otherwise
    QDateTime   _startupTime;                              // construction time, for the initial-wait timer

    // Currently-broadcast operator position (mirrors what's going out on the
    // wire; exposed to QML for the Remote ID settings page readouts).
    QGeoCoordinate _broadcastPosition;
    bool        _broadcastPositionValid         = false;
    bool        _broadcastUsingDrone            = false;   // false → GCS, true → drone

    static const uint8_t* _id_or_mac_unknown;

    // Timers
    QTimer _odidTimeoutTimer;
    QTimer _sendMessagesTimer;
};
