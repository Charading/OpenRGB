/*---------------------------------------------------------*\
|  BlinkStickController.cpp                                 |
|                                                           |
| Generic Physiqual Interface for BlinkStick LED controller |
|                                                           |
|  Foxdogface(https://gitlab.com/Foxdogface), 29/11/2022    |
\*---------------------------------------------------------*/

#include "BlinkStickController.h"

BlinkStickController::BlinkStickController(hid_device* dev_handle, const char* path, uint16_t product_id)
{
    dev                 = dev_handle;
    location            = path;
    pid                 = product_id;

    InitDevice();
}

BlinkStickController::~BlinkStickController()
{
    hid_close(dev);
}

void BlinkStickController::InitDevice()
{
   std::string serial = GetSerial().substr(9, 3);
   std::string major = serial.substr(0, 1);
   std::string minor = serial.substr(2, 1);

   if (major == "2")
   {
       generation = BLINKSTICK_PRO;
       SetMode(ARGB_MODE);

   }
   else if (major == "1")
   {
       generation = BLINKSTICK;
       SetMode(RGB_MODE);
   }
}

uint16_t BlinkStickController::GetPid()
{
    return pid;
}

std::string BlinkStickController::GetSerial()
{
    const uint8_t size = BLINCK_STICK_SERIAL_SIZE;
    wchar_t tmp[size];

    int ret = hid_get_serial_number_string(dev, tmp, size);

    if (ret != 0)
    {
        LOG_DEBUG("[BlinkStick] Get HID Serial string failed");
        return("");
    }

    std::wstring w_tmp  = std::wstring(tmp);
    std::string serial  = std::string(w_tmp.begin(), w_tmp.end());

    return serial;
}

std::string BlinkStickController::GetLocation()
{
    return("HID: " + location);
}

void BlinkStickController::SetRGBLeds(std::vector<RGBColor> colors)
{
    uint8_t buffer[4];
    buffer[0] = 0x01;

    buffer[1] = RGBGetRValue(colors[0]);
    buffer[2] = RGBGetGValue(colors[0]);
    buffer[3] = RGBGetBValue(colors[0]);

    hid_send_feature_report(dev, buffer, 4);
}

uint8_t BlinkStickController::DetermineReportID(int leds)
{
    if (leds <= 8)
    {
        return 6;
    }
    else if (leds <= 16)
    {
        return 7;
    }
    else if (leds <= 32)
    {
        return 8;
    }
    else if (leds <= 64)
    {
        return 9;
    }
    else
    {
        return -1;
    }
}

void BlinkStickController::SetARGBLedsDirect(int channel, std::vector<RGBColor>* colors)
{
    uint8_t reportID    = DetermineReportID(colors->size());
    uint8_t* buffer    = new uint8_t [COLOR_DATA_PACKET_SIZE];

    if (reportID < 6){
        return;
    }

    buffer[0] = reportID;
    buffer[1] = channel;

    uint8_t offset = 0;
    for (int colorID = 0; colorID < colors->size(); colorID++)
    {
        offset             = colorID*3 + 2;
        buffer[offset]     = RGBGetGValue(colors->at(colorID));
        buffer[offset + 1] = RGBGetRValue(colors->at(colorID));
        buffer[offset + 2] = RGBGetBValue(colors->at(colorID));
    }

    size_t buffer_size = offset+3;
    hid_send_feature_report(dev, buffer, buffer_size);
}

void BlinkStickController::SetMode(uint8_t mode)
{
    uint8_t buffer[2];
    buffer[0] = 0x04;
    buffer[1] = mode;

    hid_send_feature_report(dev, buffer, sizeof(buffer));
}

std::string BlinkStickController::GetDeviceString(int index)
{
    uint8_t buff[MAX_BUFFER_SIZE];
    buff[0] = index;

    while (buff[1] == '\0') {
        hid_get_feature_report(dev, buff, MAX_BUFFER_SIZE);
    }

    std::string data;
    for (int i = 1; i < sizeof(buff); i++)
    {
        char cur = buff[i];
        if (i > 32 || cur == '\xff')
        {
            break;
        }
        data += cur;
    }

    return data;
}

std::string BlinkStickController::GetName()
{
    return GetDeviceString(0x02);
}

std::string BlinkStickController::GetDescription()
{
    return GetDeviceString(0x03);
}
