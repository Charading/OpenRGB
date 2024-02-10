#ifndef HPOMENLAPTOPCONTROLLER_H
#define HPOMENLAPTOPCONTROLLER_H

#ifdef _WIN32

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
    void setColors(std::vector<RGBColor>& colors);
    bool isLightingSupported();
    KeyboardType getKeyboardType();
    void changeMode(KeyboardMode mode);
};

#endif

#endif // HPOMENLAPTOPCONTROLLER_H
