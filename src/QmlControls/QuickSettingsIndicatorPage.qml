/****************************************************************************
 *
 * (c) 2009-2025 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * Quick Settings Indicator Page
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls

ToolIndicatorPage {
    showExpand: false

    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var _unitsConversion: QGroundControl.unitsConversion
    FactPanelController { id: controller }

    property Fact _wpnavSpeedFact:    controller.getParameterFact(-1, "WPNAV_SPEED",    false)
    property Fact _rtlAltFact:        controller.getParameterFact(-1, "RTL_ALT",        false)
    property Fact _rtlSpeedFact:      controller.getParameterFact(-1, "RTL_SPEED",      false)
    property Fact _loitSpeedFact:     controller.getParameterFact(-1, "LOIT_SPEED",     false)
    property Fact _camOptionsFact:    controller.getParameterFact(-1, "CAM1_OPTIONS",   false)
    property Fact _autoRsmClimbFact:  controller.getParameterFact(-1, "AUTO_RSM_CLIMB", false)
    property Fact _misRestartFact:    controller.getParameterFact(-1, "MIS_RESTART",    false)
    property Fact _showMissionOnMap:  QGroundControl.settingsManager.flyViewSettings.showMissionOnMap

    // Convenience derived properties for binding. Using top-level properties (not
    // inline expressions inside the checkbox bindings) so the checkbox's `checked`
    // binding stays in sync with external param changes after the user clicks it.
    property bool _cam3DEnabled:      !!_camOptionsFact && (_camOptionsFact.rawValue & 2) !== 0
    property bool _autoRsmEnabled:    !!_autoRsmClimbFact && _autoRsmClimbFact.rawValue !== 0
    property bool _misRestartEnabled: !!_misRestartFact && _misRestartFact.rawValue !== 0
    property bool _useAutoRtlSpeed:   !!_rtlSpeedFact && _rtlSpeedFact.rawValue === 0

    // Xplorer-only toggles. CAM1_OPTIONS bit 1 (3D distance for camera trigger)
    // and AUTO_RSM_CLIMB (climb to altitude when resuming mission) are Xplorer
    // firmware behaviors that aren't present / aren't supported on the X55 build.
    readonly property bool _isXplorer:
        QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1

    // Helper: is LOIT_SPEED currently set to the given preset (cm/s)?
    function _loitSpeedIs(target) {
        return !!_loitSpeedFact && _loitSpeedFact.rawValue === target
    }

    // Helper functions for unit conversion (cm <-> user preferred vertical distance units)
    function cmToDisplayUnits(cm) {
        var meters = cm / 100.0
        return _unitsConversion.metersToAppSettingsVerticalDistanceUnits(meters)
    }

    function displayUnitsToCm(displayValue) {
        var meters = _unitsConversion.appSettingsVerticalDistanceUnitsToMeters(displayValue)
        return meters * 100.0
    }

    function cmpsToDisplaySpeed(cmps) {
        return _unitsConversion.metersSecondToAppSettingsSpeedUnits(cmps / 100.0)
    }

    function displaySpeedToCmps(displayValue) {
        return _unitsConversion.appSettingsSpeedUnitsToMetersSecond(displayValue) * 100.0
    }

    contentComponent: Component {
        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight / 2

            // Manual mission download — useful when "Auto-load mission on connect"
            // is disabled, or to refresh the displayed mission without reconnecting.
            // Routes through Vehicle.reloadMissionFromVehicle() because the Fly
            // view's PlanMasterController.loadFromVehicle() is a no-op by design.
            SettingsGroupLayout {
                heading: qsTr("Mission")
                visible: activeVehicle

                QGCCheckBox {
                    Layout.fillWidth: true
                    text:             qsTr("Show mission on map")
                    checked:          _showMissionOnMap.rawValue
                    onClicked:        _showMissionOnMap.rawValue = checked
                }

                QGCButton {
                    Layout.fillWidth: true
                    text:             qsTr("Download Mission From Vehicle")
                    enabled:          !!activeVehicle
                    onClicked: {
                        if (activeVehicle) {
                            activeVehicle.reloadMissionFromVehicle()
                        }
                    }
                }

                QGCButton {
                    Layout.fillWidth: true
                    text:             qsTr("Clear Mission on Vehicle")
                    enabled:          !!activeVehicle && !!globals.planMasterControllerFlyView
                    onClicked: {
                        mainWindow.showMessageDialog(
                            qsTr("Clear Mission"),
                            qsTr("Are you sure you want to remove all mission items and clear the mission from the vehicle?"),
                            Dialog.Yes | Dialog.Cancel,
                            function() {
                                if (globals.planMasterControllerFlyView) {
                                    globals.planMasterControllerFlyView.removeAllFromVehicle()
                                }
                            })
                    }
                }
            }

            // Mission download progress. Visible only while a download is
            // active (loadProgress is set by Vehicle::_gotProgressUpdate during
            // the manual reload). Closing the drawer to see the toolbar bar is
            // an option too; this gives in-drawer feedback if it's still open.
            SettingsGroupLayout {
                heading: qsTr("Mission download")
                visible: activeVehicle && activeVehicle.loadProgress > 0 && activeVehicle.loadProgress < 1

                ProgressBar {
                    Layout.fillWidth: true
                    from:             0
                    to:               1
                    value:            activeVehicle ? activeVehicle.loadProgress : 0
                }

                QGCLabel {
                    Layout.fillWidth:   true
                    horizontalAlignment: Text.AlignHCenter
                    text:               activeVehicle ? qsTr("%1%").arg(Math.round(activeVehicle.loadProgress * 100)) : ""
                }
            }

            // RTL altitude and speed — grouped together at the top.
            SettingsGroupLayout {
                heading: qsTr("Return to Launch")
                visible: activeVehicle && (!!_rtlAltFact || !!_rtlSpeedFact)

                RowLayout {
                    Layout.fillWidth: true
                    visible: !!_rtlAltFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("RTL Altitude (%1)").arg(_unitsConversion.appSettingsVerticalDistanceUnitsString)
                    }

                    QGCTextField {
                        id: rtlAltField
                        text: _rtlAltFact ? cmToDisplayUnits(_rtlAltFact.value).toFixed(1) : "--"
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 10

                        onEditingFinished: {
                            if (!_rtlAltFact) return
                            var value = parseFloat(text)
                            if (isNaN(value)) value = 0
                            var maxInDisplayUnits = _unitsConversion.metersToAppSettingsVerticalDistanceUnits(300)
                            value = Math.max(0, Math.min(maxInDisplayUnits, value))
                            _rtlAltFact.value = Math.round(displayUnitsToCm(value))
                            rtlAltField.text = value.toFixed(1)
                        }

                        Connections {
                            target: _rtlAltFact
                            onValueChanged: {
                                rtlAltField.text = _rtlAltFact ? cmToDisplayUnits(_rtlAltFact.value).toFixed(1) : "--"
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: !!_rtlSpeedFact && !_useAutoRtlSpeed

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("RTL Speed (%1)").arg(_unitsConversion.appSettingsSpeedUnitsString)
                    }

                    QGCTextField {
                        id: rtlSpeedField
                        text: _rtlSpeedFact && _rtlSpeedFact.rawValue > 0
                              ? cmpsToDisplaySpeed(_rtlSpeedFact.rawValue).toFixed(1) : "--"
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 10

                        onEditingFinished: {
                            if (!_rtlSpeedFact) return
                            var value = parseFloat(text)
                            if (isNaN(value)) value = 2.0
                            var mps = _unitsConversion.appSettingsSpeedUnitsToMetersSecond(value)
                            mps = Math.max(2.0, Math.min(14.0, mps))
                            _rtlSpeedFact.rawValue = Math.round(mps * 100)
                            rtlSpeedField.text = _unitsConversion.metersSecondToAppSettingsSpeedUnits(mps).toFixed(1)
                        }

                        Connections {
                            target: _rtlSpeedFact
                            onValueChanged: {
                                if (_rtlSpeedFact && _rtlSpeedFact.rawValue > 0)
                                    rtlSpeedField.text = cmpsToDisplaySpeed(_rtlSpeedFact.rawValue).toFixed(1)
                            }
                        }
                    }
                }

                QGCCheckBox {
                    text:    qsTr("Use Auto Flight Speed for RTL Speed")
                    visible: !!_rtlSpeedFact
                    checked: _useAutoRtlSpeed
                    onClicked: {
                        if (!_rtlSpeedFact) return
                        if (checked) {
                            _rtlSpeedFact.rawValue = 0
                        } else {
                            _rtlSpeedFact.rawValue = 600
                        }
                    }
                }
            }

            // Manual Flight Speed presets — three buttons that write directly
            // to LOIT_SPEED (cm/s). The active preset (if the current value
            // matches one exactly) is highlighted green. Hidden if the param
            // isn't present on the connected vehicle.
            SettingsGroupLayout {
                heading: qsTr("Manual Flight Speed")
                visible: activeVehicle && !!_loitSpeedFact

                RowLayout {
                    Layout.fillWidth: true
                    spacing:          ScreenTools.defaultFontPixelWidth

                    QGCButton {
                        text:               qsTr("Slow")
                        Layout.fillWidth:   true
                        backgroundColor:    _loitSpeedIs(400)  ? "green" : "gray"
                        onClicked:          { if (_loitSpeedFact) _loitSpeedFact.rawValue = 400 }
                    }
                    QGCButton {
                        text:               qsTr("Normal")
                        Layout.fillWidth:   true
                        backgroundColor:    _loitSpeedIs(800)  ? "green" : "gray"
                        onClicked:          { if (_loitSpeedFact) _loitSpeedFact.rawValue = 800 }
                    }
                    QGCButton {
                        text:               qsTr("Fast")
                        Layout.fillWidth:   true
                        backgroundColor:    _loitSpeedIs(1200) ? "green" : "gray"
                        onClicked:          { if (_loitSpeedFact) _loitSpeedFact.rawValue = 1200 }
                    }
                }

                // Compact display of the actual value, so non-preset values
                // (e.g. set elsewhere to 750) are still visible.
                QGCLabel {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    opacity:          0.7
                    font.pointSize:   ScreenTools.smallFontPointSize
                    text:             _loitSpeedFact
                                      ? qsTr("Current: %1 m/s").arg((_loitSpeedFact.rawValue / 100).toFixed(1))
                                      : ""
                }
            }

            // Auto Settings — a couple of one-shot toggles that aren't worth a
            // full section each. Both are simple booleans on top of their
            // respective parameters; the camera one masks bit 1 of CAM1_OPTIONS.
            SettingsGroupLayout {
                heading: qsTr("Auto Settings")
                visible: activeVehicle && (!!_wpnavSpeedFact
                                           || !!_camOptionsFact
                                           || !!_autoRsmClimbFact
                                           || !!_misRestartFact)

                // Auto Flight Speed — was "Waypoint Speed" in the prior layout.
                // Same WPNAV_SPEED param, just relocated and renamed to fit
                // alongside the other Auto-mode behaviors.
                RowLayout {
                    Layout.fillWidth: true
                    visible:          !!_wpnavSpeedFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Auto Flight Speed (m/s)")
                    }

                    QGCTextField {
                        id: wpnavSpeedField
                        text: _wpnavSpeedFact ? (_wpnavSpeedFact.value / 100).toFixed(1) : "--"
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 10

                        onEditingFinished: {
                            if (!_wpnavSpeedFact) return
                            var value = parseFloat(text)
                            if (isNaN(value)) value = 0
                            // Clamp value in display units
                            value = Math.max(0.5, Math.min(20.0, value))
                            // Write back in internal units (cm/s)
                            _wpnavSpeedFact.value = Math.round(value * 100)
                            // Update text to the clamped value
                            wpnavSpeedField.text = (value).toFixed(1)
                        }

                        Connections {
                            target: _wpnavSpeedFact
                            onValueChanged: {
                                var val = _wpnavSpeedFact ? (_wpnavSpeedFact.value / 100) : 0
                                wpnavSpeedField.text = val.toFixed(1)
                            }
                        }
                    }
                }

                QGCCheckBox {
                    text:     qsTr("Use 3D distance for camera trigger")
                    visible:  !!_camOptionsFact && _isXplorer
                    checked:  _cam3DEnabled
                    onClicked: {
                        if (!_camOptionsFact) return
                        // Set/clear bit 1 while preserving all other bits.
                        var v = _camOptionsFact.rawValue
                        _camOptionsFact.rawValue = checked ? (v | 2) : (v & ~2)
                    }
                }

                // Mission re-entry behavior. Default (unchecked, MIS_RESTART=0) is
                // "resume from last command run". Checked (MIS_RESTART=1) restarts
                // the mission from the beginning every time Auto is entered.
                QGCCheckBox {
                    text:     qsTr("Restart mission from beginning")
                    visible:  !!_misRestartFact
                    checked:  _misRestartEnabled
                    onClicked: {
                        if (_misRestartFact) _misRestartFact.rawValue = checked ? 1 : 0
                    }
                }

                // Only meaningful when resuming (MIS_RESTART=0). Dimmed but still
                // clickable when restart is selected — the param has no effect
                // there but the UI doesn't prevent users from configuring it.
                QGCCheckBox {
                    text:     qsTr("Climb to altitude when resuming mission")
                    visible:  !!_autoRsmClimbFact && _isXplorer
                    checked:  _autoRsmEnabled
                    opacity:  _misRestartEnabled ? 0.5 : 1.0
                    onClicked: {
                        if (_autoRsmClimbFact) _autoRsmClimbFact.rawValue = checked ? 1 : 0
                    }
                }
            }

        }
    }
}
