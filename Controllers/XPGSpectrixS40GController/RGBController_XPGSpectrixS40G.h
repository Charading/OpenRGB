/*-----------------------------------------*\
|  RGBController_XPGSpectrixS40G.h          |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  Corsair Vengeance Pro RGB driver         |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/30/2019   |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "XPGSpectrixS40GController.h"

class RGBController_XPGSpectrixS40G : public RGBController
{
public:
    RGBController_XPGSpectrixS40G(XPGSpectrixS40GController* spectrix_ptr);
    ~RGBController_XPGSpectrixS40G();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    XPGSpectrixS40GController* xpg;
};
