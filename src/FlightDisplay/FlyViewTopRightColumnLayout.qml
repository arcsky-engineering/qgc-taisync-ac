/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightDisplay
import QGroundControl.FlightMap
import QGroundControl.Palette
import QGroundControl.ScreenTools
import QGroundControl.Vehicle

ColumnLayout {
    width: _rightPanelWidth

    // _userExpanded persists the operator's chosen state. _photoVideoExpanded
    // is the *effective* state: forced collapsed while the FPV stream is
    // selected and the payload is ILX/VIO (record/capture buttons belong to
    // the payload camera, not the FPV feed — so we hide and lock the panel
    // to avoid the misleading association). Switching back to the CAM stream
    // auto-restores the expanded state.
    property bool   _userExpanded:          QGroundControl.loadBoolGlobalSetting(_expandedKey, true)
    property string _expandedKey:           "PhotoVideoControlExpanded"

    readonly property bool fpvLocked:      QGroundControl.videoManager.currentStream === "1"
                                            && (QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 0
                                                || QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 1)

    readonly property bool _photoVideoExpanded: !fpvLocked && _userExpanded

    // Auto-expand on FPV → CAM transition so operators see the camera controls
    // re-appear without an extra click.
    onFpvLockedChanged: {
        if (!fpvLocked && !_userExpanded) {
            _userExpanded = true
            QGroundControl.saveBoolGlobalSetting(_expandedKey, true)
        }
    }

    function _setExpanded(expanded) {
        if (fpvLocked) return   // locked while FPV stream is selected
        _userExpanded = expanded
        QGroundControl.saveBoolGlobalSetting(_expandedKey, expanded)
    }

    // Camera state for the locked-status readout under the expand arrow.
    property var  _activeVehicleForStatus:  globals.activeVehicle
    property var  _cameraForStatus:         _activeVehicleForStatus ? _activeVehicleForStatus.cameraManager.currentCameraInstance : null
    property bool _cameraInPhotoModeForStatus: _cameraForStatus
                                               && _cameraForStatus.cameraMode === MavlinkCameraControl.CAM_MODE_PHOTO
    property bool _isAirPixelForStatus:     QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 0

    TerrainProgress {
        Layout.alignment:       Qt.AlignTop
        Layout.preferredWidth:  _rightPanelWidth
    }

    // LiDAR Control (shown in place of PhotoVideoControl when LiDAR payload selected).
    // Wrapped in a Loader so FactPanelController inside LidarControl is only constructed once a
    // real vehicle is connected — otherwise it latches onto the offline-editing vehicle and
    // PTRN_ params never resolve.
    //
    // Gated on showPayloadIndicator so the LiDAR Calibration panel and the toolbar
    // payload indicator go hand-in-hand: if the operator turns off the payload
    // indicator (no payload management), LiDAR calibration also disappears.
    Loader {
        Layout.alignment:   Qt.AlignTop | Qt.AlignRight
        active:             globals.activeVehicle
                            && QGroundControl.settingsManager.flyViewSettings.showPayloadIndicator.value
                            && QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 2
        visible:            active
        sourceComponent:    lidarControlComponent

        Component {
            id: lidarControlComponent
            LidarControl { }
        }
    }

    // We use a Loader to load the photoVideoControlComponent only when the active vehicle is not null
    // This make it easier to implement PhotoVideoControl without having to check for the mavlink camera
    // to be null all over the place
    Item {
        Layout.alignment:       Qt.AlignTop | Qt.AlignRight
        Layout.preferredWidth:  _photoVideoExpanded
                                    ? _rightPanelWidth
                                    : (fpvLocked
                                        ? Math.max(expandButton.width, lockedStatusLabel.width)
                                        : expandButton.width)
        Layout.preferredHeight: _photoVideoExpanded
                                    ? photoVideoControlLoader.height
                                    : (fpvLocked
                                        ? expandButton.height + lockedStatusLabel.height + ScreenTools.defaultFontPixelHeight * 0.3
                                        : expandButton.height)
        visible:                QGroundControl.videoManager.hasVideo && (globals.activeVehicle ? true : false)
                                && QGroundControl.settingsManager.flyViewSettings.showSimpleCameraControl.value
                                && QGroundControl.settingsManager.flyViewSettings.payloadSelection.value !== 2  // hide for LiDAR
        clip:                   true

        Behavior on Layout.preferredWidth  { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
        Behavior on Layout.preferredHeight { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

        // Expanded: full PhotoVideoControl panel
        Loader {
            id:                 photoVideoControlLoader
            anchors.right:      parent.right
            sourceComponent:    globals.activeVehicle ? photoVideoControlComponent : undefined
            opacity:            _photoVideoExpanded ? 1.0 : 0.0
            // Opacity hides pixels but not input: when collapsed, the panel
            // still extends past the visible expand-arrow area and would
            // catch clicks (e.g. the PHOTO/VIDEO mode toggle right under the
            // arrow, which is what was causing the camera to flip modes when
            // tapping the locked arrow). `enabled` propagates to children, so
            // this disables every MouseArea inside PhotoVideoControl while
            // collapsed without touching the panel's own code.
            enabled:            _photoVideoExpanded

            Behavior on opacity { NumberAnimation { duration: 150 } }

            property real rightEdgeCenterInset: visible && _photoVideoExpanded ? _rightPanelWidth : 0

            Component {
                id: photoVideoControlComponent

                PhotoVideoControl {
                }
            }
        }

        // Collapse chevron (>>) — at bottom-left of the expanded panel
        Rectangle {
            anchors.left:       parent.left
            anchors.bottom:     parent.bottom
            width:              ScreenTools.defaultFontPixelHeight * 2.5
            height:             width
            radius:             ScreenTools.defaultFontPixelHeight / 3
            color:              collapseMouseArea.containsMouse ? Qt.rgba(0, 0, 0, 0.6) : Qt.rgba(0, 0, 0, 0.35)
            visible:            _photoVideoExpanded
            z:                  1

            Behavior on color { ColorAnimation { duration: 150 } }

            Image {
                anchors.centerIn:   parent
                source:             "/res/buttonRight.svg"
                height:             parent.height * 0.75
                width:              height
                fillMode:           Image.PreserveAspectFit
                sourceSize.height:  height
                mipmap:             true
            }

            MouseArea {
                id:             collapseMouseArea
                anchors.fill:   parent
                hoverEnabled:   true
                cursorShape:    Qt.PointingHandCursor
                onClicked:      _setExpanded(false)
            }
        }

        // Collapsed: small expand button (<<). When fpvLocked, the button is
        // shown as a visual anchor for the status label below it but the click
        // is suppressed (the panel can't be opened while FPV is selected).
        Rectangle {
            id:                 expandButton
            anchors.right:      parent.right
            anchors.top:        parent.top
            width:              ScreenTools.defaultFontPixelHeight * 2.5
            height:             width
            radius:             ScreenTools.defaultFontPixelHeight / 3
            color:              expandMouseArea.containsMouse && !fpvLocked
                                    ? Qt.rgba(0, 0, 0, 0.6) : Qt.rgba(0, 0, 0, 0.35)
            opacity:            fpvLocked ? 0.55 : 1.0
            visible:            !_photoVideoExpanded

            Behavior on color   { ColorAnimation { duration: 150 } }
            Behavior on opacity { NumberAnimation { duration: 150 } }

            Image {
                anchors.centerIn:   parent
                source:             "/res/buttonLeft.svg"
                height:             parent.height * 0.75
                width:              height
                fillMode:           Image.PreserveAspectFit
                sourceSize.height:  height
                mipmap:             true
            }

            MouseArea {
                id:             expandMouseArea
                anchors.fill:   parent
                hoverEnabled:   true
                enabled:        !fpvLocked
                cursorShape:    fpvLocked ? Qt.ArrowCursor : Qt.PointingHandCursor
                onClicked:      _setExpanded(true)
            }
        }

        // Locked-state status readout — shows current image count (photo mode)
        // or current record time (video mode) under the (disabled) expand
        // arrow so the operator can see capture state even while the panel
        // is locked away.
        Rectangle {
            id:                          lockedStatusLabel
            anchors.top:                 expandButton.bottom
            anchors.right:               parent.right
            anchors.topMargin:           ScreenTools.defaultFontPixelHeight * 0.3
            width:                       lockedStatusText.implicitWidth + ScreenTools.defaultFontPixelWidth * 1.5
            height:                      lockedStatusText.implicitHeight + ScreenTools.defaultFontPixelHeight * 0.4
            radius:                      ScreenTools.defaultFontPixelHeight / 3
            color:                       Qt.rgba(0, 0, 0, 0.45)
            visible:                     fpvLocked && !_photoVideoExpanded

            QGCLabel {
                id:                 lockedStatusText
                anchors.centerIn:   parent
                color:              "white"
                font.pointSize:     ScreenTools.largeFontPointSize
                font.bold:          true
                text: {
                    if (!_cameraForStatus) return "--"
                    if (!_cameraInPhotoModeForStatus) {
                        var videoIdle = _cameraForStatus.videoCaptureStatus === MavlinkCameraControl.VIDEO_CAPTURE_STATUS_STOPPED
                        return videoIdle ? "00:00:00" : _cameraForStatus.recordTimeStr
                    }
                    if (!_activeVehicleForStatus) return "00000"
                    var count = _isAirPixelForStatus
                                    ? _activeVehicleForStatus.imageCount
                                    : _activeVehicleForStatus.cameraTriggerPoints.count
                    return ('00000' + count).slice(-5)
                }
            }
        }
    }
}
