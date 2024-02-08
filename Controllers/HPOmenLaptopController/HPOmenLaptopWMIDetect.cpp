#ifdef _WIN32

#include "RGBController_HPOmenLaptopWMI.h"
#include "HPOmenLaptopController.h"
#include "Detector.h"

static void DetectHPOmenLaptopWMIControllers() {
    HPOmenLaptopController *controller = new HPOmenLaptopController();

    if (!controller->isLightingSupported() || controller->getKeyboardType() != KeyboardType::WITHOUT_NUMPAD) {
        delete controller;
        return;
    }

    RGBController *hp_omen_controller = new RGBController_HPOmenLaptopWMI(controller);
    ResourceManager::get()->RegisterRGBController(hp_omen_controller);
}

REGISTER_DETECTOR("HP Omen Laptop", DetectHPOmenLaptopWMIControllers);

#endif
