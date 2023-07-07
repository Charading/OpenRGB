/*---------------------------------------------------------------------*\
|  BloodyKeyboardController.h                                           |
|                                                                       |
|  Driver for BloodyKeyboard USB Controller                             |
|                                                                       |
|  Vladisalv K. (kaluginvlad)       06 Jun 2023                         |
|                                                                       |
\*---------------------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi/hidapi.h>
#include "LogManager.h"
#include "RGBController.h"

#define HID_MAX_STR 255

#define BLOODYKBD_WRITE_PACKET_SIZE             64
#define BLOODYKBD_COLOR_PACKET_INDEX_OFFSET     0x04
#define BLOODYKBD_COLOR_PACKET_START_OFFSET     0x06

const uint8_t BLOODYKBD_INIT_DATA[][BLOODYKBD_WRITE_PACKET_SIZE] = {
   { 0x7, 0x1f },
   { 0x7, 0x5 },
   { 0x7, 0x29 },
   { 0x7, 0x7 },
   { 0x7, 0x2a },
   { 0x7, 0x29 },
   { 0x7, 0x1e, 0x1 },
   { 0x7, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 },
   { 0x7, 0x2f, 0x0, 0x2e },
   { 0x7, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x80 },
   { 0x7, 0x3, 0x6, 0x5 },
   { 0x7, 0x6 },
   { 0x7, 0x3, 0x6, 0x1 },
   { 0x7, 0x3, 0x6, 0x1 },
   { 0x7, 0x3, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 }
};

const uint8_t BLOODYKBD_LAST_PACKET[BLOODYKBD_WRITE_PACKET_SIZE] = { 0x7, 0x5 };

/*-----------------------------------------------------*\
| Keyboard product IDs                                   |
\*-----------------------------------------------------*/
#define BLOODY_B820R_PID                0xFA10

class BloodyKeyboardController
{
public:
    BloodyKeyboardController();

    BloodyKeyboardController(hid_device* dev_handle, const char* path, uint16_t product_id);
    ~BloodyKeyboardController();

    uint16_t        GetPid();
    std::string     GetSerial();
    std::string     GetLocation();

    void            SetLedsDirect(std::vector<RGBColor> colors);

private:
    uint16_t        pid;
    std::string     location;
    hid_device*     dev;

    uint8_t color_data_buffer[6][BLOODYKBD_WRITE_PACKET_SIZE] = {
        { 0x07, 0x03, 0x06, 0x07 }, // R
        { 0x07, 0x03, 0x06, 0x08 }, // R
        { 0x07, 0x03, 0x06, 0x09 }, // G
        { 0x07, 0x03, 0x06, 0x0a }, // G
        { 0x07, 0x03, 0x06, 0x0b }, // B
        { 0x07, 0x03, 0x06, 0x0c }, // B
    };

    void            InitDevice();
    void            SetKeyColor(int keyId, uint8_t R, uint8_t G, uint8_t B);
    void            SendKeyColor();

};

