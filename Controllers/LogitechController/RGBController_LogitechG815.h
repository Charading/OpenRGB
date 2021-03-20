/*-----------------------------------------*\
|  RGBController_LogitechG815.h             |
|                                           |
|  Generic RGB Interface for Logitech G815  |
|  RGB Mechanical Gaming keyboard           |
|                                           |
|  TheRogueZeta   1/23/2020                 |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "LogitechG815Controller.h"
#include <QTimer>

class RGBController_LogitechG815 : public RGBController
{
public:
    RGBController_LogitechG815(LogitechG815Controller* logitech_ptr);
    ~RGBController_LogitechG815();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    LogitechG815Controller*   logitech;
    std::vector<RGBColor> current_colors;
};
