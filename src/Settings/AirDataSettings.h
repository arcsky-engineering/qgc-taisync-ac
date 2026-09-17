/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "SettingsGroup.h"

class AirDataSettings : public SettingsGroup
{
    Q_OBJECT
public:
    AirDataSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(airDataSyncEnabled)
    DEFINE_SETTINGFACT(airDataUploadToken)
    DEFINE_SETTINGFACT(airDataAppKey)
};
