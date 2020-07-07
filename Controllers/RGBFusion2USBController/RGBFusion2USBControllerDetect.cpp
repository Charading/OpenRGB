#include "RGBFusion2USBController.h"
#include "RGBController_RGBFusion2USB.h"
#include "dependencies/dmiinfo.h"

#define IT8297_VID 0x048D
#define IT8297_PID 0x8297
#define IT8297_PID_B550 0x5702      //Temporary name till we get more details
#define IT8297_IFC 0
#define IT8297_UPG 0xFF89

/******************************************************************************************\
*                                                                                          *
*   DetectRGBFusion2USBControllers                                                         *
*                                                                                          *
*       Detect RGB Fusion 2 devices that use IT8297 RGB controller                         *
*                                                                                          *
\******************************************************************************************/

void DetectRGBFusion2USBControllers(std::vector<RGBController*> &rgb_controllers)
{
    DMIInfo MB_info;

    if (hid_init() < 0)
        return;

    hid_device_info * info = hid_enumerate(IT8297_VID, IT8297_PID);
    if (!info)
        info = hid_enumerate(IT8297_VID, IT8297_PID_B550);   //again will clean up later with more details

    while(info)
    {
        if((info->vendor_id == IT8297_VID)
            &&(info->interface_number == IT8297_IFC)
#ifdef USE_HID_USAGE
            &&(info->product_id == IT8297_PID)
            &&(info->usage_page == IT8297_UPG))
#else
            &&(info->product_id == IT8297_PID))
#endif
        {
            hid_device * dev = hid_open_path(info->path);
            if (dev)
            {
                RGBFusion2USBController * controller = new RGBFusion2USBController(dev, info->path, MB_info.getMainboard());
                RGBController_RGBFusion2USB * rgb_controller = new RGBController_RGBFusion2USB(controller);
                rgb_controllers.push_back(rgb_controller);
            }
        }
        info = info->next;
    }

    hid_free_enumeration(info);
}
