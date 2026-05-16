/****************************************************************************
 * Toolbar MAVLink Sender Indicator
 ****************************************************************************/

import QtQuick
import QGroundControl
import QGroundControl.Controls
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette

Item {
    id: root
    width: indicatorRow.width
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    // Hide while armed for ILX (0) and VIO (1) — there's no in-flight info or action needed.
    // Keep visible for LiDAR (2) so the pattern start buttons remain reachable in flight.
    property int  _selectedPayload:  QGroundControl.settingsManager.flyViewSettings.payloadSelection.value
    property bool _vehicleArmed:     activeVehicle && activeVehicle.armed
    property bool showIndicator:     QGroundControl.settingsManager.flyViewSettings.showPayloadIndicator.value
                                     && (!_vehicleArmed || _selectedPayload === 2)
    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool savingPopupVisible: false
    property bool savingShown: false
    property bool completionShown: false


    Connections {
        target: activeVehicle

        // -----------------------------
        // STATUS CHANGED (Saving popup)
        // -----------------------------
        onGeoStatusChanged: {
            if (!activeVehicle) return

            // Entered Saving mode
            if (activeVehicle.geoSessionStatus === 3 && !savingPopupVisible) {
                savingPopupVisible = true
                mainWindow.showMessageDialog(
                    "Geotagging in Progress",
                    "Geotagging started for " + activeVehicle.imageCount + " photos.\n\n" +
                    "Do not turn off the system until geotagging is complete!"
                )
            }

            // Left Saving mode → auto-close the dialog
            else if (activeVehicle.geoSessionStatus !== 3 && savingPopupVisible) {
                savingPopupVisible = false
                //mainWindow.closeMessageDialog()      // auto-close saving dialog
            }
        }

        // -----------------------------
        // COMPLETION POPUP
        // -----------------------------
        onGeoCompletedTriggered: {
            // Ensure saving popup is closed before showing completion dialog
            if (savingPopupVisible) {
                savingPopupVisible = false
                //mainWindow.closeMessageDialog()
            }

            mainWindow.showMessageDialog(
                "Geotagging Complete",
                "Geotagging finished successfully.\n" +
                activeVehicle.geoFinalImageCount + " photos processed."
            )
        }
    }

    Row {
        id: indicatorRow
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: ScreenTools.defaultFontPixelWidth / 2

        // ICON + "Payload" caption. _iconScale visually enlarges the icon ~50%; the SVG is
        // 250x160 (1.56:1) so we use a wider-than-square box plus the scale factor to give
        // the camera body more room. The icon is allowed to overflow the column's vertical
        // bounds — toolbar parent doesn't clip its children.
        Column {
            id: iconColumn
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 0

            property real _iconScale: 1.05

            QGCColoredImage {
                id: icon
                anchors.horizontalCenter: parent.horizontalCenter
                height: (iconColumn.height - payloadLabel.height) * iconColumn._iconScale
                // SVG is 250x160 (1.56:1) — a square box wastes vertical space, so use the
                // SVG's natural aspect ratio.
                width: height * (250 / 160)
                source: "/qmlimages/CameraIcon.svg"
                fillMode: Image.PreserveAspectFit
                sourceSize.height: height
                // Always white — geotag status is shown in PhotoVideoControl for ILX, and
                // status colors here would be meaningless for VIO or LiDAR.
                color: "white"
            }

            QGCLabel {
                id: payloadLabel
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Payload")
                font.pointSize: ScreenTools.smallFontPointSize
                color: qgcPal.buttonText
            }
        }

    }

    MouseArea {
        anchors.fill: parent
        onClicked: mainWindow.showIndicatorDrawer(senderPage, root)
    }

    Component {
        id: senderPage
        MavlinkSenderPage { }
    }
}
