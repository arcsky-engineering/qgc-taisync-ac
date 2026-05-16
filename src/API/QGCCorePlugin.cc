/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "QGCCorePlugin.h"
#include "QGCLogging.h"
#include "AppSettings.h"
#include "MavlinkSettings.h"
#include "FactMetaData.h"
#ifdef QGC_GST_STREAMING
#include "GStreamer.h"
#endif
#include "HorizontalFactValueGrid.h"
#include "InstrumentValueData.h"
#include "JoystickManager.h"
#include "LogDownloadController.h"
#include "MAVLinkLib.h"
#include "QGCLoggingCategory.h"
#include "QGCOptions.h"
#include "QmlComponentInfo.h"
#include "QmlObjectListModel.h"
#ifdef QGC_QT_STREAMING
#include "QtMultimediaReceiver.h"
#endif
#include "SettingsManager.h"
#include "FlyViewSettings.h"
#include "BatteryIndicatorSettings.h"
#include "UnitsSettings.h"
#include "VideoReceiver.h"

#ifdef QGC_CUSTOM_BUILD
#include CUSTOMHEADER
#endif

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtQml/qqml.h>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>

QGC_LOGGING_CATEGORY(QGCCorePluginLog, "qgc.api.qgccoreplugin");

#ifndef QGC_CUSTOM_BUILD
Q_APPLICATION_STATIC(QGCCorePlugin, _qgcCorePluginInstance);
#endif

QGCCorePlugin::QGCCorePlugin(QObject *parent)
    : QObject(parent)
    , _defaultOptions(new QGCOptions(this))
    , _emptyCustomMapItems(new QmlObjectListModel(this))
{
    // qCDebug(QGCCorePluginLog) << Q_FUNC_INFO << this;
}

QGCCorePlugin::~QGCCorePlugin()
{
    // qCDebug(QGCCorePluginLog) << Q_FUNC_INFO << this;
}

QGCCorePlugin *QGCCorePlugin::instance()
{
#ifndef QGC_CUSTOM_BUILD
    return _qgcCorePluginInstance();
#else
    return CUSTOMCLASS::instance();
#endif
}

void QGCCorePlugin::registerQmlTypes()
{
    (void) qmlRegisterUncreatableType<QGCCorePlugin>("QGroundControl", 1, 0, "QGCCorePlugin", QStringLiteral("Reference only"));
    (void) qmlRegisterUncreatableType<QGCOptions>("QGroundControl", 1, 0, "QGCOptions", QStringLiteral("Reference only"));
    (void) qmlRegisterUncreatableType<QGCFlyViewOptions>("QGroundControl", 1, 0, "QGCFlyViewOptions", QStringLiteral("Reference only"));
}

const QVariantList &QGCCorePlugin::analyzePages()
{
    static const QVariantList analyzeList = {
        // QVariant::fromValue(new QmlComponentInfo(
        //     tr("Log Download"),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/AnalyzeView/LogDownloadPage.qml")),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/LogDownloadIcon.svg")))),
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
        // QVariant::fromValue(new QmlComponentInfo(
        //     tr("GeoTag Images"),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/AnalyzeView/GeoTagPage.qml")),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/GeoTagIcon.svg")))),
#endif
        // QVariant::fromValue(new QmlComponentInfo(
        //     tr("MAVLink Console"),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/AnalyzeView/MAVLinkConsolePage.qml")),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/MAVLinkConsoleIcon.svg")))),
#ifndef QGC_DISABLE_MAVLINK_INSPECTOR
        QVariant::fromValue(new QmlComponentInfo(
            tr("MAVLink Inspector"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/AnalyzeView/MAVLinkInspectorPage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/MAVLinkInspector.svg")))),
#endif
        // QVariant::fromValue(new QmlComponentInfo(
        //     tr("Vibration"),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/AnalyzeView/VibrationPage.qml")),
        //     QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/VibrationPageIcon")))),
    };

    return analyzeList;
}

QGCOptions *QGCCorePlugin::options()
{
    return _defaultOptions;
}

const QmlObjectListModel *QGCCorePlugin::customMapItems()
{
    return _emptyCustomMapItems;
}

bool QGCCorePlugin::adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData)
{
    // One-time check: if vehicle variant changed since last run, clear variant-dependent
    // settings so the new defaults take effect. Without this, persisted values from the
    // old variant would stick even after switching.
    static bool variantChecked = false;
    if (!variantChecked) {
        variantChecked = true;
        QSettings settings;
        const uint currentVariant = settings.value("vehicleVariant", 1).toUInt();
        if (!settings.contains("_lastAppliedVariant")) {
            // First run with variant tracking — record current variant without clearing
            settings.setValue("_lastAppliedVariant", currentVariant);
        } else if (settings.value("_lastAppliedVariant").toUInt() != currentVariant) {
            // Variant changed — clear dependent settings so new defaults apply
            settings.beginGroup(FlyViewSettings::settingsGroup);
            settings.remove(FlyViewSettings::showForwardRangefinderName);
            settings.remove(FlyViewSettings::showDownRangefinderName);
            settings.remove(FlyViewSettings::showSimpleCameraControlName);
            settings.remove(FlyViewSettings::showPayloadIndicatorName);
            settings.remove(FlyViewSettings::enableMicROMName);
            settings.endGroup();
            settings.beginGroup(BatteryIndicatorSettings::settingsGroup);
            settings.remove(BatteryIndicatorSettings::valueDisplayName);
            settings.endGroup();
            settings.setValue("_lastAppliedVariant", currentVariant);
            qCDebug(QGCCorePluginLog) << "Vehicle variant changed to" << currentVariant << "- reset dependent settings to new defaults";
        }
    }

    if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::indoorPaletteName) {
            // Default to Indoor (1) color scheme
            metaData.setRawDefaultValue(1);
            return true;
        } else if (metaData.name() == AppSettings::offlineEditingFirmwareClassName) {
            // Default to ArduPilot (3) instead of PX4 (12)
            metaData.setRawDefaultValue(3);
            return true;
        }
#ifndef Q_OS_ANDROID
        else if (metaData.name() == AppSettings::androidSaveToSDCardName) {
            return false;
        }
#endif
    }

    // Vehicle variant-aware defaults (read raw setting to avoid circular initialization)
    // AppSettings group is "" (empty), so key is just "vehicleVariant"
    const bool isXplorer = QSettings().value("vehicleVariant", 1).toUInt() == 1;

    if (settingsGroup == FlyViewSettings::settingsGroup) {
        // Rangefinders: default on for Xplorer, off for X55
        if (metaData.name() == FlyViewSettings::showForwardRangefinderName ||
            metaData.name() == FlyViewSettings::showDownRangefinderName) {
            metaData.setRawDefaultValue(isXplorer);
            return true;
        }
        // Camera control: default on for Xplorer, off for X55
        if (metaData.name() == FlyViewSettings::showSimpleCameraControlName) {
            metaData.setRawDefaultValue(isXplorer);
            return true;
        }
        // Payload indicator: default on for Xplorer, off for X55
        if (metaData.name() == FlyViewSettings::showPayloadIndicatorName) {
            metaData.setRawDefaultValue(isXplorer);
            return true;
        }
        // MicROM: hide setting entirely on Xplorer
        if (metaData.name() == FlyViewSettings::enableMicROMName) {
            if (isXplorer) {
                return false;  // false = setting not visible
            }
            return true;
        }
    } else if (settingsGroup == BatteryIndicatorSettings::settingsGroup) {
        // Battery display: Xplorer=Percentage(0), X55=Voltage(1)
        if (metaData.name() == BatteryIndicatorSettings::valueDisplayName) {
            metaData.setRawDefaultValue(isXplorer ? 0 : 1);
            return true;
        }
    } else if (settingsGroup == UnitsSettings::settingsGroup) {
        // Default to metric units regardless of system locale
        if (metaData.name() == UnitsSettings::horizontalDistanceUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::HorizontalDistanceUnitsMeters);
            return true;
        } else if (metaData.name() == UnitsSettings::verticalDistanceUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::VerticalDistanceUnitsMeters);
            return true;
        } else if (metaData.name() == UnitsSettings::speedUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::SpeedUnitsMetersPerSecond);
            return true;
        } else if (metaData.name() == UnitsSettings::areaUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::AreaUnitsSquareMeters);
            return true;
        } else if (metaData.name() == UnitsSettings::temperatureUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::TemperatureUnitsCelsius);
            return true;
        } else if (metaData.name() == UnitsSettings::weightUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::WeightUnitsKg);
            return true;
        }
    }

    return true;
}

QString QGCCorePlugin::showAdvancedUIMessage() const
{
    return tr("WARNING: You are about to enter Advanced Mode. "
              "If used incorrectly, this may cause your vehicle to malfunction thus voiding your warranty. "
              "You should do so only if instructed by customer support. "
              "Are you sure you want to enable Advanced Mode?");
}

void QGCCorePlugin::factValueGridCreateDefaultSettings(FactValueGrid* factValueGrid)
{
    if (factValueGrid->specificVehicleForCard()) {
        bool includeFWValues = factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassFixedWing || factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassVTOL || factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassAirship;

        factValueGrid->setFontSize(FactValueGrid::LargeFontSize);
        factValueGrid->appendColumn();
        factValueGrid->appendColumn();

        int rowIndex = 0;
        int colIndex = 0;

        // first cell
        QmlObjectListModel* column = factValueGrid->columns()->value<QmlObjectListModel*>(colIndex++);
        InstrumentValueData* value = column->value<InstrumentValueData*>(rowIndex);
        value->setFact("Vehicle", "AltitudeRelative");
        value->setText("Alt (rel)");
        value->setShowUnits(true);

        // second cell
        column = factValueGrid->columns()->value<QmlObjectListModel*>(colIndex++);
        value = column->value<InstrumentValueData*>(rowIndex);
        if (includeFWValues) {
            value->setFact("Vehicle", "AirSpeed");
            value->setText("AirSpd");
            value->setShowUnits(true);
        } else {
            value->setFact("Vehicle", "GroundSpeed");
            value->setIcon("arrow-simple-right.svg");
            value->setText(value->fact()->shortDescription());
            value->setShowUnits(true);
        }
    } else {
        const bool includeFWValues = ((factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassFixedWing) || (factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassVTOL) || (factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassAirship));

        factValueGrid->setFontSize(FactValueGrid::LargeFontSize);

        (void) factValueGrid->appendColumn();
        (void) factValueGrid->appendColumn();
        if (includeFWValues) {
            (void) factValueGrid->appendColumn();
        }
        factValueGrid->appendRow();

        int rowIndex = 0;
        QmlObjectListModel *column = factValueGrid->columns()->value<QmlObjectListModel*>(0);

        InstrumentValueData *value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact(QStringLiteral("Vehicle"), QStringLiteral("AltitudeRelative"));
        value->setText("Alt (rel)");
        value->setShowUnits(true);

        value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact(QStringLiteral("Vehicle"), QStringLiteral("DistanceToHome"));
        value->setText("Home Dist");
        value->setShowUnits(true);

        rowIndex = 0;
        column = factValueGrid->columns()->value<QmlObjectListModel*>(1);

        value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact(QStringLiteral("Vehicle"), QStringLiteral("FlightTime"));
        value->setText("Flight Time");
        value->setShowUnits(true);

        value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact(QStringLiteral("Vehicle"), QStringLiteral("GroundSpeed"));
        value->setText("Speed");
        value->setShowUnits(true);

        // Voltage/Current column intentionally omitted — with dual smart batteries
        // a single Battery0 voltage/current column doesn't represent the system.
        // The per-battery indicators in the toolbar surface this info instead.
    }
}

QQmlApplicationEngine *QGCCorePlugin::createQmlApplicationEngine(QObject *parent)
{
    QQmlApplicationEngine *const qmlEngine = new QQmlApplicationEngine(parent);
    qmlEngine->addImportPath(QStringLiteral("qrc:/qml"));
    qmlEngine->rootContext()->setContextProperty(QStringLiteral("joystickManager"), JoystickManager::instance());
    qmlEngine->rootContext()->setContextProperty(QStringLiteral("debugMessageModel"), QGCLogging::instance());
    qmlEngine->rootContext()->setContextProperty(QStringLiteral("logDownloadController"), LogDownloadController::instance());
    return qmlEngine;
}

void QGCCorePlugin::createRootWindow(QQmlApplicationEngine *qmlEngine)
{
    qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/QGroundControl/MainWindow/MainWindow.qml")));
}

VideoReceiver *QGCCorePlugin::createVideoReceiver(QObject *parent)
{
#ifdef QGC_GST_STREAMING
    return GStreamer::createVideoReceiver(parent);
#elif defined(QGC_QT_STREAMING)
    return QtMultimediaReceiver::createVideoReceiver(parent);
#else
    return nullptr;
#endif
}

void *QGCCorePlugin::createVideoSink(QQuickItem *widget, QObject *parent)
{
#ifdef QGC_GST_STREAMING
    return GStreamer::createVideoSink(widget, parent);
#elif defined(QGC_QT_STREAMING)
    return QtMultimediaReceiver::createVideoSink(widget, parent);
#else
    Q_UNUSED(widget); Q_UNUSED(parent);
    return nullptr;
#endif
}
void QGCCorePlugin::releaseVideoSink(void *sink)
{
#ifdef QGC_GST_STREAMING
    GStreamer::releaseVideoSink(sink);
#elif defined(QGC_QT_STREAMING)
    QtMultimediaReceiver::releaseVideoSink(sink);
#else
    Q_UNUSED(sink);
#endif
}

const QVariantList &QGCCorePlugin::toolBarIndicators()
{
    static const QVariantList toolBarIndicatorList = QVariantList(
        {
            QVariant::fromValue(QUrl::fromUserInput(QStringLiteral("qrc:/qml/QGroundControl/Toolbar/RTKGPSIndicator.qml"))),
        }
    );

    return toolBarIndicatorList;
}

QVariantList QGCCorePlugin::firstRunPromptsToShow()
{
    QList<int> rgIdsToShow;

    rgIdsToShow.append(firstRunPromptStdIds());
    rgIdsToShow.append(firstRunPromptCustomIds());

    const QList<int> rgAlreadyShownIds = AppSettings::firstRunPromptsIdsVariantToList(SettingsManager::instance()->appSettings()->firstRunPromptIdsShown()->rawValue());
    for (int idToRemove: rgAlreadyShownIds) {
        (void) rgIdsToShow.removeOne(idToRemove);
    }

    QVariantList rgVarIdsToShow;
    for (int id: rgIdsToShow) {
        rgVarIdsToShow.append(id);
    }

    return rgVarIdsToShow;
}

QString QGCCorePlugin::firstRunPromptResource(int id) const
{
    switch (id) {
    case kUnitsFirstRunPromptId:
        return QStringLiteral("/FirstRunPromptDialogs/UnitsFirstRunPrompt.qml");
    case kOfflineVehicleFirstRunPromptId:
        return QStringLiteral("/FirstRunPromptDialogs/OfflineVehicleFirstRunPrompt.qml");
    default:
        return QString();
    }
}

void QGCCorePlugin::_setShowTouchAreas(bool show)
{
    if (show != _showTouchAreas) {
        _showTouchAreas = show;
        emit showTouchAreasChanged(show);
    }
}

void QGCCorePlugin::_setShowAdvancedUI(bool show)
{
    if (show != _showAdvancedUI) {
        _showAdvancedUI = show;
        emit showAdvancedUIChanged(show);
    }
}
