#ifndef HPOMENLAPTOPCONTROLLER_WINDOWS_H
#define HPOMENLAPTOPCONTROLLER_WINDOWS_H

#include "RGBController.h"
#include <Windows.h>

struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;
struct IWbemCallResult;

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

class HPOmenLaptopController_Windows
{
private:
    /*---------------------------------------------------------*\
    | Controller private functions                              |
    \*---------------------------------------------------------*/
    int execute(int command, int commandType, int inputDataSize, BYTE* inputData, int* returnDataSize, BYTE** returnData);
    void cleanup(int fail_level);

    IWbemLocator* pLoc;
    IWbemServices* pSvc;
    IWbemClassObject* classObject;
    IWbemClassObject* methodParameters;
    IWbemClassObject* dataInClass;
    IWbemCallResult* callResult;

public:
    HPOmenLaptopController_Windows();
    ~HPOmenLaptopController_Windows();

    /*---------------------------------------------------------*\
    | Controller public functions                               |
    \*---------------------------------------------------------*/
    void setColors(std::vector<RGBColor>& colors);
    bool isLightingSupported();
    KeyboardType getKeyboardType();
    void changeMode(KeyboardMode mode);
};

#endif // HPOMENLAPTOPCONTROLLER_WINDOWS_H
