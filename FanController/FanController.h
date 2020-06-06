/*-----------------------------------------*\
|  FanController.h                          |
|                                           |
|  Definitions and types for generic fan    |
|  and pump controller interface            |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/5/2020    |
\*-----------------------------------------*/

#pragma once

#include <vector>
#include <string>
#include <mutex>

typedef void (*FanUpdateCallback)(void* receiver);
typedef std::pair<FanUpdateCallback, void*> FanUpdateBlock;

/*------------------------------------------------------------------*\
| Fan Type                                                           |
\*------------------------------------------------------------------*/
typedef struct
{
    /*--------------------------------------------------------------*\
    | Fan Information                                                |
    \*--------------------------------------------------------------*/
    std::string         name;           /* Fan name                 */
    unsigned int        speed_cmd;      /* Speed command            */
    unsigned int        speed_min;      /* Minimum speed command    */
    unsigned int        speed_max;      /* Maximum speed command    */
} fan;

class FanController
{
public:
    std::string             name;           /* controller name          */
    std::string             description;    /* controller description   */
    std::string             version;        /* controller version       */
    std::string             serial;         /* controller serial number */
    std::string             location;       /* controller location      */
    std::vector<fan>        fans;           /* Fans                     */
    std::vector<FanUpdateBlock> update_callbacks; /* Update event callbacks */
    std::mutex update_callback_mutex;

    /*---------------------------------------------------------*\
    | FanController base class constructor                      |
    \*---------------------------------------------------------*/
    //FanController();
    //~FanController();

    /*---------------------------------------------------------*\
    | Functions to be implemented in device implementation      |
    \*---------------------------------------------------------*/
    virtual void UpdateControl()                                        = 0;
    virtual void UpdateReading()                                        = 0;
    virtual void RegisterUpdateCallback(FanUpdateCallback callback, void* receiver) final;
    virtual void UnregisterUpdateCallback(FanUpdateCallback callback, void* receiver) final;
    virtual void ClearCallbacks() final;
};
