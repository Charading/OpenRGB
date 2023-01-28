/*-----------------------------------------*\
|  MSICoreliquidController.h                |
|                                           |
|  Definitions and types for MSI            |
|  Coreliquid lighting controller           |
|                                           |
|  Aapo Kossi  25/1/2023                    |
\*-----------------------------------------*/
#pragma once

#define CORELIQUID_REPORT_SIZE 0xB9
#define CORELIQUID_REPORT_ID   0x52

#include "RGBController.h"
#include <string>
#include <hidapi/hidapi.h>

class MSICoreliquidController
{
public:
    MSICoreliquidController(hid_device* dev_handle, const hid_device_info& info);

    ~MSICoreliquidController();

    std::string         GetDeviceLocation();
    std::string         GetSerialString();
    std::string         GetFirmwareVersion();
    void                SendColors
        (
        RGBColor color1,
        RGBColor color2,
        unsigned char mode,
        unsigned char speed,
        unsigned char brightness,
        unsigned char color_enabled
        );

private:
    std::string         location;
    std::string         serial_number;
    std::string         version;

    hid_device*         dev;
};
