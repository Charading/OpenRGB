/*-----------------------------------------*\
|  LogitechGLightsyncController.h           |
|                                           |
|  Definitions and types for Logitech       |
|  Lightsync lighting controllers           |
|                                           |
|  TheRogueZeta   04/21/2021                |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    LOGITECH_G_LIGHTSYNC_MODE_OFF               = 0x00,
    LOGITECH_G_LIGHTSYNC_MODE_STATIC            = 0x01,
    LOGITECH_G_LIGHTSYNC_MODE_CYCLE             = 0x02,
    LOGITECH_G_LIGHTSYNC_MODE_BREATHING         = 0x03,
};

/*---------------------------------------------------------------------------------------------*\
| Speed is 1000 for fast and 20000 for slow.                                                    |
| Values are multiplied by 100 later to give lots of GUI steps.                                 |
\*---------------------------------------------------------------------------------------------*/
enum
{   
    LOGITECH_G_LIGHTSYNC_SPEED_SLOWEST          = 0xC8,  /* Slowest speed                   */
    LOGITECH_G_LIGHTSYNC_SPEED_NORMAL           = 0x32,  /* Normal speed                    */
    LOGITECH_G_LIGHTSYNC_SPEED_FASTEST          = 0x0A,  /* Fastest speed                   */
};

class LogitechGLightsyncController
{
public:
    LogitechGLightsyncController(
            hid_device* dev_cmd_handle,
            hid_device* dev_handle, 
            const char* path, 
            unsigned char hid_dev_index, 
            unsigned char hid_feature_index, 
            unsigned char hid_fctn_ase_id,
            bool requireLock);
    ~LogitechGLightsyncController();

    std::string GetDeviceLocation();
    std::string GetSerialString();

    void        UpdateMouseLED
                   (
                    unsigned char       mode,
                    unsigned short      speed,
                    unsigned char       zone,
                    unsigned char       red,
                    unsigned char       green,
                    unsigned char       blue,
                    unsigned char       brightness
                   );
    void        SetDirectMode(bool direct);

private:
    hid_device*             dev;
    hid_device*             cmd_dev;
    std::string             location;
    unsigned char           dev_index;
    unsigned char           feature_index;
    unsigned char           fctn_ase_id;
    bool                    direct_state;
    static                  std::mutex mutex;
    bool                    lock;
};
