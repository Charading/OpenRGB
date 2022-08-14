/*-------------------------------------------------------------------*\
|  RGBController_QMKXAP.h                                             |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "QMKXAPController.h"

class RGBController_QMKXAP : public RGBController
{
public:
    RGBController_QMKXAP(QMKXAPController* controller_ptr, const char * path);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    QMKXAPController* controller;
};
