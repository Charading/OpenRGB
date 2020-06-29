#include "CMMP750Controller.h"
#include "CMMKController.h"
#include "RGBController.h"
#include "RGBController_CMMP750Controller.h"
#include "RGBController_CMMKController.h"
#include <hidapi/hidapi.h>

#define COOLERMASTER_VID                        0x2516

#define COOLERMASTER_MP750_XL_PID               0x0109
#define COOLERMASTER_MP750_MEDIUM_PID           0x0105
#define COOLERMASTER_MASTERKEYS_PRO_L_PID       0x003b
#define COOLERMASTER_MASTERKEYS_PRO_L_WHITE_PID 0x0047
#define COOLERMASTER_MASTERKEYS_PRO_S_PID       0x003c
#define COOLERMASTER_MASTERKEYS_MK750_PID       0x0067
#define COOLERMASTER_MASTERKEYS_SK630_PID       0x0089

#define COOLERMASTER_NUM_DEVICES (sizeof(cm_pids) / sizeof(cm_pids[ 0 ]))

enum
{
    CM_PID = 0,
    CM_INTERFACE = 1,
    CM_KIND = 2
};

enum DEVICE_KIND: unsigned int
{
    Mousepad = 0,
    Keyboard = 1
};

static const unsigned int cm_pids[][4] =
{  // PID,      Interface
    { COOLERMASTER_MP750_XL_PID,               0x00, DEVICE_KIND::Mousepad },     //Coolermaster MP750 (Extra Large)
    { COOLERMASTER_MP750_MEDIUM_PID,           0x00, DEVICE_KIND::Mousepad },     //Coolermaster MP750 (Medium)

    { COOLERMASTER_MASTERKEYS_PRO_L_PID,       0x01, DEVICE_KIND::Keyboard },     //Coolermaster MasterKeys Pro L
    { COOLERMASTER_MASTERKEYS_PRO_L_WHITE_PID, 0x01, DEVICE_KIND::Keyboard },     //Coolermaster MasterKeys Pro L White
    { COOLERMASTER_MASTERKEYS_PRO_S_PID,       0x01, DEVICE_KIND::Keyboard },     //Coolermaster MasterKeys Pro S
    { COOLERMASTER_MASTERKEYS_MK750_PID,       0x01, DEVICE_KIND::Keyboard },     //Coolermaster MasterKeys MK750
    { COOLERMASTER_MASTERKEYS_SK630_PID,       0x01, DEVICE_KIND::Keyboard }      //Coolermaster MasterKeys SK630
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

    //Look for the passed in cm_pids
    hid_init();
    info = hid_enumerate(COOLERMASTER_VID, 0x0);

    while(info)
    {
        hid_device* dev = NULL;

        int cm_pid_idx;

        RGBController* controller = nullptr;

        for(cm_pid_idx = 0; cm_pid_idx < COOLERMASTER_NUM_DEVICES; cm_pid_idx++)
        {
            if((info->product_id        == cm_pids[cm_pid_idx][CM_PID])
                &&(info->interface_number  == cm_pids[cm_pid_idx][CM_INTERFACE]))
            {
                if (cm_pids[cm_pid_idx][CM_KIND] == DEVICE_KIND::Mousepad)
                {
                    if (dev = hid_open_path(info->path))
                    {
                        CMMP750Controller* pad_controller = new CMMP750Controller(dev, info->manufacturer_string, info->product_string, info->path);
                        controller = new RGBController_CMMP750Controller(pad_controller);
                    }
                }
                else if (cm_pids[cm_pid_idx][CM_KIND] == DEVICE_KIND::Keyboard)
                {
                    CMMKController* mk_controller = new CMMKController(info, info->manufacturer_string, info->product_string, info->path);
                    controller = new RGBController_CMMKController(mk_controller);
                }

                break;
            }
        }

        if (controller)
        {
            rgb_controllers.push_back(controller);
        }

        info = info->next;
    }

    hid_free_enumeration(info);
}
