/*-----------------------------------------------*\
|  RGBController_JginYueGPIO.h                    |
|                                                 |
|  Generic RGB Interface JginYueGPIO Class        |
|                                                 |
|  Tong R    (tcr020)  2023/08/23                 |
|  Liu  ShiMeng   (Moon dream stars)   2003/08/23 |
|                                                 |
| Dongguan Yonghang Electronic Technology Co., Ltd|
\*-----------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "JginYueGPIOController.h"

class RGBController_JginYueGPIO : public RGBController
{
public:
    RGBController_JginYueGPIO(JginYueGPIOController* controller_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    JginYueGPIOController*        controller;
};
