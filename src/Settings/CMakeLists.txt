target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ADSBVehicleManagerSettings.cc
        ADSBVehicleManagerSettings.h
        AirDataSettings.cc
        AirDataSettings.h
        AirDataSyncManager.cc
        AirDataSyncManager.h
        AppSettings.cc
        AppSettings.h
        AutoConnectSettings.cc
        AutoConnectSettings.h
        BatteryIndicatorSettings.cc
        BatteryIndicatorSettings.h
        BrandImageSettings.cc
        BrandImageSettings.h
        MavlinkActionsSettings.cc
        MavlinkActionsSettings.h
        FirmwareUpgradeSettings.cc
        FirmwareUpgradeSettings.h
        FlightMapSettings.cc
        FlightMapSettings.h
        FlightModeSettings.cc
        FlightModeSettings.h
        FlyViewSettings.cc
        FlyViewSettings.h
        GimbalControllerSettings.cc
        GimbalControllerSettings.h
        MavlinkSettings.cc
        MavlinkSettings.h
        OfflineMapsSettings.cc
        OfflineMapsSettings.h
        PlanViewSettings.cc
        PlanViewSettings.h
        RemoteIDSettings.cc
        RemoteIDSettings.h
        RTKSettings.cc
        RTKSettings.h
        SettingsGroup.cc
        SettingsGroup.h
        SettingsManager.cc
        SettingsManager.h
        TelemetryLogManager.cc
        TelemetryLogManager.h
        UnitsSettings.cc
        UnitsSettings.h
        VideoSettings.cc
        VideoSettings.h
        MapsSettings.cc
        MapsSettings.h
        Viewer3DSettings.cc
        Viewer3DSettings.h
	NTRIPSettings.cc
	NTRIPSettings.h
)

if(NOT QGC_DISABLE_APM_MAVLINK)
    target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
            APMMavlinkStreamRateSettings.cc
            APMMavlinkStreamRateSettings.h
    )
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# The AirData app key is scoped to the single file that reads it, so changing the
# key recompiles one translation unit and relinks, instead of rebuilding the whole
# app the way a target-wide definition would. Undefined when no key was configured,
# which is how AirDataSyncManager tells an unconfigured build from one with a key.
#
# TARGET_DIRECTORY is required, not optional: source properties apply only to
# targets defined in the directory where the property is set, and the app target
# is defined in the top level CMakeLists.txt, not here. Without it the definition
# silently never reaches the compile line.
if(QGC_AIRDATA_APP_KEY)
    set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/AirDataSyncManager.cc
        TARGET_DIRECTORY ${CMAKE_PROJECT_NAME}
        APPEND PROPERTY COMPILE_DEFINITIONS QGC_AIRDATA_APP_KEY="${QGC_AIRDATA_APP_KEY}")
endif()
