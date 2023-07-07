/*---------------------------------------------------------------------*\
|  BloodyKeyboardController.cpp                                         |
|                                                                       |
|  Driver for BloodyKeyboard USB Controller                             |
|                                                                       |
|  Vladisalv K. (kaluginvlad)       06 Jun 2023                         |
|                                                                       |
\*---------------------------------------------------------------------*/

#include "BloodyKeyboardController.h"

BloodyKeyboardController::BloodyKeyboardController(hid_device* dev_handle, const char* path, uint16_t product_id)
{
    dev                 = dev_handle;
    location            = path;
    pid                 = product_id;

    for (size_t i = 0; i < 6; i++)
        std::fill(color_data_buffer[i] + BLOODYKBD_COLOR_PACKET_START_OFFSET, color_data_buffer[i] + BLOODYKBD_WRITE_PACKET_SIZE, 0);

    InitDevice();
}

BloodyKeyboardController::~BloodyKeyboardController()
{
    hid_close(dev);
}

uint16_t BloodyKeyboardController::GetPid()
{
    return pid;
}

std::string BloodyKeyboardController::GetSerial()
{
    const uint8_t   sz  = HID_MAX_STR;
    wchar_t         tmp[sz];
    int ret         = hid_get_serial_number_string(dev, tmp, sz);

    if (ret != 0)
    {
        LOG_DEBUG("[BloodyKeyboard] Get HID Serial string failed");
        return("");
    }

    std::wstring w_tmp  = std::wstring(tmp);
    std::string serial  = std::string(w_tmp.begin(), w_tmp.end());

    return serial;
}

std::string BloodyKeyboardController::GetLocation()
{
    return("HID: " + location);
}

void  BloodyKeyboardController::InitDevice()
{
    for (int i = 0; i < sizeof(BLOODYKBD_INIT_DATA) / BLOODYKBD_WRITE_PACKET_SIZE; i++) {
        hid_send_feature_report(dev, BLOODYKBD_INIT_DATA[i], BLOODYKBD_WRITE_PACKET_SIZE);
    }
    SendKeyColor();
}

void BloodyKeyboardController::SetKeyColor(int keyId, uint8_t R, uint8_t G, uint8_t B) {
    color_data_buffer[0 + keyId / 58][BLOODYKBD_COLOR_PACKET_START_OFFSET + keyId % 58] = R;
    color_data_buffer[2 + keyId / 58][BLOODYKBD_COLOR_PACKET_START_OFFSET + keyId % 58] = G;
    color_data_buffer[4 + keyId / 58][BLOODYKBD_COLOR_PACKET_START_OFFSET + keyId % 58] = B;
}

void BloodyKeyboardController::SendKeyColor() {
    for (int i = 0; i < 6; i++) {
        hid_send_feature_report(dev, color_data_buffer[i], BLOODYKBD_WRITE_PACKET_SIZE);
    }
    hid_send_feature_report(dev, BLOODYKBD_LAST_PACKET, BLOODYKBD_WRITE_PACKET_SIZE);
}

void BloodyKeyboardController::SetLedsDirect(std::vector<RGBColor> colors)
{
    for(uint8_t i = 0; i < colors.size(); i++)
    {
        SetKeyColor(i, RGBGetRValue(colors[i]),
                       RGBGetGValue(colors[i]),
                       RGBGetBValue(colors[i]));
    }
    SendKeyColor();
}


