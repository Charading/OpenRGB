#pragma once

#include <RGBController.h>
#include <vector>
#include "LogManager.h"
#include "RGBController.h"
#include "BloodyKeyboardController.h"
#include "RGBControllerKeyNames.h"

class RGBController_BloodyKeyboard : public RGBController
{
public:
    RGBController_BloodyKeyboard(BloodyKeyboardController* controller_ptr);
    ~RGBController_BloodyKeyboard();

    void                    SetupZones();
    void                    ResizeZone(int zone, int new_size);

    void                    DeviceUpdateLEDs();
    void                    UpdateZoneLEDs(int zone);
    void                    UpdateSingleLED(int led);

    void                    DeviceUpdateMode();

private:
    BloodyKeyboardController*  controller;

};

