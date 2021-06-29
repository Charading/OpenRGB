/*-----------------------------------------*\
|  HPOmen30LController.cpp                  |
|                                           |
|  Driver for HP Omen 30L RGB lighting      |
|  controller                               |
|                                           |
\*-----------------------------------------*/

#include "HPOmen30LController.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
HPOmen30LController::HPOmen30LController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;

    strcpy(device_name, "HP Omen 30L");

    fan.mode      = HP_OMEN_30L_STATIC;
    fan.speed     = HP_OMEN_30L_SPEED_MED;
    fan.intensity = 0x64;

    logo.mode      = HP_OMEN_30L_STATIC;
    logo.speed     = HP_OMEN_30L_SPEED_MED;
    logo.intensity = 0x64;

    bar.mode      = HP_OMEN_30L_STATIC;
    bar.speed     = HP_OMEN_30L_SPEED_MED;
    bar.intensity = 0x64;

    cpu.mode      = HP_OMEN_30L_STATIC;
    cpu.speed     = HP_OMEN_30L_SPEED_MED;
    cpu.intensity = 0x64;     

}

HPOmen30LController::~HPOmen30LController()
{
    hid_close(dev);
}

std::string HPOmen30LController::GetLocationString()
{
    return("HID: " + location);
}

char* HPOmen30LController::GetDeviceName()
{
    return device_name;
}

std::string HPOmen30LController::GetSerialString()
{
    std::string ret_string = "";
    return(ret_string);
}

std::string HPOmen30LController::GetEffectChannelString(unsigned char channel)
{
    std::string ret_string = "";
    return(ret_string);
}

std::string HPOmen30LController::GetFirmwareVersionString()
{
    std::string ret_string = "";
    return(ret_string);
}

void HPOmen30LController::SetZoneMode(int zone,unsigned char mode, unsigned char speed,unsigned char intensity)
{
    switch(zone)
    {
        case HP_OMEN_30L_LOGO_ZONE:
            logo.mode      = mode;
            logo.speed     = speed;
            logo.intensity = intensity;
            break;
        case HP_OMEN_30L_BAR_ZONE:
            bar.mode      = mode;
            bar.speed     = speed;
            bar.intensity = intensity;
            break;
        case HP_OMEN_30L_FAN_ZONE:
            fan.mode      = mode;
            fan.speed     = speed;
            fan.intensity = intensity;
            break;
        case HP_OMEN_30L_CPU_ZONE:
            cpu.mode      = mode;
            cpu.speed     = speed;
            cpu.intensity = intensity;
            break;
    }
}

void HPOmen30LController::SetZoneColor(int zone, std::vector<RGBColor> colors)
{
    switch(zone)
    {
        case HP_OMEN_30L_LOGO_ZONE:
            SendZoneUpdate(HP_OMEN_30L_LOGO_ZONE, logo, colors);
            break;
        case HP_OMEN_30L_BAR_ZONE:
            SendZoneUpdate(HP_OMEN_30L_BAR_ZONE, bar, colors);
            break;
        case HP_OMEN_30L_FAN_ZONE:
            SendZoneUpdate(HP_OMEN_30L_FAN_ZONE, fan, colors);
            break;
        case HP_OMEN_30L_CPU_ZONE: 
            SendZoneUpdate(HP_OMEN_30L_CPU_ZONE, cpu, colors);
            break;
    }
    
}

void HPOmen30LController::SendZoneUpdate
    (
    unsigned char zone,
    hp_zone settings,
    std::vector<RGBColor> colors
    )
{
    unsigned char usb_buf[] =
    {
        0x00, 0x00,             // [0x00-0x01]
        0x12, 0x05, 0x00, 0x00, // [0x02-0x05]
        0x00, 0x00, 0x00, 0x00, // [0x06-0x09]
        0x00, 0x00, 0x00, 0x00, // [0x0A-0x0D]
        0x00, 0x00, 0x00, 0x00, // [0x0E-0x11]
        0x00, 0x00, 0x00, 0x00, // [0x12-0x15]
        0x00, 0x00, 0x00, 0x00, // [0x16-0x19]
        0x00, 0x00, 0x00, 0x00, // [0x1A-0x1D]
        0x00, 0x00, 0x00, 0x00, // [0x1E-0x21]
        0x00, 0x00, 0x00, 0x00, // [0x22-0x25]
        0x00, 0x00, 0x00, 0x00, // [0x26-0x29]
        0x00, 0x00, 0x00, 0x00, // [0x2A-0x2D]
        0x00, 0x00, 0x00, 0x00, // [0x2E-0x31]
        0x00, 0x00, 0x00, 0x00, // [0x32-0x35] Always 0x00*4
        0x00, 0x00, 0x00, 0x00  // [0x36-0x39] zone / 0x01 / theme / speed
    };
    usb_buf[0x36]   = zone;
    if( settings.mode != HP_OMEN_30L_DIRECT )
    {
        hid_write(dev, usb_buf, 58);
    }

    usb_buf[0x37]   = 0x01;
    usb_buf[0x39]   = settings.speed;
    usb_buf[0x03]   = settings.mode;
    usb_buf[0x30]   = settings.intensity;

    if( settings.mode == HP_OMEN_30L_DIRECT )
    {
        usb_buf[0x31]   = HP_OMEN_30L_DIRECT;
    }
    else
    {
        usb_buf[0x31]   = 0x0a;
    }

    if(settings.mode == HP_OMEN_30L_DIRECT | settings.mode == HP_OMEN_30L_STATIC)
    {
        //z1
        usb_buf[0x08]   = RGBGetRValue(colors[0]);
        usb_buf[0x09]   = RGBGetGValue(colors[0]);
        usb_buf[0x0A]   = RGBGetBValue(colors[0]);
        //z2
        usb_buf[0x0B]   = RGBGetRValue(colors[1]);
        usb_buf[0x0C]   = RGBGetGValue(colors[1]);
        usb_buf[0x0D]   = RGBGetBValue(colors[1]);
        //z3
        usb_buf[0x0E]   = RGBGetRValue(colors[2]);
        usb_buf[0x0F]   = RGBGetGValue(colors[2]);
        usb_buf[0x10]   = RGBGetBValue(colors[2]);
        //z4
        usb_buf[0x11]   = RGBGetRValue(colors[3]);
        usb_buf[0x12]   = RGBGetGValue(colors[3]);
        usb_buf[0x13]   = RGBGetBValue(colors[3]);
        hid_write(dev, usb_buf, 58);
    }
    else
    {
        usb_buf[0x04]=colors.size();
        for(int i=0; i<colors.size();i++)
        {
            usb_buf[0x05]   = i+1;
        
            usb_buf[0x08] = usb_buf[0x0B] = usb_buf[0x0E] = usb_buf[0x11] = RGBGetRValue(colors[i]);
            usb_buf[0x09] = usb_buf[0x0C] = usb_buf[0x0F] = usb_buf[0x12] = RGBGetGValue(colors[i]);
            usb_buf[0x0A] = usb_buf[0x0D] = usb_buf[0x10] = usb_buf[0x13] = RGBGetBValue(colors[i]);
            usb_buf[0x36]   = zone;
            hid_write(dev, usb_buf, 58);
        }
    }
}


