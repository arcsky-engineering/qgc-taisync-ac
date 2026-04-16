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
    property bool   _isAPM:                     _activeVehicle ? _activeVehicle.apmFirmware : true
    property bool   _showAPMStreamRates:        QGroundControl.apmFirmwareSupported && _settingsManager.apmMavlinkStreamRateSettings.visible && _isAPM
    property var    _apmStartMavlinkStreams:    _mavlinkSettings.apmStartMavlinkStreams

    TelemetryLogManager { id: telemetryLogManager }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Ground Station")

        LabelledFactTextField {
            Layout.fillWidth:   true
            label:              qsTr("MAVLink System ID")
            fact:               _mavlinkSettings.gcsMavlinkSystemID
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Emit heartbeat")
            fact:               _mavlinkSettings.sendGCSHeartbeat
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
        heading:            qsTr("Stream Rates (ArduPilot Only)")
        visible:            _showAPMStreamRates

        QGCCheckBoxSlider {
            id:                 controllerByVehicleCheckBox
            Layout.fillWidth:   true
            text:               qsTr("Controlled By vehicle")
            checked:            !_apmStartMavlinkStreams.rawValue
            onClicked:          _apmStartMavlinkStreams.rawValue = !checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Raw Sensors")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateRawSensors
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Extended Status")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateExtendedStatus
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("RC Channels")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateRCChannels
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Position")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRatePosition
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Extra 1")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateExtra1
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Extra 2")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateExtra2
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Extra 3")
            fact:               _settingsManager.apmMavlinkStreamRateSettings.streamRateExtra3
            indexModel:         false
            enabled:            !controllerByVehicleCheckBox.checked
        }
    }

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

    SettingsGroupLayout {
        id:                 mavlink2SigningGroup
        Layout.fillWidth:   true
        heading:            qsTr("MAVLink 2 Signing")
        headingDescription: qsTr("Signing keys should only be sent to the vehicle over secure links.")
        visible:            _mavlink2SigningKey.visible

        property Fact _mavlink2SigningKey: _mavlinkSettings.mavlink2SigningKey

        Connections {
            target:             mavlink2SigningGroup._mavlink2SigningKey
            onRawValueChanged:  sendToVehiclePrompt.visible = true
        }

        RowLayout {
            spacing: ScreenTools.defaultFontPixelWidth

            LabelledFactTextField {
                Layout.fillWidth:           true
                textFieldPreferredWidth:    ScreenTools.defaultFontPixelWidth * 32
                label:                      qsTr("Key")
                fact:                       mavlink2SigningGroup._mavlink2SigningKey
            }

            QGCButton {
                text:       qsTr("Send to Vehicle")
                enabled:    _activeVehicle

                onClicked: {
                    sendToVehiclePrompt.visible = false
                    _activeVehicle.sendSetupSigning()
                }
            }
        }

        QGCLabel {
            id:                 sendToVehiclePrompt
            Layout.fillWidth:   true
            text:               qsTr("Signing key has changed. Don't forget to send to Vehicle(s) if needed.")
            visible:            false
        }
    }
}
