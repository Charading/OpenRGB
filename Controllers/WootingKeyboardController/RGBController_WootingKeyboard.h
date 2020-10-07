/*-----------------------------------------*\
|  RGBController_WootingKeyboard.h            |
|                                           |
|  Generic RGB Interface for Wooting RGB      |
|  keyboard devices                         |
|                                           |
|  Adam Honse (CalcProgrammer1) 7/4/2020    |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "WootingKeyboardController.h"

class RGBController_WootingKeyboard : public RGBController
{
public:
    RGBController_WootingKeyboard(WootingKeyboardController* Wooting_ptr);
    ~RGBController_WootingKeyboard();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    WootingKeyboardController*    Wooting;
};