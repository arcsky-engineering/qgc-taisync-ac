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

    // Sizing constants
    property real   _buttonHeight:              ScreenTools.defaultFontPixelHeight * 3
    property real   _captureButtonSize:         ScreenTools.defaultFontPixelHeight * 5
    property real   _panelWidth:                ScreenTools.defaultFontPixelWidth * 30

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

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

            // Video mode button
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

            // Photo mode button
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
                text:               _cameraInVideoMode
                                        ? (_videoCaptureIdle ? "00:00:00" : _camera.recordTimeStr)
                                        : (_activeVehicle ? ('00000' + _activeVehicle.cameraTriggerPoints.count).slice(-5) : "00000")
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
                    text:       qsTr("Camera Settings")
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
                    text:               qsTr("Camera Settings")
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

                    // ── Section: Camera Selection ──
                    ColumnLayout {
                        Layout.fillWidth:   true
                        spacing:            _smallMargins
                        visible:            settingsPanel._multipleMavlinkCameras || settingsPanel._multipleMavlinkCameraStreams

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
                        visible:            _camera.activeSettings.length > 0

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
                        visible:            _camera.thermalStreamInstance

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
                        visible:            _camera.hasVideoStream

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
