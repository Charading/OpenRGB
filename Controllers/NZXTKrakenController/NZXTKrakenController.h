/*---------------------------------------------------------*\
|  Definitions for NZXT Kraken                              |
|                                                           |
|  Martin Hartl (inlart), 04/04/2020                        |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include <vector>
#include <string>
#include <hidapi/hidapi.h>

enum NZXTKrakenChannel_t
{
    NZXT_KRAKEN_CHANNEL_SYNC           = 0x00, /* Sync Channel                 */
    NZXT_KRAKEN_CHANNEL_LOGO           = 0x01, /* Logo Channel                 */
    NZXT_KRAKEN_CHANNEL_RING           = 0x02, /* Ring Channel                 */
};

enum
{
    NZXT_KRAKEN_MODE_FIXED              = 0x00, /* Fixed colors mode            */
    NZXT_KRAKEN_MODE_FADING             = 0x01, /* Fading mode                  */
    NZXT_KRAKEN_MODE_SPECTRUM           = 0x02, /* Spectrum cycle mode          */
    NZXT_KRAKEN_MODE_MARQUEE            = 0x03, /* Marquee mode                 */
    NZXT_KRAKEN_MODE_COVER_MARQUEE      = 0x04, /* Cover marquee mode           */
    NZXT_KRAKEN_MODE_ALTERNATING        = 0x05, /* Alternating mode             */
    NZXT_KRAKEN_MODE_BREATHING          = 0x06, /* Breathing mode               */
    NZXT_KRAKEN_MODE_PULSE              = 0x07, /* Pulse mode                   */
    NZXT_KRAKEN_MODE_TAI_CHI            = 0x08, /* Tai Chi mode                 */
    NZXT_KRAKEN_MODE_WATER_COOLER       = 0x09, /* Water color mode             */
    NZXT_KRAKEN_MODE_LOADING            = 0x0a, /* Loading mode                 */
    NZXT_KRAKEN_MODE_WINGS              = 0x0c, /* Wings mode                   */
};

enum
{
    NZXT_KRAKEN_SPEED_SLOWEST           = 0x00, /* Slowest speed                */
    NZXT_KRAKEN_SPEED_SLOW              = 0x01, /* Slow speed                   */
    NZXT_KRAKEN_SPEED_NORMAL            = 0x02, /* Normal speed                 */
    NZXT_KRAKEN_SPEED_FAST              = 0x03, /* Fast speed                   */
    NZXT_KRAKEN_SPEED_FASTEST           = 0x04, /* Fastest speed                */
};

enum {
    NZXT_KRAKEN_CHANNEL_FAN             = 0x80,
    NZXT_KRAKEN_CHANNEL_PUMP            = 0xc0,
};

class NZXTKrakenController
{
public:
    NZXTKrakenController(hid_device* dev_handle, const char* path);
    ~NZXTKrakenController();

    std::string GetFirmwareVersion();
    std::string GetLocation();
    std::string GetSerialString();

    void UpdateStatus();
    void UpdateEffect
        (
        NZXTKrakenChannel_t channel,
        unsigned char mode,
        bool direction,
        unsigned char speed,
        int seq,
        std::vector<RGBColor> colors
        );

    void SetSpeed
        (
        unsigned char   channel,
        unsigned int    speed_cmd
        );

    unsigned int            fan_speed;
    unsigned int            pump_speed;

private:
    void SendEffect
        (
        unsigned char   channel,
        unsigned char   mode,
        bool            direction,
        unsigned char*  color_data,
        unsigned char   speed = 0x02,
        bool            movement = false,
        int             cis = 0 ,
        int             size = 0
        );

    void SetFixedSpeedProfile
        (
        unsigned char   channel,
        unsigned char   duty
        );
    void SetInstantaneousSpeed
        (
        unsigned char   channel,
        unsigned char   duty
        );

    hid_device*   dev;

    // -- status
    std::string             firmware_version;
    double                  liquid_temperature;
    std::string             location;

    bool                    supports_cooling_profiles;
};
