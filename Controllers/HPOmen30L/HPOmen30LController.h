/*-----------------------------------------*\
|  HPOmen30LController.h                    |
|                                           |
|  Driver for HP Omen 30L RGB lighting      |
|  controller                               |
|                                           |
\*-----------------------------------------*/

#include <string>
#include <hidapi/hidapi.h>
#include <vector>
typedef unsigned int RGBColor;

#define RGBGetRValue(rgb)   (rgb & 0x000000FF)
#define RGBGetGValue(rgb)   ((rgb >> 8) & 0x000000FF)
#define RGBGetBValue(rgb)   ((rgb >> 16) & 0x000000FF)

#pragma once

enum
{
    HP_OMEN_30L_STATIC      = 0x01, /* Static effect channel        */
    HP_OMEN_30L_BREATHING   = 0x06, /* Breathing effect channel     */
    HP_OMEN_30L_COLOR_CYCLE = 0x07, /* Color cycle effect channel   */
    HP_OMEN_30L_BLINKING    = 0x08, /* Led blink                    */

};

enum
{
    HP_OMEN_30L_SPEED_SLOW              = 0x01, /* Slow speed                   */
    HP_OMEN_30L_SPEED_MED               = 0x02, /* Normal speed                 */
    HP_OMEN_30L_SPEED_FAST              = 0x03, /* Fast speed                   */
};

enum
{
    HP_OMEN_30L_LOGO_ZONE            = 0x01,
    HP_OMEN_30L_BAR_ZONE             = 0x02,
    HP_OMEN_30L_FAN_ZONE             = 0x03,
    HP_OMEN_30L_CPU_ZONE             = 0x04,
};

class HPOmen30LController
{
public:
    HPOmen30LController(hid_device* dev_handle, const char* path);
    ~HPOmen30LController();

    char* GetDeviceName();

    std::string GetEffectChannelString(unsigned char channel);
    std::string GetFirmwareVersionString();
    std::string GetLocationString();
    std::string GetSerialString();

    void SetRingEffectChannel(unsigned char channel);

    void SetFanMode(unsigned char mode, unsigned char speed);
    //void SetFanColor(unsigned char red, unsigned char green, unsigned char blue);

    void SetLogoMode(unsigned char mode, unsigned char speed);
    //void SetLogoColor(unsigned char red, unsigned char green, unsigned char blue);

    void SetBarMode(unsigned char mode, unsigned char speed);
    //void SetBarColor(unsigned char red, unsigned char green, unsigned char blue);

    void SetCPUMode(unsigned char mode, unsigned char speed);
    //void SetCPUColor(unsigned char red, unsigned char green, unsigned char blue);
    void SetZoneStaticColor(int zone, unsigned char red, unsigned char green, unsigned char blue);
    void SetZonesDynamicColor(std::vector<RGBColor> colors);

private:
    char                    device_name[32];
    hid_device*             dev;
    std::string             location;

    unsigned char           current_fan_mode;
    unsigned char           current_fan_speed;
    bool                    current_fan_random_color;

    unsigned char           current_logo_mode;
    unsigned char           current_logo_speed;
    bool                    current_logo_random_color;

    unsigned char           current_bar_mode;
    unsigned char           current_bar_speed;
    bool                    current_bar_random_color;

    unsigned char           current_cpu_mode;
    unsigned char           current_cpu_speed;
    bool                    current_cpu_random_color;

    void SendZoneUpdate
        (
        unsigned char zone,
        unsigned char speed,
        unsigned char mode,
        unsigned char brightness,
        unsigned char red,
        unsigned char green,
        unsigned char blue
        );

};
