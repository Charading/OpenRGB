/*---------------------------------------------------------*\
| CryorigH7QuadLumiControllerDetect.cpp                     |
|                                                           |
|   Detector for Cryorig H7 Quad Lumi                       |
|                                                           |
|   Adam Honse (calcprogrammer1@gmail.com)      15 Apr 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "CryorigH7QuadLumiController.h"
#include "RGBController_CryorigH7QuadLumi.h"

/*-----------------------------------------------------*\
| CRYORIG/NZXT USB IDs                                  |
\*-----------------------------------------------------*/
#define NZXT_VID                    0x1E71
#define CRYORIG_H7_QUAD_LUMI_PID    0x1712

GENERIC_HOTPLUGGABLE_DETECTOR(DetectCryorigH7QuadLumi, CryorigH7QuadLumiController, RGBController_CryorigH7QuadLumi)

REGISTER_HID_DETECTOR("CRYORIG H7 Quad Lumi",   DetectCryorigH7QuadLumi, NZXT_VID,   CRYORIG_H7_QUAD_LUMI_PID);
