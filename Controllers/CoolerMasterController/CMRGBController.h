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

#define REPORT_ID_OFFSET                        1
#define CM_RGBC_PACKET_SIZE                     64 + REPORT_ID_OFFSET //This needs to have one byte extra otherwise it won't work.
#define CM_RGBC_PACKET_OFFSET_OP                0x00
#define CM_RGBC_PACKET_OFFSET_TYPE              0x01
#define CM_RGBC_PACKET_OFFSET_MODE              0x04
#define CM_RGBC_PACKET_OFFSET_SPEED             0x05
#define CM_RGBC_PACKET_OFFSET_BRIGHTNESS        0x09
#define CM_RGBC_PACKET_OFFSET_COLOUR_1          0x0A
#define CM_RGBC_PACKET_OFFSET_COLOUR_2          0x0D

#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 0x04
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 0x07
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 0x0A
#define CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 0x0D

#define CM_RGBC_INTERRUPT_TIMEOUT 250

#define CM_RGBC_SPEED_NONE      0x05
#define CM_RGBC_BRIGHTNESS_OFF  0x03

// OP OPCODES
enum
{
    CM_RGBC_OPCODE_OP_FLOW_CONTROL  = 0x41,
    CM_RGBC_OPCODE_OP_UNKNOWN_50    = 0x50,
    CM_RGBC_OPCODE_OP_WRITE         = 0x51,
    CM_RGBC_OPCODE_OP_READ          = 0x52,
};

// CONTROL FLOW OPCODES
enum
{
    CM_RGBC_OPCODE_FLOW_00  = 0x00,
    CM_RGBC_OPCODE_FLOW_01  = 0x01,
    CM_RGBC_OPCODE_FLOW_80  = 0x80,
};

// OP TYPE OPCODES
enum
{
    CM_RGBC_OPCODE_TYPE_MODE                = 0x28,
    CM_RGBC_OPCODE_TYPE_UNKNOWN_2B          = 0x2B,
    CM_RGBC_OPCODE_TYPE_UNKNOWN_2C          = 0x2C,
    CM_RGBC_OPCODE_TYPE_UNKNOWN_30          = 0x30,
    CM_RGBC_OPCODE_TYPE_UNKNOWN_55          = 0x55,
    CM_RGBC_OPCODE_TYPE_LED_INFO            = 0xA8,
};

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
    CM_RGBC_SPEED_BREATHING_SLOWEST     = 0x3C,
    CM_RGBC_SPEED_BREATHING_FASTEST     = 0x26,
    CM_RGBC_SPEED_COLOR_CYCLE_SLOWEST   = 0x96,
    CM_RGBC_SPEED_COLOR_CYCLE_FASTEST   = 0x68,
    CM_RGBC_SPEED_STAR_SLOWEST          = 0x46,
    CM_RGBC_SPEED_STAR_FASTEST          = 0x32,
};

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
    unsigned char       GetSpeed();
    unsigned char       GetBrightness();
    RGBColor            GetModeColor();
    RGBColor            GetPort1Color();
    RGBColor            GetPort2Color();
    RGBColor            GetPort3Color();
    RGBColor            GetPort4Color();
    void                SetMode(unsigned char mode, unsigned char speed, unsigned char brightness, RGBColor colour);
    void                SetLedsDirect(RGBColor color1, RGBColor color2, RGBColor color3, RGBColor color4);
private:
    std::string         device_name;
    std::string         serial;
    std::string         location;
    hid_device*         dev;

    unsigned char       current_mode;
    unsigned char       current_speed;
    unsigned char       current_brightness;
    RGBColor            current_mode_color;
    RGBColor            current_port1_color;
    RGBColor            current_port2_color;
    RGBColor            current_port3_color;
    RGBColor            current_port4_color;

    void                Send_Flow_Control(unsigned char byte_flag);
    void                Send_Apply();
};
