/*---------------------------------------------------------*\
| GigabyteAorusMouseControllerDetect.cpp                    |
|                                                           |
|   Detector for Gigabyte Aorus mouse                       |
|                                                           |
|   Morgan Guimard (morg)                       29 Dec 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "GigabyteAorusMouseController.h"
#include "RGBController_GigabyteAorusMouse.h"

/*-----------------------------------------------------*\
| Vendor ID                                             |
\*-----------------------------------------------------*/
#define HOLTEK_VID                                 0x1044

/*-----------------------------------------------------*\
| Controller product ids                                |
\*-----------------------------------------------------*/
#define AORUS_M2_PID                               0x7A40

static Controllers DetectGigabyteAorusMouseControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        GigabyteAorusMouseController*     controller         = new GigabyteAorusMouseController(dev, *info);
        RGBController_GigabyteAorusMouse* rgb_controller     = new RGBController_GigabyteAorusMouse(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("Gigabyte Aorus M2", DetectGigabyteAorusMouseControllers, HOLTEK_VID, AORUS_M2_PID, 3, 0xFF01, 0x01);
