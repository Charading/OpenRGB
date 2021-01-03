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
    STEELSERIES_QCKPRISM_STATIC = 0x00,
    STEELSERIES_QCKPRISM_RAINBOW = 0x01,
    STEELSERIES_QCKPRISM_BREATH = 0x02
};

enum
{
    QCKPRISM_CMD_EFFECTS = 0x0b,
    QCKPRISM_CMD_BRIGHTNESS = 0x0c,
    QCKPRISM_CMD_APPLY = 0x0d,
    QCKPRISM_CMD_COLOR = 0x0e,
    QCKPRISM_CMD_UNKNOWN = 0x10
};

typedef struct __attribute__((__packed__)) qckprism_color_pkt {
    unsigned short cmd;
    unsigned short count;
} qckprism_color_pkt;

typedef struct __attribute__((__packed__)) qckprism_brightness_pkt {
    unsigned short cmd;
    unsigned char brightness;
} qckprism_brightness_pkt;

typedef struct __attribute__((__packed__)) qckprism_color_body {
    /* Color */
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char x;
    unsigned short unknown1;
    unsigned short unknown2;
    unsigned char unknown3;
    unsigned char cycle;
    unsigned char unknown4;
    unsigned char zone;
} qckprism_color_body;

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

    void SetColorAll
            (
            unsigned char   red,
            unsigned char   green,
            unsigned char   blue
            );
    void SetColor
            (
            int zone_id,
            unsigned char   red,
            unsigned char   green,
            unsigned char   blue
            );

private:
    char                    device_name[32];
    hid_device*             dev;
    std::string             location;
    steelseries_type        proto;
    int                     zone_count;
};
