/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick

import QGroundControl.Controls
import QGroundControl.Palette
import QGroundControl.ScreenTools

SettingsButton {
    id: control

    icon.color: setupComplete ? textColor : "red"

    property bool setupComplete:    true
    // Xplorer fork: when true, render a small green/red status dot in the top-right
    // corner of the button. Used in the vehicle setup sidebar to surface
    // calibration state for components that report a meaningful setupComplete signal
    // (e.g. Sensors). Components whose status never changes (Radio, Power, Safety
    // with locked params) leave this false to avoid permanent green-dot noise.
    property bool showStatusDot:    false

    QGCPalette { id: _qgcPal; colorGroupEnabled: control.enabled }

    Rectangle {
        id:                     statusDot
        visible:                control.showStatusDot
        width:                  ScreenTools.defaultFontPixelWidth * 0.9
        height:                 width
        radius:                 width / 2
        color:                  control.setupComplete ? "#00d932" : "red"
        border.width:           1
        border.color:           "white"
        anchors.right:          parent.right
        anchors.top:            parent.top
        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth * 0.4
        anchors.topMargin:      ScreenTools.defaultFontPixelWidth * 0.4
    }
}
