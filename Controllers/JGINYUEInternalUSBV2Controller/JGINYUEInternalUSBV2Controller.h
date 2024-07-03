/*---------------------------------------------------------*\
| JGINYUEInternalUSBV2Controller.h                            |
|                                                           |
|   Driver for JGINYUE USB motherboard                      |
|                                                           |
|   Tong R (tcr020)                             09 Aug 2023 |
|   Liu ShiMeng(Moon dream stars)               09 Aug 2023 |
|   Dongguan Yonghang Electronic Technology Co., Ltd        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi/hidapi.h>
#include <serial_port/serial_port.h>
#include "RGBController.h"

#define JGINYUE_MAX_ZONES               2
#define JGINYUE_ADDRESSABLE_MAX_LEDS    100

enum
{
    JGINYUE_USB_MODE_OFF                = 0x10,
    JGINYUE_USB_MODE_STATIC             = 0x11,
    JGINYUE_USB_MODE_BREATHING          = 0x12,
    JGINYUE_USB_MODE_STROBE             = 0x13,
    JGINYUE_USB_MODE_CYCLING            = 0x14,
    JGINYUE_USB_MODE_RANDOM             = 0x15,
    JGINYUE_USB_MODE_MUSIC              = 0x16, /* music mode,not support yet                                   */
    JGINYUE_USB_MODE_WAVE               = 0x17,
    JGINYUE_USB_MODE_SPRING             = 0x18, /* spring mode,not support yet                                  */
    JGINYUE_USB_MODE_WATER              = 0x19,
    JGINYUE_USB_MODE_RAINBOW            = 0x1A, /* rainbow mode,not support yet                                 */
    JGINYUE_USB_MODE_DIRECT             = 0x20, /* Not the exact USB protcol  - but need a way to differentiate */
};

enum
{
    JGINYUE_USB_SPEED_MAX               = 0xFF,
    JGINYUE_USB_SPEED_MIN               = 0x00,
    JGINYUE_USB_SPEED_DEFAULT           = 0x80
};

enum
{
    JGINYUE_DIRECTION_RIGHT             = 0x00,
    JGINYUE_DIRECTION_LEFT              = 0x01
};

enum
{
    JGINYUE_USB_BRIGHTNESS_MAX          = 0xFF,
    JGINYUE_USB_BRIGHTNESS_MIN          = 0x00,
    JGINYUE_USB_BRIGHTNESS_DEFAULT      = 0xFF
};

struct AreaConfigurationV2
{
    unsigned char LED_numbers;
    unsigned char RG_Swap;
    unsigned char Direction;
    unsigned char Direct_Mode_control;          /* 0x00 = Disabled, 0x01 = Enabled                              */
    unsigned char Mode_active;
    unsigned char Color_num;
    unsigned char Color_R;
    unsigned char Color_G;
    unsigned char Color_B;
    unsigned char Color_Array[30];
    unsigned char Brightness;
    unsigned char Speed;
};

class JGINYUEInternalUSBV2Controller
{
public:
    JGINYUEInternalUSBV2Controller(hid_device* jy_hid_device, const char* path,serial_port* jy_cdc_device);
    ~JGINYUEInternalUSBV2Controller();

    unsigned int        GetZoneCount();
    std::string         GetDeviceLocation();
    std::string         GetDeviceName();
    std::string         GetSerialString();
    std::string         GetDeviceFWVersion();

    void WriteZoneMode
        (
        unsigned char   zone,
        unsigned char   mode,
        unsigned char   num_colors,
        RGBColor        rgb[10],
        unsigned char   speed,
        unsigned char   brightness,
        unsigned char   direction
        );

    void DirectLEDControl
        (
        RGBColor*       colors,
        unsigned char   zone
        );

    void                SetRGSwap(unsigned char RGSwap);
    void                Init_device();
    void                Area_resize(unsigned char led_numbers,unsigned char zone);

private:
    unsigned char       ZoneInfo;
    unsigned char       SKUID;
    unsigned char       Major_version;
    unsigned char       Minor_version;

    AreaConfigurationV2 device_config[8];
    AreaConfigurationV2 device_config_Global;
    hid_device*         jy_hid_interface;
    serial_port*        jy_cdc_interface;
    std::string         location;
    std::string         device_name;
};
