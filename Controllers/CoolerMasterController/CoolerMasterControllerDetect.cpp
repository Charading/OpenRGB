#include "CMMP750Controller.h"
#include "RGBController.h"
#include "RGBController_CMMP750Controller.h"
#include <hidapi/hidapi.h>

#define COOLERMASTER_VID 0x2516
#define COOLERMASTER_NUM_DEVICES (sizeof(cm_pids) / sizeof(cm_pids[ 0 ]))

enum
{
    CM_PID = 0,
    CM_INADDR = 1,
    CM_OUTADDR = 2,
    CM_INTERFACE = 3
};

static const unsigned int cm_pids[][4] =
{ //PID, inAddr, outAddr, interface
    { 0x0109, 0x82, 0x03, 0x00 }        //Coolermaster MP750
};

/******************************************************************************************\
*                                                                                          *
*   DetectCoolerMasterControllers                                                          *
*                                                                                          *
*       Tests the USB address to see if any CoolerMaster controllers exists there.         *
*                                                                                          *
\******************************************************************************************/

void DetectCoolerMasterControllers(std::vector<RGBController*>& rgb_controllers)
{
    hid_device_info* info;
    hid_device* dev = NULL;

    for(int cm_pid_idx = 0; cm_pid_idx < COOLERMASTER_NUM_DEVICES; cm_pid_idx++)
    {
        //Look for the passed in cm_pids
        hid_init();
        info = hid_enumerate(0x0, 0x0);

        while(info)
        {
            if((info->vendor_id == COOLERMASTER_VID)
             &&(info->product_id == cm_pids[cm_pid_idx][CM_PID])
             &&(info->interface_number == cm_pids[cm_pid_idx][CM_INTERFACE]))
            {
                dev = hid_open_path(info->path);
                break;
            }
            else
            {
                info = info->next;
            }
        }

        if(dev)
        {
           { // Success: Device was found
                CMMP750Controller* controller = new CMMP750Controller(dev, info->manufacturer_string, info->product_string, info->path);
                RGBController_CMMP750Controller* rgb_controller = new RGBController_CMMP750Controller(controller);
                rgb_controllers.push_back(rgb_controller);
            }
        }

        hid_free_enumeration(info);
    }
}
