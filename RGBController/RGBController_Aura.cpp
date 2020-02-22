/*-----------------------------------------*\
|  RGBController_Aura.cpp                   |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  Asus Aura driver                         |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/13/2019   |
\*-----------------------------------------*/

#include "RGBController_Aura.h"

int RGBController_Aura::GetDeviceMode()
{
    int  dev_mode  = aura->AuraRegisterRead(AURA_REG_MODE);
    int color_mode = MODE_COLORS_PER_LED;

    if (aura->AuraRegisterRead(AURA_REG_DIRECT))
    {
        dev_mode = 0xFFFF;
    }

    switch(dev_mode)
    {
    case AURA_MODE_OFF:
    case AURA_MODE_RAINBOW:
    case AURA_MODE_SPECTRUM_CYCLE:
    case AURA_MODE_RANDOM_FLICKER:
        color_mode = MODE_COLORS_NONE;
        break;

    case AURA_MODE_SPECTRUM_CYCLE_CHASE:
        dev_mode   = AURA_MODE_CHASE;
        color_mode = MODE_COLORS_RANDOM;
        break;

    case AURA_MODE_SPECTRUM_CYCLE_BREATHING:
        dev_mode   = AURA_MODE_BREATHING;
        color_mode = MODE_COLORS_RANDOM;
        break;

    case AURA_MODE_SPECTRUM_CYCLE_CHASE_FADE:
        dev_mode   = AURA_MODE_CHASE_FADE;
        color_mode = MODE_COLORS_RANDOM;
        break;
    }

    for(int mode = 0; mode < modes.size(); mode++)
    {
        if(modes[mode].value == dev_mode)
        {
            active_mode            = mode;
            modes[mode].color_mode = color_mode;
        }
    }

    return(active_mode);
}

void RGBController_Aura::UpdateLEDs()
{
    for(std::size_t led = 0; led < colors.size(); led++)
    {
        unsigned char red = RGBGetRValue(colors[led]);
        unsigned char grn = RGBGetGValue(colors[led]);
        unsigned char blu = RGBGetBValue(colors[led]);

        if (GetMode() == 0)
        {
            aura->SetLEDColorDirect(led, red, grn, blu);
        }
        else
        {
            aura->SetLEDColorEffect(led, red, grn, blu);
        }
    }
}

void RGBController_Aura::UpdateZoneLEDs(int zone)
{
    for (std::size_t x = 0; x < zones[zone].map.size(); x++)
    {
        for (std::size_t y = 0; y < zones[zone].map[x].size(); y++)
        {
            int           led   = zones[zone].map[x][y];
            RGBColor      color = colors[led];
            unsigned char red   = RGBGetRValue(color);
            unsigned char grn   = RGBGetGValue(color);
            unsigned char blu   = RGBGetBValue(color);

            if (GetMode() == 0)
            {
                aura->SetLEDColorDirect(led, red, grn, blu);
            }
            else
            {
                aura->SetLEDColorEffect(led, red, grn, blu);
            }
        }
    }
}

void RGBController_Aura::UpdateSingleLED(int led)
{
    RGBColor color    = colors[led];
    unsigned char red = RGBGetRValue(color);
    unsigned char grn = RGBGetGValue(color);
    unsigned char blu = RGBGetBValue(color);

    if (GetMode() == 0)
    {
        aura->SetLEDColorDirect(led, red, grn, blu);
    }
    else
    {
        aura->SetLEDColorEffect(led, red, grn, blu);
    }
}

RGBController_Aura::RGBController_Aura(AuraController * aura_ptr)
{
    std::vector<unsigned char> aura_channels;

    aura = aura_ptr;

    version = aura->GetDeviceName();
    location = aura->GetDeviceLocation();
    if((version.find("DIMM_LED") != std::string::npos) || (version.find("AUDA") != std::string::npos) )
    {
        type = DEVICE_TYPE_DRAM;
        name = "ASUS Aura DRAM";
    }
    else
    {
        type = DEVICE_TYPE_MOTHERBOARD;
        name = "ASUS Aura Motherboard";
    }

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0xFFFF;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Off;
    Off.name       = "Off";
    Off.value      = AURA_MODE_OFF;
    Off.flags      = 0;
    Off.color_mode = MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name       = "Static";
    Static.value      = AURA_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);
    
    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = AURA_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_PER_LED_COLOR;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Breathing);

    mode Flashing;
    Flashing.name       = "Flashing";
    Flashing.value      = AURA_MODE_FLASHING;
    Flashing.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Flashing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Flashing);

    mode SpectrumCycle;
    SpectrumCycle.name       = "Spectrum Cycle";
    SpectrumCycle.value      = AURA_MODE_SPECTRUM_CYCLE;
    SpectrumCycle.flags      = 0;
    SpectrumCycle.color_mode = MODE_COLORS_NONE;
    modes.push_back(SpectrumCycle);

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.value      = AURA_MODE_RAINBOW;
    Rainbow.flags      = 0;
    Rainbow.color_mode = MODE_COLORS_NONE;
    modes.push_back(Rainbow);

    mode ChaseFade;
    ChaseFade.name       = "Chase Fade";
    ChaseFade.value      = AURA_MODE_CHASE_FADE;
    ChaseFade.flags      = MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_PER_LED_COLOR;
    ChaseFade.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(ChaseFade);

    mode Chase;
    Chase.name       = "Chase";
    Chase.value      = AURA_MODE_CHASE;
    Chase.flags      = MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_PER_LED_COLOR;
    Chase.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Chase);

    mode RandomFlicker;
    RandomFlicker.name       = "Random Flicker";
    RandomFlicker.value      = AURA_MODE_RANDOM_FLICKER;
    RandomFlicker.flags      = 0;
    RandomFlicker.color_mode = MODE_COLORS_NONE;
    modes.push_back(RandomFlicker);

    colors.resize(aura->GetLEDCount());

    for (std::size_t i = 0; i < aura->GetLEDCount(); i++)
    {
        aura_channels.push_back(aura->GetChannel(i));

        led* new_led = new led();

        new_led->name = aura->GetChannelName(i);
        new_led->name.append(std::to_string(i));

        leds.push_back(*new_led);

        unsigned char red = aura->GetLEDRed(i);
        unsigned char grn = aura->GetLEDGreen(i);
        unsigned char blu = aura->GetLEDBlue(i);

        colors[i] = ToRGBColor(red, grn, blu);
    }

    std::vector<unsigned char> aura_zones;

    // Search through all LEDs and create zones for each channel type
    for (std::size_t i = 0; i < aura_channels.size(); i++)
    {
        bool matched = false;

        // Search through existing zones to make sure we don't create a duplicate zone
        for (std::size_t j = 0; j < aura_zones.size(); j++)
        {
            if (aura_channels[i] == aura_zones[j])
            {
                matched = true;
            }
        }

        // If zone does not already exist, create it
        if (matched == false)
        {
            zone* new_zone = new zone();
            std::vector<int>* zone_row = new std::vector<int>();

            // Set zone name to channel name
            new_zone->name = aura->GetChannelName(i);

            // Find all LEDs with this channel type and add them to zone
            for (std::size_t j = 0; j < aura->GetLEDCount(); j++)
            {
                if (aura->GetChannel(j) == aura_channels[i])
                {
                    zone_row->push_back(j);
                }
            }

            // Aura devices can be either single or linear, never matrix
            // That means only one row is needed
            new_zone->map.push_back(*zone_row);

            // Save channel to aura_zones so we know not to create another zone with this channel
            aura_zones.push_back(aura_channels[i]);

            // Push new zone to zones vector
            zones.push_back(*new_zone);
        }
    }

    // Initialize active mode
    active_mode = GetDeviceMode();
}

void RGBController_Aura::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_Aura::UpdateMode()
{
    if (modes[active_mode].value == 0xFFFF)
    {
        aura->SetDirect(true);
    }
    else
    {
        int new_mode = modes[active_mode].value;

        if(modes[active_mode].color_mode == MODE_COLORS_RANDOM)
        {
            switch(new_mode)
            {
            case AURA_MODE_CHASE:
                new_mode = AURA_MODE_SPECTRUM_CYCLE_CHASE;
                break;
            case AURA_MODE_BREATHING:
                new_mode = AURA_MODE_SPECTRUM_CYCLE_BREATHING;
                break;
            case AURA_MODE_CHASE_FADE:
                new_mode = AURA_MODE_SPECTRUM_CYCLE_CHASE_FADE;
                break;
            }
        }

        aura->SetMode(new_mode);
        aura->SetDirect(false);
    }
}
