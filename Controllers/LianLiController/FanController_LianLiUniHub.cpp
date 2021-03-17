/*-----------------------------------------*\
|  FanController_LianLiUniHub.hpp.h         |
|                                           |
|  Generic Fan Interface for Lian Li Uni    |
|  Hub controller                           |
|                                           |
|  Luca Lovisa 3/09/2021                    |
\*-----------------------------------------*/

#include "FanController_LianLiUniHub.h"
#include "LianLiUniHubController.h"

/* TODO FANS
#include <cstdio>

namespace {

fan makeFan() {
  fan Fan;

  Fan.speed_cmd = 0;
  Fan.speed_min = 0;
  Fan.speed_max = 0;
  Fan.rpm_rdg   = 0;

  return Fan;
}

}  // namespace

FanController_LianLiUniHub::FanController_LianLiUniHub(std::shared_ptr<LianLiUniHubController> uniHub_ptr)
{
    uniHub = uniHub_ptr;

    name        = "Lian Li Uni Hub";
//    vendor      = "Lian Li";
    version     = uniHub->GetVersion();
//    type        = DEVICE_TYPE_COOLER;
    description = "Lian Li Uni Hub";
    location    = uniHub->GetLocation();
    serial      = uniHub->GetSerial();

    for (size_t i = 0; i < UNIHUB_CHANNEL_COUNT; i++)
    {
        fan Fan = makeFan();
        Fan.name = "Channel ";
        Fan.name.append(std::to_string(i));

        Fan.speed_cmd = 0;
        Fan.speed_min = 0;
        Fan.speed_max = 3;

        fans.push_back(Fan);
    }
}

void FanController_LianLiUniHub::UpdateControl()
{
    std::lock_guard<std::recursive_mutex> lock(uniHub->GetMutex());

    bool synchronize = false;

    for (size_t channel = 0; channel < fans.size(); channel++)
    {
        uint16_t fanSpeed = convertFanSpeed(fans[channel].speed_cmd);

        if (uniHub->GetFanSpeed(channel) != fanSpeed) {
            uniHub->SetFanSpeed(channel, fanSpeed);
            synchronize = true;
        }
    }

    if (synchronize) {
        uniHub->Synchronize();
    }
}

void FanController_LianLiUniHub::UpdateReading()
{
    std::lock_guard<std::recursive_mutex> lock(uniHub->GetMutex());

    for (size_t channel = 0; channel < fans.size(); channel++)
    {
        fans[channel].rpm_rdg = uniHub->ReadFanSpeed(channel);
    }
}

uint16_t FanController_LianLiUniHub::convertFanSpeed(
        uint16_t speed
) {
    switch (speed)
    {
        case 0: return UNIHUB_FAN_SPEED_QUIET;
        case 1: return UNIHUB_FAN_SPEED_HIGH_SPEED;
        case 2: return UNIHUB_FAN_SPEED_FULL_SPEED;
        case 3: return UNIHUB_FAN_SPEED_PWM;

        default:
            return UNIHUB_FAN_SPEED_QUIET;
    }
}

*/
