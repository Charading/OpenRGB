/*-----------------------------------------*\
|  RGBController_DMX.cpp                    |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  DMX interface                            |
|                                           |
|  Adam Honse (CalcProgrammer1) 4/30/2023   |
\*-----------------------------------------*/

#include "RGBController_DMX.h"
#include <math.h>

#include "LogManager.h"

#define DMXGetRChannel(channels) (channels)       & 0x000001FF
#define DMXGetGChannel(channels) (channels >> 9)  & 0x000001FF
#define DMXGetBChannel(channels) (channels >> 18) & 0x000001FF

using namespace std::chrono_literals;

/**------------------------------------------------------------------*\
    @name DMX Devices
    @category LEDStrip
    @type Serial
    @save :x:
    @direct :white_check_mark:
    @effects :x:
    @detectors DetectDMXControllers
    @comment
\*-------------------------------------------------------------------*/

RGBController_DMX::RGBController_DMX(std::vector<DMXDevice> device_list)
{
    devices     = device_list;

    name        = "DMX Device Group";
    type        = DEVICE_TYPE_LEDSTRIP;
    description = "DMX Device";
    location    = "DMX: " + devices[0].port;

    /*-----------------------------------------*\
    | If this controller only represents a      |
    | single device, use the device name for the|
    | controller name                           |
    \*-----------------------------------------*/
    if(devices.size() == 1)
    {
        name    = devices[0].name;
    }

    /*-----------------------------------------*\
    | Open OpenDMX port                         |
    \*-----------------------------------------*/
    port = new serial_port(devices[0].port.c_str(), 250000, SERIAL_PORT_PARITY_NONE, SERIAL_PORT_SIZE_8, SERIAL_PORT_STOP_BITS_2, false);

    /*-----------------------------------------*\
    | Clear the RTS signal, which enables the   |
    | OpenDMX RS-485 drive enable               |
    \*-----------------------------------------*/
    port->serial_set_rts(false);

    memset(dmx_data, 0, sizeof(dmx_data));

    /*-----------------------------------------*\
    | Set up modes                              |
    \*-----------------------------------------*/
    mode Direct;
    Direct.name             = "Direct";
    Direct.value            = 0;
    Direct.flags            = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode       = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    for(unsigned char device_idx = 0; device_idx < devices.size(); device_idx++)
    {
        mode Brightness;
        Brightness.name           = devices[device_idx].name + " Brigthness";
        Brightness.value          = devices[device_idx].brightness_channel;
        Brightness.flags          = MODE_FLAG_HAS_BRIGHTNESS;
        Brightness.color_mode     = MODE_COLORS_NONE;
        Brightness.brightness_min = 0;
        Brightness.brightness_max = 255;
        Brightness.brightness     = 255;
        modes.push_back(Brightness);

        dmx_data[Brightness.value] = 255;

        for(unsigned char effect_idx = 0; effect_idx < devices[device_idx].num_effects; effect_idx++)
        {
            mode Effect;
            Effect.name           = devices[device_idx].name + " ";
            Effect.name.append     (devices[device_idx].effect_names[effect_idx]);
            Effect.value          = devices[device_idx].effect_channels[effect_idx];
            Effect.flags          = MODE_FLAG_HAS_SPEED;
            Effect.color_mode     = MODE_COLORS_NONE;
            Effect.speed_min      = 0;
            Effect.speed_max      = 255;
            Effect.speed          = 0;
            modes.push_back(Effect);
        }
    }

    keepalive_delay = 0ms;

    SetupZones();

    for (std::size_t device_idx = 0; device_idx < devices.size(); device_idx++)
    {
        /*-----------------------------------------*\
        | Update keepalive delay                    |
        \*-----------------------------------------*/
        if(devices[device_idx].keepalive_time > 0)
        {
            if(keepalive_delay.count() == 0 || keepalive_delay.count() > devices[device_idx].keepalive_time)
            {
                keepalive_delay = std::chrono::milliseconds(devices[device_idx].keepalive_time);
            }
        }
    }

    if(keepalive_delay.count() > 0)
    {
        keepalive_thread_run = 1;
        keepalive_thread = new std::thread(&RGBController_DMX::KeepaliveThreadFunction, this);
    }
    else
    {
        keepalive_thread_run = 0;
        keepalive_thread = nullptr;
    }
}

RGBController_DMX::~RGBController_DMX()
{
    if(keepalive_thread != nullptr)
    {
        keepalive_thread_run = 0;
        keepalive_thread->join();
        delete keepalive_thread;
    }

    /*---------------------------------------------------------*\
    | Delete the matrix map                                     |
    \*---------------------------------------------------------*/
    for(unsigned int zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        if(zones[zone_index].matrix_map != NULL)
        {
            if(zones[zone_index].matrix_map->map != NULL)
            {
                delete zones[zone_index].matrix_map->map;
            }

            delete zones[zone_index].matrix_map;
        }
    }
}

void RGBController_DMX::SetupZones()
{
    /*-----------------------------------------*\
    | Add Zones                                 |
    \*-----------------------------------------*/
    for(std::size_t zone_idx = 0; zone_idx < devices.size(); zone_idx++)
    {
        zone led_zone;
        led_zone.name           = devices[zone_idx].name;
        led_zone.type           = ZONE_TYPE_SINGLE;
        led_zone.leds_min       = devices[zone_idx].num_leds;
        led_zone.leds_max       = devices[zone_idx].num_leds;
        led_zone.leds_count     = devices[zone_idx].num_leds;
        led_zone.matrix_map     = NULL;

        zones.push_back(led_zone);
    }

    /*-----------------------------------------*\
    | Add LEDs                                  |
    \*-----------------------------------------*/
    for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        for(std::size_t led_idx = 0; led_idx < zones[zone_idx].leds_count; led_idx++)
        {
            led new_led;

            new_led.name   = zones[zone_idx].name + " LED ";
            new_led.name.append(std::to_string(led_idx + 1));
            new_led.value  = devices[zone_idx].red_channels[led_idx];
            new_led.value |= (devices[zone_idx].green_channels[led_idx]) << 9;
            new_led.value |= (devices[zone_idx].blue_channels[led_idx]) << 18;

            leds.push_back(new_led);
        }
    }

    SetupColors();
}

void RGBController_DMX::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_DMX::DeviceUpdateLEDs()
{
    last_update_time = std::chrono::steady_clock::now();

    for(unsigned int led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        dmx_data[DMXGetRChannel(leds[led_idx].value)] = RGBGetRValue(colors[led_idx]);
        dmx_data[DMXGetGChannel(leds[led_idx].value)] = RGBGetGValue(colors[led_idx]);
        dmx_data[DMXGetBChannel(leds[led_idx].value)] = RGBGetBValue(colors[led_idx]);
    }

    port->serial_break();
    port->serial_write((char*)&dmx_data, sizeof(dmx_data));
}

void RGBController_DMX::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_DMX::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_DMX::DeviceUpdateMode()
{
    if(modes[active_mode].flags & MODE_FLAG_HAS_SPEED)
    {
        dmx_data[modes[active_mode].value] = modes[active_mode].speed;
    }
    else if(modes[active_mode].flags & MODE_FLAG_HAS_BRIGHTNESS)
    {
        dmx_data[modes[active_mode].value] = modes[active_mode].brightness;
    }

    port->serial_break();
    port->serial_write((char*)&dmx_data, sizeof(dmx_data));
}

void RGBController_DMX::KeepaliveThreadFunction()
{
    while(keepalive_thread_run.load())
    {
        if((std::chrono::steady_clock::now() - last_update_time) > ( keepalive_delay * 0.95f ) )
        {
            UpdateLEDs();
        }
        std::this_thread::sleep_for(keepalive_delay / 2);
    }
}
