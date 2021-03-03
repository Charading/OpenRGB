/*-----------------------------------------*\
|  AsusAuraKeyboardController.cpp           |
|                                           |
|  Driver for ASUS Aura RGB USB             |
|  lighting controller                      |
|                                           |
|  Mola19 03/03/2021                        |
\*-----------------------------------------*/

#include "AsusAuraKeyboardController.h"

#include <cstring>
#include <string>
#include <vector>
#include <math.h>

AuraKeyboardController::AuraKeyboardController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;
}

AuraKeyboardController::~AuraKeyboardController()
{
    hid_close(dev);
}

std::string AuraKeyboardController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string AuraKeyboardController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void AuraKeyboardController::UpdateSingleLed
    (
    int led,
    unsigned char red,
    unsigned char green,
    unsigned char blue
    )
{

    /*-----------------------------------------------------*\
    | Set up message packet for single Led                  |
    \*-----------------------------------------------------*/

    unsigned char usb_buf[65];
    memset(usb_buf, 0x00, sizeof(usb_buf));
    usb_buf[0] = 0x00;
    usb_buf[1] = 0xc0;
    usb_buf[2] = 0x81;
    usb_buf[3] = 0x01;
    usb_buf[4] = 0x00;

    /*-----------------------------------------------------*\
    | Convert Led index                                     |
    \*-----------------------------------------------------*/

    usb_buf[5] = floor(led/6)*8 + led%6;
    usb_buf[6] = red;
    usb_buf[7] = green;
    usb_buf[8] = blue;


    hid_write(dev, usb_buf, 65);

}

void AuraKeyboardController::UpdateLeds
    (
    std::vector<RGBColor>    colors
    )
{

    /*-----------------------------------------------------*\
    | Loop over every column (optimisation needed)          |
    \*-----------------------------------------------------*/

    for (int i = 0; i < 192; i += 16) {
            unsigned char usb_buf[69];
            memset(usb_buf, 0x00, sizeof(usb_buf));
            usb_buf[0] = 0x00;
            usb_buf[1] = 0xc0;
            usb_buf[2] = 0x81;
            usb_buf[3] = (138-i/16*12);
            usb_buf[4] = 0x00;

            for (int n = 0; n < 6; n++){
                usb_buf[n * 4 + 5] = i + n;
                usb_buf[n * 4 + 6] = RGBGetRValue(colors[i/16*12+n]);
                usb_buf[n * 4 + 7] = RGBGetGValue(colors[i/16*12+n]);
                usb_buf[n * 4 + 8] = RGBGetBValue(colors[i/16*12+n]);
            }
            for (int n = 6; n < 8; n++){
                usb_buf[n * 4 + 5] = i + n;
                usb_buf[n * 4 + 6] = 0;
                usb_buf[n * 4 + 7] = 0;
                usb_buf[n * 4 + 8] = 0;
            }
            if (i != 176) {
                for (int n = 8; n < 14; n++) {
                    usb_buf[n * 4 + 5] = i + n;
                    usb_buf[n * 4 + 6] = RGBGetRValue(colors[i/16*12+n-2]);
                    usb_buf[n * 4 + 7] = RGBGetGValue(colors[i/16*12+n-2]);
                    usb_buf[n * 4 + 8] = RGBGetBValue(colors[i/16*12+n-2]);
                }
                for (int n = 14; n < 16; n++){
                    usb_buf[n * 4 + 5] = i + n;
                    usb_buf[n * 4 + 6] = 0;
                    usb_buf[n * 4 + 7] = 0;
                    usb_buf[n * 4 + 8] = 0;
                }
            }

            hid_write(dev, usb_buf, 69);
        }


    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
}

void AuraKeyboardController::UpdateDevice
    (
    unsigned char   mode,
    std::vector<RGBColor>    colors,
    unsigned char   dir,
    unsigned char   color_mode,
    unsigned char   speed
    )
{
    unsigned char usb_buf[65];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
if (mode == 4 || mode == 5 || mode == 7)
    {
    /*-----------------------------------------------------*\
    | If mode is Rainbow, Ripple or Quicksand               |
    \*-----------------------------------------------------*/
        usb_buf[0x00]   = 0x00;
        usb_buf[0x01]   = 0x51;
        usb_buf[0x02]   = 0x2c;
        usb_buf[0x03]   = mode;
        usb_buf[0x04]   = 0x00;
        usb_buf[0x05]   = speed;
        usb_buf[0x06]   = 0x64;
        usb_buf[0x07]   = color_mode;
        usb_buf[0x08]   = dir;
        usb_buf[0x09]   = 0x02;
        usb_buf[0x0a]   = colors.size();
        /*-----------------------------------------------------*\
        | Loop over every color given                           |
        \*-----------------------------------------------------*/
        for(unsigned int i = 0; i/4 != colors.size(); i+=4)
            {
                if (colors[i/4])
                {
                    usb_buf[11 + i] = round(100/(colors.size()-1)*(i/4+1));
                    usb_buf[12 + i] = RGBGetRValue(colors[i/4]);
                    usb_buf[13 + i] = RGBGetGValue(colors[i/4]);
                    usb_buf[14 + i] = RGBGetBValue(colors[i/4]);
                }
            }
    } else {
        usb_buf[0x00]   = 0x00;
        usb_buf[0x01]   = 0x51;
        usb_buf[0x02]   = 0x2c;
        usb_buf[0x03]   = mode;
        usb_buf[0x04]   = 0x00;
        usb_buf[0x05]   = speed;
        usb_buf[0x06]   = 0x64;
        usb_buf[0x07]   = color_mode;
        usb_buf[0x08]   = dir;
        usb_buf[0x09]   = 0x02;
        /*-----------------------------------------------------*\
        | Loop over Color1, Color2 and Background if there      |
        \*-----------------------------------------------------*/
        for(unsigned int i = 0; i/3 != colors.size(); i+=3)
            {
                if (colors[i/3])
                {
                    usb_buf[10 + i] = RGBGetRValue(colors[i/3]);
                    usb_buf[11 + i] = RGBGetGValue(colors[i/3]);
                    usb_buf[12 + i] = RGBGetBValue(colors[i/3]);
                }
            }

    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 65);

    /*-----------------------------------------------------*\
    | Set up and send packet save Packet                    |
    \*-----------------------------------------------------*/

    unsigned char usb_save_buf[65];
    memset(usb_save_buf, 0x00, sizeof(usb_save_buf));
    usb_save_buf[0x00]   = 0x00;
    usb_save_buf[0x01]   = 0x50;
    usb_save_buf[0x02]   = 0x55;
    hid_write(dev, usb_save_buf, 65);
}

