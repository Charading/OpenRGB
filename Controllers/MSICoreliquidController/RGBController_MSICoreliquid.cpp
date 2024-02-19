/*-----------------------------------------*\
|  RGBController_MSICoreliquid.cpp          |
|                                           |
|  RGB Interface for Coreliquid device      |
|  Class                                    |
|                                           |
|  Aapo Kossi     25/1/2023                 |
\*-----------------------------------------*/

#include "RGBController_MSICoreliquid.h"

/**----------------------------------------*\
    @name MSI Coreliquid
    @category cooler
    @type USB
    @save :x:
    @direct :white_check_mark:
    @effects :white_check_mark:
    @detectors DetectCoreliquidK360V2 
    @comment
\*-----------------------------------------*/

/*-----------------------------------------*\
| shorthand for common mode flags           |
\*-----------------------------------------*/
#define FLAGS_MSI_NOCOLOR           MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS
#define FLAGS_MSI_COLOR             MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR
#define FLAGS_MSI_COLOR_RANDOM      FLAGS_MSI_COLOR | MODE_FLAG_HAS_RANDOM_COLOR
#define FLAGS_MSI_SPECIFIC          MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR

RGBController_MSICoreliquid::RGBController_MSICoreliquid(MSICoreliquidController* controller_ptr)
{
    controller = controller_ptr;

    vendor      = "MSI";
    description = "MSI Coreliquid CPU cooler";
    version     = controller->GetFirmwareVersion();
    type        = DEVICE_TYPE_COOLER;
    location    = controller->GetDeviceLocation();

    /*---------------------------------------------------------*\
    | mode values are bytes that the device firmware recognises |
    \*---------------------------------------------------------*/ 
    SetupMode("Off",                     0x00, 0);
    SetupMode("Direct",                  0x01, MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR);
    SetupMode("Breathing",               0x02, FLAGS_MSI_COLOR);
    SetupMode("Flashing",                0x03, FLAGS_MSI_COLOR_RANDOM);
    SetupMode("Double Flashing",         0x04, FLAGS_MSI_COLOR_RANDOM);
    SetupMode("Lightning",               0x05, FLAGS_MSI_COLOR);
    SetupMode("Wipe",                    0x06, FLAGS_MSI_COLOR_RANDOM);
    SetupMode("Meteor",                  0x07, FLAGS_MSI_COLOR_RANDOM);
    SetupMode("Spin",                    0x08, FLAGS_MSI_COLOR_RANDOM);
    SetupMode("Planetary",               0x10, FLAGS_MSI_NOCOLOR);
    SetupMode("Double Meteor",           0x11, FLAGS_MSI_NOCOLOR);
    SetupMode("Energy",                  0x12, FLAGS_MSI_COLOR);
    SetupMode("Clock",                   0x14, FLAGS_MSI_SPECIFIC);
    SetupMode("Color Pulse",             0x15, FLAGS_MSI_NOCOLOR);
    SetupMode("Color Shift",             0x16, FLAGS_MSI_NOCOLOR);
    SetupMode("Color Wave",              0x17, FLAGS_MSI_NOCOLOR);
    SetupMode("Marquee",                 0x18, FLAGS_MSI_COLOR);
    SetupMode("Mystery",                 0x19, FLAGS_MSI_NOCOLOR);
    SetupMode("Rainbow",                 0x1A, FLAGS_MSI_NOCOLOR);
    SetupMode("Visor",                   0x1B, FLAGS_MSI_SPECIFIC);
    SetupMode("Rainbow Flashing",        0x1D, FLAGS_MSI_NOCOLOR);
    SetupMode("Rainbow Flashing 2",      0x1E, FLAGS_MSI_NOCOLOR);
    SetupMode("Random",                  0x1F, FLAGS_MSI_SPECIFIC);
    SetupMode("Double Flashing Rainbow", 0x23, FLAGS_MSI_NOCOLOR);
    SetupMode("Stack",                   0x24, FLAGS_MSI_COLOR_RANDOM);

    SetupZones();
}

RGBController_MSICoreliquid::~RGBController_MSICoreliquid()
{
    delete controller;
}

/*--------------------------------------------------------*\
| Helper function to avoid repetition of brightness, speed | 
\*--------------------------------------------------------*/
void RGBController_MSICoreliquid::SetupMode
    (
    std::string name,
    int value,
    int flags
    )
{
    int color_mode;
    unsigned char brightness_min = 0x01;
    unsigned char brightness_max = 0x01;
    unsigned char brightness;
    unsigned char speed_min      = 0x00;
    unsigned char speed_max      = 0x00;
    unsigned char speed          = 0x00;

    if (flags & MODE_FLAG_HAS_PER_LED_COLOR)
    {
        color_mode = MODE_COLORS_PER_LED;
    }
    else if (flags & MODE_FLAG_HAS_RANDOM_COLOR)
    {
        color_mode = MODE_COLORS_RANDOM;
    }
    else
    {
        color_mode = MODE_COLORS_NONE;
    }

    if (flags & MODE_FLAG_HAS_BRIGHTNESS)
    {
        brightness_max = 0x05;
    }
    brightness = brightness_max;

    if (flags & MODE_FLAG_HAS_SPEED)
    {
        speed_max = 0x02;
        speed     = 0x01;
    }

    mode new_mode;
    new_mode.name           = name;
    new_mode.value          = value;
    new_mode.flags          = flags;
    new_mode.color_mode     = color_mode;
    new_mode.brightness_min = brightness_min;
    new_mode.brightness_max = brightness_max;
    new_mode.brightness     = brightness;
    new_mode.speed_min      = speed_min;
    new_mode.speed_max      = speed_max;
    new_mode.speed          = speed;
    if (flags == FLAGS_MSI_SPECIFIC)
    {
        new_mode.colors_min     = 2;
        new_mode.colors_max     = 2;
        new_mode.colors.resize(new_mode.colors_min);
    }
    modes.push_back(new_mode);
}

void RGBController_MSICoreliquid::SetupZones()
{
    zone led_zone;

    led_zone.name       = "Radiator fan LEDs";
    led_zone.type       = ZONE_TYPE_LINEAR;
    led_zone.leds_min   = 1;
    led_zone.leds_max   = 1;
    led_zone.leds_count = 1;
    led_zone.matrix_map = NULL;
    zones.push_back(led_zone);

    led new_led;
    new_led.name = "Radiator LED";
    leds.push_back(new_led);

    SetupColors();
}

void RGBController_MSICoreliquid::ResizeZone(int, int){}

void RGBController_MSICoreliquid::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_MSICoreliquid::UpdateZoneLEDs(int)
{
    DeviceUpdateMode();
}

void RGBController_MSICoreliquid::UpdateSingleLED(int)
{
    DeviceUpdateMode();
}

/*--------------------------------------------*\
| Send configuration to device                 | 
\*--------------------------------------------*/
void RGBController_MSICoreliquid::DeviceUpdateMode()
{
    unsigned char color_enabled;
    RGBColor primary_color = colors[0];
    RGBColor secondary_color;
    if (modes[active_mode].color_mode == MODE_COLORS_PER_LED)
    {
        color_enabled = 0x80;
        primary_color = colors[0];
    }
    else if (modes[active_mode].color_mode == MODE_COLORS_MODE_SPECIFIC)
    {
        color_enabled = 0x80;
        primary_color = modes[active_mode].colors[0];
        secondary_color = modes[active_mode].colors[1];
    }
    else
    {
        color_enabled = 0x00;
    }

    controller->SendColors
    (
        primary_color,
        secondary_color,
        modes[active_mode].value,
        modes[active_mode].speed,
        modes[active_mode].brightness << 3,
        color_enabled
    );
}
