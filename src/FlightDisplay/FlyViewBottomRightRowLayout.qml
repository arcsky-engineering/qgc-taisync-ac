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
import QGroundControl.ScreenTools

// Root is an Item (not a RowLayout) so the toggle arrow can be a sibling of
// the inner row and anchor itself to the bottom-right corner — sitting
// transparently over the compass widget when expanded, and as a standalone
// button at the screen's right edge when collapsed.
Item {
    id: rootItem

    // Forwarded from the parent's `spacing:` assignment to the inner RowLayout.
    property real   spacing:                0

    // Persisted expanded/collapsed state.
    property bool   _bottomRightExpanded:   QGroundControl.loadBoolGlobalSetting(_expandedKey, true)
    property string _expandedKey:           "FlyViewBottomRightExpanded"

    // Item doesn't auto-size; mirror the inner row's content but never shrink
    // below the toggle arrow so the user can find it again when collapsed.
    implicitWidth:  Math.max(innerRow.implicitWidth,  toggleArrow.width)
    implicitHeight: Math.max(innerRow.implicitHeight, toggleArrow.height)

    function _setBottomRightExpanded(expanded) {
        _bottomRightExpanded = expanded
        QGroundControl.saveBoolGlobalSetting(_expandedKey, expanded)
    }

    RowLayout {
        id:                 innerRow
        anchors.right:      parent.right
        anchors.bottom:     parent.bottom
        spacing:            rootItem.spacing

        TelemetryValuesBar {
            Layout.alignment:       Qt.AlignBottom
            extraWidth:             instrumentPanel.extraValuesWidth
            settingsGroup:          factValueGrid.telemetryBarSettingsGroup
            specificVehicleForCard: null // Tracks active vehicle
            visible:                rootItem._bottomRightExpanded
        }

        FlyViewInstrumentPanel {
            id:                 instrumentPanel
            Layout.alignment:   Qt.AlignBottom
            visible:            QGroundControl.corePlugin.options.flyView.showInstrumentPanel
                                && _showSingleVehicleUI
                                && rootItem._bottomRightExpanded
        }
    }

    // Toggle arrow — anchored to the bottom-right of the whole widget group.
    // Background is mostly transparent when expanded so the compass beneath
    // stays visible; slightly more opaque when collapsed so the standalone
    // button is easy to find against any map background.
    Rectangle {
        id:                 toggleArrow
        anchors.right:      parent.right
        anchors.bottom:     parent.bottom
        width:              ScreenTools.defaultFontPixelHeight * 2.5
        height:             width
        radius:             ScreenTools.defaultFontPixelHeight / 3
        z:                  10
        color: {
            var hovered = bottomRightToggleMouseArea.containsMouse
            if (rootItem._bottomRightExpanded) {
                return Qt.rgba(0, 0, 0, hovered ? 0.5 : 0.15)
            }
            return Qt.rgba(0, 0, 0, hovered ? 0.6 : 0.35)
        }

        Behavior on color { ColorAnimation { duration: 150 } }

        Image {
            anchors.centerIn:   parent
            source:             rootItem._bottomRightExpanded ? "/res/buttonRight.svg" : "/res/buttonLeft.svg"
            height:             parent.height * 0.75
            width:              height
            fillMode:           Image.PreserveAspectFit
            sourceSize.height:  height
            mipmap:             true
        }

        MouseArea {
            id:             bottomRightToggleMouseArea
            anchors.fill:   parent
            hoverEnabled:   true
            cursorShape:    Qt.PointingHandCursor
            onClicked:      rootItem._setBottomRightExpanded(!rootItem._bottomRightExpanded)
        }
    }
}
