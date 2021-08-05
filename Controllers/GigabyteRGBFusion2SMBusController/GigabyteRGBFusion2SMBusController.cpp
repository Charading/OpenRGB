/*-----------------------------------------*\
|  GigabyteRGBFusion2SMBusController.cpp    |
|                                           |
|  Driver for Gigabyte Aorus RGB Fusion 2   |
|  SMBus lighting controller                |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/12/2020   |
|  Matt Harper                  5/5/2020    |
\*-----------------------------------------*/

#include "GigabyteRGBFusion2SMBusController.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#endif

RGBFusion2SMBusController::RGBFusion2SMBusController(i2c_smbus_interface* bus, rgb_fusion_dev_id dev, std::string mb_name)
{
    this->bus   = bus;
    this->dev   = dev;
    this->name  = mb_name;

    memset(led_data, 0, 10*16);

    led_count   = 10;	// Protocol supports 10 'slots'
}

RGBFusion2SMBusController::~RGBFusion2SMBusController()
{

}

unsigned int RGBFusion2SMBusController::GetLEDCount()
{
    return(led_count);
}

std::string RGBFusion2SMBusController::GetDeviceName()
{
    return(name);
}

std::string RGBFusion2SMBusController::GetDeviceLocation()
{
    std::string return_string(bus->device_name);
    char addr[5];
    snprintf(addr, 5, "0x%02X", dev);
    return_string.append(", address ");
    return_string.append(addr);
    return("I2C: " + return_string);
}

/* Writes are performed in 32 byte chunks. If we need to write the second 16 bytes,
* we must necessarily write the first 16 as well. Given that reading the existing state
* from the device is not yet possible, unfortunately we may overwrite existing device
* states if the state transition did not occur within in the same OpenRGB instance.
* That is to say, the current behavior is non-deal but the best we have
*/
void RGBFusion2SMBusController::WriteLED(int led)
{
    unsigned short register_offset = led / 2;	// Relying on integer division to truncate
    unsigned short write_register = RGB_FUSION_2_LED_START_ADDR + 2*register_offset;

    // Adjust if we are writing the second 16 bytes
    if(led % 2)
    {
        led -= 1;
    }

    #ifdef DEBUG
    std::cout << std::hex << write_register << "\t";
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)led_data[led+i][j] << " ";
        }
        std::cout << " ";
    }
    std::cout << std::endl;
    #endif

    bus->i2c_smbus_write_block_data(RGB_FUSION_2_SMBUS_ADDR, write_register, 32, led_data[led]);
}

void RGBFusion2SMBusController::Apply()
{
    // Protocol expects terminating sequence 0x01ff written to register 0x17
    bus->i2c_smbus_write_word_data(RGB_FUSION_2_SMBUS_ADDR,
                   RGB_FUSION_2_APPLY_ADDR,
                   RGB_FUSION_2_ACTION_APPLY);
}

void RGBFusion2SMBusController::SetLEDEffect
    (
    unsigned int    led,
    int             mode,
    unsigned int    speed,
    unsigned char   red,
    unsigned char   green,
    unsigned char   blue
    )
{
    led_data[led][RGB_FUSION_2_IDX_MODE]        = mode;
    led_data[led][RGB_FUSION_2_IDX_RED]         = red;
    led_data[led][RGB_FUSION_2_IDX_GREEN]       = green;
    led_data[led][RGB_FUSION_2_IDX_BLUE]        = blue;
    led_data[led][RGB_FUSION_2_IDX_BRIGHTNESS]  = 0x64;

    switch (mode)
    {
        case RGB_FUSION_2_MODE_PULSE:
        // Timer 1: On time
        // Timer 2: Off time
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x20 * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x03 * speed;
        led_data[led][RGB_FUSION_2_TIMER_2_LSB] = 0x20 * speed;
        led_data[led][RGB_FUSION_2_TIMER_2_MSB] = 0x03 * speed;
        break;

        case RGB_FUSION_2_MODE_DIGITAL_WAVE:
        // Timer 1: Wave Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x19 * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00; // I couldn't figure out what this does, but should be
        break;                                          // similar to Digital A

        case RGB_FUSION_2_MODE_DIGITAL_A:
        // Timer 1: Directly controls speed of LED sections
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x39 * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;  // Setting this to any number other than 0 slows down the effect
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x01;    // Doesn't do anything, but always defaults to 0x01
        break;

        case RGB_FUSION_2_MODE_DIGITAL_B:
        // Timer 1: Direct control of section speed and pulsing
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x2d * speed; // Main effect speed
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;         // Only sets to 0x01 in min speed. Makes light section blink for shorter periods
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;           // Changes between 0x01 and 0x04 along with speed. Controls light trail length.
        break;

        case RGB_FUSION_2_MODE_DIGITAL_C:
        // Timer 1: Effect Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x2d * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;  // Like Digital A, slows down the effect, but never sets to another number.
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;    // Fixed at 0x04. Does nothing.
        break;

        case RGB_FUSION_2_MODE_DIGITAL_D:
        // Timer 1: Effect Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x2d * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;  // Like Digital A, slows down the effect, but never sets to another number.
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;    // Fixed at 0x04. Does nothing.
        break;

        case RGB_FUSION_2_MODE_DIGITAL_E:
        // Timer 1: Effect Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x25 * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0;     // Like Digital A, slows down the effect, but never sets to another number.
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;    // Fixed at 0x04. Does nothing.
        break;

        case RGB_FUSION_2_MODE_DIGITAL_F:
        // Timer 1: Effect Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x2d * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;  // Like Digital A, slows down the effect, but never sets to another number.
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;    // Fixed at 0x04. Does nothing.
        break;

        case RGB_FUSION_2_MODE_DIGITAL_G:
        // Timer 1: Effect Speed
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x23 * speed;
        led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x00;  // Like Digital A, slows down the effect, but never sets to another number.
        led_data[led][RGB_FUSION_2_IDX_OPT_1] = 0x04;    // Fixed at 0x04. Does nothing.
        break;

        case RGB_FUSION_2_MODE_COLOR_CYCLE:
            // Timer 1: Cycle time
            led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x00;
            led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0x03 * speed;
            led_data[led][RGB_FUSION_2_IDX_OPT_1]   = 0x07;		// Number of colors to cycle through. Valid range [1-7]
            led_data[led][RGB_FUSION_2_IDX_OPT_2]   = 0x00;		// Color cycle, or color cycle and pulse. [0,1]
        break;

        case RGB_FUSION_2_MODE_FLASHING:
            /* Timer 1: On time
             * Timer 2: Interval
             * Timer 3: Cycle time */
        led_data[led][RGB_FUSION_2_TIMER_1_LSB] = 0x64;
                led_data[led][RGB_FUSION_2_TIMER_1_MSB] = 0;
                led_data[led][RGB_FUSION_2_TIMER_2_LSB] = 0xc8;
                led_data[led][RGB_FUSION_2_TIMER_2_MSB] = 0;
                led_data[led][RGB_FUSION_2_TIMER_3_LSB] = 0xd0;
                led_data[led][RGB_FUSION_2_TIMER_3_MSB] = 0x07;

            led_data[led][RGB_FUSION_2_IDX_OPT_1] = speed;		// Controls number of flashes
        break;
    }

    WriteLED(led);
}

