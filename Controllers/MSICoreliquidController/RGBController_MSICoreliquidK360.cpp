/*-----------------------------------------*\
|  RGBController_MSICoreliquidK360.cpp      |
|                                           |
|  RGB Interface for Coreliquid device      |
|  Class                                    |
|                                           |
|  Aapo Kossi     25/1/2023                 |
\*-----------------------------------------*/

#include "RGBController_MSICoreliquidK360.h"

/*-----------------------------------------*\
    @name MSI Coreliquid
    @category cooler
    @type USB
    @save :x:
    @direct :white_check_mark:
    @effects :white_check_mark:
    @detectors MSICoreliquidControllerDetect
    @comment
\*-----------------------------------------*/

/*-----------------------------------------*\
| shorthand for common mode flags           |
\*-----------------------------------------*/
#define FLAGS_MSI_NOCOLOR                                      MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS
#define FLAGS_MSI_COLOR          MODE_FLAG_HAS_RANDOM_COLOR  | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR
#define FLAGS_MSI_RANDOM         MODE_FLAG_HAS_RANDOM_COLOR  | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS

RGBController_MSICoreliquidK360::RGBController_MSICoreliquidK360(MSICoreliquidController* controller_ptr)
{
    controller = controller_ptr;
    color_enabled = 0x00;

    vendor      = "MSI";
    description = "MSI Coreliquid CPU cooler";
    version     = controller->GetFirmwareVersion();
    type        = DEVICE_TYPE_COOLER;
    location    = controller->GetDeviceLocation();

    /*---------------------------------------------------------*\
    | mode values are bytes that the device firmware recognises |
    \*---------------------------------------------------------*/ 
    SetupMode("Off",                     0x00, FLAGS_MSI_NOCOLOR);
    SetupMode("Direct",                  0x01, FLAGS_MSI_COLOR);
    SetupMode("Breathing",               0x02, FLAGS_MSI_COLOR);
    SetupMode("Flashing",                0x03, FLAGS_MSI_COLOR);
    SetupMode("Double Flashing",         0x04, FLAGS_MSI_COLOR);
    SetupMode("Lightning",               0x05, FLAGS_MSI_COLOR);
    SetupMode("Wipe",                    0x06, FLAGS_MSI_COLOR);
    SetupMode("Meteor",                  0x07, FLAGS_MSI_COLOR);
    SetupMode("Spin",                    0x08, FLAGS_MSI_COLOR);
    SetupMode("Planetary",               0x10, FLAGS_MSI_RANDOM);
    SetupMode("Double Meteor",           0x11, FLAGS_MSI_RANDOM);
    SetupMode("Energy",                  0x12, FLAGS_MSI_COLOR);
    SetupMode("Blink",                   0x13, FLAGS_MSI_COLOR);
    SetupMode("Clock",                   0x14, FLAGS_MSI_COLOR);
    SetupMode("Color Pulse",             0x15, FLAGS_MSI_RANDOM);
    SetupMode("Color Shift",             0x16, FLAGS_MSI_RANDOM);
    SetupMode("Color Wave",              0x17, FLAGS_MSI_RANDOM);
    SetupMode("Marquee",                 0x18, FLAGS_MSI_COLOR);
    SetupMode("Mystery",                 0x19, FLAGS_MSI_COLOR);
    SetupMode("Rainbow",                 0x1A, FLAGS_MSI_RANDOM);
    SetupMode("Visor",                   0x1B, FLAGS_MSI_COLOR);
    SetupMode("Rainbow Flashing",        0x1D, FLAGS_MSI_RANDOM);
    SetupMode("Rainbow flashing 2",      0x1E, FLAGS_MSI_RANDOM);
    SetupMode("Random",                  0x1F, FLAGS_MSI_COLOR);
    SetupMode("Double Flashing Rainbow", 0x23, FLAGS_MSI_RANDOM);
    SetupMode("Stack",                   0x24, FLAGS_MSI_COLOR);

    SetupZones();
}

RGBController_MSICoreliquidK360::~RGBController_MSICoreliquidK360()
{
    delete controller;
}

/*--------------------------------------------------------*\
| Helper function to avoid repetition of brightness, speed | 
\*--------------------------------------------------------*/
void RGBController_MSICoreliquidK360::SetupMode
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
    unsigned char speed;

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
    }
    speed = speed_max;

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
    modes.push_back(new_mode);
}

/*--------------------------------------------*\
|                                              |
| The two leds are not physical, but instead   |
| they represent the primary color and a       |
| secondary color that only some modes support |
|                                              |
\*--------------------------------------------*/
void RGBController_MSICoreliquidK360::SetupZones()
{
    zone led_zone;

    led_zone.name       = "Radiator fan LEDs";
    led_zone.type       = ZONE_TYPE_LINEAR;
    led_zone.leds_min   = 2;
    led_zone.leds_max   = 2;
    led_zone.leds_count = 2;
    led_zone.matrix_map = NULL;
    zones.push_back(led_zone);

    led primary;
    primary.name = "Primary color";
    leds.push_back(primary);

    led secondary;
    secondary.name = "Secondary color";
    leds.push_back(secondary);

    SetupColors();
}

void RGBController_MSICoreliquidK360::ResizeZone(int, int){}

void RGBController_MSICoreliquidK360::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_MSICoreliquidK360::UpdateZoneLEDs(int)
{
    DeviceUpdateMode();
}

void RGBController_MSICoreliquidK360::UpdateSingleLED(int)
{
    DeviceUpdateMode();
}

/*--------------------------------------------*\
| Send configuration to device                 | 
\*--------------------------------------------*/
void RGBController_MSICoreliquidK360::DeviceUpdateMode()
{
    if(modes[active_mode].color_mode == MODE_COLORS_PER_LED)
    {
        color_enabled = 0x80;
    }
    else
    {
        color_enabled = 0x00;
    }

    controller->SendColors
    (
        colors[0],
        colors[1],
        modes[active_mode].value,
        modes[active_mode].speed,
        modes[active_mode].brightness << 3,
        color_enabled
    );
}
