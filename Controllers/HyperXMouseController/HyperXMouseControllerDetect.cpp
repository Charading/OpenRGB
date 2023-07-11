/*---------------------------------------------------------*\
| HyperXMouseControllerDetect.cpp                           |
|                                                           |
|   Detector for HyperX mouse                               |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "HyperXPulsefireFPSProController.h"
#include "HyperXPulsefireSurgeController.h"
#include "HyperXPulsefireDartController.h"
#include "HyperXPulsefireRaidController.h"
#include "RGBController_HyperXPulsefireFPSPro.h"
#include "RGBController_HyperXPulsefireHaste.h"
#include "RGBController_HyperXPulsefireSurge.h"
#include "RGBController_HyperXPulsefireDart.h"
#include "RGBController_HyperXPulsefireRaid.h"

/*-----------------------------------------------------*\
| HyperX mouse vendor IDs                               |
\*-----------------------------------------------------*/
#define HYPERX_VID                              0x0951 //Kingston Technology
#define HYPERX_VID_2                            0x03F0 //HP, Hewlett-Packard Company
#define HYPERX_PULSEFIRE_SURGE_PID              0x16D3
#define HYPERX_PULSEFIRE_SURGE_PID_2            0x0490
#define HYPERX_PULSEFIRE_FPS_PRO_PID            0x16D7
#define HYPERX_PULSEFIRE_CORE_PID               0x16DE
#define HYPERX_PULSEFIRE_CORE_PID_2             0x0D8F
#define HYPERX_PULSEFIRE_DART_WIRELESS_PID      0x16E1
#define HYPERX_PULSEFIRE_DART_WIRELESS_PID_2    0x068E
#define HYPERX_PULSEFIRE_DART_WIRED_PID         0x16E2
#define HYPERX_PULSEFIRE_DART_WIRED_PID_2       0x088E
#define HYPERX_PULSEFIRE_RAID_PID               0x16E4
#define HYPERX_PULSEFIRE_HASTE_PID              0x1727
#define HYPERX_PULSEFIRE_HASTE_PID_2            0x0F8F

GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXPulsefireSurgeControllers, HyperXPulsefireSurgeController, RGBController_HyperXPulsefireSurge)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXPulsefireFPSProControllers, HyperXPulsefireFPSProController, RGBController_HyperXPulsefireFPSPro)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXPulsefireHasteControllers, HyperXPulsefireHasteController, RGBController_HyperXPulsefireHaste)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXPulsefireDartControllers, HyperXPulsefireDartController, RGBController_HyperXPulsefireDart)

static Controllers DetectHyperXPulsefireRaidControllers(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        HyperXPulsefireRaidController*     controller     = new HyperXPulsefireRaidController(dev, *info);
        RGBController_HyperXPulsefireRaid* rgb_controller = new RGBController_HyperXPulsefireRaid(controller);
        result.push_back(rgb_controller);
    }
    return result;
}   /* DetectHyperXPulsefireRaidControllers() */

REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Surge",              DetectHyperXPulsefireSurgeControllers,  HYPERX_VID,     HYPERX_PULSEFIRE_SURGE_PID,             1,      0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Surge (HP)",         DetectHyperXPulsefireSurgeControllers,  HYPERX_VID_2,   HYPERX_PULSEFIRE_SURGE_PID_2,           1,      0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire FPS Pro",            DetectHyperXPulsefireFPSProControllers, HYPERX_VID,     HYPERX_PULSEFIRE_FPS_PRO_PID,           1,      0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Core",               DetectHyperXPulsefireFPSProControllers, HYPERX_VID,     HYPERX_PULSEFIRE_CORE_PID,              1,      0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Core (HP)",          DetectHyperXPulsefireFPSProControllers, HYPERX_VID_2,   HYPERX_PULSEFIRE_CORE_PID_2,            1,      0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Dart (Wireless)",    DetectHyperXPulsefireDartControllers,   HYPERX_VID,     HYPERX_PULSEFIRE_DART_WIRELESS_PID,     2,      0xFF00);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Dart (Wireless)",    DetectHyperXPulsefireDartControllers,   HYPERX_VID_2,   HYPERX_PULSEFIRE_DART_WIRELESS_PID_2,   2,      0xFF00);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Dart (Wired)",       DetectHyperXPulsefireDartControllers,   HYPERX_VID,     HYPERX_PULSEFIRE_DART_WIRED_PID,        1,      0xFF13);
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Dart (Wired)",       DetectHyperXPulsefireDartControllers,   HYPERX_VID_2,   HYPERX_PULSEFIRE_DART_WIRED_PID_2,      1,      0xFF13);

REGISTER_HID_DETECTOR_IPU("HyperX Pulsefire Raid",              DetectHyperXPulsefireRaidControllers,   HYPERX_VID,     HYPERX_PULSEFIRE_RAID_PID,              1,      0xFF01,     0x01);

REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Haste",              DetectHyperXPulsefireHasteControllers,  HYPERX_VID_2,   HYPERX_PULSEFIRE_HASTE_PID_2,           3,      0xFF90);

#ifdef _WIN32
REGISTER_HID_DETECTOR_IP("HyperX Pulsefire Haste",              DetectHyperXPulsefireHasteControllers,  HYPERX_VID,     HYPERX_PULSEFIRE_HASTE_PID,             3,      0xFF90);
#else
REGISTER_HID_DETECTOR_IPU("HyperX Pulsefire Haste",             DetectHyperXPulsefireHasteControllers,  HYPERX_VID_2,   HYPERX_PULSEFIRE_HASTE_PID_2,           0,      0x0001,     0x01);
REGISTER_HID_DETECTOR_PU("HyperX Pulsefire Haste",              DetectHyperXPulsefireHasteControllers,  HYPERX_VID,     HYPERX_PULSEFIRE_HASTE_PID,             1,      2);
#endif
