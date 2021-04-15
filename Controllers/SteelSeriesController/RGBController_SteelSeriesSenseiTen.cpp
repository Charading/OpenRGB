/*-----------------------------------------*\
|  RGBController_SteelSeriesRival.cpp       |
|                                           |
|  Generic RGB Interface SteelSeriesRival   |
|  Class                                    |
|                                           |
|  B Horn (bahorn) 13/05/2020               |
\*-----------------------------------------*/

#include "RGBController_SteelSeriesSenseiTen.h"

RGBController_SteelSeriesSenseiTen::RGBController_SteelSeriesSenseiTen(SteelSeriesSenseiTenController* sensei_ptr)
{
    sensei = sensei_ptr;
    
    name        = sensei->GetDeviceName();
    vendor      = "SteelSeries";
    type        = DEVICE_TYPE_MOUSE; 
    description = "SteelSeries Sensei Device";
    location    = sensei->GetDeviceLocation();
    serial      = sensei->GetSerialString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = STEELSERIES_SENSEI_DIRECT;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = STEELSERIES_SENSEI_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    Breathing.speed_min  = STEELSERIES_SENSEI_EFFECT_BREATHING_MIN;
    Breathing.speed_max  = STEELSERIES_SENSEI_EFFECT_BREATHING_MAX;
    Breathing.speed      = STEELSERIES_SENSEI_EFFECT_BREATHING_MID;
    modes.push_back(Breathing);

    SetupZones();
}

RGBController_SteelSeriesSenseiTen::~RGBController_SteelSeriesSenseiTen()
{
    delete sensei;
}

void RGBController_SteelSeriesSenseiTen::SetupZones()
{
    zone logo_zone;
    logo_zone.name          = "Logo";
    logo_zone.type          = ZONE_TYPE_SINGLE;
    logo_zone.leds_min      = 1;
    logo_zone.leds_max      = 1;
    logo_zone.leds_count    = 1;
    logo_zone.matrix_map    = NULL;
    zones.push_back(logo_zone);

    led logo_led;
    logo_led.name = "Logo";
    leds.push_back(logo_led);

    zone wheel_zone;
    wheel_zone.name         = "Scroll Wheel";
    wheel_zone.type         = ZONE_TYPE_SINGLE;
    wheel_zone.leds_min     = 1;
    wheel_zone.leds_max     = 1;
    wheel_zone.leds_count   = 1;
    wheel_zone.matrix_map   = NULL;
    zones.push_back(wheel_zone);

    led wheel_led;
    wheel_led.name = "Scroll Wheel";
    leds.push_back(wheel_led);

    SetupColors();
}

void RGBController_SteelSeriesSenseiTen::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SteelSeriesSenseiTen::DeviceUpdateLEDs()
{
    UpdateZoneLEDs(0);
    UpdateZoneLEDs(1);
}

void RGBController_SteelSeriesSenseiTen::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);

    switch (modes[active_mode].value)
    {

    case STEELSERIES_SENSEI_DIRECT:
        sensei->SetColor(zone, red, grn, blu);
        break;

    case STEELSERIES_SENSEI_BREATHING:
        sensei->SetLightEffect(zone, STEELSERIES_SENSEI_BREATHING,modes[active_mode].speed, red, grn, blu);
        break;

    }
}   

void RGBController_SteelSeriesSenseiTen::UpdateSingleLED(int led)
{
    /* Each zone only has a single LED, so we can use the LED ID to reference
     * the existing zone code. */
    UpdateZoneLEDs(led);
}

void RGBController_SteelSeriesSenseiTen::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_SteelSeriesSenseiTen::DeviceUpdateMode()
{
    /* Strictly, the device actually does support different modes for the 
     * different zones, but we don't support that. */
    //steelseries_type mouse_type = rival->GetMouseType();
    /*unsigned char red = 0;
    unsigned char grn = 0;
    unsigned char blu = 0;
    red = RGBGetRValue(modes[active_mode].colors[0]);
    grn = RGBGetGValue(modes[active_mode].colors[0]);
    blu = RGBGetBValue(modes[active_mode].colors[0]);*/
    RGBColor      color = colors[0];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);
    switch (modes[active_mode].value)
    {
        case STEELSERIES_SENSEI_DIRECT:
            //sensei->SetColorAll(red, grn, blu);
        sensei->SetColorAll(blu, grn, red);
            break;
        case STEELSERIES_SENSEI_BREATHING:
            sensei->SetLightEffectAll(STEELSERIES_SENSEI_BREATHING,modes[active_mode].speed, red, grn, blu);
            break;
    }

    //DeviceUpdateLEDs();
}
