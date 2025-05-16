/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

//-------------------------------------------------------------------------
//--WPNAV Control
Item {
    id: _root
    width: ScreenTools.defaultFontPixelWidth * 7
    height: parent.height

    property bool showIndicator: true
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    // Create the FactPanelController to access parameters
    FactPanelController {
        id: controller
    }

    property Fact wpnavSpeedFact: null

    // Wait for parameters to be ready
    Timer {
        id: paramReadyTimer
        interval: 1000 // Check every 100ms
        running: true
        repeat: true
        onTriggered: {
            if (controller.parametersReady) {
                wpnavSpeedFact = controller.getParameterFact(-1, "WPNAV_SPEED")
                console.log("✅ WPNAV_SPEED loaded:", wpnavSpeedFact ? wpnavSpeedFact.value : "null")
                paramReadyTimer.stop()  // Stop the timer once parameters are ready
            }
            else
            {
                console.log("Not ready")
            }
        }
    }

//    property Fact wpnavSpeedFact: null

    // Delay fetch using Timer (e.g., 5 seconds)
//    Timer {
//        id: paramDelayTimer
//        interval: 5000    // 5000 ms = 5 seconds
//        running: true
//        repeat: false
//        onTriggered: {
//            if (controller.parametersReady && controller.parameterExists(-1, "WPNAV_SPEED")) {
//                wpnavSpeedFact = controller.getParameterFact(-1, "WPNAV_SPEED")
//                console.log("✅ WPNAV_SPEED loaded:", wpnavSpeedFact ? wpnavSpeedFact.value : "null")
//            } else {
//                console.warn("❌ Parameters not ready or WPNAV_SPEED missing")
//            }
//        }
//    }

    // Toolbar Icon
    QGCColoredImage {
        id: speedIcon
        width: height
        height: parent.height
        source: "/qmlimages/RTK.svg"
        fillMode: Image.PreserveAspectFit
        sourceSize.height: height
        color: qgcPal.buttonText
    }

    // Open popup when clicked
    MouseArea {
        anchors.fill: parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, speedInfo)
        }
    }

    // Popup content for adjusting WPNAV_SPEED
    Component {
        id: speedInfo

        Rectangle {
            width: speedCol.width + ScreenTools.defaultFontPixelWidth * 3
            height: speedCol.height + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color: qgcPal.window
            border.color: qgcPal.text

            Column {
                id: speedCol
                spacing: ScreenTools.defaultFontPixelHeight * 0.5
                anchors.margins: ScreenTools.defaultFontPixelHeight
                anchors.centerIn: parent

                QGCLabel {
                    text: qsTr("Change WP Speed")
                    font.family: ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                QGCLabel {
                    id: currentSpeedLabel
                    text: qsTr("Current: --") //wpnavSpeedFact ?
                          //qsTr("Current: %1 m/s").arg((wpnavSpeedFact.value / 100.0).toFixed(1)) :
                          //qsTr("Current: --")
                    color: qgcPal.text
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: qgcPal.text
                    opacity: 0.2
                }

                // Buttons to set WP speed
                QGCButton {
                    text: qsTr("5 m/s")
                    onClicked: {
                        if (wpnavSpeedFact) {
                            wpnavSpeedFact.value = 500
                            mainWindow.hideIndicatorPopup()
                        }
                    }
                }

                QGCButton {
                    text: qsTr("8 m/s")
                    onClicked: {
                        if (wpnavSpeedFact) {
                            wpnavSpeedFact.value = 800
                            mainWindow.hideIndicatorPopup()
                        }
                    }
                }

                QGCButton {
                    text: qsTr("10 m/s")
                    onClicked: {
                        if (wpnavSpeedFact) {
                            wpnavSpeedFact.value = 1000
                            mainWindow.hideIndicatorPopup()
                        }
                    }
                }
            }

            // Listen for external updates to the parameter value
//            Connections {
//                target: wpnavSpeedFact
//                onValueChanged: {
//                    currentSpeedLabel.text = wpnavSpeedFact ?
//                        qsTr("Current: %1 m/s").arg((wpnavSpeedFact.value / 100.0).toFixed(1)) : "--"
//                }
//            }
        }
    }
}

// -- OLD STUFF BELOW
//Item {
//    id:             _root
//    width:          (speedInfo.x + SpeedInfo.width) * 1.1//(gpsValuesColumn.x + gpsValuesColumn.width) * 1.1
//    anchors.top:    parent.top
//    anchors.bottom: parent.bottom

//    property bool showIndicator: true

//    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

//    Component {
//        id: speedInfo

//        Rectangle {
//            width:  speedCol.width   + ScreenTools.defaultFontPixelWidth  * 3
//            height: speedCol.height  + ScreenTools.defaultFontPixelHeight * 2
//            radius: ScreenTools.defaultFontPixelHeight * 0.5
//            color:  qgcPal.window
//            border.color:   qgcPal.text

//            Column {
//                id:                 speedCol
//                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
//                width:              speedLabel.width
//                anchors.margins:    ScreenTools.defaultFontPixelHeight
//                anchors.centerIn:   parent

//                QGCLabel {
//                    id:             speedLabel
//                    text:           qsTr("Change WP Speed")
//                    font.family:    ScreenTools.demiboldFontFamily
//                    anchors.horizontalCenter: parent.horizontalCenter
//                }

//                QGCButton {
//                    id: wpnavSpeedButton
//                    text: qsTr("10 m/s")
////                    visible: QGroundControl.ntrip.masterEnable
////                    anchors.horizontalCenter: parent.horizontalCenter
////                    onClicked: {
////                        QGroundControl.ntrip.enabled = !QGroundControl.ntrip.enabled
////                    }
//                }
//            }
//        }
//    }

//    QGCColoredImage {
//        id:                 speedIcon
//        width:              height
//        anchors.top:        parent.top
//        anchors.bottom:     parent.bottom
//        source:             "/qmlimages/RTK.svg"
//        fillMode:           Image.PreserveAspectFit
//        sourceSize.height:  height
//        //opacity:            (_activeVehicle && _activeVehicle.gps.count.value >= 0) ? 1 : 0.5
//        color:              qgcPal.buttonText
//    }

////    Column {
////        id:                     gpsValuesColumn
////        anchors.verticalCenter: parent.verticalCenter
////        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
////        anchors.left:           gpsIcon.right

////        QGCLabel {
////            anchors.horizontalCenter:   hdopValue.horizontalCenter
////            visible:                    _activeVehicle && !isNaN(_activeVehicle.gps.hdop.value)
////            color:                      qgcPal.buttonText
////            text:                       _activeVehicle ? _activeVehicle.gps.count.valueString : ""
////        }

////        QGCLabel {
////            id:         hdopValue
////            visible:    _activeVehicle && !isNaN(_activeVehicle.gps.hdop.value)
////            color:      qgcPal.buttonText
////            text:       _activeVehicle ? _activeVehicle.gps.hdop.value.toFixed(1) : ""
////        }
////    }

//    MouseArea {
//        anchors.fill:   parent
//        onClicked: {
//            mainWindow.showIndicatorPopup(_root, speedInfo)
//        }
//    }
//}
