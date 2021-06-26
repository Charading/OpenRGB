/*-----------------------------------------*\
|  RGBController_Alienware.cpp              |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Alienware USB Driver                     |
|                                           |
|  Gabriel Marcano (gemarcano) 4/19/2021    |
\*-----------------------------------------*/

#include "RGBController_Alienware.h"

#include <thread>
#include <chrono>

RGBController_Alienware::RGBController_Alienware(std::unique_ptr<AlienwareController> alienware_ptr)
: alienware(std::move(alienware_ptr))
{
    name                        = alienware->GetDeviceName();
    vendor                      = "Alienware Corporation";
    type                        = DEVICE_TYPE_KEYBOARD;
    description                 = "Alienware USB Device";
    location                    = alienware->GetDeviceLocation();
    serial                      = alienware->GetSerialString();
    version                     = alienware->GetFirmwareVersion();

    mode Color{};
    Color.name                 = "Static";
    Color.value                = ALIENWARE_MODE_COLOR;
    Color.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Color.color_mode           = MODE_COLORS_PER_LED;
    Color.colors_min           = 1;
    Color.colors_max           = 1;
    modes.push_back(Color);

    mode Pulse{};
    Pulse.name                 = "Flashing";
    Pulse.value                = ALIENWARE_MODE_PULSE;
    Pulse.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Pulse.color_mode           = MODE_COLORS_PER_LED;
    Pulse.colors_min           = 1;
    Pulse.colors_max           = 1;
    Pulse.speed_min            = ALIENWARE_TEMPO_MIN;
    Pulse.speed_max            = ALIENWARE_TEMPO_MAX;
    Pulse.speed                = ALIENWARE_TEMPO_MIN;
    modes.push_back(Pulse);

    mode Morph{};
    Morph.name                 = "Morph";
    Morph.value                = ALIENWARE_MODE_MORPH;
    Morph.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Morph.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Morph.colors_min           = 2 * alienware->GetZoneCount();
    Morph.colors_max           = Morph.colors_min;
    Morph.colors.resize(Morph.colors_max);
    Morph.speed_min            = ALIENWARE_TEMPO_MIN;
    Morph.speed_max            = ALIENWARE_TEMPO_MAX;
    Morph.speed                = ALIENWARE_TEMPO_MIN;
    modes.push_back(Morph);

    mode Spectrum{};
    Spectrum.name              = "Spectrum Cycle";
    Spectrum.value             = ALIENWARE_MODE_SPECTRUM;
    Spectrum.flags             = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Spectrum.color_mode        = MODE_COLORS_NONE;
    Spectrum.speed_min         = ALIENWARE_TEMPO_SPECTRUM;
    Spectrum.speed_max         = ALIENWARE_TEMPO_MAX;
    Spectrum.speed             = ALIENWARE_TEMPO_SPECTRUM;
    modes.push_back(Spectrum);

    mode Rainbow{};
    Rainbow.name               = "Rainbow Wave";
    Rainbow.value              = ALIENWARE_MODE_RAINBOW;
    Rainbow.flags              = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Rainbow.color_mode         = MODE_COLORS_NONE;
    Rainbow.speed_min          = ALIENWARE_TEMPO_SPECTRUM;
    Rainbow.speed_max          = ALIENWARE_TEMPO_MAX;
    Rainbow.speed              = ALIENWARE_TEMPO_SPECTRUM;
    modes.push_back(Rainbow);

    mode Breathing{};
    Breathing.name             = "Breathing";
    Breathing.value            = ALIENWARE_MODE_BREATHING;
    Breathing.flags            = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Breathing.color_mode       = MODE_COLORS_PER_LED;
    Pulse.colors_min           = 1;
    Pulse.colors_max           = 1;
    Breathing.speed_min        = ALIENWARE_TEMPO_MIN;
    Breathing.speed_max        = ALIENWARE_TEMPO_MAX;
    Breathing.speed            = ALIENWARE_TEMPO_MIN;
    modes.push_back(Breathing);

    SetupZones();
    alienware->UpdateDim();
}

void RGBController_Alienware::SetupZones()
{
    /*-------------------------------------------------*\
    | Set zones and leds                                |
    \*-------------------------------------------------*/
    std::vector<const char*> zone_names = alienware->GetZoneNames();
    for(unsigned int zone_idx = 0; zone_idx < alienware->GetZoneCount(); zone_idx++)
    {
        zone new_zone{};
        new_zone.name       = zone_names[zone_idx];
        new_zone.type       = ZONE_TYPE_SINGLE;
        new_zone.leds_min   = 1;
        new_zone.leds_max   = 1;
        new_zone.leds_count = 1;
        new_zone.matrix_map = nullptr;
        zones.emplace_back(std::move(new_zone));
    }

    for(unsigned int led_idx = 0; led_idx < zones.size(); led_idx++)
    {
        led new_led;
        new_led.name = zones[led_idx].name + std::string(" LED");
        leds.emplace_back(new_led);
    }

    SetupColors();
}

void RGBController_Alienware::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_Alienware::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_Alienware::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_Alienware::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_Alienware::SetCustomMode()
{
    /*---------------------------------------------------------*\
    | This device does not support custom modes                 |
    \*---------------------------------------------------------*/
}

static bool modes_eq(const mode& mode1, const mode& mode2)
{
    return mode1.name == mode2.name &&
        mode1.value == mode2.value &&
        mode1.flags == mode2.flags &&
        mode1.speed_min == mode2.speed_min &&
        mode1.speed_max == mode2.speed_max &&
        mode1.colors_min == mode2.colors_min &&
        mode1.colors_max == mode2.colors_max &&
        mode1.speed == mode2.speed &&
        mode1.direction == mode2.direction &&
        mode1.color_mode == mode2.color_mode &&
        mode1.colors == mode2.colors;
}

void RGBController_Alienware::DeviceUpdateMode()
{
    // Copy mode to get the current state-- this is racy, as the UI thread can
    // be actively modifying this variable
    auto current_mode = active_mode;
    auto mode = modes[current_mode];

    bool done = false;
    while (!done)
    {
        // Setup state per zone
        for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
        {
            auto zone = zones[zone_idx];
            // Some modes use 2000ms (0x07d0) for their duration, per traces
            uint16_t period = 0x07d0;

            alienware->SetMode(zone_idx, mode.value);
            switch (current_mode)
            {
            case ALIENWARE_MODE_COLOR:
                alienware->SetPeriod(zone_idx, period);
                alienware->SetColor(zone_idx, colors[zone.start_idx]);
                alienware->SetTempo(zone_idx, ALIENWARE_TEMPO_MAX);
                break;
            case ALIENWARE_MODE_PULSE:
                alienware->SetPeriod(zone_idx, period);
                alienware->SetColor(zone_idx, colors[zone.start_idx]);
                alienware->SetTempo(zone_idx, mode.speed);
                break;
            case ALIENWARE_MODE_MORPH:
                alienware->SetPeriod(zone_idx, period);
                alienware->SetColor(zone_idx,
                    mode.colors[zone_idx*2], mode.colors[zone_idx*2+1]);
                alienware->SetTempo(zone_idx, mode.speed);
                break;
            case ALIENWARE_MODE_SPECTRUM:
            case ALIENWARE_MODE_RAINBOW:
                alienware->SetPeriod(zone_idx, ALIENWARE_DURATION_SPECTRUM);
                alienware->SetTempo(zone_idx, mode.speed);
                break;
            case ALIENWARE_MODE_BREATHING:
                alienware->SetPeriod(zone_idx, period);
                alienware->SetColor(zone_idx, colors[zone.start_idx], 0x0);
                alienware->SetTempo(zone_idx, mode.speed);
                break;
            }
        }
        // Due to rate-limiting, this can take more than one second to execute
        alienware->UpdateController();

        // Re-run update if there's anything that's changed from under us...
        // This wouldn't be necessary if there were a way to lock modes and
        // active mode from the UI/other threads...
        auto new_current_mode = active_mode;
        auto new_mode = modes[current_mode];
        done = current_mode == new_current_mode && modes_eq(new_mode, mode);
        current_mode = new_current_mode;
        mode = new_mode;
    }
}
