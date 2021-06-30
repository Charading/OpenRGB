/*---------------------------------------------------------*\
|  Definitions for Generic LED Strip Interface              |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef BLINKYTAPE_H
#define BLINKYTAPE_H

#include "RGBController.h"
#include "serial_port.h"
#include "net_port.h"
#include <vector>


#ifndef TRUE
#define TRUE true
#define FALSE false
#endif

#ifndef WIN32
#define LPSTR           char *
#define strtok_s        strtok_r
#endif

struct BlinkyTapeDevice
{
    std::string     port;
    unsigned int    num_leds;
};

class BlinkyTapeController
{
public:
    BlinkyTapeController();
    ~BlinkyTapeController();

    void        Initialize(char* ledstring);

    void        InitializeSerial(char* portname, int baud);

    char*       GetLEDString();
    std::string GetLocation();

    void        SetLEDs(std::vector<RGBColor> colors);

    int num_leds;

private:
    char led_string[1024];
    std::string port_name;
    serial_port *serialport;
};

#endif
