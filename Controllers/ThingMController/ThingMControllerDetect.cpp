/*---------------------------------------------------------*\
| ThingMControllerDetect.cpp                                |
|                                                           |
|   Detector for ThingM Blink                               |
|                                                           |
|   Eric S (edbgon)                             01 Oct 2021 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "BlinkController.h"
#include "RGBController_BlinkController.h"

#define THINGM_VID                              0x27B8

#define THINGM_BLINK_PID                        0x01ED

/******************************************************************************************\
*                                                                                          *
*   DetectThingMControllers                                                          *
*                                                                                          *
*       Tests the USB address to see if any CoolerMaster controllers exists there.         *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectThingMBlink, BlinkController, RGBController_BlinkController)

REGISTER_HID_DETECTOR_PU("ThingM blink(1) mk2",          DetectThingMBlink,          THINGM_VID,   THINGM_BLINK_PID,    0xFF00,     0x01);
