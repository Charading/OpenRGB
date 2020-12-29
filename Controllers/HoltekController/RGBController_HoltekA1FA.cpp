/*--------------------------------------------------------------*\
|  RGBController_HoltekA1FA.cpp                                  |
|                                                                |
|  Generic RGB Interface for Holtek based Mousemat [04d9:a1fa]   |
|                                                                |
|  Edoardo Ridolfi (edo2313) 26/12/2020                          |
\*--------------------------------------------------------------*/

#include "RGBController_HoltekA1FA.h"

RGBController_HoltekA1FA::RGBController_HoltekA1FA(HoltekA1FAController* holtek_ptr)
{
    holtek = holtek_ptr;

    name        = "Holtek Mousemat Device";
    vendor      = "Holtek";
    type        = DEVICE_TYPE_MOUSEMAT;
    description = "Holtek Mousemat Device";
    location    = holtek->GetDeviceLocation();
    serial      = holtek->GetSerialString();

    mode Static;
    Static.name       = "Static";
    Static.value      = HOLTEK_A1FA_MODE_STATIC;
    Static.speed      = HOLTEK_A1FA_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = HOLTEK_A1FA_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    Breathing.speed_min  = HOLTEK_A1FA_SPEED_SLOWEST;
    Breathing.speed_max  = HOLTEK_A1FA_SPEED_FASTEST;
    Breathing.speed      = HOLTEK_A1FA_SPEED_NORMAL;
    modes.push_back(Breathing);

    mode Neon;
    Neon.name           = "Neon";
    Neon.value          = HOLTEK_A1FA_MODE_NEON;
    Neon.flags          = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Neon.color_mode     = MODE_COLORS_NONE;
    Neon.speed_min      = HOLTEK_A1FA_SPEED_SLOWEST;
    Neon.speed_max      = HOLTEK_A1FA_SPEED_FASTEST;
    Neon.speed          = HOLTEK_A1FA_SPEED_NORMAL;
    modes.push_back(Neon);

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.value      = HOLTEK_A1FA_MODE_RAINBOW;
    Rainbow.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Rainbow.color_mode = MODE_COLORS_NONE;
    Rainbow.speed_min  = HOLTEK_A1FA_SPEED_SLOWEST;
    Rainbow.speed_max  = HOLTEK_A1FA_SPEED_FASTEST;
    Rainbow.speed      = HOLTEK_A1FA_SPEED_NORMAL;
    modes.push_back(Rainbow);

    SetupZones();
}

void RGBController_HoltekA1FA::SetupZones()
{
    zone mouse_zone;
    mouse_zone.name          = "Mousemat";
    mouse_zone.type          = ZONE_TYPE_SINGLE;
    mouse_zone.leds_min      = 1;
    mouse_zone.leds_max      = 1;
    mouse_zone.leds_count    = 1;
    mouse_zone.matrix_map    = NULL;
    zones.push_back(mouse_zone);

    led mouse_led;
    mouse_led.name = "Mousemat";
    leds.push_back(mouse_led);

    SetupColors();
}

void RGBController_HoltekA1FA::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_HoltekA1FA::DeviceUpdateLEDs()
{
    unsigned char mode = modes[active_mode].value;
    unsigned char speed = modes[active_mode].speed;
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char green = RGBGetGValue(colors[0]);
    unsigned char blue = RGBGetBValue(colors[0]);

    holtek->SendData(mode, speed, red, green, blue);
}

void RGBController_HoltekA1FA::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HoltekA1FA::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HoltekA1FA::SetCustomMode()
{
    DeviceUpdateLEDs();
}

void RGBController_HoltekA1FA::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
