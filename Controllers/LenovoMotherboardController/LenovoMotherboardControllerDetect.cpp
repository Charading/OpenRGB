/*---------------------------------------------------------*\
| LenovoMotherboardControllerDetect.cpp                     |
|                                                           |
|   Detector for Lenovo motherboard                         |
|                                                           |
|   Morgan Guimard (morg)                       26 Dec 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "LenovoMotherboardController.h"
#include "RGBController_LenovoMotherboard.h"
#include "dmiinfo.h"

/*---------------------------------------------------------*\
| vendor ID                                                 |
\*---------------------------------------------------------*/
#define LENOVO_MB_VID                                  0x17EF

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define LENOVO_MB_PID                                  0xC955

static Controllers DetectLenovoMotherboardControllers(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        DMIInfo dmi;
        LenovoMotherboardController*     controller         = new LenovoMotherboardController(dev, *info);
        RGBController_LenovoMotherboard* rgb_controller     = new RGBController_LenovoMotherboard(controller);
        rgb_controller->name                                = name + " " + dmi.getMainboard();
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_PU("Lenovo", DetectLenovoMotherboardControllers, LENOVO_MB_VID, LENOVO_MB_PID, 0xFF89, 0xCC);
