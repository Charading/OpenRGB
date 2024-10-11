/*---------------------------------------------------------*\
| KeychronKeyboardControllerDetect.cpp                      |
|                                                           |
|   Detector for Keychron keyboard                          |
|                                                           |
|   Morgan Guimard (morg)                       20 Feb 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "KeychronKeyboardController.h"
#include "RGBController_KeychronKeyboard.h"

/*---------------------------------------------------------*\
| KeychronKeyboard vendor ID                                |
\*---------------------------------------------------------*/
#define KEYCHRON_KEYBOARD_VID                          0x05AC

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define KEYCHRON_K3_V2_OPTICAL_RGB_PID                 0x024F

static Controllers DetectKeychronKeyboardControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        KeychronKeyboardController*     controller      = new KeychronKeyboardController(dev, *info);
        RGBController_KeychronKeyboard* rgb_controller  = new RGBController_KeychronKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("Keychron Gaming Keyboard 1", DetectKeychronKeyboardControllers, KEYCHRON_KEYBOARD_VID, KEYCHRON_K3_V2_OPTICAL_RGB_PID, 0, 0x0001, 0x06);
