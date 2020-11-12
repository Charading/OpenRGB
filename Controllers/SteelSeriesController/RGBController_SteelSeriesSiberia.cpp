/*-----------------------------------------*\
|  RGBController_SteelSeriesSiberia.cpp     |
|                                           |
|  Generic RGB Interface SteelSeriesSiberia |
|  Class                                    |
|                                           |
|  E Karlsson (pilophae) 18/06/2020         |
\*-----------------------------------------*/

#include "RGBController_SteelSeriesSiberia.h"

RGBController_SteelSeriesSiberia::RGBController_SteelSeriesSiberia(SteelSeriesSiberiaController* siberia_ptr)
{
    siberia = siberia_ptr;
    
    name        = siberia->GetDeviceName();
    type        = DEVICE_TYPE_HEADSET; 
    description = "SteelSeries Siberia Device";
    //location    = siberia->GetDeviceLocation();

    mode Static;
    Static.name       = "Static";
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    SetupZones();
}

void RGBController_SteelSeriesSiberia::SetupZones()
{
    /* Siberia 350 only has one Zone */
    zone earpiece_zone;
    earpiece_zone.name          = "Headset";
    earpiece_zone.type          = ZONE_TYPE_SINGLE;
    earpiece_zone.leds_min      = 1;
    earpiece_zone.leds_max      = 1;
    earpiece_zone.leds_count    = 1;
    earpiece_zone.matrix_map    = NULL;
    zones.push_back(earpiece_zone);

    led earpiece_led;
    earpiece_led.name = "Headset LED";
    leds.push_back(earpiece_led);

    SetupColors();
}

void RGBController_SteelSeriesSiberia::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SteelSeriesSiberia::DeviceUpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);
    siberia->SetColor(red, grn, blu);
}

void RGBController_SteelSeriesSiberia::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);
    siberia->SetColor(red, grn, blu);
}

void RGBController_SteelSeriesSiberia::UpdateSingleLED(int led)
{
    /* Each zone only has a single LED, so we can use the LED ID to reference
     * the existing zone code. */
    UpdateZoneLEDs(led);
}

void RGBController_SteelSeriesSiberia::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_SteelSeriesSiberia::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
