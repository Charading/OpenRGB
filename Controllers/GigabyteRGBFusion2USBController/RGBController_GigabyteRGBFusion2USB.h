/*-----------------------------------------*\
|  RGBController_GigabyteRGBFusion2USB.h    |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Gigabyte RGB Fusion 2.0 USB Driver       |
|                                           |
|  Author: jackun 1/8/2020                  |
|  Maintainer: Chris M (Dr_No)              |
\*-----------------------------------------*/

#pragma once
#include "Detector.h"
#include "RGBController.h"
#include "GigabyteRGBFusion2USBController.h"
#include <map>
#include <vector>

#define RGBFusion2_Digital_LEDS_Min 0;
#define RGBFusion2_Digital_LEDS_Max 1024;
#define RGBFusion2_Digital_Direct_Offset (HDR_D_LED1_RGB - HDR_D_LED1);

template<typename K, typename V>
static std::map<V, K> reverse_map(const std::map<K, V>& m)
{
    std::map<V, K> r;
    for (const auto& kv : m)
        r[kv.second] = kv.first;
    return r;
}

typedef std::map< std::string, int > FwdLedHeaders;
typedef std::map< int, std::string > RvrseLedHeaders;

struct LedPort
{
    std::string name;
    int         header;
    int         count;
};

typedef std::map< std::string, std::string > MBName;
typedef std::map< std::string, std::vector<LedPort> > ZoneLeds;
typedef std::map< std::string, ZoneLeds> KnownLayout;

class RGBController_RGBFusion2USB: public RGBController
{
public:
    RGBController_RGBFusion2USB(RGBFusion2USBController* controller_ptr, std::string _detector_name);
    ~RGBController_RGBFusion2USB();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    void        Load_Device_Config();
    void        Init_Controller();
    int         GetLED_Zone(int led_idx);
    MBName      MBName2Layout;
    bool        custom_layout;
    std::string             detector_name;  /* controller detector name - NB To be moved to RGBController.h*/

    RGBFusion2USBController*        controller;
    IT8297Report                    report;
    ZoneLeds                        layout;
};
