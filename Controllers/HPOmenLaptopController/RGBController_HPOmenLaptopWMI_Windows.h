#ifndef RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS2_H
#define RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS2_H

#include "HPOmenLaptopController_Windows.h"
#include "RGBController.h"

class RGBController_HPOmenLaptopWMI_Windows : public RGBController
{
private:
    HPOmenLaptopController_Windows *controller;
public:
    RGBController_HPOmenLaptopWMI_Windows(HPOmenLaptopController_Windows *controller);
    ~RGBController_HPOmenLaptopWMI_Windows();

    void SetupZones() override;
    void ResizeZone(int zone, int new_size) override; /* Not supported */
    void DeviceUpdateLEDs() override;
    void UpdateZoneLEDs(int zone) override;
    void UpdateSingleLED(int led) override; /* Not supported */
    void DeviceUpdateMode() override;
};

#endif // RGBCONTROLLER_HPOMENLAPTOPWMI_WINDOWS2_H
