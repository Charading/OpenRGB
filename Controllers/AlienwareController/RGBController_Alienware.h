/*-----------------------------------------*\
|  RGBController_Alienware.h                |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Alienware RGB USB Driver                 |
|                                           |
|  Gabriel Marcano (gemarcano) 4/19/2021    |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "AlienwareController.h"

#include <memory>
#include <chrono>

class RGBController_Alienware : public RGBController
{
public:
    RGBController_Alienware(std::unique_ptr<AlienwareController> alienware_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

    void        SetCustomMode();

private:
    std::unique_ptr<AlienwareController> alienware;
    std::chrono::steady_clock::time_point last_packet_ts;
};
