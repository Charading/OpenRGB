/*---------------------------------------------------------*\
| AsusAuraMainboardController.h                             |
|                                                           |
|   Driver for ASUS Aura mainboard                          |
|                                                           |
|   Martin Hartl (inlart)                       25 Apr 2020 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi/hidapi.h>
#include "RGBController.h"
#include "AsusAuraUSBController.h"

#define AURA_MAINBOARD_DIRECT_CHANNEL               4

enum
{
    AURA_MAINBOARD_CONTROL_MODE_EFFECT            = 0x35,     /* Effect control mode                  */
    AURA_MAINBOARD_CONTROL_MODE_EFFECT_COLOR      = 0x36,     /* Effect color control mode            */
    AURA_MAINBOARD_CONTROL_MODE_COMMIT            = 0x3F,     /* Commit mode                          */
};

enum
{
    AURA_ADDRESSABLE_HEADER_PROTOCOL_GEN1         = 0x00,     /* Addressable RGB Gen1 mode */
    AURA_ADDRESSABLE_HEADER_PROTOCOL_GEN2         = 0x02,     /* Addressable RGB Gen2 mode */
};

#pragma pack(push, 1)
struct AuraMainboardAddressableHeaderConfig
{
    unsigned char has_header;
    unsigned char protocol;
    unsigned char unknown2;
    unsigned char unknown3;
    unsigned char unknown4;
    unsigned char unknown5;
};
struct AuraMainboardConfigTable
{
    unsigned char unknown0;
    unsigned char unknown1;
    unsigned char num_addressable_headers;
    AuraMainboardAddressableHeaderConfig headers[4];
    unsigned char num_total_mainboard_leds;
    unsigned char unknown28;
    unsigned char num_rgb_headers;
    unsigned char unknown30;
    unsigned char unknown31;
    unsigned char header_subchannel_count[4];
    unsigned char unknown36[24];
};
#pragma pack(pop)

class AuraMainboardController : public AuraUSBController
{
public:
    AuraMainboardController(hid_device* dev_handle, const char* path);
    ~AuraMainboardController();

    void SetChannelLEDs
        (
        unsigned char   channel,
        RGBColor *      colors,
        unsigned int    num_colors
        );

    void SetMode
        (
        unsigned char   channel,
        unsigned char   mode,
        unsigned char   red,
        unsigned char   grn,
        unsigned char   blu
        );

    void SetMode
        (
        unsigned char   channel,
        unsigned char   mode,
        unsigned char   red,
        unsigned char   grn,
        unsigned char   blu,
        bool            shutdown_effect
        );

    void SendCommit();

private:
    unsigned int mode;

    unsigned short GetMask(int start, int size);

    void SendEffect
        (
        unsigned char   channel,
        unsigned char   mode,
        bool            shutdown_effect
        );

    void SendColor
        (
        unsigned char   channel,
        unsigned char   start_led,
        unsigned char   led_count,
        unsigned char*  led_data,
        bool            shutdown_effect
        );
};
