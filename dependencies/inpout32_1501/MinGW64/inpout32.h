#pragma once

#include <windows.h>

#ifdef INPOUT32_PRIVATE
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//Functions exported from DLL.
//For easy inclusion is user projects.
//Original InpOut32 function support
void	DLLEXPORT Out32(short PortAddress, short data);
short	DLLEXPORT Inp32(short PortAddress);

//My extra functions for making life easy
BOOL	DLLEXPORT IsInpOutDriverOpen();  // Returns TRUE if the InpOut driver was opened successfully
BOOL	DLLEXPORT IsXP64Bit();			 // Returns TRUE if the OS is 64bit (x64) Windows.
void    DLLEXPORT SetDebugCallback(void(*)(int, const char*)); // Replaces debug func with provided callback
int     DLLEXPORT InitDriver();          // Attempts to restart the driver if it failed to load

//DLLPortIO function support
UCHAR   DLLEXPORT DlPortReadPortUchar (USHORT port);
void    DLLEXPORT DlPortWritePortUchar(USHORT port, UCHAR Value);

USHORT  DLLEXPORT DlPortReadPortUshort (USHORT port);
void    DLLEXPORT DlPortWritePortUshort(USHORT port, USHORT Value);

ULONG	DLLEXPORT DlPortReadPortUlong(ULONG port);
void	DLLEXPORT DlPortWritePortUlong(ULONG port, ULONG Value);

//WinIO function support (Untested and probably does NOT work - esp. on x64!)
PBYTE	DLLEXPORT MapPhysToLin(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle);
BOOL	DLLEXPORT UnmapPhysicalMemory(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);
BOOL	DLLEXPORT GetPhysLong(PBYTE pbPhysAddr, PDWORD pdwPhysVal);
BOOL	DLLEXPORT SetPhysLong(PBYTE pbPhysAddr, DWORD dwPhysVal);

}
