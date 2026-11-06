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
import QGroundControl.Palette
import QGroundControl.ScreenTools

// On-screen ILX quick controls: shutter / aperture / ISO steppers and focus (AF, Focus & Lock,
// manual near/far) for real-time adjustment without opening ILX Settings. Semi-transparent,
// docked to the middle of the left edge, collapsible to a tab. Commands and TAG-E readback are
// implemented on Vehicle (apStepExposure, apFocusAndLock, apFocusDrive*), shared with the
// ILX Settings panel. Values shown are the TAG-E readback; a stepped value shows amber until
// the readback changes (or 3s pass).
Item {
    id:         _root
    width:      row.width
    height:     row.height
    // Hidden while the FPV stream is selected — these controls belong to the payload (CAM)
    // camera, same rule that collapses the photo/video panel (FlyViewTopRightColumnLayout.fpvLocked).
    visible:    _vehicle && _isIlx && _vehicle.airPixelComponentId > 0 && !_fpvSelected

    property var  _vehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property bool _isIlx:       QGroundControl.settingsManager.flyViewSettings.payloadSelection.value === 0
    property bool _fpvSelected: QGroundControl.videoManager.currentStream === "1"
    property bool _open:    QGroundControl.loadBoolGlobalSetting(_openKey, true)
    property string _openKey: "IlxQuickControlsOpen"

    // Everything is scaled up from the stock control sizes so it's easy to hit on a tablet.
    property real _s:       1.5
    property real _fontPt:  ScreenTools.defaultFontPointSize * _s
    property real _smallPt: ScreenTools.smallFontPointSize * _s
    property real _btn:     ScreenTools.defaultFontPixelHeight * 1.8 * _s
    property real _gap:     ScreenTools.defaultFontPixelWidth * 0.5 * _s
    property real _valueW:  ScreenTools.defaultFontPixelWidth * 8 * _s
    property color _amber:  qgcPal.colorOrange
    property color _bg:     Qt.rgba(0, 0, 0, 0.35)    // buttons and collapse tab; dark so they read over bright sky too

    readonly property int  _mode:         _vehicle ? _vehicle.apExpMode : 0     // 1=M 2=P 3=A 4=S
    readonly property bool _shutterAuto:  _mode === 2 || _mode === 3
    readonly property bool _apertureAuto: _mode === 2 || _mode === 4
    readonly property bool _isoAuto:      _vehicle && _vehicle.apISOAuto
    readonly property bool _mf:           _vehicle && _vehicle.apAFMode === 1

    readonly property string _modeText:     ["--", "M", "P", "A", "S"][_mode] || "--"
    readonly property string _modeName:     [qsTr("Mode --"), qsTr("Manual"), qsTr("Program"), qsTr("Aperture Pri."), qsTr("Shutter Pri.")][_mode] || qsTr("Mode --")
    readonly property string _shutterText:  _shutterAuto ? qsTr("Auto")
                                            : (_vehicle && _vehicle.apShutterSpeed > 0 ? "1/" + _vehicle.apShutterSpeed : "--")
    readonly property string _apertureText: _apertureAuto ? qsTr("Auto")
                                            : (_vehicle && _vehicle.apAperture > 0 ? "f/" + _vehicle.apAperture.toFixed(1) : "--")
    readonly property string _isoText:      !_vehicle ? "--"
                                            : (_isoAuto ? qsTr("Auto") : (_vehicle.apCameraISO > 0 ? _vehicle.apCameraISO.toString() : "--"))
    readonly property string _afText:       !_vehicle ? "--"
                                            : (_mf ? qsTr("MF lock") : _vehicle.apAFMode === 2 ? "AF-S" : _vehicle.apAFMode === 32772 ? "AF-C" : "AF")

    property int _focusSpeed: 3     // manual focus drive speed, 1..7

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    // See-through buttons floating directly on the video (no panel behind them). Text stays
    // solid white for legibility; QGCButton's own highlight overlay still shows presses.
    // Bare labels get a dark outline (ShadowLabel) so they read over any scene.
    component ShadowLabel: QGCLabel {
        font.pointSize: _fontPt
        style:      Text.Outline
        styleColor: Qt.rgba(0, 0, 0, 0.8)
    }
    component QuickButton: QGCButton {
        backgroundColor:    _bg
        pointSize:          _fontPt
        textColor:          enabled ? "white" : Qt.rgba(1, 1, 1, 0.4)
        showBorder:         false
    }

    function _setOpen(open) {
        _open = open
        QGroundControl.saveBoolGlobalSetting(_openKey, open)
    }

    // ── Pending readback ──
    property string _pendKey: ""
    property var    _pendVal
    Timer { id: pendTimer; interval: 3000; onTriggered: _pendKey = "" }
    function _valueFor(key) {
        switch (key) {
        case "shutter":  return _vehicle.apShutterSpeed
        case "aperture": return _vehicle.apAperture
        case "iso":      return _vehicle.apCameraISO
        }
        return undefined
    }
    function _step(key, dir) {
        if (!_vehicle) return
        _pendVal = _valueFor(key)
        _pendKey = key
        pendTimer.restart()
        _vehicle.apStepExposure(key === "aperture" ? dir : 0, key === "iso" ? dir : 0, key === "shutter" ? dir : 0)
    }
    Connections {
        target: _vehicle
        function onApCameraChanged() {
            if (_pendKey !== "" && _valueFor(_pendKey) !== _pendVal) _pendKey = ""
        }
    }

    RowLayout {
        id:         row
        spacing:    0

        // ── Panel ──
        Rectangle {
            Layout.preferredWidth:  content.implicitWidth + _gap * 4
            Layout.preferredHeight: content.implicitHeight + _gap * 4
            visible:                _open
            color:                  "transparent"   // buttons float on their own
            radius:                 ScreenTools.defaultFontPixelWidth

            DeadMouseArea { anchors.fill: parent }

            ColumnLayout {
                id:                 content
                anchors.centerIn:   parent
                spacing:            _gap

                // Header: exposure mode + focus state (mode is changed in ILX Settings)
                RowLayout {
                    Layout.fillWidth:   true
                    ShadowLabel {
                        text:           _modeName
                        font.bold:      true
                        color:          "white"
                    }
                    Item { Layout.fillWidth: true }
                    ShadowLabel {
                        text:           _afText
                        font.bold:      true
                        font.pointSize: _smallPt
                        color:          _mf ? _amber : "white"
                    }
                }

                // Shutter / Aperture / ISO
                Repeater {
                    model: [
                        { key: "shutter",  label: qsTr("Shutter") },
                        { key: "aperture", label: qsTr("Aperture") },
                        { key: "iso",      label: qsTr("ISO") }
                    ]

                    RowLayout {
                        id:         stepRow
                        spacing:    _gap

                        readonly property bool _locked: modelData.key === "shutter"  ? _shutterAuto
                                                      : modelData.key === "aperture" ? _apertureAuto
                                                      : _isoAuto

                        QuickButton {
                            text:                   "−"
                            Layout.preferredWidth:  _btn
                            Layout.preferredHeight: _btn
                            enabled:                !stepRow._locked
                            onClicked:              _step(modelData.key, -1)
                        }

                        // Value. ISO: tap toggles Auto.
                        Item {
                            Layout.preferredWidth:  _valueW
                            Layout.preferredHeight: _btn
                            opacity:                stepRow._locked && modelData.key !== "iso" ? 0.5 : 1.0

                            ColumnLayout {
                                anchors.centerIn:   parent
                                spacing:            0
                                ShadowLabel {
                                    Layout.alignment:   Qt.AlignHCenter
                                    text:               modelData.key === "iso" ? qsTr("ISO (tap: A)") : modelData.label
                                    font.pointSize:     _smallPt
                                    color:              "white"
                                    opacity:            0.8
                                }
                                ShadowLabel {
                                    Layout.alignment:   Qt.AlignHCenter
                                    font.bold:          true
                                    color:              _pendKey === modelData.key ? _amber : "white"
                                    text:               modelData.key === "shutter"  ? _shutterText
                                                      : modelData.key === "aperture" ? _apertureText
                                                      : _isoText
                                }
                            }

                            MouseArea {
                                anchors.fill:   parent
                                enabled:        modelData.key === "iso"
                                onClicked:      if (_vehicle) _vehicle.apToggleAutoIso()
                            }
                        }

                        QuickButton {
                            text:                   "+"
                            Layout.preferredWidth:  _btn
                            Layout.preferredHeight: _btn
                            enabled:                !stepRow._locked
                            onClicked:              _step(modelData.key, 1)
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Qt.rgba(1, 1, 1, 0.25) }

                // Focus, AF modes: autofocus once, or focus and lock (switches to MF)
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            _gap
                    visible:            !_mf
                    QuickButton {
                        text:                   qsTr("AF")
                        Layout.fillWidth:       true
                        Layout.preferredHeight: _btn
                        onClicked:              if (_vehicle) _vehicle.apCameraCommand(135)
                    }
                    QuickButton {
                        text:                   qsTr("Lock")
                        Layout.fillWidth:       true
                        Layout.preferredHeight: _btn
                        onClicked:              if (_vehicle) _vehicle.apFocusAndLock()
                    }
                }

                // Focus, MF (locked): hold « / » to drive near / far; tap the speed to change it.
                // Hidden unless enabled in ILX Settings > Advanced (manual focus drive unconfirmed).
                RowLayout {
                    Layout.fillWidth:   true
                    spacing:            _gap
                    visible:            _mf && QGroundControl.settingsManager.flyViewSettings.ilxManualFocusControls.rawValue
                    onVisibleChanged:   if (!visible && _vehicle) _vehicle.apFocusDriveStop()

                    QuickButton {
                        text:                   "«"
                        Layout.preferredWidth:  _btn
                        Layout.preferredHeight: _btn
                        onPressed:              if (_vehicle) _vehicle.apFocusDriveStart(-_focusSpeed)
                        onReleased:             if (_vehicle) _vehicle.apFocusDriveStop()
                        onCanceled:             if (_vehicle) _vehicle.apFocusDriveStop()
                    }
                    QuickButton {
                        text:                   qsTr("Spd %1").arg(_focusSpeed)
                        Layout.fillWidth:       true
                        Layout.preferredHeight: _btn
                        pointSize:              _smallPt
                        onClicked:              _focusSpeed = [1, 2, 3, 5, 7][([1, 2, 3, 5, 7].indexOf(_focusSpeed) + 1) % 5]
                    }
                    QuickButton {
                        text:                   "»"
                        Layout.preferredWidth:  _btn
                        Layout.preferredHeight: _btn
                        onPressed:              if (_vehicle) _vehicle.apFocusDriveStart(_focusSpeed)
                        onReleased:             if (_vehicle) _vehicle.apFocusDriveStop()
                        onCanceled:             if (_vehicle) _vehicle.apFocusDriveStop()
                    }
                }
                QuickButton {
                    text:                   qsTr("Refocus & Lock")
                    Layout.fillWidth:       true
                    Layout.preferredHeight: _btn
                    visible:                _mf
                    onClicked:              if (_vehicle) _vehicle.apFocusAndLock()
                }
            }
        }

        // ── Collapse tab: always shown; collapsed it carries a one-line read-out ──
        Rectangle {
            Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 1.6 * _s
            Layout.preferredHeight: _open ? ScreenTools.defaultFontPixelHeight * 4 * _s : tabText.implicitWidth + ScreenTools.defaultFontPixelHeight * 2.5 * _s
            color:                  _bg
            radius:                 ScreenTools.defaultFontPixelWidth

            ShadowLabel {
                id:                 chevron
                anchors.top:        parent.top
                anchors.topMargin:  ScreenTools.defaultFontPixelHeight * 0.3
                anchors.horizontalCenter: parent.horizontalCenter
                text:               _open ? "‹" : "›"
                font.bold:          true
                font.pointSize:     ScreenTools.largeFontPointSize * _s
                color:              "white"
            }

            ShadowLabel {
                id:                 tabText
                anchors.centerIn:   parent
                anchors.verticalCenterOffset: ScreenTools.defaultFontPixelHeight * 0.5 * _s
                rotation:           -90
                visible:            !_open
                text:               _modeText + "  " + _shutterText + "  " + _apertureText + "  ISO " + _isoText
                font.pointSize:     _smallPt
                font.bold:          true
                color:              "white"
            }

            MouseArea {
                anchors.fill:   parent
                onClicked:      _setOpen(!_open)
            }
        }
    }
}
