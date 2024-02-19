#include "RGBController_HPOmenLaptopWMI_Windows.h"
#include "HPOmenLaptopController_Windows.h"
#include "Detector.h"

static void DetectHPOmenLaptopWMIControllers() {
    HPOmenLaptopController_Windows *controller = new HPOmenLaptopController_Windows();

    if(!controller->isLightingSupported() || controller->getKeyboardType() != KeyboardType::WITHOUT_NUMPAD)
    {
        delete controller;
        return;
    }

    RGBController *hp_omen_controller = new RGBController_HPOmenLaptopWMI_Windows(controller);
    ResourceManager::get()->RegisterRGBController(hp_omen_controller);
}

REGISTER_DETECTOR("HP Omen 4-Zone Laptop Keyboard", DetectHPOmenLaptopWMIControllers);
