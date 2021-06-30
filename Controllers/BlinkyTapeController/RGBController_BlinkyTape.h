/*-----------------------------------------*\
|  RGBController_LEDStrip.h                 |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  KeyboardVisualizer LED strip interface   |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/20/2019   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "serial_port.h"
#include "BlinkyTapeController.h"

class RGBController_BlinkyTape : public RGBController
{
public:
    RGBController_BlinkyTape(BlinkyTapeController* blinkytape_ptr);
    ~RGBController_BlinkyTape();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    BlinkyTapeController* blinkytape;
};
