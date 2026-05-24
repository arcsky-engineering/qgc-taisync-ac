import QtQuick
import QtQuick.Controls

import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.Palette
import QGroundControl.Controllers

// Xplorer fork: stripped to a single calibration-status line per sensor class.
// Hardware IDs and priority labels (Primary/Secondary/External etc.) were removed
// as technical noise; the only end-user-actionable signal here is "needs calibration".
Item {
    anchors.fill: parent

    APMSensorsComponentController { id: controller }

    APMSensorParams {
        id:                     sensorParams
        factPanelController:    controller
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    function compassesNeedSetup() {
        for (var i = 0; i < sensorParams.rgCompassAvailable.length; i++) {
            if (sensorParams.rgCompassAvailable[i] && !sensorParams.rgCompassCalibrated[i]) {
                return true
            }
        }
        return false
    }

    Column {
        anchors.fill:   parent
        spacing:        2

        VehicleSummaryRow {
            labelText: qsTr("Compass")
            valueText: compassesNeedSetup() ? qsTr("Calibration required") : qsTr("Ready")
        }

        VehicleSummaryRow {
            labelText: qsTr("Accelerometer")
            valueText: controller.accelSetupNeeded ? qsTr("Calibration required") : qsTr("Ready")
        }

        VehicleSummaryRow {
            labelText: qsTr("Barometer")
            valueText: qsTr("Ready")
        }
    }
}
