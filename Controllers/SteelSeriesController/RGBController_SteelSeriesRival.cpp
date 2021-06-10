/*-----------------------------------------*\
|  RGBController_SteelSeriesRival.cpp       |
|                                           |
|  Generic RGB Interface SteelSeriesRival   |
|  Class                                    |
|                                           |
|  B Horn (bahorn) 13/05/2020               |
\*-----------------------------------------*/

#include "RGBController_SteelSeriesRival.h"

typedef struct
{
    const char* name;
    const int value;
}KeyPair;

static const KeyPair mouse_leds[]=
{
    {"Scroll Wheel", 0x10},
    {"Left 1", 0x12},
    {"Left 2", 0x14},
    {"Left 3", 0x16},
    {"Right 1", 0x13},
    {"Right 2", 0x15},
    {"Right 3", 0x17},
};
RGBController_SteelSeriesRival::RGBController_SteelSeriesRival(SteelSeriesRivalController* rival_ptr)
{
    rival = rival_ptr;
    
    name        = rival->GetDeviceName();
    vendor      = "SteelSeries";
    type        = DEVICE_TYPE_MOUSE; 
    description = "SteelSeries Rival Device";
    location    = rival->GetDeviceLocation();
    serial      = rival->GetSerialString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = STEELSERIES_RIVAL_DIRECT;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Pulsate;
    Pulsate.name       = "Pulsate";
    Pulsate.value      = STEELSERIES_RIVAL_PULSATE;
    Pulsate.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Pulsate.color_mode = MODE_COLORS_PER_LED;
    Pulsate.speed_min  = STEELSERIES_RIVAL_EFFECT_PULSATE_MIN;
    Pulsate.speed_max  = STEELSERIES_RIVAL_EFFECT_PULSATE_MAX;
    Pulsate.speed      = STEELSERIES_RIVAL_EFFECT_PULSATE_MID;
    modes.push_back(Pulsate);

    SetupZones();
}

RGBController_SteelSeriesRival::~RGBController_SteelSeriesRival()
{
    delete rival;
}

void RGBController_SteelSeriesRival::SetupZones()
{
    /* Rival 100 Series only has one Zone */
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
    logo_led.value = 0;
    leds.push_back(logo_led);

    /* Rival 300 extends this by adding another LED + Zone */
    if (rival->GetMouseType() == RIVAL_300) {
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
        wheel_led.value = 1;
        leds.push_back(wheel_led);
    }
    /* Rival 650 extends this by adding another LED + Zone */
    if (rival->GetMouseType() == RIVAL_650) {
        zone mouse_zone;
        mouse_zone.name         = "Mouse Leds";
        mouse_zone.type         = ZONE_TYPE_LINEAR;
        mouse_zone.leds_min     = 7;
        mouse_zone.leds_max     = 7;
        mouse_zone.leds_count   = 7;
        mouse_zone.matrix_map   = NULL;
        zones.push_back(mouse_zone);
        leds.pop_back();
        logo_led.value = 0x11;
        leds.push_back(logo_led);

        for(const KeyPair led_pair: mouse_leds) {
            led mouse_led;
            mouse_led.name = led_pair.name;
            mouse_led.value = led_pair.value;
            leds.push_back(mouse_led);
        }

    }
    SetupColors();
}

void RGBController_SteelSeriesRival::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SteelSeriesRival::DeviceUpdateLEDs()
{
    for (unsigned int i = 0; i < leds.size(); i++)
    {
        unsigned char red = RGBGetRValue(colors[i]);
        unsigned char grn = RGBGetGValue(colors[i]);
        unsigned char blu = RGBGetBValue(colors[i]);
        rival->SetColor(leds[i].value, red, grn, blu);
    }
}

void RGBController_SteelSeriesRival::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);

    if(zone == 0)
    {
        rival->SetColorAll(red, grn, blu);
    }
    else
    {
        /* We can add custom cases depending on different devices here.  */
        switch(rival->GetMouseType())
        {
            case RIVAL_300:
                rival->SetColor(zone, red, grn, blu);
                break;

            default:
                break;
        }
    }
}   

void RGBController_SteelSeriesRival::UpdateSingleLED(int led)
{
    /* Each zone only has a single LED, so we can use the LED ID to reference
     * the existing zone code. */
    UpdateZoneLEDs(led);
}

void RGBController_SteelSeriesRival::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_SteelSeriesRival::DeviceUpdateMode()
{
    /* Strictly, the device actually does support different modes for the 
     * different zones, but we don't support that. */
    //steelseries_type mouse_type = rival->GetMouseType();
    switch (modes[active_mode].value)
    {
        case STEELSERIES_RIVAL_DIRECT:
            rival->SetLightEffectAll(STEELSERIES_RIVAL_EFFECT_DIRECT);
            break;
        case STEELSERIES_RIVAL_PULSATE:
            rival->SetLightEffectAll(modes[active_mode].speed);
            break;
    }

    DeviceUpdateLEDs();
}
