#ifndef RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS_H
#define RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS_H

#include "HPOmenLaptopController_windows.h"
#include "RGBController.h"

class RGBController_HPOmenLaptopWMI_windows : public RGBController
{
private:
    HPOmenLaptopController_windows *controller;
public:
    RGBController_HPOmenLaptopWMI_windows(HPOmenLaptopController_windows *controller);
    ~RGBController_HPOmenLaptopWMI_windows();

    void SetupZones() override;
    void ResizeZone(int zone, int new_size) override; /* Not supported */
    void DeviceUpdateLEDs() override;
    void UpdateZoneLEDs(int zone) override;
    void UpdateSingleLED(int led) override; /* Not supported */
    void DeviceUpdateMode() override;
};

#endif // RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS_H
