/*-----------------------------------------*\
|  RGBController_Razer.h                    |
|                                           |
|  Generic RGB Interface for Razer devices  |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/22/2021   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "RazerController.h"

class RGBController_Razer : public RGBController
{
public:
    RGBController_Razer(RazerController* controller_ptr);
    ~RGBController_Razer();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    RazerController*    controller;
};
