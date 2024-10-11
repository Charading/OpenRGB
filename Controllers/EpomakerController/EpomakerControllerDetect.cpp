/*---------------------------------------------------------*\
| EpomakerControllerDetect.cpp                              |
|                                                           |
|   Detector for Epomaker keyboard                          |
|                                                           |
|   Alvaro Munoz (alvaromunoz)                  05 Jun 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "EpomakerController.h"
#include "RGBController_EpomakerController.h"

#define EPOMAKER_VID                    0x3151
#define EPOMAKER_TH80_Pro_USB_PID       0x4010
#define ATTACKSHARK_K86_USB_PID         0x4015
#define EPOMAKER_TH80_Pro_Dongle_PID    0x4011 /* Attack shark's Dongle is the same. */
#define EPOMAKER_TH80_Pro_BT_PID        0x4013
#define ATTACKSHARK_K86_BT_PID          0x4012

/******************************************************************************************\
*                                                                                          *
*   DetectEpomakerControllers                                                              *
*                                                                                          *
*       Tests the USB address to see if any Epomaker Controllers exists there.             *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectEpomakerControllers, EpomakerController, RGBController_EpomakerController)

REGISTER_HID_DETECTOR_I("Epomaker TH80 Pro (USB Cable)", DetectEpomakerControllers, EPOMAKER_VID, EPOMAKER_TH80_Pro_USB_PID, 2);
REGISTER_HID_DETECTOR_I("Epomaker TH80 Pro (USB Dongle)", DetectEpomakerControllers, EPOMAKER_VID, EPOMAKER_TH80_Pro_Dongle_PID, 2);
REGISTER_HID_DETECTOR_I("Attack Shark K86 (USB Cable)", DetectEpomakerControllers, EPOMAKER_VID, ATTACKSHARK_K86_USB_PID, 2);

/*---------------------------------------------------------*\
| Bluetooth Not implemented                                 |
\*---------------------------------------------------------*/
//REGISTER_HID_DETECTOR("Epomaker TH80 Pro (Bluetooth)", DetectEpomakerControllers, EPOMAKER_VID, EPOMAKER_TH80_Pro_BT_PID);
//REGISTER_HID_DETECTOR("Attack Shark K86 (Bluetooth)", DetectEpomakerControllers, EPOMAKER_VID, ATTACKSHARK_K86_BT_PID);
