/*-----------------------------------------*\
|  RGBController_Sinowealth.h               |
|                                           |
|  Definitions and types for Sinowealth     |
|  mice, including Glorious                 |
|                                           |
|  Niels Westphal (crashniels) 20/5/2020    |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "SinowealthKeyboardController.h"

class RGBController_SinowealthKeyboard : public RGBController
{
public:
    RGBController_SinowealthKeyboard(SinowealthKeyboardController* sinowealth_ptr);
    ~RGBController_SinowealthKeyboard();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();

    void        DeviceUpdateMode();


private:
    SinowealthKeyboardController* sinowealth;
};
