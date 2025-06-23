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

//-------------------------------------------------------------------------
//-- GPS Indicator
Item {
    id:             _root
    width:          (gpsValuesColumn.x + gpsValuesColumn.width) * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    Component {
        id: gpsInfo

        Rectangle {
            width:  gpsCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: gpsCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 gpsCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(gpsGrid.width, gpsLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             gpsLabel
                    text:           (_activeVehicle && _activeVehicle.gps.count.value >= 0) ? qsTr("GPS Status") : qsTr("GPS Data Unavailable")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 gpsGrid
                    visible:            (_activeVehicle && _activeVehicle.gps.count.value >= 0)
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    anchors.horizontalCenter: parent.horizontalCenter
                    columns: 2

                    QGCLabel { text: qsTr("GPS Count:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps.count.valueString : qsTr("N/A", "No data to display") }
                    QGCLabel { text: qsTr("GPS Lock:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps.lock.enumStringValue : qsTr("N/A", "No data to display") }
                    QGCLabel { text: qsTr("HDOP:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps.hdop.valueString : qsTr("--.--", "No data to display") }
                    QGCLabel { text: qsTr("VDOP:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps.vdop.valueString : qsTr("--.--", "No data to display") }
                    QGCLabel { text: qsTr("Course Over Ground:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps.courseOverGround.valueString : qsTr("--.--", "No data to display") }
                }
                QGCButton {
                    id: ntripToggleButton
                    text: QGroundControl.ntrip.enabled ? qsTr("Disconnect NTRIP") : qsTr("Connect NTRIP")
                    visible: QGroundControl.ntrip.masterEnable
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        QGroundControl.ntrip.enabled = !QGroundControl.ntrip.enabled
                    }
                }
                QGCLabel {
                    text: {
                        const status = QGroundControl.ntrip.connectionStatus;
                        return status === 0 ? "NTRIP STATUS: Off"
                             : status === 1 ? "NTRIP STATUS: Connecting"
                             : status === 2 ? "NTRIP STATUS: Connected"
                             : status === 3 ? "NTRIP STATUS: Retrying"
                             : status === 4 ? "NTRIP STATUS: Timed Out"
                             : "NTRIP STATUS: Unknown";
                    }
                    visible: QGroundControl.ntrip.masterEnable
                }
            }
        }
    }

    QGCColoredImage {
        id:                 gpsIcon
        width:              height
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        fillMode:           Image.PreserveAspectFit
        sourceSize.height:  height
        opacity:            (_activeVehicle && _activeVehicle.gps.count.value >= 0) ? 1 : 0.5
        source: {
            const lock = _activeVehicle ? _activeVehicle.gps.lock.rawValue : 0
            const ntripEnabled = QGroundControl.ntrip.masterEnable && QGroundControl.ntrip.enabled
            const ntripConnected = QGroundControl.ntrip.connectionStatus === 2

            if (lock === 6) { // RTK Fixed
                if (!ntripEnabled) return "/qmlimages/RTK-fixed.svg"
                return ntripConnected ? "/qmlimages/RTK-fixed-ntrip-good.svg" : "/qmlimages/RTK-fixed-ntrip-bad.svg"
            }
            if (lock === 5) { // RTK Float
                if (!ntripEnabled) return "/qmlimages/RTK-float.svg"
                return ntripConnected ? "/qmlimages/RTK-float-ntrip-good.svg" : "/qmlimages/RTK-float-ntrip-bad.svg"
            }
            // Normal GPS (lock != 5,6)
            if (!ntripEnabled) return "/qmlimages/Gps.svg"
            return ntripConnected ? "/qmlimages/Gps-ntrip-good.svg" : "/qmlimages/Gps-ntrip-bad.svg"
        }
        color: {
            const ntripEnabled = QGroundControl.ntrip.masterEnable && QGroundControl.ntrip.enabled
            const ntripConnected = QGroundControl.ntrip.connectionStatus === 2
            if (ntripEnabled) {
                return ntripConnected ? "green" : "red"
            }
            return qgcPal.buttonText
        }
    }

    Column {
        id:                     gpsValuesColumn
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
        anchors.left:           gpsIcon.right

        QGCLabel {
            anchors.horizontalCenter:   hdopValue.horizontalCenter
            visible:                    _activeVehicle && !isNaN(_activeVehicle.gps.hdop.value)
            color:                      qgcPal.buttonText
            text:                       _activeVehicle ? _activeVehicle.gps.count.valueString : ""
        }

        QGCLabel {
            id:         hdopValue
            visible:    _activeVehicle && !isNaN(_activeVehicle.gps.hdop.value)
            color:      qgcPal.buttonText
            text:       _activeVehicle ? _activeVehicle.gps.hdop.value.toFixed(1) : ""
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, gpsInfo)
        }
    }
}
