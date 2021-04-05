#pragma once
#include "RGBController.h"
#include "CreativeSoundBlasterG6XController.h"

class RGBController_CreativeSoundBlasterG6X: public RGBController
{
public:
    RGBController_CreativeSoundBlasterG6X(CreativeSoundBlasterG6XController* creative_device);
    ~RGBController_CreativeSoundBlasterG6X();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    CreativeSoundBlasterG6XController*   creative;
};
