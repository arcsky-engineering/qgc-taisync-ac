/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QtLocation
import QtPositioning
import QtQuick.Window
import QtQml.Models

import QGroundControl
import QGroundControl.Controls
import QGroundControl.Controllers
import QGroundControl.Controls
import QGroundControl.FactSystem
import QGroundControl.FlightDisplay
import QGroundControl.FlightMap
import QGroundControl.Palette
import QGroundControl.ScreenTools
import QGroundControl.Vehicle
import TaisyncInfo

// This is the ui overlay layer for the widgets/tools for Fly View
Item {
    id: _root

    property var    parentToolInsets
    property var    totalToolInsets:        _totalToolInsets
    property var    mapControl
    property bool   isViewer3DOpen:         false

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController
    property var    _geoFenceController:    _planMasterController.geoFenceController
    property var    _rallyPointController:  _planMasterController.rallyPointController
    property var    _guidedController:      globals.guidedControllerFlyView
    property real   _margins:               ScreenTools.defaultFontPixelWidth / 2
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75
    property rect   _centerViewport:        Qt.rect(0, 0, width, height)
    property real   _rightPanelWidth:       ScreenTools.defaultFontPixelWidth * 30
    property alias  _gripperMenu:           gripperOptions
    property real   _layoutMargin:          ScreenTools.defaultFontPixelWidth * 0.75
    property bool   _layoutSpacing:         ScreenTools.defaultFontPixelWidth
    property bool   _showSingleVehicleUI:   true

    property bool utmspActTrigger

    QGCToolInsets {
        id:                     _totalToolInsets
        leftEdgeTopInset:       toolStrip.leftEdgeTopInset
        leftEdgeCenterInset:    toolStrip.leftEdgeCenterInset
        leftEdgeBottomInset:    virtualJoystickMultiTouch.visible ? virtualJoystickMultiTouch.leftEdgeBottomInset : parentToolInsets.leftEdgeBottomInset
        rightEdgeTopInset:      topRightPanel.rightEdgeTopInset
        rightEdgeCenterInset:   topRightPanel.rightEdgeCenterInset
        rightEdgeBottomInset:   bottomRightRowLayout.rightEdgeBottomInset
        topEdgeLeftInset:       toolStrip.topEdgeLeftInset
        topEdgeCenterInset:     mapScale.topEdgeCenterInset
        topEdgeRightInset:      topRightPanel.topEdgeRightInset
        bottomEdgeLeftInset:    virtualJoystickMultiTouch.visible ? virtualJoystickMultiTouch.bottomEdgeLeftInset : parentToolInsets.bottomEdgeLeftInset
        bottomEdgeCenterInset:  bottomRightRowLayout.bottomEdgeCenterInset
        bottomEdgeRightInset:   virtualJoystickMultiTouch.visible ? virtualJoystickMultiTouch.bottomEdgeRightInset : bottomRightRowLayout.bottomEdgeRightInset
    }

    FlyViewTopRightPanel {
        id:                     topRightPanel
        anchors.top:            parent.top
        anchors.right:          parent.right
        anchors.topMargin:      _layoutMargin
        anchors.rightMargin:    _layoutMargin
        maximumHeight:          parent.height - (bottomRightRowLayout.height + _margins * 5)

        property real topEdgeRightInset:    height + _layoutMargin
        property real rightEdgeTopInset:    width + _layoutMargin
        property real rightEdgeCenterInset: rightEdgeTopInset
    }

    FlyViewTopRightColumnLayout {
        id:                 topRightColumnLayout
        anchors.margins:    _layoutMargin
        anchors.top:        parent.top
        anchors.bottom:     bottomRightRowLayout.top
        anchors.right:      parent.right
        spacing:            _layoutSpacing
        visible:           !topRightPanel.visible

        property real topEdgeRightInset:    childrenRect.height + _layoutMargin
        property real rightEdgeTopInset:    width + _layoutMargin
        property real rightEdgeCenterInset: rightEdgeTopInset
    }

    FlyViewBottomRightRowLayout {
        id:                 bottomRightRowLayout
        anchors.margins:    _layoutMargin
        anchors.bottom:     parent.bottom
        anchors.right:      parent.right
        spacing:            _layoutSpacing

        property real bottomEdgeRightInset:     height + _layoutMargin
        property real bottomEdgeCenterInset:    bottomEdgeRightInset
        property real rightEdgeBottomInset:     width + _layoutMargin
    }

    FlyViewMissionCompleteDialog {
        missionController:      _missionController
        geoFenceController:     _geoFenceController
        rallyPointController:   _rallyPointController
    }

    GuidedActionConfirm {
        anchors.margins:            _toolsMargin
        anchors.top:                parent.top
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          QGroundControl.zOrderTopMost
        guidedController:           _guidedController
        guidedValueSlider:          _guidedValueSlider
        utmspSliderTrigger:         utmspActTrigger
    }

    // ── RTSP Stream Switcher (FPV / Payload) ──
    Rectangle {
        id:                 streamSwitcher
        anchors.right:      parent.right
        anchors.rightMargin: _toolsMargin
        y:                  parent.height * 0.6 - height / 2
        width:              streamRow.width + ScreenTools.defaultFontPixelWidth * 2
        height:             streamRow.height + ScreenTools.defaultFontPixelHeight
        radius:             ScreenTools.defaultFontPixelWidth
        color:              Qt.rgba(0, 0, 0, 0.45)
        visible:            QGroundControl.videoManager.hasVideo

        property bool _isStream1: QGroundControl.videoManager.currentStream === "1"

        RowLayout {
            id:                 streamRow
            anchors.centerIn:   parent
            spacing:            2

            // FPV button
            Rectangle {
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 9
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 3.5
                radius:                 ScreenTools.defaultFontPixelWidth * 0.75
                color:                  streamSwitcher._isStream1 ? "#DE881E" : Qt.rgba(1, 1, 1, 0.08)
                border.color:           streamSwitcher._isStream1 ? Qt.lighter("#DE881E", 1.3) : Qt.rgba(1, 1, 1, 0.15)
                border.width:           streamSwitcher._isStream1 ? 2 : 1

                Behavior on color        { ColorAnimation { duration: 200 } }
                Behavior on border.color { ColorAnimation { duration: 200 } }

                ColumnLayout {
                    anchors.centerIn:   parent
                    spacing:            1

                    QGCColoredImage {
                        Layout.alignment:       Qt.AlignHCenter
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.2
                        Layout.preferredWidth:  Layout.preferredHeight
                        source:                 "/qmlimages/camera_video.svg"
                        fillMode:               Image.PreserveAspectFit
                        sourceSize.height:      Layout.preferredHeight
                        color:                  streamSwitcher._isStream1 ? "white" : Qt.rgba(1, 1, 1, 0.6)
                    }

                    Text {
                        Layout.alignment:       Qt.AlignHCenter
                        text:                   "FPV"
                        font.pixelSize:         ScreenTools.defaultFontPixelSize * 0.7
                        font.bold:              true
                        font.letterSpacing:     1.5
                        color:                  streamSwitcher._isStream1 ? "white" : Qt.rgba(1, 1, 1, 0.6)

                        Behavior on color { ColorAnimation { duration: 200 } }
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    cursorShape:    Qt.PointingHandCursor
                    onClicked: {
                        if (!streamSwitcher._isStream1)
                            QGroundControl.videoManager.switchRTSPStream()
                    }
                }
            }

            // Payload button
            Rectangle {
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 9
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 3.5
                radius:                 ScreenTools.defaultFontPixelWidth * 0.75
                color:                  !streamSwitcher._isStream1 ? "#DE881E" : Qt.rgba(1, 1, 1, 0.08)
                border.color:           !streamSwitcher._isStream1 ? Qt.lighter("#DE881E", 1.3) : Qt.rgba(1, 1, 1, 0.15)
                border.width:           !streamSwitcher._isStream1 ? 2 : 1

                Behavior on color        { ColorAnimation { duration: 200 } }
                Behavior on border.color { ColorAnimation { duration: 200 } }

                ColumnLayout {
                    anchors.centerIn:   parent
                    spacing:            1

                    QGCColoredImage {
                        Layout.alignment:       Qt.AlignHCenter
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.2
                        Layout.preferredWidth:  Layout.preferredHeight
                        source:                 "/qmlimages/camera_video.svg"
                        fillMode:               Image.PreserveAspectFit
                        sourceSize.height:      Layout.preferredHeight
                        color:                  !streamSwitcher._isStream1 ? "white" : Qt.rgba(1, 1, 1, 0.6)
                    }

                    Text {
                        Layout.alignment:       Qt.AlignHCenter
                        text:                   "CAM"
                        font.pixelSize:         ScreenTools.defaultFontPixelSize * 0.7
                        font.bold:              true
                        font.letterSpacing:     0.5
                        color:                  !streamSwitcher._isStream1 ? "white" : Qt.rgba(1, 1, 1, 0.6)

                        Behavior on color { ColorAnimation { duration: 200 } }
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    cursorShape:    Qt.PointingHandCursor
                    onClicked: {
                        if (streamSwitcher._isStream1)
                            QGroundControl.videoManager.switchRTSPStream()
                    }
                }
            }
        }
    }


    TaisyncInfo {
        id:taisyncPro
    }

    property bool _paramBoxShowEnable: QGroundControl.settingsManager.appSettings.taisyncFlyViewShow.value
    property bool _paramBoxVisible: true
    property bool _paramBoxLoaded: false
    Rectangle
    {
        property bool _draged: false

        id: paramBox
        border.width: 1
        border.color: "black"
        x: parent.width/2-width/2
        y: parent.height/2-height/2
        width: gridLayout.implicitWidth + hideButton.width * 2
        height: gridLayout.implicitHeight + ScreenTools.defaultFontPixelWidth * 2
        radius: 5
        clip: true
        color: Qt.rgba(255,255,255,100/255)
        visible: _paramBoxShowEnable && _paramBoxVisible && _paramBoxLoaded
        Component.onCompleted: {
            Qt.callLater(function() {
                x = parent.width/2-width/2
                y = parent.height/2-height/2
                _paramBoxLoaded = true
            });
        }
        onWidthChanged: {
            if (!_draged) x = parent.width/2-width/2
        }
        onHeightChanged: {
            if (!_draged) y = parent.height/2-height/2
        }
        MouseArea
        {
            anchors.fill: parent
            drag.target: paramBox
            onPressed: {
                // When press, we think the box will drag
                // then paramBox's center will not automic fix to the centre
                paramBox._draged = true
            }
        }

        GridLayout {
            property var _paramModel: [
                {"key": "airRSSI1",     "value": "-"+taisyncPro.airRSSI0 +"dBm"},
                {"key": "gndRSSI1",     "value": "-"+taisyncPro.gndRSSI0+"dBm"},
                {"key": "airRSSI2",     "value": "-"+taisyncPro.airRSSI1+"dBm"},
                {"key": "gndRSSI2",     "value": "-"+taisyncPro.gndRSSI1+"dBm"},
                {"key": "airSNR",       "value": taisyncPro.airSNR + "dB"},
                {"key": "gndSNR",       "value": taisyncPro.gndSNR + "dB"},
                {"key": "airPass",      "value": taisyncPro.airLDPCPass},
                {"key": "gndPass",      "value": taisyncPro.gndLDPCPass},
                {"key": "airFailed",    "value": taisyncPro.airLDPCFailed},
                {"key": "gndFailed",    "value": taisyncPro.gndLDPCFailed},
                {"key": "airAnt",       "value": taisyncPro.ant},
                {"key": "gndAnt",       "value": taisyncPro.antGnd},
                {"key": "freq",         "value": taisyncPro.currFreq},
                {"key": "mcs",          "value": taisyncPro.mcs},
                {"key": "range",        "value": taisyncPro.range + "m"},
                {"key": "rate",         "value": taisyncPro.dataRate+"kbps"},
            ]

            id: gridLayout
            columns: 4
            columnSpacing: ScreenTools.defaultFontPixelWidth * 3
            rowSpacing: ScreenTools.defaultFontPixelWidth
            anchors.centerIn: parent

            Repeater {
                model: gridLayout._paramModel.length * 2

                delegate: Label {
                    required property int index
                    property real _maxWidth: 0

                    text: index %2 === 0 ? gridLayout._paramModel[Math.floor(index/2)].key : gridLayout._paramModel[Math.floor(index/2)].value
                    font.pointSize: 12
                    color: "#000000"
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.fillWidth: true
                    Layout.preferredWidth: _maxWidth
                    onImplicitWidthChanged: {
                        if (implicitWidth > _maxWidth) {
                            _maxWidth = implicitWidth
                        }
                    }
                }
            }
        }

        Item {
            id: hideButton
            anchors.right: parent.right
            anchors.top: parent.top
            width: 50
            height: 50
            Image {
                anchors.margins: paramBox.border.width
                fillMode: Image.PreserveAspectFit
                source: "/res/hide.png"
                width: parseInt(parent.width * 0.8)
                height: width
                anchors.right: parent.right
                anchors.top: parent.top
            }
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                onClicked:
                {
                    _paramBoxVisible = false;
                }
            }
        }
    }
    Rectangle {
        id: viewButton
        visible: _paramBoxShowEnable && !_paramBoxVisible && _paramBoxLoaded
        x: parent.width/2-width/2
        y: parent.height/2-height/2
        width: 50
        height: 50
        border.width: 1
        border.color: "black"
        radius: 5
        color: Qt.rgba(255,255,255,100/255)
        Image {
            width: parseInt(parent.width * 0.8)
            height: width
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "/res/view.png"
        }
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                _paramBoxVisible = true;
            }
            drag.target: viewButton
        }
    }

    //-- Virtual Joystick
    Loader {
        id:                         virtualJoystickMultiTouch
        z:                          QGroundControl.zOrderTopMost + 1
        anchors.right:              parent.right
        anchors.rightMargin:        anchors.leftMargin
        height:                     Math.min(parent.height * 0.25, ScreenTools.defaultFontPixelWidth * 16)
        visible:                    _virtualJoystickEnabled && !QGroundControl.videoManager.fullScreen && !(_activeVehicle ? _activeVehicle.usingHighLatencyLink : false)
        anchors.bottom:             parent.bottom
        anchors.bottomMargin:       bottomLoaderMargin
        anchors.left:               parent.left   
        anchors.leftMargin:         ( y > toolStrip.y + toolStrip.height ? toolStrip.width / 2 : toolStrip.width * 1.05 + toolStrip.x) 
        source:                     "qrc:/qml/QGroundControl/FlightDisplay/VirtualJoystick.qml"
        active:                     _virtualJoystickEnabled && !(_activeVehicle ? _activeVehicle.usingHighLatencyLink : false)

        property real bottomEdgeLeftInset:     parent.height-y
        property bool autoCenterThrottle:      QGroundControl.settingsManager.appSettings.virtualJoystickAutoCenterThrottle.rawValue
        property bool leftHandedMode:          QGroundControl.settingsManager.appSettings.virtualJoystickLeftHandedMode.rawValue
        property bool _virtualJoystickEnabled: QGroundControl.settingsManager.appSettings.virtualJoystick.rawValue
        property real bottomEdgeRightInset:    parent.height-y
        property var  _pipViewMargin:          _pipView.visible ? parentToolInsets.bottomEdgeLeftInset + ScreenTools.defaultFontPixelHeight * 2 : 
                                               bottomRightRowLayout.height + ScreenTools.defaultFontPixelHeight * 1.5

        property var  bottomLoaderMargin:      _pipViewMargin >= parent.height / 2 ? parent.height / 2 : _pipViewMargin

        // Width is difficult to access directly hence this hack which may not work in all circumstances
        property real leftEdgeBottomInset:  visible ? bottomEdgeLeftInset + width/18 - ScreenTools.defaultFontPixelHeight*2 : 0
        property real rightEdgeBottomInset: visible ? bottomEdgeRightInset + width/18 - ScreenTools.defaultFontPixelHeight*2 : 0
        property real rootWidth:            _root.width
        property var  itemX:                virtualJoystickMultiTouch.x   // real X on screen

        onRootWidthChanged: virtualJoystickMultiTouch.status == Loader.Ready && visible ? virtualJoystickMultiTouch.item.uiTotalWidth = rootWidth : undefined
        onItemXChanged:     virtualJoystickMultiTouch.status == Loader.Ready && visible ? virtualJoystickMultiTouch.item.uiRealX = itemX : undefined

        //Loader status logic
        onLoaded: {
            if (virtualJoystickMultiTouch.visible) {
                virtualJoystickMultiTouch.item.calibration = true 
                virtualJoystickMultiTouch.item.uiTotalWidth = rootWidth
                virtualJoystickMultiTouch.item.uiRealX = itemX
            } else {
                virtualJoystickMultiTouch.item.calibration = false
            }
        }
    }

    FlyViewToolStrip {
        id:                     toolStrip
        anchors.leftMargin:     _toolsMargin + parentToolInsets.leftEdgeCenterInset
        anchors.topMargin:      _toolsMargin + parentToolInsets.topEdgeLeftInset
        anchors.left:           parent.left
        anchors.top:            parent.top
        z:                      QGroundControl.zOrderWidgets
        maxHeight:              parent.height - y - parentToolInsets.bottomEdgeLeftInset - _toolsMargin
        visible:                false //!QGroundControl.videoManager.fullScreen

        onDisplayPreFlightChecklist: {
            if (!preFlightChecklistLoader.active) {
                preFlightChecklistLoader.active = true
            }
            preFlightChecklistLoader.item.open()
        }

        property real topEdgeLeftInset:     visible ? y + height : 0
        property real leftEdgeTopInset:     visible ? x + width : 0
        property real leftEdgeCenterInset:  leftEdgeTopInset
    }

    GripperMenu {
        id: gripperOptions
    }

    VehicleWarnings {
        anchors.centerIn:   parent
        z:                  QGroundControl.zOrderTopMost
    }

    MapScale {
        id:                 mapScale
        anchors.margins:    _toolsMargin
        anchors.left:       toolStrip.right
        anchors.top:        parent.top
        mapControl:         _mapControl
        buttonsOnLeft:      true
        visible:            !ScreenTools.isTinyScreen && QGroundControl.corePlugin.options.flyView.showMapScale && !isViewer3DOpen && mapControl.pipState.state === mapControl.pipState.fullState

        property real topEdgeCenterInset: visible ? y + height : 0
    }

    Loader {
        id: preFlightChecklistLoader
        sourceComponent: preFlightChecklistPopup
        active: false
    }

    Component {
        id: preFlightChecklistPopup
        FlyViewPreFlightChecklistPopup {
        }
    }
}
