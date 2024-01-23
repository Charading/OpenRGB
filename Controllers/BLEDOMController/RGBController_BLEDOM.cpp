#include "RGBController_BLEDOM.h"
#include <QDateTime>

#define BLEDOM_MODE_DIRECT 0
#define BLEDOM_MODE_OFF 1

RGBController_BLEDOM::RGBController_BLEDOM(BLEDOMController* controller_ptr) {
    controller  = controller_ptr;

    name        = "BLEDOM";
    vendor      = "N/A";
    type        = DEVICE_TYPE_LIGHT;
    version     = "N/A";
    description = "BLE LED Strip Controller";
    serial      = controller->GetBluetoothAddress();
    location    = controller->GetBluetoothAddress();

    mode Direct;
    Direct.name           = "Direct";
    Direct.value          = 0;
    Direct.flags          = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Direct.color_mode     = MODE_COLORS_PER_LED;
    Direct.brightness     = 0x64;
    Direct.brightness_min = 0;
    Direct.brightness_max = 0x64;
    modes.push_back(Direct);

    mode Off;
    Off.name              = "Off";
    Off.value             = 0;
    Off.flags             = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Off.color_mode        = MODE_COLORS_PER_LED;
    Off.brightness        = 0;
    Off.brightness_min    = 0;
    Off.brightness_max    = 0;
    modes.push_back(Off);

    SetupZones();
}

RGBController_BLEDOM::~RGBController_BLEDOM()
{
    delete controller;
}

void RGBController_BLEDOM::SetupZones()
{
    zone led_zone;
    led_zone.name         = "RGB Light";
    led_zone.type         = ZONE_TYPE_SINGLE;
    led_zone.leds_min     = 1;
    led_zone.leds_max     = 1;
    led_zone.leds_count   = 1;
    led_zone.matrix_map   = NULL;
    zones.push_back(led_zone);

    led new_led;
    new_led.name = "RGB Light";

    leds.push_back(new_led);

    SetupColors();
}

void RGBController_BLEDOM::ResizeZone(int zone, int new_size)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_BLEDOM::DeviceUpdateLEDs()
{
    qint64 current = QDateTime::currentMSecsSinceEpoch();
    if(current - lastUpdate < 50) return;
    lastUpdate = current;
    if(modes[active_mode].value == BLEDOM_MODE_OFF) {
        controller->SetPower(false);
    }
    else if (modes[active_mode].value == BLEDOM_MODE_DIRECT)
    {
        unsigned char red = RGBGetRValue(colors[0]);
        unsigned char grn = RGBGetGValue(colors[0]);
        unsigned char blu = RGBGetBValue(colors[0]);

        controller->SetColor(red, grn, blu);
        controller->SetBrightness(modes[active_mode].brightness);
    }

}

void RGBController_BLEDOM::UpdateZoneLEDs(int zone)
{
    DeviceUpdateLEDs();
}

void RGBController_BLEDOM::UpdateSingleLED(int led)
{
    DeviceUpdateLEDs();
}

void RGBController_BLEDOM::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
