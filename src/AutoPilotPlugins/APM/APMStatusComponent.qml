/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.ScreenTools

// Read-only status page. Most values arrive from periodic "~" telemetry, so rows
// show a dash until the first message (up to ~2 min after connect).
//
// Layout: Aircraft and Payload in the left column; batteries + interface board (Xplorer) or the
// hybrid module (X55) stacked in the right column. The Flow wraps the right column
// below the left one when the window is too narrow for both.
SetupPage {
    id:             statusPage
    pageComponent:  statusPageComponent

    Component {
        id: statusPageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:    _margins

            FactPanelController { id: controller }

            readonly property real  _margins:       ScreenTools.defaultFontPixelHeight
            readonly property real  _groupWidth:    ScreenTools.defaultFontPixelWidth * 45
            readonly property bool  _isXplorer:     QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
            readonly property var   _vehicle:       controller.vehicle
            readonly property var   _info:          _vehicle ? _vehicle.systemInfo : null
            readonly property string _noValue:      "—"

            property Fact _statRuntime: controller.getParameterFact(-1, "STAT_RUNTIME", false /* reportMissing */)
            property Fact _statBootCnt: controller.getParameterFact(-1, "STAT_BOOTCNT", false /* reportMissing */)

            // STAT_* are updated by the firmware, so re-read them each time the page opens
            Component.onCompleted: {
                controller.refreshParameter(-1, "STAT_RUNTIME")
                controller.refreshParameter(-1, "STAT_BOOTCNT")
            }

            function _text(fact) {
                if (!fact) {
                    return _noValue
                }
                var value = fact.rawValue
                return (value === undefined || value === null || value === "") ? _noValue : value
            }

            function _number(fact, decimals, suffix) {
                if (!fact || fact.rawValue === undefined || fact.rawValue === null || fact.rawValue === "" || isNaN(fact.rawValue)) {
                    return _noValue
                }
                return Number(fact.rawValue).toFixed(decimals) + (suffix ? " " + suffix : "")
            }

            // Left column
            Column {
                spacing: _margins

                SettingsGroupLayout {
                    heading:    qsTr("Aircraft")
                    width:      _groupWidth

                    LabelledLabel {
                        label:      qsTr("Model")
                        labelText:  _text(_info ? _info.model : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Serial Number")
                        labelText:  _text(_info ? _info.serialNumber : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Firmware")
                        labelText:  _text(_info ? _info.firmwareVersion : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Flight Hours")
                        labelText:  _number(_info ? _info.flightHours : null, 1, qsTr("hrs"))
                    }
                    LabelledLabel {
                        label:      qsTr("Total On Time")
                        labelText:  _statRuntime ? (_statRuntime.rawValue / 3600).toFixed(1) + " " + qsTr("hrs") : _noValue
                    }
                    LabelledLabel {
                        label:      qsTr("Total Boot Count")
                        labelText:  _statBootCnt ? _statBootCnt.rawValue : _noValue
                    }
                }

                SettingsGroupLayout {
                    heading:    qsTr("Payload")
                    width:      _groupWidth

                    LabelledLabel {
                        label:      qsTr("Gimbal Firmware")
                        labelText:  _text(_info ? _info.gimbalFirmwareVersion : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Camera Model")
                        labelText:  _text(_info ? _info.cameraModel : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Camera Control Firmware")
                        labelText:  _text(_info ? _info.cameraFirmwareVersion : null)
                    }
                }
            }

            // Right column
            Column {
                spacing: _margins

                Repeater {
                    model: _isXplorer ? 2 : 0

                    SettingsGroupLayout {
                        heading:    qsTr("Battery %1").arg(_battery)
                        width:      _groupWidth

                        readonly property int  _battery:        index + 1
                        readonly property var  _installedFact:  _info ? _info["battery" + _battery + "Installed"] : null
                        // Bay reported empty by the firmware (see VehicleSystemInfoFactGroup ~BAT parsing)
                        readonly property bool _notInstalled:   _installedFact !== null && _installedFact.rawValue === 0

                        LabelledLabel {
                            label:      qsTr("Serial Number")
                            labelText:  _notInstalled ? qsTr("Not installed") : _text(_info ? _info["battery" + _battery + "Serial"] : null)
                        }
                        LabelledLabel {
                            label:      qsTr("Firmware")
                            labelText:  _text(_info ? _info["battery" + _battery + "FirmwareVersion"] : null)
                        }
                        LabelledLabel {
                            label:      qsTr("Cycle Count")
                            labelText:  _number(_info ? _info["battery" + _battery + "Cycles"] : null, 0, "")
                        }
                    }
                }

                SettingsGroupLayout {
                    heading:    qsTr("Battery Interface Board")
                    width:      _groupWidth
                    visible:    _isXplorer

                    LabelledLabel {
                        label:      qsTr("Firmware")
                        labelText:  _text(_info ? _info.batteryInterfaceFirmwareVersion : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Hardware")
                        labelText:  _text(_info ? _info.batteryInterfaceHardwareVersion : null)
                    }
                }

                SettingsGroupLayout {
                    heading:    qsTr("Hybrid Module")
                    width:      _groupWidth
                    visible:    !_isXplorer && _info && _info.hybridSerial.rawValue !== ""

                    LabelledLabel {
                        label:      qsTr("Serial Number")
                        labelText:  _text(_info ? _info.hybridSerial : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Hardware")
                        labelText:  _text(_info ? _info.hybridHardwareVersion : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Firmware")
                        labelText:  _text(_info ? _info.hybridFirmwareVersion : null)
                    }
                    LabelledLabel {
                        label:      qsTr("Runtime")
                        labelText:  _number(_info ? _info.hybridRuntimeHours : null, 1, qsTr("hrs"))
                    }
                    LabelledLabel {
                        label:      qsTr("Until Maintenance")
                        labelText:  _number(_info ? _info.hybridMaintenanceHours : null, 1, qsTr("hrs"))
                    }
                }
            }
        }
    }
}
