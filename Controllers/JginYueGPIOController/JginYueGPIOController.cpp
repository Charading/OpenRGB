/*-----------------------------------------*\
|  JginYueGPIOController.cpp                |
|                                           |
|  Driver for JginYue internal USB          |
|  lighting controller                      |
|                                           |
|                                           |
|                                           |
\*-----------------------------------------*/
#include <cstring>
#include <string>
#include <stdio.h>


#include "RGBController.h"
#include "ResourceManager.h"
#include "SettingsManager.h"
#include "JginYueGPIOController.h"
#include "dependencies/dmiinfo.h"

#define JGINYUE_USB_GENERAL_COMMAND_HEADER              0x01
#define JGINYUE_USB_LED_STRIPE_SET_COMMAND_HEADER       0x05
#define JGINYUE_USB_MODE_SET_COMMAND_HEADER             0x06
#define JGINYUE_USB_PER_LED_SET_COMMAND_HEADER          0x04

#define JGINYUE_USB_GET_FW_VERSION                      0xA0
#define JGINYUE_USB_GET_FW_REPLY                        0x5A
#define JGINYUE_RG_DEFAULT                              0x01
#define JGINYUE_RG_SWAP                                 0x00


JginYueGPIOController::JginYueGPIOController(HMODULE hModule1)
{

    DMIInfo         dmi;

    hModule       = hModule1;

    device_name   = "JGINYUE " + dmi.getMainboard();

    Init_device();
}

JginYueGPIOController::~JginYueGPIOController()
{
    FreeLibrary(hModule);
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

}

void JginYueGPIOController::DirectLEDControl
    (
    RGBColor*              colors,
    unsigned char          zone,
    unsigned char          num_LED
    )
{

}


void JginYueGPIOController::Area_resize(unsigned char led_numbers,unsigned char zone)
{

}
