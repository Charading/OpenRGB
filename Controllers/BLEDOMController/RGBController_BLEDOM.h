#ifndef RGBCONTROLLER_BLEDOM_H
#define RGBCONTROLLER_BLEDOM_H

#include <RGBController.h>
#include "BLEDOMController.h"

class RGBController_BLEDOM : public RGBController
{
public:
    RGBController_BLEDOM(BLEDOMController* controller_ptr);
    ~RGBController_BLEDOM();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    BLEDOMController*   controller;
    long long int       lastUpdate = 0;

};

#endif // RGBCONTROLLER_BLEDOM_H
