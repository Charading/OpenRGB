﻿/*---------------------------------------------------------*\
| GigabyteAorusLaptopControllerDetect.cpp                   |
|                                                           |
|   Detector for Gigabyte Aorus laptop                      |
|                                                           |
|   Morgan Guimard (morg)                       05 Jun 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "GigabyteAorusLaptopController.h"
#include "RGBController_GigabyteAorusLaptop.h"

/*---------------------------------------------------------*\
| Gigabyte vendor ID                                        |
\*---------------------------------------------------------*/
#define GIGABYTE_AORUS_LAPTOP_VID                      0x0414

/*---------------------------------------------------------*\
| AORUS Laptops PID                                         |
\*---------------------------------------------------------*/
#define AORUS_17X_BACKLIGHT_PID                        0x7A42
#define AORUS_17X_KEYBOARD_PID                         0x7A3F

#define AORUS_15BKF_BACKLIGHT_PID                        0x7A44
#define AORUS_15BKF_KEYBOARD_PID                         0x7A43

static Controllers DetectGigabyteAorusLaptopControllers(hid_device_info* info, const std::string& /*name*/, GIGABYTE_AORUS_LAPTOP_DEV_TYPE dev_type)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        GigabyteAorusLaptopController*     controller       = new GigabyteAorusLaptopController(dev, *info);
        RGBController_GigabyteAorusLaptop* rgb_controller   = new RGBController_GigabyteAorusLaptop(controller, dev_type);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectGigabyteAorusLaptopKeyboardControllers(hid_device_info* info, const std::string& name)
{
    return DetectGigabyteAorusLaptopControllers(info, name, GIGABYTE_AORUS_LAPTOP_KEYBOARD_TYPE);
}

static Controllers DetectGigabyteAorusLaptopBacklightControllers(hid_device_info* info, const std::string& name)
{
    return DetectGigabyteAorusLaptopControllers(info, name, GIGABYTE_AORUS_LAPTOP_BACKLIGHT_TYPE);
}

REGISTER_HID_DETECTOR_IPU("Gigabyte Aorus 17X Keyboard",  DetectGigabyteAorusLaptopKeyboardControllers,  GIGABYTE_AORUS_LAPTOP_VID, AORUS_17X_KEYBOARD_PID, 3, 0xFF01, 0x01);
REGISTER_HID_DETECTOR_IPU("Gigabyte Aorus 17X Backlight", DetectGigabyteAorusLaptopBacklightControllers, GIGABYTE_AORUS_LAPTOP_VID, AORUS_17X_BACKLIGHT_PID, 3, 0xFF01, 0x01);

REGISTER_HID_DETECTOR_IPU("Gigabyte Aorus 15BKF Keyboard",  DetectGigabyteAorusLaptopKeyboardControllers,  GIGABYTE_AORUS_LAPTOP_VID, AORUS_15BKF_KEYBOARD_PID, 3, 0xFF01, 0x01);
REGISTER_HID_DETECTOR_IPU("Gigabyte Aorus 15BKF Backlight", DetectGigabyteAorusLaptopBacklightControllers, GIGABYTE_AORUS_LAPTOP_VID, AORUS_15BKF_BACKLIGHT_PID, 3, 0xFF01, 0x01);
