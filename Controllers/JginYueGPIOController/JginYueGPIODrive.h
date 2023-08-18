#pragma once
#include <Windows.h>
#define ALDERLAKE_GPP_DW0_F8 0xE06A0A50
#define ALDERLAKE_GPP_DW0_F8_HI 0x84000201
#define ALDERLAKE_GPP_DW0_F8_LOW 0x84000200

typedef PBYTE(__stdcall* ProcMapPhysToLin)(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE* pPhysicalMemoryHandle);
typedef BOOL(__stdcall* ProcUnmapPhysicalMemory)(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);

int GPIOdrive(unsigned int* GRB_ptr ,unsigned int num_LED,HMODULE hModule);
