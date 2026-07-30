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
import QGroundControl.FactControls
import QGroundControl.Palette
import QGroundControl.Controls
import QGroundControl.ScreenTools

SetupPage {
    id:             safetyPage
    pageComponent:  safetyPageComponent

    // Unit conversion helpers for cm <-> user preferred vertical distance units
    property var _unitsConversion: QGroundControl.unitsConversion

    function cmToDisplayUnits(cm) {
        var meters = cm / 100.0
        return _unitsConversion.metersToAppSettingsVerticalDistanceUnits(meters)
    }

    function displayUnitsToCm(displayValue) {
        var meters = _unitsConversion.appSettingsVerticalDistanceUnitsToMeters(displayValue)
        return meters * 100.0
    }

    // Xplorer uses percentage-based failsafe thresholds via custom ArduPilot
    // parameters BATT*_LOW_SOC and BATT*_CRT_SOC (INT8, 0-100 %, 0=disabled).
    // The old voltage<->percent conversion helpers are no longer needed —
    // the FactTextField writes the SOC parameter directly.

    Component {
        id: safetyPageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:    _margins

            FactPanelController { id: controller; }

            QGCPalette { id: ggcPal; colorGroupEnabled: true }

            // Xplorer fork: route battery instance labels by vehicleVariant. See APMPowerComponent.qml.
            readonly property bool _isXplorer:              QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
            readonly property string _battPrefix1:          _isXplorer ? "BATT2" : "BATT"
            readonly property string _battPrefix2:          _isXplorer ? "BATT3" : "BATT2"

            property Fact _batt1Monitor:                    controller.getParameterFact(-1, _battPrefix1 + "_MONITOR")
            property Fact _batt2Monitor:                    controller.getParameterFact(-1, _battPrefix2 + "_MONITOR", false /* reportMissing */)
            // On X55 only one user-facing battery exists (BATT_); Battery 2 is always
            // suppressed even if the firmware happens to expose BATT2_* params.
            property bool _batt2MonitorAvailable:           _isXplorer && controller.parameterExists(-1, _battPrefix2 + "_MONITOR")
            property bool _batt1MonitorEnabled:             _batt1Monitor.rawValue !== 0
            property bool _batt2MonitorEnabled:             _batt2MonitorAvailable ? _batt2Monitor.rawValue !== 0 : false
            property bool _batt1ParamsAvailable:            controller.parameterExists(-1, _battPrefix1 + "_CAPACITY")
            property bool _batt2ParamsAvailable:            controller.parameterExists(-1, _battPrefix2 + "_CAPACITY")

            property Fact _failsafeBatt1LowAct:             controller.getParameterFact(-1, _battPrefix1 + "_FS_LOW_ACT", false /* reportMissing */)
            property Fact _failsafeBatt2LowAct:             controller.getParameterFact(-1, _battPrefix2 + "_FS_LOW_ACT", false /* reportMissing */)
            property Fact _failsafeBatt1CritAct:            controller.getParameterFact(-1, _battPrefix1 + "_FS_CRT_ACT", false /* reportMissing */)
            property Fact _failsafeBatt2CritAct:            controller.getParameterFact(-1, _battPrefix2 + "_FS_CRT_ACT", false /* reportMissing */)
            property Fact _failsafeBatt1LowMah:             controller.getParameterFact(-1, _battPrefix1 + "_LOW_MAH", false /* reportMissing */)
            property Fact _failsafeBatt2LowMah:             controller.getParameterFact(-1, _battPrefix2 + "_LOW_MAH", false /* reportMissing */)
            property Fact _failsafeBatt1CritMah:            controller.getParameterFact(-1, _battPrefix1 + "_CRT_MAH", false /* reportMissing */)
            property Fact _failsafeBatt2CritMah:            controller.getParameterFact(-1, _battPrefix2 + "_CRT_MAH", false /* reportMissing */)
            property Fact _failsafeBatt1LowVoltage:         controller.getParameterFact(-1, _battPrefix1 + "_LOW_VOLT", false /* reportMissing */)
            property Fact _failsafeBatt2LowVoltage:         controller.getParameterFact(-1, _battPrefix2 + "_LOW_VOLT", false /* reportMissing */)
            property Fact _failsafeBatt1CritVoltage:        controller.getParameterFact(-1, _battPrefix1 + "_CRT_VOLT", false /* reportMissing */)
            property Fact _failsafeBatt2CritVoltage:        controller.getParameterFact(-1, _battPrefix2 + "_CRT_VOLT", false /* reportMissing */)
            // Minimum arming voltage — surfaced here (under the failsafe thresholds)
            // for Xplorer since the Power tab is hidden. As a percentage, full range.
            property Fact _failsafeBatt1ArmVoltage:         controller.getParameterFact(-1, _battPrefix1 + "_ARM_VOLT", false /* reportMissing */)
            property Fact _failsafeBatt2ArmVoltage:         controller.getParameterFact(-1, _battPrefix2 + "_ARM_VOLT", false /* reportMissing */)

            // Xplorer-only: percentage-of-charge failsafe thresholds. Backed
            // by our custom ArduPilot params BATT*_LOW_SOC / BATT*_CRT_SOC /
            // BATT*_ARM_SOC (INT8, 0-100 %, 0 disables the threshold).
            // The ARM_SOC facts may be null on older firmware — the UI falls
            // back to arming voltage in that case.
            property Fact _failsafeBatt1LowSoc:             controller.getParameterFact(-1, _battPrefix1 + "_LOW_SOC", false /* reportMissing */)
            property Fact _failsafeBatt2LowSoc:             controller.getParameterFact(-1, _battPrefix2 + "_LOW_SOC", false /* reportMissing */)
            property Fact _failsafeBatt1CritSoc:            controller.getParameterFact(-1, _battPrefix1 + "_CRT_SOC", false /* reportMissing */)
            property Fact _failsafeBatt2CritSoc:            controller.getParameterFact(-1, _battPrefix2 + "_CRT_SOC", false /* reportMissing */)
            property Fact _failsafeBatt1ArmSoc:             controller.getParameterFact(-1, _battPrefix1 + "_ARM_SOC", false /* reportMissing */)
            property Fact _failsafeBatt2ArmSoc:             controller.getParameterFact(-1, _battPrefix2 + "_ARM_SOC", false /* reportMissing */)

            property Fact _armingCheck: controller.getParameterFact(-1, "ARMING_CHECK")

            property real _margins:         ScreenTools.defaultFontPixelHeight
            property real _innerMargin:     _margins / 2
            property bool _showIcon:        !ScreenTools.isTinyScreen
            property bool _roverFirmware:   controller.parameterExists(-1, "MODE1") // This catches all usage of ArduRover firmware vehicle types: Rover, Boat...


            property string _restartRequired: qsTr("Requires vehicle reboot")

            Component {
                id: batteryFailsafeComponent

                Column {
                    spacing: _margins

                    // Xplorer: parallel-pack mirroring. When the primary fact
                    // changes (either user edit or PARAM_VALUE load), copy to
                    // the secondary pack so BATT2/BATT3 stay in lockstep. If
                    // they're already equal, Fact's setter is a no-op so no
                    // spurious PARAM_SET goes out.
                    Connections {
                        target:  failsafeBattLowAct
                        enabled: !!mirrorBattLowAct
                        function onRawValueChanged() { mirrorBattLowAct.rawValue = failsafeBattLowAct.rawValue }
                    }
                    Connections {
                        target:  failsafeBattCritAct
                        enabled: !!mirrorBattCritAct
                        function onRawValueChanged() { mirrorBattCritAct.rawValue = failsafeBattCritAct.rawValue }
                    }
                    Connections {
                        target:  failsafeBattLowVoltage
                        enabled: !!mirrorBattLowVoltage
                        function onRawValueChanged() { mirrorBattLowVoltage.rawValue = failsafeBattLowVoltage.rawValue }
                    }
                    Connections {
                        target:  failsafeBattCritVoltage
                        enabled: !!mirrorBattCritVoltage
                        function onRawValueChanged() { mirrorBattCritVoltage.rawValue = failsafeBattCritVoltage.rawValue }
                    }
                    Connections {
                        target:  failsafeBattArmVoltage
                        enabled: !!mirrorBattArmVoltage
                        function onRawValueChanged() { mirrorBattArmVoltage.rawValue = failsafeBattArmVoltage.rawValue }
                    }
                    Connections {
                        target:  failsafeBattLowSoc
                        enabled: !!mirrorBattLowSoc
                        function onRawValueChanged() { mirrorBattLowSoc.rawValue = failsafeBattLowSoc.rawValue }
                    }
                    Connections {
                        target:  failsafeBattCritSoc
                        enabled: !!mirrorBattCritSoc
                        function onRawValueChanged() { mirrorBattCritSoc.rawValue = failsafeBattCritSoc.rawValue }
                    }
                    Connections {
                        target:  failsafeBattArmSoc
                        enabled: !!mirrorBattArmSoc
                        function onRawValueChanged() { mirrorBattArmSoc.rawValue = failsafeBattArmSoc.rawValue }
                    }

                    GridLayout {
                        id:             gridLayout
                        columnSpacing:  _margins
                        rowSpacing:     _margins
                        columns:        2
                        QGCLabel { text: qsTr("Low action:") }
                        FactComboBox {
                            fact:               failsafeBattLowAct
                            indexModel:         false
                            Layout.fillWidth:   true
                        }

                        QGCLabel { text: qsTr("Critical action:") }
                        FactComboBox {
                            fact:               failsafeBattCritAct
                            indexModel:         false
                            Layout.fillWidth:   true
                        }

                        // ── X55: voltage thresholds ──
                        QGCLabel {
                            text:       qsTr("Low voltage threshold:")
                            visible:    !_isXplorer
                        }
                        FactTextField {
                            fact:               failsafeBattLowVoltage
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            !_isXplorer
                        }

                        QGCLabel {
                            text:       qsTr("Critical voltage threshold:")
                            visible:    !_isXplorer
                        }
                        FactTextField {
                            fact:               failsafeBattCritVoltage
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            !_isXplorer
                        }

                        // ── Xplorer: percentage-of-charge thresholds ──
                        // Directly write BATT*_LOW_SOC / BATT*_CRT_SOC. Mirroring
                        // to the parallel pack (BATT3_*) happens in the
                        // Connections blocks above.
                        QGCLabel {
                            text:       qsTr("Low battery percentage:")
                            visible:    _isXplorer && failsafeBattLowSoc
                        }
                        FactTextField {
                            fact:               failsafeBattLowSoc
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            _isXplorer && failsafeBattLowSoc
                        }

                        QGCLabel {
                            text:       qsTr("Critical battery percentage:")
                            visible:    _isXplorer && failsafeBattCritSoc
                        }
                        FactTextField {
                            fact:               failsafeBattCritSoc
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            _isXplorer && failsafeBattCritSoc
                        }

                        // Minimum arming threshold — surfaced on the Safety tab
                        // for Xplorer (Power tab is hidden for that variant).
                        // Prefer BATT*_ARM_SOC (percentage) when the firmware
                        // exposes it; fall back to BATT*_ARM_VOLT (voltage)
                        // otherwise. X55 keeps its arming input on the Power tab.
                        QGCLabel {
                            text:       qsTr("Minimum arming percentage:")
                            visible:    _isXplorer && failsafeBattArmSoc
                        }
                        FactTextField {
                            fact:               failsafeBattArmSoc
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            _isXplorer && failsafeBattArmSoc
                        }

                        QGCLabel {
                            text:       qsTr("Minimum arming voltage:")
                            visible:    _isXplorer && !failsafeBattArmSoc && failsafeBattArmVoltage
                        }
                        FactTextField {
                            fact:               failsafeBattArmVoltage
                            showUnits:          true
                            Layout.fillWidth:   true
                            visible:            _isXplorer && !failsafeBattArmSoc && failsafeBattArmVoltage
                        }
                    } // GridLayout
                } // Column
            }

            Component {
                id: restartRequiredComponent

                ColumnLayout {
                    spacing: ScreenTools.defaultFontPixelWidth

                    QGCLabel {
                        text: _restartRequired
                    }

                    QGCButton {
                        text:       qsTr("Reboot vehicle")
                        onClicked:  controller.vehicle.rebootVehicle()
                    }
                }
            }

            Column {
                spacing: _margins / 2
                visible: _batt1MonitorEnabled

                QGCLabel {
                    text:       _isXplorer ? qsTr("Battery Failsafe Settings") : qsTr("Battery Failsafe Triggers")
                    font.bold:   true
                }

                Rectangle {
                    width:  battery1FailsafeLoader.x + battery1FailsafeLoader.width + _margins
                    height: battery1FailsafeLoader.y + battery1FailsafeLoader.height + _margins
                    color:  ggcPal.windowShade

                    Loader {
                        id:                 battery1FailsafeLoader
                        anchors.margins:    _margins
                        anchors.top:        parent.top
                        anchors.left:       parent.left
                        sourceComponent:    _batt1ParamsAvailable ? batteryFailsafeComponent : restartRequiredComponent

                        property Fact battMonitor:              _batt1Monitor
                        property bool battParamsAvailable:      _batt1ParamsAvailable
                        property Fact failsafeBattLowAct:       _failsafeBatt1LowAct
                        property Fact failsafeBattCritAct:      _failsafeBatt1CritAct
                        property Fact failsafeBattLowMah:       _failsafeBatt1LowMah
                        property Fact failsafeBattCritMah:      _failsafeBatt1CritMah
                        property Fact failsafeBattLowVoltage:   _failsafeBatt1LowVoltage
                        property Fact failsafeBattCritVoltage:  _failsafeBatt1CritVoltage
                        property Fact failsafeBattArmVoltage:   _failsafeBatt1ArmVoltage
                        property Fact failsafeBattLowSoc:       _failsafeBatt1LowSoc
                        property Fact failsafeBattCritSoc:      _failsafeBatt1CritSoc
                        property Fact failsafeBattArmSoc:       _failsafeBatt1ArmSoc
                        // Xplorer: BATT2/BATT3 packs run in parallel, so the user
                        // edits one set of values and we mirror writes to the
                        // second pack. Null for non-Xplorer (no mirroring).
                        property Fact mirrorBattLowAct:         _isXplorer ? _failsafeBatt2LowAct      : null
                        property Fact mirrorBattCritAct:        _isXplorer ? _failsafeBatt2CritAct     : null
                        property Fact mirrorBattLowVoltage:     _isXplorer ? _failsafeBatt2LowVoltage  : null
                        property Fact mirrorBattCritVoltage:    _isXplorer ? _failsafeBatt2CritVoltage : null
                        property Fact mirrorBattArmVoltage:     _isXplorer ? _failsafeBatt2ArmVoltage  : null
                        property Fact mirrorBattLowSoc:         _isXplorer ? _failsafeBatt2LowSoc      : null
                        property Fact mirrorBattCritSoc:        _isXplorer ? _failsafeBatt2CritSoc     : null
                        property Fact mirrorBattArmSoc:         _isXplorer ? _failsafeBatt2ArmSoc      : null
                    }
                } // Rectangle
            } // Column - Battery Failsafe Settings


            Column {
                spacing: _margins / 2
                visible: _batt2MonitorEnabled && !_isXplorer

                QGCLabel {
                    text:       qsTr("Battery 2 Failsafe Triggers")
                    font.bold:   true
                }

                Rectangle {
                    width:  battery2FailsafeLoader.x + battery2FailsafeLoader.width + _margins
                    height: battery2FailsafeLoader.y + battery2FailsafeLoader.height + _margins
                    color:  ggcPal.windowShade

                    Loader {
                        id:                 battery2FailsafeLoader
                        anchors.margins:    _margins
                        anchors.top:        parent.top
                        anchors.left:       parent.left
                        sourceComponent:    _batt2ParamsAvailable ? batteryFailsafeComponent : restartRequiredComponent

                        property Fact battMonitor:              _batt2Monitor
                        property bool battParamsAvailable:      _batt2ParamsAvailable
                        property Fact failsafeBattLowAct:       _failsafeBatt2LowAct
                        property Fact failsafeBattCritAct:      _failsafeBatt2CritAct
                        property Fact failsafeBattLowMah:       _failsafeBatt2LowMah
                        property Fact failsafeBattCritMah:      _failsafeBatt2CritMah
                        property Fact failsafeBattLowVoltage:   _failsafeBatt2LowVoltage
                        property Fact failsafeBattCritVoltage:  _failsafeBatt2CritVoltage
                        property Fact failsafeBattArmVoltage:   _failsafeBatt2ArmVoltage
                        property Fact failsafeBattLowSoc:       _failsafeBatt2LowSoc
                        property Fact failsafeBattCritSoc:      _failsafeBatt2CritSoc
                        property Fact failsafeBattArmSoc:       _failsafeBatt2ArmSoc
                        // Mirror properties intentionally null on this loader.
                        // The shared batteryFailsafeComponent references these
                        // names in Connections blocks; they need to resolve
                        // here even though nothing is mirrored from the
                        // second battery.
                        property Fact mirrorBattLowAct:         null
                        property Fact mirrorBattCritAct:        null
                        property Fact mirrorBattLowVoltage:     null
                        property Fact mirrorBattCritVoltage:    null
                        property Fact mirrorBattArmVoltage:     null
                        property Fact mirrorBattLowSoc:         null
                        property Fact mirrorBattCritSoc:        null
                        property Fact mirrorBattArmSoc:         null
                    }
                } // Rectangle
            } // Column - Battery Failsafe Settings

            Component {
                id: planeGeneralFS

                Column {
                    spacing: _margins / 2

                    property Fact _failsafeThrEnable:   controller.getParameterFact(-1, "THR_FAILSAFE")
                    property Fact _failsafeThrValue:    controller.getParameterFact(-1, "THR_FS_VALUE")
                    property Fact _failsafeGCSEnable:   controller.getParameterFact(-1, "FS_GCS_ENABL")

                    QGCLabel {
                        text:       qsTr("Failsafe Triggers")
                        font.bold:   true
                    }

                    Rectangle {
                        width:  fsColumn.x + fsColumn.width + _margins
                        height: fsColumn.y + fsColumn.height + _margins
                        color:  qgcPal.windowShade

                        ColumnLayout {
                            id:                 fsColumn
                            anchors.margins:    _margins
                            anchors.left:       parent.left
                            anchors.top:        parent.top

                            RowLayout {
                                QGCCheckBox {
                                    id:                 throttleEnableCheckBox
                                    text:               qsTr("Throttle PWM threshold:")
                                    checked:            _failsafeThrEnable.value === 1

                                    onClicked: _failsafeThrEnable.value = (checked ? 1 : 0)
                                }

                                FactTextField {
                                    fact:               _failsafeThrValue
                                    showUnits:          true
                                    enabled:            throttleEnableCheckBox.checked
                                }
                            }

                            QGCCheckBox {
                                text:       qsTr("GCS failsafe")
                                checked:    _failsafeGCSEnable.value != 0
                                onClicked:  _failsafeGCSEnable.value = checked ? 1 : 0
                            }
                        }
                    } // Rectangle - Failsafe trigger settings
                } // Column - Failsafe trigger settings
            }

            Loader {
                sourceComponent: controller.vehicle.fixedWing ? planeGeneralFS : undefined
            }

            Component {
                id: roverGeneralFS

                Column {
                    spacing: _margins / 2

                    property Fact _failsafeGCSEnable:   controller.getParameterFact(-1, "FS_GCS_ENABLE")
                    property Fact _failsafeThrEnable:   controller.getParameterFact(-1, "FS_THR_ENABLE")
                    property Fact _failsafeThrValue:    controller.getParameterFact(-1, "FS_THR_VALUE")
                    property Fact _failsafeAction:      controller.getParameterFact(-1, "FS_ACTION")
                    property Fact _failsafeCrashCheck:  controller.getParameterFact(-1, "FS_CRASH_CHECK")

                    QGCLabel {
                        id:         failsafeLabel
                        text:       qsTr("Failsafe Triggers")
                        font.bold:   true
                    }

                    Rectangle {
                        id:     failsafeSettings
                        width:  fsGrid.x + fsGrid.width + _margins
                        height: fsGrid.y + fsGrid.height + _margins
                        color:  ggcPal.windowShade

                        GridLayout {
                            id:                 fsGrid
                            anchors.margins:    _margins
                            anchors.left:       parent.left
                            anchors.top:        parent.top
                            columns:            2

                            QGCLabel { text: qsTr("Ground Station failsafe:") }
                            FactComboBox {
                                Layout.fillWidth:   true
                                fact:               _failsafeGCSEnable
                                indexModel:         false
                            }

                            QGCLabel { text: qsTr("Radio failsafe:") }
                            FactComboBox {
                                Layout.fillWidth:   true
                                fact:               _failsafeThrEnable
                                indexModel:         false
                            }

                            QGCLabel { text: qsTr("PWM threshold:") }
                            FactTextField {
                                Layout.fillWidth:   true
                                fact:               _failsafeThrValue
                            }

                            QGCLabel { text: qsTr("Failsafe Crash Check:") }
                            FactComboBox {
                                Layout.fillWidth:   true
                                fact:               _failsafeCrashCheck
                                indexModel:         false
                            }
                        }
                    } // Rectangle - Failsafe Settings
                } // Column - Failsafe Settings
            }

            Loader {
                sourceComponent: _roverFirmware ? roverGeneralFS : undefined
            }

            Component {
                id: copterGeneralFS

                Column {
                    spacing: _margins / 2

                    property Fact _failsafeGCSEnable:               controller.getParameterFact(-1, "FS_GCS_ENABLE")
                    property Fact _failsafeBattLowAct:              controller.getParameterFact(-1, "r." + _battPrefix1 + "_FS_LOW_ACT", false /* reportMissing */)
                    property Fact _failsafeBattMah:                 controller.getParameterFact(-1, "r." + _battPrefix1 + "_LOW_MAH", false /* reportMissing */)
                    property Fact _failsafeBattVoltage:             controller.getParameterFact(-1, "r." + _battPrefix1 + "_LOW_VOLT", false /* reportMissing */)
                    property Fact _failsafeThrEnable:               controller.getParameterFact(-1, "FS_THR_ENABLE")
                    property Fact _failsafeThrValue:                controller.getParameterFact(-1, "FS_THR_VALUE")
                    property Fact _failsafeOptions:                 controller.getParameterFact(-1, "FS_OPTIONS")

                    // "Continue with Mission in Auto Mode" is driven by FS_OPTIONS, not the legacy
                    // FS_THR_ENABLE=2 / FS_GCS_ENABLE=2 enum values (removed in firmware 4.0+).
                    //   bit 0 (1)  = Continue if in Auto on RC/throttle failsafe
                    //   bit 1 (2)  = Continue if in Auto on GCS failsafe
                    //   bit 3 (8)  = Continue if landing on any failsafe (kept as the baseline)
                    // Selecting "Continue" on either combo writes the combined value 11; any other
                    // action writes the 8 baseline. The two continue options are therefore coupled.
                    readonly property int _fsOptionsBaseline:       8
                    readonly property int _fsOptionsContinueAuto:   11

                    // Fixed width so the closed combobox (and therefore this whole
                    // General Failsafe group) keeps a stable width regardless of the
                    // current selection — otherwise the group reflows in the parent
                    // Flow layout. Sized for the realistic options; the removed-in-4.0
                    // GCS option is ~70 chars and not selectable on current firmware.
                    // Declared on the component root so the comboboxes can see it.
                    readonly property real _failsafeComboWidth: ScreenTools.defaultFontPixelWidth * 34

                    QGCLabel {
                        text:       qsTr("General Failsafe Triggers")
                        font.bold:   true
                    }

                    Rectangle {
                        width:  generalFailsafeColumn.x + generalFailsafeColumn.width + _margins
                        height: generalFailsafeColumn.y + generalFailsafeColumn.height + _margins
                        color:  ggcPal.windowShade

                        Column {
                            id:                 generalFailsafeColumn
                            anchors.margins:    _margins
                            anchors.top:        parent.top
                            anchors.left:       parent.left
                            spacing:            _margins

                            GridLayout {
                                columnSpacing:  _margins
                                rowSpacing:     _margins
                                columns:        2

                                QGCLabel { text: qsTr("Ground Station failsafe:") }
                                QGCComboBox {
                                    model:                  [qsTr("Disabled/NoAction"), qsTr("RTL"),
                                        qsTr("Continue with Mission in Auto Mode"), qsTr("Land")]
                                    // If GCS failsafe is disabled it stays "Disabled" regardless of the
                                    // FS_OPTIONS continue bit (which the radio failsafe may have set via 11).
                                    // Otherwise: Continue in Auto is FS_OPTIONS bit 1; else map by FS_GCS_ENABLE.
                                    currentIndex:           (_failsafeGCSEnable.value === 0) ? 0 :
                                                                ((_failsafeOptions.rawValue & 2) ? 2 :
                                                                    (_failsafeGCSEnable.value === 5 ? 3 : 1))
                                    Layout.preferredWidth:  _failsafeComboWidth

                                    onActivated: (index) => {
                                        switch (index) {
                                        case 0: // Disabled/NoAction
                                            _failsafeGCSEnable.value = 0
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        case 1: // RTL
                                            _failsafeGCSEnable.value = 1
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        case 2: // Continue with Mission in Auto Mode
                                            _failsafeGCSEnable.value = 1
                                            _failsafeOptions.rawValue = _fsOptionsContinueAuto
                                            break
                                        case 3: // Land
                                            _failsafeGCSEnable.value = 5
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        }
                                    }
                                }

                                QGCLabel { text: qsTr("Radio failsafe:") }
                                QGCComboBox {
                                    model:                  [qsTr("Disabled"), qsTr("Always RTL"),
                                        qsTr("Continue with Mission in Auto Mode"), qsTr("Always Land")]
                                    // Continue in Auto is FS_OPTIONS bit 0; otherwise map by FS_THR_ENABLE.
                                    currentIndex:           (_failsafeOptions.rawValue & 1) ? 2 :
                                                                (_failsafeThrEnable.value === 0 ? 0 :
                                                                    (_failsafeThrEnable.value === 3 ? 3 : 1))
                                    Layout.preferredWidth:  _failsafeComboWidth

                                    onActivated: (index) => {
                                        switch (index) {
                                        case 0: // Disabled
                                            _failsafeThrEnable.value = 0
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        case 1: // Always RTL
                                            _failsafeThrEnable.value = 1
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        case 2: // Continue with Mission in Auto Mode
                                            _failsafeThrEnable.value = 1
                                            _failsafeOptions.rawValue = _fsOptionsContinueAuto
                                            break
                                        case 3: // Always Land
                                            _failsafeThrEnable.value = 3
                                            _failsafeOptions.rawValue = _fsOptionsBaseline
                                            break
                                        }
                                    }
                                }

                                // QGCLabel { text: qsTr("PWM threshold:") }
                                // FactTextField {
                                //     fact:               _failsafeThrValue
                                //     showUnits:          true
                                //     Layout.fillWidth:   true
                                // }
                            } // GridLayout
                        } // Column
                    } // Rectangle - Failsafe Settings
                } // Column - General Failsafe Settings
            }

            Loader {
                sourceComponent: controller.vehicle.multiRotor ? copterGeneralFS : undefined
            }

            Component {
                id: copterGeoFence

                Column {
                    spacing: _margins / 2

                    property Fact _fenceAction: controller.getParameterFact(-1, "FENCE_ACTION")
                    property Fact _fenceAltMax: controller.getParameterFact(-1, "FENCE_ALT_MAX")
                    property Fact _fenceEnable: controller.getParameterFact(-1, "FENCE_ENABLE")
                    property Fact _fenceMargin: controller.getParameterFact(-1, "FENCE_MARGIN")
                    property Fact _fenceRadius: controller.getParameterFact(-1, "FENCE_RADIUS")
                    property Fact _fenceType:   controller.getParameterFact(-1, "FENCE_TYPE")

                    readonly property int _maxAltitudeFenceBitMask: 1
                    readonly property int _circleFenceBitMask:      2
                    readonly property int _polygonFenceBitMask:     4

                    QGCLabel {
                        text:           qsTr("GeoFence")
                        font.bold:      true
                    }

                    Rectangle {
                        width:  mainLayout.width + (_margins * 2)
                        height: mainLayout.height + (_margins * 2)
                        color:  ggcPal.windowShade

                        ColumnLayout {
                            id:         mainLayout
                            x:          _margins
                            y:          _margins
                            spacing:    ScreenTools.defaultFontPixellHeight / 2

                            FactCheckBox {
                                id:     enabledCheckBox
                                text:   qsTr("Enabled")
                                fact:   _fenceEnable
                            }

                            GridLayout {
                                columns:    2
                                enabled:    enabledCheckBox.checked

                                QGCCheckBox {
                                    text:       qsTr("Maximum Altitude")
                                    checked:    _fenceType.rawValue & _maxAltitudeFenceBitMask

                                    onClicked: {
                                        if (checked) {
                                            _fenceType.rawValue |= _maxAltitudeFenceBitMask
                                        } else {
                                            _fenceType.value &= ~_maxAltitudeFenceBitMask
                                        }
                                    }
                                }

                                FactTextField {
                                    fact: _fenceAltMax
                                }

                                QGCCheckBox {
                                    text:       qsTr("Circle centered on Home")
                                    checked:    _fenceType.rawValue & _circleFenceBitMask

                                    onClicked: {
                                        if (checked) {
                                            _fenceType.rawValue |= _circleFenceBitMask
                                        } else {
                                            _fenceType.value &= ~_circleFenceBitMask
                                        }
                                    }
                                }

                                FactTextField {
                                    fact:       _fenceRadius
                                    showUnits:  true
                                }

                                QGCCheckBox {
                                    text:       qsTr("Inclusion/Exclusion Circles+Polygons")
                                    checked:    _fenceType.rawValue & _polygonFenceBitMask

                                    onClicked: {
                                        if (checked) {
                                            _fenceType.rawValue |= _polygonFenceBitMask
                                        } else {
                                            _fenceType.value &= ~_polygonFenceBitMask
                                        }
                                    }
                                }

                                Item {
                                    height: 1
                                    width:  1
                                }
                            } // GridLayout

                            Item {
                                height: 1
                                width:  1
                            }

                            GridLayout {
                                columns: 2
                                enabled: enabledCheckBox.checked

                                QGCLabel {
                                    text: qsTr("Breach action")
                                }

                                FactComboBox {
                                    sizeToContents: true
                                    fact:           _fenceAction
                                }

                                QGCLabel {
                                    text: qsTr("Fence margin")
                                }

                                FactTextField {
                                    fact: _fenceMargin
                                }
                            }
                        }
                    } // Rectangle - GeoFence Settings
                } // Column - GeoFence Settings
            }

            Loader {
                width: flowLayout.width
                sourceComponent: controller.vehicle.multiRotor ? copterRTL : undefined
            }

            Loader {
                visible: false  // Hide GeoFence section
                width: flowLayout.width
                sourceComponent: controller.vehicle.multiRotor ? copterGeoFence : undefined
            }

            Component {
                id: copterRTL

                Column {
                    spacing: _margins / 2

                    property Fact _landSpeedFact:   controller.getParameterFact(-1, "LAND_SPEED")
                    property Fact _rtlAltFact:      controller.getParameterFact(-1, "RTL_ALT")
                    property Fact _rtlLoitTimeFact: controller.getParameterFact(-1, "RTL_LOIT_TIME")
                    property Fact _rtlAltFinalFact: controller.getParameterFact(-1, "RTL_ALT_FINAL")

                    QGCLabel {
                        id:             rtlLabel
                        text:           qsTr("Return to Launch")
                        font.bold:      true
                    }

                    Rectangle {
                        id:     rtlSettings
                        width:  rltAltField.x + rltAltField.width + _margins
                        height: Math.max(returnAltRadio.y + returnAltRadio.height, icon.height + icon.anchors.margins) + _margins
                        color:  ggcPal.windowShade

                        QGCColoredImage {
                            id:                 icon
                            visible:            _showIcon
                            anchors.margins:    _margins
                            anchors.left:       parent.left
                            anchors.top:        parent.top
                            height:             ScreenTools.defaultFontPixelWidth * 20
                            width:              ScreenTools.defaultFontPixelWidth * 20
                            color:              ggcPal.text
                            sourceSize.width:   width
                            mipmap:             true
                            fillMode:           Image.PreserveAspectFit
                            source:             "/qmlimages/ReturnToHomeAltitude.svg"
                        }

                        QGCRadioButton {
                            id:                 returnAtCurrentRadio
                            anchors.margins:    _innerMargin
                            anchors.left:       _showIcon ? icon.right : parent.left
                            anchors.top:        parent.top
                            text:               qsTr("Return at current altitude")
                            checked:            _rtlAltFact.value == 0

                            onClicked: _rtlAltFact.value = 0
                        }

                        QGCRadioButton {
                            id:                 returnAltRadio
                            anchors.topMargin:  _innerMargin
                            anchors.top:        returnAtCurrentRadio.bottom
                            anchors.left:       returnAtCurrentRadio.left
                            text:               qsTr("Return at specified altitude (%1):").arg(_unitsConversion.appSettingsVerticalDistanceUnitsString)
                            checked:            _rtlAltFact.value != 0

                            onClicked: _rtlAltFact.value = 1500
                        }

                        QGCTextField {
                            id: rltAltField
                            anchors.leftMargin: _margins
                            anchors.left: returnAltRadio.right
                            anchors.baseline: returnAltRadio.baseline
                            text: _rtlAltFact ? cmToDisplayUnits(_rtlAltFact.value).toFixed(1) : "--"
                            enabled: returnAltRadio.checked
                            inputMethodHints: Qt.ImhFormattedNumbersOnly

                            onEditingFinished: {
                                if (!_rtlAltFact) return
                                var value = parseFloat(text)
                                if (isNaN(value)) value = 0
                                // Clamp in display units (equivalent to 0-300m)
                                var maxInDisplayUnits = _unitsConversion.metersToAppSettingsVerticalDistanceUnits(300)
                                value = Math.max(0, Math.min(maxInDisplayUnits, value))
                                // Write back in cm
                                _rtlAltFact.value = Math.round(displayUnitsToCm(value))
                                text = value.toFixed(1)
                            }

                            Connections {
                                target: _rtlAltFact
                                onValueChanged: rltAltField.text = _rtlAltFact ? cmToDisplayUnits(_rtlAltFact.value).toFixed(1) : "--"
                            }
                        }
                    } // Rectangle - RTL Settings
                } // Column - RTL Settings
            }

            Component {
                id: planeRTL

                Column {
                    spacing: _margins / 2

                    property Fact _rtlAltFact: {
                        if (controller.firmwareMajorVersion < 4 || (controller.firmwareMajorVersion === 4 && controller.firmwareMinorVersion < 5)) {
                            return controller.getParameterFact(-1, "ALT_HOLD_RTL")
                        } else {
                            return controller.getParameterFact(-1, "RTL_ALTITUDE")
                        }
                    }

                    QGCLabel {
                        text:           qsTr("Return to Launch")
                        font.bold:      true
                    }

                    Rectangle {
                        width:  rltAltField.x + rltAltField.width + _margins
                        height: rltAltField.y + rltAltField.height + _margins
                        color:  qgcPal.windowShade

                        QGCRadioButton {
                            id:                 returnAtCurrentRadio
                            anchors.margins:    _margins
                            anchors.left:       parent.left
                            anchors.top:        parent.top
                            text:               qsTr("Return at current altitude")
                            checked:            _rtlAltFact.value < 0

                            onClicked: _rtlAltFact.value = -1
                        }

                        QGCRadioButton {
                            id:                 returnAltRadio
                            anchors.topMargin:  _margins / 2
                            anchors.left:       returnAtCurrentRadio.left
                            anchors.top:        returnAtCurrentRadio.bottom
                            text:               qsTr("Return at specified altitude:")
                            checked:            _rtlAltFact.value >= 0

                            onClicked: _rtlAltFact.value = 10000
                        }

                        FactTextField {
                            id:                 rltAltField
                            anchors.leftMargin: _margins
                            anchors.left:       returnAltRadio.right
                            anchors.baseline:   returnAltRadio.baseline
                            fact:               _rtlAltFact
                            showUnits:          true
                            enabled:            returnAltRadio.checked
                        }
                    } // Rectangle - RTL Settings
                } // Column - RTL Settings
            }

            Loader {
                sourceComponent: controller.vehicle.fixedWing ? planeRTL : undefined
            }

            // Column {
            //     spacing: _margins / 2

            //     QGCLabel {
            //         text:           qsTr("Arming Checks")
            //         font.bold:      true
            //     }

            //     Rectangle {
            //         width:  flowLayout.width
            //         height: armingCheckInnerColumn.height + (_margins * 2)
            //         color:  ggcPal.windowShade

            //         Column {
            //             id:                 armingCheckInnerColumn
            //             anchors.margins:    _margins
            //             anchors.top:        parent.top
            //             anchors.left:       parent.left
            //             anchors.right:      parent.right
            //             spacing: _margins

            //             FactBitmask {
            //                 id:                 armingCheckBitmask
            //                 anchors.left:       parent.left
            //                 anchors.right:      parent.right
            //                 firstEntryIsAll:    true
            //                 fact:               _armingCheck
            //             }

            //             QGCLabel {
            //                 id:             armingCheckWarning
            //                 anchors.left:   parent.left
            //                 anchors.right:  parent.right
            //                 wrapMode:       Text.WordWrap
            //                 color:          qgcPal.warningText
            //                 text:            qsTr("Warning: Turning off arming checks can lead to loss of Vehicle control.")
            //                 visible:        _armingCheck.value != 1
            //             }
            //         }
            //     } // Rectangle - Arming checks
            // } // Column - Arming Checks
        } // Flow
    } // Component - safetyPageComponent
} // SetupView
