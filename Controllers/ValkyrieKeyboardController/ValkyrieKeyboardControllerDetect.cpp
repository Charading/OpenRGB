/*---------------------------------------------------------*\
| ValkyrieKeyboardControllerDetect.cpp                      |
|                                                           |
|   Detector for Valkyrie keyboard                          |
|                                                           |
|   Nollie (Nuonuo)                             06 Dec 2023 |
|   Bartholomew Ho (imnotmental)                01 Feb 2024 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "RGBController_ValkyrieKeyboard.h"

static Controllers DetectValkyrieKeyboardControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        ValkyrieKeyboardController*       controller        = new ValkyrieKeyboardController(dev, info->path, info->product_id, info->interface_number);
        RGBController_ValkyrieKeyboard*   rgb_controller    = new RGBController_ValkyrieKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

/* DetectValkyrieKeyboardControllers() */
REGISTER_HID_DETECTOR_IPU("Valkyrie VK99 Pro",  DetectValkyrieKeyboardControllers,    VALKYRIE_VID,   VALKYRIE_99_PRO_PID,    3,  0xFF13, 0x0001);
REGISTER_HID_DETECTOR_IPU("Valkyrie VK99",      DetectValkyrieKeyboardControllers,    VALKYRIE_VID,   VALKYRIE_99_NORMAL_PID, 2,  0xFF13, 0x0001);
