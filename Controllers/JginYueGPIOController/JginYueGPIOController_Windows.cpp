/*-----------------------------------------*\
|  JginYueGPIOController_Windows.cpp        |
|                                           |
|  Driver for JginYue internal USB          |
|  lighting controller                      |
|                                           |
|  Tong R    (tcr020)  2023/08/23           |
|  Liu  ShiMeng(Moon dream stars) 2003/08/23|
|                                           |
|        Dongguan Yonghang Electronic       |
|           Technology Co., Ltd             |
\*-----------------------------------------*/
#include <cstring>
#include <string>
#include <stdio.h>


#include "RGBController.h"
#include "ResourceManager.h"
#include "SettingsManager.h"
#include "JginYueGPIOController_Windows.h"
#include "dependencies/dmiinfo.h"
#include "JginYueGPIODrive_Windows.h"



JginYueGPIOController::JginYueGPIOController()
{

    DMIInfo         dmi;

    device_name   = "JGINYUE " + dmi.getMainboard();

    Init_device();
}

JginYueGPIOController::~JginYueGPIOController()
{

}
unsigned int JginYueGPIOController::GetZoneCount()
{
    return(JGINYUE_MAX_ZONES);
}

std::string JginYueGPIOController::GetDeviceLocation()
{
    return("Intel PCH");
}

std::string JginYueGPIOController::GetDeviceName()
{
    return(device_name);
}

std::string JginYueGPIOController::GetSerialString()
{
    return("");
}

std::string JginYueGPIOController::GetDeviceFWVirson()
{
    return("");
}


void JginYueGPIOController::Init_device()
{
    GPIOdrive_API(GRB_buffer,JGINYUE_ADDRESSABLE_MAX_LEDS);
}

void JginYueGPIOController::DirectLEDControl
    (
    RGBColor*              colors,
    unsigned char          zone,
    unsigned char          num_LED
    )
{

    unsigned int r = 0, g = 0, b = 0, GRB = 0;
    for(unsigned int i =0;i<num_LED;i++)
    {
        r=RGBGetRValue(colors[i]);
        g=RGBGetGValue(colors[i]);
        b=RGBGetBValue(colors[i]);
        GRB=((unsigned int)((g << 16) | (r << 8) | (b)));
        GRB_buffer[i]=GRB;
    }
    GPIOdrive_API(GRB_buffer,num_LED);

}


void JginYueGPIOController::Area_resize(unsigned char led_numbers,unsigned char zone)
{

    for(unsigned int i = led_numbers; i < JGINYUE_ADDRESSABLE_MAX_LEDS; i++)
    {
        GRB_buffer[i]=0x00000000;
    }
    GPIOdrive_API(GRB_buffer,JGINYUE_ADDRESSABLE_MAX_LEDS);

}
