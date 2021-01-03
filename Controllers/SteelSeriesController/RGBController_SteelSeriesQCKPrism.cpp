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
    unsigned char red1 = RGBGetRValue(colors[0]);
    unsigned char grn1 = RGBGetGValue(colors[0]);
    unsigned char blu1 = RGBGetBValue(colors[0]);

    unsigned char red2 = RGBGetRValue(colors[1]);
    unsigned char grn2 = RGBGetGValue(colors[1]);
    unsigned char blu2 = RGBGetBValue(colors[1]);

    // both have to be set at the same time
    prism->SetColor(0, red1, grn1, blu1);
    prism->SetColor(1, red2, grn2, blu2);
}

void RGBController_SteelSeriesQCKPrism::UpdateZoneLEDs(int zone)
{

    printf("Hit\n");
    unsigned char red = RGBGetRValue(colors[zone]);
    unsigned char grn = RGBGetGValue(colors[zone]);
    unsigned char blu = RGBGetBValue(colors[zone]);
    prism->SetColor(zone, red, grn, blu);
}

void RGBController_SteelSeriesQCKPrism::UpdateSingleLED(int led)
{
    printf("Hit\n");
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
    prism->SetLightEffectAll(STEELSERIES_QCKPRISM_STATIC);

    DeviceUpdateLEDs();
}
