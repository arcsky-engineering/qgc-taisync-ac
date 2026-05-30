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
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controllers
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.MultiVehicleManager
import QGroundControl.Palette

SettingsPage {
    property var    _settingsManager:           QGroundControl.settingsManager
    property var    _mavlinkSettings:           _settingsManager.mavlinkSettings
    property var    _appSettings:               _settingsManager.appSettings
    property bool   _disableAllDataPersistence: _appSettings.disableAllPersistence.rawValue
    property var    _activeVehicle:             QGroundControl.multiVehicleManager.activeVehicle
    property string _notConnectedStr:           qsTr("Not Connected")
    TelemetryLogManager { id: telemetryLogManager }

    // Ground Station: only the MAVLink System ID is exposed (range 245..255,
    // enforced by the fact's min/max metadata). Needed when running a second
    // GCS on the same link so the autopilot can distinguish them. "Emit
    // heartbeat" stays hidden — it must remain true for the autopilot's GCS
    // failsafe to behave correctly.
    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Ground Station")

        LabelledFactTextField {
            Layout.fillWidth:   true
            label:              qsTr("MAVLink System ID")
            fact:               _mavlinkSettings.gcsMavlinkSystemID
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("MAVLink Forwarding")

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Enable")
            fact:               _mavlinkSettings.forwardMavlink
            visible:            fact.visible
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Use TCP")
            fact:               _mavlinkSettings.forwardMavlinkByTcp
            visible:            fact.visible
            enabled:            _mavlinkSettings.forwardMavlink.rawValue
        }

        LabelledFactTextField {
            Layout.fillWidth:           true
            textFieldPreferredWidth:    ScreenTools.defaultFontPixelWidth * 20
            label:                      qsTr("Host name")
            fact:                       _mavlinkSettings.forwardMavlinkHostName
            visible:                    fact.visible
            enabled:                    _mavlinkSettings.forwardMavlink.rawValue
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Logging")
        visible:            !_disableAllDataPersistence

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Save log after each flight")
            fact:               _telemetrySave
            visible:            fact.visible
            property Fact _telemetrySave: _mavlinkSettings.telemetrySave
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Save logs even if vehicle was not armed")
            fact:               _telemetrySaveNotArmed
            visible:            fact.visible
            enabled:            _mavlinkSettings.telemetrySave.rawValue
            property Fact _telemetrySaveNotArmed: _mavlinkSettings.telemetrySaveNotArmed
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Start telemetry log on vehicle connect")
            fact:               _telemetryLogOnConnect
            visible:            fact.visible
            enabled:            _mavlinkSettings.telemetrySave.rawValue
            property Fact _telemetryLogOnConnect: _mavlinkSettings.telemetryLogOnConnect
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Save CSV log of telemetry data")
            fact:               _saveCsvTelemetry
            visible:            fact.visible
            property Fact _saveCsvTelemetry: _mavlinkSettings.saveCsvTelemetry
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Saved Telemetry Logs")
        headingDescription: telemetryLogManager.totalCount > 0
                                ? qsTr("%1 logs, %2 total").arg(telemetryLogManager.totalCount).arg(telemetryLogManager.totalSizeStr)
                                : qsTr("No logs found")
        visible:            !_disableAllDataPersistence

        Rectangle {
            Layout.fillWidth:   true
            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 14
            color:              qgcPal.window
            border.color:       qgcPal.groupBorder
            border.width:       1
            visible:            telemetryLogManager.totalCount > 0

            QGCListView {
                id:                 logListView
                anchors.fill:       parent
                anchors.margins:    ScreenTools.defaultFontPixelWidth
                clip:               true
                model:              telemetryLogManager.logFiles
                spacing:            2

                delegate: Rectangle {
                    width:  logListView.width
                    height: logEntryRow.height + ScreenTools.defaultFontPixelHeight * 0.5
                    color:  object.selected ? qgcPal.buttonHighlight : "transparent"
                    radius: ScreenTools.defaultFontPixelHeight * 0.25

                    RowLayout {
                        id:                     logEntryRow
                        anchors.left:           parent.left
                        anchors.right:          parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
                        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth
                        spacing:                ScreenTools.defaultFontPixelWidth

                        QGCCheckBox {
                            checked:    object.selected
                            onClicked:  object.selected = checked
                        }

                        QGCLabel {
                            Layout.fillWidth:   true
                            text:               object.name
                            color:              object.selected ? qgcPal.buttonHighlightText : qgcPal.text
                        }

                        QGCLabel {
                            text:   object.sizeStr
                            color:  object.selected ? qgcPal.buttonHighlightText : qgcPal.text
                        }
                    }

                    MouseArea {
                        anchors.fill:   parent
                        onClicked:      object.selected = !object.selected
                        z:              -1
                    }
                }
            }
        }

        QGCLabel {
            Layout.fillWidth:   true
            text:               qsTr("No telemetry logs found in save directory.")
            visible:            telemetryLogManager.totalCount === 0
        }

        RowLayout {
            Layout.fillWidth:   true
            spacing:            ScreenTools.defaultFontPixelWidth

            QGCButton {
                text:       qsTr("Select All")
                enabled:    telemetryLogManager.totalCount > 0
                onClicked:  telemetryLogManager.selectAll()
            }

            QGCButton {
                text:       qsTr("Select None")
                enabled:    telemetryLogManager.selectedCount > 0
                onClicked:  telemetryLogManager.selectNone()
            }

            QGCButton {
                text:       qsTr("Delete Selected")
                enabled:    telemetryLogManager.selectedCount > 0
                onClicked:  deleteConfirmDialog.open()

                MessageDialog {
                    id:         deleteConfirmDialog
                    visible:    false
                    buttons:    MessageDialog.Yes | MessageDialog.No
                    title:      qsTr("Delete Selected Logs")
                    text:       qsTr("Delete %1 selected telemetry log(s)?").arg(telemetryLogManager.selectedCount)
                    onButtonClicked: function (button, role) {
                        if (button === MessageDialog.Yes) {
                            telemetryLogManager.deleteSelected()
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true }

            QGCButton {
                text:       qsTr("Refresh")
                onClicked:  telemetryLogManager.refresh()
            }
        }

        RowLayout {
            Layout.fillWidth:   true
            spacing:            ScreenTools.defaultFontPixelWidth

            QGCButton {
                text:       telemetryLogManager.isCapturing ? qsTr("Capturing...") : qsTr("Start Capture")
                enabled:    !telemetryLogManager.isCapturing && !(_activeVehicle && _activeVehicle.armed)
                visible:    !(_activeVehicle && _activeVehicle.armed)
                onClicked:  telemetryLogManager.startCapture()
            }

            QGCButton {
                text:       qsTr("Stop & Save")
                enabled:    telemetryLogManager.isCapturing
                onClicked:  telemetryLogManager.stopAndSaveCapture()
            }
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Vehicle Logs (SD Card)")
        headingDescription: qsTr("Tap to erase log files on the vehicle. Press and hold (3 seconds) to FORMAT the SD card. Use Format only when Erase fails to clear a 'Logging Failed' state.")

        LogDownloadController { id: vehicleLogController }

        QGCLabel {
            Layout.fillWidth:   true
            wrapMode:           Text.WordWrap
            color:              qgcPal.warningText
            visible:            !_activeVehicle || _activeVehicle.armed
            text:               !_activeVehicle
                                    ? qsTr("Not connected to a vehicle.")
                                    : qsTr("Vehicle is armed. Disarm to use these commands.")
        }

        QGCButton {
            id:                 vehicleLogEraseButton
            Layout.fillWidth:   true
            enabled:            _activeVehicle && !_activeVehicle.armed && !_activeVehicle.isOfflineEditingVehicle
            text:               vehicleLogHoldTimer.holdSecondsRemaining > 0
                                    ? qsTr("Hold to FORMAT… %1").arg(vehicleLogHoldTimer.holdSecondsRemaining)
                                    : qsTr("Erase Vehicle Logs (hold to Format)")

            property bool _longPressFired: false

            Timer {
                id:         vehicleLogHoldTimer
                interval:   100
                repeat:     true
                readonly property int holdMs: 3000
                property double startedAt: 0
                property int holdSecondsRemaining: 0
                onTriggered: {
                    var elapsed = Date.now() - startedAt
                    holdSecondsRemaining = Math.max(0, Math.ceil((holdMs - elapsed) / 1000))
                    if (elapsed >= holdMs) {
                        stop()
                        holdSecondsRemaining = 0
                        vehicleLogEraseButton._longPressFired = true
                        vehicleLogFormatConfirmDialog.open()
                    }
                }
            }

            onPressed: {
                _longPressFired = false
                vehicleLogHoldTimer.startedAt = Date.now()
                vehicleLogHoldTimer.holdSecondsRemaining = 3
                vehicleLogHoldTimer.start()
            }
            onReleased: {
                vehicleLogHoldTimer.stop()
                var wasLong = _longPressFired
                vehicleLogHoldTimer.holdSecondsRemaining = 0
                if (!wasLong) {
                    vehicleLogEraseConfirmDialog.open()
                }
            }
            onCanceled: {
                vehicleLogHoldTimer.stop()
                vehicleLogHoldTimer.holdSecondsRemaining = 0
                _longPressFired = false
            }
        }

        MessageDialog {
            id:         vehicleLogEraseConfirmDialog
            buttons:    MessageDialog.Yes | MessageDialog.No
            title:      qsTr("Erase Vehicle Logs")
            text:       qsTr("Delete all log files on the vehicle's SD card? Vehicle must be disarmed.")
            onButtonClicked: function (button, role) {
                if (button === MessageDialog.Yes) {
                    vehicleLogController.eraseAll()
                }
            }
        }

        MessageDialog {
            id:         vehicleLogFormatConfirmDialog
            buttons:    MessageDialog.Yes | MessageDialog.No
            title:      qsTr("FORMAT SD Card")
            text:       qsTr("This will FORMAT the vehicle's SD card and erase ALL data permanently.\n\nUse only when normal Erase fails (e.g. 'Logging Failed' persists). The logger will reinitialise after format. Continue?")
            onButtonClicked: function (button, role) {
                if (button === MessageDialog.Yes) {
                    vehicleLogController.formatSdCard()
                }
            }
        }
    }

    // Stream Rates section removed — operators should not tune ArduPilot
    // stream rates from the GUI; the vehicle defaults are kept.

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Link Status (Current Vehicle))")

        LabelledLabel {
            Layout.fillWidth:   true
            label:              qsTr("Total messages sent (computed)")
            labelText:          _activeVehicle ? _activeVehicle.mavlinkSentCount : _notConnectedStr
        }

        LabelledLabel {
            Layout.fillWidth:   true
            label:              qsTr("Total messages received")
            labelText:          _activeVehicle ? _activeVehicle.mavlinkReceivedCount : _notConnectedStr
        }

        LabelledLabel {
            Layout.fillWidth:   true
            label:              qsTr("Total message loss")
            labelText:          _activeVehicle ? _activeVehicle.mavlinkLossCount : _notConnectedStr
        }

        LabelledLabel {
            Layout.fillWidth:   true
            label:              qsTr("Loss rate:")
            labelText:          _activeVehicle ? _activeVehicle.mavlinkLossPercent.toFixed(0) + '%' : _notConnectedStr
        }

        LabelledLabel {
            Layout.fillWidth:   true
            label:              qsTr("Signing:")
            labelText:          _activeVehicle ? (_activeVehicle.mavlinkSigning ? "On" : "Off") : _notConnectedStr
        }
    }

    // MAVLink 2 Signing section removed — not user-configurable in this build.
}
