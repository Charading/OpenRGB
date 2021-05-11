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

#define CORSAIR_COMMANDER_CORE_PACKET_SIZE            1025
#define CORSAIR_COMMANDER_CORE_PREAMBLE_OFFSET        10
#define CORSAIR_ELITE_CAPELLIX_PUMP_LED_OFFSET        87
#define CORSAIR_QL_FAN_ZONE_OFFSET                    102

enum
{
    CORSAIR_COMMANDER_CORE_MODE_DIRECT         = 0x00,
};

class CorsairCommanderCoreController
{
public:
    //CorsairCapellixController(libusb_device_handle* dev_handle);
    CorsairCommanderCoreController(hid_device* dev_handle, const char* path);
    ~CorsairCommanderCoreController();

    void              SetDirectColor
                          (
                              std::vector<RGBColor>,
                              std::vector<zone>
                          );
    void              KeepaliveThread();
    std::vector<int>  DetectRGBFans();
    void              SetFanMode();
private:
    hid_device*             dev;
    std::thread*            keepalive_thread;
    std::atomic<bool>       keepalive_thread_run;
    std::atomic<bool>       send_keepalive;
    std::vector<int>        fanleds;
    std::chrono::time_point<std::chrono::steady_clock> last_commit_time;
    void            SendMultiPkt
                        (
                            unsigned char buffarray[][5],
                            int r,
                            int c
                        );
    void            SendMultiPktLg
                        (
                            unsigned char buffarray[][15],
                            int r,
                            int c
                        );
    void            SendCommit();
    void            InitController();

};
