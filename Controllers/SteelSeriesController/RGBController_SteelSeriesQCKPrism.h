/*------------------------------------------*\
|  RGBController_SteelSeriesQCKPrism.h       |
|                                            |
|  Generic RGB Interface SteelSeriesQCKPrism |
|  Class                                     |
|                                            |
|  B Horn (bahorn) 30/12/2020                |
\*------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "SteelSeriesQCKPrismController.h"

class RGBController_SteelSeriesQCKPrism : public RGBController
{
public:
    RGBController_SteelSeriesQCKPrism(SteelSeriesQCKPrismController* prism_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    SteelSeriesQCKPrismController*     prism;
};
