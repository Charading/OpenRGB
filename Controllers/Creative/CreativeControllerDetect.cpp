#include "CreativeSoundBlasterG6XController.h"
#include "RGBController_CreativeSoundBlasterG6X.h"
#include "Detector.h"
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| Creative vendor ID                                    |
\*-----------------------------------------------------*/
#define CREATIVE_VID                            0x041E
/*-----------------------------------------------------*\
| SoundCards                                            |
\*-----------------------------------------------------*/
#define CREATIVE_SOUNDBLASTERX_G6_PID           0x3256

void DetectCreativeDevice(hid_device_info* info, const std::string&)
{
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        CreativeSoundBlasterG6XController* controller = new CreativeSoundBlasterG6XController(dev, info->path);
        RGBController_CreativeSoundBlasterG6X* rgb_controller = new RGBController_CreativeSoundBlasterG6X(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------*\
| Sound Cards                                                                                                                                       |
\*-------------------------------------------------------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_I("Creative SoundBlasterX G6",               DetectCreativeDevice,         CREATIVE_VID,     CREATIVE_SOUNDBLASTERX_G6_PID, 4);
