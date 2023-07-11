/*---------------------------------------------------------*\
| OKSKeyboardControllerDetect.cpp                           |
|                                                           |
|   Detector for OKS keyboard                               |
|                                                           |
|   Merafour (OKS)                              24 Feb 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "OKSKeyboardController.h"
#include "RGBController_OKSKeyboard.h"

/******************************************************************************************\
*  DetectOKSKeyboardControllers                                                            *
*  Reference: DuckyKeyboardController                                                      *
*  Tests the USB address to see if a OKS Optical Axis RGB Keyboard controller exists there.*
*  Reference:DetectDuckyKeyboardControllers                                                *
\******************************************************************************************/

static Controllers DetectOKSKeyboardControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        OKSKeyboardController*     controller     = new OKSKeyboardController(dev, info->path, info->product_id);
        RGBController_OKSKeyboard* rgb_controller = new RGBController_OKSKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}   /* DetectOKSKeyboardControllers() */

REGISTER_HID_DETECTOR_I("OKS Optical Axis RGB",           DetectOKSKeyboardControllers, OKS_VID, OKS_OPTICAL_RGB_PID,   1);
