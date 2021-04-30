/*-------------------------------------------------------------------*\
|  CMARGBController.h                                                 |
|                                                                     |
|  Driver for Coolermaster RGB USB Controller                         |
|                                                                     |
|  Nic W (midgetspy)        13th Apr 2021                             |
|                                                                     |
|  Simple RGB device with 5 modes                                     |
|                                                                     |
\*-------------------------------------------------------------------*/

#include <string>
#include <hidapi/hidapi.h>
#include "RGBController.h"

#pragma once
#define CM_RGBC_NUM_LEDS 4

#define CM_RGBC_PACKET_SIZE 64
#define CM_RGBC_PACKET_OFFSET_MODE 0x04
#define CM_RGBC_PACKET_OFFSET_SPEED 0x05
#define CM_RGBC_PACKET_OFFSET_BRIGHTNESS 0x09
#define CM_RGBC_PACKET_OFFSET_COLOUR_1 0x0A
#define CM_RGBC_PACKET_OFFSET_COLOUR_2 0x0D

#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 0x04
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 0x07
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 0x0A
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 0x0D

#define CM_RGBC_INTERRUPT_TIMEOUT 250

#define CM_RGBC_SPEED_NONE 0x05
#define CM_RGBC_BRIGHTNESS_OFF 0x03

// MODES
enum
{
    CM_RGBC_MODE_STATIC          = 0x00,
    CM_RGBC_MODE_BREATHING       = 0x01,
    CM_RGBC_MODE_COLOR_CYCLE     = 0x02,
    CM_RGBC_MODE_STAR            = 0x03,
    CM_RGBC_MODE_MULTIPLE        = 0x04,
    CM_RGBC_MODE_OFF             = 0xFE,
};

// SPEED
enum
{
    CM_RGBC_SPEED_SLOWEST        = 0x00,
    CM_RGBC_SPEED_SLOW           = 0x00,
    CM_RGBC_SPEED_NORMAL         = 0x00,
    CM_RGBC_SPEED_FAST           = 0x00,
    CM_RGBC_SPEED_FASTEST        = 0x04,
};
static unsigned char CM_RGBC_SPEED_BREATHING[5]   { 0x3C, 0x37, 0x31, 0x2C, 0x26 };
static unsigned char CM_RGBC_SPEED_STAR[5]        { 0x46, 0x41, 0x3C, 0x37, 0x32 };
static unsigned char CM_RGBC_SPEED_COLOR_CYCLE[5] { 0x96, 0x8C, 0x80, 0x6E, 0x68 };


// BRIGHTNESS
// no idea what uses this yet
enum
{
    CM_RGBC_BRIGHTNESS_MIN       = 0x00,
    CM_RGBC_BRIGHTNESS_MAX       = 0x04,
};
static unsigned char CM_RGBC_BRIGHTNESS[5]        { 0x3F, 0x5A, 0x7F, 0x99, 0xFF };

class CMRGBController
{
public:
    CMRGBController(hid_device* dev_handle, char *_path);
    ~CMRGBController();

    std::string GetDeviceName();
    std::string GetSerial();
    std::string GetLocation();

    void                GetStatus();
    unsigned char       GetMode();
    void                SetMode(unsigned char mode, unsigned char speed, RGBColor colour);
    void                SetLedsDirect(RGBColor *led_colours, unsigned int led_count);
private:
    std::string         device_name;
    std::string         serial;
    std::string         location;
    hid_device*         dev;

    unsigned char       current_mode;
    unsigned char       current_speed;
    unsigned char       current_red;
    unsigned char       current_green;
    unsigned char       current_blue;
    unsigned char       current_brightness;

    unsigned char GetSpeedValue(unsigned char mode, unsigned char speed);
    unsigned char GetBrightnessValue(unsigned char mode, unsigned char brightness);
    unsigned char** CreatePacketsForSimpleModes(unsigned char mode, unsigned char speed, RGBColor colour, bool random_colour);
};
