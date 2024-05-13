/*-----------------------------------------*\
|  CorsairVengeanceController.h             |
|                                           |
|  Driver for Corsair Vengeance RGB RAM     |
|  lighting controller                      |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/8/2019    |
\*-----------------------------------------*/

#include "CorsairVengeanceController.h"
#include <cstring>
using namespace std::chrono_literals;

CorsairVengeanceController::CorsairVengeanceController(i2c_smbus_interface* bus, corsair_dev_id dev)
{
    this->bus = bus;
    this->dev = dev;

    strcpy(device_name, "Corsair Vengeance RGB");
    led_data[0] = 0xc;
}

CorsairVengeanceController::~CorsairVengeanceController()
{

}

std::string CorsairVengeanceController::GetDeviceName()
{
    return(device_name);
}

std::string CorsairVengeanceController::GetDeviceLocation()
{
    std::string return_string(bus->device_name);
    char addr[5];
    snprintf(addr, 5, "0x%02X", dev);
    return_string.append(", address ");
    return_string.append(addr);
    return("I2C: " + return_string);
}


void CorsairVengeanceController::SetAllColors(unsigned char red, unsigned char green, unsigned char blue)
{
    for(unsigned int led = 0; led < CORSAIR_VEN_LED_COUNT; led++)
    {
        SetLEDColor(led, red, green, blue);
    }
}


void CorsairVengeanceController::SetLEDColor(unsigned int led, unsigned char red, unsigned char green, unsigned char blue)
{
    if (led >= CORSAIR_VEN_LED_COUNT)
    {
        return;
    }

    unsigned int offset     = (led * 3) + 1;
    led_data[offset]        = red;
    led_data[offset + 1]    = green;
    led_data[offset + 2]    = blue;
}

void CorsairVengeanceController::SetMode(unsigned char /*mode*/)
{
    bus->i2c_smbus_write_byte_data(dev, CORSAIR_VENGEANCE_RGB_CMD_MODE, CORSAIR_VENGEANCE_RGB_MODE_SINGLE);
}

size_t CorsairVengeanceController::GetLEDCount()
{
    return CORSAIR_VEN_LED_COUNT;
}

unsigned char CorsairVengeanceController::crc8
    (
    unsigned char   init,
    unsigned char   poly,
    unsigned char*  data,
    unsigned char   len
    )
{
    unsigned char crc = init;

    for(unsigned int i = 0; i < len; i++)
    {
        unsigned char val = data[i];
        for(unsigned char mask = 0x80; mask != 0; mask >>= 1)
        {
            unsigned char bit;
            if ((val & mask) != 0)
            {
                bit = (crc & 0x80) ^ 0x80;
            }
            else
            {
                bit = crc & 0x80;
            }

            if (bit == 0)
            {
                crc <<= 1;
            }
            else
            {
                crc = (crc << 1) ^ poly;
            }
        }
    }

    return crc;
}


void CorsairVengeanceController::ApplyColors()
{
    unsigned char data[sizeof(led_data)];
    memcpy(data, led_data, sizeof(led_data));
    unsigned char crc = crc8(0x0, 0x7, data, sizeof(data) - 1);
    data[sizeof(data) - 1] = crc;

    bus->i2c_smbus_write_block_data(dev, 0x31, 0x20, data);
    std::this_thread::sleep_for(800us);
    bus->i2c_smbus_write_block_data(dev, 0x32, sizeof(data) - 0x20, data + 0x20);
    std::this_thread::sleep_for(200us);
}

