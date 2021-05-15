/*-----------------------------------------*\
|  FanController_Network.h                  |
|                                           |
|  Generic Fan Interface Network Class      |
|                                           |
|  swilde 5/2021                            |
\*-----------------------------------------*/

#pragma once

#include "FanController.h"
#include "NetworkClient.h"

class FanController_Network : public FanController
{
public:
    FanController_Network(NetworkClient * client_ptr, unsigned int dev_idx_val);

    void        UpdateControl();
    void        UpdateReading();

private:
    NetworkClient *     client;
    unsigned int        dev_idx;
};
