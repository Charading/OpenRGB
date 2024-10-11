/*---------------------------------------------------------*\
| CorsairCommanderCoreControllerDetect.cpp                  |
|                                                           |
|   Detector for Corsair Commander Core                     |
|                                                           |
|   Jeff P.                                                 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "CorsairCommanderCoreController.h"
#include "RGBController_CorsairCommanderCore.h"

/*-----------------------------------------------------*\
| Corsair vendor ID                                     |
\*-----------------------------------------------------*/
#define CORSAIR_VID                         0x1B1C

/*-----------------------------------------------------*\
| Commander Core product IDs                            |
\*-----------------------------------------------------*/
#define CORSAIR_COMMANDER_CORE_PID          0x0C1C
#define CORSAIR_COMMANDER_CORE2_PID         0x0C32

/******************************************************************************************\
*                                                                                          *
*   DetectCorsairCapellixControllers                                                       *
*                                                                                          *
*       Tests the USB address to see if a Corsair RGB Cooler controller exists there.      *
*                                                                                          *
\******************************************************************************************/

static Controllers DetectCorsairCapellixHIDControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        CorsairCommanderCoreController*     controller     = new CorsairCommanderCoreController(dev, info->path, info->product_id);
        RGBController_CorsairCommanderCore* rgb_controller = new RGBController_CorsairCommanderCore(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("Corsair Commander Core", DetectCorsairCapellixHIDControllers, CORSAIR_VID, CORSAIR_COMMANDER_CORE_PID, 0x00, 0xFF42, 0x01);
REGISTER_HID_DETECTOR_IPU("Corsair Commander Core", DetectCorsairCapellixHIDControllers, CORSAIR_VID, CORSAIR_COMMANDER_CORE2_PID, 0x00, 0xFF42, 0x01);
