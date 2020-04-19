/*-----------------------------------------*\
|  RGBController_OpenRazer.cpp              |
|                                           |
|  Generic RGB Interface for OpenRazer      |
|  kernel drivers for Chroma peripherals    |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/15/2019   |
\*-----------------------------------------*/

#include "RGBController_OpenRazerWindows.h"

#include <algorithm>
#include <fstream>
#include <string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hid.h>

void RGBController_OpenRazer::UpdateLEDs()
{
    switch(matrix_type)
    {
        case RAZER_TYPE_MATRIX_FRAME:
        case RAZER_TYPE_MATRIX_NOFRAME:
        case RAZER_TYPE_MATRIX_STATIC:
            {
                char update_value = 1;
                
                for (unsigned int row = 0; row < matrix_rows; row++)
                {
                    unsigned int output_array_size;
                    unsigned int output_offset;
                    unsigned int row_offset = (row * matrix_cols);

                    if(matrix_type == RAZER_TYPE_MATRIX_FRAME)
                    {
                        output_array_size = 3 + (matrix_cols* 3);
                        output_offset = 3;
                    }
                    else
                    {
                        output_array_size = 3;
                        output_offset = 0;
                    }
                    
                    char* output_array = new char[output_array_size];

                    if(matrix_type == RAZER_TYPE_MATRIX_FRAME)
                    {
                        output_array[0] = row;
                        output_array[1] = 0;
                        output_array[2] = matrix_cols - 1;
                    }

                    for(unsigned int col = 0; col < matrix_cols; col++)
                    {
                        unsigned int color_idx = col + row_offset;
                        output_array[(col * 3) + 0 + output_offset] = (char)RGBGetRValue(colors[color_idx]);
                        output_array[(col * 3) + 1 + output_offset] = (char)RGBGetGValue(colors[color_idx]);
                        output_array[(col * 3) + 2 + output_offset] = (char)RGBGetBValue(colors[color_idx]);
                    }
                    
                    if(matrix_type == RAZER_TYPE_MATRIX_FRAME)
                    {
                        razer_functions->matrix_custom_frame->store(razer_device, NULL, output_array, output_array_size);
                    }
                    else if(matrix_type == RAZER_TYPE_MATRIX_NOFRAME)
                    {
                        razer_functions->matrix_effect_custom->store(razer_device, NULL, output_array, output_array_size);
                    }
                    else
                    {
                        razer_functions->matrix_effect_static->store(razer_device, NULL, output_array, output_array_size);
                    }
                }
                
                if(matrix_type == RAZER_TYPE_MATRIX_FRAME)
                {
                    razer_functions->matrix_effect_custom->store(razer_device, NULL, &update_value, 1);
                }
            }
            break;
        #if 0
        case RAZER_TYPE_NOMATRIX:
            {
                unsigned int output_array_size = 3;
                char output_array[output_array_size];
                char update_value = 0;

                output_array[0] = (char)RGBGetRValue(colors[0]);
                output_array[1] = (char)RGBGetGValue(colors[0]);
                output_array[2] = (char)RGBGetBValue(colors[0]);
                logo_led_rgb.write(output_array, output_array_size);

                output_array[0] = (char)RGBGetRValue(colors[1]);
                output_array[1] = (char)RGBGetGValue(colors[1]);
                output_array[2] = (char)RGBGetBValue(colors[1]);
                scroll_led_rgb.write(output_array, output_array_size);

                logo_led_rgb.flush();
                scroll_led_rgb.flush();

                logo_led_effect.write(&update_value, 1);
                scroll_led_effect.write(&update_value, 1);
                logo_led_effect.flush();
                scroll_led_effect.flush();
            }
            break;
            #endif
    }
}

void RGBController_OpenRazer::UpdateZoneLEDs(int /*zone*/)
{
    UpdateLEDs();
}

void RGBController_OpenRazer::UpdateSingleLED(int /*led*/)
{
    UpdateLEDs();
}

void RGBController_OpenRazer::SetupMatrixDevice(device_fn_type* razer_functions, unsigned int rows, unsigned int cols)
{
    if(!razer_functions->matrix_custom_frame)
    {
        if(!razer_functions->matrix_effect_custom)
        {
            matrix_type = RAZER_TYPE_MATRIX_STATIC;
        }
        else
        {
            matrix_type = RAZER_TYPE_MATRIX_NOFRAME;
        }

        matrix_rows = 1;
        matrix_cols = 1;
    }
    else
    {
        matrix_type = RAZER_TYPE_MATRIX_FRAME;

        matrix_rows = rows;
        matrix_cols = cols;
    }
}

void RGBController_OpenRazer::SetupNonMatrixDevice()
{
    matrix_type = RAZER_TYPE_NOMATRIX;
}

RGBController_OpenRazer::RGBController_OpenRazer(device * razer_device, device_fn_type* razer_functions)
{
    char string_buf[1024];
    
    this->razer_device = razer_device;
    this->razer_functions = razer_functions;

    /*-----------------------------------------------------------------*\
    | Start device at -1.  This indicates the device was not detected   |
    \*-----------------------------------------------------------------*/
    device_index = -1;

    /*-----------------------------------------------------------------*\
    | Get the device name from the OpenRazer driver                     |
    \*-----------------------------------------------------------------*/
    razer_functions->device_type->show(razer_device, NULL, string_buf);
    name = string_buf;
    name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
    name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());
    
    /*-----------------------------------------------------------------*\
    | Set the description to indicate this is an OpenRazer device       |
    \*-----------------------------------------------------------------*/
    description = "OpenRazer Device";

    /*-----------------------------------------------------------------*\
    | Set the device path as the location                               |
    \*-----------------------------------------------------------------*/
    location = "";

    /*-----------------------------------------------------------------*\
    | Get the serial number from the dev path                           |
    \*-----------------------------------------------------------------*/
    razer_functions->device_serial->show(razer_device, NULL, string_buf);
    serial = string_buf;

    /*-----------------------------------------------------------------*\
    | Get the firmware version from the dev path                        |
    \*-----------------------------------------------------------------*/
    razer_functions->firmware_version->show(razer_device, NULL, string_buf);
    version = string_buf;

    /*-----------------------------------------------------------------*\
    | Loop through all known devices to look for a name match           |
    \*-----------------------------------------------------------------*/
    for (unsigned int i = 0; i < RAZER_NUM_DEVICES; i++)
    {
        if (device_list[i]->name == name)
        {
            /*---------------------------------------------------------*\
            | Set device ID                                             |
            \*---------------------------------------------------------*/
            device_index = i;

            /*---------------------------------------------------------*\
            | Set device type                                           |
            \*---------------------------------------------------------*/
            type = device_list[i]->type;

            /*---------------------------------------------------------*\
            | Initialize modes                                          |
            \*---------------------------------------------------------*/
            if(razer_functions->matrix_effect_custom)
            {
                mode Custom;
                Custom.name       = "Custom";
                Custom.value      = RAZER_MODE_CUSTOM;
                Custom.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
                Custom.color_mode = MODE_COLORS_PER_LED;
                modes.push_back(Custom);
            }

            if(razer_functions->matrix_effect_none || razer_functions->logo_matrix_effect_none || razer_functions->scroll_matrix_effect_none)
            {
                mode Off;
                Off.name       = "Off";
                Off.value      = RAZER_MODE_OFF;
                Off.flags      = 0;
                Off.color_mode = MODE_COLORS_NONE;
                modes.push_back(Off);
            }

            if(razer_functions->matrix_effect_static || razer_functions->logo_matrix_effect_static || razer_functions->scroll_matrix_effect_static)
            {
                mode Static;
                Static.name       = "Static";
                Static.value      = RAZER_MODE_STATIC;
                Static.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
                Static.colors_min = 1;
                Static.colors_max = 1;
                Static.color_mode = MODE_COLORS_MODE_SPECIFIC;
                Static.colors.resize(1);
                modes.push_back(Static);
            }

            if(razer_functions->matrix_effect_breath || razer_functions->logo_matrix_effect_breath || razer_functions->scroll_matrix_effect_breath)
            {
                mode Breathing;
                Breathing.name       = "Breathing";
                Breathing.value      = RAZER_MODE_BREATHING;
                Breathing.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR;
                Breathing.colors_min = 1;
                Breathing.colors_max = 2;
                Breathing.color_mode = MODE_COLORS_MODE_SPECIFIC;
                Breathing.colors.resize(2);
                modes.push_back(Breathing);
            }

            if(razer_functions->matrix_effect_spectrum || razer_functions->logo_matrix_effect_spectrum || razer_functions->scroll_matrix_effect_spectrum)
            {
                mode SpectrumCycle;
                SpectrumCycle.name       = "Spectrum Cycle";
                SpectrumCycle.value      = RAZER_MODE_SPECTRUM_CYCLE;
                SpectrumCycle.flags      = 0;
                SpectrumCycle.color_mode = MODE_COLORS_NONE;
                modes.push_back(SpectrumCycle);
            }

            if(razer_functions->matrix_effect_wave)
            {
                mode Wave;
                Wave.name       = "Wave";
                Wave.value      = RAZER_MODE_WAVE;
                Wave.flags      = MODE_FLAG_HAS_DIRECTION_LR;
                Wave.direction  = MODE_DIRECTION_RIGHT;
                Wave.color_mode = MODE_COLORS_NONE;
                modes.push_back(Wave);
            }

            if(razer_functions->matrix_effect_reactive || razer_functions->logo_matrix_effect_reactive || razer_functions->scroll_matrix_effect_reactive)
            {
                mode Reactive;
                Reactive.name       = "Reactive";
                Reactive.value      = RAZER_MODE_REACTIVE;
                Reactive.flags      = 0;
                Reactive.color_mode = MODE_COLORS_NONE;
                modes.push_back(Reactive);
            }

            /*---------------------------------------------------------*\
            | Initialize file descriptors                               |
            \*---------------------------------------------------------*/
            if(device_list[i]->matrix_type == true)
            {
                SetupMatrixDevice(razer_functions, device_list[i]->rows, device_list[i]->cols);    
            }
            else
            {
                SetupNonMatrixDevice();
            }
            
            SetupZones();
        }
    }

}

void RGBController_OpenRazer::SetupZones()
{
    /*---------------------------------------------------------*\
    | Fill in zone information based on device table            |
    \*---------------------------------------------------------*/
    for(unsigned int zone_id = 0; zone_id < RAZER_MAX_ZONES; zone_id++)
    {
        if(device_list[device_index]->zones[zone_id] != NULL)
        {
            zone new_zone;

            new_zone.name       = device_list[device_index]->zones[zone_id]->name;
            new_zone.type       = device_list[device_index]->zones[zone_id]->type;

            new_zone.leds_count = device_list[device_index]->zones[zone_id]->rows * device_list[device_index]->zones[zone_id]->cols;
            new_zone.leds_min   = new_zone.leds_count;
            new_zone.leds_max   = new_zone.leds_count;

            zones.push_back(new_zone);
        }
    }

    for(unsigned int zone_id = 0; zone_id < zones.size(); zone_id++)
    {
        for (unsigned int row_id = 0; row_id < device_list[device_index]->zones[zone_id]->rows; row_id++)
        {
            for (unsigned int col_id = 0; col_id < device_list[device_index]->zones[zone_id]->cols; col_id++)
            {
                led* new_led = new led();

                new_led->name = device_list[device_index]->zones[zone_id]->name + " LED ";
                new_led->name.append(std::to_string(col_id + 1));
                leds.push_back(*new_led);
            }
        }
    }

    SetupColors();
}

void RGBController_OpenRazer::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_OpenRazer::SetCustomMode()
{
    SetMode(RAZER_MODE_CUSTOM);
}

void RGBController_OpenRazer::UpdateMode()
{
    char update_value[6];

    update_value[0] = 1;

    switch(matrix_type)
    {
        case RAZER_TYPE_MATRIX_FRAME:
        case RAZER_TYPE_MATRIX_NOFRAME:
            {
                switch(modes[active_mode].value)
                {
                    case RAZER_MODE_CUSTOM:
                        razer_functions->matrix_effect_custom->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_OFF:
                        razer_functions->matrix_effect_none->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_STATIC:
                        update_value[0] = RGBGetRValue(modes[active_mode].colors[0]);
                        update_value[1] = RGBGetGValue(modes[active_mode].colors[0]);
                        update_value[2] = RGBGetBValue(modes[active_mode].colors[0]);
                        razer_functions->matrix_effect_static->store(razer_device, NULL, update_value, 3);
                        break;

                    case RAZER_MODE_BREATHING:
                        switch(modes[active_mode].color_mode)
                        {
                            case MODE_COLORS_MODE_SPECIFIC:
                                update_value[0] = RGBGetRValue(modes[active_mode].colors[0]);
                                update_value[1] = RGBGetGValue(modes[active_mode].colors[0]);
                                update_value[2] = RGBGetBValue(modes[active_mode].colors[0]);

                                if(modes[active_mode].colors.size() == 2)
                                {
                                    update_value[3] = RGBGetRValue(modes[active_mode].colors[1]);
                                    update_value[4] = RGBGetGValue(modes[active_mode].colors[1]);
                                    update_value[5] = RGBGetBValue(modes[active_mode].colors[1]);

                                    razer_functions->matrix_effect_breath->store(razer_device, NULL, update_value, 6);
                                }
                                else
                                {
                                    razer_functions->matrix_effect_breath->store(razer_device, NULL, update_value, 3);
                                }
                                break;

                            case MODE_COLORS_RANDOM:
                                razer_functions->matrix_effect_breath->store(razer_device, NULL, update_value, 1);
                                break;
                        }
                        break;

                    case RAZER_MODE_SPECTRUM_CYCLE:
                        razer_functions->matrix_effect_spectrum->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_WAVE:
                        switch(modes[active_mode].direction)
                        {
                            case MODE_DIRECTION_LEFT:
                                update_value[0] = '2';
                                break;
                            
                            default:
                                update_value[0] = '1';
                                break;
                        }
                        razer_functions->matrix_effect_wave->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_REACTIVE:
                        razer_functions->matrix_effect_reactive->store(razer_device, NULL, update_value, 1);
                        break;
                }
            }
            break;

        case RAZER_TYPE_NOMATRIX:
            {
                switch(modes[active_mode].value)
                {
                    case RAZER_MODE_CUSTOM:
                        razer_functions->matrix_effect_custom->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_OFF:
                        razer_functions->logo_matrix_effect_none->store(razer_device, NULL, update_value, 1);
                        razer_functions->scroll_matrix_effect_none->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_STATIC:
                        update_value[0] = RGBGetRValue(modes[active_mode].colors[0]);
                        update_value[1] = RGBGetGValue(modes[active_mode].colors[0]);
                        update_value[2] = RGBGetBValue(modes[active_mode].colors[0]);
                        razer_functions->logo_matrix_effect_static->store(razer_device, NULL, update_value, 3);
                        razer_functions->scroll_matrix_effect_static->store(razer_device, NULL, update_value, 3);
                        break;
                    
                    case RAZER_MODE_BREATHING:
                        switch(modes[active_mode].color_mode)
                        {
                            case MODE_COLORS_MODE_SPECIFIC:
                                update_value[0] = RGBGetRValue(modes[active_mode].colors[0]);
                                update_value[1] = RGBGetGValue(modes[active_mode].colors[0]);
                                update_value[2] = RGBGetBValue(modes[active_mode].colors[0]);

                                if(modes[active_mode].colors.size() == 2)
                                {
                                    update_value[3] = RGBGetRValue(modes[active_mode].colors[1]);
                                    update_value[4] = RGBGetGValue(modes[active_mode].colors[1]);
                                    update_value[5] = RGBGetBValue(modes[active_mode].colors[1]);

                                    razer_functions->logo_matrix_effect_breath->store(razer_device, NULL, update_value, 6);
                                    razer_functions->scroll_matrix_effect_breath->store(razer_device, NULL, update_value, 6);
                                }
                                else
                                {
                                    razer_functions->logo_matrix_effect_breath->store(razer_device, NULL, update_value, 3);
                                    razer_functions->scroll_matrix_effect_breath->store(razer_device, NULL, update_value, 3);
                                }
                                break;

                            case MODE_COLORS_RANDOM:
                                razer_functions->logo_matrix_effect_breath->store(razer_device, NULL, update_value, 1);
                                razer_functions->scroll_matrix_effect_breath->store(razer_device, NULL, update_value, 1);
                                break;
                        }
                        break;

                    case RAZER_MODE_SPECTRUM_CYCLE:
                        razer_functions->logo_matrix_effect_spectrum->store(razer_device, NULL, update_value, 1);
                        razer_functions->scroll_matrix_effect_spectrum->store(razer_device, NULL, update_value, 1);
                        break;

                    case RAZER_MODE_REACTIVE:
                        razer_functions->logo_matrix_effect_reactive->store(razer_device, NULL, update_value, 1);
                        razer_functions->scroll_matrix_effect_reactive->store(razer_device, NULL, update_value, 1);
                        break;
                    }
            }
            break;

    }
}
