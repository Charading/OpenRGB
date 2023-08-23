#include "RGBController_JginYueGPIO.h"
#include "JginYueGPIOController.h"
#include "RGBController.h"
#include "Detector.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include "dependencies/dmiinfo.h"

void DetectJginYueGPIOController()
{
    HMODULE hModule = NULL;
    #ifdef _WIN64
    hModule = LoadLibraryA("inpoutx64.dll");//testing is there the lib manually installed
    //hModule = LoadLibraryA("OpenJginYueRGB.dll");
    #endif
    DMIInfo dmi;
    std::string vender = dmi.getManufacturer();
    std::string MB_name =dmi.getMainboard();
    if (hModule == NULL)
    {
        return ;
    }

    if (vender !="JGINYUE")
    {
        FreeLibrary(hModule);
        return ;
    }
    if  (
        !((MB_name=="Z790M Snow Dream")||(MB_name=="H610M-HD")||(MB_name=="B660I Snow Dream")||
          (MB_name=="H610M-GAMING")||(MB_name=="B760I GAMING")||(MB_name=="H610I-GAMING")||
          (MB_name=="B760M GAMING")||(MB_name=="B760I Snow Dream")||(MB_name=="H610M-HD")||
          (MB_name=="B760M GAMING D5")||(MB_name=="B760M Snow Dream")||(MB_name=="B760M-VDH"))
        )
    {
        FreeLibrary(hModule);
        return;
    }

    if(hModule)
    {
        JginYueGPIOController*       controller      =new JginYueGPIOController();
        RGBController_JginYueGPIO*   rgb_controller  =new RGBController_JginYueGPIO(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
    FreeLibrary(hModule);
}

REGISTER_DETECTOR("JginYue PCH ARGB Controller",DetectJginYueGPIOController);
