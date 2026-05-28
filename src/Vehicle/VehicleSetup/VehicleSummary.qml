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
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactSystem
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.MultiVehicleManager
import QGroundControl.Palette

// Xplorer fork: the original tile-based summary showed mostly inert technical
// detail (channel numbers, sensor IDs, monitor types, etc.) — none of which was
// actionable for an end user. This is now a simple navigation grid: one large
// button per setup section that takes you directly to that section's editor.
// Calibration state is surfaced in the sidebar via the status dot on the
// "Sensors" button (see ConfigButton.qml).
Rectangle {
    id:             _summaryRoot
    anchors.fill:   parent
    color:          qgcPal.window

    property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var _components: _vehicle ? _vehicle.autopilotPlugin.vehicleComponents : []

    // Xplorer (variant 1) hides the Power tab — and therefore its summary tile.
    // The arming threshold moves to Safety. X55 keeps the full tile set.
    readonly property bool _isXplorer: QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
    readonly property var _tileNames: _isXplorer
                                        ? [ "Radio", "RC Options", "Sensors", "Safety" ]
                                        : [ "Radio", "RC Options", "Sensors", "Power", "Safety" ]

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    function componentByName(n) {
        for (var i = 0; i < _components.length; i++) {
            if (_components[i].name === n) return _components[i]
        }
        return null
    }

    // Top-level banner driven by the autopilot plugin's setupComplete flag.
    // This still surfaces "needs setup" if any component reports incomplete —
    // useful for catching a sensors-needs-calibration condition.
    property bool _setupComplete: _vehicle ? _vehicle.autopilotPlugin.setupComplete : true

    ColumnLayout {
        anchors.fill:           parent
        anchors.margins:        ScreenTools.defaultFontPixelWidth * 2
        spacing:                ScreenTools.defaultFontPixelHeight

        QGCLabel {
            Layout.fillWidth:       true
            wrapMode:               Text.WordWrap
            color:                  _setupComplete ? qgcPal.text : qgcPal.warningText
            font.bold:              true
            horizontalAlignment:    Text.AlignHCenter
            text:                   _setupComplete
                                      ? qsTr("Select a setup section below or from the menu on the left.")
                                      : qsTr("WARNING: Your vehicle requires setup prior to flight. Please resolve the items marked in red.")
        }

        GridLayout {
            Layout.alignment:   Qt.AlignHCenter
            columns:            3
            rowSpacing:         ScreenTools.defaultFontPixelHeight
            columnSpacing:      ScreenTools.defaultFontPixelWidth * 2

            // Vehicle-component tiles. Power is omitted on Xplorer (see _tileNames).
            Repeater {
                model: _tileNames

                SettingsButton {
                    id:                     tileButton
                    Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 22
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 4
                    text:                   modelData
                    icon.source:            _comp ? _comp.iconResource : ""
                    enabled:                _comp !== null

                    property var _comp: componentByName(modelData)

                    // Override SettingsButton background: rim Rectangle (always
                    // visible) wraps a fill Rectangle (opacity-controlled by
                    // hover/pressed/checked, matching stock SettingsButton feel).
                    background: Rectangle {
                        color:          "transparent"
                        radius:         ScreenTools.defaultFontPixelWidth / 2
                        border.color:   "white"
                        border.width:   1

                        Rectangle {
                            anchors.fill:       parent
                            anchors.margins:    parent.border.width
                            color:              qgcPal.buttonHighlight
                            radius:             parent.radius - parent.border.width
                            opacity:            tileButton.checked || tileButton.pressed
                                                    ? 1
                                                    : (tileButton.enabled && tileButton.hovered ? 0.2 : 0)
                        }
                    }

                    // Center the icon + label within the tile (the stock
                    // SettingsButton layout left-aligns them, which looks off
                    // on a large square tile).
                    contentItem: Item {
                        Row {
                            anchors.centerIn:   parent
                            spacing:            ScreenTools.defaultFontPixelWidth

                            QGCColoredImage {
                                source:                 tileButton.icon.source
                                color:                  tileButton.icon.color
                                width:                  ScreenTools.defaultFontPixelHeight
                                height:                 ScreenTools.defaultFontPixelHeight
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            QGCLabel {
                                text:                   tileButton.text
                                color:                  tileButton.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }

                    // Health dot: only show for components whose setupComplete signal
                    // actually varies (Sensors). Others would render a permanent
                    // green dot which is just visual noise.
                    Rectangle {
                        visible:                tileButton._comp && tileButton.text === "Sensors"
                        width:                  ScreenTools.defaultFontPixelWidth * 1.1
                        height:                 width
                        radius:                 width / 2
                        color:                  tileButton._comp && tileButton._comp.setupComplete ? "#00d932" : "red"
                        border.color:           "white"
                        border.width:           1
                        anchors.right:          parent.right
                        anchors.top:            parent.top
                        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth * 0.4
                        anchors.topMargin:      ScreenTools.defaultFontPixelWidth * 0.4
                    }

                    onClicked: {
                        if (_comp) {
                            setupView.showVehicleComponentPanel(_comp)
                        }
                    }
                }
            }

            // Parameters tile uses a different navigation path.
            SettingsButton {
                id:                     paramsTile
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 22
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 4
                text:                   qsTr("Parameters")
                icon.source:            "/qmlimages/subMenuButtonImage.png"

                background: Rectangle {
                    color:          "transparent"
                    radius:         ScreenTools.defaultFontPixelWidth / 2
                    border.color:   "white"
                    border.width:   1

                    Rectangle {
                        anchors.fill:       parent
                        anchors.margins:    parent.border.width
                        color:              qgcPal.buttonHighlight
                        radius:             parent.radius - parent.border.width
                        opacity:            paramsTile.checked || paramsTile.pressed
                                                ? 1
                                                : (paramsTile.enabled && paramsTile.hovered ? 0.2 : 0)
                    }
                }

                contentItem: Item {
                    Row {
                        anchors.centerIn:   parent
                        spacing:            ScreenTools.defaultFontPixelWidth

                        QGCColoredImage {
                            source:                 paramsTile.icon.source
                            color:                  paramsTile.icon.color
                            width:                  ScreenTools.defaultFontPixelHeight
                            height:                 ScreenTools.defaultFontPixelHeight
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        QGCLabel {
                            text:                   paramsTile.text
                            color:                  paramsTile.textColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }

                onClicked:              setupView.showParametersPanel()
            }
        }

        // Filler so the grid stays near the top.
        Item { Layout.fillHeight: true; Layout.fillWidth: true }
    }
}
