/*-------------------------------------------------------------------*\
|  RGBController_CMRGBController.h                                    |
|                                                                     |
|  Driver for Coolermaster RGB USB Controller                         |
|                                                                     |
|  Nic W (midgetspy)        13th Apr 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#pragma once
#include "RGBController.h"
#include "CMRGBController.h"
#include <vector>

#define CM_RGBC_NUM_PORTS 4

class RGBController_CMRGBController : public RGBController
{
public:
    RGBController_CMRGBController(CMRGBController* cmargb_ptr);
    ~RGBController_CMRGBController();

    void        SetupZones();
    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

    CMRGBController* cmargb;
private:
    int MidPoint(int a, int b);
};
