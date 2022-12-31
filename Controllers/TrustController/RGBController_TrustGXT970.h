/*-----------------------------------------*\
|  RGBController_TrustGXT970.h              |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Trust GXT 970 USB Driver                 |
|                                           |
|  David Heidelberg 31/12/2022              |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "TrustGXT970Controller.h"

class RGBController_TrustGXT970 : public RGBController
{
public:
    RGBController_TrustGXT970(TrustGXT970Controller* controller_ptr);
    ~RGBController_TrustGXT970();

    void SetupZones();
    void ResizeZone(int zone, int new_size);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);

    void DeviceUpdateMode();

private:
    TrustGXT970Controller* controller;
};
