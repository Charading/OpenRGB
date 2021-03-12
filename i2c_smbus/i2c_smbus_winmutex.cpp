/*-----------------------------------------*\
|  i2c_smbus_winmutex.h                     |
|                                           |
|  Global mutex definitión for smbus
|  interlock in Windows                     |
|                                           |
|                                           |
|  Felipe Urzúa (Cheerpipe)    03/06/2021   |
\*-----------------------------------------*/
#ifdef _WIN32
#include "i2c_smbus_winmutex.h"
#include <cstring>

HANDLE i2c_smbus_winmutex::i2c_smbus_winmutex_mutant = CreateMutex(
    NULL,
    FALSE,
    I2C_SMBUS_WINMUTEX_NAME);

i2c_smbus_winmutex::i2c_smbus_winmutex()
{
    if (i2c_smbus_winmutex::i2c_smbus_winmutex_mutant == NULL)
    {
        //Error creating or opening Global Mutex. Do something to handle this path
    }
    else
    {
        ReleaseMutex(i2c_smbus_winmutex::i2c_smbus_winmutex_mutant);
    }
}

i2c_smbus_winmutex::~i2c_smbus_winmutex()
{
    ReleaseMutex(i2c_smbus_winmutex::i2c_smbus_winmutex_mutant);
}

bool i2c_smbus_winmutex::WaitAndLock()
{
    if (i2c_smbus_winmutex::i2c_smbus_winmutex_mutant == 0)
    {
        return false;
    }

    i2c_smbus_winmutex_unlock_result = WaitForSingleObject(
        i2c_smbus_winmutex::i2c_smbus_winmutex_mutant,
        200);

    if (i2c_smbus_winmutex_unlock_result == WAIT_OBJECT_0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool i2c_smbus_winmutex::Unlock()
{
    if (!ReleaseMutex(i2c_smbus_winmutex::i2c_smbus_winmutex_mutant))
    {
        // Handle error. Do Some thing to handle this.
        return false;
    }
    else
    {
        return true;
    }
}
#endif
