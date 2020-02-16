/************************************************\
*            RGBController_AuraGPU.cpp           *
*   Generic RGB Interface for OpenRGB Aura GPU   *
*                                                *
*        Jan Rettig (Klapstuhl) 14.02.2020       *
\************************************************/


#include "RGBController_AuraGPU.h"

int RGBController_AuraGPU::GetDeviceMode()
{
    int dev_mode = aura_gpu->AuraGPURegisterRead(AURA_GPU_REG_MODE);
    int color_mode = MODE_COLORS_PER_LED;

    if(dev_mode = AURA_GPU_MODE_STATIC)
    {
        if (aura_gpu->direct) dev_mode = AURA_GPU_MODE_DIRECT;
        else if (aura_gpu->AuraGPURegisterRead(AURA_GPU_REG_ENABLE)) dev_mode = AURA_GPU_MODE_OFF;
    }

    switch(dev_mode)
    {
    case AURA_GPU_MODE_OFF:
    case AURA_GPU_MODE_SPECTRUM_CYCLE:
        color_mode = MODE_COLORS_NONE;
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

void RGBController_AuraGPU::UpdateLEDs()
{
    for(std::size_t led = 0; led < colors.size(); led++)
    {
        unsigned char red = RGBGetRValue(colors[led]);
        unsigned char grn = RGBGetGValue(colors[led]);
        unsigned char blu = RGBGetBValue(colors[led]);

        if (GetMode() == 0)
        {
            aura_gpu->SetLEDColorsDirect(red, grn, blu);
        }
        else 
        {
            aura_gpu->SetLEDColorsEffect(red, grn, blu);
        }
    }
}

void RGBController_AuraGPU::UpdateZoneLEDs(int zone)
{
    UpdateLEDs();
}

void RGBController_AuraGPU::UpdateSingleLED(int led)
{
    UpdateLEDs();
}

RGBController_AuraGPU::RGBController_AuraGPU(AuraGPUController * aura_gpu_ptr)
{
    aura_gpu = aura_gpu_ptr;
    

    name = aura_gpu->GetDeviceName();
    version = "0.00.1";
    location = aura_gpu->GetDeviceLocation();
    type = DEVICE_TYPE_GPU;

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = AURA_GPU_MODE_DIRECT;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Off;
    Off.name       = "Off";
    Off.value      = AURA_GPU_MODE_OFF;
    Off.flags      = 0;
    Off.color_mode = MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name       = "Static";
    Static.value      = AURA_GPU_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Breathing;  
    Breathing.name       = "Breathing";
    Breathing.value      = AURA_GPU_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Breathing);

    mode Flashing;
    Flashing.name       = "Flashing";
    Flashing.value      = AURA_GPU_MODE_FLASHING;
    Flashing.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Flashing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Flashing);

    mode Spectrum_Cycle;
    Spectrum_Cycle.name       = "Spectrum Cycle";
    Spectrum_Cycle.value      = AURA_GPU_MODE_SPECTRUM_CYCLE;
    Spectrum_Cycle.flags      = 0;
    Spectrum_Cycle.color_mode = MODE_COLORS_NONE;
    modes.push_back(Spectrum_Cycle);

    colors.resize(1);

    led aura_gpu_led;                                                       
    aura_gpu_led.name = "GPU";
    leds.push_back(aura_gpu_led);

    zone aura_gpu_zone;
    aura_gpu_zone.name = "GPU";
    aura_gpu_zone.type = ZONE_TYPE_SINGLE;

    std::vector<int> aura_gpu_zone_map;
    aura_gpu_zone_map.push_back(0);
    aura_gpu_zone.map.push_back(aura_gpu_zone_map);
    zones.push_back(aura_gpu_zone);

    unsigned char red = aura_gpu->GetLEDRed();
    unsigned char grn = aura_gpu->GetLEDGreen();
    unsigned char blu = aura_gpu->GetLEDBlue();
    
    colors[0] =  ToRGBColor(red, grn, blu);

    active_mode = GetDeviceMode();
}

void RGBController_AuraGPU::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_AuraGPU::UpdateMode()
{
    int new_mode = modes[active_mode].value;
    aura_gpu->direct = false;
    
    switch(new_mode)
    {
        
    // Set all LEDs to 0 and Mode to static as a workaround for the non existing Off Mode
    case AURA_GPU_MODE_OFF:
        aura_gpu->SetLEDColorsEffect(0, 0, 0);
        aura_gpu->AuraGPURegisterWrite(AURA_GPU_MODE_OFF, 0x01);
        new_mode = AURA_GPU_MODE_STATIC;
        break;
    
    // Direct mode is done by switching to Static and not applying color changes
    case AURA_GPU_MODE_DIRECT:
        aura_gpu->direct = true;
        new_mode = AURA_GPU_MODE_STATIC;
        break;
    }

    aura_gpu->SetMode(new_mode);
}