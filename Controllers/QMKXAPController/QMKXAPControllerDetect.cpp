/*-------------------------------------------------------------------*\
|  QMKXAPControllerDetect.cpp                                         |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi/hidapi.h>

#include "RGBController_QMKXAP.h"
#include "QMKXAPController.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "Detector.h"

#define XAP_USAGE_PAGE 0xFF51
#define XAP_USAGE 0x0058

void DetectQMKXAPControllers(hid_device_info* info, const std::string& name) {
    hid_device* dev = hid_open_path(info->path);

    if (dev)
    {
        QMKXAPController* controller = new QMKXAPController(dev);

        if (controller->CheckSubsystems()) {
            RGBController_QMKXAP* rgb_controller = new RGBController_QMKXAP(controller);
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        } else {
            LOG_WARNING("[QMK XAP] Keyboard missing required subsystems");
        }
    }
}

#ifdef USE_HID_USAGE
REGISTER_HID_DETECTOR_PU_ONLY("QMK XAP Keyboard", DetectQMKXAPControllers, XAP_USAGE_PAGE, XAP_USAGE);
#endif