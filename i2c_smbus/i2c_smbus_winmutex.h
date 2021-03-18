/*-----------------------------------------*\
|  i2c_smbus_winmutex.h                     |
|                                           |
|  Global mutex definitión for smbus        |
|  interlock on Windows                     |
|                                           |
|                                           |
|  Felipe Urzúa (Cheerpipe)    03/06/2021   |
\*-----------------------------------------*/
#ifdef _WIN32
#include <string>
#include <Windows.h>

#define I2C_SMBUS_WINMUTEX_NAME             "Global\\Access_SMBUS.HTP.Method"

class i2c_smbus_winmutex
{
public:
    i2c_smbus_winmutex();
    ~i2c_smbus_winmutex();

     bool            WaitAndLock();
     bool            Unlock();
     static HANDLE           i2c_smbus_winmutex_mutant;
     DWORD                   i2c_smbus_winmutex_unlock_result;
};

#endif
