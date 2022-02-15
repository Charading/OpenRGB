/*-----------------------------------------*\
|  RGBController_CorsairVoidPro.h           |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Corsair Void Pro Driver                  |
|                                           |
|  Guimard Morgan (morg) 2/08/2022          |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "CorsairVoidProController.h"

class RGBController_CorsairVoidPro : public RGBController
{
public:
    RGBController_CorsairVoidPro(CorsairVoidProController* controller_ptr);
    ~RGBController_CorsairVoidPro();

    void SetupZones();
    void ResizeZone(int zone, int new_size);
    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);
    void DeviceUpdateMode();
    void SetCustomMode();

private:
    CorsairVoidProController* controller;
};
