/*-----------------------------------------*\
|  RGBController_LianLiUniHub.h             |
|                                           |
|  Generic RGB Interface for Lian Li Uni    |
|  Hub USB controller driver                |
|                                           |
|  Luca Lovisa 2/20/2021                    |
\*-----------------------------------------*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "LianLiUniHubController.h"
#include "RGBController.h"

class RGBController_LianLiUniHub : public RGBController
{
public:
    RGBController_LianLiUniHub(std::shared_ptr<LianLiUniHubController> uniHub_ptr);

public:
    /* RGBController interface */
    void SetupZones()                       override;

    void ResizeZone(int zone, int new_size) override;

    void DeviceUpdateLEDs()                 override;
    void UpdateZoneLEDs(int zone)           override;
    void UpdateSingleLED(int led)           override;

    void DeviceUpdateMode()                 override;

    void SetCustomMode()                    override;

private:
    /* Convert from "OpenRGB units" to "Uni Hub units" */
    uint8_t convertAnyFanCount(
            uint8_t count
    );
    uint8_t convertLedSpeed(
            uint8_t speed
    );
    uint8_t convertLedDirection(
            uint8_t direction
    );

    uint8_t convertLedCountToFanCount(
            uint8_t count
    );

private:
    std::shared_ptr<LianLiUniHubController> uniHub;
    bool                                    initializedMode;
};
