/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.12
import QtQuick.Layouts  1.12

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0

ColumnLayout {
    id:         root
    spacing:    ScreenTools.defaultFontPixelHeight / 4

    property real   _innerRadius:           (width - (_topBottomMargin * 3)) / 4
    property real   _outerRadius:           _innerRadius + _topBottomMargin
    property real   _spacing:               ScreenTools.defaultFontPixelHeight * 0.33
    property real   _topBottomMargin:       (width * 0.05) / 2

    //signal minimizeRequested() // signal to notify parent

    QGCPalette { id: qgcPal }

    Rectangle {
        id:                 visualInstrument
        height:             _outerRadius * 2
        Layout.fillWidth:   true
        radius:             _outerRadius
        color:              qgcPal.window
        opacity: 0.7
        property real globalOpacity: 0.8
        //property bool isMinimized: false // track minimize state

        DeadMouseArea { anchors.fill: parent }

        QGCAttitudeWidget {
            id:                     attitude
            anchors.leftMargin:     _topBottomMargin
            anchors.left:           parent.left
            size:                   _innerRadius * 2
            vehicle:                globals.activeVehicle
            anchors.verticalCenter: parent.verticalCenter
        }

        QGCCompassWidget {
            id:                     compass
            anchors.leftMargin:     _spacing
            anchors.left:           attitude.right
            size:                   _innerRadius * 2
            vehicle:                globals.activeVehicle
            anchors.verticalCenter: parent.verticalCenter
        }

//        Component.onCompleted: {
//            for (var i = 0; i < children.length; i++) {
//                if (children[i].hasOwnProperty("opacity")) {
//                    children[i].opacity = globalOpacity;
//                }
//            }
//        }
//        Image {
//            id:             minimizeButton
//            source:         "/qmlimages/pipHide.svg"
//            mipmap:         true
//            rotation:       180
//            fillMode:       Image.PreserveAspectFit
//            anchors.right:   parent.right
//            anchors.top:     parent.top
//            //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
//            height:         ScreenTools.defaultFontPixelHeight * 2.5
//            width:          ScreenTools.defaultFontPixelHeight * 2.5
//            sourceSize.height:  height
//            MouseArea {
//                anchors.fill:   parent
//                onClicked:      minimizeRequested() //visualInstrument.isMinimized = !visualInstrument.isMinimized
//            }
//        }
    }

    TerrainProgress {
        Layout.fillWidth: true
    }
}
