/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightMap

// Adds visual items associated with the Flight Plan to the map.
// Currently only used by Fly View even though it's called PlanMapItems!
Item {
    id: _root

    property var    map                     ///< Map control to show items on
    property bool   largeMapView            ///< true: map takes up entire view, false: map is in small window
    property var    planMasterController    ///< Reference to PlanMasterController for vehicle
    property var    vehicle                 ///< Vehicle associated with these items

    property var    _map:                       map
    property var    _vehicle:                   vehicle
    property var    _missionController:         planMasterController.missionController
    property var    _geoFenceController:        planMasterController.geoFenceController
    property var    _rallyPointController:      planMasterController.rallyPointController
    property var    _guidedController:          globals.guidedControllerFlyView
    property var    _missionLineViewComponent

    property string fmode: vehicle.flightMode
    // Master switch — Fly View only. Hides waypoint markers, the polyline,
    // and direction arrows in one go. The mission stays loaded.
    property bool   _showMissionOnMap: QGroundControl.settingsManager.flyViewSettings.showMissionOnMap.rawValue

    // Index of the first and last MAV_CMD_NAV_WAYPOINT items in visualItems.
    // Used by the "First and last only" Fly View setting so the markers attach
    // to actual scan endpoints rather than Mission Settings / Takeoff / Land /
    // RTL. Recomputed when visualItems changes (i.e. mission loads/edits).
    readonly property int _navWaypointCmd: 16 // MAV_CMD_NAV_WAYPOINT
    property int _firstRealWaypointIndex: -1
    property int _lastRealWaypointIndex:  -1

    function _updateRealWaypointIndices() {
        var items = _missionController ? _missionController.visualItems : null
        var first = -1
        var last  = -1
        if (items) {
            for (var i = 0; i < items.count; i++) {
                var item = items.get(i)
                if (item && item.isSimpleItem && item.command === _navWaypointCmd) {
                    if (first === -1) first = i
                    last = i
                }
            }
        }
        _firstRealWaypointIndex = first
        _lastRealWaypointIndex  = last
    }

    Connections {
        target: _missionController
        function onVisualItemsChanged() { _updateRealWaypointIndices() }
    }

    // Add the mission item visuals to the map
    Repeater {
        id: missionItemRepeater
        model: (largeMapView && _showMissionOnMap) ? _missionController.visualItems : 0

        delegate: MissionItemMapVisual {
            map:            _map
            vehicle:        _vehicle
            isFirstItem:    index === _firstRealWaypointIndex
            isLastItem:     index === _lastRealWaypointIndex
            onClicked:      _guidedController.confirmAction(_guidedController.actionSetWaypoint, Math.max(object.sequenceNumber, 1))
        }
    }

    Component.onCompleted: {
        _updateRealWaypointIndices()
        _missionLineViewComponent = missionLineViewComponent.createObject(map)
        if (_missionLineViewComponent.status === Component.Error)
            console.log(_missionLineViewComponent.errorString())
        map.addMapItemGroup(_missionLineViewComponent)
    }

    Component.onDestruction: {
        if (_missionLineViewComponent) {
            // Must remove MapItemGroup before destruction, otherwise we crash on quit
            map.removeMapItemGroup(_missionLineViewComponent)
            _missionLineViewComponent.destroy()
        }
    }

    Component {
        id: missionLineViewComponent

        MapItemGroup {
            visible: _showMissionOnMap
            MissionLineView {
                model: _missionController.simpleFlightPathSegments
            }

            MapItemView {
                model: _missionController.directionArrows

                delegate: MapLineArrow {
                    fromCoord:      object ? object.coordinate1 : undefined
                    toCoord:        object ? object.coordinate2 : undefined
                    arrowPosition:  3
                    z:              QGroundControl.zOrderWaypointLines + 1
                }
            }
        }
    }
}
