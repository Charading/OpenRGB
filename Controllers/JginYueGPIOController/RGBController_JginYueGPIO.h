/*-----------------------------------------------*\
|  RGBController_JginYueGPIO.h             |
|                                                 |
|  Generic RGB Interface JginYueGPIO Class |
|                                                 |
|  Adam Honse (CalcProgrammer1) 2/25/2020         |
\*-----------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "JginYueGPIOController.h"

class RGBController_JginYueGPIO : public RGBController
{
public:
    RGBController_JginYueGPIO(JginYueGPIOController* controller_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    JginYueGPIOController*        controller;
};
