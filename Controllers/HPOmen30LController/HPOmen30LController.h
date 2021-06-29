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

struct hp_zone
{
    unsigned char  mode;
    unsigned char  speed;
    unsigned char  intensity;
};

enum
{
    HP_OMEN_30L_STATIC      = 0x01, /* Static effect channel        */
    HP_OMEN_30L_DIRECT      = 0x04, /* Direct for effects plugin    */
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
    void SetZoneMode(int zone,unsigned char mode, unsigned char speed,unsigned char intensity);
    void SetZoneColor(int zone, std::vector<RGBColor> colors);

private:
    char                    device_name[32];
    hid_device*             dev;
    std::string             location;

    hp_zone                 fan;
    hp_zone                 logo;
    hp_zone                 bar;
    hp_zone                 cpu;
    //unsigned char           current_fan_mode;
    //unsigned char           current_fan_speed;
    
    //unsigned char           current_logo_mode;
    //unsigned char           current_logo_speed;

    //unsigned char           current_bar_mode;
    //unsigned char           current_bar_speed;

    //unsigned char           current_cpu_mode;
    //unsigned char           current_cpu_speed;


    void SendZoneUpdate
        (
        unsigned char zone,
        hp_zone settings,
        std::vector<RGBColor> colors
        );

};
