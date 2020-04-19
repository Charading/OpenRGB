/*-----------------------------------------*\
|  AuraAddressableController.h              |
|                                           |
|  Definitions and types for ASUS Aura      |
|  Addressable RGB lighting controller      |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/18/2020   |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    AURA_MODE_OFF                       = 0,        /* OFF mode                             */
    AURA_MODE_STATIC                    = 1,        /* Static color mode                    */
    AURA_MODE_BREATHING                 = 2,        /* Breathing effect mode                */
    AURA_MODE_FLASHING                  = 3,        /* Flashing effect mode                 */
    AURA_MODE_SPECTRUM_CYCLE            = 4,        /* Spectrum Cycle mode                  */
    AURA_MODE_RAINBOW                   = 5,        /* Rainbow effect mode                  */
    AURA_MODE_SPECTRUM_CYCLE_BREATHING  = 6,        /* Rainbow Breathing effect mode        */
    AURA_MODE_CHASE_FADE                = 7,        /* Chase with Fade effect mode          */
    AURA_MODE_SPECTRUM_CYCLE_CHASE_FADE = 8,        /* Chase with Fade, Rainbow effect mode */
    AURA_MODE_CHASE                     = 9,        /* Chase effect mode                    */
    AURA_MODE_SPECTRUM_CYCLE_CHASE      = 10,       /* Chase with Rainbow effect mode       */
    AURA_MODE_SPECTRUM_CYCLE_WAVE       = 11,       /* Wave effect mode                     */
    AURA_MODE_CHASE_RAINBOW_PULSE       = 12,       /* Chase with  Rainbow Pulse effect mode*/
    AURA_MODE_RANDOM_FLICKER            = 13,       /* Random flicker effect mode           */
    AURA_MODE_MUSIC                     = 14,       /* Music effect mode                    */
    AURA_MODE_DIRECT                    = 0xFF,     /* Direct control mode                  */
};

enum
{
    AURA_CONTROL_MODE_EFFECT            = 0x3B,     /* Effect control mode                  */
    AURA_CONTROL_MODE_DIRECT            = 0x40,     /* Direct control mode                  */
    AURA_REQUEST_FIRMWARE_VERSION       = 0x82,     /* Request firmware string              */
    AURA_REQUEST_CONFIG_TABLE           = 0xB0,     /* Request configuration table          */
};

class AuraAddressableController
{
public:
    AuraAddressableController(hid_device* dev_handle);
    ~AuraAddressableController();

    unsigned int GetChannelCount();

    std::string GetDeviceName();

    void SetChannelLEDs
        (
        unsigned char   channel,
        RGBColor *      colors,
        unsigned int    num_colors
        );

    void SetMode
        (
        unsigned char mode,
        unsigned char red,
        unsigned char grn,
        unsigned char blu
        );

private:
    char                    device_name[16];
    unsigned char           config_table[60];
    hid_device*             dev;
    unsigned int            led_count;
    
    void GetConfigTable();

    void GetFirmwareVersion();

    void SendEffect
        (
        unsigned char   channel,
        unsigned char   mode,
        unsigned char   red,
        unsigned char   grn,
        unsigned char   blu
        );

    void SendDirect
        (
        unsigned char   device,
        unsigned char   start_led,
        unsigned char   led_count,
        unsigned char*  led_data
        );

    void    SendDirectApply
                (
                unsigned char   channel
                );
};
