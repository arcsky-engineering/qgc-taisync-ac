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
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.AutoPilotPlugin
import MAVLink

//-------------------------------------------------------------------------
//-- Battery Indicator
Item {
    id:             control
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          batteryIndicatorRow.width

    property bool       showIndicator:      true
    property bool       waitForParameters:  false   // UI won't show until parameters are ready
    property Component  expandedPageComponent

    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property var    _batterySettings:   QGroundControl.settingsManager.batteryIndicatorSettings
    property Fact   _indicatorDisplay:  _batterySettings.valueDisplay
    property bool   _showPercentage:    _indicatorDisplay.rawValue === 0
    property bool   _showVoltage:       _indicatorDisplay.rawValue === 1
    property bool   _showBoth:          _indicatorDisplay.rawValue === 2

    // Properties to hold the thresholds
    property int threshold1: _batterySettings.threshold1.rawValue
    property int threshold2: _batterySettings.threshold2.rawValue   

    Row {
        id:             batteryIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom

        Repeater {
            model: _activeVehicle ? _activeVehicle.batteries : 0

            Loader {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                sourceComponent:    batteryVisual

                property var battery: object
            }
        }
    }
    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorDrawer(batteryPopup, control)
        }
    }

    Component {
        id: batteryPopup

        ToolIndicatorPage {
            showExpand:         expandedComponent ? true : false
            waitForParameters:  control.waitForParameters
            contentComponent:   batteryContentComponent
            expandedComponent:  batteryExpandedComponent
        }
    }

    Component {
        id: batteryVisual

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom

            // The battery icon draws two independent signals:
            //   * fill LEVEL — always tracks percentRemaining, so the indicator
            //     keeps reflecting real capacity when a failsafe fires (the
            //     failsafe threshold is often above empty, so the pack still
            //     has plenty of margin left when LOW/CRITICAL trip).
            //   * fill COLOR — tinted orange/red on failsafe states so the
            //     operator still sees an unmistakable warning without the
            //     icon lying about how much charge remains.
            // Emergency/failed/unhealthy states are the only ones that use a
            // dedicated icon (the "!" glyph) because there the fill amount is
            // meaningless.

            function _svgFromPercent() {
                if (isNaN(battery.percentRemaining.rawValue)) return null
                if (battery.percentRemaining.rawValue > threshold1) return "/qmlimages/BatteryGreen.svg"
                if (battery.percentRemaining.rawValue > threshold2) return "/qmlimages/BatteryYellowGreen.svg"
                return "/qmlimages/BatteryYellow.svg"
            }

            function getBatteryColor() {
                // Failsafe overrides — always want the warning color even if
                // percent still has margin.
                switch (battery.chargeState.rawValue) {
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_LOW:
                        return qgcPal.colorOrange
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_CRITICAL:
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_EMERGENCY:
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_FAILED:
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_UNHEALTHY:
                        return qgcPal.colorRed
                }
                // OK (or undefined): color by remaining percent against thresholds.
                if (!isNaN(battery.percentRemaining.rawValue)) {
                    if (battery.percentRemaining.rawValue > threshold1) return qgcPal.colorGreen
                    if (battery.percentRemaining.rawValue > threshold2) return qgcPal.colorYellowGreen
                    return qgcPal.colorYellow
                }
                return qgcPal.text
            }

            function getBatterySvgSource() {
                // Emergency/failed/unhealthy use the "!" icon (fill level is
                // meaningless when the pack is in fault state).
                switch (battery.chargeState.rawValue) {
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_EMERGENCY:
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_FAILED:
                    case MAVLink.MAV_BATTERY_CHARGE_STATE_UNHEALTHY:
                        return "/qmlimages/BatteryEMERGENCY.svg"
                }
                // Every other state (including LOW / CRITICAL failsafe) shows
                // the actual percentage-based fill. Color is applied by
                // getBatteryColor() above.
                var svg = _svgFromPercent()
                if (svg) return svg
                return "/qmlimages/Battery.svg"     // fallback when percent is unknown
            }

            function getBatteryPercentageText() {
                if (!isNaN(battery.percentRemaining.rawValue)) {
                    if (battery.percentRemaining.rawValue > 98.9) {
                        return qsTr("100%")
                    } else {
                        return battery.percentRemaining.valueString + battery.percentRemaining.units
                    }
                } else if (!isNaN(battery.voltage.rawValue)) {
                    return battery.voltage.valueString + battery.voltage.units
                } else if (battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED) {
                    return battery.chargeState.enumStringValue
                }
                return qsTr("n/a")
            }

            function getBatteryVoltageText() {
                if (!isNaN(battery.voltage.rawValue)) {
                    return battery.voltage.valueString + battery.voltage.units
                } else if (battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED) {
                    return battery.chargeState.enumStringValue
                }
                return qsTr("n/a")
            }

            QGCColoredImage {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                width:              height
                sourceSize.width:   width
                source:             getBatterySvgSource()
                fillMode:           Image.PreserveAspectFit
                color:              getBatteryColor()
            }

           ColumnLayout {
                id:                     batteryInfoColumn
                anchors.top:            parent.top
                anchors.bottom:         parent.bottom
                spacing:                0

                QGCLabel {
                    Layout.alignment:       Qt.AlignHCenter
                    verticalAlignment:      Text.AlignVCenter
                    color:                  qgcPal.text
                    text:                   getBatteryPercentageText()
                    font.pointSize:         _showBoth ? ScreenTools.defaultFontPointSize : ScreenTools.mediumFontPointSize
                    visible:                _showBoth || _showPercentage
                }

                QGCLabel {
                    Layout.alignment:       Qt.AlignHCenter
                    font.pointSize:         _showBoth ? ScreenTools.defaultFontPointSize : ScreenTools.mediumFontPointSize
                    color:                  qgcPal.text
                    text:                   getBatteryVoltageText()
                    visible:                _showBoth || _showVoltage
                }
            }
        }
    }

    Component {
        id: batteryContentComponent

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight / 2

            // Compute minimum flight time across all batteries
            property string minFlightTime: {
                if (!_activeVehicle || !_activeVehicle.batteries || _activeVehicle.batteries.count === 0)
                    return ""

                var minTime = Number.MAX_VALUE
                var hasValidTime = false

                for (var i = 0; i < _activeVehicle.batteries.count; i++) {
                    var battery = _activeVehicle.batteries.get(i)
                    if (battery && !isNaN(battery.timeRemaining.rawValue)) {
                        hasValidTime = true
                        if (battery.timeRemaining.rawValue < minTime) {
                            minTime = battery.timeRemaining.rawValue
                        }
                    }
                }

                if (!hasValidTime)
                    return ""

                // Use the timeRemainingStr from the battery with minimum time
                for (var j = 0; j < _activeVehicle.batteries.count; j++) {
                    var batt = _activeVehicle.batteries.get(j)
                    if (batt && batt.timeRemaining.rawValue === minTime) {
                        return batt.timeRemainingStr.value
                    }
                }
                return ""
            }

            // Flight Time section at top (independent of individual batteries)
            SettingsGroupLayout {
                heading:        qsTr("Flight Time")
                contentSpacing: 0
                showDividers:   false
                visible:        minFlightTime !== ""

                LabelledLabel {
                    label:      qsTr("Remaining")
                    labelText:  minFlightTime
                }
            }

            Component {
                id: batteryValuesAvailableComponent

                QtObject {
                    property bool functionAvailable:         battery.function.rawValue !== MAVLink.MAV_BATTERY_FUNCTION_UNKNOWN
                    property bool showFunction:              functionAvailable && battery.function.rawValue != MAVLink.MAV_BATTERY_FUNCTION_ALL
                    property bool temperatureAvailable:      !isNaN(battery.temperature.rawValue)
                    property bool currentAvailable:          !isNaN(battery.current.rawValue)
                    property bool percentRemainingAvailable: !isNaN(battery.percentRemaining.rawValue)
                }
            }

            Repeater {
                model: _activeVehicle ? _activeVehicle.batteries : 0

                SettingsGroupLayout {
                    heading:        qsTr("Battery %1").arg(_activeVehicle.batteries.length === 1 ? qsTr("Status") : object.id.rawValue)
                    contentSpacing: 0
                    showDividers:   false

                    property var batteryValuesAvailable: batteryValuesAvailableLoader.item

                    Loader {
                        id:                 batteryValuesAvailableLoader
                        sourceComponent:    batteryValuesAvailableComponent

                        property var battery: object
                    }

                    LabelledLabel {
                        label:      qsTr("Remaining")
                        labelText:  object.percentRemaining.valueString + " " + object.percentRemaining.units
                        visible:    batteryValuesAvailable.percentRemainingAvailable
                    }

                    LabelledLabel {
                        label:      qsTr("Voltage")
                        labelText:  object.voltage.valueString + " " + object.voltage.units
                    }

                    LabelledLabel {
                        label:      qsTr("Temperature")
                        labelText:  object.temperature.valueString + " " + object.temperature.units
                        visible:    batteryValuesAvailable.temperatureAvailable
                    }

                    LabelledLabel {
                        label:      qsTr("Function")
                        labelText:  object.function.enumStringValue
                        visible:    batteryValuesAvailable.showFunction
                    }
                }
            }
        }
    }

    Component {
        id: batteryExpandedComponent

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight / 2

            FactPanelController { id: controller }

            SettingsGroupLayout {
                heading:            qsTr("Battery Display")
                Layout.fillWidth:   true

                LabelledFactComboBox {
                    id:             editModeCheckBox
                    label:          qsTr("Value")
                    fact:           _fact
                    visible:        _fact,visible

                    property Fact _fact: QGroundControl.settingsManager.batteryIndicatorSettings.valueDisplay
                }

                // ColumnLayout {
                //     QGCLabel { text: qsTr("Coloring") }

                //     RowLayout {
                //         spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Reduced spacing between elements

                //         // Battery 100%
                //         RowLayout {
                //             spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Tighter spacing for icon and label
                //             QGCColoredImage {
                //                 source: "/qmlimages/BatteryGreen.svg"
                //                 width: ScreenTools.defaultFontPixelWidth * 6
                //                 height: width
                //                 fillMode: Image.PreserveAspectFit
                //                 color: qgcPal.colorGreen
                //             }
                //             QGCLabel { text: qsTr("100%") }
                //         }

                //         // Threshold 1
                //         RowLayout {
                //             spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Tighter spacing for icon and field
                //             QGCColoredImage {
                //                 source: "/qmlimages/BatteryYellowGreen.svg"
                //                 width: ScreenTools.defaultFontPixelWidth * 6
                //                 height: width
                //                 fillMode: Image.PreserveAspectFit
                //                 color: qgcPal.colorYellowGreen
                //             }
                //             FactTextField {
                //                 id: threshold1Field
                //                 fact: _batterySettings.threshold1
                //                 implicitWidth: ScreenTools.defaultFontPixelWidth * 6
                //                 height: ScreenTools.defaultFontPixelHeight * 1.5
                //                 enabled: fact.visible
                //                 onEditingFinished: {
                //                     // Validate and set the new threshold value
                //                     _batterySettings.setThreshold1(parseInt(text));
                //                 }
                //             }
                //         }

                //         // Threshold 2
                //         RowLayout {
                //             spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Tighter spacing for icon and field
                //             QGCColoredImage {
                //                 source: "/qmlimages/BatteryYellow.svg"
                //                 width: ScreenTools.defaultFontPixelWidth * 6
                //                 height: width
                //                 fillMode: Image.PreserveAspectFit
                //                 color: qgcPal.colorYellow
                //             }
                //             FactTextField {
                //                 fact: _batterySettings.threshold2
                //                 implicitWidth: ScreenTools.defaultFontPixelWidth * 6
                //                 height: ScreenTools.defaultFontPixelHeight * 1.5
                //                 enabled: fact.visible
                //                 onEditingFinished: {
                //                     // Validate and set the new threshold value
                //                     _batterySettings.setThreshold2(parseInt(text));                                
                //                 }
                //             }
                //         }

                //         // Low state
                //         RowLayout {
                //             spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Tighter spacing for icon and label
                //             QGCColoredImage {
                //                 source: "/qmlimages/BatteryOrange.svg"
                //                 width: ScreenTools.defaultFontPixelWidth * 6
                //                 height: width
                //                 fillMode: Image.PreserveAspectFit
                //                 color: qgcPal.colorOrange
                //             }
                //             QGCLabel { text: qsTr("Low") }
                //         }

                //         // Critical state
                //         RowLayout {
                //             spacing: ScreenTools.defaultFontPixelWidth * 0.05  // Tighter spacing for icon and label
                //             QGCColoredImage {
                //                 source: "/qmlimages/BatteryCritical.svg"
                //                 width: ScreenTools.defaultFontPixelWidth * 6
                //                 height: width
                //                 fillMode: Image.PreserveAspectFit
                //                 color: qgcPal.colorRed
                //             }
                //             QGCLabel { text: qsTr("Critical") }
                //         }
                //     }
                // }
            } // end of settings group layout

            // Loader {
            //     Layout.fillWidth: true
            //     sourceComponent: expandedPageComponent
            // }

            // SettingsGroupLayout {
            //     visible: _activeVehicle.autopilotPlugin.knownVehicleComponentAvailable(AutoPilotPlugin.KnownPowerVehicleComponent) &&
            //                 QGroundControl.corePlugin.showAdvancedUI

            //     LabelledButton {
            //         label:      qsTr("Vehicle Power")
            //         buttonText: qsTr("Configure")

            //         onClicked: {
            //             mainWindow.showKnownVehicleComponentConfigPage(AutoPilotPlugin.KnownPowerVehicleComponent)
            //             mainWindow.closeIndicatorDrawer()
            //         }
            //     }                
            // }
        }
    }
}
