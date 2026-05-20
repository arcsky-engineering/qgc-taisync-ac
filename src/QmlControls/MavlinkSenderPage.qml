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

    //--------------------------------------
    // Payload-related parameters
    //--------------------------------------
    property Fact _serialBaudFact
    property Fact _camTypeFact
    property int payloadType: 0   // 0=unknown, 1=ILX, 2=VIO

    // Payload selection: 0=ILX-LR1, 1=VIO, 2=LiDAR  (persisted across restarts)
    // _selectedPayload reflects the authoritative persisted value — all other UI depends on this.
    // _pendingPayload is the combo-box in-flight choice; only persisted when user hits Apply.
    // Closing the drawer without applying discards _pendingPayload (property is re-initialised on reopen).
    property int _selectedPayload: _flyViewSettings.payloadSelection.value
    property int _pendingPayload:  _selectedPayload

    function _savePayloadSelection(index) {
        _flyViewSettings.payloadSelection.value = index
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
    function applyPayloadConfig(baud, camtype) {

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

        // Update RTSP automatically based on payload type.
        // VIO and ILX (Sony) each have their own dedicated stream URL.
        if (baud === _vioBaud)
            QGroundControl.settingsManager.videoSettings.rtspUrl2.value =
                "rtsp://192.168.144.10:8554/vio"
        else if (baud === _ilxBaud)
            QGroundControl.settingsManager.videoSettings.rtspUrl2.value =
                "rtsp://192.168.144.122/stream-1.sdp"

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
                        model:              ["ILX-LR1", "VIO", "LiDAR"]
                        currentIndex:       _pendingPayload
                        onActivated: function(index) { _pendingPayload = index }
                    }

                    QGCButton {
                        text:               "Apply"
                        Layout.fillWidth:   true
                        visible:            _pendingPayload < 2   // ILX or VIO
                        onClicked: {
                            _savePayloadSelection(_pendingPayload)
                            if (_pendingPayload === 0)
                                applyPayloadConfig(_ilxBaud, 5)
                            else
                                applyPayloadConfig(_vioBaud, 6)
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
                    property bool _paramsOk:   !!_ptrnPnum

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
                            onActivated: function(index) { if (lidarCol._ptrnPnum) lidarCol._ptrnPnum.value = index + 1 }
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
                            onActivated: function(index) { if (lidarCol._ptrnRadius) lidarCol._ptrnRadius.value = _values[index] }
                        }
                    }

                    // ── Pattern Speed ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Pattern Speed"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              ["5 m/s", "6 m/s", "7 m/s"]
                            property var _values: [5, 6, 7]
                            currentIndex: {
                                if (!lidarCol._ptrnSpeed) return 0
                                var v = lidarCol._ptrnSpeed.value
                                for (var i = 0; i < _values.length; i++)
                                    if (Math.abs(v - _values[i]) < 0.5) return i
                                return 0
                            }
                            onActivated: function(index) { if (lidarCol._ptrnSpeed) lidarCol._ptrnSpeed.value = _values[index] }
                        }
                    }

                    // ── Straight Line Speed ──
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2; visible: lidarCol._paramsOk

                        QGCLabel { text: "Straight Line Speed"; font.pointSize: ScreenTools.smallFontPointSize }

                        QGCComboBox {
                            Layout.fillWidth:   true
                            model:              ["5 m/s", "6 m/s", "7 m/s", "10 m/s"]
                            property var _values: [5, 6, 7, 10]
                            currentIndex: {
                                if (!lidarCol._ptrnSlspd) return 0
                                var v = lidarCol._ptrnSlspd.value
                                for (var i = 0; i < _values.length; i++)
                                    if (Math.abs(v - _values[i]) < 0.5) return i
                                return 0
                            }
                            onActivated: function(index) { if (lidarCol._ptrnSlspd) lidarCol._ptrnSlspd.value = _values[index] }
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
                            onActivated: function(index) { if (lidarCol._ptrnShape) lidarCol._ptrnShape.value = index }
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
                            enabled:            activeVehicle && activeVehicle.armed
                            onClicked: {
                                if (lidarCol._ptrnTrigger) lidarCol._ptrnTrigger.value = 1
                            }
                        }

                        QGCButton {
                            text:               "Start Pattern 2"
                            Layout.fillWidth:   true
                            visible:            lidarCol._ptrnPnum && lidarCol._ptrnPnum.value === 1
                            enabled:            activeVehicle && activeVehicle.armed
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
                }
            }
        }
    }
}
