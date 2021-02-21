/*-----------------------------------------*\
|  RGBController_LianLiUniHub.cpp           |
|                                           |
|  Generic RGB Interface for Lian Li Uni    |
|  Hub USB controller driver                |
|                                           |
|  Luca Lovisa 2/20/2021                    |
\*-----------------------------------------*/

#include "RGBController_LianLiUniHub.h"

#include <cassert>
#include <string>

RGBController_LianLiUniHub::RGBController_LianLiUniHub(LianLiUniHubController* uniHub_ptr)
    : initializedMode(false)
{
    uniHub = uniHub_ptr;

    name        = "Lian Li Uni Hub";
    vendor      = "Lian Li";
    version     = "";
    type        = DEVICE_TYPE_UNKNOWN; /* TODO What device type is this? */
    description = "Lian Li Uni Hub";
    location    = uniHub->GetLocation();
    serial      = uniHub->GetSerial();

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.value      = UNIHUB_LED_MODE_RAINBOW;
    Rainbow.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_DIRECTION_LR;
    Rainbow.speed_min  = 1;
    Rainbow.speed_max  = 5;
    Rainbow.color_mode = MODE_COLORS_NONE;
    modes.push_back(Rainbow);

    mode StaticColor;
    StaticColor.name       = "Static Color";
    StaticColor.value      = UNIHUB_LED_MODE_STATIC_COLOR;
    StaticColor.flags      = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR;
    StaticColor.colors_min = 1;
    StaticColor.colors_max = 64;
    StaticColor.color_mode = MODE_COLORS_PER_LED;
    StaticColor.colors.resize(64);
    modes.push_back(StaticColor);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = UNIHUB_LED_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR;
    Breathing.speed_min  = 1;
    Breathing.speed_max  = 5;
    Breathing.colors_min = 1;
    Breathing.colors_max = 64;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    Breathing.colors.resize(1);
    modes.push_back(Breathing);

    mode ColorCycle;
    ColorCycle.name       = "Color Cycle";
    ColorCycle.value      = UNIHUB_LED_MODE_COLOR_CYCLE;
    ColorCycle.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    ColorCycle.speed_min  = 1;
    ColorCycle.speed_max  = 5;
    ColorCycle.colors_min = 3;
    ColorCycle.colors_max = 3;
    ColorCycle.color_mode = MODE_COLORS_MODE_SPECIFIC;
    ColorCycle.colors.resize(3);
    modes.push_back(ColorCycle);

    mode Runway;
    Runway.name       = "Runway";
    Runway.value      = UNIHUB_LED_MODE_RUNWAY;
    Runway.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Runway.speed_min  = 1;
    Runway.speed_max  = 5;
    Runway.colors_min = 2;
    Runway.colors_max = 2;
    Runway.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Runway.colors.resize(2);
    modes.push_back(Runway);

    /* Not yet implemented. See LianLiUniHubController.h */
#if 0
    mode RunwaySync;
#endif

    mode Stagggered;
    Stagggered.name       = "Stagggered";
    Stagggered.value      = UNIHUB_LED_MODE_STAGGGERED;
    Stagggered.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Stagggered.speed_min  = 1;
    Stagggered.speed_max  = 5;
    Stagggered.colors_min = 2;
    Stagggered.colors_max = 2;
    Stagggered.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Stagggered.colors.resize(2);
    modes.push_back(Stagggered);

    mode Mixing;
    Mixing.name       = "Mixing";
    Mixing.value      = UNIHUB_LED_MODE_MIXING;
    Mixing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Mixing.speed_min  = 1;
    Mixing.speed_max  = 5;
    Mixing.colors_min = 2;
    Mixing.colors_max = 2;
    Mixing.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Mixing.colors.resize(2);
    modes.push_back(Mixing);

    mode Meteor;
    Meteor.name       = "Meteor";
    Meteor.value      = UNIHUB_LED_MODE_METEOR;
    Meteor.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Meteor.speed_min  = 1;
    Meteor.speed_max  = 5;
    Meteor.colors_min = 2;
    Meteor.colors_max = 2;
    Meteor.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Meteor.colors.resize(2);
    modes.push_back(Meteor);

    /* Not yet implemented. See LianLiUniHubController.h */
#if 0
    mode MeteorSync;
#endif

    mode Firework;
    Firework.name       = "Firework";
    Firework.value      = UNIHUB_LED_MODE_FIREWORK;
    Firework.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Firework.speed_min  = 1;
    Firework.speed_max  = 5;
    Firework.colors_min = 2;
    Firework.colors_max = 2;
    Firework.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Firework.colors.resize(2);
    modes.push_back(Firework);

    mode Stack;
    Stack.name       = "Stack";
    Stack.value      = UNIHUB_LED_MODE_STACK;
    Stack.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Stack.speed_min  = 1;
    Stack.speed_max  = 5;
    Stack.colors_min = 1;
    Stack.colors_max = 1;
    Stack.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Stack.colors.resize(1);
    modes.push_back(Stack);

    mode StackMultiColor;
    StackMultiColor.name       = "Stack Multi Color";
    StackMultiColor.value      = UNIHUB_LED_MODE_STACK_MULTI_COLOR;
    StackMultiColor.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_DIRECTION_LR;
    StackMultiColor.speed_min  = 1;
    StackMultiColor.speed_max  = 5;
    StackMultiColor.color_mode = MODE_COLORS_NONE;
    modes.push_back(StackMultiColor);

    mode Neon;
    Neon.name       = "Neon";
    Neon.value      = UNIHUB_LED_MODE_NEON;
    Neon.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Neon.speed_min  = 1;
    Neon.speed_max  = 5;
    Neon.color_mode = MODE_COLORS_NONE;
    modes.push_back(Neon);

    RGBController_LianLiUniHub::SetupZones();
}

RGBController_LianLiUniHub::~RGBController_LianLiUniHub()
{
    delete uniHub;
}

void RGBController_LianLiUniHub::SetupZones()
{
    /*-------------------------------------------------*\
    | Only set LED count on the first run               |
    \*-------------------------------------------------*/
    bool first_run = false;

    if(zones.size() == 0)
    {
        first_run = true;
    }

    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    colors.clear();
    zones.resize(UNIHUB_CHANNEL_COUNT);

    /*-------------------------------------------------*\
    | Set zones and leds                                |
    \*-------------------------------------------------*/
    int addressableCounter = 1;
    for (unsigned int channel_idx = 0; channel_idx < zones.size(); channel_idx++)
    {
        zones[channel_idx].name       = "Channel ";
        zones[channel_idx].name.append(std::to_string(addressableCounter));

        addressableCounter++;

        zones[channel_idx].type       = ZONE_TYPE_LINEAR;

        zones[channel_idx].leds_min   = 0;
        zones[channel_idx].leds_max   = UNIHUB_CHANLED_COUNT;

        if (first_run)
        {
            zones[channel_idx].leds_count = zones[channel_idx].leds_min;
        }

        for (unsigned int led_ch_idx = 0; led_ch_idx < zones[channel_idx].leds_count; led_ch_idx++)
        {
            led new_led;
            new_led.name = zones[channel_idx].name;
            new_led.name.append(", LED ");
            new_led.name.append(std::to_string(led_ch_idx + 1));
            new_led.value = channel_idx;

            leds.push_back(new_led);
        }

        zones[channel_idx].matrix_map = NULL;
    }

    SetupColors();
}

void RGBController_LianLiUniHub::ResizeZone(int zone, int new_size)
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

void RGBController_LianLiUniHub::DeviceUpdateLEDs()
{
    if(!initializedMode)
    {
        DeviceUpdateMode();
    }
    for(size_t channel = 0; channel < zones.size(); channel++)
    {
        uint8_t fanCount = convertLedCountToFanCount(zones[channel].leds_count);
        uniHub->SetAnyFanCount(channel, convertAnyFanCount(fanCount));
        uniHub->SetLedColors(channel, zones[channel].colors, zones[channel].leds_count);
    }

    uniHub->Synchronize();
}

void RGBController_LianLiUniHub::UpdateZoneLEDs(int zone)
{
    if(!initializedMode)
    {
        DeviceUpdateMode();
    }
    unsigned int channel = zone;

    uint8_t fanCount = convertLedCountToFanCount(zones[channel].leds_count);
    uniHub->SetAnyFanCount(channel, convertAnyFanCount(fanCount));
    uniHub->SetLedColors(channel, zones[channel].colors, zones[channel].leds_count);

    uniHub->Synchronize();
}

void RGBController_LianLiUniHub::UpdateSingleLED(int led)
{
    if(!initializedMode)
    {
        DeviceUpdateMode();
    }
    unsigned int channel = leds[led].value;

    uint8_t fanCount = convertLedCountToFanCount(zones[channel].leds_count);
    uniHub->SetAnyFanCount(channel, convertAnyFanCount(fanCount));
    uniHub->SetLedColors(channel, zones[channel].colors, zones[channel].leds_count);

    uniHub->Synchronize();
}

void RGBController_LianLiUniHub::DeviceUpdateMode()
{
    initializedMode = true;

    for (size_t channel = 0; channel < zones.size(); channel++)
    {
        uint8_t fanCount = convertLedCountToFanCount(zones[channel].leds_count);
        uniHub->SetAnyFanCount(channel, convertAnyFanCount(fanCount));

        switch (modes[active_mode].color_mode)
        {
            case MODE_COLORS_PER_LED: {
                uniHub->SetLedColors(channel, zones[channel].colors, zones[channel].leds_count);
            } break;

            case MODE_COLORS_MODE_SPECIFIC: {
                uniHub->SetLedColors(channel, modes[active_mode].colors.data(), modes[active_mode].colors.size());
            } break;

            default: {
                uniHub->SetLedColors(channel, nullptr, 0);
            } break;
        }

        uniHub->SetLedMode(channel, modes[active_mode].value);

        if (modes[active_mode].flags & MODE_FLAG_HAS_SPEED)
        {
            uniHub->SetLedSpeed(channel, convertLedSpeed(modes[active_mode].speed));
        }
        else
        {
            uniHub->SetLedSpeed(channel, UNIHUB_LED_SPEED_000);
        }

        if (modes[active_mode].flags & MODE_FLAG_HAS_DIRECTION_LR)
        {
            uniHub->SetLedDirection(channel, convertLedDirection(modes[active_mode].direction));
        }
        else
        {
            uniHub->SetLedDirection(channel, UNIHUB_LED_DIRECTION_LTR);
        }
    }

    uniHub->Synchronize();
}

void RGBController_LianLiUniHub::SetCustomMode()
{

}

uint8_t RGBController_LianLiUniHub::convertAnyFanCount(
        uint8_t count
) {
    switch (count)
    {
        case 1: return UNIHUB_ANY_FAN_COUNT_001;
        case 2: return UNIHUB_ANY_FAN_COUNT_002;
        case 3: return UNIHUB_ANY_FAN_COUNT_003;
        case 4: return UNIHUB_ANY_FAN_COUNT_004;

        default:
            assert(false);
            return UNIHUB_ANY_FAN_COUNT_001;
    }
}

uint8_t RGBController_LianLiUniHub::convertLedSpeed(
        uint8_t speed
) {
    switch (speed)
    {
        case 1: return UNIHUB_LED_SPEED_000;
        case 2: return UNIHUB_LED_SPEED_025;
        case 3: return UNIHUB_LED_SPEED_050;
        case 4: return UNIHUB_LED_SPEED_075;
        case 5: return UNIHUB_LED_SPEED_100;

        default:
            assert(false);
            return UNIHUB_LED_SPEED_050;
    }
}

uint8_t RGBController_LianLiUniHub::convertLedDirection(
        uint8_t direction
) {
    switch (direction)
    {
        case 0: return UNIHUB_LED_DIRECTION_LTR;
        case 1: return UNIHUB_LED_DIRECTION_RTL;

        default:
            assert(false);
            return UNIHUB_LED_DIRECTION_LTR;
    }
}

/* Converts 0-16 to 1, 17-32 to 2, 33-48 to 3  and 49-64+ to 4 */
uint8_t RGBController_LianLiUniHub::convertLedCountToFanCount(
        uint8_t count
) {
    if (count < 0x01)
    {
        count = 0x01;
    }
    if (count > 0x40)
    {
        count = 0x40;
    }

    return (count -1) / 16 + 1;
}
