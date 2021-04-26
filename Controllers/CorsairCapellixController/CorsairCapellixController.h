/*---------------------------------------------------------*\
|  Definitions for Corsair Capellix Series                  |
|  Based on code by:                                        |
|  Adam Honse (calcprogrammer1@gmail.com), 8/17/2020        |
\*---------------------------------------------------------*/

#include "RGBController.h"
#include <vector>
#include <chrono>
#include <hidapi/hidapi.h>

#pragma once

#define CORSAIR_CAPELLIX_PACKET_SIZE 1025
#define CORSAIR_CAPELLIX_RESPONSE_PACKET_SIZE 769

enum
{
    //byte 0x0A
    CORSAIR_CAPELLIX_SPEED_SLOW         = 0x03,
    CORSAIR_CAPELLIX_SPEED_MED          = 0x04,
    CORSAIR_CAPELLIX_SPEED_FAST         = 0x05,
};

enum
{
    //byte 0x0B
    CORSAIR_CAPELLIX_DIRECTION_RIGHT    = 0x04, //rainbow wave, color wave, sequential, visor, color warp
    CORSAIR_CAPELLIX_DIRECTION_LEFT     = 0x05, //rainbow wave, color wave, sequential, visor, color warp, marquee
    CORSAIR_CAPELLIX_DIRECTION_CW       = 0x06, //spiral rainbow, arc
    CORSAIR_CAPELLIX_DIRECTION_CCW      = 0x07, //spiral rainbow, arc
    CORSAIR_CAPELLIX_DIRECTION_INWARD   = 0x08, //heartbeat
    CORSAIR_CAPELLIX_DIRECTION_OUTWARD  = 0x00, //heartbeat
    CORSAIR_CAPELLIX_DIRECTION_RANDOM   = 0x01, //rain
};

enum
{
    //byte 0x09
    CORSAIR_CAPELLIX_COLOR_MODE_SETPATTERN   = 0x00, //rainbow wave, spiral rainbow, rainbow, color warp, static
    CORSAIR_CAPELLIX_COLOR_MODE_CONFIGURABLE = 0x01, //color shift, color pulse, color wave, sequential, strobing, visor, marquee, rain, arc, heartbeat
    CORSAIR_CAPELLIX_COLOR_MODE_RANDOM       = 0x02, //color shift, color pulse, color wave, sequential, strobing, visor, rain, arc, heartbeat

};

enum
{
    //byte 0x03
    CORSAIR_CAPELLIX_COLOR_CONFIG_NONE      = 0x08, //rainbow wave, spiral rainbow, rainbow, color shift, color pulse, color wave, sequential, strobing, visor, rain, color warp, arc, heartbeat
    CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE    = 0x0c, //sequential, marquee, static
    CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL      = 0x10, //color shift, color pulse, color wave, strobing, visor, rain, arc, heartbeat
};

enum
{
    CORSAIR_CAPELLIX_MODE_DIRECT         = 0x00,
    CORSAIR_CAPELLIX_MODE_STATIC         = 0x01,
    CORSAIR_CAPELLIX_MODE_RAINBOWWAVE    = 0x02,
    CORSAIR_CAPELLIX_MODE_SPIRALRAINBOW  = 0x03,
    CORSAIR_CAPELLIX_MODE_RAINBOW        = 0x04,
    CORSAIR_CAPELLIX_MODE_COLORSHIFT     = 0x05,
    CORSAIR_CAPELLIX_MODE_COLORPULSE     = 0x06,
    CORSAIR_CAPELLIX_MODE_COLORWAVE      = 0x07,
    CORSAIR_CAPELLIX_MODE_SEQUENTTIAL    = 0x08,
    CORSAIR_CAPELLIX_MODE_STROBING       = 0x09,
    CORSAIR_CAPELLIX_MODE_VISOR          = 0x0A,
    CORSAIR_CAPELLIX_MODE_MARQUEE        = 0x0B,
    CORSAIR_CAPELLIX_MODE_RAIN           = 0x0C,
    CORSAIR_CAPELLIX_MODE_COLORWARP      = 0x0D,
    CORSAIR_CAPELLIX_MODE_ARC            = 0x0E,
    CORSAIR_CAPELLIX_MODE_HEARTBEAT      = 0x0F,
};

class CorsairCapellixController
{
public:
    //CorsairCapellixController(libusb_device_handle* dev_handle);
    CorsairCapellixController(hid_device* dev_handle, const char* path);
    ~CorsairCapellixController();

    void              SetDirectMode();

    void              SetDirectColor
                          (
                              //unsigned char           red,
                              //unsigned char           grn,
                              //unsigned char           blu
                              std::vector<RGBColor>
                          );
    void              KeepaliveThread();
    void              StartKeepaliveThread();
    void              PauseKeepaliveThread();
    void              SendHWMode
                          (
                              unsigned int            mode,
                              unsigned int            speed,
                              unsigned int            direction,
                              unsigned int            colormode,
                              std::vector<RGBColor> & colors
                          );

    std::vector<int>  DetectFans();

private:
    hid_device*             dev;
    std::string             firmware_version;
    std::string             location;
    std::thread*            keepalive_thread;
    std::atomic<bool>       keepalive_thread_run;
    std::atomic<bool>       sendKeepalive;


    std::chrono::time_point<std::chrono::steady_clock> last_commit_time;

    void            SetColors
                        (
                            int colormode,
                            std::vector<RGBColor> & colors,
                            int numcolors,
                            unsigned char *usb_buf
                        );
    void            SetDirection
                        (
                            int mode,
                            int direction,
                            unsigned char *usb_buf
                        );
    void            SetHWMode();
    void            ExitDirectMode();
    void            SendMultiPkt
                        (
                            unsigned char buffarray[][5],
                            int r,
                            int c
                        );
    void            SendCommit();
};
