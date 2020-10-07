/*-----------------------------------------*\
|  WootingKeyboardController.h                |
|                                           |
|  Definitions and types for Wooting RGB      |
|  keyboard lighting controller             |
|                                           |
|  Adam Honse (CalcProgrammer1) 7/4/2020    |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum RGB_PARTS {
    PART0, PART1, PART2, PART3, PART4
};

class WootingKeyboardController
{
public:
    WootingKeyboardController(hid_device* dev_handle);
    ~WootingKeyboardController();

    void SendColors
    (
    unsigned char*  buf0,
    unsigned char*  buf1,
    unsigned char*  buf2,
    unsigned char*  buf3
    );

private:
    hid_device*             dev;

    void SendInitialize();

    bool wooting_usb_send_feature
            (
            unsigned char command,
            unsigned char param0,
            unsigned char param1,
            unsigned char param2,
            unsigned char param3
            );
    bool wooting_usb_send_buffer
            (
            RGB_PARTS part_number,
            unsigned char rgb_buffer[]
            );
};
