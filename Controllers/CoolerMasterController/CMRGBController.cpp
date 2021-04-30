/*-------------------------------------------------------------------*\
|  CMRGBController.cpp                                          |
|                                                                     |
|  Driver for Coolermaster Small ARGB USB Controller                  |
|                                                                     |
|  Chris M (Dr_No)          31st Jan 2021                             |
|                                                                     |
|  Simple RGB device with 5 modes                                     |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "CMRGBController.h"
#include <cstring>

CMRGBController::CMRGBController(hid_device* dev_handle, char *_path)
{
    const int szTemp = 256;
    wchar_t tmpName[szTemp];

    dev                     = dev_handle;
    location                = _path;

    hid_get_manufacturer_string(dev, tmpName, szTemp);
    std::wstring wName = std::wstring(tmpName);
    device_name = std::string(wName.begin(), wName.end());

    hid_get_product_string(dev, tmpName, szTemp);
    wName = std::wstring(tmpName);
    device_name.append(" ").append(std::string(wName.begin(), wName.end()));

    hid_get_serial_number_string(dev, tmpName, szTemp);
    wName = std::wstring(tmpName);
    serial = std::string(wName.begin(), wName.end());
}

std::string CMRGBController::GetDeviceName()
{
    return device_name;
}

std::string CMRGBController::GetSerial()
{
    return serial;
}

std::string CMRGBController::GetLocation()
{
    return("HID: " + location);
}

CMRGBController::~CMRGBController()
{
    if(dev)
    {
        hid_close(dev);
    }
}
