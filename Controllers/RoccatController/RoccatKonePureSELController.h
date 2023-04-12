/*---------------------------------------------------------*\
|  Roccat Kone Pure SEL                                     |
|                                                           |
|  Warren Dumortier (nwarrenfl@gmail.com), 23/03/2023       |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include <hidapi/hidapi.h>

#define ROCCAT_KONE_PURE_SEL_PACKET_SIZE 9

enum
{
    ROCCAT_KONE_PURE_SEL_MODE_OFF,
    ROCCAT_KONE_PURE_SEL_MODE_BREATHING,
    ROCCAT_KONE_PURE_SEL_MODE_STATIC
};

class RoccatKonePureSELController
{
    public:
        RoccatKonePureSELController(hid_device* dev_handle, const hid_device_info& info);
        ~RoccatKonePureSELController();

        std::string     GetDeviceName();
        std::string     GetSerial();
        std::string     GetLocation();

        void            SetMode(int mode, std::vector<RGBColor> colors);
        void            SendUpdate();

    private:
        std::string     device_name;
        std::string     serial;
        std::string     location;
        hid_device*     dev;
        unsigned char   usb_packet_buf[ROCCAT_KONE_PURE_SEL_PACKET_SIZE];
};
