/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AirDataSettings.h"

#include <QtQml/qqml.h>

DECLARE_SETTINGGROUP(AirData, "AirData")
{
    qmlRegisterUncreatableType<AirDataSettings>("QGroundControl.SettingsManager", 1, 0, "AirDataSettings", "Reference only");
}

DECLARE_SETTINGSFACT(AirDataSettings, airDataSyncEnabled)
DECLARE_SETTINGSFACT(AirDataSettings, airDataUploadToken)
DECLARE_SETTINGSFACT(AirDataSettings, airDataAppKey)
