/*-----------------------------------------*\
|  RGBController_MSICoreliquidK360.h        |
|                                           |
|  RGB Interface for Coreliquid device      |
|  Class                                    |
|                                           |
|  Aapo Kossi     25/1/2023                 |
\*-----------------------------------------*/

#pragma once

#include <string>
#include "RGBController.h"
#include "MSICoreliquidController.h"


class RGBController_MSICoreliquidK360 : public RGBController
{
public:
    RGBController_MSICoreliquidK360(MSICoreliquidController* controller_ptr);
    ~RGBController_MSICoreliquidK360();

    void SetupZones();
    void SetupMode(std::string name, int value, int flags);
    void ResizeZone(int zone, int new_size);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);
    void DeviceUpdateMode();

private:    
    MSICoreliquidController* controller;
    unsigned char color_enabled;
};
