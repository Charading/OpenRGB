/*---------------------------------------------------------*\
| HoltekControllerDetect.cpp                                |
|                                                           |
|   Detector for Holtek devices                             |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "HoltekA070Controller.h"
#include "RGBController_HoltekA070.h"
#include "HoltekA1FAController.h"
#include "RGBController_HoltekA1FA.h"

/*-----------------------------------------------------*\
| Holtek Semiconductor Inc. vendor ID                   |
\*-----------------------------------------------------*/
#define HOLTEK_VID                    0x04D9
/*-----------------------------------------------------*\
| Mouse product IDs                                     |
\*-----------------------------------------------------*/
#define HOLTEK_A070_PID               0xA070
/*-----------------------------------------------------*\
| Mousemats product IDs                                 |
\*-----------------------------------------------------*/
#define HOLTEK_A1FA_PID               0xA1FA

GENERIC_HOTPLUGGABLE_DETECTOR(DetectHoltekControllers, HoltekA070Controller, RGBController_HoltekA070)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectHoltekMousemats, HoltekA1FAController, RGBController_HoltekA1FA)

REGISTER_HID_DETECTOR_IPU("Holtek USB Gaming Mouse", DetectHoltekControllers, HOLTEK_VID, HOLTEK_A070_PID, 1, 0xFF00, 2);
REGISTER_HID_DETECTOR_IPU("Holtek Mousemat",         DetectHoltekMousemats,   HOLTEK_VID, HOLTEK_A1FA_PID, 2, 0xFF00, 0xFF00);
