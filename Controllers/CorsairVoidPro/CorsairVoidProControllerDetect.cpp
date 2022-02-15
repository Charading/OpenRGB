#include "Detector.h"
#include "CorsairVoidProController.h"
#include "RGBController.h"
#include "RGBController_CorsairVoidPro.h"

#include <hidapi/hidapi.h>
#include <unordered_set>

#include "LogManager.h"

/*---------------------------------------------------------*\
| Corsair vendor ID                                         |
\*---------------------------------------------------------*/
#define CORSAIR_VID                                    0x1B1C

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define CORSAIR_VOID_PRO_PID                           0x0A14

/*---------------------------------------------------------*\
| IPU                                                       |
\*---------------------------------------------------------*/
#define CORSAIR_VOID_PRO_LIGHT_CONTROL_USAGE             0x01
#define CORSAIR_VOID_PRO_LIGHT_DATA_USAGE                0xCB
#define CORSAIR_VOID_PRO_USAGE_PAGE                    0xFFC5

/*---------------------------------------------------------------------*\
| Tracks the paths used in DetectCorsairVoidProControllers so multiple  |
| devices can be detected without all controlling the same device.      |
\*---------------------------------------------------------------------*/
static std::unordered_set<std::string> corsair_void_pro_used_paths;

/*--------------------------------------------------------------------------------*\
| Removes all entries in used_paths so device discovery does not skip any of them. |
\*--------------------------------------------------------------------------------*/
void ResetCorsairVoidProControllersPaths()
{
    corsair_void_pro_used_paths.clear();
}

void DetectCorsairVoidProControllers(hid_device_info* info, const std::string& name)
{
    /*-------------------------------------------------------------------------------------------------*\
    | Corsair Void Pro controller uses multiple usages                                                  |
    \*-------------------------------------------------------------------------------------------------*/
    hid_device* dev_lights_control = nullptr;
    hid_device* dev_lights_data    = nullptr;
    hid_device_info* info_full     = hid_enumerate(CORSAIR_VID, CORSAIR_VOID_PRO_PID);
    hid_device_info* info_temp     = info_full;

    /*--------------------------------------------------------------------------------------------*\
    | Keep track of paths so they can be added to used_paths only if both interfaces can be found. |
    \*--------------------------------------------------------------------------------------------*/
    std::string dev_interface_lights_control_path;
    std::string dev_interface_lights_data_path;

     LOG_DEBUG("[CorsairVoidProControllerDetect] DetectCorsairVoidProControllers ------- \n IPU %02X,%04X,%02X VID:PID %04X:%04X",
              info->interface_number,
              info->usage_page,
              info->usage,
              info->vendor_id,
              info->product_id
              );

    while(info_temp)
    {
        /*----------------------------------------------------------------------------*\
        | Check for paths used on an already registered ARGB controller to avoid       |
        | registering multiple controllers that refer to the same physical hardware.   |
        \*----------------------------------------------------------------------------*/

        LOG_DEBUG("[CorsairVoidProControllerDetect]: enumerations: IPU %02X,%04X,%02X VID:PID %04X:%04X",
                 info_temp->interface_number,
                 info_temp->usage_page,
                 info_temp->usage,
                 info_temp->vendor_id,
                 info_temp->product_id
                 );


        if(info_temp->vendor_id                                 == info->vendor_id
        && info_temp->product_id                                == info->product_id
        && corsair_void_pro_used_paths.find(info_temp->path)    == corsair_void_pro_used_paths.end() )
        {
            if(info_temp->usage_page == CORSAIR_VOID_PRO_LIGHT_CONTROL_USAGE)
            {
                LOG_DEBUG("[CorsairVoidProControllerDetect]: Found CORSAIR_VOID_PRO_LIGHT_CONTROL_USAGE");
                dev_lights_control = hid_open_path(info_temp->path);
                dev_interface_lights_control_path = info_temp->path;
            }
            else if(info_temp->usage_page == CORSAIR_VOID_PRO_LIGHT_DATA_USAGE)
            {
                LOG_DEBUG("[CorsairVoidProControllerDetect]: Found CORSAIR_VOID_PRO_LIGHT_DATA_USAGE");

                dev_lights_data = hid_open_path(info_temp->path);
                dev_interface_lights_data_path = info_temp->path;
            }
        }

        if(dev_lights_control && dev_lights_data)
        {
            LOG_DEBUG("[CorsairVoidProControllerDetect]: Found both devices");
            break;
        }

        info_temp = info_temp->next;
    }

    hid_free_enumeration(info_full);

    if(dev_lights_control && dev_lights_data)
    {
        CorsairVoidProController*     controller      = new CorsairVoidProController(dev_lights_control, dev_lights_data, *info);
        RGBController_CorsairVoidPro* rgb_controller  = new RGBController_CorsairVoidPro(controller);
        rgb_controller->name                          = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);

        corsair_void_pro_used_paths.insert(dev_interface_lights_control_path);
        corsair_void_pro_used_paths.insert(dev_interface_lights_data_path);
    }
    else
    {
        LOG_DEBUG("[CorsairVoidProControllerDetect]: could not find all required devices");
        // Not all of them could be opened, do some cleanup
        hid_close(dev_lights_control);
        hid_close(dev_lights_data);
    }
}

REGISTER_HID_DETECTOR_IP("Corsair Void Pro", DetectCorsairVoidProControllers, CORSAIR_VID, CORSAIR_VOID_PRO_PID, 3, CORSAIR_VOID_PRO_USAGE_PAGE);//, 0xFFC5, 0xCB);
REGISTER_PRE_DETECTION_HOOK(ResetCorsairVoidProControllersPaths);
