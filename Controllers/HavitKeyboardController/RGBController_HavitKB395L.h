/*-----------------------------------------*\
|  RGBController_HavitKB395L.h             |
|                                           |
|  Generic RGB Interface for Havit KB395L   |
|  Keyboard                                 |
|                                           |
|  Lisander Lopez 7/7/2021                  |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "HavitKB395LController.h"

class RGBController_HavitKB395L : public RGBController
{
public:
    RGBController_HavitKB395L(HavitKB395LController* havit_ptr);
    ~RGBController_HavitKB395L();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    HavitKB395LController*   havit;
    
    /*-----------------------------------------*\
        This function is used to convert user
        color, into a color that the keyboard
        can display (This keyboard has small
        color set).
        
        Returns 0 if val < 126
        Returns 1 Otherwise
    \*-----------------------------------------*/
    unsigned char rgbToBinary(unsigned char val);
};
