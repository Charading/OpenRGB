/*-----------------------------------------*\
|  RGBController_ASRockPolychromeUSB.h      |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  ASRock ASR LED and Polychrome RGB Driver |
|                                           |
|  Ed Kambulow (Dredvard) 12/26/2020        |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "ASRockPolychromeUSBController.h"

class RGBController_PolychromeUSB : public RGBController
{
public:
    RGBController_PolychromeUSB(PolychromeUSBController* polychrome_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    PolychromeUSBController* polychrome;
    unsigned char GetDeviceMode(unsigned char zone);
    std::vector<PolychromeZoneInfo> zones_info;
    bool                        initializedMode;
};
