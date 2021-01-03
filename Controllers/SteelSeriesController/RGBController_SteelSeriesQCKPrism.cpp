/*------------------------------------------*\
|  RGBController_SteelSeriesQCKPrism.cpp     |
|                                            |
|  Generic RGB Interface SteelSeriesQCKPrism |
|  Class                                     |
|                                            |
|  B Horn (bahorn) 30/12/2020                |
\*------------------------------------------*/

#include "RGBController_SteelSeriesQCKPrism.h"

RGBController_SteelSeriesQCKPrism::RGBController_SteelSeriesQCKPrism(SteelSeriesQCKPrismController* prism_ptr)
{
    prism = prism_ptr;
    
    name        = prism->GetDeviceName();
    vendor      = "SteelSeries";
    type        = DEVICE_TYPE_MOUSEMAT; 
    description = "SteelSeries QCK Prism Mousemat";
    location    = prism->GetDeviceLocation();
    serial      = prism->GetSerialString();

    mode Static;
    Static.name       = "Static";
    Static.value      = STEELSERIES_QCKPRISM_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Rainbow;
    Static.name       = "Rainbow";
    Static.value      = STEELSERIES_QCKPRISM_RAINBOW;
    Static.flags      = MODE_FLAG_HAS_RANDOM_COLOR| MODE_FLAG_HAS_BRIGHTNESS;
    Static.color_mode = MODE_COLORS_RANDOM;
    modes.push_back(Static);

    SetupZones();
}

void RGBController_SteelSeriesQCKPrism::SetupZones()
{
    zone fst_zone, snd_zone;
    fst_zone.name          = "First Zone";
    fst_zone.type          = ZONE_TYPE_SINGLE;
    fst_zone.leds_min      = 1;
    fst_zone.leds_max      = 1;
    fst_zone.leds_count    = 1;
    fst_zone.matrix_map    = NULL;
    zones.push_back(fst_zone);

    snd_zone.name          = "Second Zone";
    snd_zone.type          = ZONE_TYPE_SINGLE;
    snd_zone.leds_min      = 1;
    snd_zone.leds_max      = 1;
    snd_zone.leds_count    = 1;
    snd_zone.matrix_map    = NULL;
    zones.push_back(snd_zone);

    led fst_led, snd_led;
    fst_led.name = "Zone 1 LED";
    leds.push_back(fst_led);

    snd_led.name = "Zone 2 LED";
    leds.push_back(snd_led);

    SetupColors();
}

void RGBController_SteelSeriesQCKPrism::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SteelSeriesQCKPrism::DeviceUpdateLEDs()
{
    UpdateZoneLEDs(0);
    UpdateZoneLEDs(1);
}

void RGBController_SteelSeriesQCKPrism::UpdateZoneLEDs(int zone)
{

    unsigned char red = RGBGetRValue(colors[zone]);
    unsigned char grn = RGBGetGValue(colors[zone]);
    unsigned char blu = RGBGetBValue(colors[zone]);
    prism->SetColor(zone, red, grn, blu);
}

void RGBController_SteelSeriesQCKPrism::UpdateSingleLED(int led)
{
    /* Each zone only has a single LED, so we can use the LED ID to reference
     * the existing zone code. */
    UpdateZoneLEDs(led);
}

void RGBController_SteelSeriesQCKPrism::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_SteelSeriesQCKPrism::DeviceUpdateMode()
{

    switch (modes[active_mode].value)
    {
        case STEELSERIES_QCKPRISM_STATIC:
            //rival->SetLightEffectAll(STEELSERIES_RIVAL_EFFECT_STATIC);
            break;
        case STEELSERIES_QCKPRISM_RAINBOW:
            break;
    }

    DeviceUpdateLEDs();
}
