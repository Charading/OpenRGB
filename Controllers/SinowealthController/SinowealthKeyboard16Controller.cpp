/*------------------------------------------*\
|  SinowealthKeyboard16Controller.cpp        |
|                                            |
|  Definitions and types for Sinowealth      |
|  Keyboard with PID:0016,                   |
|  Hopefully generic for this PID,           |
|  this was made spefically for ZUOYA X51    |
|                                            |
|  Zagorodnikov Aleksey (glooom) 26.07.2021  |
|  based on initial implementation from      |
|  Dmitri Kalinichenko (Dima-Kal) 23/06/2021 |
\*-----------------------------------------=*/

#include "SinowealthKeyboard16Controller.h"
#include "LogManager.h"
#include <cstring>
#include <chrono>

using namespace std::chrono_literals;
using namespace kbd16;

static unsigned char request_init[]         = {0x05, 0x01, 0xaa, 0xbb, 0x2f, 0x3e};
static unsigned char request_modes[]        = {0x05, 0x83, 0x00, 0x00, 0x00, 0x00};
static unsigned char request_colors[]       = {0x05, 0x88, 0xA8, 0x00, 0x40, 0x00};
static unsigned char request_per_led_cm12[] = {0x05, 0x89, 0xAC, 0x00, 0x40, 0x00};
static unsigned char request_per_led_cm34[] = {0x05, 0x89, 0xB0, 0x00, 0x40, 0x00};
static unsigned char request_per_led_cm5[]  = {0x05, 0x89, 0xB4, 0x00, 0x40, 0x00};


SinowealthKeyboard16Controller::SinowealthKeyboard16Controller(hid_device* dev_handle_id_4, hid_device* dev_handle_id_5, char* path, std::string dev_name)
{
    dev_report_id_4 = dev_handle_id_4;
    dev_report_id_5 = dev_handle_id_5;
    name = dev_name;

    led_count       = 132;

    current_mode    = MODE_OFF;
    location        = path;

    memset(mode_config_buf, 0x00, sizeof(mode_config_buf));
    initCommunication();
    UpdateConfigurationFromDevice();
}

SinowealthKeyboard16Controller::~SinowealthKeyboard16Controller()
{
    hid_close(dev_report_id_4);
    hid_close(dev_report_id_5);
}

std::string SinowealthKeyboard16Controller::GetLocation()
{
    return("HID: " + location);
}

unsigned char SinowealthKeyboard16Controller::GetCurrentMode()
{
    return current_mode;
}

unsigned int SinowealthKeyboard16Controller::GetLEDCount()
{
    return led_count;
}

std::string SinowealthKeyboard16Controller::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev_report_id_4, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void SinowealthKeyboard16Controller::SetLEDsDirect(std::vector<RGBColor> colors)
{
    const int colors_offset = led_count;

    int i = colors_start_idx + (current_custom_preset * led_count * 3);
    for(const auto &color : colors)
    {
        per_button_color_buf[i] = RGBGetBValue(color);
        per_button_color_buf[i+colors_offset] = RGBGetGValue(color);
        per_button_color_buf[i+colors_offset+colors_offset] = RGBGetRValue(color);
        i++;
    }

    sendConfig(per_button_color_buf);
}

void SinowealthKeyboard16Controller::SetMode(unsigned char mode, unsigned char brightness, unsigned char speed, bool direction_left, unsigned char color_mode)
{
    if(mode >= MODE_PER_KEY1)
    {
        current_custom_preset = (mode&0x0F);
        mode_config_buf[current_mode_idx] = MODE_PER_KEY;
        mode_config_buf[per_key_mode_idx] = 1;
        mode_config_buf[current_mode_idx+1] = (0x20 | current_custom_preset);

        GetButtonColorsConfig(per_button_color_buf);
    }
    else
    {
        mode_config_buf[current_mode_idx] = mode;
        device_modes[mode].brightness = brightness;
        device_modes[mode].speed = speed;
        device_modes[mode].color = color_mode;
        device_modes[mode].direction_left = direction_left;
        mode_config_buf[per_key_mode_idx] = 0;
    }

    if(sendConfig(mode_config_buf))
    {
        current_mode = mode;
    }
}

void SinowealthKeyboard16Controller::SetColorsForMode(unsigned char mode, RGBColor *profiles)
{
    if(mode >= MODE_PER_KEY1){
        return;
    }

    for(int i = 0; i < COLOR_PRESETS_IN_MODE; i++)
    {
        modes_colors[mode].preset[i].red = RGBGetRValue(profiles[i]);
        modes_colors[mode].preset[i].green = RGBGetGValue(profiles[i]);
        modes_colors[mode].preset[i].blue = RGBGetBValue(profiles[i]);
    }

    sendConfig(colors_config_buf);
}

// Before apply new settings, keyboard needs turn off LEDs and wait ~200ms, otherwise sometime glitches happens.
void SinowealthKeyboard16Controller::ClearMode()
{
    mode_config_buf[per_key_mode_idx] = 0;
    mode_config_buf[current_mode_idx] = 0;
    mode_config_buf[current_mode_idx+1] = 0;
    sendConfig(mode_config_buf);
    std::this_thread::sleep_for(200ms);
}

std::vector<ModeCfg> SinowealthKeyboard16Controller::GetDeviceModes()
{
    std::vector<ModeCfg> modes;

    for(int i=0; i<profiles_count; i++){
        modes.push_back(device_modes[i]);
    }
    return modes;
}

std::vector<ModeColorCfg> SinowealthKeyboard16Controller::GetDeviceColors()
{
    std::vector<ModeColorCfg> presets;

    for(int i=0; i<profiles_count; i++)
    {
        presets.push_back(modes_colors[i]);
    }
    return presets;
}

std::vector<RGBColor> SinowealthKeyboard16Controller::GetPerLedColors()
{
    const int colors_offset = led_count;
    int start_idx = colors_start_idx;

     // CM2 and CM4 presets are located in second half of corresponding arrays
    if(current_custom_preset == 1 || current_custom_preset == 3)
    {
        start_idx += (led_count * 3);
    }

    std::vector<RGBColor> res;
    res.resize(led_count);

    for(unsigned int i =0; i < led_count; i++)
    {
        RGBColor color = (per_button_color_buf[start_idx+i] << 16)
                | (per_button_color_buf[start_idx+i+colors_offset] << 8)
                | (per_button_color_buf[start_idx+i+colors_offset+colors_offset]);

        res[i] = color;
    }
    return res;
}

void SinowealthKeyboard16Controller::UpdateConfigurationFromDevice()
{
    GetModesConfig(mode_config_buf);
    GetColorsConfig(colors_config_buf);

    //mode_config_buf[current_mode_idx+1] = 0x20;
    current_mode = mode_config_buf[current_mode_idx];
    device_modes = (struct ModeCfg*) &mode_config_buf[profiles_start_idx];
    modes_colors = (struct ModeColorCfg*) &colors_config_buf[colors_start_idx];

    if(current_mode == MODE_PER_KEY)
    {
        current_custom_preset = (mode_config_buf[current_mode_idx+1] & 0x0F);
        current_mode = ((MODE_PER_KEY&0xF) << 4) | current_custom_preset;
        GetButtonColorsConfig(per_button_color_buf);
    }
}

void SinowealthKeyboard16Controller::GetModesConfig(unsigned char *buf)
{
    if(!getConfig(request_modes, buf))
    {
        LOG_ERROR("[%s] Could not read modes config table", name.c_str());
    }
}

void SinowealthKeyboard16Controller::GetColorsConfig(unsigned char *buf)
{
    if(!getConfig(request_colors, buf))
    {
        LOG_ERROR("[%s] Could not read colors config table", name.c_str());
    }
}

void SinowealthKeyboard16Controller::GetButtonColorsConfig(unsigned char *buf)
{
    unsigned char *req;
    switch(current_custom_preset)
    {
    case 2: // CM3
    case 3: // CM4
        req = request_per_led_cm34;
        break;
    case 4: // CM5
        req = request_per_led_cm5;
        break;
    case 0: // CM1
    case 1: // CM2
    default:
        req = request_per_led_cm12;
        break;
    }

    if(!getConfig(req, buf)){
        LOG_ERROR("[%s] Could not read per LED config table", name.c_str());
        return;
    }
}

void SinowealthKeyboard16Controller::initCommunication()
{
    // This needs to be sent first time after powerup keyboard.
    hid_send_feature_report(dev_report_id_4, request_init, 6);
}

bool SinowealthKeyboard16Controller::getConfig(unsigned char *request, unsigned char *buf)
{
    hid_send_feature_report(dev_report_id_4, request, 6);

    unsigned char response[PAYLOAD_LEN];
    memset(response, 0x00, PAYLOAD_LEN);
    response[0] = 0x06;
    if(hid_get_feature_report(dev_report_id_5, response, PAYLOAD_LEN) != -1)
    {
        response[1] &= ~0x80; // Clear response flag
        response[2] = request[2];
        response[3] = request[3];
        response[4] = request[4];
        response[5] = request[5];
        memcpy(buf,response,PAYLOAD_LEN);
        read_config_error = false;
        return true;
    }
    read_config_error = true;
    return false;
}

bool SinowealthKeyboard16Controller::sendConfig(unsigned char *buf)
{
    if(read_config_error)
    {
        LOG_ERROR("[%s] Can't send new config if reading old config fail", name.c_str());
        return false;
    }

    int result = hid_send_feature_report(dev_report_id_5, buf, PAYLOAD_LEN);
    return (result != -1);
}
