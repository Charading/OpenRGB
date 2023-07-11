/*---------------------------------------------------------*\
| EKControllerDetect.cpp                                    |
|                                                           |
|   Detector for EK Loop Connect                            |
|                                                           |
|   Chris M (Dr_No)                             16 Jul 2020 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "EKController.h"
#include "RGBController_EKController.h"

#define EK_VID                0x0483
#define EK_LOOP_CONNECT       0x5750

/******************************************************************************************\
*                                                                                          *
*   DetectEKControllers                                                                    *
*                                                                                          *
*       Tests the USB address to see if any EK Controllers exists there.                   *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectEKControllers, EKController, RGBController_EKController)

REGISTER_HID_DETECTOR_IPU("EK Loop Connect", DetectEKControllers, EK_VID, EK_LOOP_CONNECT, 0, 0xFFA0, 1);
