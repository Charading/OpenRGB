/*-----------------------------------------*\
|  JginYueGPIOController.h                  |
|                                           |
|  Driver for JginYue internal USB          |
|  lighting controller                      |
|                                           |
|                                           |
|                                           |
\*-----------------------------------------*/
#include "RGBController.h"
#include <string>
#include <Windows.h>

#pragma once
#define JGINYUE_MAX_ZONES               1
#define JGINYUE_ADDRESSABLE_MAX_LEDS    10
#define RGBtoGRB(rgb)   ((rgb >> 16) & 0x000000FF + (rgb<<8) & 0x00FF0000 + (rgb>>8) & 0x0000FF00)

enum
{
    JGINYUE_GPIO_MODE_OFF                       =0x10,
    JGINYUE_GPIO_MODE_STATIC                    =0x11,
    JGINYUE_GPIO_MODE_DIRECT                    =0x20,       /*Not the exact USB protrol  - but need a way to differentiate  */
};

class JginYueGPIOController
{
public:

    JginYueGPIOController(HMODULE hModule1);
    ~JginYueGPIOController();

    unsigned int                                GetZoneCount();
    std::string                                 GetDeviceLocation();
    std::string                                 GetDeviceName();
    std::string                                 GetSerialString();
    std::string                                 GetDeviceFWVirson();


    void DirectLEDControl
        (
        RGBColor*       colors,
        unsigned char   zone
        );

    void                                        Init_device();
    void                                        Area_resize(unsigned char led_numbers,unsigned char zone);



private:
    HMODULE                                     hModule = NULL;
    std::string                                 device_name;
};
