/*-------------------------------------------------------------------*\
|  CMR6000Controller.h                                                |
|                                                                     |
|  Driver for Coolermaster based AMD Radeon GPU (6000 series)         |
|                                                                     |
|  Eric S (edbgon)          2nd Feb 2021                              |
|                                                                     |
\*-------------------------------------------------------------------*/

#include <string>
#include <array>
#include <hidapi/hidapi.h>

#pragma once

#define CM_INTERRUPT_TIMEOUT 250
#define CM_DEVICE_NAME_SIZE (sizeof(device_name) / sizeof(device_name[ 0 ]))
#define CM_SERIAL_SIZE (sizeof(serial) / sizeof(serial[ 0 ]))
#define HID_MAX_STR 255

enum
{
    CM_MR6000_MODE_OFF           = 0x00, //Off
    CM_MR6000_MODE_STATIC        = 0x01, //Static Mode
    CM_MR6000_MODE_MORSE         = 0x02, //Morse code (need text entry?)
    CM_MR6000_MODE_BREATHING     = 0x03, //Breathing
    CM_MR6000_MODE_COLORCYCLE    = 0x04, //Color cycle
};

class CMR6000Controller
{
public:
    CMR6000Controller(hid_device* dev_handle, char *_path);
    ~CMR6000Controller();

    std::string GetDeviceName();
    std::string GetSerial();
    std::string GetLocation();

    unsigned char GetMode();
    unsigned char GetLedRed();
    unsigned char GetLedGreen();
    unsigned char GetLedBlue();
    unsigned char GetLedSpeed();
    void SetMode(unsigned char mode, unsigned char speed);
    void SetColor(unsigned char red, unsigned char green, unsigned char blue);

private:
    std::string             device_name;
    std::string             serial;
    std::string             location;
    hid_device*             dev;

    unsigned char           current_mode;
    unsigned char           current_speed;

    unsigned char           current_red;
    unsigned char           current_green;
    unsigned char           current_blue;

    void GetStatus();
    void SendUpdate();
};
