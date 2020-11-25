#ifndef RGBGROUPCONTROLLER_H
#define RGBGROUPCONTROLLER_H

#pragma once
#include "RGBController.h"
#include <vector>

class RGBGroupController : public RGBController
{
public:
    RGBGroupController(std::string _name, std::vector<RGBController *> _rgb_group);
    ~RGBGroupController();

    std::vector<RGBController*> rgb_group;

    void        SetupZones();
    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    void        SetupModes();
};

#endif // RGBGROUPCONTROLLER_H
