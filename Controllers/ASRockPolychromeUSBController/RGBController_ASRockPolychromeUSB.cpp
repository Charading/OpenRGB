/*-----------------------------------------*\
|  RGBController_ASRockPolychromeUSB.cpp    |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  ASRock ASR LED and Polychrome RGB Driver |
|                                           |
|  Ed Kambulow (Dredvard) 12/26/2020        |
\*-----------------------------------------*/

#include "RGBController_ASRockPolychromeUSB.h"



#define ASROCK_USB_MAX_ZONES    7
#define ASROCK_TYPE_POLYCHROME_USB  4

#define ASROCK_ADDRESSABLE_MAX_LEDS     22



//TODO Merge this with RGBController



RGBController_PolychromeUSB::RGBController_PolychromeUSB(PolychromeUSBController* polychrome_ptr)
{
    polychrome = polychrome_ptr;

    name        = polychrome->GetDeviceName();
    vendor      = "ASRock";
    // version     = polychrome->GetFirmwareVersion();
    version = "Unknown";
    type        = DEVICE_TYPE_MOTHERBOARD;
    location    = polychrome->GetDeviceLocation();
    switch(polychrome->GetASRockType())
    {


                /*---------------------------------------------------------------------*\
                | Comment out until per zone modes are working. These are only for ARGB |
                \*---------------------------------------------------------------------*/
//                mode Spring;
//                Spring.name                 = "Spring";
//                Spring.value                = POLYCHROME_USB_MODE_SPRING;
//                Spring.flags                = MODE_FLAG_HAS_SPEED;
//                Spring.speed_min            = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Spring.speed_max            = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Spring.speed                = POLYCHROME_USB_SPEED_DEFAULT_SPRING;
//                Spring.color_mode           = MODE_COLORS_PER_LED;
//                modes.push_back(Spring);

//                mode Stack;
//                Stack.name                  = "Stack";
//                Stack.value                 = POLYCHROME_USB_MODE_STACK;
//                Stack.flags                 = MODE_FLAG_HAS_SPEED;
//                Stack.speed_min             = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Stack.speed_max             = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Stack.speed                 = POLYCHROME_USB_SPEED_DEFAULT_STACK;
//                Stack.color_mode            = MODE_COLORS_PER_LED;
//                modes.push_back(Stack);

//                mode Cram;
//                Cram.name                   = "Cram";
//                Cram.value                  = POLYCHROME_USB_MODE_CRAM;
//                Cram.flags                  = MODE_FLAG_HAS_SPEED;
//                Cram.speed_min              = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Cram.speed_max              = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Cram.speed                  = POLYCHROME_USB_SPEED_DEFAULT_CRAM;
//                Cram.color_mode             = MODE_COLORS_PER_LED;
//                modes.push_back(Cram);

//                mode Scan;
//                Scan.name                   = "Scan";
//                Scan.value                  = POLYCHROME_USB_MODE_SCAN;
//                Scan.flags                  = MODE_FLAG_HAS_SPEED;
//                Scan.speed_min              = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Scan.speed_max              = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Scan.speed                  = POLYCHROME_USB_SPEED_DEFAULT_SCAN;
//                Scan.color_mode             = MODE_COLORS_PER_LED;
//                modes.push_back(Scan);

//                mode Neon;
//                Neon.name                   = "Neon";
//                Neon.value                  = POLYCHROME_USB_MODE_NEON;
//                Neon.flags                  = MODE_FLAG_HAS_SPEED;
//                Neon.speed_min              = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Neon.speed_max              = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Neon.speed                  = POLYCHROME_USB_SPEED_DEFAULT_NEON;
//                Neon.color_mode             = MODE_COLORS_PER_LED;
//                modes.push_back(Neon);

//                mode Water;
//                Water.name                  = "Water";
//                Water.value                 = POLYCHROME_USB_MODE_WATER;
//                Water.flags                 = MODE_FLAG_HAS_SPEED;
//                Water.speed_min             = POLYCHROME_USB_SPEED_MIN_ARGB;
//                Water.speed_max             = POLYCHROME_USB_SPEED_MAX_ARGB;
//                Water.speed                 = POLYCHROME_USB_SPEED_DEFAULT_WATER;
//                Water.color_mode            = MODE_COLORS_PER_LED;
//                modes.push_back(Water);

//                mode Rainbow;
//                Rainbow.name                = "Rainbow";
//                Rainbow.value               = POLYCHROME_USB_MODE_RAINBOW;
//                Rainbow.flags               = MODE_FLAG_HAS_SPEED;
//                Rainbow.speed_min           = POLYCHROME_USB_SPEED_MIN_RAINBOW;
//                Rainbow.speed_max           = POLYCHROME_USB_SPEED_MAX_RAINBOW;
//                Rainbow.speed               = POLYCHROME_USB_SPEED_DEFAULT_RAINBOW;
//                Rainbow.color_mode          = MODE_COLORS_NONE;
//                modes.push_back(Rainbow);
 

        case ASROCK_TYPE_POLYCHROME_USB:
            {
                description = "ASRock Polychrome USB Device";

                /*  Doesn't appear to have a direct mode  
                Note:   No changes are saved after powering off computer so may be direct
                        PolychromeRGB (ASRock software) starts session with an A4 command which might enter programming mode
                        Nuvatron then respond with A6 prefix
                mode Direct;
                Direct.name       = "Direct";
                Direct.value      = AURA_MODE_DIRECT;
                Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
                Direct.color_mode = MODE_COLORS_PER_LED;
                modes.push_back(Direct);
                */

                mode Static;
                Static.name       = "Static";
                Static.value      = POLYCHROME_USB_MODE_STATIC;
                Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
                Static.color_mode = MODE_COLORS_PER_LED;
                modes.push_back(Static);

                mode Breathing;
                Breathing.name       = "Breathing";
                Breathing.value      = POLYCHROME_USB_MODE_BREATHING;
                Breathing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
                Breathing.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Breathing.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Breathing.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Breathing.color_mode = MODE_COLORS_PER_LED;
                modes.push_back(Breathing);

                mode Strobe;
                Strobe.name       = "Strobe";
                Strobe.value      = POLYCHROME_USB_MODE_STROBE;
                Strobe.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR;
                Strobe.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Strobe.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Strobe.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Strobe.color_mode = MODE_COLORS_PER_LED;
                modes.push_back(Strobe);

                mode SpectrumCycle;
                SpectrumCycle.name       = "Spectrum Cycle";
                SpectrumCycle.value      = POLYCHROME_USB_MODE_SPECTRUM_CYCLE;
                SpectrumCycle.flags      = MODE_FLAG_HAS_SPEED;
                SpectrumCycle.speed_min  = POLYCHROME_USB_SPEED_MIN;
                SpectrumCycle.speed_max  = POLYCHROME_USB_SPEED_MAX;
                SpectrumCycle.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                SpectrumCycle.color_mode = MODE_COLORS_NONE;
                modes.push_back(SpectrumCycle);

                mode Random;
                Random.name       = "Random";
                Random.value      = POLYCHROME_USB_MODE_RANDOM;
                Random.flags      = MODE_FLAG_HAS_SPEED;
                Random.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Random.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Random.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Random.color_mode = MODE_COLORS_NONE;
                modes.push_back(Random);

                mode Music;
                Random.name       = "Music";
                Random.value      = POLYCHROME_USB_MODE_RANDOM;
                Random.flags      = MODE_FLAG_HAS_BRIGHTNESS;
                Random.color_mode = MODE_COLORS_NONE;
                modes.push_back(Random);

                mode Wave;
                Wave.name       = "Wave";
                Wave.value      = POLYCHROME_USB_MODE_WAVE;
                Wave.flags      = MODE_FLAG_HAS_SPEED;
                Wave.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Wave.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Wave.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Wave.color_mode = MODE_COLORS_NONE;
                modes.push_back(Wave);

                mode Spring;
                Spring.name       = "Spring";
                Spring.value      = POLYCHROME_USB_MODE_SPRING;
                Spring.flags      = MODE_FLAG_HAS_SPEED;
                Spring.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Spring.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Spring.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Spring.color_mode = MODE_COLORS_NONE;
                modes.push_back(Spring);

                mode Stack;
                Stack.name       = "Stack";
                Stack.value      = POLYCHROME_USB_MODE_STACK;
                Stack.flags      = MODE_FLAG_HAS_SPEED;
                Stack.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Stack.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Stack.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Stack.color_mode = MODE_COLORS_NONE;
                modes.push_back(Stack);

                mode Cram;
                Cram.name       = "Cram";
                Cram.value      = POLYCHROME_USB_MODE_CRAM;
                Cram.flags      = MODE_FLAG_HAS_SPEED;
                Cram.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Cram.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Cram.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Cram.color_mode = MODE_COLORS_NONE;
                modes.push_back(Cram);

                mode Scan;
                Scan.name       = "Scan";
                Scan.value      = POLYCHROME_USB_MODE_SCAN;
                Scan.flags      = MODE_FLAG_HAS_SPEED;
                Scan.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Scan.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Scan.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Scan.color_mode = MODE_COLORS_NONE;
                modes.push_back(Scan);

                mode Neon;
                Neon.name       = "Neon";
                Neon.value      = POLYCHROME_USB_MODE_NEON;
                Neon.flags      = 0;
                Neon.color_mode = MODE_COLORS_NONE;
                modes.push_back(Neon);

                mode Water;
                Water.name       = "Water";
                Water.value      = POLYCHROME_USB_MODE_WATER;
                Water.flags      = MODE_FLAG_HAS_SPEED;
                Water.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Water.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Water.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Water.color_mode = MODE_COLORS_NONE;
                modes.push_back(Water);

                mode Rainbow;
                Rainbow.name       = "Rainbow";
                Rainbow.value      = POLYCHROME_USB_MODE_RAINBOW;
                Rainbow.flags      = MODE_FLAG_HAS_SPEED;
                Rainbow.speed_min  = POLYCHROME_USB_SPEED_MIN;
                Rainbow.speed_max  = POLYCHROME_USB_SPEED_MAX;
                Rainbow.speed      = POLYCHROME_USB_SPEED_DEFAULT;
                Rainbow.color_mode = MODE_COLORS_NONE;
                modes.push_back(Rainbow);

                mode Test;
                Test.name       = "Test";
                Test.value      = POLYCHROME_USB_MODE_TEST;
                Test.flags      = 0;
                Test.color_mode = MODE_COLORS_NONE;
                modes.push_back(Test);
            }
            break;
    }

     SetupZones();

     // Initialize active mode
     unsigned char zone=5;
     active_mode = GetDeviceMode(zone);

}



//TODO Ensure led in  count correct
void RGBController_PolychromeUSB::SetupZones()
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
    zones.resize(polychrome->GetChannelCount());

    /*-------------------------------------------------*\
    | Set zones and leds                                |
    \*-------------------------------------------------*/

    int addressableCounter = 1;
    for (unsigned int channel_idx = 0; channel_idx < zones.size(); channel_idx++)
    {
        PolychromeDeviceInfo device_info = polychrome->GetPolychromeDevices()[channel_idx];

        zones[channel_idx].type     = ZONE_TYPE_LINEAR;
        //TODO
        if (device_info.device_type== PolychromeDeviceType::ADDRESSABLE)
        {
            zones[channel_idx].name       = polychrome_USB_zone_names[channel_idx];
            zones[channel_idx].leds_min   = 0;
            zones[channel_idx].leds_max   = ASROCK_ADDRESSABLE_MAX_LEDS;
            // TODO Correct Addresable
            zones[channel_idx].leds_min   = device_info.num_leds;
            zones[channel_idx].leds_max   = device_info.num_leds;
            zones[channel_idx].leds_count = device_info.num_leds; 
        }
        else if (device_info.device_type==PolychromeDeviceType::FIXED) 
        {
            zones[channel_idx].name       = polychrome_USB_zone_names[channel_idx];
            zones[channel_idx].leds_min   = device_info.num_leds;
            zones[channel_idx].leds_max   = device_info.num_leds;
            zones[channel_idx].leds_count = device_info.num_leds; 
        }       

        addressableCounter++;
        if(first_run)
        {
            //zones[channel_idx].leds_count = 0;
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

        /*---------------------------------------------------------*\
    | Initialize colors for each LED                            |
    \*---------------------------------------------------------*/
    
    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        unsigned char  led = (unsigned char) leds[led_idx].value;

        //unsigned char red = aura->GetLEDRed(led);
        colors[led_idx]=polychrome->GetZoneConfig(led).color;     // TODO Get addressable instead of zone idx
    }
    /*---------------------------------------------------------*\
    | Initialize zone info to track zone, speed, mode           |
    |  B550 Boards have modes, speed for each zone              
    \*---------------------------------------------------------*/
     for (unsigned int channel_idx = 0; channel_idx < zones.size(); channel_idx++)
    {
        PolychromeZoneInfo zoneinfo;
        zoneinfo = polychrome->GetZoneConfig(channel_idx); 
        zones_info.push_back(zoneinfo);
    }

}

void RGBController_PolychromeUSB::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

// TODO Review adn Correct LED Updates - Currently can't do any ARGB Control
void RGBController_PolychromeUSB::DeviceUpdateLEDs()
{

    for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        if (PDEBUG) printf("DULED");
        unsigned char set_mode=zones_info[zone_idx].mode;
        if (set_mode>POLYCHROME_USB_NUM_MODES){
            //Setting to invalid mode will cause Nuvatron and MB to freeze until hard power down
            printf("Error: Mode %d !=%d attempt\n",set_mode,active_mode);
            set_mode=active_mode % POLYCHROME_USB_NUM_MODES;
        }
        polychrome->SetChannelLEDs(zone_idx, set_mode, zones_info[zone_idx].speed,zones[zone_idx].colors, zones[zone_idx].leds_count);
        
    }
}

void RGBController_PolychromeUSB::UpdateZoneLEDs(int zone)
{

    //polychrome->SetChannelLEDs(zone_uc, zones[zone].colors, zones[zone].leds_count);
    if (PDEBUG) printf("UZLED");
    unsigned char set_mode=zones_info[zone].mode;
    if (set_mode>POLYCHROME_USB_NUM_MODES){
            //Setting to invalid mode will cause Nuvatron and MB to freeze until hard power down
            printf("Error: Mode %d !- %d attempt\n",set_mode,active_mode);
            set_mode=active_mode % POLYCHROME_USB_NUM_MODES;
    }
    polychrome->SetChannelLEDs(zone, set_mode, zones_info[zone].speed,zones[zone].colors, zones[zone].leds_count);

}

void RGBController_PolychromeUSB::UpdateSingleLED(int led)
{

    unsigned int channel = leds[led].value;
    if (PDEBUG) printf("USLED");
    unsigned char set_mode=zones_info[channel].mode;
    if (set_mode>POLYCHROME_USB_NUM_MODES){
            //Setting to invalid mode will cause Nuvatron and MB to freeze until hard power down
            printf("Error: Mode %d !- %d attempt\n",set_mode,active_mode);
            set_mode=active_mode % POLYCHROME_USB_NUM_MODES;
    }
    polychrome->SetChannelLEDs(channel, set_mode, modes[channel].speed,zones[channel].colors, zones[channel].leds_count);
    
}


unsigned char RGBController_PolychromeUSB::GetDeviceMode(unsigned char zone){

    int  dev_mode;
    int color_mode = MODE_COLORS_PER_LED;

    dev_mode=polychrome->GetZoneConfig(zone).mode;
    //Note: polychorom color_mode is only NONE or PER_LED (no random currently)
    active_mode= dev_mode;
    return (active_mode);
  
}


void RGBController_PolychromeUSB::SetCustomMode()
{
    //TODO Verify if this Mode does permanently write (if power off does it return to this state)
    active_mode = POLYCHROME_USB_MODE_STATIC ;
}


void RGBController_PolychromeUSB::DeviceUpdateMode()
{
    unsigned char r,g,b;

    RGBColor rgb= ToRGBColor(0, 0, 0) ;
    if(modes[active_mode].color_mode == MODE_COLORS_MODE_SPECIFIC)
    {
        rgb=modes[active_mode].colors[0];
     }
     r=RGBGetRValue(rgb);
     b=RGBGetBValue(rgb);
     g=RGBGetGValue(rgb);
    if(PDEBUG){
        printf("DuM");
    }
    unsigned char tmode=modes[active_mode].value;
    if (tmode== POLYCHROME_USB_MODE_TEST)
    {
        printf("TestCommand: ");
        polychrome->TestCommand();
        return;
    }


    for(unsigned int zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    
    {
        if(zones[zone_idx].leds_count > 0)
        {

            unsigned char set_mode=(unsigned char) modes[active_mode].value;
            zones_info[zone_idx].mode=(unsigned char) modes[active_mode].value;
            zones_info[zone_idx].speed=(unsigned char) modes[active_mode].speed;
            
            if (set_mode>=POLYCHROME_USB_NUM_MODES){
                    //Setting to invalid mode will cause Nuvatron and MB to freeze until hard power down
                    printf("Error: Mode %d !- %d attempt\n",set_mode,active_mode);
                    set_mode=active_mode % POLYCHROME_USB_NUM_MODES;
                    
            }
            polychrome->SetChannelLEDs(zone_idx, set_mode, modes[zone_idx].speed,zones[zone_idx].colors, zones[zone_idx].leds_count);

        }
    }

}
