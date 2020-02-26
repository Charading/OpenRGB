#include "RGBController.h"
#include <cstring>

unsigned char * RGBController::GetDeviceDescription()
{
    unsigned int data_ptr = 0;
    unsigned int data_size = 0;

    /*---------------------------------------------------------*\
    | Calculate data size                                       |
    \*---------------------------------------------------------*/
    unsigned short name_len         = strlen(name.c_str())          + 1;
    unsigned short description_len  = strlen(description.c_str())   + 1;
    unsigned short version_len      = strlen(version.c_str())       + 1;
    unsigned short serial_len       = strlen(serial.c_str())        + 1;
    unsigned short location_len     = strlen(location.c_str())      + 1;
    unsigned short num_modes        = modes.size();
    unsigned short num_zones        = zones.size();
    unsigned short num_leds         = leds.size();
    unsigned short num_colors       = colors.size();

    unsigned short *mode_name_len   = new unsigned short[num_modes];
    unsigned short *zone_name_len   = new unsigned short[num_zones];
    unsigned short *led_name_len    = new unsigned short[num_leds];

    unsigned short *mode_num_colors = new unsigned short[num_modes];

    unsigned short *zone_map_rows   = new unsigned short[num_zones];

    data_size += sizeof(device_type);
    data_size += name_len           + sizeof(name_len);
    data_size += description_len    + sizeof(description_len);
    data_size += version_len        + sizeof(version_len);
    data_size += serial_len         + sizeof(serial_len);
    data_size += location_len       + sizeof(location_len);

    data_size += sizeof(num_modes);
    data_size += sizeof(active_mode);

    for(int mode_index = 0; mode_index < num_modes; mode_index++)
    {
        mode_name_len[mode_index]   = strlen(modes[mode_index].name.c_str()) + 1;
        mode_num_colors[mode_index] = modes[mode_index].colors.size();

        data_size += mode_name_len[mode_index] + sizeof(mode_name_len[mode_index]);
        data_size += sizeof(modes[mode_index].value);
        data_size += sizeof(modes[mode_index].flags);
        data_size += sizeof(modes[mode_index].speed_min);
        data_size += sizeof(modes[mode_index].speed_max);
        data_size += sizeof(modes[mode_index].colors_min);
        data_size += sizeof(modes[mode_index].colors_max);
        data_size += sizeof(modes[mode_index].speed);
        data_size += sizeof(modes[mode_index].direction);
        data_size += sizeof(modes[mode_index].color_mode);
        data_size += sizeof(mode_num_colors[mode_index]);
        data_size += (mode_num_colors[mode_index] * sizeof(RGBColor));
    }

    data_size += sizeof(num_zones);

    for(int zone_index = 0; zone_index < num_zones; zone_index++)
    {
        zone_name_len[zone_index]   = strlen(zones[zone_index].name.c_str()) + 1;
        zone_map_rows[zone_index] = zones[zone_index].map.size();
        
        data_size += zone_name_len[zone_index] + sizeof(zone_name_len[zone_index]);
        data_size += sizeof(zones[zone_index].type);
        data_size += sizeof(zone_map_rows[zone_index]);

        for(int row_index = 0; row_index < zone_map_rows[zone_index]; row_index++)
        {
            data_size += sizeof(unsigned short);

            for(int row_led_index = 0; row_led_index < zones[zone_index].map[row_index].size(); row_led_index++)
            {
                data_size += sizeof(int);
            }
        }
    }

    data_size += sizeof(num_leds);

    for(int led_index = 0; led_index < num_leds; led_index++)
    {
        led_name_len[led_index] = strlen(leds[led_index].name.c_str()) + 1;

        data_size += led_name_len[led_index] + sizeof(led_name_len[led_index]);
    }

    data_size += sizeof(num_colors);
    data_size += num_colors * sizeof(RGBColor);

    /*---------------------------------------------------------*\
    | Create data buffer                                        |
    \*---------------------------------------------------------*/
    unsigned char *data_buf = new unsigned char[data_size];

    /*---------------------------------------------------------*\
    | Copy in type                                              |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &type, sizeof(device_type));
    data_ptr += sizeof(device_type);

    /*---------------------------------------------------------*\
    | Copy in name (size+data)                                  |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &name_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], name.c_str());
    data_ptr += name_len;

    /*---------------------------------------------------------*\
    | Copy in description (size+data)                           |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &description_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], description.c_str());
    data_ptr += description_len;
    
    /*---------------------------------------------------------*\
    | Copy in version (size+data)                               |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &version_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], version.c_str());
    data_ptr += version_len;

    /*---------------------------------------------------------*\
    | Copy in serial (size+data)                                |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &serial_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], serial.c_str());
    data_ptr += serial_len;

    /*---------------------------------------------------------*\
    | Copy in location (size+data)                              |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &location_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], location.c_str());
    data_ptr += location_len;

    /*---------------------------------------------------------*\
    | Copy in number of modes (data)                            |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_modes, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in active mode (data)                                |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &active_mode, sizeof(active_mode));
    data_ptr += sizeof(active_mode);

    /*---------------------------------------------------------*\
    | Copy in modes                                             |
    \*---------------------------------------------------------*/
    for(int mode_index = 0; mode_index < num_modes; mode_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in mode name (size+data)                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &mode_name_len[mode_index], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], modes[mode_index].name.c_str());
        data_ptr += mode_name_len[mode_index];

        /*---------------------------------------------------------*\
        | Copy in mode value (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].value, sizeof(modes[mode_index].value));
        data_ptr += sizeof(modes[mode_index].value);

        /*---------------------------------------------------------*\
        | Copy in mode flags (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].flags, sizeof(modes[mode_index].flags));
        data_ptr += sizeof(modes[mode_index].flags);

        /*---------------------------------------------------------*\
        | Copy in mode speed_min (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].speed_min, sizeof(modes[mode_index].speed_min));
        data_ptr += sizeof(modes[mode_index].speed_min);

        /*---------------------------------------------------------*\
        | Copy in mode speed_max (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].speed_max, sizeof(modes[mode_index].speed_max));
        data_ptr += sizeof(modes[mode_index].speed_max);

        /*---------------------------------------------------------*\
        | Copy in mode colors_min (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].colors_min, sizeof(modes[mode_index].colors_min));
        data_ptr += sizeof(modes[mode_index].colors_min);

        /*---------------------------------------------------------*\
        | Copy in mode colors_max (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].colors_max, sizeof(modes[mode_index].colors_max));
        data_ptr += sizeof(modes[mode_index].colors_max);

        /*---------------------------------------------------------*\
        | Copy in mode speed (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].speed, sizeof(modes[mode_index].speed));
        data_ptr += sizeof(modes[mode_index].speed);

        /*---------------------------------------------------------*\
        | Copy in mode direction (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].direction, sizeof(modes[mode_index].direction));
        data_ptr += sizeof(modes[mode_index].direction);

        /*---------------------------------------------------------*\
        | Copy in mode color_mode (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &modes[mode_index].color_mode, sizeof(modes[mode_index].color_mode));
        data_ptr += sizeof(modes[mode_index].color_mode);

        /*---------------------------------------------------------*\
        | Copy in mode number of colors                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &mode_num_colors[mode_index], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        /*---------------------------------------------------------*\
        | Copy in mode mode colors                                  |
        \*---------------------------------------------------------*/
        for(int color_index = 0; color_index < mode_num_colors[mode_index]; color_index++)
        {
            /*---------------------------------------------------------*\
            | Copy in color (data)                                      |
            \*---------------------------------------------------------*/
            memcpy(&data_buf[data_ptr], &modes[mode_index].colors[color_index], sizeof(modes[mode_index].colors[color_index]));
            data_ptr += sizeof(modes[mode_index].colors[color_index]);
        }
    }

    /*---------------------------------------------------------*\
    | Copy in number of zones (data)                            |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_zones, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in zones                                             |
    \*---------------------------------------------------------*/
    for(int zone_index = 0; zone_index < num_zones; zone_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in zone name (size+data)                             |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &zone_name_len[zone_index], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], zones[zone_index].name.c_str());
        data_ptr += zone_name_len[zone_index];

        /*---------------------------------------------------------*\
        | Copy in zone type (data)                                  |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &zones[zone_index].type, sizeof(zones[zone_index].type));
        data_ptr += sizeof(zones[zone_index].type);

        /*---------------------------------------------------------*\
        | Copy in zone map number of rows                           |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &zone_map_rows[zone_index], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        /*---------------------------------------------------------*\
        | Copy in rows                                              |
        \*---------------------------------------------------------*/
        for(int row_index = 0; row_index < zone_map_rows[zone_index]; row_index++)
        {
            /*---------------------------------------------------------*\
            | Copy row size                                             |
            \*---------------------------------------------------------*/
            unsigned short zone_map_row_size = zones[zone_index].map[row_index].size();
            memcpy(&data_buf[data_ptr], &zone_map_row_size, sizeof(unsigned short));
            data_ptr += sizeof(unsigned short);

            /*---------------------------------------------------------*\
            | Copy in row LEDs                                          |
            \*---------------------------------------------------------*/
            for(int row_led_index = 0; row_led_index < zone_map_row_size; row_led_index++)
            {
                /*---------------------------------------------------------*\
                | Copy row size                                             |
                \*---------------------------------------------------------*/
                memcpy(&data_buf[data_ptr], &zones[zone_index].map[row_index][row_led_index], sizeof(zones[zone_index].map[row_index][row_led_index]));
                data_ptr += sizeof(zones[zone_index].map[row_index][row_led_index]);
            }
        }
    }

    /*---------------------------------------------------------*\
    | Copy in number of LEDs (data)                             |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_leds, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in LEDs                                              |
    \*---------------------------------------------------------*/
    for(int led_index = 0; led_index < num_leds; led_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in LED name (size+data)                              |
        \*---------------------------------------------------------*/
        unsigned short ledname_len = strlen(leds[led_index].name.c_str()) + 1;
        memcpy(&data_buf[data_ptr], &ledname_len, sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], leds[led_index].name.c_str());
        data_ptr += ledname_len;
    }

    /*---------------------------------------------------------*\
    | Copy in number of colors (data)                           |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_colors, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in colors                                            |
    \*---------------------------------------------------------*/
    for(int color_index = 0; color_index < num_colors; color_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in color (data)                                      |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &colors[color_index], sizeof(colors[color_index]));
        data_ptr += sizeof(colors[color_index]);
    }


    delete[] mode_name_len;
    delete[] zone_name_len;
    delete[] led_name_len;

    delete[] mode_num_colors;
    delete[] zone_map_rows;

    return(data_buf);
}

void RGBController::ReadDeviceDescription(unsigned char* data_buf)
{
    unsigned int data_ptr = 0;

    /*---------------------------------------------------------*\
    | Copy in type                                              |
    \*---------------------------------------------------------*/
    memcpy(&type, &data_buf[data_ptr], sizeof(device_type));
    data_ptr += sizeof(device_type);

    /*---------------------------------------------------------*\
    | Copy in name                                              |
    \*---------------------------------------------------------*/
    unsigned short name_len;
    memcpy(&name_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    name = (char *)&data_buf[data_ptr];
    data_ptr += name_len;

    /*---------------------------------------------------------*\
    | Copy in description                                       |
    \*---------------------------------------------------------*/
    unsigned short description_len;
    memcpy(&description_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    description = (char *)&data_buf[data_ptr];
    data_ptr += description_len;

    /*---------------------------------------------------------*\
    | Copy in version                                           |
    \*---------------------------------------------------------*/
    unsigned short version_len;
    memcpy(&version_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    version = (char *)&data_buf[data_ptr];
    data_ptr += version_len;
    
    /*---------------------------------------------------------*\
    | Copy in serial                                            |
    \*---------------------------------------------------------*/
    unsigned short serial_len;
    memcpy(&serial_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    serial = (char *)&data_buf[data_ptr];
    data_ptr += serial_len;

    /*---------------------------------------------------------*\
    | Copy in location                                          |
    \*---------------------------------------------------------*/
    unsigned short location_len;
    memcpy(&location_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    location = (char *)&data_buf[data_ptr];
    data_ptr += location_len;

    /*---------------------------------------------------------*\
    | Copy in number of modes (data)                            |
    \*---------------------------------------------------------*/
    unsigned short num_modes;
    memcpy(&num_modes, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in active mode (data)                                |
    \*---------------------------------------------------------*/
    memcpy(&active_mode, &data_buf[data_ptr], sizeof(active_mode));
    data_ptr += sizeof(active_mode);

    /*---------------------------------------------------------*\
    | Copy in modes                                             |
    \*---------------------------------------------------------*/
    for(int mode_index = 0; mode_index < num_modes; mode_index++)
    {
        mode new_mode;

        /*---------------------------------------------------------*\
        | Copy in mode name (size+data)                             |
        \*---------------------------------------------------------*/
        unsigned short modename_len;
        memcpy(&modename_len, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        new_mode.name = (char *)&data_buf[data_ptr];
        data_ptr += modename_len;

        /*---------------------------------------------------------*\
        | Copy in mode value (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.value, &data_buf[data_ptr], sizeof(new_mode.value));
        data_ptr += sizeof(new_mode.value);

        /*---------------------------------------------------------*\
        | Copy in mode flags (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.flags, &data_buf[data_ptr], sizeof(new_mode.flags));
        data_ptr += sizeof(new_mode.flags);

        /*---------------------------------------------------------*\
        | Copy in mode speed_min (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.speed_min, &data_buf[data_ptr], sizeof(new_mode.speed_min));
        data_ptr += sizeof(new_mode.speed_min);

        /*---------------------------------------------------------*\
        | Copy in mode speed_max (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.speed_max, &data_buf[data_ptr], sizeof(new_mode.speed_max));
        data_ptr += sizeof(new_mode.speed_max);

        /*---------------------------------------------------------*\
        | Copy in mode colors_min (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.colors_min, &data_buf[data_ptr], sizeof(new_mode.colors_min));
        data_ptr += sizeof(new_mode.colors_min);

        /*---------------------------------------------------------*\
        | Copy in mode colors_max (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.colors_max, &data_buf[data_ptr], sizeof(new_mode.colors_max));
        data_ptr += sizeof(new_mode.colors_max);

        /*---------------------------------------------------------*\
        | Copy in mode speed (data)                                 |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.speed, &data_buf[data_ptr], sizeof(new_mode.speed));
        data_ptr += sizeof(new_mode.speed);

        /*---------------------------------------------------------*\
        | Copy in mode direction (data)                             |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.direction, &data_buf[data_ptr], sizeof(new_mode.direction));
        data_ptr += sizeof(new_mode.direction);

        /*---------------------------------------------------------*\
        | Copy in mode color_mode (data)                            |
        \*---------------------------------------------------------*/
        memcpy(&new_mode.color_mode, &data_buf[data_ptr], sizeof(new_mode.color_mode));
        data_ptr += sizeof(new_mode.color_mode);

        /*---------------------------------------------------------*\
        | Copy in mode number of colors                             |
        \*---------------------------------------------------------*/
        unsigned short mode_num_colors;
        memcpy(&mode_num_colors, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        /*---------------------------------------------------------*\
        | Copy in mode mode colors                                  |
        \*---------------------------------------------------------*/
        for(int color_index = 0; color_index < mode_num_colors; color_index++)
        {
            /*---------------------------------------------------------*\
            | Copy in color (data)                                      |
            \*---------------------------------------------------------*/
            RGBColor new_color;
            memcpy(&new_color, &data_buf[data_ptr], sizeof(RGBColor));
            data_ptr += sizeof(modes[mode_index].colors[color_index]);

            new_mode.colors.push_back(new_color);
        }

        modes.push_back(new_mode);
    }

    /*---------------------------------------------------------*\
    | Copy in number of zones (data)                            |
    \*---------------------------------------------------------*/
    unsigned short num_zones;
    memcpy(&num_zones, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in zones                                             |
    \*---------------------------------------------------------*/
    for(int zone_index = 0; zone_index < num_zones; zone_index++)
    {
        zone new_zone;

        /*---------------------------------------------------------*\
        | Copy in zone name (size+data)                             |
        \*---------------------------------------------------------*/
        unsigned short zonename_len;
        memcpy(&zonename_len, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        new_zone.name = (char *)&data_buf[data_ptr];
        data_ptr += zonename_len;

        /*---------------------------------------------------------*\
        | Copy in zone type (data)                                  |
        \*---------------------------------------------------------*/
        memcpy(&new_zone.type, &data_buf[data_ptr], sizeof(new_zone.type));
        data_ptr += sizeof(new_zone.type);

        /*---------------------------------------------------------*\
        | Copy in zone map number of rows                           |
        \*---------------------------------------------------------*/
        unsigned short zone_map_rows;
        memcpy(&zone_map_rows, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        /*---------------------------------------------------------*\
        | Copy in rows                                              |
        \*---------------------------------------------------------*/
        for(int row_index = 0; row_index < zone_map_rows; row_index++)
        {
            std::vector<int> new_zone_map_row;

            /*---------------------------------------------------------*\
            | Copy row size                                             |
            \*---------------------------------------------------------*/
            unsigned short zone_map_row_size;
            memcpy(&zone_map_row_size, &data_buf[data_ptr], sizeof(unsigned short));
            data_ptr += sizeof(unsigned short);

            /*---------------------------------------------------------*\
            | Copy in row LEDs                                          |
            \*---------------------------------------------------------*/
            for(int row_led_index = 0; row_led_index < zone_map_row_size; row_led_index++)
            {
                int new_index;

                /*---------------------------------------------------------*\
                | Copy row size                                             |
                \*---------------------------------------------------------*/
                memcpy(&new_index, &data_buf[data_ptr], sizeof(int));
                data_ptr += sizeof(int);

                new_zone_map_row.push_back(new_index);
            }

            new_zone.map.push_back(new_zone_map_row);
        }

        zones.push_back(new_zone);
    }

    /*---------------------------------------------------------*\
    | Copy in number of LEDs (data)                             |
    \*---------------------------------------------------------*/
    unsigned short num_leds;
    memcpy(&num_leds, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in LEDs                                              |
    \*---------------------------------------------------------*/
    for(int led_index = 0; led_index < num_leds; led_index++)
    {
        led new_led;

        /*---------------------------------------------------------*\
        | Copy in LED name (size+data)                              |
        \*---------------------------------------------------------*/
        unsigned short ledname_len;
        memcpy(&ledname_len, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        new_led.name = (char *)&data_buf[data_ptr];
        data_ptr += ledname_len;

        leds.push_back(new_led);
    }

    /*---------------------------------------------------------*\
    | Copy in number of colors (data)                           |
    \*---------------------------------------------------------*/
    unsigned short num_colors;
    memcpy(&num_colors, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in colors                                            |
    \*---------------------------------------------------------*/
    for(int color_index = 0; color_index < num_colors; color_index++)
    {
        RGBColor new_color;

        /*---------------------------------------------------------*\
        | Copy in color (data)                                      |
        \*---------------------------------------------------------*/
        memcpy(&new_color, &data_buf[data_ptr], sizeof(RGBColor));
        data_ptr += sizeof(RGBColor);

        colors.push_back(new_color);
    }
}

RGBColor RGBController::GetLED(int led)
{
    if(led < colors.size())
    {
        return(colors[led]);
    }
    else
    {
        return(0x00000000);
    }
}

void RGBController::SetLED(int led, RGBColor color)
{
    if(led < colors.size())
    {
        colors[led] = color;

        UpdateSingleLED(led);
    }
}

void RGBController::SetAllLEDs(RGBColor color)
{
    for(int led = 0; led < colors.size(); led++)
    {
        colors[led] = color;
    }

    UpdateLEDs();
}

void RGBController::SetAllZoneLEDs(int zone, RGBColor color)
{
    for (std::size_t x = 0; x < zones[zone].map.size(); x++)
    {
        for (std::size_t y = 0; y < zones[zone].map[x].size(); y++)
        {
            colors[zones[zone].map[x][y]] = color;
        }
    }

    UpdateZoneLEDs(zone);
}

int RGBController::GetMode()
{
    return(active_mode);
}

void RGBController::SetMode(int mode)
{
    active_mode = mode;

    UpdateMode();
}

std::string device_type_to_str(device_type type)
{
    switch(type)
    {
    case DEVICE_TYPE_MOTHERBOARD:
        return "Motherboard";
    case DEVICE_TYPE_DRAM:
        return "DRAM";
    case DEVICE_TYPE_GPU:
        return "GPU";
    case DEVICE_TYPE_COOLER:
        return "Cooler";
    case DEVICE_TYPE_LEDSTRIP:
        return "LED Strip";
    case DEVICE_TYPE_KEYBOARD:
        return "Keyboard";
    case DEVICE_TYPE_MOUSE:
        return "Mouse";
    case DEVICE_TYPE_MOUSEMAT:
        return "Mousemat";
    case DEVICE_TYPE_HEADSET:
        return "Headset";
    default:
        return "Unknown";
    }
}
