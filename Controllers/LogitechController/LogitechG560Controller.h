/*-----------------------------------------*\
|  LogitechG560Controller.h                 |
|                                           |
|  Definitions and types for Logitech G560  |
|  RGB Speaker                              |
|                                           |
|  Cheerpipe     10/28/2020                 |
|  Based on                                 |
|  TheRogueZeta   8/31/2020                 |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    LOGITECH_G560_MODE_OFF                      = 0x00,
    LOGITECH_G560_MODE_STATIC                   = 0x01,
};

class LogitechG560Controller
{
public:
    LogitechG560Controller(hid_device* dev_handle);
    ~LogitechG560Controller();

    void        SendSpeakerMode
                   (
                    unsigned char       zone,           //0x04
                    unsigned char       mode,           //0x05
                    unsigned char       red,            //0x06
                    unsigned char       green,          //0x07
                    unsigned char       blue            //0x08
                   );

private:
    hid_device*             dev;
};
