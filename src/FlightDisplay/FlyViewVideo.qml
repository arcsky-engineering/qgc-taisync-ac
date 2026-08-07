/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick

import QGroundControl
import QGroundControl.Controls
import QGroundControl.Controllers
import QGroundControl.ScreenTools

Item {
    id: _root

    property Item pipView
    property Item pipState: videoPipState

    property int    _track_rec_x:       0
    property int    _track_rec_y:       0

    PipState {
        id:         videoPipState
        pipView:    _root.pipView
        isDark:     true

        onWindowAboutToOpen: {
            QGroundControl.videoManager.stopVideo()
            videoStartDelay.start()
        }

        onWindowAboutToClose: {
            QGroundControl.videoManager.stopVideo()
            videoStartDelay.start()
        }

        onStateChanged: {
            if (pipState.state !== pipState.fullState) {
                QGroundControl.videoManager.fullScreen = false
            }
        }
    }

    Timer {
        id:           videoStartDelay
        interval:     2000;
        running:      false
        repeat:       false
        onTriggered:  QGroundControl.videoManager.startVideo()
    }

    //-- Video Streaming
    FlightDisplayViewVideo {
        id:             videoStreaming
        anchors.fill:   parent
        useSmallFont:   _root.pipState.state !== _root.pipState.fullState
        visible:        QGroundControl.videoManager.isStreamSource
    }
    //-- UVC Video (USB Camera or Video Device)
    Loader {
        id:             cameraLoader
        anchors.fill:   parent
        visible:        QGroundControl.videoManager.isUvc
        source:         QGroundControl.videoManager.uvcEnabled ? "qrc:/qml/QGroundControl/FlightDisplay/FlightDisplayViewUVC.qml" : "qrc:/qml/QGroundControl/FlightDisplay//FlightDisplayViewDummy.qml"
    }

    QGCLabel {
        text: qsTr(" ")
        font.pointSize: ScreenTools.largeFontPointSize
        visible: QGroundControl.videoManager.fullScreen && flyViewVideoMouseArea.containsMouse
        anchors.centerIn: parent

        onVisibleChanged: {
            if (visible) {
                labelAnimation.start()
            }
        }

        PropertyAnimation on opacity {
            id: labelAnimation
            duration: 10000
            from: 1.0
            to: 0.0
            easing.type: Easing.InExpo
        }
    }

    OnScreenGimbalController {
        id:                      onScreenGimbalController
        anchors.fill:            parent
        screenX:                 flyViewVideoMouseArea.mouseX
        screenY:                 flyViewVideoMouseArea.mouseY
        cameraTrackingEnabled:   videoStreaming._camera && videoStreaming._camera.trackingEnabled
    }

    MouseArea {
        // onDoubleClicked may conflict with onClicked, causing the callback to fail.
        // So, the fullScreen toggle operation has been moved to onClicked for handling
        property real preClickedTime: 0
        property real doubleClickedMinTime: 100
        property real doubleClickedMaxTime: 300

        id:                         flyViewVideoMouseArea
        anchors.fill:               parent
        enabled:                    pipState.state === pipState.fullState
        hoverEnabled:               true

        property double x0:         0
        property double x1:         0
        property double y0:         0
        property double y1:         0
        property double offset_x:   0
        property double offset_y:   0
        property double radius:     20
        property var trackingROI:   null
        property var trackingStatus: trackingStatusComponent.createObject(flyViewVideoMouseArea, {})

        onClicked:       {
            onScreenGimbalController.clickControl()

            let now = (new Date()).getTime();
            let delta = now - flyViewVideoMouseArea.preClickedTime
            if (delta >= doubleClickedMinTime && delta <= doubleClickedMaxTime) {
                QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
                // clear flag
                flyViewVideoMouseArea.preClickedTime = 0
            } else {
                flyViewVideoMouseArea.preClickedTime = now
            }
        }
        // onDoubleClicked: QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen

        onPressed:(mouse) => {
            onScreenGimbalController.pressControl()

            _track_rec_x = mouse.x
            _track_rec_y = mouse.y

            //create a new rectangle at the wanted position
            if(videoStreaming._camera) {
                if (videoStreaming._camera.trackingEnabled) {
                    trackingROI = trackingROIComponent.createObject(flyViewVideoMouseArea, {
                        "x": mouse.x,
                        "y": mouse.y
                    });
                }
            }
        }
        onPositionChanged: (mouse) => {
            //on move, update the width of rectangle
            if (trackingROI !== null) {
                if (mouse.x < trackingROI.x) {
                    trackingROI.x = mouse.x
                    trackingROI.width = Math.abs(mouse.x - _track_rec_x)
                } else {
                    trackingROI.width = Math.abs(mouse.x - trackingROI.x)
                }
                if (mouse.y < trackingROI.y) {
                    trackingROI.y = mouse.y
                    trackingROI.height = Math.abs(mouse.y - _track_rec_y)
                } else {
                    trackingROI.height = Math.abs(mouse.y - trackingROI.y)
                }
            }
        }
        onReleased: (mouse) => {
            onScreenGimbalController.releaseControl()
            
            //if there is already a selection, delete it
            if (trackingROI !== null) {
                trackingROI.destroy();
            }

            if(videoStreaming._camera) {
                if (videoStreaming._camera.trackingEnabled) {
                    // order coordinates --> top/left and bottom/right
                    x0 = Math.min(_track_rec_x, mouse.x)
                    x1 = Math.max(_track_rec_x, mouse.x)
                    y0 = Math.min(_track_rec_y, mouse.y)
                    y1 = Math.max(_track_rec_y, mouse.y)

                    //calculate offset between video stream rect and background (black stripes)
                    offset_x = (parent.width - videoStreaming.getWidth()) / 2
                    offset_y = (parent.height - videoStreaming.getHeight()) / 2

                    //convert absolute coords in background to absolute video stream coords
                    x0 = x0 - offset_x
                    x1 = x1 - offset_x
                    y0 = y0 - offset_y
                    y1 = y1 - offset_y

                    //convert absolute to relative coordinates and limit range to 0...1
                    x0 = Math.max(Math.min(x0 / videoStreaming.getWidth(), 1.0), 0.0)
                    x1 = Math.max(Math.min(x1 / videoStreaming.getWidth(), 1.0), 0.0)
                    y0 = Math.max(Math.min(y0 / videoStreaming.getHeight(), 1.0), 0.0)
                    y1 = Math.max(Math.min(y1 / videoStreaming.getHeight(), 1.0), 0.0)

                    //use point message if rectangle is very small
                    if (Math.abs(_track_rec_x - mouse.x) < 10 && Math.abs(_track_rec_y - mouse.y) < 10) {
                        var pt  = Qt.point(x0, y0)
                        videoStreaming._camera.startTracking(pt, radius / videoStreaming.getWidth())
                    } else {
                        var rec = Qt.rect(x0, y0, x1 - x0, y1 - y0)
                        videoStreaming._camera.startTracking(rec)
                    }
                    _track_rec_x = 0
                    _track_rec_y = 0
                }
            }
        }

        Component {
            id: trackingROIComponent

            Rectangle {
                color:              Qt.rgba(0.1,0.85,0.1,0.25)
                border.color:       "green"
                border.width:       1
            }
        }

        Component {
            id: trackingStatusComponent

            Rectangle {
                color:              "transparent"
                border.color:       "red"
                border.width:       5
                radius:             5
            }
        }

        Timer {
            id: trackingStatusTimer
            interval:               50
            repeat:                 true
            running:                true
            onTriggered: {
                if (videoStreaming._camera) {
                    if (videoStreaming._camera.trackingEnabled && videoStreaming._camera.trackingImageStatus) {
                        var margin_hor = (parent.parent.width - videoStreaming.getWidth()) / 2
                        var margin_ver = (parent.parent.height - videoStreaming.getHeight()) / 2
                        var left = margin_hor + videoStreaming.getWidth() * videoStreaming._camera.trackingImageRect.left
                        var top = margin_ver + videoStreaming.getHeight() * videoStreaming._camera.trackingImageRect.top
                        var right = margin_hor + videoStreaming.getWidth() * videoStreaming._camera.trackingImageRect.right
                        var bottom = margin_ver + !isNaN(videoStreaming._camera.trackingImageRect.bottom) ? videoStreaming.getHeight() * videoStreaming._camera.trackingImageRect.bottom : top + (right - left)
                        var width = right - left
                        var height = bottom - top

                        flyViewVideoMouseArea.trackingStatus.x = left
                        flyViewVideoMouseArea.trackingStatus.y = top
                        flyViewVideoMouseArea.trackingStatus.width = width
                        flyViewVideoMouseArea.trackingStatus.height = height
                    } else {
                        flyViewVideoMouseArea.trackingStatus.x = 0
                        flyViewVideoMouseArea.trackingStatus.y = 0
                        flyViewVideoMouseArea.trackingStatus.width = 0
                        flyViewVideoMouseArea.trackingStatus.height = 0
                    }
                }
            }
        }
    }

    ProximityRadarVideoView{
        anchors.fill:   parent
        vehicle:        QGroundControl.multiVehicleManager.activeVehicle
    }

    ObstacleDistanceOverlayVideo {
        id: obstacleDistance
        showText: pipState.state === pipState.fullState
    }

    // ── Tap-to-Focus overlay (ILX / TAG-E) ──
    // Armed only when: the feature is toggled on (flyViewSettings.tapToFocusEnabled),
    // the ILX payload is selected (payloadSelection == 0), the payload (secondary/CAM)
    // video is the one being shown (videoManager.currentStream == "2"), the video is
    // the full view (not the small PIP), and a TAG-E component id is known.
    // While armed it sits on top and intercepts taps, so a tap sets the AF point
    // instead of moving the on-screen gimbal; when not armed it is disabled and taps
    // fall through to flyViewVideoMouseArea (gimbal / tracking / double-click fullscreen).
    MouseArea {
        id:             tapToFocusArea
        anchors.fill:   parent
        z:              1
        hoverEnabled:   false
        cursorShape:    Qt.CrossCursor

        property var  _vehicle:      QGroundControl.multiVehicleManager.activeVehicle
        property bool _isIlx:        QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 0
        property bool _payloadVideo: QGroundControl.videoManager.currentStream === "2"
        property bool _tracking:     videoStreaming._camera && videoStreaming._camera.trackingEnabled
        property bool _armed:        QGroundControl.settingsManager.flyViewSettings.tapToFocusEnabled.rawValue &&
                                     _isIlx && _payloadVideo && !_tracking &&
                                     _root.pipState.state === _root.pipState.fullState &&
                                     _vehicle && _vehicle.airPixelComponentId > 0

        enabled:        _armed

        // Double-click still toggles fullscreen (matching flyViewVideoMouseArea's
        // timing). We must handle it here because while armed this overlay consumes
        // the taps that the underlying handler would otherwise use — otherwise the
        // user could neither enter nor exit fullscreen, and in fullscreen the ILX
        // popup (with the off switch) is hidden. A single tap sets focus.
        property real preClickedTime:       0
        property real doubleClickedMinTime: 100
        property real doubleClickedMaxTime: 300

        onClicked: (mouse) => {
            var now = (new Date()).getTime()
            var delta = now - preClickedTime
            if (delta >= doubleClickedMinTime && delta <= doubleClickedMaxTime) {
                QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
                preClickedTime = 0
                return
            }
            preClickedTime = now

            var vw = videoStreaming.getWidth()
            var vh = videoStreaming.getHeight()
            if (vw <= 0 || vh <= 0) return

            // Strip the letterbox bars, then normalize to the video content (0..1).
            var lx = (width  - vw) / 2
            var ly = (height - vh) / 2
            var xNorm = Math.max(0, Math.min((mouse.x - lx) / vw, 1))
            var yNorm = Math.max(0, Math.min((mouse.y - ly) / vh, 1))

            // The camera's AF field doesn't cover the whole displayed video: the
            // reachable area is inset from each edge (box size, plus the still-frame
            // vs 16:9-video FOV difference — larger vertically because the 16:9 video
            // is a vertical crop of the 3:2 still). So the camera's bracket always
            // lands more toward center than a naive 1..100 mapping, and can't reach
            // the video edges at all. Model that reachable region as an inset band
            // [inset, 1-inset] of the video, separately for X and Y. We draw the
            // reticle at the reachable position (so it matches the bracket) and map
            // that band onto the camera's 1..100 command range.
            //
            // Tune on the bench: if the camera bracket still lands more central than
            // the reticle at an edge, RAISE that axis's inset; if the bracket lands
            // outside the reticle (nearer the edge), LOWER it. Center is unaffected.
            var ax = _afInsetXPct / 100
            var ay = _afInsetYPct / 100
            var rx = 1 - 2 * ax
            var ry = 1 - 2 * ay
            var drawXNorm = Math.max(ax, Math.min(xNorm, 1 - ax))
            var drawYNorm = Math.max(ay, Math.min(yNorm, 1 - ay))
            var xPct = Math.round(((drawXNorm - ax) / rx) * 99 + 1)   // band -> 1..100
            var yPct = Math.round(((drawYNorm - ay) / ry) * 99 + 1)

            var compId = _vehicle.airPixelComponentId
            // DO_DIGICAM_CONFIGURE (202): p1=1101 sets the AF point (X,Y percent),
            // then p1=135 triggers autofocus (half-press) at that point. These are
            // paced apart via afTriggerTimer — the TAG-E drops rapid-fire sends
            // (see PhotoVideoControl's preset queue), so firing 135 in the same
            // instant as 1101 gets it dropped and nothing focuses. showError=false
            // because this is a frequent gesture — no error dialogs on every tap.
            _vehicle.sendCommand(compId, 202, false, 1101, xPct, yPct, 0, 0, 0, 0)
            _pendingCompId  = compId
            _afPulsesLeft   = _afPulseCount
            afTriggerTimer.restart()
            console.log("[TAP_FOCUS] compId", compId, "point", xPct + "%," + yPct + "%")

            // Draw the reticle where the camera can actually focus (the inset band),
            // not at the raw finger, so it lines up with the camera's bracket.
            focusReticle.x = lx + drawXNorm * vw - focusReticle.width  / 2
            focusReticle.y = ly + drawYNorm * vh - focusReticle.height / 2
            focusReticleAnim.restart()
        }

        // After moving the AF point (1101) we fire the AF trigger (135) as a short
        // burst rather than a single pulse. A lone 135 focuses fine when the camera
        // is idle, but right after a point move the TAG-E needs a *sustained*
        // half-press to hunt and lock at the new spot — the dev noted repeating 135
        // prolongs the half-press. First pulse is delayed one interval so the point
        // move settles first (and isn't dropped as a rapid-fire duplicate).
        property int _pendingCompId:      0
        readonly property int _afPulseCount: 3
        property int _afPulsesLeft:       0

        // AF-field inset (percent) from each video edge — the reachable focus band.
        // Y is usually larger than X because the 16:9 video is a vertical crop of the
        // 3:2 still frame. Live calibration knobs (steppers in the ILX AUTOFOCUS
        // panel); tune per the note in onClicked.
        property real _afInsetXPct: QGroundControl.settingsManager.flyViewSettings.tapToFocusInsetX.value
        property real _afInsetYPct: QGroundControl.settingsManager.flyViewSettings.tapToFocusInsetY.value
        Timer {
            id:                 afTriggerTimer
            interval:           300
            repeat:             true
            triggeredOnStart:   false
            onTriggered: {
                if (tapToFocusArea._pendingCompId <= 0 || tapToFocusArea._afPulsesLeft <= 0) {
                    stop()
                    return
                }
                tapToFocusArea._vehicle.sendCommand(tapToFocusArea._pendingCompId, 202, false, 135, 0, 0, 0, 0, 0, 0)
                tapToFocusArea._afPulsesLeft--
                console.log("[TAP_FOCUS] AF pulse sent, remaining", tapToFocusArea._afPulsesLeft)
            }
        }
    }

    // Brief focus reticle drawn at the tapped point for visual feedback.
    Rectangle {
        id:             focusReticle
        width:          ScreenTools.defaultFontPixelHeight * 3
        height:         width
        radius:         4
        color:          "transparent"
        border.color:   "#00e0ff"
        border.width:   2
        opacity:        0
        visible:        opacity > 0
        z:              2

        SequentialAnimation {
            id: focusReticleAnim
            ParallelAnimation {
                NumberAnimation { target: focusReticle; property: "opacity"; from: 0.0; to: 1.0; duration: 120 }
                NumberAnimation { target: focusReticle; property: "scale";   from: 1.6; to: 1.0; duration: 180; easing.type: Easing.OutBack }
            }
            PauseAnimation  { duration: 500 }
            NumberAnimation { target: focusReticle; property: "opacity"; to: 0.0; duration: 300 }
        }
    }
}
