/***********************************************************************\
*                                                                       *
* InpOut32drv.cpp                                                       *
*                                                                       *
* The entry point for the InpOut DLL                                    *
* Provides the 32 and 64bit implementation of InpOut32 DLL to install   *
* and call the appropriate driver and write directly to hardware ports. *
*                                                                       *
* Written by Phillip Gibbons (Highrez.co.uk)                            *
* Based on orriginal, written by Logix4U (www.logix4u.net)              *
*                                                                       *
\***********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "inpout32.h"
#include "hwinterfacedrv.h"
#include "resource.h"

#ifdef __cplusplus
extern "C" {
#endif

int inst32();
int inst64();
int install();
int start(const wchar_t *pszDriver);

//First, lets set the DRIVERNAME depending on our configuraiton.
#define DRIVERNAMEx64 "inpoutx64\0"
#define DRIVERNAMEi386 "inpout32\0"

static HANDLE hdriver=NULL;
static wchar_t path[MAX_PATH];
static HINSTANCE hmodule;
static int sysver;
static bool use64;
static void(* logCb)(int, const char*);

void CloseDriver(void);
void logDefault(int, const char* p)
{
    puts(p);
}

BOOL APIENTRY DllMain( HINSTANCE  hModule, 
					  DWORD  ul_reason_for_call, 
                      LPVOID /*lpReserved*/
					  )
{
	hmodule = hModule;
    use64 = IsXP64Bit();
    logCb = &logDefault;

	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			sysver = SystemVersion();
			if(sysver==2)
			{
                InitDriver();
			}
		}

		break;
	case DLL_PROCESS_DETACH:
		{
			if(sysver==2)
			{
                CloseDriver();
			}
		}
		break;
	}
	return TRUE;
}

/***********************************************************************/

void CloseDriver(void)
{
	if (hdriver)
	{
        logCb(4, "Closing InpOut driver...\n");
		CloseHandle(hdriver);
		hdriver=NULL;
	}
}

void Out32(short PortAddress, short data)
{
	{
		switch(sysver)
		{
		case 1:
#ifdef _M_IX86
			_outp( PortAddress,data);	//Will ONLY compile on i386 architecture
#endif
			break;
		case 2:
			unsigned int error;
			DWORD BytesReturned;        
            BYTE Buffer[3]={0};
			unsigned short* pBuffer;
			pBuffer = (unsigned short *)&Buffer[0];
			*pBuffer = LOWORD(PortAddress);
			Buffer[2] = LOBYTE(data);

			error = DeviceIoControl(hdriver,
				IOCTL_WRITE_PORT_UCHAR,
				&Buffer,
				3,
				NULL,
				0,
				&BytesReturned,
				NULL);
			break;
		}
	}
}

/*********************************************************************/

short Inp32(short PortAddress)
{
	{
		BYTE retval(0);
		switch(sysver)
		{
		case 1:
#ifdef _M_IX86
			retval = _inp(PortAddress);
#endif
			return retval;
			break;
		case 2:
			unsigned int error;
			DWORD BytesReturned;
            unsigned char Buffer[3]={0};
			unsigned short * pBuffer;
			pBuffer = (unsigned short *)&Buffer;
			*pBuffer = LOWORD(PortAddress);
			Buffer[2] = 0;
			error = DeviceIoControl(hdriver,
				IOCTL_READ_PORT_UCHAR,
				&Buffer,
				2,
				&Buffer,
				1,
				&BytesReturned,
				NULL);

			if (error==0)
			{
				DWORD dwError = GetLastError();
                char tszError[255];
                sprintf(tszError, "Inp32: Error %d\n", dwError);
                logCb(3, tszError);
			}

			//Do this to ensure only the first byte is returned, we dont really want to return a short as were only reading a byte.
			//but we also dont want to change the InpOut interface!
			UCHAR ucRes = (UCHAR)Buffer[0];
			return ucRes;

			break;
		}
	}
	return 0;
}

/*********************************************************************/

int InitDriver()
{
    logCb(4, "Attempting to open InpOut driver...\n");
    TCHAR szFileName[MAX_PATH] = {0};
    _stprintf_s(szFileName, MAX_PATH, _T("\\\\.\\%s"), use64 ? DRIVERNAMEx64 : DRIVERNAMEi386);

	hdriver = CreateFile(szFileName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	if(hdriver == INVALID_HANDLE_VALUE) 
	{
        if(start(use64 ? L"" DRIVERNAMEx64 : L"" DRIVERNAMEi386))
		{
            int nResult = install();

            if(nResult) return nResult;

            nResult = start(use64 ? L"" DRIVERNAMEx64 : L"" DRIVERNAMEi386);

			if (nResult == ERROR_SUCCESS)
			{
				hdriver = CreateFile(szFileName, 
					GENERIC_READ | GENERIC_WRITE, 
					0, 
					NULL,
					OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL);

				if(hdriver != INVALID_HANDLE_VALUE) 
				{
                    logCb(4, "Successfully opened ");
                    logCb(4, use64 ? DRIVERNAMEx64 : DRIVERNAMEi386);
                    logCb(4, " driver\n");
					return 0;
				}
			}
			else
			{
                char szMsg[255] = {0};
                sprintf(szMsg, "Unable to open %s driver. Error code %d.\n", use64 ? DRIVERNAMEx64 : DRIVERNAMEi386, nResult);
                logCb(3, szMsg);

				//RemoveDriver();
			}
		}
		return 1;
	}
    logCb(4, "Successfully opened ");
    logCb(4, use64 ? DRIVERNAMEx64 : DRIVERNAMEi386);
    logCb(4, " driver\n");
	return 0;
}

/***********************************************************************/

int install()
{
    wchar_t szDriverSys[MAX_PATH];
    wcscpy_s(szDriverSys, MAX_PATH, use64 ? L"" DRIVERNAMEi386 : L"" DRIVERNAMEx64);
    wcscat_s(szDriverSys, MAX_PATH, _T(".sys\0"));
	
	SC_HANDLE  Mgr;
	SC_HANDLE  Ser;
    GetSystemDirectoryW(path, MAX_PATH);
    HRSRC hResource = FindResource(hmodule, use64 ? MAKEINTRESOURCE(IDR_INPOUT32) : MAKEINTRESOURCE(IDR_INPOUTX64), _T("bin"));
	if(hResource)
	{
		HGLOBAL binGlob = LoadResource(hmodule, hResource);

		if(binGlob)
		{
			void *binData = LockResource(binGlob);

			if(binData)
			{
				HANDLE file;
				_tcscat_s(path, sizeof(path), _T("\\Drivers\\"));
				_tcscat_s(path, sizeof(path), szDriverSys);
			
				PVOID OldValue;
				DisableWOW64(&OldValue);
				file = CreateFile(path,
					GENERIC_WRITE,
					0,
					NULL,
					CREATE_ALWAYS,
					0,
					NULL);

				if(file && file != INVALID_HANDLE_VALUE)
				{
					DWORD size, written;

					size = SizeofResource(hmodule, hResource);
					WriteFile(file, binData, size, &written, NULL);
					CloseHandle(file);
				}
				else
				{
					//error
				}
                if(use64)
                {
                    RevertWOW64(&OldValue);
                }
			}
		}
	}

	Mgr = OpenSCManager (NULL, NULL,SC_MANAGER_ALL_ACCESS);
	if (Mgr == NULL)
	{							//No permission to create service
		if (GetLastError() == ERROR_ACCESS_DENIED) 
		{
			return 5;  // error access denied
		}
	}	
	else
	{
		TCHAR szFullPath[MAX_PATH] = _T("System32\\Drivers\\\0");
		_tcscat_s(szFullPath, MAX_PATH, szDriverSys);
        Ser = CreateServiceW (Mgr,
            use64 ? L"" DRIVERNAMEi386 : L"" DRIVERNAMEx64,
            use64 ? L"" DRIVERNAMEi386 : L"" DRIVERNAMEx64,
            SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,             
			SERVICE_AUTO_START,               
			SERVICE_ERROR_NORMAL,               
			szFullPath,  
			NULL,                               
			NULL,                              
			NULL,                               
			NULL,                              
			NULL                               
			);
        CloseServiceHandle(Ser);
    }
	CloseServiceHandle(Mgr);

	return 0;
}

/**************************************************************************/
int start(const wchar_t* pszDriver)
{
	SC_HANDLE  Mgr;
	SC_HANDLE  Ser;

	Mgr = OpenSCManager (NULL, NULL,SC_MANAGER_ALL_ACCESS);

	if (Mgr == NULL)
	{							//No permission to create service
		if (GetLastError() == ERROR_ACCESS_DENIED) 
		{
            Mgr = OpenSCManagerW(NULL, NULL, GENERIC_READ);
            Ser = OpenServiceW(Mgr, pszDriver, GENERIC_EXECUTE);
			if (Ser)
			{    // we have permission to start the service
				if(!StartService(Ser,0,NULL))
				{
					CloseServiceHandle (Ser);
					return 4; // we could open the service but unable to start
				}
			}
		}
	}
	else
	{// Successfuly opened Service Manager with full access
        Ser = OpenServiceW(Mgr, pszDriver, SERVICE_ALL_ACCESS);
		if (Ser)
		{
			if(!StartService(Ser,0,NULL))
			{
				CloseServiceHandle (Ser);
				return 3; // opened the Service handle with full access permission, but unable to start
			}
			else
			{
				CloseServiceHandle (Ser);
				return 0;
			}
		}
	}
	return 1;
}

BOOL IsInpOutDriverOpen()
{
	sysver = SystemVersion();
	if (sysver==2)
	{
		if (hdriver!=INVALID_HANDLE_VALUE && hdriver != NULL)
			return TRUE;
	}
	else if (sysver==1)
	{
		return TRUE;
	}
	return FALSE;
}

UCHAR DlPortReadPortUchar (USHORT port)
{
	UCHAR retval(0);
	switch(sysver)
	{
	case 1:
#ifdef _M_IX86
		retval = _inp((USHORT)port);
#endif
		return retval;
		break;
	case 2:
		unsigned int error;
		DWORD BytesReturned;
        BYTE Buffer[3]={0};
		unsigned short * pBuffer;
		pBuffer = (unsigned short *)&Buffer;
		*pBuffer = port;
		Buffer[2] = 0;
		error = DeviceIoControl(hdriver,
			IOCTL_READ_PORT_UCHAR,
			&Buffer,
			sizeof(Buffer),
			&Buffer,
			sizeof(Buffer),
			&BytesReturned,
			NULL);

		return((UCHAR)Buffer[0]);

		break;
	}
	return 0;
}

void DlPortWritePortUchar (USHORT port, UCHAR Value)
{
	switch(sysver)
	{
	case 1:
#ifdef _M_IX86
		_outp((UINT)port,Value);	//Will ONLY compile on i386 architecture
#endif
		break;
	case 2:
		unsigned int error;
		DWORD BytesReturned;        
        BYTE Buffer[3]={0};
		unsigned short * pBuffer;
		pBuffer = (unsigned short *)&Buffer[0];
		*pBuffer = port;
		Buffer[2] = Value;

		error = DeviceIoControl(hdriver,
			IOCTL_WRITE_PORT_UCHAR,
			&Buffer,
			sizeof(Buffer),
			NULL,
			0,
			&BytesReturned,
			NULL);
		break;
	}
}

USHORT  DlPortReadPortUshort (USHORT port)
{
	if (sysver!=2)
		return 0;

    //ULONG retval(0);
	unsigned int error;
	DWORD BytesReturned;
	unsigned short sPort=port;
	error = DeviceIoControl(hdriver,
		IOCTL_READ_PORT_USHORT,
		&sPort,
		sizeof(unsigned short),
		&sPort,
		sizeof(unsigned short),
		&BytesReturned,
		NULL);
	return(sPort);
}

void  DlPortWritePortUshort (USHORT port, USHORT Value)
{
	if (sysver!=2)
		return;

	unsigned int error;
	DWORD BytesReturned;        
    BYTE Buffer[5]={0};
	unsigned short * pBuffer;
	pBuffer = (unsigned short *)&Buffer[0];
	*pBuffer = LOWORD(port);
	*(pBuffer+1) = Value;

	error = DeviceIoControl(hdriver,
		IOCTL_WRITE_PORT_USHORT,
		&Buffer,
		sizeof(Buffer),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

ULONG  DlPortReadPortUlong(ULONG port)
{
	if (sysver!=2)
		return 0;

    //ULONG retval(0);
	unsigned int error;
	DWORD BytesReturned;
	unsigned long lPort=port;

    PULONG  ulBuffer;
    ulBuffer = (PULONG)&lPort;
    //ULONG test = ulBuffer[0];

	error = DeviceIoControl(hdriver,
		IOCTL_READ_PORT_ULONG,
		&lPort,
		sizeof(unsigned long),
		&lPort,
		sizeof(unsigned long),
		&BytesReturned,
		NULL);
	return(lPort);
}

void  DlPortWritePortUlong (ULONG port, ULONG Value)
{
	if (sysver!=2)
		return;

	unsigned int error;
	DWORD BytesReturned;        
    BYTE Buffer[8] = {0};
	unsigned long* pBuffer;
	pBuffer = (unsigned long*)&Buffer[0];
	*pBuffer = port;
	*(pBuffer+1) = Value;

	error = DeviceIoControl(hdriver,
		IOCTL_WRITE_PORT_ULONG,
		&Buffer,
		sizeof(Buffer),
		NULL,
		0,
		&BytesReturned,
		NULL);
}


// Support functions for WinIO
PBYTE  MapPhysToLin(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle)
{
	PBYTE pbLinAddr;
	tagPhys32Struct Phys32Struct;
	DWORD dwBytesReturned;

	if (sysver!=2)
        return NULL;

	Phys32Struct.dwPhysMemSizeInBytes = dwPhysSize;
	Phys32Struct.pvPhysAddress = pbPhysAddr;

	if (!DeviceIoControl(hdriver, IOCTL_WINIO_MAPPHYSTOLIN, &Phys32Struct,
		sizeof(tagPhys32Struct), &Phys32Struct, sizeof(tagPhys32Struct),
		&dwBytesReturned, NULL))
		return NULL;
	else
	{
#ifdef _M_X64
		pbLinAddr = (PBYTE)((LONGLONG)Phys32Struct.pvPhysMemLin + (LONGLONG)pbPhysAddr - (LONGLONG)Phys32Struct.pvPhysAddress);
#else
		pbLinAddr = (PBYTE)((DWORD)Phys32Struct.pvPhysMemLin + (DWORD)pbPhysAddr - (DWORD)Phys32Struct.pvPhysAddress);
#endif
		*pPhysicalMemoryHandle = Phys32Struct.PhysicalMemoryHandle;
	}
	return pbLinAddr;
}


BOOL  UnmapPhysicalMemory(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr)
{
	tagPhys32Struct Phys32Struct;
	DWORD dwBytesReturned;

	if (sysver!=2)
		return false;

	Phys32Struct.PhysicalMemoryHandle = PhysicalMemoryHandle;
	Phys32Struct.pvPhysMemLin = pbLinAddr;

	if (!DeviceIoControl(hdriver, IOCTL_WINIO_UNMAPPHYSADDR, &Phys32Struct,
		sizeof(tagPhys32Struct), NULL, 0, &dwBytesReturned, NULL))
		return false;

	return true;
}

BOOL  GetPhysLong(PBYTE pbPhysAddr, PDWORD pdwPhysVal)
{
	PDWORD pdwLinAddr;
	HANDLE PhysicalMemoryHandle;

	if (sysver!=2)
		return false;

	pdwLinAddr = (PDWORD)MapPhysToLin(pbPhysAddr, 4, &PhysicalMemoryHandle);
	if (pdwLinAddr == NULL)
		return false;

	*pdwPhysVal = *pdwLinAddr;
	UnmapPhysicalMemory(PhysicalMemoryHandle, (PBYTE)pdwLinAddr);
	return true;
}


BOOL  SetPhysLong(PBYTE pbPhysAddr, DWORD dwPhysVal)
{
	PDWORD pdwLinAddr;
	HANDLE PhysicalMemoryHandle;
	if (sysver!=2)
		return false;

	pdwLinAddr = (PDWORD)MapPhysToLin(pbPhysAddr, 4, &PhysicalMemoryHandle);
	if (pdwLinAddr == NULL)
		return false;

	*pdwLinAddr = dwPhysVal;
	UnmapPhysicalMemory(PhysicalMemoryHandle, (PBYTE)pdwLinAddr);
	return true;
}

#ifdef __cplusplus
}
#endif
