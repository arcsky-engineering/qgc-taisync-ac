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
//-- Forward Rangefinder (Forward Distance Sensor) Indicator
Item {
    id:             control
    width:          contentRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: _activeVehicle && _showForwardRangefinder.rawValue

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var    _showForwardRangefinder: QGroundControl.settingsManager.flyViewSettings.showForwardRangefinder
    property var    _distanceSensors:       _activeVehicle ? _activeVehicle.distanceSensors : null
    property real   _distance:              _distanceSensors ? _distanceSensors.rotationNone.rawValue : NaN
    property bool   _isActive:              !isNaN(_distance)

    // Soft-enable parameter resolved from a Loader so FactPanelController only constructs
    // once a real vehicle is connected (otherwise it latches onto the offline-editing vehicle).
    property Fact _btnFact:       paramLoader.item ? paramLoader.item.btnFact     : null
    property Fact _distFact:      paramLoader.item ? paramLoader.item.distFact    : null
    property Fact _condFact:      paramLoader.item ? paramLoader.item.condFact    : null
    property Fact _actAutoFact:   paramLoader.item ? paramLoader.item.actAutoFact : null
    property Fact _actManFact:    paramLoader.item ? paramLoader.item.actManFact  : null
    property Fact _altFact:       paramLoader.item ? paramLoader.item.altFact     : null
    property Fact _sampFact:      paramLoader.item ? paramLoader.item.sampFact    : null
    property bool _btnEnabled:    !!_btnFact && _btnFact.rawValue !== 0
    property bool _btnPresent:    !!_btnFact

    // Live activation status broadcast by firmware as NAMED_VALUE_INT "FWDAVD_ST".
    //   0 = Disabled (master gate off — param 0 or RC switch low when configured)
    //   1 = Standby   (master on, but altitude/mode gates fail or no useful sensor data)
    //   2 = Monitoring (sensor Good, distance >= FWDAVD_DIST)
    //   3 = Threat    (sensor Good, distance < FWDAVD_DIST, no action yet)
    //   4 = Blocking  (avoidance action firing — RTL/Land/stick-lock)
    //   255 = never received (legacy fw fallback)
    property var  _statusFactGroup: _activeVehicle ? _activeVehicle.getFactGroup("apmCopterStatus") : null
    property var  _statusFact:      _statusFactGroup ? _statusFactGroup.getFact("fwdAvdStatus") : null
    property int  _liveStatus:      _statusFact ? _statusFact.rawValue : 255
    property bool _liveStatusKnown: _liveStatus >= 0 && _liveStatus <= 4
    property bool _blocking:        _liveStatusKnown && _liveStatus === 4

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
            if (_liveStatus === 1) return qsTr("Standby")
            if (_liveStatus === 2) return qsTr("Monitoring")
            if (_liveStatus === 3) return qsTr("Obstacle Detected")
            return qsTr("BLOCKING")  // 4
        }
        // Legacy firmware fallback (no FWDAVD_ST broadcast).
        if (_btnPresent && !_btnEnabled) {
            return qsTr("Disabled")
        }
        if (!_isActive) {
            return qsTr("No Data")
        }
        return qsTr("Active")
    }

    // Dot color: pure "is the feature enabled?" indicator. Green when master
    // gate is on (any non-zero state), gray when off. State 0 means the
    // firmware-side master gate dropped (param off OR RC switch low when
    // configured). Falls back to the param value when no live status has arrived.
    function getDotColor() {
        if (_liveStatusKnown) {
            return _liveStatus === 0 ? qgcPal.colorGrey : qgcPal.colorGreen
        }
        return _btnEnabled ? qgcPal.colorGreen : qgcPal.colorGrey
    }

    // Icon color: gray when off or no useful sensor data; green when monitoring
    // and clear; red when a threat is in the trigger band or avoidance is firing.
    // Falls back to legacy distance-based coloring when no live status has arrived.
    function getIconColor() {
        if (_liveStatusKnown) {
            if (_liveStatus <= 1) return qgcPal.colorGrey   // off or standby
            if (_liveStatus === 2) return qgcPal.colorGreen // monitoring
            return qgcPal.colorRed                          // threat or blocking
        }
        return getDistanceColor()
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
                property Fact btnFact:     (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_BTN_EN",   false) : null
                property Fact distFact:    (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_DIST",     false) : null
                property Fact condFact:    (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_COND",     false) : null
                property Fact actAutoFact: (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_ACT_AUTO", false) : null
                property Fact actManFact:  (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_ACT_MAN",  false) : null
                property Fact altFact:     (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_ALT",      false) : null
                property Fact sampFact:    (reload, true) ? ctrl.getParameterFact(-1, "FWDAVD_SAMP",     false) : null

                Connections {
                    target: _activeVehicle ? _activeVehicle.parameterManager : null
                    function onFactAdded(componentId, fact) {
                        if (!fact) return
                        if (fact.name.indexOf("FWDAVD_") === 0) reload++
                    }
                }
            }
        }
    }

    Component {
        id: forwardRangefinderInfoPage

        ToolIndicatorPage {
            showExpand: false

            contentComponent: SettingsGroupLayout {
                heading:                qsTr("Forward Rangefinder")
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 30

                LabelledLabel {
                    label:      qsTr("Status")
                    labelText:  getStatusText()
                }

                LabelledLabel {
                    label:      qsTr("Distance (%1)").arg(_unitsConversion.appSettingsHorizontalDistanceUnitsString)
                    labelText:  _isActive ? _displayDistanceStr : qsTr("--")
                }

                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            _btnPresent

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

                // Trigger Distance (FWDAVD_DIST) — meters in autopilot, displayed in horizontal units.
                // Clamp-on-finish to 2..30 m so values outside that range visibly snap.
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_distFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Trigger Distance (%1)").arg(_unitsConversion.appSettingsHorizontalDistanceUnitsString)
                    }
                    QGCTextField {
                        id:                     distField
                        Layout.minimumWidth:    ScreenTools.defaultFontPixelWidth * 10
                        inputMethodHints:       Qt.ImhFormattedNumbersOnly
                        text: _distFact
                              ? _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(_distFact.rawValue).toFixed(1)
                              : "--"

                        onEditingFinished: {
                            if (!_distFact) return
                            var v = parseFloat(text)
                            if (isNaN(v)) v = _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(2)
                            var minDisp = _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(2)
                            var maxDisp = _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(30)
                            v = Math.max(minDisp, Math.min(maxDisp, v))
                            var meters = _unitsConversion.appSettingsHorizontalDistanceUnitsToMeters(v)
                            _distFact.rawValue = meters
                            text = v.toFixed(1)
                        }

                        Connections {
                            target: _distFact
                            function onRawValueChanged() {
                                distField.text = _distFact
                                    ? _unitsConversion.metersToAppSettingsHorizontalDistanceUnits(_distFact.rawValue).toFixed(1)
                                    : "--"
                            }
                        }
                    }
                }

                // Sensitivity slider — 4 discrete positions mapped to FWDAVD_SAMP.
                // Left = least sensitive (10 samples, ~1.0 s of in-band readings needed
                // to trigger). Right = most sensitive (1 sample, fires on first hit).
                // Middle stops: 6 samples (~0.6 s), 3 samples (~0.3 s).
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_sampFact

                    // Discrete sample-count values, indexed by slider position
                    // (0 = leftmost = least sensitive).
                    readonly property var samplesOptions: [10, 6, 3, 1]

                    function nearestIndex(v) {
                        var bestIdx = 0
                        var bestDiff = Math.abs(samplesOptions[0] - v)
                        for (var i = 1; i < samplesOptions.length; i++) {
                            var diff = Math.abs(samplesOptions[i] - v)
                            if (diff < bestDiff) { bestDiff = diff; bestIdx = i }
                        }
                        return bestIdx
                    }

                    QGCLabel {
                        text: qsTr("Sensitivity")
                        Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                    }
                    QGCLabel {
                        text:           qsTr("Less")
                        opacity:        0.7
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                    QGCSlider {
                        id:                     sensitivitySlider
                        Layout.fillWidth:       true
                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.8
                        from:                   0
                        to:                     3
                        stepSize:               1
                        snapMode:               Slider.SnapAlways
                        value:                  _sampFact ? parent.nearestIndex(_sampFact.rawValue) : 2
                        // Only commit the param when the user releases the handle —
                        // pressed transitions to false on mouse-up / touch-release.
                        // This prevents flooding the autopilot with intermediate
                        // values during a drag.
                        onPressedChanged: {
                            if (!pressed && _sampFact) {
                                _sampFact.rawValue = parent.samplesOptions[Math.round(value)]
                            }
                        }

                        // Override handle with a larger circle for easier dragging
                        // and a more visible position indicator.
                        handle: Rectangle {
                            x:              sensitivitySlider.leftPadding
                                            + sensitivitySlider.visualPosition * (sensitivitySlider.availableWidth - width)
                            y:              sensitivitySlider.topPadding
                                            + sensitivitySlider.availableHeight / 2 - height / 2
                            implicitWidth:  ScreenTools.defaultFontPixelHeight * 1.4
                            implicitHeight: ScreenTools.defaultFontPixelHeight * 1.4
                            radius:         width / 2
                            color:          qgcPal.button
                            border.color:   qgcPal.buttonText
                            border.width:   1
                        }
                    }
                    QGCLabel {
                        text:           qsTr("More")
                        opacity:        0.7
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                    QGCLabel {
                        text:           _sampFact ? qsTr("(%1)").arg(_sampFact.rawValue) : ""
                        Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 4
                        opacity:        0.7
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                }

                // Trigger Condition (FWDAVD_COND): 0 None, 1 Above Home Alt, 2 Above Terrain Alt
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_condFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Trigger Condition")
                    }
                    QGCComboBox {
                        Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 18
                        model:        ["No Altitude Check", "Above Home Altitude", "Above Terrain Altitude"]
                        currentIndex: _condFact ? _condFact.rawValue : 0
                        onActivated:  function(index) { if (_condFact) _condFact.rawValue = index }
                    }
                }

                // Auto-mode action (FWDAVD_ACT_AUTO): 0 None, 1 Land, 2 RTL, 3 SmartRTL
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_actAutoFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Auto Mode Action")
                    }
                    QGCComboBox {
                        Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 18
                        model:        ["None", "Land", "RTL", "SmartRTL"]
                        currentIndex: _actAutoFact ? _actAutoFact.rawValue : 0
                        onActivated:  function(index) { if (_actAutoFact) _actAutoFact.rawValue = index }
                    }
                }

                // Manual-mode action (FWDAVD_ACT_MAN): 0 None, 1 Land, 2 RTL, 3 SmartRTL, 4 Stop
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_actManFact

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Manual Mode Action")
                    }
                    QGCComboBox {
                        Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 18
                        model:        ["None", "Land", "RTL", "SmartRTL", "Stop with Lockout"]
                        currentIndex: _actManFact ? _actManFact.rawValue : 0
                        onActivated:  function(index) { if (_actManFact) _actManFact.rawValue = index }
                    }
                }

                // Altitude threshold (FWDAVD_ALT): only shown if Trigger Condition > 0.
                // Param is in meters, displayed in vertical units. Clamp-on-finish to 0..100 m.
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            ScreenTools.defaultFontPixelWidth
                    visible:            !!_altFact && !!_condFact && _condFact.rawValue > 0

                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("Altitude Threshold (%1)").arg(_unitsConversion.appSettingsVerticalDistanceUnitsString)
                    }
                    QGCTextField {
                        id:                     altField
                        Layout.minimumWidth:    ScreenTools.defaultFontPixelWidth * 10
                        inputMethodHints:       Qt.ImhFormattedNumbersOnly
                        text: _altFact
                              ? _unitsConversion.metersToAppSettingsVerticalDistanceUnits(_altFact.rawValue).toFixed(1)
                              : "--"

                        onEditingFinished: {
                            if (!_altFact) return
                            var v = parseFloat(text)
                            if (isNaN(v)) v = 0
                            var maxDisp = _unitsConversion.metersToAppSettingsVerticalDistanceUnits(100)
                            v = Math.max(0, Math.min(maxDisp, v))
                            var meters = _unitsConversion.appSettingsVerticalDistanceUnitsToMeters(v)
                            _altFact.rawValue = meters
                            text = v.toFixed(1)
                        }

                        Connections {
                            target: _altFact
                            function onRawValueChanged() {
                                altField.text = _altFact
                                    ? _unitsConversion.metersToAppSettingsVerticalDistanceUnits(_altFact.rawValue).toFixed(1)
                                    : "--"
                            }
                        }
                    }
                }

                QGCLabel {
                    Layout.fillWidth:   true
                    visible:            !_btnPresent
                    text:               qsTr("FWDAVD_BTN_EN parameter not found. Update firmware to use the GCS button.")
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

        // Container for icon with status indicator dot + blocking alert badge
        Item {
            width:                  rangefinderIcon.width + (statusDot.visible ? statusDot.width * 0.5 : 0)
            anchors.top:            parent.top
            anchors.bottom:         parent.bottom

            // Icon is dimmed when there's no useful sensor data to color it by:
            // status 0 (off) and 1 (standby/no useful reading). States 2/3/4 are
            // all "we have a meaningful reading" — icon bright at full opacity.
            property bool _iconDimmed: _liveStatusKnown ? (_liveStatus <= 1)
                                                        : (_btnPresent && !_btnEnabled)

            // Forward rangefinder icon (rotated radar)
            QGCColoredImage {
                id:                     rangefinderIcon
                width:                  height
                anchors.top:            parent.top
                anchors.bottom:         parent.bottom
                sourceSize.height:      height
                source:                 "/InstrumentValueIcons/radar-rangefinder2.svg"
                fillMode:               Image.PreserveAspectFit
                rotation:               -90
                color:                  parent._iconDimmed ? qgcPal.colorGrey : getIconColor()
                opacity:                parent._iconDimmed ? 0.5 : 1.0
            }

            // Status indicator dot (bottom-right corner of icon)
            Rectangle {
                id:                     statusDot
                width:                  ScreenTools.defaultFontPixelHeight * 0.6
                height:                 width
                radius:                 width / 2
                color:                  getDotColor()
                visible:                _btnPresent || _liveStatusKnown
                anchors.bottom:         rangefinderIcon.bottom
                anchors.right:          rangefinderIcon.right
                anchors.bottomMargin:   -height * 0.1
                anchors.rightMargin:    -width * 0.1

                border.width:           1
                border.color:           "white"
            }

            // Blocking alert badge — flashing red circle with white "!" centered
            // over the icon. Visible only when the autopilot is actively firing
            // an avoidance action (status 4). 500 ms cycle (250 up / 250 down).
            Rectangle {
                id:                     alertBadge
                visible:                _blocking
                width:                  rangefinderIcon.width * 0.6
                height:                 width
                radius:                 width / 2
                color:                  qgcPal.colorRed
                border.color:           "white"
                border.width:           2
                anchors.horizontalCenter: rangefinderIcon.horizontalCenter
                anchors.verticalCenter:   rangefinderIcon.verticalCenter

                SequentialAnimation on opacity {
                    loops:   Animation.Infinite
                    running: alertBadge.visible
                    NumberAnimation { to: 1.0; duration: 250 }
                    NumberAnimation { to: 0.3; duration: 250 }
                }

                QGCLabel {
                    anchors.centerIn: parent
                    text:             "!"
                    color:            "white"
                    font.bold:        true
                    font.pointSize:   ScreenTools.mediumFontPointSize
                }
            }
        }

        // Distance value text
        QGCLabel {
            anchors.verticalCenter: parent.verticalCenter
            text:                   _isActive ? _displayDistanceStr : "--"
            color: {
                var dimmed = _liveStatusKnown ? (_liveStatus <= 1)
                                              : (_btnPresent && !_btnEnabled)
                return dimmed ? qgcPal.colorGrey : getIconColor()
            }
            font.pointSize:         ScreenTools.mediumFontPointSize
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked:      mainWindow.showIndicatorDrawer(forwardRangefinderInfoPage, control)
    }
}
