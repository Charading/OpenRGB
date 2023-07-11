/*---------------------------------------------------------*\
| HyperXKeyboardControllerDetect.cpp                        |
|                                                           |
|   Driver for HyperX keyboards                             |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "HyperXAlloyEliteController.h"
#include "HyperXAlloyElite2Controller.h"
#include "HyperXAlloyFPSController.h"
#include "HyperXAlloyOriginsController.h"
#include "HyperXAlloyOriginsCoreController.h"
#include "HyperXAlloyOrigins60and65Controller.h"
#include "RGBController_HyperXAlloyElite.h"
#include "RGBController_HyperXAlloyElite2.h"
#include "RGBController_HyperXAlloyFPS.h"
#include "RGBController_HyperXAlloyOrigins.h"
#include "RGBController_HyperXAlloyOriginsCore.h"
#include "RGBController_HyperXAlloyOrigins60and65.h"

/*-----------------------------------------------------*\
| HyperX keyboard vendor and product IDs                |
\*-----------------------------------------------------*/
#define HYPERX_KEYBOARD_VID                        0x0951

#define HYPERX_ALLOY_ELITE_PID                     0x16BE
#define HYPERX_ALLOY_ELITE_2_PID                   0x1711
#define HYPERX_ALLOY_FPS_RGB_PID                   0x16DC
#define HYPERX_ALLOY_ORIGINS_PID                   0x16E5
#define HYPERX_ALLOY_ORIGINS_CORE_PID              0x16E6
#define HYPERX_ALLOY_ORIGINS_60_PID                0x1734

/*-----------------------------------------------------*\
| HyperX keyboard vendor and product IDs (HP)           |
\*-----------------------------------------------------*/
#define HP_KEYBOARD_VID                            0x03F0

#define HYPERX_ALLOY_ELITE_2_HP_PID                0x058F
#define HYPERX_ALLOY_ORIGINS_60_HP_PID             0x0C8E
#define HYPERX_ALLOY_ORIGINS_65_HP_PID             0x038F
#define HYPERX_ALLOY_ORIGINS_CORE_HP_PID           0x098F
#define HYPERX_ALLOY_ORIGINS_HP_PID                0x0591

AlloyOrigins60and65MappingLayoutType GetAlloyOrigins60and65MappingLayoutType(int pid)
{
    switch(pid)
    {
        case HYPERX_ALLOY_ORIGINS_60_PID:
        case HYPERX_ALLOY_ORIGINS_60_HP_PID:
            return ALLOY_ORIGINS_60_LAYOUT;

        case HYPERX_ALLOY_ORIGINS_65_HP_PID:
            return ALLOY_ORIGINS_65_LAYOUT;

        default:
            return ALLOY_ORIGINS_60_LAYOUT;
    }
}

GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXAlloyElite, HyperXAlloyEliteController, RGBController_HyperXAlloyElite)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXAlloyElite2, HyperXAlloyElite2Controller, RGBController_HyperXAlloyElite2)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXAlloyFPS, HyperXAlloyFPSController, RGBController_HyperXAlloyFPS)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHyperXAlloyOrigins, HyperXAlloyOriginsController, RGBController_HyperXAlloyOrigins)

static Controllers DetectHyperXAlloyOriginsCore(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        HyperXAlloyOriginsCoreController*     controller     = new HyperXAlloyOriginsCoreController(dev, info);
        RGBController_HyperXAlloyOriginsCore* rgb_controller = new RGBController_HyperXAlloyOriginsCore(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectHyperXAlloyOrigins60and65(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        HyperXAlloyOrigins60and65Controller* controller         = new HyperXAlloyOrigins60and65Controller(dev, info->path);
        AlloyOrigins60and65MappingLayoutType layout             = GetAlloyOrigins60and65MappingLayoutType(info->product_id);
        RGBController_HyperXAlloyOrigins60and65* rgb_controller = new RGBController_HyperXAlloyOrigins60and65(controller, layout);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IP("HyperX Alloy Elite RGB",        DetectHyperXAlloyElite,          HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ELITE_PID,           2, 0xFF01);
REGISTER_HID_DETECTOR_IP("HyperX Alloy FPS RGB",          DetectHyperXAlloyFPS,            HYPERX_KEYBOARD_VID, HYPERX_ALLOY_FPS_RGB_PID,         2, 0xFF01);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins Core",      DetectHyperXAlloyOriginsCore,    HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_CORE_PID,    2);

REGISTER_HID_DETECTOR_I("HyperX Alloy Origins Core (HP)", DetectHyperXAlloyOriginsCore,    HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_CORE_HP_PID, 2);

#ifdef _WIN32
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins",           DetectHyperXAlloyOrigins,        HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_PID,         3);
REGISTER_HID_DETECTOR_IP("HyperX Alloy Elite 2",          DetectHyperXAlloyElite2,         HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ELITE_2_PID,         3, 0xFF90);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 60",        DetectHyperXAlloyOrigins60and65, HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_60_PID,      3);

REGISTER_HID_DETECTOR_IP("HyperX Alloy Elite 2 (HP)",     DetectHyperXAlloyElite2,         HP_KEYBOARD_VID,     HYPERX_ALLOY_ELITE_2_HP_PID,      3, 0xFF90);
REGISTER_HID_DETECTOR_IP("HyperX Alloy Origins (HP)",     DetectHyperXAlloyOrigins,        HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_HP_PID,      3, 0xFF90);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 60 (HP)",   DetectHyperXAlloyOrigins60and65, HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_60_HP_PID,   3);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 65 (HP)",   DetectHyperXAlloyOrigins60and65, HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_65_HP_PID,   3);
#else
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins",           DetectHyperXAlloyOrigins,        HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_PID,         0);
REGISTER_HID_DETECTOR_I("HyperX Alloy Elite 2",           DetectHyperXAlloyElite2,         HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ELITE_2_PID,         0);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 60",        DetectHyperXAlloyOrigins60and65, HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_60_PID,      0);

REGISTER_HID_DETECTOR_I("HyperX Alloy Elite 2 (HP)",      DetectHyperXAlloyElite2,         HP_KEYBOARD_VID,     HYPERX_ALLOY_ELITE_2_HP_PID,      0);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins (HP)",      DetectHyperXAlloyOrigins,        HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_HP_PID,      0);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 60 (HP)",   DetectHyperXAlloyOrigins60and65, HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_60_HP_PID,   0);
REGISTER_HID_DETECTOR_I("HyperX Alloy Origins 65 (HP)",   DetectHyperXAlloyOrigins60and65, HP_KEYBOARD_VID,     HYPERX_ALLOY_ORIGINS_65_HP_PID,   0);
#endif
