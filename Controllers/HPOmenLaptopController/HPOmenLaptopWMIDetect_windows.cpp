#include "RGBController_HPOmenLaptopWMI_windows.h"
#include "HPOmenLaptopController_windows.h"
#include "Detector.h"

static void DetectHPOmenLaptopWMIControllers() {
    HPOmenLaptopController_windows *controller = new HPOmenLaptopController_windows();

    if(!controller->isLightingSupported() || controller->getKeyboardType() != KeyboardType::WITHOUT_NUMPAD)
    {
        delete controller;
        return;
    }

    RGBController *hp_omen_controller = new RGBController_HPOmenLaptopWMI_windows(controller);
    ResourceManager::get()->RegisterRGBController(hp_omen_controller);
}

REGISTER_DETECTOR("HP Omen 4-Zone Laptop Keyboard", DetectHPOmenLaptopWMIControllers);
