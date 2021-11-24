/*-----------------------------------------*\
|  RGBController_HavitKB395L.cpp            |
|                                           |
|  Generic RGB Interface for Havit KB395L   |
|  Keyboard                                 |
|                                           |
|  Lisander Lopez 7/7/2021                  |
\*-----------------------------------------*/

#include "RGBController_HavitKB395L.h"
#include "LogManager.h"

RGBController_HavitKB395L::RGBController_HavitKB395L(HavitKB395LController* havit_ptr)
{
    havit = havit_ptr;
    
    name        = "Havit KB395L Keyboard";
    vendor      = "Havit";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Havit Keyboard Device";
    location    = havit->GetDeviceLocation();
    serial      = havit->GetSerialString();

    mode Static;
    Static.name             = "Static";
    Static.value            = HAVIT_KB395L_MODE_STATIC;
    Static.flags            = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode       = MODE_COLORS_PER_LED;
    Static.brightness_min   = 0x00;
    Static.brightness_max   = 0x32;
    Static.brightness       = 0x32;

    modes.push_back(Static);
    
    SetupZones();
}

RGBController_HavitKB395L::~RGBController_HavitKB395L()
{
    delete havit;
}

void RGBController_HavitKB395L::SetupZones()
{
    zone* entire_board = new zone();
    entire_board->name           = "Entire Keyboard";
    entire_board->type           = ZONE_TYPE_SINGLE;
    entire_board->leds_min       = 1;
    entire_board->leds_max       = 1;
    entire_board->leds_count     = 1;
    entire_board->matrix_map     = NULL;
    zones.push_back(*entire_board);

    led* entire_led = new led();
    entire_led->name = "Entire Keyboard";
    leds.push_back(*entire_led);

    SetupColors();
}

void RGBController_HavitKB395L::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_HavitKB395L::DeviceUpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);
    unsigned char brightness = modes[active_mode].brightness;
    
    havit->SendStaticColor(rgbToBinary(red), rgbToBinary(grn), rgbToBinary(blu), brightness);
}

void RGBController_HavitKB395L::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HavitKB395L::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HavitKB395L::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_HavitKB395L::DeviceUpdateMode()
{
    // Not Implemented
}

unsigned char RGBController_HavitKB395L::rgbToBinary(unsigned char val) 
{
    return val >> 7;
}