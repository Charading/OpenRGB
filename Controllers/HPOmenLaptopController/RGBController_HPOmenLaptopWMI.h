#ifndef RGBCONTROLLER_HPOMENLAPTOPWMI_H
#define RGBCONTROLLER_HPOMENLAPTOPWMI_H

#ifdef _WIN32

#include "HPOmenLaptopController.h"
#include "RGBController.h"

class RGBController_HPOmenLaptopWMI : public RGBController
{
private:
    HPOmenLaptopController *controller;
public:
    RGBController_HPOmenLaptopWMI(HPOmenLaptopController *controller);
    ~RGBController_HPOmenLaptopWMI();

    void SetupZones() override;
    void ResizeZone(int zone, int new_size) override; /* Not supported */
    void DeviceUpdateLEDs() override;
    void UpdateZoneLEDs(int zone) override;
    void UpdateSingleLED(int led) override; /* Not supported */
    void DeviceUpdateMode() override;
};

#endif

#endif // RGBCONTROLLER_HPOMENLAPTOPWMI_H
