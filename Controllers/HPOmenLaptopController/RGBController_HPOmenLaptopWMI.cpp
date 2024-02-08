#include "RGBController_HPOmenLaptopWMI.h"

RGBController_HPOmenLaptopWMI::RGBController_HPOmenLaptopWMI(HPOmenLaptopController *controller) {
    this->controller = controller;
}

RGBController_HPOmenLaptopWMI::~RGBController_HPOmenLaptopWMI() {
    delete this->controller;
}

void RGBController_HPOmenLaptopWMI::SetupZones() {
    /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    zone keyboard_zone;
    keyboard_zone.leds_count = 4;
    keyboard_zone.leds_min = 0;
    keyboard_zone.leds_max = 4;
    keyboard_zone.name = "Keyboard";
    keyboard_zone.matrix_map = NULL;
    keyboard_zone.type = ZONE_TYPE_LINEAR;
    this->zones.push_back(keyboard_zone);

    /*---------------------------------------------------------*\
    | Set up LEDs                                               |
    \*---------------------------------------------------------*/
    led zone1_leds;
    zone1_leds.name = "Zone 1";
    zone1_leds.value = 0;
    this->leds.push_back(zone1_leds);

    led zone2_leds;
    zone2_leds.name = "Zone 2";
    zone2_leds.value = 0;
    this->leds.push_back(zone2_leds);

    led zone3_leds;
    zone3_leds.name = "Zone 3";
    zone3_leds.value = 0;
    this->leds.push_back(zone3_leds);

    led zone4_leds;
    zone4_leds.name = "Zone 4";
    zone4_leds.value = 0;
    this->leds.push_back(zone4_leds);

    SetupColors();
}

void RGBController_HPOmenLaptopWMI::ResizeZone(int zone, int new_size) {
    /*---------------------------------------------------------*\
    | Not Supported                                             |
    \*---------------------------------------------------------*/
}

void RGBController_HPOmenLaptopWMI::DeviceUpdateLEDs() {
    /*---------------------------------------------------------*\
    | Set new colors for each zone                              |
    \*---------------------------------------------------------*/

    controller->setZoneColors(this->colors);
}

void RGBController_HPOmenLaptopWMI::UpdateZoneLEDs(int zone) {
    DeviceUpdateLEDs();
}

void RGBController_HPOmenLaptopWMI::UpdateSingleLED(int led) {
    /*---------------------------------------------------------*\
    | Not Supported                                             |
    \*---------------------------------------------------------*/
}

void RGBController_HPOmenLaptopWMI::DeviceUpdateMode() {
    /*---------------------------------------------------------*\
    | Change keyboard rgb mode                                  |
    \*---------------------------------------------------------*/

    controller->changeMode((KeyboardMode)this->modes[active_mode].value);
}
