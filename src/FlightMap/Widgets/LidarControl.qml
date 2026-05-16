/****************************************************************************
 * LiDAR Control Widget
 *
 * Shown in place of PhotoVideoControl when LiDAR payload mode is selected.
 * Provides quick-access Start Pattern buttons from the fly view.
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import QGroundControl
import QGroundControl.ScreenTools
import QGroundControl.Controls
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls

Rectangle {
    id:         _root
    width:      _panelWidth
    height:     mainColumn.implicitHeight + (_margins * 2)
    color:      Qt.rgba(qgcPal.window.r, qgcPal.window.g, qgcPal.window.b, 0.5)
    radius:     _margins

    property real   _margins:       ScreenTools.defaultFontPixelHeight / 2
    property real   _smallMargins:  ScreenTools.defaultFontPixelWidth / 2
    property real   _buttonHeight:  ScreenTools.defaultFontPixelHeight * 3
    property real   _panelWidth:    ScreenTools.defaultFontPixelWidth * 30
    property var    _activeVehicle: globals.activeVehicle

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    FactPanelController { id: controller }

    // Bumped on every factAdded so the Fact bindings below re-evaluate as Lua script params trickle in
    property int _factReloadTrigger: 0

    Connections {
        target: _activeVehicle ? _activeVehicle.parameterManager : null
        function onFactAdded(componentId, fact) {
            if (fact && fact.name && fact.name.indexOf("PTRN_") === 0) {
                _factReloadTrigger++
            }
        }
    }

    // _factReloadTrigger referenced to force re-eval when params arrive later
    property Fact _ptrnTrigger: (_factReloadTrigger, _activeVehicle) ? controller.getParameterFact(-1, "PTRN_TRIGGER", false) : null
    property Fact _ptrnPnum:    (_factReloadTrigger, _activeVehicle) ? controller.getParameterFact(-1, "PTRN_PNUM",    false) : null

    // Calibration 1 is available for all Pattern Types; Calibration 2 only for "Figure-8 Continuous" (PTRN_PNUM === 1)
    property bool _showCal1: !!_ptrnTrigger
    property bool _showCal2: !!_ptrnTrigger && !!_ptrnPnum && _ptrnPnum.value === 1

    DeadMouseArea { anchors.fill: parent }

    ColumnLayout {
        id:                 mainColumn
        anchors.fill:       parent
        anchors.margins:    _margins
        spacing:            _margins

        // ── Title ──
        QGCLabel {
            Layout.alignment:   Qt.AlignHCenter
            text:               qsTr("LiDAR Calibration")
            font.pointSize:     ScreenTools.mediumFontPointSize
            font.bold:          true
        }

        // ── Params missing warning ──
        QGCLabel {
            Layout.fillWidth:   true
            visible:            !_ptrnTrigger
            text:               qsTr("PTRN_ parameters not found.\nEnsure the LiDAR pattern script is loaded.")
            wrapMode:           Text.WordWrap
            color:              qgcPal.colorOrange
            font.pointSize:     ScreenTools.smallFontPointSize
            horizontalAlignment: Text.AlignHCenter
        }

        // ── Start Calibration 1 (all Pattern Types) ──
        QGCButton {
            Layout.fillWidth:       true
            Layout.preferredHeight: _buttonHeight
            text:                   qsTr("Start Pattern 1")
            enabled:                _activeVehicle && _activeVehicle.armed && _ptrnTrigger
            visible:                _showCal1
            onClicked:              { if (_ptrnTrigger) _ptrnTrigger.value = 1 }
        }

        // ── Start Calibration 2 (Figure-8 Continuous only) ──
        QGCButton {
            Layout.fillWidth:       true
            Layout.preferredHeight: _buttonHeight
            text:                   qsTr("Start Pattern 2")
            enabled:                _activeVehicle && _activeVehicle.armed && _ptrnTrigger
            visible:                _showCal2
            onClicked:              { if (_ptrnTrigger) _ptrnTrigger.value = 2 }
        }

        // ── Arm hint ──
        QGCLabel {
            Layout.alignment:   Qt.AlignHCenter
            Layout.fillWidth:   true
            visible:            _showCal1 && _activeVehicle && !_activeVehicle.armed
            text:               qsTr("Arm the vehicle to start a pattern")
            font.pointSize:     ScreenTools.smallFontPointSize
            color:              qgcPal.colorOrange
            horizontalAlignment: Text.AlignHCenter
            wrapMode:           Text.WordWrap
        }
    }
}
