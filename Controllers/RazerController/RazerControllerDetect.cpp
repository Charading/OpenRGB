/*---------------------------------------------------------*\
| RazerControllerDetect.cpp                                 |
|                                                           |
|   Detector for Razer devices                              |
|                                                           |
|   Adam Honse (CalcProgrammer1)                22 Jan 2021 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <unordered_set>
#include <hidapi.h>
#include "Detector.h"
#include "LogManager.h"
#include "RazerController.h"
#include "RazerKrakenController.h"
#include "RazerDevices.h"
#include "ResourceManager.h"
#include "RGBController.h"
#include "RGBController_Razer.h"
#include "RGBController_RazerAddressable.h"
#include "RGBController_RazerKraken.h"

/******************************************************************************************\
*                                                                                          *
*   DetectRazerControllers                                                                 *
*                                                                                          *
*       Tests the USB address to see if a Razer controller exists there.                   *
*                                                                                          *
\******************************************************************************************/

void DetectRazerControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        RazerController* controller = new RazerController(dev, dev, info->path, info->product_id, name);

        RGBController_Razer* rgb_controller = new RGBController_Razer(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}   /* DetectRazerControllers() */

/******************************************************************************************\
*                                                                                          *
*   DetectRazerARGBControllers                                                             *
*                                                                                          *
*       Tests the USB address to see if a Razer ARGB controller exists there.              *
*                                                                                          *
\******************************************************************************************/


/*---------------------------------------------------------------------*\
| Tracks the paths used in DetectRazerARGBControllers so multiple Razer |
| devices can be detected without all controlling the same device.      |
\*---------------------------------------------------------------------*/
static std::unordered_set<std::string> used_paths;

/*--------------------------------------------------------------------------------*\
| Removes all entries in used_paths so device discovery does not skip any of them. |
\*--------------------------------------------------------------------------------*/
void ResetRazerARGBControllersPaths()
{
    used_paths.clear();
}

void DetectRazerARGBControllers(hid_device_info* info, const std::string& name)
{
    /*-------------------------------------------------------------------------------------------------*\
    | Razer's ARGB controller uses two different interfaces, one for 90-byte Razer report packets and   |
    | one for 320-byte ARGB packets.  Interface 0 for 90-byte and interface 1 for 320-byte.             |
    |                                                                                                   |
    | Create a local copy of the HID enumerations for the Razer ARGB controller VID/PID and iterate     |
    | through it.  This prevents detection from failing if interface 1 comes before interface 0 in the  |
    | main info list.                                                                                   |
    \*-------------------------------------------------------------------------------------------------*/
     hid_device* dev_interface_0 = nullptr;
     hid_device* dev_interface_1 = nullptr;
     hid_device_info* info_full = hid_enumerate(RAZER_VID, RAZER_CHROMA_ADDRESSABLE_RGB_CONTROLLER_PID);
     hid_device_info* info_temp = info_full;
    /*--------------------------------------------------------------------------------------------*\
    | Keep track of paths so they can be added to used_paths only if both interfaces can be found. |
    \*--------------------------------------------------------------------------------------------*/
     std::string dev_interface_0_path;
     std::string dev_interface_1_path;

     while(info_temp)
     {
        /*----------------------------------------------------------------------------*\
        | Check for paths used on an already registered Razer ARGB controller to avoid |
        | registering multiple controllers that refer to the same physical hardware.   |
        \*----------------------------------------------------------------------------*/
         if(info_temp->vendor_id             == info->vendor_id
         && info_temp->product_id            == info->product_id
         && used_paths.find(info_temp->path) == used_paths.end() )
         {
             if(info_temp->interface_number == 0)
             {
                 dev_interface_0 = hid_open_path(info_temp->path);
                 dev_interface_0_path = info_temp->path;
             }
             else if(info_temp->interface_number == 1)
             {
                 dev_interface_1 = hid_open_path(info_temp->path);
                 dev_interface_1_path = info_temp->path;
             }
         }
         if(dev_interface_0 && dev_interface_1)
         {
             break;
         }
         info_temp = info_temp->next;
     }

     hid_free_enumeration(info_full);

     if(dev_interface_0 && dev_interface_1)
     {
         RazerController* controller                    = new RazerController(dev_interface_0, dev_interface_1, info->path, info->product_id, name);
         RGBController_RazerAddressable* rgb_controller = new RGBController_RazerAddressable(controller);
         ResourceManager::get()->RegisterRGBController(rgb_controller);
         used_paths.insert(dev_interface_0_path);
         used_paths.insert(dev_interface_1_path);
     }
     else
     {
         // Not all of them could be opened, do some cleanup
         hid_close(dev_interface_0);
         hid_close(dev_interface_1);
     }
}

/******************************************************************************************\
*                                                                                          *
*   DetectRazerKrakenController                                                            *
*                                                                                          *
*       Tests the USB address to see if a Razer Kraken controller exists there.            *
*                                                                                          *
\******************************************************************************************/

void DetectRazerKrakenControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        RazerKrakenController* controller = new RazerKrakenController(dev, info->path, info->product_id, name);

        RGBController_RazerKraken* rgb_controller = new RGBController_RazerKraken(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}   /* DetectRazerKrakenControllers() */

/*-----------------------------------------------------------------------------------------------------*\
| Keyboards                                                                                             |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow 2019",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_2019_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow Chroma",                         DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_CHROMA_PID,                    0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow Chroma Tournament Edition",      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_CHROMA_TE_PID,                 0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow Chroma V2",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_CHROMA_V2_PID,                 0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow Elite",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_ELITE_PID,                     0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow Overwatch",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_OVERWATCH_PID,                 0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_PID,                        0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3 Pro (Wired)",                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_PRO_WIRED_PID,              0x02,   0x01,   0x02);
// REGISTER_HID_DETECTOR_PU ("Razer Blackwidow V3 Pro (Bluetooth)",             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_PRO_BLUETOOTH_PID,          0x01,   0x00);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3 Pro (Wireless)",              DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_PRO_WIRELESS_PID,           0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3 TKL",                         DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_TKL_PID,                    0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3 Mini (Wired)",                DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_MINI_WIRED_PID,             0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V3 Mini (Wireless)",             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V3_MINI_WIRELESS_PID,          0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V4",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V4_PID,                        0x03,   0x01,   0x00);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V4 Pro",                         DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V4_PRO_PID,                    0x03,   0x01,   0x00);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow V4 X",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_V4_X_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow X Chroma",                       DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_X_CHROMA_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blackwidow X Chroma Tournament Edition",    DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLACKWIDOW_X_CHROMA_TE_PID,               0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cynosa Chroma",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_CYNOSA_CHROMA_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cynosa Chroma V2",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_CYNOSA_V2_PID,                            0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cynosa Lite",                               DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_CYNOSA_LITE_PID,                          0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker Chroma",                       DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_CHROMA_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker V2",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_V2_PID,                      0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker V2 Pro TKL (Wired)",           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_V2_PRO_TKL_WIRED_PID,        0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker V2 Pro TKL (Wireless)",        DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_V2_PRO_TKL_WIRELESS_PID,     0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker V2 Pro (Wired)",               DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_V2_PRO_WIRED_PID,            0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Deathstalker V2 Pro (Wireless)",            DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHSTALKER_V2_PRO_WIRELESS_PID,         0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman",                                  DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_PID,                             0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman Elite",                            DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_ELITE_PID,                       0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman Mini",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_MINI_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman Mini Analog",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_MINI_ANALOG_PID,                 0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman Tournament Edition",               DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_TE_PID,                          0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman V2 Analog",                        DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_V2_ANALOG_PID,                   0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman V2 TKL",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_V2_TKL_PID,                      0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Huntsman V2",                               DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_HUNTSMAN_V2_PID,                          0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Ornata Chroma",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_CHROMA_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata Chroma V2",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_CHROMA_V2_PID,                     0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata V3",                                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_V3_PID,                            0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata V3 Rev2",                            DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_V3_REV2_PID,                       0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata V3 TKL",                             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_V3_TKL_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata V3 X",                               DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_V3_X_PID,                          0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Ornata V3 X Rev2",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_ORNATA_V3_X_REV2_PID,                     0x02,   0x01,   0x02);
/*-----------------------------------------------------------------------------------------------------*\
| Laptops                                                                                               |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Blade (2016)",                              DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2016_PID,                           0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade (Late 2016)",                         DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_LATE_2016_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 14 (2021)",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_14_2021_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 14 (2022)",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_14_2022_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 14 (2023)",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_14_2023_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2022)",                           DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_15_2022_PID,                        0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2018 Advanced)",                  DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2018_ADVANCED_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2018 Base)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2018_BASE_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2018 Mercury)",                   DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2018_MERCURY_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2019 Advanced)",                  DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2019_ADVANCED_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2019 Base)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2019_BASE_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2019 Mercury)",                   DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2019_MERCURY_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2019 Studio)",                    DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2019_STUDIO_PID,                    0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2020 Advanced)",                  DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2020_ADVANCED_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2020 Base)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2020_BASE_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (Late 2020)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_LATE_2020_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2021 Advanced)",                  DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2021_ADVANCED_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (Late 2021 Advanced)",             DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_LATE_2021_ADVANCED_PID,             0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2021 Base)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2021_BASE_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade 15 (2021 Base)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_2021_BASE_V2_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro (2016)",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_2016_PID,                       0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro (2017)",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_2017_PID,                       0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro (2017 FullHD)",                   DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_2017_FULLHD_PID,                0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro (2019)",                          DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_2019_PID,                       0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro (Late 2019)",                     DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_LATE_2019_PID,                  0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro 17 (2020)",                       DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_17_2020_PID,                    0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Pro 17 (2021)",                       DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_PRO_17_2021_PID,                    0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (2016)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_2016_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (Late 2016)",                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_LATE_2016_PID,              0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (2017)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_2017_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (Late 2017)",                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_LATE_2017_PID,              0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (2019)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_2019_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (Late 2019)",                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_LATE_2019_PID,              0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (2020)",                      DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_2020_PID,                   0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Blade Stealth (Late 2020)",                 DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BLADE_STEALTH_LATE_2020_PID,              0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Book 13 (2020)",                            DEVICE_TYPE_KEYBOARD, DetectRazerControllers,        RAZER_VID,  RAZER_BOOK_13_2020_PID,                         0x02,   0x01,   0x02);

/*-----------------------------------------------------------------------------------------------------*\
| Mice                                                                                                  |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Abyssus Elite D.Va Edition",                DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_ABYSSUS_ELITE_DVA_EDITION_PID,            0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Abyssus Essential",                         DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_ABYSSUS_ESSENTIAL_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk",                                  DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_PID,                             0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk Essential",                        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_ESSENTIAL_PID,                   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk Ultimate (Wired)",                 DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_ULTIMATE_WIRED_PID,              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk Ultimate (Wireless)",              DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_ULTIMATE_WIRELESS_PID,           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk V2",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V2_PID,                          0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk V3",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V3_PID,                          0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk V3 Pro (Wired)",                   DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V3_PRO_WIRED_PID,                0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk V3 Pro (Wireless)",                DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V3_PRO_WIRELESS_PID,             0x00,   0x01,   0x02);
// REGISTER_HID_DETECTOR_PU ("Razer Basilisk V3 Pro (Bluetooth)",               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V3_PRO_BLUETOOTH_PID,                    0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Basilisk V3 X HyperSpeed",                  DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_BASILISK_V3_X_HYPERSPEED_PID,             0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cobra",                                     DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_COBRA_PID,                                0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cobra Pro (Wired)",                         DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_COBRA_PRO_WIRED_PID,                      0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Cobra Pro (Wireless)",                      DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_COBRA_PRO_WIRELESS_PID,                   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder Chroma",                         DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_CHROMA_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder Elite",                          DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_ELITE_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder Essential",                      DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_ESSENTIAL_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder Essential V2",                   DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_ESSENTIAL_V2_PID,              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder Essential White Edition",        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_ESSENTIAL_WHITE_EDITION_PID,   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder V2",                             DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_V2_PID,                        0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder V2 Mini",                        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_V2_MINI_PID,                   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder V2 Pro (Wired)",                 DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_V2_PRO_WIRED_PID,              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Deathadder V2 Pro (Wireless)",              DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DEATHADDER_V2_PRO_WIRELESS_PID,           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Diamondback",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_DIAMONDBACK_CHROMA_PID,                   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Lancehead 2017 (Wired)",                    DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_LANCEHEAD_2017_WIRED_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Lancehead 2017 (Wireless)",                 DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_LANCEHEAD_2017_WIRELESS_PID,              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Lancehead 2019 (Wired)",                    DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_LANCEHEAD_2019_WIRED_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Lancehead 2019 (Wireless)",                 DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_LANCEHEAD_2019_WIRELESS_PID,              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Lancehead Tournament Edition",              DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_LANCEHEAD_TE_WIRED_PID,                   0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2012 (Wired)",                        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2012_WIRED_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2012 (Wireless)",                     DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2012_WIRELESS_PID,                  0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2015 (Wired)",                        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2015_WIRED_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2015 (Wireless)",                     DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2015_WIRELESS_PID,                  0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2018 (Wired)",                        DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2018_WIRED_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba 2018 (Wireless)",                     DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_2018_WIRELESS_PID,                  0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba Elite",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_ELITE_PID,                          0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba Hyperflux (Wired)",                   DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_HYPERFLUX_PID,                      0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mamba Tournament Edition",                  DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_MAMBA_TE_PID,                             0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Chroma",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_CHROMA_PID,                          0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Classic",                              DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_CLASSIC_PID,                         0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Epic Chroma",                          DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_EPIC_CHROMA_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Left Handed",                          DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_LEFT_HANDED_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Hex V2",                               DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_HEX_V2_PID,                          0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Trinity",                              DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_TRINITY_PID,                         0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Pro (Wired)",                          DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_PRO_WIRED_PID,                       0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Pro (Wireless)",                       DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_PRO_WIRELESS_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Pro V2 (Wired)",                       DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_PRO_V2_WIRED_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Naga Pro V2 (Wireless)",                    DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_NAGA_PRO_V2_WIRELESS_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Viper",                                     DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_VIPER_PID,                                0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Viper 8kHz",                                DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_VIPER_8KHZ_PID,                           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Viper Mini",                                DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_VIPER_MINI_PID,                           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Viper Ultimate (Wired)",                    DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_VIPER_ULTIMATE_WIRED_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Viper Ultimate (Wireless)",                 DEVICE_TYPE_MOUSE, DetectRazerControllers,        RAZER_VID,  RAZER_VIPER_ULTIMATE_WIRELESS_PID,              0x00,   0x01,   0x02);

/*-----------------------------------------------------------------------------------------------------*\
| Keypads                                                                                               |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Orbweaver Chroma",                          DEVICE_TYPE_KEYPAD, DetectRazerControllers,        RAZER_VID,  RAZER_ORBWEAVER_CHROMA_PID,                     0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Tartarus Chroma",                           DEVICE_TYPE_KEYPAD, DetectRazerControllers,        RAZER_VID,  RAZER_TARTARUS_CHROMA_PID,                      0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Tartarus Pro",                              DEVICE_TYPE_KEYPAD, DetectRazerControllers,        RAZER_VID,  RAZER_TARTARUS_PRO_PID,                         0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Tartarus V2",                               DEVICE_TYPE_KEYPAD, DetectRazerControllers,        RAZER_VID,  RAZER_TARTARUS_V2_PID,                          0x02,   0x01,   0x02);

/*-----------------------------------------------------------------------------------------------------*\
| Headsets                                                                                              |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Kraken 7.1",                                DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_CLASSIC_PID,                       0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Kraken 7.1",                                DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_CLASSIC_ALT_PID,                   0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Kraken 7.1 Chroma",                         DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_PID,                               0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Kraken 7.1 V2",                             DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_V2_PID,                            0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Kraken Kitty Edition",                      DEVICE_TYPE_HEADSET, DetectRazerControllers,        RAZER_VID,  RAZER_KRAKEN_KITTY_EDITION_PID,                 0x01,   0x01,   0x03);
REGISTER_HID_DETECTOR_IPU("Razer Kraken Kitty Black Edition",                DEVICE_TYPE_HEADSET, DetectRazerControllers,        RAZER_VID,  RAZER_KRAKEN_KITTY_BLACK_EDITION_PID,           0x01,   0x01,   0x03);
REGISTER_HID_DETECTOR_IPU("Razer Kraken Kitty Black Edition V2",             DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_KITTY_BLACK_EDITION_V2_PID,        0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Kraken Ultimate",                           DEVICE_TYPE_HEADSET, DetectRazerKrakenControllers,  RAZER_VID,  RAZER_KRAKEN_ULTIMATE_PID,                      0x03,   0x0C,   0x01);
REGISTER_HID_DETECTOR_I(  "Razer Tiamat 7.1 V2",                             DEVICE_TYPE_HEADSET, DetectRazerControllers,        RAZER_VID,  RAZER_TIAMAT_71_V2_PID,                         0x00                );

/*-----------------------------------------------------------------------------------------------------*\
| Mousemats                                                                                             |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Firefly",                                   DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_FIREFLY_PID,                              0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Firefly V2",                                DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_FIREFLY_V2_PID,                           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Firefly Hyperflux",                         DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_FIREFLY_HYPERFLUX_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Goliathus",                                 DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_GOLIATHUS_CHROMA_PID,                     0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Goliathus Chroma 3XL",                      DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_GOLIATHUS_CHROMA_3XL_PID,                 0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Goliathus Extended",                        DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_GOLIATHUS_CHROMA_EXTENDED_PID,            0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Strider Chroma",                            DEVICE_TYPE_MOUSEMAT, DetectRazerControllers,        RAZER_VID,  RAZER_STRIDER_CHROMA_PID,                       0x00,   0x01,   0x02);

/*-----------------------------------------------------------------------------------------------------*\
| Accessories                                                                                           |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IPU("Razer Base Station Chroma",                       DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_BASE_STATION_CHROMA_PID,                  0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Base Station V2 Chroma",                    DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_BASE_STATION_V2_CHROMA_PID,               0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Charging Pad Chroma",                       DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CHARGING_PAD_CHROMA_PID,                  0x00,   0x0C,   0x01);
REGISTER_HID_DETECTOR_I  ("Razer Chroma Addressable RGB Controller",         DEVICE_TYPE_ACCESSORY, DetectRazerARGBControllers,    RAZER_VID,  RAZER_CHROMA_ADDRESSABLE_RGB_CONTROLLER_PID,    0x00                );
REGISTER_HID_DETECTOR_IPU("Razer Chroma HDK",                                DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CHROMA_HDK_PID,                           0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Chroma Mug Holder",                         DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CHROMA_MUG_PID,                           0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Chroma PC Case Lighting Kit",               DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CHROMA_PC_CASE_LIGHTING_KIT_PID,          0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Core",                                      DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CORE_PID,                                 0x00,   0xFF00, 0x01);
REGISTER_HID_DETECTOR_IPU("Razer Core X",                                    DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_CORE_X_PID,                               0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Laptop Stand Chroma",                       DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_LAPTOP_STAND_CHROMA_PID,                  0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Laptop Stand Chroma V2",                    DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_LAPTOP_STAND_CHROMA_V2_PID,               0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Leviathan V2",                              DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_LEVIATHAN_V2_PID,                         0x02,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Leviathan V2 X",                            DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_LEVIATHAN_V2X_PID,                        0x00,   0x0C,   0x01);
REGISTER_HID_DETECTOR_IPU("Razer Mouse Bungee V3 Chroma",                    DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_MOUSE_BUNGEE_V3_CHROMA_PID,               0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mouse Dock Chroma",                         DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_MOUSE_DOCK_CHROMA_PID,                    0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Razer Mouse Dock Pro",                            DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_MOUSE_DOCK_PRO_PID,                       0x00,   0x01,   0x02);
REGISTER_HID_DETECTOR_IPU("Lian Li O11 Dynamic - Razer Edition",             DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_O11_DYNAMIC_PID,                          0x02,   0x01,   0x02);
REGISTER_HID_DETECTOR_PU ("Razer Seiren Emote",                              DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_SEIREN_EMOTE_PID,                         0x0C,   0x01        );
REGISTER_HID_DETECTOR_PU("Razer Thunderbolt 4 Dock Chroma",                  DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_THUNDERBOLT_4_DOCK_CHROMA_PID,            0x0C,   0x01        );

/*-----------------------------------------------------------------------------------------------------*\
| Nommo devices seem to have an issue where interface 1 doesn't show on Linux or MacOS.  Due to the way |
| hidapi works on these operating systems, it is acceptable to use interface 0 instead.  Interface 1    |
| must be used on Windows.                                                                              |
\*-----------------------------------------------------------------------------------------------------*/
#ifdef _WIN32
REGISTER_HID_DETECTOR_IPU("Razer Nommo Chroma",                              DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_NOMMO_CHROMA_PID,                         0x01,   0x01,   0x03);
REGISTER_HID_DETECTOR_IPU("Razer Nommo Pro",                                 DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_NOMMO_PRO_PID,                            0x01,   0x01,   0x03);
#else
REGISTER_HID_DETECTOR_IPU("Razer Nommo Chroma",                              DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_NOMMO_CHROMA_PID,                         0x00,   0x01,   0x00);
REGISTER_HID_DETECTOR_IPU("Razer Nommo Pro",                                 DEVICE_TYPE_ACCESSORY, DetectRazerControllers,        RAZER_VID,  RAZER_NOMMO_PRO_PID,                            0x00,   0x01,   0x00);
#endif

/*-----------------------------------------------------------------------------------------------------*\
| Need to clean up some stuff before we scan/rescan                                                     |
\*-----------------------------------------------------------------------------------------------------*/
REGISTER_PRE_DETECTION_HOOK(ResetRazerARGBControllersPaths);
