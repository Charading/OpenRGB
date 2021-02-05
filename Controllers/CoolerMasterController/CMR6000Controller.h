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
    CM_MR6000_MODE_COLOR_CYCLE   = 0x02, //Color cycle
    CM_MR6000_MODE_BREATHE       = 0x03, //Color cycle
};

enum
{
    MR6000_CYCLE_SPEED_SLOWEST              = 0x96, /* Slowest speed                */
    MR6000_CYCLE_SPEED_SLOW                 = 0x8C, /* Slow speed                   */
    MR6000_CYCLE_SPEED_NORMAL               = 0x80, /* Normal speed                 */
    MR6000_CYCLE_SPEED_FAST                 = 0x6E, /* Fast speed                   */
    MR6000_CYCLE_SPEED_FASTEST              = 0x68, /* Fastest speed                */

    MR6000_BREATHE_SPEED_SLOWEST            = 0x3C, /* Slowest speed                */
    MR6000_BREATHE_SPEED_SLOW               = 0x37, /* Slow speed                   */
    MR6000_BREATHE_SPEED_NORMAL             = 0x31, /* Normal speed                 */
    MR6000_BREATHE_SPEED_FAST               = 0x2C, /* Fast speed                   */
    MR6000_BREATHE_SPEED_FASTEST            = 0x26, /* Fastest speed                */
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
    void SetMode(unsigned char mode, unsigned char speed, unsigned char red, unsigned char green, unsigned char blue);

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
