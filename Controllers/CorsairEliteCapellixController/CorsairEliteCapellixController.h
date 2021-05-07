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

#define CORSAIR_CAPELLIX_PACKET_SIZE            1025
#define CORSAIR_CAPELLIX_PREAMBLE_OFFSET        10
#define CORSAIR_CAPELLIX_PUMP_LED_OFFSET        87
#define CORSAIR_CAPELLIX_QL_FAN_ZONE_OFFSET     102

enum
{
    CORSAIR_CAPELLIX_MODE_DIRECT         = 0x00,
};

class CorsairEliteCapellixController
{
public:
    //CorsairCapellixController(libusb_device_handle* dev_handle);
    CorsairEliteCapellixController(hid_device* dev_handle, const char* path);
    ~CorsairEliteCapellixController();

    void              SetDirectColor
                          (
                              std::vector<RGBColor>
                          );
    void              KeepaliveThread();
    std::vector<int>  DetectRGBFans();

private:
    hid_device*             dev;
    std::thread*            keepalive_thread;
    std::atomic<bool>       keepalive_thread_run;
    std::atomic<bool>       fan_mode_set;
    std::vector<int>        fanleds;
    std::chrono::time_point<std::chrono::steady_clock> last_commit_time;
    void            SendMultiPkt
                        (
                            unsigned char buffarray[][5],
                            int r,
                            int c
                        );
    void            SendCommit();
    void            SetFanMode();
};
