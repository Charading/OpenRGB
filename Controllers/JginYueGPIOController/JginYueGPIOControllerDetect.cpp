#include "RGBController_JginYueGPIO.h"
#include "JginYueGPIOController.h"
#include "RGBController.h"
#include "Detector.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void DetectJginYueGPIOController()
{
    HMODULE hModule = NULL;
    #ifdef _WIN64
    hModule = LoadLibraryA("inpoutx64.dll");
    #endif

    if (hModule == NULL)
    {
        return ;
    }

    if(hModule)
    {
        JginYueGPIOController*       controller      =new JginYueGPIOController(hModule);
        RGBController_JginYueGPIO*   rgb_controller  =new RGBController_JginYueGPIO(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_DETECTOR("JginYue PCH ARGB Controller",DetectJginYueGPIOController);
