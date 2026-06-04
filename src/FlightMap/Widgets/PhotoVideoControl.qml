/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtPositioning
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.ScreenTools
import QGroundControl.Controls
import QGroundControl.Palette
import QGroundControl.Vehicle
import QGroundControl.Controllers
import QGroundControl.FactSystem
import QGroundControl.FactControls

Rectangle {
    id:         _root
    width:      _panelWidth
    height:     mainColumn.implicitHeight + (_margins * 2)
    color:      Qt.rgba(qgcPal.window.r, qgcPal.window.g, qgcPal.window.b, 0.5)
    radius:     _margins
    visible:    _camera.capturesVideo || _camera.capturesPhotos

    property real   _margins:                   ScreenTools.defaultFontPixelHeight / 2
    property real   _smallMargins:              ScreenTools.defaultFontPixelWidth / 2
    property var    _activeVehicle:             globals.activeVehicle
    property var    _cameraManager:             _activeVehicle.cameraManager
    property var    _camera:                    _cameraManager.currentCameraInstance
    property bool   _cameraInPhotoMode:         _camera.cameraMode === MavlinkCameraControl.CAM_MODE_PHOTO
    property bool   _cameraInVideoMode:         !_cameraInPhotoMode
    property bool   _videoCaptureIdle:          _camera.videoCaptureStatus === MavlinkCameraControl.VIDEO_CAPTURE_STATUS_STOPPED
    property bool   _photoCaptureSingleIdle:    _camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IDLE
    property bool   _photoCaptureIntervalIdle:  _camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_INTERVAL_IDLE
    property bool   _photoCaptureIdle:          _photoCaptureSingleIdle || _photoCaptureIntervalIdle

    // payloadSelection is the authoritative source of truth (0=ILX, 1=VIO, 2=LiDAR, 3=MAVLink Camera).
    // Heartbeat/PARAM_EXT detection is only used as a fallback when the user hasn't yet
    // picked a payload (covered by payloadSelection==0 default) and the autopilot is
    // explicitly advertising a different payload. This prevents stale ILX detection
    // from sticking after the user has switched to VIO via the toolbar.
    property int    _payloadSelection:          QGroundControl.settingsManager.flyViewSettings.payloadSelection.value
    property bool   _isAirPixel:                _activeVehicle && _payloadSelection === 0
    property bool   _isVIO:                     _activeVehicle && _payloadSelection === 1
    property int    _apCompId:                  _activeVehicle ? _activeVehicle.airPixelComponentId : 0
    property bool   _showDefaultCamSettings:    false   // toggle for debugging

    // Sizing constants
    property real   _buttonHeight:              ScreenTools.defaultFontPixelHeight * 3
    property real   _captureButtonSize:         ScreenTools.defaultFontPixelHeight * 5
    property real   _panelWidth:                ScreenTools.defaultFontPixelWidth * 30

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    // Mapping-preset command queue. TAG-E is sensitive to rapid-fire PARAM_EXT sends
    // (apFormatCard spaces writes ~400ms apart) so we pace these out via a Timer.
    property var _presetQueue: []
    Timer {
        id:                 presetTimer
        interval:           300
        repeat:             true
        triggeredOnStart:   true
        onTriggered: {
            if (_presetQueue.length === 0) { stop(); return }
            var fn = _presetQueue.shift()
            fn()
        }
    }
    function _applyMappingDefaults() {
        if (!_activeVehicle) return
        _presetQueue = [
            function() { _activeVehicle.apSetExpMode(4) },                        // S (shutter priority)
            function() { _activeVehicle.apSetParamUint("TG_SHTTERSPD", 2000) },   // 1/2000
            function() { _activeVehicle.apSetAFMode(32772) },                     // AF-C
            function() { _activeVehicle.apSetParamUint("TG_ISO", 16777215) }      // Auto ISO
        ]
        presetTimer.restart()
    }

    // Force photo mode on startup
    property bool _photoModeForced: false
    onVisibleChanged: {
        if (visible && !_photoModeForced && _camera && _camera.hasModes && _cameraInVideoMode) {
            _camera.setCameraModePhoto()
            _photoModeForced = true
        }
    }
    Connections {
        target: _camera
        function onCameraModeChanged() {
            if (!_photoModeForced && _camera.hasModes && _cameraInVideoMode) {
                _camera.setCameraModePhoto()
                _photoModeForced = true
            }
        }
    }

    DeadMouseArea { anchors.fill: parent }

    ColumnLayout {
        id:                 mainColumn
        anchors.fill:       parent
        anchors.margins:    _margins
        spacing:            _margins

        // ── Camera Name ──
        QGCLabel {
            Layout.alignment:       Qt.AlignHCenter
            text:                   _camera.modelName
            font.pointSize:         ScreenTools.mediumFontPointSize
            font.bold:              true
            visible:                _cameraManager.cameras.length > 1
        }

        // ── Photo / Video Mode Selector ──
        RowLayout {
            Layout.fillWidth:   true
            spacing:            _margins
            visible:            _camera.hasModes

            // Photo mode button (first/left)
            Rectangle {
                Layout.fillWidth:       true
                Layout.preferredHeight: _buttonHeight
                radius:                 ScreenTools.defaultFontPixelWidth
                color:                  _cameraInPhotoMode ? qgcPal.brandingPurple : qgcPal.windowShadeLight
                border.color:           _cameraInPhotoMode ? Qt.lighter(qgcPal.brandingPurple, 1.3) : qgcPal.groupBorder
                border.width:           _cameraInPhotoMode ? 2 : 1

                ColumnLayout {
                    anchors.centerIn:   parent
                    spacing:            2

                    QGCColoredImage {
                        Layout.alignment:       Qt.AlignHCenter
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                        Layout.preferredWidth:  Layout.preferredHeight
                        source:                 "/qmlimages/camera_photo.svg"
                        fillMode:               Image.PreserveAspectFit
                        sourceSize.height:      Layout.preferredHeight
                        color:                  _cameraInPhotoMode ? "white" : qgcPal.text
                    }

                    QGCLabel {
                        Layout.alignment:   Qt.AlignHCenter
                        text:               qsTr("PHOTO")
                        font.pointSize:     ScreenTools.smallFontPointSize
                        font.bold:          true
                        color:              _cameraInPhotoMode ? "white" : qgcPal.text
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    enabled:        _cameraInVideoMode ? _videoCaptureIdle : true
                    onClicked:      _camera.setCameraModePhoto()
                    cursorShape:    Qt.PointingHandCursor
                }
            }

            // Video mode button (second/right)
            Rectangle {
                Layout.fillWidth:       true
                Layout.preferredHeight: _buttonHeight
                radius:                 ScreenTools.defaultFontPixelWidth
                color:                  _cameraInVideoMode ? qgcPal.brandingPurple : qgcPal.windowShadeLight
                border.color:           _cameraInVideoMode ? Qt.lighter(qgcPal.brandingPurple, 1.3) : qgcPal.groupBorder
                border.width:           _cameraInVideoMode ? 2 : 1

                ColumnLayout {
                    anchors.centerIn:   parent
                    spacing:            2

                    QGCColoredImage {
                        Layout.alignment:       Qt.AlignHCenter
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                        Layout.preferredWidth:  Layout.preferredHeight
                        source:                 "/qmlimages/camera_video.svg"
                        fillMode:               Image.PreserveAspectFit
                        sourceSize.height:      Layout.preferredHeight
                        color:                  _cameraInVideoMode ? "white" : qgcPal.text
                    }

                    QGCLabel {
                        Layout.alignment:   Qt.AlignHCenter
                        text:               qsTr("VIDEO")
                        font.pointSize:     ScreenTools.smallFontPointSize
                        font.bold:          true
                        color:              _cameraInVideoMode ? "white" : qgcPal.text
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    enabled:        _cameraInPhotoMode ? _photoCaptureIdle : true
                    onClicked:      _camera.setCameraModeVideo()
                    cursorShape:    Qt.PointingHandCursor
                }
            }
        }

        // ── Capture Button ──
        Rectangle {
            Layout.alignment:       Qt.AlignHCenter
            Layout.preferredWidth:  _captureButtonSize
            Layout.preferredHeight: _captureButtonSize
            color:                  "transparent"
            radius:                 width * 0.5
            border.color:           qgcPal.buttonText
            border.width:           3

            Rectangle {
                anchors.centerIn:   parent
                width:              parent.width * (_isShootingInCurrentMode ? 0.4 : 0.7)
                height:             width
                radius:             _isShootingInCurrentMode ? ScreenTools.defaultFontPixelWidth / 2 : width * 0.5
                color:              _isShootingInCurrentMode || _canShootInCurrentMode ? qgcPal.colorRed : qgcPal.colorGrey

                Behavior on width  { NumberAnimation { duration: 150 } }
                Behavior on radius { NumberAnimation { duration: 150 } }

                property bool _isShootingInPhotoMode:   _cameraInPhotoMode && _camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IN_PROGRESS
                property bool _isShootingInVideoMode:   (!_cameraInPhotoMode && _camera.videoCaptureStatus === MavlinkCameraControl.VIDEO_CAPTURE_STATUS_RUNNING)
                property bool _isShootingInCurrentMode: _cameraInPhotoMode ? _isShootingInPhotoMode : _isShootingInVideoMode
                property bool _isShootingInOtherMode:   _cameraInPhotoMode ? _isShootingInVideoMode : _isShootingInPhotoMode
                property bool _canShootInCurrentMode:   _isShootingInOtherMode ?
                                                            (_cameraInPhotoMode ? _camera.photosInVideoMode : _camera.videoInPhotoMode) :
                                                            true
            }

            MouseArea {
                anchors.fill:   parent
                onClicked:      toggleShooting()
                cursorShape:    Qt.PointingHandCursor

                function toggleShooting() {
                    if (_cameraInPhotoMode) {
                        if (_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_INTERVAL_IN_PROGRESS) {
                            _camera.stopTakePhoto()
                        } else if (_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IDLE || _camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_INTERVAL_IDLE) {
                            _camera.takePhoto()
                        }
                    } else {
                        _camera.toggleVideoRecording()
                    }
                }
            }
        }

        // ── Record Time / Capture Count ──
        Rectangle {
            Layout.alignment:       Qt.AlignHCenter
            color:                  !_videoCaptureIdle && !_photoCaptureIdle ? "transparent" : qgcPal.colorRed
            Layout.preferredWidth:  statusLabel.width + (_margins * 2)
            Layout.preferredHeight: statusLabel.height + (_smallMargins * 2)
            radius:                 ScreenTools.defaultFontPixelWidth / 2

            QGCLabel {
                id:                 statusLabel
                anchors.centerIn:   parent
                text: {
                    if (_cameraInVideoMode)
                        return _videoCaptureIdle ? "00:00:00" : _camera.recordTimeStr
                    if (!_activeVehicle) return "00000"
                    var count = _isAirPixel ? _activeVehicle.imageCount
                                            : _activeVehicle.cameraTriggerPoints.count
                    return ('00000' + count).slice(-5)
                }
                font.pointSize:     ScreenTools.largeFontPointSize
                font.bold:          true
            }
        }


        // ── Status Info (storage, battery) ──
        ColumnLayout {
            Layout.alignment:   Qt.AlignHCenter
            spacing:            0

            QGCLabel {
                Layout.alignment:   Qt.AlignHCenter
                text:               qsTr("Free Space: ") + _camera.storageFreeStr
                font.pointSize:     ScreenTools.defaultFontPointSize
                visible:            _camera.storageStatus === MavlinkCameraControl.STORAGE_READY
            }

            QGCLabel {
                Layout.alignment:   Qt.AlignHCenter
                text:               qsTr("Battery: ") + _camera.batteryRemainingStr
                font.pointSize:     ScreenTools.defaultFontPointSize
                visible:            _camera.batteryRemaining >= 0
            }
        }

        // ── Camera Tracking ──
        ColumnLayout {
            Layout.fillWidth:   true
            spacing:            _smallMargins
            visible:            _camera && _camera.hasTracking

            Rectangle {
                Layout.fillWidth:       true
                Layout.preferredHeight: _buttonHeight
                radius:                 ScreenTools.defaultFontPixelWidth
                color:                  _camera.trackingEnabled ? qgcPal.colorRed : qgcPal.windowShadeLight
                border.color:           _camera.trackingEnabled ? Qt.lighter(qgcPal.colorRed, 1.3) : qgcPal.groupBorder
                border.width:           1

                RowLayout {
                    anchors.centerIn:   parent
                    spacing:            _margins

                    QGCColoredImage {
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                        Layout.preferredWidth:  Layout.preferredHeight
                        source:                 "/qmlimages/TrackingIcon.svg"
                        fillMode:               Image.PreserveAspectFit
                        sourceSize.height:      Layout.preferredHeight
                        color:                  _camera.trackingEnabled ? "white" : qgcPal.text
                    }

                    QGCLabel {
                        text:       qsTr("Tracking")
                        font.bold:  true
                        color:      _camera.trackingEnabled ? "white" : qgcPal.text
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    cursorShape:    Qt.PointingHandCursor
                    onClicked: {
                        _camera.trackingEnabled = !_camera.trackingEnabled;
                        if (!_camera.trackingEnabled) {
                            _camera.stopTracking()
                        }
                    }
                }
            }
        }

        // ── Zoom Slider ──
        ColumnLayout {
            Layout.alignment:   Qt.AlignHCenter
            Layout.fillWidth:   true
            spacing:            _smallMargins
            visible:            false // _camera.hasZoom

            QGCLabel {
                Layout.alignment:   Qt.AlignHCenter
                text:               qsTr("Zoom")
                font.pointSize:     ScreenTools.smallFontPointSize
            }

            QGCSlider {
                Layout.fillWidth:   true
                to:                 100
                from:               0
                value:              _camera.zoomLevel
                live:               true
                onValueChanged:     _camera.zoomLevel = value
            }
        }

        // ── VIO EO Zoom (large buttons mirroring the popup +/-) ──
        RowLayout {
            Layout.fillWidth:   true
            spacing:            _smallMargins
            visible:            _isVIO

            QGCButton {
                text:                   "\u2212"
                Layout.fillWidth:       true
                Layout.preferredHeight: _buttonHeight
                pointSize:              ScreenTools.largeFontPointSize
                onClicked: {
                    if (!_activeVehicle) return
                    var cur = _activeVehicle.vioEOZoom
                    if (cur > 0) _activeVehicle.vioSetEOZoom(cur - 1)
                }
            }

            QGCLabel {
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 6
                horizontalAlignment:    Text.AlignHCenter
                verticalAlignment:      Text.AlignVCenter
                font.pointSize:         ScreenTools.mediumFontPointSize
                font.bold:              true
                text: {
                    var labels = ["1x","2x","4x","6x","8x","10x","12x","14x","16x","18x","20x","30x"]
                    var idx = _activeVehicle ? _activeVehicle.vioEOZoom : 0
                    return idx < labels.length ? labels[idx] : idx + "?"
                }
            }

            QGCButton {
                text:                   "+"
                Layout.fillWidth:       true
                Layout.preferredHeight: _buttonHeight
                pointSize:              ScreenTools.largeFontPointSize
                onClicked: {
                    if (!_activeVehicle) return
                    var cur = _activeVehicle.vioEOZoom
                    if (cur < 11) _activeVehicle.vioSetEOZoom(cur + 1)
                }
            }
        }

        // ── Settings Toggle Button ──
        Rectangle {
            id:                     settingsButton
            Layout.fillWidth:       true
            Layout.preferredHeight: _buttonHeight * 0.75
            radius:                 ScreenTools.defaultFontPixelWidth
            color:                  settingsPopup.visible ? qgcPal.windowShade : qgcPal.windowShadeLight
            border.color:           qgcPal.groupBorder
            border.width:           1

            RowLayout {
                anchors.centerIn:   parent
                spacing:            _margins

                QGCColoredImage {
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.2
                    Layout.preferredWidth:  Layout.preferredHeight
                    source:                 "/res/gear-black.svg"
                    mipmap:                 true
                    fillMode:               Image.PreserveAspectFit
                    sourceSize.height:      Layout.preferredHeight
                    color:                  qgcPal.text
                }

                QGCLabel {
                    text:       _isAirPixel ? qsTr("ILX Settings") : _isVIO ? qsTr("VIO Settings") : qsTr("Camera Settings")
                    font.bold:  true
                }
            }

            MouseArea {
                anchors.fill:   parent
                cursorShape:    Qt.PointingHandCursor
                onClicked: {
                    if (settingsPopup.visible)
                        settingsPopup.close()
                    else
                        settingsPopup.open()
                }
            }
        }
    }

    // ── Settings Pop-out Panel (appears to the left) ──
    Popup {
        id:             settingsPopup
        x:              -width - _margins
        y:              0
        width:          _panelWidth + (_margins * 2)
        padding:        _margins
        closePolicy:    Popup.CloseOnPressOutside | Popup.CloseOnEscape

        background: Rectangle {
            color:          Qt.rgba(qgcPal.window.r, qgcPal.window.g, qgcPal.window.b, 0.85)
            radius:         _margins
            border.color:   qgcPal.groupBorder
            border.width:   1

            layer.enabled:  true
        }

        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
            NumberAnimation { property: "x"; from: -settingsPopup.width; to: settingsPopup.x; duration: 200; easing.type: Easing.OutCubic }
        }
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150 }
        }

        ColumnLayout {
            id:                     settingsPanel
            anchors.fill:           parent
            spacing:                _margins

            property bool _multipleMavlinkCameras:          _cameraManager.cameras.count > 1
            property bool _multipleMavlinkCameraStreams:    _camera.streamLabels.length > 1
            property bool _cameraStorageSupported:          _camera.storageStatus !== MavlinkCameraControl.STORAGE_NOT_SUPPORTED
            property var  _videoSettings:                   QGroundControl.settingsManager.videoSettings

            // ── Header ──
            RowLayout {
                Layout.fillWidth:   true

                QGCLabel {
                    text:               _isAirPixel ? qsTr("ILX Settings") : _isVIO ? qsTr("VIO Settings") : qsTr("Camera Settings")
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    font.bold:          true
                    Layout.fillWidth:   true
                }

                Rectangle {
                    Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 1.5
                    Layout.preferredHeight: Layout.preferredWidth
                    radius:                 Layout.preferredWidth / 2
                    color:                  closeMouseArea.containsMouse ? qgcPal.windowShadeLight : "transparent"

                    QGCLabel {
                        anchors.centerIn:   parent
                        text:               "X"
                        font.bold:          true
                    }

                    MouseArea {
                        id:             closeMouseArea
                        anchors.fill:   parent
                        hoverEnabled:   true
                        cursorShape:    Qt.PointingHandCursor
                        onClicked:      settingsPopup.close()
                    }
                }
            }

            // ── Scrollable content ──
            Flickable {
                Layout.fillWidth:       true
                Layout.fillHeight:      true
                Layout.preferredHeight: settingsPanelContent.height
                Layout.maximumHeight:   _root.height * 1.5
                contentHeight:          settingsPanelContent.height
                clip:                   true
                flickableDirection:     Flickable.VerticalFlick
                boundsBehavior:        Flickable.StopAtBounds

                ColumnLayout {
                    id:     settingsPanelContent
                    width:  parent.width
                    spacing: _margins

                    // ════════════════════════════════════════════
                    //  AirPixel Camera Controls
                    //  Readback from PARAM_EXT, step via DO_DIGICAM_CONFIGURE
                    // ════════════════════════════════════════════
                    ColumnLayout {
                        id:                 apControls
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            _isAirPixel

                        property real _lblW:     ScreenTools.defaultFontPixelWidth * 8
                        property real _stepBtnW: ScreenTools.defaultFontPixelWidth * 5
                        property bool _showAdvanced: false

                        // AUTO_TILT_EN is a classic ArduPilot param on the autopilot. Reactive on
                        // factAdded so the checkbox appears as soon as the param arrives, even
                        // if it lands after this widget is instantiated. Explicitly requested via
                        // getMissingParameters so we don't depend on a component-wide initial sync.
                        //
                        // Geotag-on-landing is NOT a classic param — it's TG_LAND_DET on the TAG-E,
                        // delivered via PARAM_EXT_VALUE on component 100. Decoded in Vehicle.cc and
                        // exposed as _activeVehicle.apLandDetect (-1 unknown / 0 off / 1 on). No
                        // FactPanelController hookup needed for it.
                        FactPanelController { id: ilxController }
                        property int _factReload: 0
                        property Fact _autoTiltEnFact: (_factReload, true)
                            ? ilxController.getParameterFact(-1, "AUTO_TILT_EN", false)
                            : null
                        Connections {
                            target: _activeVehicle ? _activeVehicle.parameterManager : null
                            function onFactAdded(componentId, fact) {
                                if (fact && fact.name === "AUTO_TILT_EN") apControls._factReload++
                            }
                        }

                        function _requestIlxParams() {
                            if (!_activeVehicle) return
                            ilxController.getMissingParameters(["AUTO_TILT_EN"])
                        }
                        Component.onCompleted: _requestIlxParams()
                        Connections {
                            target: globals
                            function onActiveVehicleChanged() { apControls._requestIlxParams() }
                        }

                        // ── command helpers ──
                        function cfg(p1)              { _activeVehicle.sendCommand(_apCompId, 202, true, p1, 0, 0, 0, 0, 0, 0) }
                        function step(p1, p2, p3, p4) { _activeVehicle.sendCommand(_apCompId, 202, true, p1, p2, p3, p4, 0, 0, 0) }

                        // ── MAPPING PRESET ──
                        QGCButton {
                            Layout.fillWidth:   true
                            text:               qsTr("Apply Mapping Defaults")
                            enabled:            !presetTimer.running
                            onClicked:          _applyMappingDefaults()
                        }

                        // ── AUTO NADIR (Down) — toggles AUTO_TILT_EN ──
                        FactCheckBox {
                            Layout.fillWidth:   true
                            visible:            !!apControls._autoTiltEnFact
                            text:               "  " + qsTr("Auto NADIR (Down)")
                            fact:               apControls._autoTiltEnFact
                            checkedValue:       1
                            uncheckedValue:     0
                        }

                        // ── GEOTAG ON LANDING — toggles TG_LAND_DET via PARAM_EXT ──
                        // Default (checked / =1): TAG-E geotags the session automatically on landing.
                        // Unchecked (=0): operator-driven flow — show the manual Geotag Now and
                        // Reset Session buttons below. Geotagging itself always runs on the TAG-E;
                        // these controls only govern when tags are *written* and let the operator
                        // start a fresh session between flights.
                        //
                        // apSetLandDetect also sends FW_SAVE_CFG=1 after a short delay so the
                        // choice persists across TAG-E reboots — hence the "Setting is persistent"
                        // hint below the checkbox.
                        //
                        // Hidden until the TAG-E publishes the first PARAM_EXT_VALUE for TG_LAND_DET
                        // (apLandDetect stays -1 until then), so we don't render a stale state.
                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            0
                            visible:            _activeVehicle && _activeVehicle.apLandDetect >= 0

                            QGCCheckBox {
                                id:                 landDetectCheck
                                Layout.fillWidth:   true
                                text:               "  " + qsTr("Geotag on Landing")
                                checked:            _activeVehicle && _activeVehicle.apLandDetect === 1
                                onClicked:          if (_activeVehicle) _activeVehicle.apSetLandDetect(checked)
                            }
                            QGCLabel {
                                Layout.fillWidth:       true
                                Layout.leftMargin:      ScreenTools.defaultFontPixelWidth * 2
                                text:                   qsTr("(Setting is persistent)")
                                font.pointSize:         ScreenTools.smallFontPointSize
                                opacity:                0.7
                            }
                        }

                        // Manual geotag session controls — only relevant when landing-detect is off.
                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins
                            visible:            _activeVehicle && _activeVehicle.apLandDetect === 0

                            QGCButton {
                                Layout.fillWidth:   true
                                text:               qsTr("Geotag Now")
                                onClicked:          if (_activeVehicle) _activeVehicle.apGeotagNow()
                            }

                            QGCButton {
                                Layout.fillWidth:   true
                                text:               qsTr("Reset Geotagging Session")
                                onClicked:          resetSessionConfirm.open()

                                MessageDialog {
                                    id:         resetSessionConfirm
                                    title:      qsTr("Reset Geotagging Session")
                                    text:       qsTr("This drops all logged geotag data and starts a fresh session on the TAG-E. Continue?")
                                    buttons:    MessageDialog.Yes | MessageDialog.No
                                    onButtonClicked: function (button) {
                                        if (button === MessageDialog.Yes) apControls.cfg(260)
                                        resetSessionConfirm.close()
                                    }
                                }
                            }
                        }

                        // ── ADVANCED TOGGLE ──
                        RowLayout {
                            Layout.fillWidth: true
                            QGCLabel { text: qsTr("Show Advanced"); Layout.fillWidth: true }
                            QGCSwitch {
                                checked:    apControls._showAdvanced
                                onClicked:  apControls._showAdvanced = checked
                            }
                        }

                        // ── EXPOSURE MODE ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder; visible: apControls._showAdvanced }
                        QGCLabel { text: qsTr("EXPOSURE MODE"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true; visible: apControls._showAdvanced }

                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCButton {
                                text: "M"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apExpMode === 1 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetExpMode(1)
                            }
                            QGCButton {
                                text: "P"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apExpMode === 2 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetExpMode(2)
                            }
                            QGCButton {
                                text: "A"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apExpMode === 3 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetExpMode(3)
                            }
                            QGCButton {
                                text: "S"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apExpMode === 4 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetExpMode(4)
                            }
                        }

                        // ── EXPOSURE VALUES ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder; visible: apControls._showAdvanced }
                        QGCLabel { text: qsTr("EXPOSURE"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true; visible: apControls._showAdvanced }

                        // Shutter Speed
                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCLabel { text: qsTr("Speed"); Layout.preferredWidth: apControls._lblW }
                            QGCLabel {
                                text: _activeVehicle && _activeVehicle.apShutterSpeed > 0
                                      ? "1/" + _activeVehicle.apShutterSpeed : "--"
                                Layout.fillWidth: true; horizontalAlignment: Text.AlignRight
                            }
                            QGCButton { text: "\u2212"; Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,0,0,99) }
                            QGCButton { text: "+";      Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,0,0,101) }
                        }

                        // Aperture
                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCLabel { text: qsTr("Aperture"); Layout.preferredWidth: apControls._lblW }
                            QGCLabel {
                                text: _activeVehicle && _activeVehicle.apAperture > 0
                                      ? "f/" + _activeVehicle.apAperture.toFixed(1) : "--"
                                Layout.fillWidth: true; horizontalAlignment: Text.AlignRight
                            }
                            QGCButton { text: "\u2212"; Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,99,0,0) }
                            QGCButton { text: "+";      Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,101,0,0) }
                        }

                        // ISO (Auto + step)
                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCLabel { text: qsTr("ISO"); Layout.preferredWidth: apControls._lblW }
                            QGCLabel {
                                text: {
                                    if (!_activeVehicle) return "--"
                                    if (_activeVehicle.apISOAuto) return "Auto"
                                    return _activeVehicle.apCameraISO > 0
                                           ? _activeVehicle.apCameraISO.toString() : "--"
                                }
                                Layout.fillWidth: true; horizontalAlignment: Text.AlignRight
                            }
                            QGCButton { text: "A";      Layout.preferredWidth: apControls._stepBtnW
                                        onClicked: _activeVehicle.apSetParamUint("TG_ISO", 16777215) }
                            QGCButton { text: "\u2212"; Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,0,99,0) }
                            QGCButton { text: "+";      Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.step(0,0,101,0) }
                        }

                        // Exposure Compensation
                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCLabel { text: qsTr("Exp Comp"); Layout.preferredWidth: apControls._lblW }
                            QGCLabel {
                                text: {
                                    if (!_activeVehicle) return "--"
                                    var v = _activeVehicle.apExpCorr
                                    return (v > 0 ? "+" : "") + v.toFixed(1)
                                }
                                Layout.fillWidth: true; horizontalAlignment: Text.AlignRight
                            }
                            QGCButton { text: "\u2212"; Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.cfg(123) }
                            QGCButton { text: "+";      Layout.preferredWidth: apControls._stepBtnW; onClicked: apControls.cfg(122) }
                        }

                        // ── FOCUS MODE ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder; visible: apControls._showAdvanced }
                        QGCLabel { text: qsTr("FOCUS"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true; visible: apControls._showAdvanced }

                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCButton {
                                text: "AF-S"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apAFMode === 2 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetAFMode(2)
                            }
                            QGCButton {
                                text: "AF-C"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apAFMode === 32772 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetAFMode(32772)
                            }
                        }

                        // ── IMAGE RESOLUTION ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder; visible: apControls._showAdvanced }
                        QGCLabel { text: qsTr("IMAGE SIZE"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true; visible: apControls._showAdvanced }

                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            visible: apControls._showAdvanced
                            QGCButton {
                                text: "L"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apImgRes === 1 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetImgRes(1)
                            }
                            QGCButton {
                                text: "M"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apImgRes === 2 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetImgRes(2)
                            }
                            QGCButton {
                                text: "S"; Layout.fillWidth: true
                                backgroundColor: _activeVehicle && _activeVehicle.apImgRes === 3 ? "green" : "gray"
                                onClicked: _activeVehicle.apSetImgRes(3)
                            }
                        }

                        // ── FORMAT ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder; visible: apControls._showAdvanced }

                        QGCButton {
                            text:               qsTr("Format SD Card")
                            Layout.fillWidth:   true
                            visible:            apControls._showAdvanced
                            onClicked:          apFormatConfirm.open()

                            MessageDialog {
                                id:         apFormatConfirm
                                title:      qsTr("Format Camera Storage")
                                text:       qsTr("This will erase all files on the camera SD card. Continue?")
                                buttons:    MessageDialog.Yes | MessageDialog.No
                                onButtonClicked: function (button, role) {
                                    if (button === MessageDialog.Yes) {
                                        _activeVehicle.apFormatCard()
                                    }
                                    apFormatConfirm.close()
                                }
                            }
                        }

                        // ── EXPORT PROCESSING LOG ──
                        // Tells the TAG-E to write its processing log to the microSD card in its
                        // own slot. Non-destructive; useful for diagnostics when geotagging fails.
                        // Per the dev, DO_DIGICAM_CONFIGURE p1=1100 is the documented command, but
                        // current TAG-E firmware may not implement it yet — verify with the device
                        // logs / SD card output, and check console for the cfg send line below.
                        QGCButton {
                            text:               qsTr("Export Processing Log to SD")
                            Layout.fillWidth:   true
                            visible:            apControls._showAdvanced
                            onClicked: {
                                console.log("[AP_EXPORT_LOG] sending DO_DIGICAM_CONFIGURE p1=1100 to compId", _apCompId)
                                apControls.cfg(1100)
                            }
                        }
                    }

                    // ════════════════════════════════════════════
                    //  VIO Camera Controls
                    // ════════════════════════════════════════════
                    ColumnLayout {
                        id:                 vioControls
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            _isVIO && !_isAirPixel

                        property real _btnW: ScreenTools.defaultFontPixelWidth * 5

                        // ── CAMERA SOURCE ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder }
                        QGCLabel { text: qsTr("CAMERA SOURCE"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true }

                        QGCComboBox {
                            id:                 vioSourceCombo
                            Layout.fillWidth:   true
                            // Index in `model` ↔ vioCameraSource value mapping
                            property var _values: [0, 1, 2, 3, 6]
                            model: ["EO + IR", "EO", "IR", "IR + EO", "Side by Side"]

                            currentIndex: {
                                if (!_activeVehicle) return 0
                                var idx = _values.indexOf(_activeVehicle.vioCameraSource)
                                return idx >= 0 ? idx : 0
                            }
                            onActivated: function(index) {
                                if (_activeVehicle && index >= 0 && index < _values.length) {
                                    _activeVehicle.vioSetSource(_values[index])
                                }
                            }
                        }

                        // ── IR PALETTE ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder }
                        QGCLabel { text: qsTr("IR PALETTE"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true }

                        QGCComboBox {
                            Layout.fillWidth: true
                            model: ["WhiteHot", "BlackHot", "Rainbow", "RainbowHC", "Ironbow", "Lava", "Arctic", "Globow", "Gradedfire", "Hottest"]
                            currentIndex: _activeVehicle ? _activeVehicle.vioIRPalette : 0
                            onActivated: function(index) { _activeVehicle.vioSetIRPalette(index) }
                        }

                        // ── IR ZOOM ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder }
                        QGCLabel { text: qsTr("IR ZOOM"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true }

                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            Repeater {
                                model: [
                                    { label: "1x", val: 0 },
                                    { label: "2x", val: 1 },
                                    { label: "4x", val: 3 },
                                    { label: "8x", val: 7 }
                                ]
                                QGCButton {
                                    text: modelData.label; Layout.fillWidth: true
                                    backgroundColor: _activeVehicle && _activeVehicle.vioIRZoom === modelData.val ? "green" : "gray"
                                    onClicked: _activeVehicle.vioSetIRZoom(modelData.val)
                                }
                            }
                        }

                        // ── EO ZOOM ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder }
                        QGCLabel { text: qsTr("EO ZOOM"); font.pointSize: ScreenTools.smallFontPointSize; font.bold: true }

                        RowLayout {
                            Layout.fillWidth: true; spacing: _smallMargins
                            QGCLabel {
                                text: {
                                    // Map SR zoom level index to display multiplier
                                    var labels = ["1x","2x","4x","6x","8x","10x","12x","14x","16x","18x","20x","30x"]
                                    var idx = _activeVehicle ? _activeVehicle.vioEOZoom : 0
                                    return idx < labels.length ? labels[idx] : idx + "?"
                                }
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                font.bold: true
                            }
                            QGCButton {
                                text: "\u2212"; Layout.preferredWidth: vioControls._btnW
                                onClicked: {
                                    var cur = _activeVehicle.vioEOZoom
                                    if (cur > 0) _activeVehicle.vioSetEOZoom(cur - 1)
                                }
                            }
                            QGCButton {
                                text: "+"; Layout.preferredWidth: vioControls._btnW
                                onClicked: {
                                    var cur = _activeVehicle.vioEOZoom
                                    if (cur < 11) _activeVehicle.vioSetEOZoom(cur + 1)
                                }
                            }
                        }

                        // ── Show/hide default camera settings ──
                        Rectangle { Layout.fillWidth: true; height: 1; color: qgcPal.groupBorder }
                        QGCButton {
                            Layout.fillWidth: true
                            text: _showDefaultCamSettings ? qsTr("Hide Default Settings") : qsTr("Show Default Settings")
                            onClicked: _showDefaultCamSettings = !_showDefaultCamSettings
                        }
                    }

                    // ── Section: Camera Selection ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            (!_isAirPixel && !_isVIO || _showDefaultCamSettings) && (settingsPanel._multipleMavlinkCameras || settingsPanel._multipleMavlinkCameraStreams)

                        Rectangle {
                            Layout.fillWidth:       true
                            Layout.preferredHeight: 1
                            color:                  qgcPal.groupBorder
                        }

                        QGCLabel {
                            text:               qsTr("CAMERA")
                            font.pointSize:     ScreenTools.smallFontPointSize
                            font.bold:          true
                            color:              qgcPal.text
                        }

                        // Camera selector
                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins
                            visible:            settingsPanel._multipleMavlinkCameras

                            QGCLabel {
                                text:           qsTr("Camera")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }

                            QGCComboBox {
                                Layout.fillWidth:   true
                                sizeToContents:     true
                                model:              _cameraManager.cameraLabels
                                currentIndex:       _cameraManager.currentCamera
                                onActivated:        (index) => { _cameraManager.currentCamera = index }
                            }
                        }

                        // Video stream selector
                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins
                            visible:            settingsPanel._multipleMavlinkCameraStreams

                            QGCLabel {
                                text:           qsTr("Video Stream")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }

                            QGCComboBox {
                                Layout.fillWidth:   true
                                sizeToContents:     true
                                model:              _camera.streamLabels
                                currentIndex:       _camera.currentStream
                                onActivated:        (index) => { _camera.currentStream = index }
                            }
                        }
                    }

                    // ── Section: Exposure / Camera Settings (from activeSettings) ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            (!_isAirPixel && !_isVIO || _showDefaultCamSettings) && _camera.activeSettings.length > 0

                        Rectangle {
                            Layout.fillWidth:       true
                            Layout.preferredHeight: 1
                            color:                  qgcPal.groupBorder
                        }

                        QGCLabel {
                            text:               qsTr("EXPOSURE")
                            font.pointSize:     ScreenTools.smallFontPointSize
                            font.bold:          true
                            color:              qgcPal.text
                        }

                        Repeater {
                            model: _camera.activeSettings

                            ColumnLayout {
                                Layout.fillWidth:   true
                                spacing:            _smallMargins

                                property var    _fact:      _camera.getFact(modelData)
                                property bool   _isBool:    _fact.typeIsBool
                                property bool   _isCombo:   !_isBool && _fact.enumStrings.length > 0
                                property bool   _isSlider:  _fact && !isNaN(_fact.increment)
                                property bool   _isEdit:    !_isBool && !_isSlider && _fact.enumStrings.length < 1

                                QGCLabel {
                                    text:           _fact.shortDescription
                                    font.pointSize: ScreenTools.smallFontPointSize
                                }

                                FactComboBox {
                                    Layout.fillWidth:   true
                                    sizeToContents:     true
                                    fact:               _fact
                                    indexModel:         false
                                    visible:            _isCombo
                                }

                                FactTextField {
                                    Layout.fillWidth:   true
                                    fact:               _fact
                                    visible:            _isEdit
                                }

                                QGCSlider {
                                    Layout.fillWidth:           true
                                    to:                         _fact.max
                                    from:                       _fact.min
                                    stepSize:                   _fact.increment
                                    visible:                    _isSlider
                                    live:                       false
                                    property bool initialized:  false

                                    onValueChanged: {
                                        if (!initialized) {
                                            return
                                        }
                                        _fact.value = value
                                    }

                                    Component.onCompleted: {
                                        value = _fact.value
                                        initialized = true
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth:   true
                                    visible:            _isBool

                                    Item { Layout.fillWidth: true }

                                    QGCSwitch {
                                        checked:    _fact ? _fact.value : false
                                        onClicked:  _fact.value = checked ? 1 : 0
                                    }
                                }
                            }
                        }
                    }

                    // ── Section: Thermal ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            (!_isAirPixel && !_isVIO || _showDefaultCamSettings) && _camera.thermalStreamInstance

                        Rectangle {
                            Layout.fillWidth:       true
                            Layout.preferredHeight: 1
                            color:                  qgcPal.groupBorder
                        }

                        QGCLabel {
                            text:               qsTr("THERMAL")
                            font.pointSize:     ScreenTools.smallFontPointSize
                            font.bold:          true
                            color:              qgcPal.text
                        }

                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins

                            QGCLabel {
                                text:           qsTr("View Mode")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }

                            QGCComboBox {
                                Layout.fillWidth:   true
                                sizeToContents:     true
                                model:              [ qsTr("Off"), qsTr("Blend"), qsTr("Full"), qsTr("Picture In Picture") ]
                                currentIndex:       _camera.thermalMode
                                onActivated:        (index) => { _camera.thermalMode = index }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins
                            visible:            _camera.thermalMode === MavlinkCameraControl.THERMAL_BLEND

                            QGCLabel {
                                text:           qsTr("Blend Opacity")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }

                            QGCSlider {
                                Layout.fillWidth:   true
                                to:                 100
                                from:               0
                                value:              _camera.thermalOpacity
                                live:               true
                                onValueChanged:     _camera.thermalOpacity = value
                            }
                        }
                    }

                    // ── Section: Display ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            (!_isAirPixel && !_isVIO || _showDefaultCamSettings) && _camera.hasVideoStream

                        Rectangle {
                            Layout.fillWidth:       true
                            Layout.preferredHeight: 1
                            color:                  qgcPal.groupBorder
                        }

                        QGCLabel {
                            text:               qsTr("DISPLAY")
                            font.pointSize:     ScreenTools.smallFontPointSize
                            font.bold:          true
                            color:              qgcPal.text
                        }

                        RowLayout {
                            Layout.fillWidth:   true

                            QGCLabel {
                                text:               qsTr("Grid Lines")
                                font.pointSize:     ScreenTools.smallFontPointSize
                                Layout.fillWidth:   true
                            }

                            QGCSwitch {
                                checked:    settingsPanel._videoSettings.gridLines.rawValue
                                onClicked:  settingsPanel._videoSettings.gridLines.rawValue = checked ? 1 : 0
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth:   true
                            spacing:            _smallMargins

                            QGCLabel {
                                text:           qsTr("Video Fit")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }

                            FactComboBox {
                                Layout.fillWidth:   true
                                sizeToContents:     true
                                fact:               settingsPanel._videoSettings.videoFit
                                indexModel:         false
                            }
                        }
                    }

                    // ── Section: Storage & Reset ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            !_isAirPixel && !_isVIO || _showDefaultCamSettings

                        Rectangle {
                            Layout.fillWidth:       true
                            Layout.preferredHeight: 1
                            color:                  qgcPal.groupBorder
                        }

                        QGCLabel {
                            text:               qsTr("STORAGE")
                            font.pointSize:     ScreenTools.smallFontPointSize
                            font.bold:          true
                            color:              qgcPal.text
                        }

                        RowLayout {
                            Layout.fillWidth:   true
                            spacing:            _margins

                            QGCButton {
                                Layout.fillWidth:   true
                                text:               qsTr("Reset Defaults")
                                onClicked:          resetPrompt.open()

                                MessageDialog {
                                    id:                 resetPrompt
                                    title:              qsTr("Reset Camera to Factory Settings")
                                    text:               qsTr("Confirm resetting all settings?")
                                    buttons:            MessageDialog.Yes | MessageDialog.No

                                    onButtonClicked: function (button, role) {
                                        switch (button) {
                                        case MessageDialog.Yes:
                                            _camera.resetSettings()
                                            resetPrompt.close()
                                            break;
                                        case MessageDialog.No:
                                            resetPrompt.close()
                                            break;
                                        }
                                    }
                                }
                            }

                            QGCButton {
                                Layout.fillWidth:   true
                                text:               qsTr("Format")
                                visible:            settingsPanel._cameraStorageSupported
                                onClicked:          formatPrompt.open()

                                MessageDialog {
                                    id:                 formatPrompt
                                    title:              qsTr("Format Camera Storage")
                                    text:               qsTr("Confirm erasing all files?")
                                    buttons:            MessageDialog.Yes | MessageDialog.No

                                    onButtonClicked: function (button, role) {
                                        switch (button) {
                                        case MessageDialog.Yes:
                                            _camera.formatCard()
                                            formatPrompt.close()
                                            break;
                                        case MessageDialog.No:
                                            formatPrompt.close()
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
