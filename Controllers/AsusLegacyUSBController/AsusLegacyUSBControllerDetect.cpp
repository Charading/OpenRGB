/*---------------------------------------------------------*\
| AsusLegacyUSBControllerDetect.cpp                         |
|                                                           |
|   Detector for ASUS legacy USB devices                    |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <stdexcept>
#include "HidDetector.h"
#include "AsusCerberusKeyboardController.h"
#include "AsusSagarisKeyboardController.h"
#include "AsusStrixClawController.h"
#include "RGBController_AsusCerberusKeyboard.h"
#include "RGBController_AsusSagarisKeyboard.h"
#include "RGBController_AsusStrixClaw.h"

#define ASUS_LEGACY_USB_VID         0x195D
#define ASUS_USB_VID                0x0B05

#define ASUS_CERBERUS_MECH_PID      0x2047
#define ASUS_SAGARIS_GK1100_PID     0x1835
#define ASUS_ROG_STRIX_CLAW_PID     0x1016

static Controllers DetectAsusCerberusMech(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        AsusCerberusKeyboardController*     controller     = new AsusCerberusKeyboardController(dev, info->path, info->release_number);
        RGBController_AsusCerberusKeyboard* rgb_controller = new RGBController_AsusCerberusKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectAsusSagarisKeyboard(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        AsusSagarisKeyboardController*     controller          = new AsusSagarisKeyboardController(dev, info->path, info->release_number);
        RGBController_AsusSagarisKeyboard* rgb_controller      = new RGBController_AsusSagarisKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAsusStrixClaw, StrixClawController, RGBController_StrixClaw)

REGISTER_HID_DETECTOR_IPU("ASUS Cerberus Mech",  DetectAsusCerberusMech,    ASUS_LEGACY_USB_VID, ASUS_CERBERUS_MECH_PID,  1, 0xFF01, 1);
REGISTER_HID_DETECTOR_IPU("ASUS Sagaris GK1100", DetectAsusSagarisKeyboard, ASUS_USB_VID,        ASUS_SAGARIS_GK1100_PID, 1, 0xFF02, 2);
REGISTER_HID_DETECTOR_IPU("ASUS ROG Strix Claw", DetectAsusStrixClaw,       ASUS_LEGACY_USB_VID, ASUS_ROG_STRIX_CLAW_PID, 0, 0xFF01, 1);
