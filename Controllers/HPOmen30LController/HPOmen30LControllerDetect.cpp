/*---------------------------------------------------------*\
| HPOmen30LControllerDetect.cpp                             |
|                                                           |
|   Detector for HP Omen 30L                                |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "HPOmen30LController.h"
#include "RGBController_HPOmen30L.h"

#define HP_OMEN_30L_VID 0x103C
#define HP_OMEN_30L_PID 0x84FD

/******************************************************************************************\
*                                                                                          *
*   DetectHPOmen30LController                                                              *
*                                                                                          *
*       Tests the USB address to see if an HP Omen 30L controller exists there.            *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectHPOmen30LController, HPOmen30LController, RGBController_HPOmen30L)

REGISTER_HID_DETECTOR("HP Omen 30L", DetectHPOmen30LController, HP_OMEN_30L_VID, HP_OMEN_30L_PID);
