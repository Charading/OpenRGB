#ifndef HPOMENLAPTOPCONTROLLER_H
#define HPOMENLAPTOPCONTROLLER_H

#include "RGBController.h"
#include <windows.h>

enum KeyboardType
{
    INVALID = 0,
    NORMAL,
    WITH_NUMPAD,
    WITHOUT_NUMPAD,
    RGB
};

enum KeyboardMode
{
    OFF = 0,
    DIRECT
};

class HPOmenLaptopController
{
private:
    /*---------------------------------------------------------*\
    | Controller private functions                              |
    \*---------------------------------------------------------*/
    int execute(int command, int commandType, int inputDataSize, BYTE* inputData, int* returnDataSize, BYTE** returnData);

public:
    HPOmenLaptopController();
    ~HPOmenLaptopController();

    /*---------------------------------------------------------*\
    | Controller public functions                               |
    \*---------------------------------------------------------*/
    void setZoneColors(std::vector<RGBColor>& colors);
    bool isLightingSupported();
    KeyboardType getKeyboardType();
    void changeMode(KeyboardMode mode);
};

#endif // HPOMENLAPTOPCONTROLLER_H
