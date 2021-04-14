/*--------------------------------------------*\
|  DrevoCaliburV2TEController.h                |
|                                              |
|  Generic RGB Interface for Drevo Calibur V2  |
|                                              |
|  Nissansneshi (N I S S A N) 14/04/2021       |
\*--------------------------------------------*/

#include "RGBcontroller.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

class DrevoCaliburV2TEController
{
    public:
    DrevoCaliburV2TEController(hid_device* dev_handle, const char* path);
    ~HyperXAlloyOriginsController();

    std::string   GetDeviceLocation();
    std::string   GetSerialString();

    void SetLEDsDirect(std::vector<RGBColor> colors);

private:
   hid_device*        dev;
   std::string        location;
   void   SendDirectInitialization();
   void   SendDirectColorPacket
   (
       RGBColor*      color_data,
       unsigned int   color_count
   );
};