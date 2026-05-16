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

ColumnLayout {
    width: _rightPanelWidth

    property bool   _photoVideoExpanded:    QGroundControl.loadBoolGlobalSetting(_expandedKey, true)
    property string _expandedKey:           "PhotoVideoControlExpanded"

    function _setExpanded(expanded) {
        _photoVideoExpanded = expanded
        QGroundControl.saveBoolGlobalSetting(_expandedKey, expanded)
    }

    TerrainProgress {
        Layout.alignment:       Qt.AlignTop
        Layout.preferredWidth:  _rightPanelWidth
    }

    // LiDAR Control (shown in place of PhotoVideoControl when LiDAR payload selected).
    // Wrapped in a Loader so FactPanelController inside LidarControl is only constructed once a
    // real vehicle is connected — otherwise it latches onto the offline-editing vehicle and
    // PTRN_ params never resolve.
    Loader {
        Layout.alignment:   Qt.AlignTop | Qt.AlignRight
        active:             globals.activeVehicle
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
        Layout.preferredWidth:  _photoVideoExpanded ? _rightPanelWidth : expandButton.width
        Layout.preferredHeight: _photoVideoExpanded ? photoVideoControlLoader.height : expandButton.height
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

        // Collapsed: small expand button (<<)
        Rectangle {
            id:                 expandButton
            anchors.right:      parent.right
            width:              ScreenTools.defaultFontPixelHeight * 2.5
            height:             width
            radius:             ScreenTools.defaultFontPixelHeight / 3
            color:              expandMouseArea.containsMouse ? Qt.rgba(0, 0, 0, 0.6) : Qt.rgba(0, 0, 0, 0.35)
            visible:            !_photoVideoExpanded

            Behavior on color { ColorAnimation { duration: 150 } }

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
                cursorShape:    Qt.PointingHandCursor
                onClicked:      _setExpanded(true)
            }
        }
    }
}
