/*---------------------------------------------------------*\
| AnnePro2ControllerDetect.cpp                              |
|                                                           |
|   Detector for Obins Lab AnnePro2 keyboard                |
|                                                           |
|   Sergey Gavrilov (DrZlo13)                   06 Jun 2021 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AnnePro2Controller.h"
#include "RGBController_AnnePro2.h"

/*---------------------------------------------------------*\
| Anne Pro 2 vendor IDs                                     |
\*---------------------------------------------------------*/
#define ANNE_PRO_2_VID_1                            0x04D9
#define ANNE_PRO_2_VID_2                            0x3311

/*---------------------------------------------------------*\
| Anne Pro 2 product IDs                                    |
\*---------------------------------------------------------*/
#define ANNE_PRO_2_PID_1                            0x8008
#define ANNE_PRO_2_PID_2                            0x8009
#define ANNE_PRO_2_PID_3                            0xA292
#define ANNE_PRO_2_PID_4                            0xA293
#define ANNE_PRO_2_PID_5                            0xA297

/******************************************************************************************\
*                                                                                          *
*   DetectAnnePro2Controllers                                                              *
*                                                                                          *
*       Tests the USB address to see if an Obins Lab AnnePro2 keyboard exists there.       *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAnnePro2Controllers, AnnePro2Controller, RGBController_AnnePro2)

REGISTER_HID_DETECTOR_I("Anne Pro 2", DetectAnnePro2Controllers, ANNE_PRO_2_VID_1, ANNE_PRO_2_PID_1, 1);
REGISTER_HID_DETECTOR_I("Anne Pro 2", DetectAnnePro2Controllers, ANNE_PRO_2_VID_1, ANNE_PRO_2_PID_2, 1);
REGISTER_HID_DETECTOR_I("Anne Pro 2", DetectAnnePro2Controllers, ANNE_PRO_2_VID_1, ANNE_PRO_2_PID_3, 1);
REGISTER_HID_DETECTOR_I("Anne Pro 2", DetectAnnePro2Controllers, ANNE_PRO_2_VID_1, ANNE_PRO_2_PID_4, 1);
REGISTER_HID_DETECTOR_I("Anne Pro 2", DetectAnnePro2Controllers, ANNE_PRO_2_VID_2, ANNE_PRO_2_PID_5, 1);
