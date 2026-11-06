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
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls

Item {
    id:             control
    width:          gimbalIndicatorIcon.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property var    activeVehicle:          QGroundControl.multiVehicleManager.activeVehicle
    property var    gimbalController:       activeVehicle.gimbalController
    property int    _selectedPayload:       QGroundControl.settingsManager.flyViewSettings.payloadSelection.value
    property bool   showIndicator:          gimbalController && gimbalController.gimbals.count && _selectedPayload !== 2
    property var    activeGimbal:           gimbalController.activeGimbal
    property bool   joystickButtonsAvailable: activeVehicle.joystickEnabled

    property var    margins:                ScreenTools.defaultFontPixelWidth
    property var    panelRadius:            ScreenTools.defaultFontPixelWidth * 0.5
    property var    buttonHeight:           height * 1.6
    property var    squareButtonPadding:    ScreenTools.defaultFontPixelWidth
    property var    separatorHeight:        buttonHeight * 0.9
    property var    settingsPanelVisible:   false
    property bool   tuningPanelVisible:     false   // ILX "Speed/Smooth" section, collapsed by default

    // One labelled  -  value  +  row in the ILX Speed/Smooth section. Steps snap to multiples of 5
    // within from..to. currentValue is the value last read back (-1 = unknown, row disabled).
    // Taps are collected and commit() fires once after a short pause, so tapping + several
    // times sends one param set instead of one per tap.
    component TuningStepperRow: RowLayout {
        id:         tuningRow
        spacing:    margins

        property string label
        property int    from:           10
        property int    to:             90
        property int    currentValue:   -1
        property int    _pending:       -1
        property int    _shown:         _pending >= 0 ? _pending : currentValue

        signal commit(int value)

        function _step(dir) {
            var base = _shown
            var v = dir > 0 ? Math.floor(base / 5) * 5 + 5 : Math.ceil(base / 5) * 5 - 5
            _pending = Math.max(from, Math.min(to, v))
            commitTimer.restart()
        }

        Timer {
            id:         commitTimer
            interval:   600
            onTriggered: {
                var v = tuningRow._pending
                tuningRow._pending = -1
                if (v >= 0 && v !== tuningRow.currentValue) {
                    tuningRow.commit(v)
                }
            }
        }

        QGCLabel {
            text:                   tuningRow.label
            Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 14
        }
        QGCButton {
            text:                   "-"
            fontWeight:             Font.DemiBold
            backRadius:             panelRadius * 0.5
            enabled:                tuningRow._shown > tuningRow.from
            Layout.preferredWidth:  Layout.preferredHeight
            Layout.preferredHeight: buttonHeight * 1.26   // 0.7 * 1.8
            onClicked:              tuningRow._step(-1)
        }
        QGCLabel {
            text:                   tuningRow._shown >= 0 ? tuningRow._shown : "--"
            font.weight:            Font.DemiBold
            horizontalAlignment:    Text.AlignHCenter
            Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 5
        }
        QGCButton {
            text:                   "+"
            fontWeight:             Font.DemiBold
            backRadius:             panelRadius * 0.5
            enabled:                tuningRow._shown >= 0 && tuningRow._shown < tuningRow.to
            Layout.preferredWidth:  Layout.preferredHeight
            Layout.preferredHeight: buttonHeight * 1.26   // 0.7 * 1.8
            onClicked:              tuningRow._step(1)
        }
    }

    // Popup panel, appears when clicking top toolbar gimbal indicator
    Component {
        id: gimbalControlsPage

        ToolIndicatorPage {
            contentComponent: GridLayout {
                // Header
                QGCLabel {
                    text:                   qsTr("Gimbal<br> Controls")
                    font.pointSize:         ScreenTools.smallFontPointSize
                    Layout.preferredWidth:  buttonHeight * 1.1
                    font.weight:            Font.DemiBold
                }

                // Action buttons
                Repeater {
                    id: simpleGimbalButtonsRepeater

                    model: [
                        {id: "yawLock", text: activeGimbal.yawLock ? qsTr("Yaw <br> Follow") : qsTr("Yaw <br> Lock")},
                        {id: "center",  text: qsTr("Center")},
                        {id: "tilt90",  text: qsTr("Tilt 90")}
                    ]

                    QGCButton {
                        property var callbackList: [
                            {"yawLock": function(){ gimbalController.toggleGimbalYawLock(!activeGimbal.yawLock) }},
                            {"center":  function(){ gimbalController.centerGimbal() }},
                            {"tilt90":  function(){ gimbalController.sendPitchBodyYaw(-90, 0) }}
                        ]

                        Layout.preferredWidth:  Layout.preferredHeight
                        Layout.preferredHeight: buttonHeight
                        Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                        text:                   modelData.text
                        fontWeight:             Font.DemiBold
                        pointSize:              ScreenTools.smallFontPointSize
                        backRadius:             panelRadius * 0.5
                        leftPadding:            squareButtonPadding
                        rightPadding:           squareButtonPadding
                        onClicked: {
                            var callback = callbackList.find(function(item) {
                                return item.hasOwnProperty(modelData.id)
                            })
                            if (callback !== undefined) {
                                callback[modelData.id]()
                            }
                        }
                    }
                }

                // Separator
                Rectangle {
                    Layout.leftMargin:      margins
                    Layout.preferredWidth:  2
                    Layout.preferredHeight: separatorHeight
                    color:                  qgcPal.windowShade
                }

                // Settings toggle button — hidden for ILX payload (the on-screen-control settings
                // it exposes only apply to VIO; ILX users have no use for them).
                QGCButton {
                    id:                     extendedOptionsButton
                    Layout.leftMargin:      margins
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignBottom
                    text:                   qsTr("Settings")
                    fontWeight:             Font.DemiBold
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    checkable:              true
                    checked:                control.settingsPanelVisible
                    visible:                _selectedPayload !== 0   // hide for ILX
                    leftPadding:            squareButtonPadding
                    rightPadding:           squareButtonPadding
                    onCheckedChanged: {
                        if (checked !== control.settingsPanelVisible) {
                            control.settingsPanelVisible = checked
                        }
                    }
                }

                // Speed/Smooth toggle — ILX only. Takes the Settings button's slot.
                QGCButton {
                    Layout.leftMargin:      margins
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignBottom
                    text:                   qsTr("Speed/<br>Smooth")
                    fontWeight:             Font.DemiBold
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    checkable:              true
                    checked:                control.tuningPanelVisible
                    visible:                _selectedPayload === 0
                    leftPadding:            squareButtonPadding
                    rightPadding:           squareButtonPadding
                    onCheckedChanged: {
                        if (checked !== control.tuningPanelVisible) {
                            control.tuningPanelVisible = checked
                        }
                        if (checked) {
                            control.activeVehicle.gimbalRequestTuning()
                        }
                    }
                }

                // Speed/Smooth panel (ILX). Smooth = Gremsy RC_LPF_PAN/TILT on the gimbal (compid 154,
                // sent out-of-band); Speed = autopilot MNT1_RC_RATE through the normal Fact system.
                ColumnLayout {
                    id:                 tuningPanel
                    Layout.row:         3
                    Layout.columnSpan:  8
                    Layout.fillWidth:   true
                    Layout.topMargin:   margins
                    visible:            tuningPanelVisible && _selectedPayload === 0
                    spacing:            margins

                    property var  _rcRateFact: tuningController.getParameterFact(-1, "MNT1_RC_RATE", false)

                    FactPanelController { id: tuningController }

                    Component.onCompleted: {
                        if (control.tuningPanelVisible && _selectedPayload === 0) {
                            control.activeVehicle.gimbalRequestTuning()
                        }
                    }

                    TuningStepperRow {
                        label:          qsTr("Smooth - Yaw")
                        from:           10
                        to:             90
                        currentValue:   control.activeVehicle.gimbalRcLpfPan
                        onCommit:       (value) => control.activeVehicle.gimbalSetParam("RC_LPF_PAN", value)
                    }
                    TuningStepperRow {
                        label:          qsTr("Smooth - Pitch")
                        from:           10
                        to:             90
                        currentValue:   control.activeVehicle.gimbalRcLpfTilt
                        onCommit:       (value) => control.activeVehicle.gimbalSetParam("RC_LPF_TILT", value)
                    }
                    TuningStepperRow {
                        label:          qsTr("Speed")
                        from:           10
                        to:             100
                        currentValue:   tuningPanel._rcRateFact ? Math.round(tuningPanel._rcRateFact.rawValue) : -1
                        onCommit:       (value) => { tuningPanel._rcRateFact.rawValue = value }
                    }
                }

                // Settings panel (on-screen control + joystick speed) — also hidden for ILX,
                // even if it had been opened previously before payload changed.
                GridLayout {
                    Layout.row:         2
                    Layout.columnSpan:  8
                    Layout.fillWidth:   true
                    height:             buttonHeight * 1.5
                    visible:            settingsPanelVisible && _selectedPayload !== 0
                    columns:            2
                    rowSpacing:         margins

                    FactCheckBox {
                        id:                 enableOnScreenControlCheckbox
                        text:               "  " + QGroundControl.settingsManager.gimbalControllerSettings.EnableOnScreenControl.shortDescription
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.EnableOnScreenControl
                        checkedValue:       1
                        uncheckedValue:     0
                        Layout.columnSpan:  2
                    }

                    QGCLabel {
                        text:               qsTr("Control type: ")
                        visible:            enableOnScreenControlCheckbox.checked
                    }
                    FactComboBox {
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.ControlType
                        visible:            enableOnScreenControlCheckbox.checked
                    }

                    QGCLabel {
                        text:               qsTr("Horizontal FOV")
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 0
                    }
                    FactTextField {
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.CameraHFov
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 0
                    }

                    QGCLabel {
                        text:               qsTr("Vertical FOV")
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 0
                    }
                    FactTextField {
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.CameraVFov
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 0
                    }

                    QGCLabel {
                        text:               qsTr("Max speed:")
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 1
                    }
                    FactTextField {
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.CameraSlideSpeed
                        visible:            enableOnScreenControlCheckbox.checked && QGroundControl.settingsManager.gimbalControllerSettings.ControlType.rawValue === 1
                    }

                    Rectangle {
                        Layout.columnSpan:       2
                        Layout.preferredHeight:  2
                        Layout.fillWidth:        true
                        Layout.margins:          margins
                        color:                   qgcPal.windowShade
                        visible:                 joystickButtonsAvailable && QGroundControl.settingsManager.gimbalControllerSettings.visible
                    }

                    QGCLabel {
                        text:               qsTr("Joystick buttons speed:")
                        visible:            joystickButtonsAvailable && QGroundControl.settingsManager.gimbalControllerSettings.visible
                    }
                    FactTextField {
                        fact:               QGroundControl.settingsManager.gimbalControllerSettings.joystickButtonsSpeed
                        visible:            joystickButtonsAvailable && QGroundControl.settingsManager.gimbalControllerSettings.visible
                        showHelp:           true
                    }
                }
            }
        }
    }

    // Toolbar icon column: payload icon (with optional lock overlay) on top, "Gimbal" caption below.
    // _iconScale enlarges the visible icon ~50% above what the toolbar height would normally allow;
    // the icon is rasterized at the larger size for sharpness and is allowed to overflow the
    // column's vertical bounds — toolbar parent doesn't clip its children.
    property real _iconScale: 1.5

    Column {
        id:             gimbalIndicatorIcon
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        0

        QGCColoredImage {
            id:                 gimbalIcon
            anchors.horizontalCenter: parent.horizontalCenter
            height:             (gimbalIndicatorIcon.height - gimbalLabel.height) * _iconScale
            width:              height
            source:             "/gimbal/payload.svg"
            fillMode:           Image.PreserveAspectFit
            sourceSize.height:  height
            color:              qgcPal.buttonText

            // Small lock overlay when gimbal is in yaw-lock mode
            QGCColoredImage {
                id:                  lockOverlay
                visible:             !!activeGimbal && activeGimbal.yawLock
                width:               parent.width * 0.45
                height:              width
                anchors.top:         parent.top
                anchors.right:       parent.right
                anchors.topMargin:   parent.height * 0.05
                anchors.rightMargin: parent.width * 0.05
                source:              "/InstrumentValueIcons/lock-closed.svg"
                fillMode:            Image.PreserveAspectFit
                sourceSize.height:   height
                color:               qgcPal.buttonText
            }
        }

        QGCLabel {
            id:                 gimbalLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text:               qsTr("Gimbal")
            font.pointSize:     ScreenTools.smallFontPointSize
            color:              qgcPal.buttonText
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked:      mainWindow.showIndicatorDrawer(gimbalControlsPage, control)
    }
}
