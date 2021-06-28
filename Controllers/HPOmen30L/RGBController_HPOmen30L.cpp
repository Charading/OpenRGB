/*-----------------------------------------*\
|  RGBController_HPOmen30L.cpp              |
|                                           |
|  Generic RGB Interface for HP Omen 30L    |
|                                           |
\*-----------------------------------------*/

#include "RGBController_HPOmen30L.h"

RGBController_HPOmen30L::RGBController_HPOmen30L(HPOmen30LController* omen_ptr)
{
    omen = omen_ptr;

    name        = "HP Omen 30L";
    vendor      = "Hewlet Packard";
    type        = DEVICE_TYPE_MOTHERBOARD;
    description = "Case lights for Omen 30L";
    version     = "";
    location    = omen->GetLocationString();
    serial      = "";

    mode Static;
    Static.name       = "Static";
    Static.value      = HP_OMEN_30L_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = HP_OMEN_30L_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Breathing.speed_min  = HP_OMEN_30L_SPEED_SLOW;
    Breathing.speed_max  = HP_OMEN_30L_SPEED_FAST;
    Breathing.speed      = HP_OMEN_30L_SPEED_MED;
    Breathing.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Breathing.colors_min = 1;
    Breathing.colors_max = 4;
    Breathing.colors.resize(3);
    modes.push_back(Breathing);
/*
    mode ColorCycle;
    ColorCycle.name       = "Color Cycle";
    ColorCycle.value      = HP_OMEN_30L_COLOR_CYCLE;
    ColorCycle.flags      = MODE_FLAG_HAS_SPEED;
    ColorCycle.speed_min  = HP_OMEN_30L_SPEED_SLOW;
    ColorCycle.speed_max  = HP_OMEN_30L_SPEED_FAST;
    ColorCycle.speed      = HP_OMEN_30L_SPEED_MED;
    ColorCycle.color_mode = MODE_COLORS_MODE_SPECIFIC;
    ColorCycle.colors_min = 1;
    ColorCycle.colors_max = 4;
    ColorCycle.colors.resize(3);
    modes.push_back(ColorCycle);

    mode Blinking;
    Blinking.name       = "Blinking";
    Blinking.value      = HP_OMEN_30L_BLINKING;
    Blinking.flags      = MODE_FLAG_HAS_SPEED;
    Blinking.speed_min  = HP_OMEN_30L_SPEED_SLOW;
    Blinking.speed_max  = HP_OMEN_30L_SPEED_FAST;
    Blinking.speed      = HP_OMEN_30L_SPEED_MED;
    Blinking.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Blinking.colors_min = 1;
    Blinking.colors_max = 4;
    Blinking.colors.resize(3);
    modes.push_back(Blinking);
*/
    SetupZones();
}

RGBController_HPOmen30L::~RGBController_HPOmen30L()
{
    delete omen;
}

void RGBController_HPOmen30L::SetupZones()
{
    /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    zone logo_zone;
    logo_zone.name          = "Omen Logo";
    logo_zone.type          = ZONE_TYPE_SINGLE;
    logo_zone.leds_min      = 1;
    logo_zone.leds_max      = 1;
    logo_zone.leds_count    = 1;
    logo_zone.matrix_map    = NULL;
    zones.push_back(logo_zone);

    zone light_bar;
    light_bar.name           = "Light Bar";
    light_bar.type           = ZONE_TYPE_SINGLE;
    light_bar.leds_min       = 1;
    light_bar.leds_max       = 1;
    light_bar.leds_count     = 1;
    light_bar.matrix_map     = NULL;
    zones.push_back(light_bar);

    zone ring_zone;
    ring_zone.name          = "Front Fan";
    ring_zone.type          = ZONE_TYPE_SINGLE;
    ring_zone.leds_min      = 1;
    ring_zone.leds_max      = 1;
    ring_zone.leds_count    = 1;
    ring_zone.matrix_map    = NULL;
    zones.push_back(ring_zone);

    zone cpu_zone;
    cpu_zone.name          = "CPU Cooler";
    cpu_zone.type          = ZONE_TYPE_SINGLE;
    cpu_zone.leds_min      = 1;
    cpu_zone.leds_max      = 1;
    cpu_zone.leds_count    = 1;
    cpu_zone.matrix_map    = NULL;
    zones.push_back(cpu_zone);

    /*---------------------------------------------------------*\
    | Set up LEDs                                               |
    \*---------------------------------------------------------*/
    led logo_led;
    logo_led.name = "Logo LED";
    leds.push_back(logo_led);

    led bar_led;
    bar_led.name = "Bar LED";
    leds.push_back(bar_led);

    led fan_led;
    fan_led.name = "Fan LED";
    leds.push_back(fan_led);

    led cpu_led;
    cpu_led.name = "CPU LED";
    leds.push_back(cpu_led);



    SetupColors();
}

void RGBController_HPOmen30L::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_HPOmen30L::DeviceUpdateLEDs()
{
    unsigned char red;
    unsigned char grn;
    unsigned char blu;

    if(modes[active_mode].value == HP_OMEN_30L_STATIC){
        for(int i=0; i<colors.size();i++){
            red = RGBGetRValue(colors[i]);
            grn = RGBGetGValue(colors[i]);
            blu = RGBGetBValue(colors[i]);
            omen->SetZoneStaticColor(i,red, grn, blu);
        }
    }else{
        omen->SetZonesDynamicColor(colors);
    }
}

void RGBController_HPOmen30L::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);
    if(modes[active_mode].value == HP_OMEN_30L_STATIC)
        omen->SetZoneStaticColor(zone,red, grn, blu);
}

void RGBController_HPOmen30L::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_HPOmen30L::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_HPOmen30L::DeviceUpdateMode()
{
    switch(modes[active_mode].value)
    {
        case HP_OMEN_30L_BREATHING:
            omen->SetFanMode(HP_OMEN_30L_BREATHING, modes[active_mode].speed );
            omen->SetLogoMode(HP_OMEN_30L_BREATHING, modes[active_mode].speed);
            omen->SetBarMode(HP_OMEN_30L_BREATHING, modes[active_mode].speed );
            omen->SetCPUMode(HP_OMEN_30L_BREATHING, modes[active_mode].speed );
            break;
        
        case HP_OMEN_30L_COLOR_CYCLE:
            omen->SetFanMode(HP_OMEN_30L_COLOR_CYCLE, modes[active_mode].speed);
            omen->SetLogoMode(HP_OMEN_30L_COLOR_CYCLE, modes[active_mode].speed);
            omen->SetBarMode(HP_OMEN_30L_COLOR_CYCLE, modes[active_mode].speed );
            omen->SetCPUMode(HP_OMEN_30L_COLOR_CYCLE, modes[active_mode].speed );
            break;

        default:
            omen->SetFanMode(HP_OMEN_30L_STATIC, modes[active_mode].speed);
            omen->SetLogoMode(HP_OMEN_30L_STATIC, modes[active_mode].speed);
            omen->SetBarMode(HP_OMEN_30L_STATIC, modes[active_mode].speed );
            omen->SetCPUMode(HP_OMEN_30L_STATIC, modes[active_mode].speed );
            break;
    }

    DeviceUpdateLEDs();
}
