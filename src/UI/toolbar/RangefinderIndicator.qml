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

//-------------------------------------------------------------------------
//-- Rangefinder (Downward Distance Sensor) Indicator
Item {
    id:             control
    width:          contentRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: _activeVehicle && _showDownRangefinder.rawValue

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var    _showDownRangefinder:   QGroundControl.settingsManager.flyViewSettings.showDownRangefinder
    property var    _distanceSensors:       _activeVehicle ? _activeVehicle.distanceSensors : null
    property real   _distance:              _distanceSensors ? _distanceSensors.rotationPitch270.rawValue : NaN
    property bool   _isActive:              !isNaN(_distance)

    // Variant-aware active-state input:
    //   Xplorer (variant == 1): firmware broadcasts RFND_ST (see _statusFact below).
    //   X55     (variant == 0): no firmware status broadcast — drive active state
    //                           from a user-configured RC channel reading.
    readonly property bool _isXplorer:          QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
    readonly property var  _rcChannelSetting:   QGroundControl.settingsManager.flyViewSettings.rangefinderRCChannel
    readonly property int  _rcChannel:          _rcChannelSetting ? _rcChannelSetting.rawValue : 0
    readonly property var  _rcValues:           _activeVehicle ? _activeVehicle.rcChannelValues : []
    readonly property real _rcPwm:              (_rcChannel > 0 && _rcChannel <= _rcValues.length)
                                                    ? _rcValues[_rcChannel - 1] : NaN
    readonly property bool _rcMonitoring:       !_isXplorer && _rcChannel > 0
    readonly property bool _rcActive:           _rcMonitoring && !isNaN(_rcPwm) && _rcPwm > 1500

    // Soft-enable parameter resolved from a Loader so FactPanelController only constructs
    // once a real vehicle is connected (otherwise it latches onto the offline-editing vehicle).
    property Fact _btnFact:      paramLoader.item ? paramLoader.item.btnFact : null
    property Fact _landAltFact:  paramLoader.item ? paramLoader.item.landAltFact : null
    property Fact _landEnFact:   paramLoader.item ? paramLoader.item.landEnFact : null
    property bool _btnEnabled:   !!_btnFact && _btnFact.rawValue !== 0
    property bool _btnPresent:   !!_btnFact
    property bool _landEnabled:  !!_landEnFact && _landEnFact.rawValue !== 0

    // Live activation status broadcast by firmware as NAMED_VALUE_INT "RFND_ST".
    // 0=disabled, 1=no-data, 2=standby (enabled but surface tracking not engaged),
    // 3=active (surface tracking running). 255=never received (legacy fw fallback).
    property var  _statusFactGroup: _activeVehicle ? _activeVehicle.getFactGroup("apmCopterStatus") : null
    property var  _statusFact:      _statusFactGroup ? _statusFactGroup.getFact("rfndStatus") : null
    property int  _liveStatus:      _statusFact ? _statusFact.rawValue : 255
    property bool _liveStatusKnown: _liveStatus >= 0 && _liveStatus <= 3

    // Unit conversion for system units
    property var    _unitsConversion:       QGroundControl.unitsConversion
    property real   _displayDistance:       _isActive ? _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(_distance) : NaN
    property string _displayDistanceStr:    _isActive ? _displayDistance.toFixed(1) : "--"
    property string _units:                 _unitsConversion.appSettingsHorizontalDistanceUnitsString

    // Icon distance-based color (only meaningful when soft enable is on)
    function getDistanceColor() {
        if (!_isActive) {
            return qgcPal.colorGrey
        }
        if (_distance < 1.0) {
            return qgcPal.colorRed
        }
        if (_distance < 3.0) {
            return qgcPal.colorOrange
        }
        return qgcPal.colorGreen
    }

    function getStatusText() {
        if (_liveStatusKnown) {
            if (_liveStatus === 0) return qsTr("Disabled")
            if (_liveStatus === 1) return qsTr("No Data")
            if (_liveStatus === 2) return qsTr("Standby")
            return qsTr("Active")
        }
        // X55: RC channel drives the active label.
        if (_rcMonitoring) {
            if (isNaN(_rcPwm))    return qsTr("No Data")
            return _rcActive ? qsTr("Active") : qsTr("Standby")
        }
        // Legacy firmware fallback (no RFND_ST broadcast, no RC monitoring).
        if (_btnPresent && !_btnEnabled) {
            return qsTr("Disabled")
        }
        if (!_isActive) {
            return qsTr("No Data")
        }
        return qsTr("Active")
    }

    // Dot color: gray=disabled, red=no data, orange=standby (enabled but
    // surface tracking not engaged), green=active. Falls back to RC-channel
    // monitoring on X55, then to the legacy param-based coloring.
    function getDotColor() {
        if (_liveStatus === 3) return qgcPal.colorGreen
        if (_liveStatus === 2) return qgcPal.colorOrange
        if (_liveStatus === 1) return qgcPal.colorRed
        if (_liveStatus === 0) return qgcPal.colorGrey
        if (_rcMonitoring)     return _rcActive ? qgcPal.colorGreen : qgcPal.colorGrey
        return _btnEnabled ? qgcPal.colorGreen : qgcPal.colorGrey
    }

    Loader {
        id:                 paramLoader
        active:             _activeVehicle !== null
        sourceComponent:    paramComp

        Component {
            id: paramComp
            Item {
                FactPanelController { id: ctrl }
                // Bumped on factAdded so the bindings below re-evaluate if a param shows up later.
                property int reload: 0
                property Fact btnFact:     (reload, true) ? ctrl.getParameterFact(-1, "RFND_BTN_EN",  false) : null
                property Fact landAltFact: (reload, true) ? ctrl.getParameterFact(-1, "LAND_RNG_ALT", false) : null
                property Fact landEnFact:  (reload, true) ? ctrl.getParameterFact(-1, "LAND_RNG_EN",  false) : null

                Connections {
                    target: _activeVehicle ? _activeVehicle.parameterManager : null
                    function onFactAdded(componentId, fact) {
                        if (!fact) return
                        if (fact.name === "RFND_BTN_EN"
                            || fact.name === "LAND_RNG_ALT"
                            || fact.name === "LAND_RNG_EN") reload++
                    }
                }
            }
        }
    }

    Component {
        id: rangefinderInfoPage

        ToolIndicatorPage {
            showExpand: false

            contentComponent: SettingsGroupLayout {
                heading:                qsTr("Downward Rangefinder")
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 30

                LabelledLabel {
                    label:      qsTr("Status")
                    labelText:  getStatusText()
                }

                LabelledLabel {
                    label:      qsTr("Distance (%1)").arg(_unitsConversion.appSettingsHorizontalDistanceUnitsString)
                    labelText:  _isActive ? _displayDistanceStr : qsTr("--")
                }

                // ON/OFF buttons drive RFND_BTN_EN — an Xplorer firmware construct.
                // Hidden on X55, which uses RC channel monitoring (not a GCS button)
                // for rangefinder enable/disable.
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            _btnPresent && _isXplorer

                    QGCButton {
                        text:               qsTr("ON")
                        Layout.fillWidth:   true
                        backgroundColor:    _btnEnabled ? "green" : "gray"
                        onClicked:          { if (_btnFact) _btnFact.rawValue = 1 }
                    }
                    QGCButton {
                        text:               qsTr("OFF")
                        Layout.fillWidth:   true
                        backgroundColor:    !_btnEnabled ? "green" : "gray"
                        onClicked:          { if (_btnFact) _btnFact.rawValue = 0 }
                    }
                }

                // "Use rangefinder for landing" — LAND_RNG_EN master enable.
                // This feature is independent of the rangefinder being on for
                // terrain following: even with RFND_BTN_EN=0, if LAND_RNG_EN is
                // set and the sensor is healthy, the autopilot will use the
                // rangefinder to limit descent speed near the ground.
                // Xplorer-only: the landing-altitude helper is part of the
                // Xplorer surface tracking flow, not used on X55.
                QGCCheckBox {
                    text:       qsTr("Use rangefinder for landing")
                    visible:    !!_landEnFact && _isXplorer
                    checked:    _landEnabled
                    onClicked: {
                        if (_landEnFact) _landEnFact.rawValue = checked ? 1 : 0
                    }
                }

                // Land Altitude (LAND_RNG_ALT) — slider 2..15 m. Stored as cm
                // in the autopilot. Only shown when LAND_RNG_EN is set.
                // Xplorer-only (paired with the "Use rangefinder for landing" toggle).
                ColumnLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelHeight / 4
                    visible:            !!_landAltFact && _landEnabled && _isXplorer

                    RowLayout {
                        Layout.fillWidth: true
                        QGCLabel {
                            Layout.fillWidth: true
                            text: qsTr("Land Altitude (%1)").arg(_unitsConversion.appSettingsVerticalDistanceUnitsString)
                        }
                        QGCLabel {
                            text: _landAltFact
                                  ? _unitsConversion.metersToAppSettingsVerticalDistanceUnits(_landAltFact.rawValue / 100).toFixed(1)
                                  : "--"
                            opacity: 0.8
                        }
                    }

                    QGCSlider {
                        id:                     landAltSlider
                        Layout.fillWidth:       true
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.8
                        from:                   6.0    // meters
                        to:                     15.0
                        stepSize:               1.0    // snap to whole meters: 6, 7, 8, ..., 15
                        snapMode:               Slider.SnapAlways
                        // Map cm fact -> meters for the slider position. If the
                        // stored value isn't on a 1 m boundary (e.g. 6.5 m),
                        // SnapAlways will pull the handle to the nearest valid
                        // stop on first interaction.
                        value: _landAltFact ? Math.max(from, Math.min(to, _landAltFact.rawValue / 100)) : 8.0
                        // Commit only on release (pressed → false). Avoids spamming
                        // the autopilot with intermediate values while dragging.
                        onPressedChanged: {
                            if (!pressed && _landAltFact) {
                                _landAltFact.rawValue = Math.round(value * 100)
                            }
                        }

                        // Bigger handle for easier dragging.
                        handle: Rectangle {
                            x:              landAltSlider.leftPadding
                                            + landAltSlider.visualPosition * (landAltSlider.availableWidth - width)
                            y:              landAltSlider.topPadding
                                            + landAltSlider.availableHeight / 2 - height / 2
                            implicitWidth:  ScreenTools.defaultFontPixelHeight * 1.4
                            implicitHeight: ScreenTools.defaultFontPixelHeight * 1.4
                            radius:         width / 2
                            color:          qgcPal.button
                            border.color:   qgcPal.buttonText
                            border.width:   1
                        }
                    }
                }

                // "Update firmware" warning — Xplorer-only since the GCS button is
                // an Xplorer feature. X55 has no such button by design.
                QGCLabel {
                    Layout.fillWidth:   true
                    visible:            !_btnPresent && _isXplorer
                    text:               qsTr("RFND_BTN_EN parameter not found. Update firmware to use the GCS button.")
                    wrapMode:           Text.WordWrap
                    color:              qgcPal.colorOrange
                    font.pointSize:     ScreenTools.smallFontPointSize
                }
            }
        }
    }

    Row {
        id:             contentRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth * 0.5

        // Container for icon with status indicator dot
        Item {
            width:                  rangefinderIcon.width + (statusDot.visible ? statusDot.width * 0.5 : 0)
            anchors.top:            parent.top
            anchors.bottom:         parent.bottom

            // Dim the icon when the sensor isn't providing useful data: disabled (0)
            // or enabled-but-no-data (1). Standby (2) and active (3) both leave the
            // icon bright since the distance reading is meaningful in both cases.
            // Falls back to legacy _btnEnabled-based dimming if no live status has arrived.
            property bool _iconDimmed: _liveStatusKnown ? (_liveStatus <= 1) : (_btnPresent && !_btnEnabled)

            // Rangefinder icon - terrain icon represents ground distance
            QGCColoredImage {
                id:                     rangefinderIcon
                width:                  height
                anchors.top:            parent.top
                anchors.bottom:         parent.bottom
                sourceSize.height:      height
                source:                 "/res/terrain.svg"
                fillMode:               Image.PreserveAspectFit
                color:                  parent._iconDimmed ? qgcPal.colorGrey : getDistanceColor()
                opacity:                parent._iconDimmed ? 0.5 : (_isActive ? 1.0 : 0.5)
            }

            // Status indicator dot (bottom-right corner of icon)
            Rectangle {
                id:                     statusDot
                width:                  ScreenTools.defaultFontPixelHeight * 0.6
                height:                 width
                radius:                 width / 2
                color:                  getDotColor()
                visible:                _btnPresent || _liveStatusKnown || _rcMonitoring
                anchors.bottom:         rangefinderIcon.bottom
                anchors.right:          rangefinderIcon.right
                anchors.bottomMargin:   -height * 0.1
                anchors.rightMargin:    -width * 0.1

                border.width:           1
                border.color:           "white"
            }
        }

        // Distance value text
        QGCLabel {
            anchors.verticalCenter: parent.verticalCenter
            text:                   _isActive ? _displayDistanceStr : "--"
            color: {
                var dimmed = _liveStatusKnown ? (_liveStatus <= 1) : (_btnPresent && !_btnEnabled)
                return dimmed ? qgcPal.colorGrey : getDistanceColor()
            }
            font.pointSize:         ScreenTools.mediumFontPointSize
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked:      mainWindow.showIndicatorDrawer(rangefinderInfoPage, control)
    }
}
