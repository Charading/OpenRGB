/*-----------------------------------------*\
|  CorsairVoidProController.h               |
|                                           |
|  Driver for Corsair Void Pro lighting     |
|  controller - header file                 |
|                                           |
|  Guimard Morgan (morg) 2/08/2022          |
\*-----------------------------------------*/
#pragma once

#include "RGBController.h"
#include <string>
#include <hidapi/hidapi.h>



/*-----------------------------------------*\
|  Report Ids                               |
|                                           |
|  0xCB color control                       |
|  0xC8 lights on/off                       |
|  0xC9 battery status                      |
|  0xCA notification sound                  |
|  0xFF ??? not in used IPU                 |
\*-----------------------------------------*/
#define CORSAIR_VOID_PRO_PACKET_SIZE    20
#define CORSAIR_VOID_PRO_REPORT_ID      0xCB
#define CORSAIR_VOID_PRO_NUMBER_OF_LEDS 2

enum
{
    DIRECT_MODE_VALUE = 0x06
};


enum
{
    DIRECT_MODE_SEPARATOR_R1 = 0x1C,
    DIRECT_MODE_SEPARATOR_G1 = 0x16,
    DIRECT_MODE_SEPARATOR_B1 = 0x17,
    DIRECT_MODE_SEPARATOR_R2 = 0x1D,
    DIRECT_MODE_SEPARATOR_G2 = 0x18,
    DIRECT_MODE_SEPARATOR_B2 = 0x19
};


class CorsairVoidProController
{
public:
    CorsairVoidProController(hid_device* dev, const hid_device_info& info);
    ~CorsairVoidProController();

    std::string                 GetSerialString();
    std::string                 GetDeviceLocation();
    std::string                 GetFirmwareVersion();

    void                        SetDirect(std::vector<RGBColor> colors);    

protected:
    hid_device* dev;

private:

    std::string                 location;
    std::string                 serial_number;
    std::string                 version;

    void                        SendState(bool on);
};
