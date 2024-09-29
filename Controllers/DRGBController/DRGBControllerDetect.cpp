/*---------------------------------------------------------*\
| DRGBControllerDetect.cpp                                  |
|                                                           |
|   Driver for DRGBmods                                     |
|                                                           |
|   Zhi Yan                                     25 Jun 2024 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <vector>
#include <hidapi.h>
#include "Detector.h"
#include "DRGBController.h"
#include "RGBController.h"
#include "RGBController_DRGB.h"

void DetectDRGBControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        wchar_t product[128];
        hid_get_product_string(dev, product, 128);
        std::wstring product_str(product);
        DRGBController*     controller              = new DRGBController(dev, info->path,info->product_id);
        RGBController_DRGB* rgb_controller          = new RGBController_DRGB(controller);
        rgb_controller->name                        = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR("DRGB LED V4",    DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV4_VID, DRGB_LED_V4_PID);
REGISTER_HID_DETECTOR("DRGB ULTRA V4F", DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV4_VID, DRGB_ULTRA_V4F_PID);
REGISTER_HID_DETECTOR("DRGB CORE V4F",  DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV4_VID, DRGB_CORE_V4F_PID);
REGISTER_HID_DETECTOR("DRGB SIG V4F",   DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV4_VID, DRGB_SIG_V4F_PID);

REGISTER_HID_DETECTOR("DRGB LED",       DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV3_VID, DRGB_LED_V3_PID);
REGISTER_HID_DETECTOR("DRGB Ultra V3",  DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV3_VID, DRGB_Ultra_V3_PID);
REGISTER_HID_DETECTOR("DRGB CORE V3",   DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV3_VID, DRGB_CORE_V3_PID);

REGISTER_HID_DETECTOR("DRGB LED Controller",     DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV2_VID, DRGB_LED_PID);
REGISTER_HID_DETECTOR("DRGB ULTRA",              DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV2_VID, DRGB_ULTRA_PID);
REGISTER_HID_DETECTOR("DRGB SIG AB",             DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV2_VID, DRGB_SIG_AB_PID);
REGISTER_HID_DETECTOR("DRGB SIG CD",             DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV2_VID, DRGB_SIG_CD_PID);
REGISTER_HID_DETECTOR("DRGB Strimer Controller", DEVICE_TYPE_LEDSTRIP, DetectDRGBControllers, DRGBV2_VID, DRGB_Strimer_PID);
