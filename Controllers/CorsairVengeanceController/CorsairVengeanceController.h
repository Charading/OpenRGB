/*-----------------------------------------*\
|  CorsairVengeanceController.h             |
|                                           |
|  Definitions and types for Corsair        |
|  Vengeance RGB RAM lighting controller    |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/8/2019    |
\*-----------------------------------------*/

#include <string>
#include "i2c_smbus.h"

#pragma once

typedef unsigned char	corsair_dev_id;
typedef unsigned char   corsair_cmd;

enum
{
    CORSAIR_VENGEANCE_RGB_CMD_FADE_TIME = 0xA4,     /* Fade Time, 0 for Static              */
    CORSAIR_VENGEANCE_RGB_CMD_HOLD_TIME = 0xA5,     /* Hold Time                            */
    CORSAIR_VENGEANCE_RGB_CMD_MODE      = 0xA6,     /* Mode Control Value                   */
    CORSAIR_VENGEANCE_RGB_CMD_RED_VAL   = 0xB0,     /* Red Color Value                      */
    CORSAIR_VENGEANCE_RGB_CMD_GREEN_VAL = 0xB1,     /* Green Color Value                    */
    CORSAIR_VENGEANCE_RGB_CMD_BLUE_VAL  = 0xB2,     /* Blue Color Value                     */
};

enum
{
    CORSAIR_VENGEANCE_RGB_MODE_SINGLE   = 0x00,     /* Single Color Effect Mode             */
    CORSAIR_VENGEANCE_RGB_MODE_FADE     = 0x01,     /* Fade Through Colors                  */
    CORSAIR_VENGEANCE_RGB_MODE_PULSE    = 0x02,     /* Pulse Through Colors                 */
    CORSAIR_NUMBER_MODES                            /* Number of Corsair modes              */
};

class CorsairVengeanceController
{
public:
    CorsairVengeanceController(i2c_smbus_interface* bus, corsair_dev_id dev);
    ~CorsairVengeanceController();

    std::string     GetDeviceName();
    std::string     GetDeviceLocation();
    void            SetMode(unsigned char mode);

    size_t          GetLEDCount();
    void            SetAllColors(unsigned char red, unsigned char green, unsigned char blue);
    void            SetLEDColor(unsigned int led, unsigned char red, unsigned char green, unsigned char blue);
    void            ApplyColors();
    bool            WaitReady();

private:
    static constexpr size_t CORSAIR_VEN_LED_COUNT = 10;
    unsigned char led_data[CORSAIR_VEN_LED_COUNT * 3 + 2 + 6];

    char                    device_name[32];

    i2c_smbus_interface *   bus;
    corsair_dev_id          dev;

    static unsigned char crc8(unsigned char init, unsigned char poly, unsigned char *data, unsigned char len);
};
