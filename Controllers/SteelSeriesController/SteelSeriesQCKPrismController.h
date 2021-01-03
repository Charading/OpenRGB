/*-----------------------------------------*\
|  SteelSeriesQCKPrismController.h          |
|                                           |
|  Definitions and types for SteelSeries    |
|  QCK Prism lighting controller            |
|                                           |
|  B Horn (bahorn) 30/12/2020               |
\*-----------------------------------------*/

#include <string>
#include <hidapi/hidapi.h>

#include "SteelSeriesGeneric.h"

#pragma once

enum
{
    STEELSERIES_QCKPRISM_STATIC = 0x00
};


class SteelSeriesQCKPrismController
{
public:
    SteelSeriesQCKPrismController
        (
        hid_device*         dev_handle,
        const char*         path
        );

    ~SteelSeriesQCKPrismController();

    std::string GetDeviceLocation();
    char*       GetDeviceName();
    std::string GetSerialString();

    void Save();

    void Apply();

    void Brightness(float value);

    void SetLightEffect
            (
            unsigned char   zone_id,
            unsigned char   effect
            );

    void SetLightEffectAll
            (
            unsigned char   effect
            );

    void SetColor
            (
            unsigned char   red1,
            unsigned char   green1,
            unsigned char   blue1,
            unsigned char   red2,
            unsigned char   green2,
            unsigned char   blue2
            );

private:
    char                    device_name[32];
    hid_device*             dev;
    std::string             location;
    steelseries_type        proto;
};
