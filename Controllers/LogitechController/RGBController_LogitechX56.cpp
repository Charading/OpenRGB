/*-----------------------------------------*\
|  RGBController_LogitechX56.cpp            |
|                                           |
|  Generic RGB Interface Logitech X56       |
|  Class                                    |
|                                           |
|  Edbgon 11/06/2021                        |
\*-----------------------------------------*/

#include "RGBController_LogitechX56.h"

RGBController_LogitechX56::RGBController_LogitechX56(LogitechX56Controller* x56_ptr)
{
    x56         = x56_ptr;
    
    name        = x56->GetDeviceName();
    vendor      = "Logitech";
    type        = DEVICE_TYPE_GAMEPAD; 
    description = "Logitech X56 Device";
    location    = x56->GetDeviceLocation();
    serial      = x56->GetSerialString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_LogitechX56::~RGBController_LogitechX56()
{
    delete x56;
}

void RGBController_LogitechX56::SetupZones()
{
    /*---------------------------------------------------------*\
    | Each device has only one zone and LED                     |
    \*---------------------------------------------------------*/
    zone mousemat_zone;
    mousemat_zone.name          = "X56";
    mousemat_zone.type          = ZONE_TYPE_SINGLE;
    mousemat_zone.leds_min      = 1;
    mousemat_zone.leds_max      = 1;
    mousemat_zone.leds_count    = 1;
    mousemat_zone.matrix_map    = NULL;
    zones.push_back(mousemat_zone);

    led bot_led;
    bot_led.name = "LED";
    leds.push_back(bot_led);

    SetupColors();
}

void RGBController_LogitechX56::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_LogitechX56::DeviceUpdateLEDs()
{
    x56->SetColors(colors);
}

void RGBController_LogitechX56::UpdateZoneLEDs(int /*zone*/)
{
    /*---------------------------------------------------------*\
    | Packet expects both LEDs                                  |
    \*---------------------------------------------------------*/
    DeviceUpdateLEDs();
}

void RGBController_LogitechX56::UpdateSingleLED(int /*led*/)
{
    /*---------------------------------------------------------*\
    | Packet expects both LEDs                                  |
    \*---------------------------------------------------------*/
    DeviceUpdateLEDs();
}

void RGBController_LogitechX56::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_LogitechX56::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
