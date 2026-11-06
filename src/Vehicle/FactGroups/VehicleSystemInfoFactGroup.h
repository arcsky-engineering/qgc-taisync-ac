/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"

#include <QtCore/QElapsedTimer>

/// Arcsky aircraft identity / maintenance info, assembled from:
///   - boot banner STATUSTEXT        "Xplorer v1.0.3 (045b0843)" / "X55 v1.0.0 (...)"
///   - "~" metadata STATUSTEXT       ~ARCSKY,<model>,<serial>,<flight_hours>
///                                   ~BAT<n>,<serial>,<cycles>             (Xplorer; all-digit serial = empty bay)
///                                   ~BMS<n>,<serial>,<fw>                 (Xplorer, BMS interface board)
///                                   ~BIF,<fw>,<hw>                        (Xplorer, BMS interface board itself)
///                                   ~HM,<serial>,HW<v>,FW<v>,<rt_h>,<maint_h> (X55)
///   - BMS online STATUSTEXT         "BMS<n> Online: FW vX.Y.Z, SN:<sn>, Cycles:<c>"
///   - DATA64 type 200/201 (X55 META_TX bit 1) carrying the same text without the "~"
///   - payload components:
///       gimbal  GIMBAL_DEVICE_INFORMATION (requested by us, QGC never asks the device for it),
///               with the VERSION_X/Y/Z params as a fallback
///       camera  CAMERA_INFORMATION (requested by QGCCameraManager), and the boot STATUSTEXT
///               "...HW:<v> | SW:<v>" when we happen to be connected for it
/// Values are not persisted; they start empty for every new Vehicle.
class VehicleSystemInfoFactGroup : public FactGroup
{
    Q_OBJECT
    Q_PROPERTY(Fact *firmwareVersion            READ firmwareVersion            CONSTANT)
    Q_PROPERTY(Fact *model                      READ model                      CONSTANT)
    Q_PROPERTY(Fact *serialNumber               READ serialNumber               CONSTANT)
    Q_PROPERTY(Fact *flightHours                READ flightHours                CONSTANT)
    Q_PROPERTY(Fact *battery1Installed          READ battery1Installed          CONSTANT)
    Q_PROPERTY(Fact *battery1Serial             READ battery1Serial             CONSTANT)
    Q_PROPERTY(Fact *battery1FirmwareVersion    READ battery1FirmwareVersion    CONSTANT)
    Q_PROPERTY(Fact *battery1Cycles             READ battery1Cycles             CONSTANT)
    Q_PROPERTY(Fact *battery2Installed          READ battery2Installed          CONSTANT)
    Q_PROPERTY(Fact *battery2Serial             READ battery2Serial             CONSTANT)
    Q_PROPERTY(Fact *battery2FirmwareVersion    READ battery2FirmwareVersion    CONSTANT)
    Q_PROPERTY(Fact *battery2Cycles             READ battery2Cycles             CONSTANT)
    Q_PROPERTY(Fact *batteryInterfaceFirmwareVersion READ batteryInterfaceFirmwareVersion CONSTANT)
    Q_PROPERTY(Fact *batteryInterfaceHardwareVersion READ batteryInterfaceHardwareVersion CONSTANT)
    Q_PROPERTY(Fact *hybridSerial               READ hybridSerial               CONSTANT)
    Q_PROPERTY(Fact *hybridHardwareVersion      READ hybridHardwareVersion      CONSTANT)
    Q_PROPERTY(Fact *hybridFirmwareVersion      READ hybridFirmwareVersion      CONSTANT)
    Q_PROPERTY(Fact *hybridRuntimeHours         READ hybridRuntimeHours         CONSTANT)
    Q_PROPERTY(Fact *hybridMaintenanceHours     READ hybridMaintenanceHours     CONSTANT)
    Q_PROPERTY(Fact *gimbalFirmwareVersion      READ gimbalFirmwareVersion      CONSTANT)
    Q_PROPERTY(Fact *cameraModel                READ cameraModel                CONSTANT)
    Q_PROPERTY(Fact *cameraFirmwareVersion      READ cameraFirmwareVersion      CONSTANT)

public:
    explicit VehicleSystemInfoFactGroup(QObject *parent = nullptr);

    Fact *firmwareVersion() { return &_firmwareVersionFact; }
    Fact *model() { return &_modelFact; }
    Fact *serialNumber() { return &_serialNumberFact; }
    Fact *flightHours() { return &_flightHoursFact; }
    Fact *battery1Installed() { return &_battery1InstalledFact; }
    Fact *battery1Serial() { return &_battery1SerialFact; }
    Fact *battery1FirmwareVersion() { return &_battery1FirmwareVersionFact; }
    Fact *battery1Cycles() { return &_battery1CyclesFact; }
    Fact *battery2Installed() { return &_battery2InstalledFact; }
    Fact *battery2Serial() { return &_battery2SerialFact; }
    Fact *battery2FirmwareVersion() { return &_battery2FirmwareVersionFact; }
    Fact *battery2Cycles() { return &_battery2CyclesFact; }
    Fact *batteryInterfaceFirmwareVersion() { return &_batteryInterfaceFirmwareVersionFact; }
    Fact *batteryInterfaceHardwareVersion() { return &_batteryInterfaceHardwareVersionFact; }
    Fact *hybridSerial() { return &_hybridSerialFact; }
    Fact *hybridHardwareVersion() { return &_hybridHardwareVersionFact; }
    Fact *hybridFirmwareVersion() { return &_hybridFirmwareVersionFact; }
    Fact *hybridRuntimeHours() { return &_hybridRuntimeHoursFact; }
    Fact *hybridMaintenanceHours() { return &_hybridMaintenanceHoursFact; }
    Fact *gimbalFirmwareVersion() { return &_gimbalFirmwareVersionFact; }
    Fact *cameraModel() { return &_cameraModelFact; }
    Fact *cameraFirmwareVersion() { return &_cameraFirmwareVersionFact; }

    // Overrides from FactGroup
    void handleMessage(Vehicle *vehicle, const mavlink_message_t &message) final;

private:
    struct BatteryFacts {
        Fact *installed = nullptr;
        Fact *serial = nullptr;
        Fact *firmwareVersion = nullptr;
        Fact *cycles = nullptr;
    };

    void _handleStatusText(const mavlink_message_t &message);
    void _handleData64(const mavlink_message_t &message);
    bool _parseMetadata(const QString &line);
    BatteryFacts _batteryFacts(int index);
    void _setBatterySerial(const BatteryFacts &battery, const QString &serial);
    void _setBatteryEmpty(const BatteryFacts &battery);
    void _handlePayloadMessage(Vehicle *vehicle, const mavlink_message_t &message);
    void _requestGimbalInfo(Vehicle *vehicle, uint8_t compId);
    void _handleGimbalParamValue(const mavlink_message_t &message);
    void _handleCameraInformation(const mavlink_message_t &message);

    Fact _firmwareVersionFact = Fact(0, QStringLiteral("firmwareVersion"), FactMetaData::valueTypeString);
    Fact _modelFact = Fact(0, QStringLiteral("model"), FactMetaData::valueTypeString);
    Fact _serialNumberFact = Fact(0, QStringLiteral("serialNumber"), FactMetaData::valueTypeString);
    Fact _flightHoursFact = Fact(0, QStringLiteral("flightHours"), FactMetaData::valueTypeDouble);
    Fact _battery1InstalledFact = Fact(0, QStringLiteral("battery1Installed"), FactMetaData::valueTypeDouble);   ///< NaN unknown, 0 empty bay, 1 pack present
    Fact _battery1SerialFact = Fact(0, QStringLiteral("battery1Serial"), FactMetaData::valueTypeString);
    Fact _battery1FirmwareVersionFact = Fact(0, QStringLiteral("battery1FirmwareVersion"), FactMetaData::valueTypeString);
    Fact _battery1CyclesFact = Fact(0, QStringLiteral("battery1Cycles"), FactMetaData::valueTypeDouble);
    Fact _battery2InstalledFact = Fact(0, QStringLiteral("battery2Installed"), FactMetaData::valueTypeDouble);   ///< NaN unknown, 0 empty bay, 1 pack present
    Fact _battery2SerialFact = Fact(0, QStringLiteral("battery2Serial"), FactMetaData::valueTypeString);
    Fact _battery2FirmwareVersionFact = Fact(0, QStringLiteral("battery2FirmwareVersion"), FactMetaData::valueTypeString);
    Fact _battery2CyclesFact = Fact(0, QStringLiteral("battery2Cycles"), FactMetaData::valueTypeDouble);
    Fact _batteryInterfaceFirmwareVersionFact = Fact(0, QStringLiteral("batteryInterfaceFirmwareVersion"), FactMetaData::valueTypeString);
    Fact _batteryInterfaceHardwareVersionFact = Fact(0, QStringLiteral("batteryInterfaceHardwareVersion"), FactMetaData::valueTypeString);
    Fact _hybridSerialFact = Fact(0, QStringLiteral("hybridSerial"), FactMetaData::valueTypeString);
    Fact _hybridHardwareVersionFact = Fact(0, QStringLiteral("hybridHardwareVersion"), FactMetaData::valueTypeString);
    Fact _hybridFirmwareVersionFact = Fact(0, QStringLiteral("hybridFirmwareVersion"), FactMetaData::valueTypeString);
    Fact _hybridRuntimeHoursFact = Fact(0, QStringLiteral("hybridRuntimeHours"), FactMetaData::valueTypeDouble);
    Fact _hybridMaintenanceHoursFact = Fact(0, QStringLiteral("hybridMaintenanceHours"), FactMetaData::valueTypeDouble);
    Fact _gimbalFirmwareVersionFact = Fact(0, QStringLiteral("gimbalFirmwareVersion"), FactMetaData::valueTypeString);
    Fact _cameraModelFact = Fact(0, QStringLiteral("cameraModel"), FactMetaData::valueTypeString);
    Fact _cameraFirmwareVersionFact = Fact(0, QStringLiteral("cameraFirmwareVersion"), FactMetaData::valueTypeString);

    int _gimbalInfoRequestCount = 0;
    QElapsedTimer _gimbalInfoRequestTimer;
    int _gimbalVersionParts[3] = { -1, -1, -1 };    ///< VERSION_X/Y/Z param fallback
    bool _cameraFirmwareFromStatusText = false;     ///< boot text wins over CAMERA_INFORMATION decoding

    static constexpr int kGimbalInfoMaxRequests = 5;
    static constexpr qint64 kGimbalInfoRequestIntervalMs = 10000;

    static constexpr uint8_t kData64TypeIdentity = 200;     ///< ARCSKY_D64_IDENTITY in the X55 firmware
    static constexpr uint8_t kData64TypeHybridModule = 201; ///< ARCSKY_D64_HM in the X55 firmware
};
