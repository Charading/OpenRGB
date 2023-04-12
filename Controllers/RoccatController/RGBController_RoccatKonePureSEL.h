/*---------------------------------------------------------*\
|  Roccat Kone Pure SEL                                     |
|                                                           |
|  Warren Dumortier (nwarrenfl@gmail.com), 23/03/2023       |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "RoccatKonePureSELController.h"

class RGBController_RoccatKonePureSEL : public RGBController
{
    public:
        RGBController_RoccatKonePureSEL(RoccatKonePureSELController* controller_ptr);
        ~RGBController_RoccatKonePureSEL();

        void DeviceUpdateLEDs();
        void DeviceUpdateMode();

        void UpdateZoneLEDs(int zone);
        void UpdateSingleLED(int led);

        void SetupZones();
        void ResizeZone(int zone, int new_size);

    private:
        RoccatKonePureSELController* controller;
};
