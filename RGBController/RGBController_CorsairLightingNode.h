/*-----------------------------------------*\
|  RGBController_CorsairLightingNode.h           |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Lighting Node Pro                        |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/12/2020   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "CorsairLightingNodeController.h"

class RGBController_CorsairLightingNode : public RGBController
{
public:
    RGBController_CorsairLightingNode(CorsairLightingNodeController* corsair_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    CorsairLightingNodeController*   corsair;
    std::vector<unsigned int>   leds_channel;
    std::vector<unsigned int>   zones_channel;
};
