/* Filename		smbus_detect.h
 * Author		Stavros Avramidis
 * Description	Detects ports of the SMBUS via Windows queries
 * Date			31/1/2019
 */

#pragma once

#define _WIN32_DCOM
#include <cstring>
#include <iostream>
#include <regex>

#include <comdef.h>
#include <Wbemidl.h>

/*TODO: Find location resources with WMI
*/


#pragma comment(lib, "wbemuuid.lib")

struct DetectSMBUHandle{
    char* name;
    uint8_t pci_bus;
    uint8_t deviceID;
    uint8_t function;
};

int detectSMBUS(DetectSMBUHandle& handle);