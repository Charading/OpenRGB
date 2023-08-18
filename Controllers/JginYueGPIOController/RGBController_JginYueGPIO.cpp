/*------------------------------------------------*\
|  RGBController_JginYueGPIO.cpp                   |
|                                                  |
|  Generic RGB Interface JginYueGPIO Class         |
|                                                  |
|  Adam Honse (CalcProgrammer1) 2/25/2020          |
|  Adam Honse (CalcProgrammer1) 2/25/2020          |
\*------------------------------------------------*/

#include "RGBController_JginYueGPIO.h"
#include <string.h>

#define JGINYUE_MAX_ZONES               1
#define JGINYUE_ADDRESSABLE_MAX_LEDS    100
/**------------------------------------------------------------------*\
    @name JginYueGPIO
    @category JginYue devices
    @type  USB
    @save :white_check_mark: or :robot: or :o: or :x:
    @direct :white_check_mark: or :rotating_light: or :o: or :x:
    @effects :white_check_mark: or :rotating_light: or :tools: or :o: or :x:
    @detectors DetectJginYueGPIO,DetectJginYueGPIO2
    @comment Insert multiline JginYueGPIO comment here

        | Symbol | Meaning |
        | :---: | :--- |
        | :white_check_mark: | Fully supported by OpenRGB |
        | :rotating_light: | Support is problematic |
        | :robot: | Feature is automatic and can not be turned off |
        | :tools: | Partially supported by OpenRGB |
        | :o: | Not currently supported by OpenRGB |
        | :x: | Not applicable for this device |
*/

RGBController_JginYueGPIO::RGBController_JginYueGPIO(JginYueGPIOController* controller_ptr)
{
    controller                      =controller_ptr;

    name                            =controller->GetDeviceName();
    description                     ="JGINYUE USB ARGB Device";
    vendor                          ="JGINYUE";
    type                            =DEVICE_TYPE_MOTHERBOARD;
    location                        =controller->GetDeviceLocation();
    version                         =controller->GetDeviceFWVirson();

    mode Off;
    Off.name                        ="Off";
    Off.value                       =JGINYUE_GPIO_MODE_OFF;
    Off.flags                       =0;
    Off.color_mode                  =MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name                     ="Static";
    Static.value                    =JGINYUE_GPIO_MODE_STATIC;
    Static.flags                    =MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.color_mode               =MODE_COLORS_MODE_SPECIFIC;
    Static.colors_max               =1;
    Static.colors_min               =1;
    Static.colors.resize(1);
    modes.push_back(Static);


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
    zones[0].leds_max   = 100;
    zones[0].matrix_map = NULL;

    if(first_run)
    {
        zones[0].leds_count=10;
    }

    for (unsigned int led_idx = 0 , j=1; led_idx < zones[0].leds_count ; led_idx++)
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
    for (int i = 0; i < JGINYUE_MAX_ZONES; i++)
    {
        UpdateZoneLEDs(i);
    }

}

void RGBController_JginYueGPIO::UpdateZoneLEDs(int zone)
{
    unsigned char area;
    switch (zone)
    {
    case 0:
        area = 0x01;
        break;
    case 1:
        area = 0x02;
        break;
    default:
        area = 0x01;
        break;
    }
    controller->DirectLEDControl(zones[zone].colors,area);
}

void RGBController_JginYueGPIO::UpdateSingleLED(int led)
{
    int zone;
    zone = leds[led].value;
    UpdateZoneLEDs(zone);
}


void RGBController_JginYueGPIO::DeviceUpdateMode()
{
    unsigned char area;




}
