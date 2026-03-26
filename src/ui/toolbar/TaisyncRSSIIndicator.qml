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
//-- Taisync Radio RSSI Indicator
Item {
    id:             _root
    width:          rssiRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: _taisyncAvailable && _taisyncOnline

    property bool _taisyncAvailable:  typeof taisyncRemoteHandler !== "undefined"
    property bool _taisyncOnline:     _taisyncAvailable ? taisyncRemoteHandler.online : false
    property int  _airLinkQuality:    _taisyncAvailable ? taisyncRemoteHandler.airLinkQuaity : 0
    property int  _gndLinkQuality:    _taisyncAvailable ? taisyncRemoteHandler.gndLinkQuaity : 0
    property int  _displayPercent:    Math.min(_airLinkQuality, _gndLinkQuality)

    Component {
        id: taisyncRSSIInfo

        Rectangle {
            width:  taisyncCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: taisyncCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 taisyncCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(taisyncGrid.width, taisyncLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             taisyncLabel
                    text:           qsTr("Taisync Radio Status")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 taisyncGrid
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    columns:            2
                    anchors.horizontalCenter: parent.horizontalCenter

                    QGCLabel { text: qsTr("Air RSSI:") }
                    QGCLabel { text: _taisyncAvailable ? ("-" + taisyncRemoteHandler.airRssi0 + ", -" + taisyncRemoteHandler.airRssi1 + " dBm") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Gnd RSSI:") }
                    QGCLabel { text: _taisyncAvailable ? ("-" + taisyncRemoteHandler.gndRssi0 + ", -" + taisyncRemoteHandler.gndRssi1 + " dBm") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Air Link Quality:") }
                    QGCLabel { text: _airLinkQuality + "%" }

                    QGCLabel { text: qsTr("Gnd Link Quality:") }
                    QGCLabel { text: _gndLinkQuality + "%" }

                    QGCLabel { text: qsTr("Air SNR:") }
                    QGCLabel { text: _taisyncAvailable ? (taisyncRemoteHandler.airSnr + " dB") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Gnd SNR:") }
                    QGCLabel { text: _taisyncAvailable ? (taisyncRemoteHandler.gndSnr + " dB") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Frequency:") }
                    QGCLabel { text: _taisyncAvailable ? (taisyncRemoteHandler.gndFreq + " MHz") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Distance:") }
                    QGCLabel { text: _taisyncAvailable ? (taisyncRemoteHandler.gndDistance + " m") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Eth TX Rate:") }
                    QGCLabel { text: _taisyncAvailable ? (taisyncRemoteHandler.ethTxRate + " bps") : qsTr("N/A") }

                    QGCLabel { text: qsTr("Air LDPC Failed:") }
                    QGCLabel { text: _taisyncAvailable ? taisyncRemoteHandler.airLDPCFailed : qsTr("N/A") }

                    QGCLabel { text: qsTr("Gnd LDPC Failed:") }
                    QGCLabel { text: _taisyncAvailable ? taisyncRemoteHandler.gndLDPCFailed : qsTr("N/A") }
                }
            }
        }
    }

    Row {
        id:             rssiRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing:                0

            QGCLabel {
                text:                   _taisyncOnline ? _displayPercent + "%" : "--"
                font.pointSize:         ScreenTools.smallFontPointSize
                anchors.horizontalCenter: parent.horizontalCenter
            }

            QGCColoredImage {
                width:              height
                height:             rssiRow.height * 0.65
                sourceSize.height:  height
                source:             "/qmlimages/RC.svg"
                fillMode:           Image.PreserveAspectFit
                opacity:            _taisyncOnline ? 1 : 0.5
                color:              qgcPal.buttonText
            }
        }

        SignalStrength {
            anchors.verticalCenter: parent.verticalCenter
            size:                   parent.height * 0.5
            percent:                _taisyncOnline ? _displayPercent : 0
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, taisyncRSSIInfo)
        }
    }
}
