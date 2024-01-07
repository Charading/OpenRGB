﻿/*-----------------------------------------*\
|  ValkyrieKeyboardController.h             |
|                                           |
|  Definitions and types for Valkyrie RGB   |
|  keyboard lighting controller             |
|                                           |
|  Nollie(Nuonuo)               2023/12/6   |
\*-----------------------------------------*/
#pragma once

#include "RGBController.h"
#include <string>
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| Valkyrie vendor ID                                   |
\*-----------------------------------------------------*/
#define VALKYRIE_VID                        0x05AC

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define VALKYRIE_99_PRO_PID                0x024F

class ValkyrieKeyboardController
{
public:
    ValkyrieKeyboardController(hid_device* dev_handle, const char* path, const unsigned short pid);
    ~ValkyrieKeyboardController();

    std::string     GetDeviceLocation();
    std::string     GetSerialString();
    unsigned short  GetUSBPID();
    
    void SendColors
            (
            unsigned char*  color_data,
            unsigned int    color_data_size
            );

private:
    hid_device*             dev;
    std::string             location;
    unsigned short          usb_pid;

    void SendInitializeColorPacket();
    void SendTerminateColorPacket();
    int key_code_99[98] =
    {
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x77,
        0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x67,
        0x74,0x20,0x21,0x22,0x7A,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,
        0x2E,0x2F,0x30,0x31,0x43,0x76,0x32,0x33,0x34,0x7B,0x37,0x38,0x39,0x3A,
        0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x55,0x79,0x44,0x45,0x46,0x49,
        0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x65,0x56,0x57,
        0x58,0x6A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x62,0x63,0x64,0x66,0x68,0x69
    };


};
