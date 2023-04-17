/*---------------------------------------------------------*\
|  RGBController_BlinkStick.h                               |
|                                                           |
|  Generic RGB Interface for BlinkStick LED controller      |
|                                                           |
|  Foxdogface(https://github.com/Foxdogface), 29/11/2022    |
\*---------------------------------------------------------*/

#pragma once
#include "RGBController.h"
#include "BlinkStickController.h"

struct BlinkStick_zone
{
    std::string             name;
    std::vector<uint8_t>    zone_leds;
};

typedef std::vector<BlinkStick_zone> BlinkStick_layout;

class RGBController_BlinkStick : public RGBController
{
public:
    RGBController_BlinkStick(BlinkStickController* controller_ptr);
    ~RGBController_BlinkStick();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    BlinkStickController*   controller;

    void        InitZones();
    
};
