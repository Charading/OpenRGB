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
#include "CorsairCapellixController.h"

class RGBController_CorsairCapellix : public RGBController
{
public:
    RGBController_CorsairCapellix(CorsairCapellixController* corsair_ptr);
    ~RGBController_CorsairCapellix();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    CorsairCapellixController*     corsair;
    std::vector<int> RGBController_CorsairCapellix::DetectFans();
};
