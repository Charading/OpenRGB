/*---------------------------------------------------------*\
| MSI3ZoneControllerDetect.cpp                              |
|                                                           |
|   Detector for MSI/SteelSeries 3-Zone keyboard            |
|                                                           |
|   Adam Honse (CalcProgrammer1)                25 Dec 2019 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "MSI3ZoneController.h"
#include "RGBController_MSI3Zone.h"

#define MSI_3_ZONE_KEYBOARD_VID 0x1770
#define MSI_3_ZONE_KEYBOARD_PID 0xFF00

GENERIC_HOTPLUGGABLE_DETECTOR(DetectMSI3ZoneControllers, MSI3ZoneController, RGBController_MSI3Zone)

REGISTER_HID_DETECTOR("MSI 3-Zone Laptop", DetectMSI3ZoneControllers, MSI_3_ZONE_KEYBOARD_VID, MSI_3_ZONE_KEYBOARD_PID);
