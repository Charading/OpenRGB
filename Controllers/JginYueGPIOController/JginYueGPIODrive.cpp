#pragma once
#include <Windows.h>
#include "JginYueGPIODrive.h"

int GPIOdrive_API(unsigned int* GRB_ptr ,unsigned int num_LED)
{
    typedef void (* ProcGPIOdrive)(unsigned int* GRB_ptr ,unsigned int num_LED,unsigned int MotherBoardtype);
    ProcGPIOdrive GPIOdrive = NULL;
    HMODULE hModule = NULL;
    hModule = LoadLibraryA("openJGINYUErgbV2.dll");
    if(hModule == NULL)
    {
        return 0;
    }
    GPIOdrive = (ProcGPIOdrive)GetProcAddress(hModule, "GPIOdrive");
    GPIOdrive(GRB_ptr,num_LED,1);
    FreeLibrary(hModule);
    return 1;
}
