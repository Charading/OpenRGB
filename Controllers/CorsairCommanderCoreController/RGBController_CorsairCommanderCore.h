/*-----------------------------------------*\
|  RGBController_CorsairCapellix.h          |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Capellix Series                          |
|  Based on code by:                        |
|  Adam Honse (CalcProgrammer1) 8/17/2020   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "CorsairCommanderCoreController.h"

class RGBController_CorsairCommanderCore : public RGBController
{
public:
    RGBController_CorsairCommanderCore(CorsairCommanderCoreController* corsair_ptr);
    ~RGBController_CorsairCommanderCore();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    CorsairCommanderCoreController*     corsair;
};
