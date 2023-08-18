#pragma once
#include <iostream>
#include <Windows.h>
#include "dependencies/tscns/tscns.h"
#include "JginYueGPIODrive.h"

int GPIOdrive(unsigned int* GRB_ptr ,unsigned int num_LED,HMODULE hModule)
{
    ProcMapPhysToLin MapPhysToLin = NULL;
    ProcUnmapPhysicalMemory UnmapPhysicalMemory = NULL;
    HANDLE PhysMemHANDLE = NULL;
    PDWORD pdwlinaddr = NULL;

    if (num_LED<5)
    {
        num_LED = 5;
    }

    MapPhysToLin = (ProcMapPhysToLin)GetProcAddress(hModule, "MapPhysToLin");
    UnmapPhysicalMemory = (ProcUnmapPhysicalMemory)GetProcAddress(hModule, "UnmapPhysicalMemory");
    pdwlinaddr = (PDWORD)MapPhysToLin((PBYTE)(ALDERLAKE_GPP_DW0_F8), 4, &PhysMemHANDLE);
    if (pdwlinaddr == NULL)
    {
        std::cout << "Fail" << std::endl;
        return 0;
    }
    int64_t delay_time_preset_0_ns = 280000;
    int64_t delay_time_0_ns = 1300;
    int64_t delay_time_1_ns = 500;
    unsigned int package_num = num_LED * 48;

    TSCNS tscns;
    tscns.init();
    int64_t time_fin = 0;
    int64_t time0 = tscns.rdns();
    int64_t time_now = 0;
    int64_t time_next = 0;


    pdwlinaddr = (PDWORD)MapPhysToLin((PBYTE)(ALDERLAKE_GPP_DW0_F8), 4, &PhysMemHANDLE);
    if (pdwlinaddr == NULL)
    {
        std::cout << "Fail" << std::endl;
        return 0;
    }

    for (unsigned int i = 0, j = 0, k = 0, m = 0, n = 0; i <= package_num; )
    {
        time_now = tscns.rdns();

        if (m == 0)
        {
            time0 = tscns.rdns();
            m = 1;
            *pdwlinaddr = (DWORD)ALDERLAKE_GPP_DW0_F8_LOW;
            time_next = time_now + delay_time_preset_0_ns;
        }
        if (time_next <= time_now)
        {
            if (i == package_num)
            {
                *pdwlinaddr = (DWORD)ALDERLAKE_GPP_DW0_F8_HI;
                i++;
                break;
            }
            n = i / 48;
            j = 24 - (i / 2) % 24 - 1;
            k = (GRB_ptr[n] >> j) & 0x1U;
            switch (i % 2)
            {
            case 0:
                *pdwlinaddr = (DWORD)ALDERLAKE_GPP_DW0_F8_HI;
                if (k)
                {
                    time_fin = tscns.rdns();
                    time_next = time_fin + delay_time_0_ns;
                }
                else
                {
                    time_fin = tscns.rdns();
                    time_next = time_fin + delay_time_1_ns;
                }

                break;
            case 1:
                *pdwlinaddr = (DWORD)ALDERLAKE_GPP_DW0_F8_LOW;
                if (k)
                {
                    time_fin = tscns.rdns();
                    time_next = time_fin + delay_time_1_ns;
                }
                else
                {
                    time_fin = tscns.rdns();
                    time_next = time_fin + delay_time_0_ns;
                }
                break;
            default:

                break;
            }
            if (i == package_num -1)
            {
                time_next = time_now + 280000;
            }
            i++;
        }
    }
    UnmapPhysicalMemory(PhysMemHANDLE, (PBYTE)pdwlinaddr);
    return 1;

}
