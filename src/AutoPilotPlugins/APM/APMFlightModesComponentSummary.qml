import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.Palette

// Xplorer fork: Switch Options summary for both variants. The only difference is
// the channel range — Xplorer reserves RC5..RC8 for locked flight-mode aux switches
// (AltHold/Loiter/RTL/Auto via defaults.parm) so operators only use RC9..RC16;
// X55 has no reserved range so RC5..RC16 are all operator-assignable.
Item {
    anchors.fill:   parent

    FactPanelController { id: controller; }

    readonly property bool _isXplorer:    QGroundControl.settingsManager.appSettings.vehicleVariant.rawValue === 1
    readonly property int  _rcStart:      _isXplorer ? 9 : 5
    readonly property int  _rcCount:      16 - _rcStart + 1

    Column {
        anchors.fill: parent

        Repeater {
            model: _rcCount

            VehicleSummaryRow {
                labelText: qsTr("Channel %1").arg(index + _rcStart)
                valueText: {
                    var f = controller.getParameterFact(-1, "r.RC" + (index + _rcStart) + "_OPTION")
                    if (!f) return qsTr("Unused")
                    return f.enumStringValue ? f.enumStringValue : f.rawValue.toString()
                }
            }
        }
    }
}
