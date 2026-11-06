/****************************************************************************
 * Mavlink Sender Page (QML-only payload detection)
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.FactSystem
import QGroundControl.FactControls

ToolIndicatorPage {
    showExpand: false

    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    FactPanelController { id: controller }

    //--------------------------------------
    // Configurable payload settings
    //--------------------------------------
    property var    _flyViewSettings:   QGroundControl.settingsManager.flyViewSettings
    property string _serialParamName:   "SERIAL" + _flyViewSettings.payloadSerialPort.value + "_BAUD"
    property int    _ilxBaud:           _flyViewSettings.payloadIlxBaud.value
    property int    _vioBaud:           _flyViewSettings.payloadVioBaud.value
    property int    _mavCamBaud:        _flyViewSettings.payloadMavlinkCamBaud.value
    property int    _mavCamType:        _flyViewSettings.payloadMavlinkCamType.value

    //--------------------------------------
    // Payload-related parameters
    //--------------------------------------
    property Fact _serialBaudFact
    property Fact _camTypeFact
    property int payloadType: 0   // 0=unknown, 1=ILX, 2=VIO

    // Payload selection: 0=ILX-LR1, 1=VIO, 2=LiDAR, 3=MAVLink Camera (generic)
    // _selectedPayload reflects the authoritative persisted value — all other UI depends on this.
    // _pendingPayload is the combo-box in-flight choice; only persisted when user hits Apply.
    // Closing the drawer without applying discards _pendingPayload (property is re-initialised on reopen).
    property int _selectedPayload: _flyViewSettings.payloadSelection.value
    // A fresh/unconfigured airframe still has the default selection persisted (ILX on Xplorer),
    // so don't present it as the active payload unless the drone's params actually match it.
    // -1 = nothing chosen yet; the combo shows "Not configured" and no Apply button.
    property bool _selectedConfigured: _payloadConfigured(_selectedPayload)
    property int  _pendingPayload:     _selectedConfigured ? _selectedPayload : -1

    function _savePayloadSelection(index) {
        _flyViewSettings.payloadSelection.value = index
    }

    // True when CAM1_TYPE / SERIALx_BAUD match what Apply writes for payload `index`.
    // Params not loaded yet counts as configured so we don't flag a vehicle we can't read.
    function _payloadConfigured(index) {
        if (!_serialBaudFact || !_camTypeFact)
            return true

        const baud = _serialBaudFact.value
        const cam  = _camTypeFact.value

        switch (index) {
            case 0: return baud === _ilxBaud && cam === 5
            case 1: return baud === _vioBaud && cam === 6
            case 2: return cam === 0
            case 3: return baud === _mavCamBaud && cam === _mavCamType
        }
        return false
    }

    //--------------------------------------
    // Detect payload based on parameters
    //--------------------------------------
    function detectPayloadType() {
        if (!_serialBaudFact || !_camTypeFact)
            return 0

        const baud = _serialBaudFact.value
        const cam  = _camTypeFact.value

        if (baud === _ilxBaud && cam === 5) return 1   // ILX-LR1
        if (baud === _vioBaud && cam === 6) return 2   // VIO

        return 0
    }

    //--------------------------------------
    // Load parameters when vehicle changes
    //--------------------------------------
    function loadPayloadParams() {
        if (activeVehicle) {
            _serialBaudFact = controller.getParameterFact(-1, _serialParamName, false)
            _camTypeFact    = controller.getParameterFact(-1, "CAM1_TYPE", false)
            payloadType = detectPayloadType()
        } else {
            _serialBaudFact = undefined
            _camTypeFact    = undefined
            payloadType = 0
        }
    }

    Connections {
        target: QGroundControl.multiVehicleManager
        onActiveVehicleChanged: loadPayloadParams()
    }

    Component.onCompleted: loadPayloadParams()

    //--------------------------------------
    // Update payloadType if params change
    //--------------------------------------
    Connections {
        target: _serialBaudFact
        onValueChanged: payloadType = detectPayloadType()
    }

    Connections {
        target: _camTypeFact
        onValueChanged: payloadType = detectPayloadType()
    }

    //--------------------------------------
    // Apply payload configuration
    //--------------------------------------
    // rtspOverride: if non-empty, write this URL into videoSettings.rtspUrl2.
    // Empty means leave the user's existing RTSP setting alone (used for the
    // generic MAVLink Camera payload where we don't know the stream URL).
    function applyPayloadConfig(baud, camtype, rtspOverride) {

        if (!activeVehicle || !_serialBaudFact || !_camTypeFact) {
            mainWindow.showMessageDialog(
                "Payload Info",
                "Payload params not available"
            )
            return
        }

        var changed = false

        if (_serialBaudFact.value !== baud) {
            _serialBaudFact.value = baud
            changed = true
        }

        if (_camTypeFact.value !== camtype) {
            _camTypeFact.value = camtype
            changed = true
        }

        if (rtspOverride && rtspOverride.length > 0) {
            QGroundControl.settingsManager.videoSettings.rtspUrl2.value = rtspOverride
        }

        if (changed) {
            mainWindow.showMessageDialog("Payload Info", "Payload parameters changed, rebooting...")

            Qt.callLater(function() {
                activeVehicle.rebootVehicle()
                mainWindow.closeIndicatorDrawer()
            })
        } else {
            mainWindow.showMessageDialog("Payload Info", "Payload parameters need no change")
        }
    }


    //============================================================
    //                       PAGE CONTENT
    //============================================================
    contentComponent: Component {
        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight

            //----------------------------------------------------
            // PAYLOAD SELECTION (dropdown)
            //----------------------------------------------------
            SettingsGroupLayout {
                heading: "Payload Selection"
                visible: activeVehicle && !activeVehicle.armed

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight / 2

                    QGCComboBox {
                        id:                 payloadCombo
                        Layout.fillWidth:   true
                        model:              ["ILX-LR1", "VIO", "LiDAR", "MAVLink Camera"]
                        currentIndex:       _pendingPayload
                        alternateText:      _pendingPayload < 0 ? qsTr("Not configured - select payload") : ""
                        onActivated: function(index) { _pendingPayload = index }
                    }

                    // preferredWidth stops the unwrapped text from widening the whole drawer
                    QGCLabel {
                        Layout.fillWidth:       true
                        Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 25
                        visible:                !_selectedConfigured
                        wrapMode:               Text.WordWrap
                        font.pointSize:         ScreenTools.smallFontPointSize
                        color:                  qgcPal.colorOrange
                        text:                   qsTr("Drone is not configured for a payload.\n%1=%2, CAM1_TYPE=%3\nSelect the payload and tap Apply.")
                                                .arg(_serialParamName)
                                                .arg(_serialBaudFact ? _serialBaudFact.value : "?")
                                                .arg(_camTypeFact ? _camTypeFact.value : "?")
                    }

                    QGCButton {
                        text:               "Apply"
                        Layout.fillWidth:   true
                        visible:            _pendingPayload === 0 || _pendingPayload === 1   // ILX or VIO
                        onClicked: {
                            _savePayloadSelection(_pendingPayload)
                            if (_pendingPayload === 0)
                                applyPayloadConfig(_ilxBaud, 5, "rtsp://192.168.144.122/stream-1.sdp")
                            else
                                applyPayloadConfig(_vioBaud, 6, "rtsp://192.168.144.10:8554/vio")
                        }
                    }

                    QGCButton {
                        text:               "Apply"
                        Layout.fillWidth:   true
                        visible:            _pendingPayload === 2   // LiDAR
                        onClicked: {
                            _savePayloadSelection(_pendingPayload)
                            // Set CAM1_TYPE=0 to prevent camera manager from loading
                            if (_camTypeFact && _camTypeFact.value !== 0) {
                                _camTypeFact.value = 0
                                mainWindow.showMessageDialog("Payload Info",
                                    "Payload Changed to LiDAR. Rebooting...")
                                Qt.callLater(function() {
                                    activeVehicle.rebootVehicle()
                                    mainWindow.closeIndicatorDrawer()
                                })
                            } else {
                                mainWindow.showMessageDialog("Payload Info",
                                    "LiDAR payload already active")
                            }
                        }
                    }

                    // Generic MAVLink Camera (e.g. Sentera 65R): writes the user-configured
                    // CAM1_TYPE (5 or 6) and SERIALx_BAUD from FlyView settings. No RTSP
                    // override — the user manages the stream URL themselves in Video settings.
                    QGCButton {
                        text:               "Apply"
                        Layout.fillWidth:   true
                        visible:            _pendingPayload === 3
                        onClicked: {
                            _savePayloadSelection(_pendingPayload)
                            applyPayloadConfig(_mavCamBaud, _mavCamType, "")
                        }
                    }
                }
            }

            //----------------------------------------------------
            // LiDAR PATTERN SETTINGS
            //----------------------------------------------------
            SettingsGroupLayout {
                heading: "LiDAR Pattern Settings"
                visible: _selectedPayload === 2 && activeVehicle

                ColumnLayout {
                    id:                 lidarCol
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelHeight / 2

                    property var _ptrnPnum:    activeVehicle ? controller.getParameterFact(-1, "PTRN_PNUM",    false) : null
                    property var _ptrnRadius:  activeVehicle ? controller.getParameterFact(-1, "PTRN_RADIUS",  false) : null
                    property var _ptrnSpeed:   activeVehicle ? controller.getParameterFact(-1, "PTRN_SPEED",   false) : null
                    property var _ptrnSlspd:   activeVehicle ? controller.getParameterFact(-1, "PTRN_SLSPD",   false) : null
                    property var _ptrnShape:   activeVehicle ? controller.getParameterFact(-1, "PTRN_SHAPE",   false) : null
                    property var _ptrnTrigger: activeVehicle ? controller.getParameterFact(-1, "PTRN_TRIGGER", false) : null
                    property var _ptrnToAuto:  activeVehicle ? controller.getParameterFact(-1, "PTRN_TOAUTO",  false) : null
                    property var _ptrnMinAlt:  activeVehicle ? controller.getParameterFact(-1, "PTRN_MINALT",  false) : null
                    property bool _paramsOk:   !!_ptrnPnum

                    // Script refuses to start below PTRN_MINALT (above home, clamped 10-120 m); absent on older scripts
                    property var  _minAltValues: [10, 20, 30, 50]
                    property real _minAlt:       _ptrnMinAlt ? Math.max(10, Math.min(120, _ptrnMinAlt.value)) : 0
                    property bool _altOk:        !_ptrnMinAlt || (!!activeVehicle && activeVehicle.altitudeRelative.rawValue > _minAlt)
                    property bool _canStart:     !!activeVehicle && activeVehicle.armed && _altOk

                    // Xplorer: Guided mode flies the pattern under WPNAV_SPEED / WPNAV_ACCEL, so only
                    // speeds it can actually hold are offered. Mirrors apply_speed_limits() in
                    // copter-fly-figure-8-v2.lua (which caps anything set by other means) — keep in sync.
                    property bool _isXplorer:   QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
                    property var  _wpnavSpeed:  activeVehicle ? controller.getParameterFact(-1, "WPNAV_SPEED", false) : null
                    property var  _wpnavAccel:  activeVehicle ? controller.getParameterFact(-1, "WPNAV_ACCEL", false) : null
                    property bool _limitsOk:    _isXplorer && _paramsOk && !!_wpnavSpeed && !!_wpnavAccel && !!_ptrnRadius && !!_ptrnShape
                    property real _wpSpeed:     _limitsOk ? _wpnavSpeed.value / 100 : 0
                    property real _wpAccel:     _limitsOk && _wpnavAccel.value > 0 ? _wpnavAccel.value / 100 : 2.5
                    // U-turn is always a circle of PTRN_RADIUS; the lemniscate's tightest radius is 0.75 R
                    property real _rMin:        _limitsOk ? Math.max(25, Math.min(150, _ptrnRadius.value))
                                                            * ((_ptrnShape.value === 0 && _ptrnPnum.value !== 3) ? 0.75 : 1.0) : 0
                    property real _speedLimit:  Math.min(Math.sqrt(0.8 * _wpAccel * _rMin), _wpSpeed)

                    property var _speedValues:  _achievable([5, 6, 7], _speedLimit)
                    property var _slspdValues:  _achievable([5, 6, 7, 10], _wpSpeed)

                    function _achievable(options, limit) {
                        if (!_limitsOk) return options
                        var ok = options.filter(function(v) { return v <= limit })
                        return ok.length > 0 ? ok : [options[0]]
                    }

                    // Index of the stored value; an out-of-list value above the range shows as the
                    // fastest option, which is what the script will fly it as.
                    function _valueIndex(values, fact) {
                        if (!fact) return 0
                        var v = fact.value
                        for (var i = 0; i < values.length; i++)
                            if (Math.abs(v - values[i]) < 0.5) return i
                        return v > values[values.length - 1] ? values.length - 1 : 0
                    }

                    // After a radius / style / type change, pull stored speeds down to the fastest
                    // option still on offer so the settings never hold an unflyable value.
                    function _clampSpeeds() {
                        if (!_limitsOk) return
                        var maxSpd = _speedValues[_speedValues.length - 1]
                        var maxSl  = _slspdValues[_slspdValues.length - 1]
                        if (_ptrnSpeed && _ptrnSpeed.value > maxSpd) _ptrnSpeed.value = maxSpd
                        if (_ptrnSlspd && _ptrnSlspd.value > maxSl)  _ptrnSlspd.value = maxSl
                    }

                    QGCLabel {
                        text:               "PTRN_ parameters not found.\nEnsure the LiDAR pattern script is loaded."
                        visible:            !lidarCol._paramsOk
                        wrapMode:           Text.WordWrap
                        Layout.fillWidth:   true
                    }

                    // ── LiDAR Type ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Pattern Type"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              ["Figure-8 Continuous", "Figure-8 Decel", "U-Turn"]
                            currentIndex:       lidarCol._ptrnPnum ? lidarCol._ptrnPnum.value - 1 : 0
                            onActivated: function(index) {
                                if (lidarCol._ptrnPnum) lidarCol._ptrnPnum.value = index + 1
                                Qt.callLater(lidarCol._clampSpeeds)
                            }
                        }
                    }

                    // ── Pattern Radius ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Pattern Radius"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              ["25 m", "40 m", "50 m"]
                            property var _values: [25, 40, 50]
                            currentIndex: {
                                if (!lidarCol._ptrnRadius) return 2
                                var v = lidarCol._ptrnRadius.value
                                for (var i = 0; i < _values.length; i++)
                                    if (Math.abs(v - _values[i]) < 0.5) return i
                                return 2
                            }
                            onActivated: function(index) {
                                if (lidarCol._ptrnRadius) lidarCol._ptrnRadius.value = _values[index]
                                Qt.callLater(lidarCol._clampSpeeds)
                            }
                        }
                    }

                    // ── Pattern Speed ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Pattern Speed"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              lidarCol._speedValues.map(function(v) { return v + " m/s" })
                            currentIndex:       lidarCol._valueIndex(lidarCol._speedValues, lidarCol._ptrnSpeed)
                            onActivated: function(index) { if (lidarCol._ptrnSpeed) lidarCol._ptrnSpeed.value = lidarCol._speedValues[index] }
                        }
                    }

                    // ── Straight Line Speed ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Straight Line Speed"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              lidarCol._slspdValues.map(function(v) { return v + " m/s" })
                            currentIndex:       lidarCol._valueIndex(lidarCol._slspdValues, lidarCol._ptrnSlspd)
                            onActivated: function(index) { if (lidarCol._ptrnSlspd) lidarCol._ptrnSlspd.value = lidarCol._slspdValues[index] }
                        }
                    }

                    // ── Pattern Style ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Pattern Style"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              ["Figure-8", "Circular"]
                            currentIndex:       lidarCol._ptrnShape ? lidarCol._ptrnShape.value : 0
                            onActivated: function(index) {
                                if (lidarCol._ptrnShape) lidarCol._ptrnShape.value = index
                                Qt.callLater(lidarCol._clampSpeeds)
                            }
                        }
                    }

                    // ── Minimum Start Altitude (PTRN_MINALT) ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk && !!lidarCol._ptrnMinAlt

                        QGCLabel { text: "Minimum Start Altitude"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              lidarCol._minAltValues.map(function(v) { return v + " m" })
                            currentIndex:       lidarCol._valueIndex(lidarCol._minAltValues, lidarCol._ptrnMinAlt)
                            onActivated: function(index) { if (lidarCol._ptrnMinAlt) lidarCol._ptrnMinAlt.value = lidarCol._minAltValues[index] }
                        }
                    }

                    // ── Switch to Auto on Completion (PTRN_TOAUTO) ──
                    // Visible only if the Lua script exposes the param. The FactCheckBox's
                    // checked state reads back from the autopilot, so the box stays in sync
                    // with whatever the script currently has set.
                    FactCheckBox {
                        Layout.fillWidth:   true
                        visible:            lidarCol._paramsOk && !!lidarCol._ptrnToAuto
                        text:               "  Switch to Auto on Completion"
                        fact:               lidarCol._ptrnToAuto
                        checkedValue:       1
                        uncheckedValue:     0
                    }

                    // ── Start Pattern ──
                    Rectangle {
                        Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder
                        visible: lidarCol._paramsOk
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: ScreenTools.defaultFontPixelWidth / 2
                        visible: lidarCol._paramsOk

                        QGCButton {
                            text:               "Start Pattern 1"
                            Layout.fillWidth:   true
                            enabled:            lidarCol._canStart
                            onClicked: {
                                if (lidarCol._ptrnTrigger) lidarCol._ptrnTrigger.value = 1
                            }
                        }

                        QGCButton {
                            text:               "Start Pattern 2"
                            Layout.fillWidth:   true
                            visible:            lidarCol._ptrnPnum && lidarCol._ptrnPnum.value === 1
                            enabled:            lidarCol._canStart
                            onClicked: {
                                if (lidarCol._ptrnTrigger) lidarCol._ptrnTrigger.value = 2
                            }
                        }
                    }

                    QGCLabel {
                        text:               "Arm the vehicle to start a pattern"
                        visible:            lidarCol._paramsOk && activeVehicle && !activeVehicle.armed
                        font.pointSize:     ScreenTools.smallFontPointSize
                        color:              qgcPal.colorOrange
                        Layout.fillWidth:   true
                    }

                    QGCLabel {
                        text:               qsTr("Climb above %1 m to start a pattern").arg(lidarCol._minAlt.toFixed(0))
                        visible:            lidarCol._paramsOk && activeVehicle && activeVehicle.armed && !lidarCol._altOk
                        font.pointSize:     ScreenTools.smallFontPointSize
                        color:              qgcPal.colorOrange
                        Layout.fillWidth:   true
                    }
                }
            }
        }
    }
}
