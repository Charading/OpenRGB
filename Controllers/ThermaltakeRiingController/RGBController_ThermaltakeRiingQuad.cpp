/*-------------------------------------------------------------------*\
|  RGBController_ThermaltakeRiingQuad.cpp                             |
|                                                                     |
|  Driver for Thermaltake Riing Quad Controller                       |
|                                                                     |
|  Chris M (Dr_No)          15th Feb 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_ThermaltakeRiingQuad.h"

RGBController_ThermaltakeRiingQuad::RGBController_ThermaltakeRiingQuad(ThermaltakeRiingQuadController* quad_ptr)
{
    quad = quad_ptr;

    name        = quad->GetDeviceName();
    vendor      = "Thermaltake";
    type        = DEVICE_TYPE_COOLER;
    description = quad->GetDeviceName();
    location    = quad->GetDeviceLocation();
    serial      = quad->GetSerial();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = TT_QUAD_MODE_DIRECT;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.speed_min  = 0;
    Direct.speed_max  = 0;
    Direct.speed      = 0;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    /*mode Static;
    Static.name       = "Static";
    Static.value      = TT_QUAD_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.colors_min = 1;
    Static.colors_max = 1;
    Static.speed_min  = 0;
    Static.speed_max  = 0;
    Static.speed      = 0;
    Static.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Static.colors.resize(1);
    modes.push_back(Static);*/

    /*mode Flow;
    Flow.name       = "Flow";
    Flow.value      = TT_QUAD_MODE_FLOW;
    Flow.flags      = MODE_FLAG_HAS_SPEED;
    Flow.speed_min  = TT_QUAD_SPEED_SLOW;
    Flow.speed_max  = TT_QUAD_SPEED_FASTER;
    Flow.speed      = TT_QUAD_SPEED_NORMAL;
    Flow.color_mode = MODE_COLORS_NONE;
    modes.push_back(Flow);*/

    /*mode Spectrum;
    Spectrum.name       = "Spectrum";
    Spectrum.value      = TT_QUAD_MODE_SPECTRUM;
    Spectrum.flags      = MODE_FLAG_HAS_SPEED;
    Spectrum.speed_min  = TT_QUAD_SPEED_SLOW;
    Spectrum.speed_max  = TT_QUAD_SPEED_FASTER;
    Spectrum.speed      = TT_QUAD_SPEED_NORMAL;
    Spectrum.color_mode = MODE_COLORS_NONE;
    modes.push_back(Spectrum);*/

    //radar
    //weather
    //player
    //swirl
    //scan
    //stack
    //heartbeat
    //raindrop
    //twinkle
    //aurora
    //blackhole
    //shootingstar
    //cross

    /*mode Ripple;
    Ripple.name       = "Ripple";
    Ripple.value      = TT_QUAD_MODE_RIPPLE;
    Ripple.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Ripple.speed_min  = TT_QUAD_SPEED_SLOW;
    Ripple.speed_max  = TT_QUAD_SPEED_FASTER;
    Ripple.speed      = TT_QUAD_SPEED_NORMAL;
    Ripple.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Ripple);*/

    /*mode Blink;
    Blink.name       = "Blink";
    Blink.value      = TT_QUAD_MODE_BLINK;
    Blink.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Blink.speed_min  = TT_QUAD_SPEED_SLOW;
    Blink.speed_max  = TT_QUAD_SPEED_FASTER;
    Blink.speed      = TT_QUAD_SPEED_NORMAL;
    Blink.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Blink);*/

    /*mode Pulse;
    Pulse.name       = "Pulse";
    Pulse.value      = TT_QUAD_MODE_PULSE;
    Pulse.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Pulse.speed_min  = TT_QUAD_SPEED_SLOW;
    Pulse.speed_max  = TT_QUAD_SPEED_FASTER;
    Pulse.speed      = TT_QUAD_SPEED_NORMAL;
    Pulse.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Pulse);*/

    /*mode Wave;
    Wave.name       = "Wave";
    Wave.value      = TT_QUAD_MODE_WAVE;
    Wave.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
    Wave.speed_min  = TT_QUAD_SPEED_SLOW;
    Wave.speed_max  = TT_QUAD_SPEED_FASTER;
    Wave.speed      = TT_QUAD_SPEED_NORMAL;
    Wave.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Wave);*/

    Init_Controller();
    SetupZones();
}

RGBController_ThermaltakeRiingQuad::~RGBController_ThermaltakeRiingQuad()
{
    delete quad;
}

void RGBController_ThermaltakeRiingQuad::Init_Controller()
{
    for (unsigned int zone_index = 0; zone_index < TT_QUAD_ZONES; zone_index++)
    {
        zone quad_zone;
        quad_zone.name          = "Riing Zone " + std::to_string(zone_index + 1);
        quad_zone.type          = ZONE_TYPE_LINEAR;
        quad_zone.leds_min      = 0;
        quad_zone.leds_max      = 54;   //Riing Quad protocol must support 54 LEDs
        quad_zone.leds_count    = 0;
        quad_zone.matrix_map    = NULL;
        zones.push_back(quad_zone);
    }
}

void RGBController_ThermaltakeRiingQuad::SetupZones()
{
    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    colors.clear();

    /*-------------------------------------------------*\
    | Set leds                                          |
    \*-------------------------------------------------*/
    for (std::size_t zone_index = 0; zone_index < TT_QUAD_ZONES; zone_index++)
    {
        int led_count   = zones[zone_index].leds_count;
        for (std::size_t led_index = 0; led_index < led_count; led_index++)
        {
            led new_led;

            new_led.name = zones[zone_index].name + " ";
            new_led.name.append(" LED " + std::to_string(led_index));
            new_led.value = zone_index + 1;

            leds.push_back(new_led);
        }
    }

    SetupColors();
}

void RGBController_ThermaltakeRiingQuad::ResizeZone(int zone, int new_size)
{
    if((size_t) zone >= zones.size())
    {
        return;
    }

    if(((unsigned int)new_size >= zones[zone].leds_min) && ((unsigned int)new_size <= zones[zone].leds_max))
    {
        zones[zone].leds_count = new_size;

        SetupZones();
    }
}

void RGBController_ThermaltakeRiingQuad::DeviceUpdateLEDs()
{
    for(std::size_t zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        quad->SetChannelLEDs(leds[zones[zone_index].start_idx].value, zones[zone_index].colors, zones[zone_index].leds_count);
    }
}

void RGBController_ThermaltakeRiingQuad::UpdateZoneLEDs(int zone)
{
    quad->SetChannelLEDs(leds[zones[zone].start_idx].value, zones[zone].colors, zones[zone].leds_count);
}

void RGBController_ThermaltakeRiingQuad::UpdateSingleLED(int led)
{
    int zone_index = leds[led].value - 1;
    quad->SetChannelLEDs(leds[led].value, zones[zone_index].colors, zones[zone_index].leds_count);
}

void RGBController_ThermaltakeRiingQuad::SetCustomMode()
{
    for(std::size_t mode_index = 0; mode_index < modes.size() ; mode_index++)
    {
        if (modes[mode_index].value == TT_QUAD_MODE_DIRECT)
        {
            active_mode = mode_index;
            break;
        }
    }
}

void RGBController_ThermaltakeRiingQuad::DeviceUpdateMode()
{
    quad->SetMode(modes[active_mode].value, modes[active_mode].speed);
    DeviceUpdateLEDs();
}
