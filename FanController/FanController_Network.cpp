/*-----------------------------------------*\
|  FanController_Network.cpp                |
|                                           |
|  Generic Fan Interface Network Class      |
|                                           |
|  swilde 5/2021                            |
\*-----------------------------------------*/

#include <cstring>

#include "FanController_Network.h"

FanController_Network::FanController_Network(NetworkClient * client_ptr, unsigned int dev_idx_val)
{
    client  = client_ptr;
    dev_idx = dev_idx_val;
}

void FanController_Network::UpdateControl()
{
    unsigned char * data = GetFansCmdDescription();
    unsigned int size;

    memcpy(&size, &data[0], sizeof(unsigned int));

    client->SendRequest_FanController_UpdateControl(dev_idx, data, size);

    delete[] data;
}

void FanController_Network::UpdateReading()
{
    client->SendRequest_FanController_Reading(dev_idx);
}
