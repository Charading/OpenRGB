/*---------------------------------------------------------*\
|  BlinkyTapeController.cpp                                 |
|                                                           |
|  BlinkyTape controller interface                          |
|                                                           |
|  Matt Mets (matt@blinkinlabs.com), 07/01/2021             |
\*---------------------------------------------------------*/

#include "BlinkyTapeController.h"

#include <fstream>
#include <iostream>
#include <string>

#ifndef WIN32
#define LPSTR           char *
#define strtok_s        strtok_r
#endif

BlinkyTapeController::BlinkyTapeController()
{
}


BlinkyTapeController::~BlinkyTapeController()
{
    if(serialport != NULL)
    {
        delete serialport;
    }
}

void BlinkyTapeController::Initialize(char* ledstring)
{
    LPSTR   numleds = NULL;
    LPSTR   next = NULL;

    port_name = strtok_s(ledstring, ",", &next);

    //Check for the number of LEDs
    if (strlen(next))
    {
        numleds = strtok_s(next, ",", &next);
    }

    if (numleds != NULL && strlen(numleds))
    {
        num_leds = atoi(numleds);
    }

    serialport = new serial_port();

    if(!serialport->serial_open(port_name.c_str(), 115200)) {
        delete serialport;
    }
}



std::string BlinkyTapeController::GetLocation()
{
    if(serialport != NULL)
    {
        return("COM: " + port_name);
    }
    else
    {
        return("");
    }
}

char* BlinkyTapeController::GetLEDString()
{
    return(led_string);
}

void BlinkyTapeController::SetLEDs(std::vector<RGBColor> colors)
{
    unsigned char *serial_buf;

    /*-------------------------------------------------------------*\
    | BlinkyTape Protocol                                           |
    |                                                               |
    |   Packet size: Number of data bytes + 1                       |
    |                                                               |
    |   0-n: Data Byte (0-254)                                      |
    |   n+1: Packet End Byte (0xFF)                                 |
    \*-------------------------------------------------------------*/
    unsigned int payload_size   = (colors.size() * 3);
    unsigned int packet_size    = payload_size + 1;

    serial_buf = new unsigned char[packet_size];

    /*-------------------------------------------------------------*\
    | Set up end byte                                               |
    \*-------------------------------------------------------------*/
    serial_buf[packet_size - 1] = 0xFF;

    /*-------------------------------------------------------------*\
    | Copy in color data in RGB order                               |
    \*-------------------------------------------------------------*/
    for(unsigned int color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        unsigned int color_offset = color_idx * 3;

        serial_buf[0x00 + color_offset]     = min(254,RGBGetRValue(colors[color_idx]));
        serial_buf[0x01 + color_offset]     = min(254,RGBGetGValue(colors[color_idx]));
        serial_buf[0x02 + color_offset]     = min(254,RGBGetBValue(colors[color_idx]));
    }

    /*-------------------------------------------------------------*\
    | Send the packet                                               |
    \*-------------------------------------------------------------*/
    if (serialport != NULL)
    {
        serialport->serial_write((char *)serial_buf, packet_size);
    }

    delete[] serial_buf;
}

