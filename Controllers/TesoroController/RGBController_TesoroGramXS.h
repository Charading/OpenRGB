/*-----------------------------------------*\
|  RGBController_TesoroGramXS.h             |
|                                           |
|  RGB Interface for Tesoro Gram XS         |
|  Keyboard                                 |
|                                           |
|  Ondřej Niesner 8/31/2023                 |
\*-----------------------------------------*/

#include "RGBController.h"
#include "TesoroGramXSController.h"

#pragma once

class RGBController_TesoroGramXS : public RGBController
{
public:
    RGBController_TesoroGramXS(TesoroGramXSController *controller_ptr);
    ~RGBController_TesoroGramXS();

    void SetupZones();

    void ResizeZone(int zone, int new_size);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);

    void DeviceUpdateMode();

private:
    TesoroGramXSController *controller;
};
