/*---------------------------------------------------------*\
|  BlinkStickController.h                                   |
|                                                           |
| Generic Physiqual Interface for BlinkStick LED controller |
|                                                           |
|  Foxdogface(https://github.com/Foxdogface), 29/11/2022    |
\*---------------------------------------------------------*/

#include <string>
#include <hidapi/hidapi.h>
#include "LogManager.h"
#include "RGBController.h"

/*-----------------------------------------------------*\
| BlinkSticks modes IDs                                 |
\*-----------------------------------------------------*/
#define RGB_MODE    0
#define ARGB_MODE   2

/*-----------------------------------------------------*\
| BlinkSticks USB constants                             |
\*-----------------------------------------------------*/
#define BLINCK_STICK_SERIAL_SIZE        13
#define BLINCK_STICK_WRITE_PACKET_SIZE  4
#define MAX_BUFFER_SIZE                 64
#define COLOR_DATA_PACKET_SIZE          64*3 + 2

/*-----------------------------------------------------*\
| BlinkSticks types IDs                                 |
\*-----------------------------------------------------*/
#define UNKNOWN                 0
#define BLINKSTICK              1
#define BLINKSTICK_PRO          2
#define BLINKSTICK_STRIP        3
#define BLINKSTICK_SQUARE       4
#define BLINKSTICK_NANO         5
#define BLINKSTICK_FLEX         6

class BlinkStickController
{
public:
    BlinkStickController(hid_device* dev_handle, const char* path, uint16_t product_id);
    ~BlinkStickController();

    uint16_t        GetPid();
    std::string     GetSerial();
    std::string     GetLocation();
    std::string     GetName();
    std::string     GetDescription();
    uint8_t         generation;

    void            SetMode(uint8_t mode);
    void            SetRGBLeds(std::vector<RGBColor> colors);
    void            SetARGBLedsDirect(int channel, std::vector<RGBColor>* colors);
    void            SetSingleLed(std::vector<RGBColor> colors, int index);
private:
    uint16_t        pid;
    std::string     location;
    hid_device*     dev;
    std::string     GetDeviceString(int index);

    void            InitDevice();

    uint8_t         DetermineReportID(int leds);
};
