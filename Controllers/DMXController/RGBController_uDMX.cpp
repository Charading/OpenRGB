#include "RGBController_uDMX.h"
#include "SettingsManager.h"
#include "ResourceManager.h"

#define DMXGetRChannel(channels) (channels)       & 0x000001FF
#define DMXGetGChannel(channels) (channels >> 9)  & 0x000001FF
#define DMXGetBChannel(channels) (channels >> 18) & 0x000001FF

RGBController_uDMX::RGBController_uDMX(uDMXController* controller_ptr)
{
    controller     = controller_ptr;

    name        = controller->GetDeviceName();
    vendor      = "Van Ooijen Technische Informatica";
    description = "uDMX Device";
    type        = DEVICE_TYPE_ACCESSORY;

    /*---------------*\
    | Zero out buffer |
    \*---------------*/

    memset(controller->DMX_Buffer, 0x00, sizeof(controller->DMX_Buffer));

    SetupZones();
}

RGBController_uDMX::~RGBController_uDMX()
{
    delete controller;
}

void RGBController_uDMX::SetupZones()
{
    json dmx_settings;

    /*---------------------------------------*\
    | Get uDMX settings from settings manager |
    \*---------------------------------------*/

    dmx_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("DMXDevices");

    modes.clear();
    leds.clear();
    colors.clear();
    zones.clear();
    colors.clear();

    /*-----------------------------------------------*\
    | If the uDMX settings contains devices, process  |
    \*-----------------------------------------------*/

    if(dmx_settings.contains("udmx"))
    {
        std::string                           name;
        unsigned char                  num_effects;
        unsigned short          brightness_channel;

        /*-----------------*\
        | Setup Direct Mode |
        \*-----------------*/

        mode Direct;
        Direct.name       = "Direct";
        Direct.value      = 0xFFFF;
        Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
        Direct.color_mode = MODE_COLORS_PER_LED;
        modes.push_back(Direct);

        /*--------------------*\
        | Loop through devices |
        \*--------------------*/
        for(unsigned int device_idx = 0; device_idx < dmx_settings["udmx"].size(); device_idx++)
        {
            if(dmx_settings["udmx"][device_idx].contains("name"))
            {
                name = dmx_settings["udmx"][device_idx]["name"];
            }

            /*------------------------*\
            | Setup brightness channel |
            \*------------------------*/

            if(dmx_settings["udmx"][device_idx].contains("brightness_channel"))
            {
                brightness_channel = dmx_settings["udmx"][device_idx]["brightness_channel"];

                mode Brightness;
                Brightness.name           = name + " Brigthness";
                Brightness.value          = brightness_channel - 1;
                Brightness.flags          = MODE_FLAG_HAS_BRIGHTNESS;
                Brightness.color_mode     = MODE_COLORS_NONE;
                Brightness.brightness_min = 0;
                Brightness.brightness_max = 255;
                Brightness.brightness     = 255;
                modes.push_back(Brightness);

                /*-------------------------- -*\
                | Set default brightness (255) |
                \*----------------------------*/

                controller->DMX_Buffer[Brightness.value] = 255;
            }

            /*------------------*\
            | Setup effect modes |
            \*------------------*/

            num_effects = dmx_settings["udmx"][device_idx]["effects"].size();

            for(unsigned char effect_idx = 0; effect_idx < num_effects; effect_idx++)
            {
                std::string  effect_name;
                unsigned int effect_channel;

                if(dmx_settings["udmx"][device_idx]["effects"][effect_idx].contains("name"))
                {
                    effect_name = dmx_settings["udmx"][device_idx]["effects"][effect_idx]["name"];
                }

                if(dmx_settings["udmx"][device_idx]["effects"][effect_idx].contains("channel"))
                {
                    effect_channel = dmx_settings["udmx"][device_idx]["effects"][effect_idx]["channel"];
                }

                mode Effect;
                Effect.name           = name + " " + effect_name;
                Effect.value          = effect_channel - 1;
                Effect.flags          = MODE_FLAG_HAS_SPEED;
                Effect.color_mode     = MODE_COLORS_NONE;
                Effect.speed_min      = 0;
                Effect.speed_max      = 255;
                Effect.speed          = 0;
                modes.push_back(Effect);
            }

            /*-------------------------------------*\
            | Setup zones (Each device is one zone) |
            \*-------------------------------------*/

            zones.resize(dmx_settings["udmx"].size());

            zones[device_idx].name       = name;
            zones[device_idx].leds_min   = dmx_settings["udmx"][device_idx]["leds"].size();
            zones[device_idx].leds_max   = dmx_settings["udmx"][device_idx]["leds"].size();
            zones[device_idx].leds_count = dmx_settings["udmx"][device_idx]["leds"].size();
            zones[device_idx].matrix_map = NULL;

            if(zones[device_idx].leds_count < 1)
            {
                zones[device_idx].type = ZONE_TYPE_SINGLE;
            }
            else
            {
                zones[device_idx].type = ZONE_TYPE_LINEAR;
            }

            /*----------*\
            | Setup leds |
            \*----------*/

            for(unsigned char led_idx = 0; led_idx < zones[device_idx].leds_count; led_idx++)
            {
                unsigned int red = 0;
                unsigned int green = 0;
                unsigned int blue = 0;

                if(dmx_settings["udmx"][device_idx]["leds"][led_idx].contains("red_channel"))
                {
                    red = dmx_settings["udmx"][device_idx]["leds"][led_idx]["red_channel"];
                }

                if(dmx_settings["udmx"][device_idx]["leds"][led_idx].contains("green_channel"))
                {
                    green = dmx_settings["udmx"][device_idx]["leds"][led_idx]["green_channel"];
                }

                if(dmx_settings["udmx"][device_idx]["leds"][led_idx].contains("blue_channel"))
                {
                    blue = dmx_settings["udmx"][device_idx]["leds"][led_idx]["blue_channel"];
                }

                led new_led;
                new_led.name   = name;
                new_led.value  = red - 1;
                new_led.value |= (green - 1) << 9;
                new_led.value |= (blue - 1) << 18;

                if(zones[device_idx].leds_count > 1)
                {
                    new_led.name.append(", LED " + std::to_string(led_idx + 1));
                }

                leds.push_back(new_led);
            }
        }
    }
    else
    {
        mode Default;
        Default.name  = "Please configure your DXM device";
        Default.value = 0x00;
        modes.push_back(Default);
    }

    SetupColors();
}

void RGBController_uDMX::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*-------------------------------------------*\
    | This device does not support resizing zones |
    \*-------------------------------------------*/
}

void RGBController_uDMX::DeviceUpdateLEDs()
{
    for(unsigned int led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        controller->DMX_Buffer[DMXGetRChannel(leds[led_idx].value)] = RGBGetRValue(colors[led_idx]);
        controller->DMX_Buffer[DMXGetGChannel(leds[led_idx].value)] = RGBGetGValue(colors[led_idx]);
        controller->DMX_Buffer[DMXGetBChannel(leds[led_idx].value)] = RGBGetBValue(colors[led_idx]);
    }

    controller->SendPacket();
}

void RGBController_uDMX::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_uDMX::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_uDMX::DeviceUpdateMode()
{
    if(modes[active_mode].flags & MODE_FLAG_HAS_SPEED)
    {
        controller->DMX_Buffer[modes[active_mode].value] = modes[active_mode].speed;
    }
    else if(modes[active_mode].flags & MODE_FLAG_HAS_BRIGHTNESS)
    {
        controller->DMX_Buffer[modes[active_mode].value] = modes[active_mode].brightness;
    }

    controller->SendPacket();
}
