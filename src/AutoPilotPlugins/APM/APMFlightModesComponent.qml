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
import QGroundControl.Controllers
import QGroundControl.ScreenTools

SetupPage {
    id:             flightModePage
    pageComponent:  flightModePageComponent

    // Xplorer fork: variant-aware presentation.
    //   Xplorer (variant == 1): only the curated Switch Options block (RC9..RC16) shows.
    //   X55     (variant == 0): a minimal Flight Modes block (FLTMODE1..6) shows in
    //                           addition to the Switch Options block.
    readonly property bool _isXplorer: QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1

    readonly property string _modeChannelParam: controller.modeChannelParam
    readonly property string _modeParamPrefix:  controller.modeParamPrefix
    readonly property var    _pwmStrings:       [ "PWM 0 - 1230", "PWM 1231 - 1360", "PWM 1361 - 1490", "PWM 1491 - 1620", "PWM 1621 - 1749", "PWM 1750 +"]

    property real   _margins:                   ScreenTools.defaultFontPixelHeight
    property Fact   _nullFact
    property bool   _fltmodeChExists:           controller.parameterExists(-1, _modeChannelParam)
    property Fact   _fltmodeCh:                 _fltmodeChExists ? controller.getParameterFact(-1, _modeChannelParam) : _nullFact
    property bool   _ch7OptAvailable:           controller.parameterExists(-1, "CH7_OPT")
    // Variant-aware RC option range:
    //   Xplorer: channels 5-8 are reserved for flight mode aux switches
    //     (AltHold/Loiter/RTL/Auto) and locked via defaults.parm. Operators
    //     only assign aux functions to RC9..RC16.
    //   X55: no reserved range — operators can assign aux functions to RC5..RC16.
    property int    _rcOptionStart:             _isXplorer ? 9 : 5
    property int    _rcOptionStop:              16
    property bool   _customSimpleMode:          controller.simpleMode === APMFlightModesComponentController.SimpleModeCustom

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    APMFlightModesComponentController {
        id:         controller
    }

    Component {
        id: flightModePageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:     _margins

            // Column {
            //     spacing: _margins

            //     QGCLabel {
            //         id:             flightModeLabel
            //         text:           qsTr("Flight Mode Settings") + (_fltmodeChExists ? "" : qsTr(" (Channel 5)"))
            //         font.bold:      true
            //     }

            //     Rectangle {
            //         id:     flightModeSettings
            //         width:  flightModeColumn.width + (_margins * 2)
            //         height: flightModeColumn.height + ScreenTools.defaultFontPixelHeight
            //         color:  qgcPal.windowShade

            //         Column {
            //             id:                 flightModeColumn
            //             anchors.margins:    ScreenTools.defaultFontPixelWidth
            //             anchors.left:       parent.left
            //             anchors.top:        parent.top
            //             spacing:            ScreenTools.defaultFontPixelHeight

            //             Row {
            //                 spacing:    _margins
            //                 visible:    _fltmodeChExists

            //                 QGCLabel {
            //                     id:                 modeChannelLabel
            //                     anchors.baseline:   modeChannelCombo.baseline
            //                     text:               qsTr("Flight mode channel:")
            //                 }

            //                 QGCComboBox {
            //                     id:             modeChannelCombo
            //                     width:          ScreenTools.defaultFontPixelWidth * 15
            //                     model:          [ qsTr("Not assigned"), qsTr("Channel 1"), qsTr("Channel 2"),
            //                         qsTr("Channel 3"),    qsTr("Channel 4"), qsTr("Channel 5"),
            //                         qsTr("Channel 6"),    qsTr("Channel 7"), qsTr("Channel 8") ]

            //                     currentIndex:   _fltmodeCh.value
            //                     onActivated: (index) => { _fltmodeCh.value = index }
            //                 }
            //             }

            //             GridLayout {
            //                 rows:   _customSimpleMode ? 7 : 6
            //                 flow:   GridLayout.TopToBottom

            //                 QGCLabel { text: ""; visible: _customSimpleMode }
            //                 Repeater {
            //                     model:  6

            //                     QGCLabel {
            //                         text:   qsTr("Flight Mode ") + index
            //                         color:  controller.activeFlightMode == index ? "yellow" : qgcPal.text

            //                         property int index: modelData + 1
            //                     }
            //                 }

            //                 QGCLabel { text: ""; visible: _customSimpleMode }
            //                 Repeater {
            //                     model:  6

            //                     FactComboBox {
            //                         Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 15
            //                         fact:                   controller.getParameterFact(-1, _modeParamPrefix + index)
            //                         indexModel:             false

            //                         property int index: modelData + 1
            //                     }
            //                 }

            //                 QGCLabel {
            //                     text:           qsTr("Simple")
            //                     font.pointSize: ScreenTools.smallFontPointSize
            //                     visible:        _customSimpleMode
            //                 }
            //                 Repeater {
            //                     model:  controller.simpleModeEnabled
            //                     QGCCheckBox {
            //                         Layout.alignment:   Qt.AlignHCenter
            //                         visible:            _customSimpleMode
            //                         checked:            modelData
            //                         onClicked:          controller.setSimpleMode(index, checked)
            //                     }
            //                 }

            //                 QGCLabel {
            //                     text:           qsTr("Super-Simple")
            //                     font.pointSize: ScreenTools.smallFontPointSize
            //                     visible:        _customSimpleMode
            //                 }
            //                 Repeater {
            //                     model:  controller.superSimpleModeEnabled
            //                     QGCCheckBox {
            //                         Layout.alignment:   Qt.AlignHCenter
            //                         visible:            _customSimpleMode
            //                         checked:            modelData
            //                         onClicked:          controller.setSuperSimpleMode(index, checked)
            //                     }
            //                 }

            //                 QGCLabel { text: ""; visible: _customSimpleMode }
            //                 Repeater {
            //                     model:  6

            //                     QGCLabel { text: _pwmStrings[modelData] }
            //                 }
            //             }

            //             RowLayout {
            //                 spacing: _margins
            //                 visible: controller.simpleModesSupported

            //                 QGCLabel { text: qsTr("Simple Mode") }

            //                 QGCComboBox {
            //                     model:          controller.simpleModeNames
            //                     currentIndex:   controller.simpleMode
            //                     onActivated: (index) => { controller.simpleMode = index }
            //                 }
            //             }
            //         } // Column - Flight Modes
            //     } // Rectangle - Flight Modes
            // } // Column - Flight Modes

            Column {
                spacing: _margins

                QGCLabel {
                    id:                 channelOptionsLabel
                    text:               qsTr("Switch Options")
                    font.bold:          true
                }

                Rectangle {
                    id:     channelOptionsSettings
                    width:  channelOptColumn.width + (_margins * 2)
                    height: channelOptColumn.height + ScreenTools.defaultFontPixelHeight
                    color:  qgcPal.windowShade

                    Column {
                        id:                 channelOptColumn
                        anchors.margins:    ScreenTools.defaultFontPixelWidth
                        anchors.left:       parent.left
                        anchors.top:        parent.top
                        spacing:            ScreenTools.defaultFontPixelHeight

                        Repeater {
                            model: _rcOptionStop - _rcOptionStart + 1

                            Row {
                                spacing: ScreenTools.defaultFontPixelWidth

                                property int index: modelData + _rcOptionStart
                                property Fact nullFact: Fact { }

                                QGCLabel {
                                    anchors.baseline:   optCombo.baseline
                                    text:               qsTr("Channel option %1 :").arg(index)
                                    color:              controller.channelOptionEnabled[modelData + (_ch7OptAvailable ? 1 : 0)] ? "yellow" : qgcPal.text
                                }

                                FactComboBox {
                                    id:         optCombo
                                    // Width sized for longest curated entry (e.g. "Camera Record Video", "RangeFinder Enable").
                                    width:      ScreenTools.defaultFontPixelWidth * 28
                                    fact:       controller.getParameterFact(-1, "r.RC" + index + "_OPTION")
                                    indexModel: false
                                }
                            }
                        } // Repeater -- Channel options
                    } // Column - Channel options
                } // Rectangle - Channel options
            } // Column - Channel options
        } // Flow
    } // Component - flightModePageComponent
} // SetupPage
