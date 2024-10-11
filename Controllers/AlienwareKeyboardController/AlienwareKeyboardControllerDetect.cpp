/*---------------------------------------------------------*\
| AlienwareKeyboardControllerDetect.cpp                     |
|                                                           |
|   Detector for Alienware Keyboard                         |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AlienwareAW510KController.h"
#include "AlienwareAW410KController.h"
#include "RGBController.h"
#include "RGBController_AlienwareAW510K.h"
#include "RGBController_AlienwareAW410K.h"

/*-----------------------------------------------------*\
| Alienware vendor ID                                   |
\*-----------------------------------------------------*/
#define ALIENWARE_VID                           0x04F2

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define ALIENWARE_AW510K_PID                    0x1830
#define ALIENWARE_AW410K_PID                    0x1968

/******************************************************************************************\
*                                                                                          *
*   DetectAlienwareKeyboardControllers                                                     *
*                                                                                          *
*       Tests the USB address to see if a Alienware RGB Keyboard controller exists there.  *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAlienwareAW510KControllers, AlienwareAW510KController, RGBController_AlienwareAW510K)
GENERIC_HOTPLUGGABLE_DETECTOR(DetectAlienwareAW410KControllers, AlienwareAW410KController, RGBController_AlienwareAW410K)

REGISTER_HID_DETECTOR_IPU("Alienware AW510K",   DetectAlienwareAW510KControllers,   ALIENWARE_VID,  ALIENWARE_AW510K_PID,   0x02,   0xFF00, 0x01);
REGISTER_HID_DETECTOR_IPU("Alienware AW410K",   DetectAlienwareAW410KControllers,   ALIENWARE_VID,  ALIENWARE_AW410K_PID,   0x02,   0xFF00, 0x01);
