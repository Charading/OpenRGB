/*---------------------------------------------------------*\
| ZalmanZSyncControllerDetect.cpp                           |
|                                                           |
|   Detector for Zalman Z Sync                              |
|                                                           |
|   Adam Honse (CalcProgrammer1)                30 Jan 2021 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "ZalmanZSyncController.h"
#include "RGBController_ZalmanZSync.h"

#define ZALMAN_VID          0x1C57
#define ZALMAN_Z_SYNC_PID   0x7ED0

/******************************************************************************************\
*                                                                                          *
*   DetectZalmanZSyncControllers                                                           *
*                                                                                          *
*       Detect devices supported by the Zalman Z Sync driver                               *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectZalmanZSyncControllers, ZalmanZSyncController, RGBController_ZalmanZSync)

REGISTER_HID_DETECTOR("Zalman Z Sync", DetectZalmanZSyncControllers, ZALMAN_VID, ZALMAN_Z_SYNC_PID);
