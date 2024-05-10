/*------------------------------------------------*\
|  RGBController_JginYueGPIO_Windows.cpp           |
|                                                  |
|  Generic RGB Interface JginYuePCH Class          |
|                                                  |
|  Tong R    (tcr020)  2023/08/23                  |
|  Liu  ShiMeng   (Moon dream stars)   2003/08/23  |
|                                                  |
|  Dongguan Yonghang Electronic Technology Co., Ltd|
\*------------------------------------------------*/

#include "RGBController_JginYueGPIO_Windows.h"
#include <string.h>

#define JGINYUE_MAX_ZONES               1
#define JGINYUE_ADDRESSABLE_MAX_LEDS    50
/**------------------------------------------------------------------*\
    @name JginYueGPIO
    @category Motherboard
    @type  PCH
    @save ::x:
    @direct :white_check_mark:
    @effects :x:
    @detectors DetectJginYueGPIO,

*/

RGBController_JginYueGPIO::RGBController_JginYueGPIO(JginYueGPIOController* controller_ptr)
{
    controller                      =controller_ptr;

    name                            =controller->GetDeviceName();
    description                     ="JGINYUE PCH ARGB Device";
    vendor                          ="JGINYUE";
    type                            =DEVICE_TYPE_MOTHERBOARD;
    location                        =controller->GetDeviceLocation();
    version                         =controller->GetDeviceFWVirson();

    mode Direct;
    Direct.name                     ="Direct";
    Direct.value                    =JGINYUE_GPIO_MODE_DIRECT;
    Direct.flags                    =MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode               =MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}




void RGBController_JginYueGPIO::SetupZones()
{
    bool first_run = false;

    if(zones.size() == 0)
    {
        first_run = true;
    }

    leds.clear();
    colors.clear();
    zones.resize(JGINYUE_MAX_ZONES);


    zones[0].name       = "ARGB_Header";
    zones[0].type       = ZONE_TYPE_LINEAR;
    zones[0].leds_min   = 1;
    zones[0].leds_max   = 50;
    zones[0].matrix_map = NULL;

    if(first_run)
    {
        zones[0].leds_count=10;
    }

    for(unsigned int led_idx = 0 , j=1; led_idx < zones[0].leds_count ; led_idx++)
    {
        led ARGB_1_HEADER;
        ARGB_1_HEADER.name    = "ARGB_HEADER_1_LED No." + std::to_string(j);
        ARGB_1_HEADER.value   = 0;
        leds.push_back(ARGB_1_HEADER);
        j++;
    }
    SetupColors();
}

void RGBController_JginYueGPIO::ResizeZone(int zone, int new_size)
{
    zones[zone].leds_count=new_size;
    SetupZones();
    controller->Area_resize(new_size,zone);
}

void RGBController_JginYueGPIO::DeviceUpdateLEDs()
{
    for(int i = 0; i < JGINYUE_MAX_ZONES; i++)
    {
        UpdateZoneLEDs(i);
    }
}

void RGBController_JginYueGPIO::UpdateZoneLEDs(int zone)
{
    controller->DirectLEDControl(zones[zone].colors,zone,zones[zone].leds_count);
}

void RGBController_JginYueGPIO::UpdateSingleLED(int led)
{
    int zone;
    zone = leds[led].value;
    UpdateZoneLEDs(zone);
}


void RGBController_JginYueGPIO::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
    controller->DirectLEDControl(controller->LEDupdate, 0, zones[0].leds_count);
}


