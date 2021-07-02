#include "Detector.h"
#include "BlinkyTapeController.h"
#include "RGBController.h"
#include "RGBController_BlinkyTape.h"
#include "SettingsManager.h"
#include "find_usb_serial_port.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************************\
*                                                                                          *
*   DetectBlinkyTapeControllers                                                            *
*                                                                                          *
*       Detect BlinkyTape devices                                                          *
*                                                                                          *
\******************************************************************************************/

void DetectBlinkyTapeControllers(std::vector<RGBController*> &rgb_controllers)
{
    unsigned short VID = 0x1d50;
    unsigned short PID = 0x605e;

    std::vector<std::string *> device_locations = find_usb_serial_port(VID, PID);

    for(std::string * device_location : device_locations) {
        int led_count = 64;

        std::string value = *device_location + ","  + std::to_string(led_count);

        BlinkyTapeController* new_blinkytape = new BlinkyTapeController();
        new_blinkytape->Initialize((char *)value.c_str());

        RGBController_BlinkyTape* new_controller = new RGBController_BlinkyTape(new_blinkytape);
        rgb_controllers.push_back(new_controller);
    }

}

REGISTER_DETECTOR("BlinkyTape", DetectBlinkyTapeControllers);
