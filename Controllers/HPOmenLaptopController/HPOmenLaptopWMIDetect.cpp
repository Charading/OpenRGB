#ifdef _WIN32

#include "HPOmenLaptopController.h"
#include "Detector.h"

static void DetectHPOmenLaptopWMIControllers() {
    HPOmenLaptopController *controller = new HPOmenLaptopController();

    if (!controller->IsLightingSupported() || controller->GetKeyboardType() != KeyboardType::WITHOUT_NUMPAD) {
        delete controller;
        return;
    }

    /*RGBController *hp_omen_controller = new RGBControlelr_HPOmenLaptopWMI();
    ResourceManager::get()->RegisterRGBController(hp_omen_controller);*/
}

REGISTER_DETECTOR("HP Omen Laptop", DetectHPOmenLaptopWMIControllers);

#endif
