/*------------------------------------------*\
|  HyperXAlloyOriginsCoreController.cpp      |
|                                            |
|  Driver for HyperX Alloy Origins Core      |
|  RGB Keyboard lighting controller          |
|                                            |
|  Volodymyr Nazarchuk (Vavooon) 4/28/2021   |
|  mike white (kamaaina)          6/9/2021   |
\*------------------------------------------*/

#include "HyperXAlloyOriginsCoreController.h"

#include <cstring>

// TODO: move vid and pid to a header file so it can be reused
//       these are defined in HyperXKeyboardControllerDetect.cpp and copied here
#define HYPERX_KEYBOARD_VID             0x0951
#define HYPERX_ALLOY_ORIGINS_CORE_PID   0x16E6

// Skip these indices in the color output
static unsigned int skip_idx[] = {6,  7, 14, 15, 22, 23, 30, 31, 38, 39, 44, 46, 47, 54, 55, 58, 60, 61, 62, 63, 70, 71,	78, 79, 86, 87, 94, 95, 101, 102, 103, 109, 110, 111, 118, 119};

HyperXAlloyOriginsCoreController::HyperXAlloyOriginsCoreController(hid_device* dev_handle, const char* path)
{
    dev = dev_handle;
    location = path;
    firmwareVersion = "";
}

HyperXAlloyOriginsCoreController::~HyperXAlloyOriginsCoreController()
{
    hid_close(dev);
}

std::string HyperXAlloyOriginsCoreController::GetDeviceLocation()
{
    return("HID " + location);
}

std::string HyperXAlloyOriginsCoreController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

std::string HyperXAlloyOriginsCoreController::GetFirmwareVersion()
{
   char buf[8];
   memset(buf, '\0', sizeof(buf));
   hid_device_info* device = hid_enumerate(HYPERX_KEYBOARD_VID, HYPERX_ALLOY_ORIGINS_CORE_PID);
   while(device)
   {
      unsigned short version = device->release_number;
      sprintf(buf, "%.2X.%.2X", (version & 0xFF00) >> 8, version & 0x00FF);
      firmwareVersion = buf;
      break;
   }
   hid_free_enumeration(device);

   return firmwareVersion;
}

void HyperXAlloyOriginsCoreController::SetLEDsDirect(std::vector<RGBColor> colors)
{
    for(unsigned int skip_cnt = 0; skip_cnt < (sizeof(skip_idx) / sizeof(skip_idx[0])); skip_cnt++)
    {
        colors.insert(colors.begin() + skip_idx[skip_cnt], 0x00000000);
    }

    unsigned char buf[380];
    memset(buf, 0x00, sizeof(buf));

    int offset = 0;
    int rowPos = 0;

    for(unsigned int color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        if (color_idx > 0 && color_idx % 16 == 0)
        {
            offset += 48;
            rowPos = 0;
        }
        buf[rowPos + offset] = RGBGetGValue(colors[color_idx]);
        buf[rowPos + offset + 16] = RGBGetRValue(colors[color_idx]);
        buf[rowPos + offset + 32] = RGBGetBValue(colors[color_idx]);
        rowPos++;
    }

    unsigned int sentBytes = 0;
    unsigned int bytesToSend = sizeof(buf);
    unsigned int payloadSize = 60;
    unsigned int seq = 0;

    while (sentBytes < bytesToSend) {
        if (bytesToSend - sentBytes < payloadSize)
        {
            payloadSize = bytesToSend - sentBytes;
        }

        unsigned char packet[65];
        memset(packet, 0x00, sizeof(packet));

#ifdef _WIN32
        // on windows, hid_write() omits the first byte which should be the report ID
        // linux packet size is 65 bytes, but windows is 64
        // instead of messing with the win32 libusb library, just start the packet
        // data at byte[1] instead of byte[0]. not elegant, but it works...
        packet[1] = 0xA2;
        packet[2] = seq++;
        packet[4] = payloadSize;

        memcpy(&packet[5], &buf[sentBytes], payloadSize);
#else
        packet[0] = 0xA2;
        packet[1] = seq++;
        packet[3] = payloadSize;

        memcpy(&packet[4], &buf[sentBytes], payloadSize);
#endif

        hid_write(dev, packet, 65);

        sentBytes += payloadSize;
    }
}
