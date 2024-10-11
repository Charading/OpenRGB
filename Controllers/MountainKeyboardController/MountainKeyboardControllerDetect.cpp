/*---------------------------------------------------------*\
| MountainKeyboardControllerDetect.cpp                      |
|                                                           |
|   Detector for Mountain keyboard                          |
|                                                           |
|   Wojciech Lazarski                              Jan 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "MountainKeyboardController.h"
#include "RGBController_MountainKeyboard.h"

/*----------------------------------------------------------------------------------------*\
|                                                                                          |
|   DetectMountainKeyboardControllers                                                      |
|                                                                                          |
|       Tests the USB address to see if a Mountain RGB Keyboard controller exists there.   |
|                                                                                          |
\*----------------------------------------------------------------------------------------*/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectMountainKeyboardControllers, MountainKeyboardController, RGBController_MountainKeyboard)

REGISTER_HID_DETECTOR_IPU("Mountain Everest", DetectMountainKeyboardControllers, MOUNTAIN_VID, MOUNTAIN_EVEREST_PID, 3, 0xFF00, 0x01);
