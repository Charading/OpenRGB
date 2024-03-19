/*-----------------------------------------*\
|  RGBController_uDMX.h                     |
|                                           |
|  Generic RGB Interface for uDMX interface |
|                                           |
|  Francesco Goicochea                      |
|  Bartholomew Ho (imnotmental) 03/20/2024  |
\*-----------------------------------------*/

#include <RGBController.h>
#include <uDMXController.h>

class RGBController_uDMX : public RGBController
{
public:

    RGBController_uDMX(uDMXController* controller_ptr);
    ~RGBController_uDMX();

    void SetupZones();
    void ResizeZone(int /*zone*/, int /*new_size*/);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int /*zone*/);
    void UpdateSingleLED(int /*led*/);

    void DeviceUpdateMode();

private:
    uDMXController*        controller;
};
