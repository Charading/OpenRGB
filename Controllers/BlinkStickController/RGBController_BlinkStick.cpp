/*---------------------------------------------------------*\
|  RGBController_BlinkStick.cpp                             |
|                                                           |
|  Generic RGB Interface for BlinkStick Led controller      |
|                                                           |
|  Foxdogface(https://gitlab.com/Foxdogface), 29/11/2022    |
\*---------------------------------------------------------*/

#include "RGBController_BlinkStick.h"

static const BlinkStick_layout pro_layout
{
    {
        "R channel",    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63}
    },
    {
        "G channel",    {64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127}
    },
    {
        "B channel",    {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191}
    }
};

RGBController_BlinkStick::RGBController_BlinkStick(BlinkStickController* controller_ptr)
{
    controller  = controller_ptr;

    name        = "BlinkStick";
    vendor      = "Agile Innovative Ltd";
    type        = DEVICE_TYPE_LEDSTRIP;
    description = "BlinkStick Controller Device";
    location    = controller->GetLocation();

    if(controller->generation == BLINKSTICK_PRO)
    {
        mode ARGB;
        ARGB.name = "ARGB LED srtip";
        ARGB.value = ARGB_MODE;
        ARGB.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
        ARGB.color_mode = MODE_COLORS_PER_LED;
        modes.push_back(ARGB);
    }
    else if(controller->generation == BLINKSTICK)
    {
        mode RGBStrip;
        RGBStrip.name       = "RGBStrip";
        RGBStrip.value      = RGB_MODE;
        RGBStrip.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
        RGBStrip.color_mode = MODE_COLORS_PER_LED;
        RGBStrip.colors_max = 1;
        RGBStrip.colors.resize(1);
        modes.push_back(RGBStrip);
    }

    InitZones();
}

RGBController_BlinkStick::~RGBController_BlinkStick()
{
    delete controller;
}

void RGBController_BlinkStick::InitZones()
{
    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    colors.clear();

    if (controller->generation == BLINKSTICK_PRO)
    {
        BlinkStick_layout layout = pro_layout;

        for(uint8_t zone_idx = 0; zone_idx < layout.size(); zone_idx++)
        {
            BlinkStick_zone BS_zone = layout[zone_idx];

            zone new_zone;
            new_zone.name           = BS_zone.name;
            new_zone.leds_min       = 1;
            new_zone.leds_max       = static_cast<unsigned int>(BS_zone.zone_leds.size());
            new_zone.leds_count     = new_zone.leds_min;
            new_zone.type           = ZONE_TYPE_LINEAR;
            new_zone.matrix_map     = NULL;
            zones.push_back(new_zone);

            for(unsigned int lp_idx = 0; lp_idx < zones[zone_idx].leds_count; lp_idx++)
            {
                led new_led;

                new_led.value   = BS_zone.zone_leds[lp_idx];
                new_led.name    = BS_zone.name.substr(0, 1) + std::to_string(lp_idx);

                leds.push_back(new_led);
            }
        }
    }
    else if(controller->generation == BLINKSTICK)
    {
        zone new_zone;
        new_zone.name           = "RGB Strip";
        new_zone.leds_min       = 1;
        new_zone.leds_max       = new_zone.leds_min;
        new_zone.leds_count     = new_zone.leds_min;
        new_zone.type           = ZONE_TYPE_SINGLE;
        new_zone.matrix_map     = NULL;
        zones.push_back(new_zone);

        led new_led;

        new_led.value = 0;
        new_led.name = "RGB LED";

        leds.push_back(new_led);

    }

    SetupColors();
}

void RGBController_BlinkStick::SetupZones()
{
    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    colors.clear();

    if (controller->generation == BLINKSTICK_PRO && modes[active_mode].value == ARGB_MODE)
    {
        BlinkStick_layout layout = pro_layout;

        for(uint8_t zone_idx = 0; zone_idx < layout.size(); zone_idx++)
        {
            for(unsigned int lp_idx = 0; lp_idx < zones[zone_idx].leds_count; lp_idx++)
            {
                led new_led;

                new_led.value       = pro_layout[zone_idx].zone_leds[lp_idx];
                new_led.name    = zones[zone_idx].name.substr(0, 1) + std::to_string(lp_idx);

                leds.push_back(new_led);
            }
        }
    }

    SetupColors();
}

void RGBController_BlinkStick::ResizeZone(int zone, int new_size)
{
    /*-------------------------------------------------*\
    | Explicitly cast these to avoid compiler warnings  |
    \*-------------------------------------------------*/
    const unsigned int zone_u       = static_cast<unsigned int>(zone);
    const unsigned int new_size_u   = static_cast<unsigned int>(new_size);

    /*-------------------------------------------------*\
    | Check that the zone is in bounds                  |
    \*-------------------------------------------------*/
    if((zone_u > zones.size()) || (zone < 0))
    {
        return;
    }

    /*-------------------------------------------------*\
    | And that the new size is in bounds                |
    \*-------------------------------------------------*/
    if((new_size_u > zones.at(zone).leds_max) || (new_size_u < zones.at(zone).leds_min))
    {
        return;
    }

    /*-------------------------------------------------*\
    | And that there's actually a change                |
    \*-------------------------------------------------*/
    if(zones.at(zone).leds_count == new_size_u)
    {
        return;
    }

    /*-------------------------------------------------*\
    | If the new size is less than the current size,    |
    | just chop off the end                             |
    \*-------------------------------------------------*/
    if(zones[zone].leds_count > new_size_u)
    {
        zones[zone].leds_count = new_size;
    }

    zones[zone].leds_count = new_size;

    SetupZones();
}

void RGBController_BlinkStick::DeviceUpdateLEDs()
{
    if (modes[active_mode].value == ARGB_MODE)
    {
        for (int zone_idx = 0; zone_idx < zones.size(); zone_idx++)
        {
            zone curr_zone  = zones[zone_idx];
            int start_idx   = curr_zone.start_idx;
            int end_idx     = start_idx + curr_zone.leds_count;

            std::vector<RGBColor>* color_list = new std::vector<RGBColor>[curr_zone.leds_count];

            for (uint8_t i = start_idx; i < end_idx; i++)
            {
                color_list->push_back(colors[i]);
            }

            controller->SetARGBLedsDirect(zone_idx, color_list);
        }
    }
}

void RGBController_BlinkStick::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_BlinkStick::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_BlinkStick::DeviceUpdateMode()
{

}
