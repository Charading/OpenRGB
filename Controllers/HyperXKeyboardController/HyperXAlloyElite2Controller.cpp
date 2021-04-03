/*-----------------------------------------*\
|  HyperXAlloyElite2Controller.cpp          |
|                                           |
|  Driver for HyperX Alloy Elite2 RGB       |
|  Keyboard lighting controller             |
|                                           |
|  KundaPanda (vojdo) 02/04/2021            |
\*-----------------------------------------*/

#include "HyperXAlloyElite2Controller.h"

#include <cstring>

// Skip these indices in the color output
static const unsigned int SKIP_INDICES[] = { 6, 23, 29, 41, 47, 70, 71, 75, 76, 77, 87, 88, 93, 99, 100, 102, 108, 113 };

HyperXAlloyElite2Controller::HyperXAlloyElite2Controller(hid_device* dev_handle, const char* path)
  : dev(dev_handle)
  , location(path) {}

HyperXAlloyElite2Controller::~HyperXAlloyElite2Controller()
{
    hid_close(dev);
}

std::string HyperXAlloyElite2Controller::GetDeviceLocation()
{
    return("HID " + location);
}

std::string HyperXAlloyElite2Controller::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void HyperXAlloyElite2Controller::SetLEDsDirect(const std::vector<RGBColor>& colors)
{

    /*
     * Variables to keep track of color sending and skipping
     * */
    size_t packets_sent = 0;
    size_t buf_idx = 1;
    size_t skipped = 0;
    unsigned char buf[65] = { 0x00 };
    const unsigned int* skip_idx = &SKIP_INDICES[0];
    size_t color_idx = 0;

    /*
     * Initialize direct control
     * */
    SendDirectInitialization();

    while (color_idx < colors.size())
    {
        buf[buf_idx] = 0x81;
        if (*skip_idx == color_idx + skipped)
        {
            /* Add null data for skip index */
            memset(&buf[buf_idx + 1], 0x00, 3);
            ++skip_idx;
            if (skip_idx >= SKIP_INDICES + sizeof(SKIP_INDICES) / sizeof(unsigned int))
            {
                skip_idx = SKIP_INDICES;
            }
            ++skipped;
        }
        else
        {
            /* copy color data to buffer */
            memcpy(&buf[buf_idx + 1], (unsigned char *) &colors[color_idx], 3);
            ++color_idx;
        }
        buf_idx += 4;
        if (buf_idx == sizeof(buf))
        {
            /* Send packet if buffer is full and reset buffer indexing */
            hid_send_feature_report(dev, buf, sizeof(buf));
            buf_idx = 1;
            ++packets_sent;
        }
        else if (color_idx == colors.size())
        {
            /* Set empty frame as the rest of the packet and send */
            std::fill((unsigned int *)(buf + buf_idx), (unsigned int *)(buf + 65), 0x00);
            hid_send_feature_report(dev, buf, sizeof(buf));
            ++packets_sent;
        }
    }
    /* Send any remaining packets as empty, though there should be none remaining */
    std::fill((unsigned int *)(buf + 1), (unsigned int *)(buf + 65), 0x00);
    for (size_t remaining_packet = 0; packets_sent + remaining_packet < 9; ++remaining_packet)
    {
        hid_send_feature_report(dev, buf, sizeof(buf));
    }
}

const unsigned char HyperXAlloyElite2Controller::initialization_buffer[65] = {0x00, 0x04, 0xF2};

void HyperXAlloyElite2Controller::SendDirectInitialization()
{
    hid_send_feature_report(dev, HyperXAlloyElite2Controller::initialization_buffer, sizeof(HyperXAlloyElite2Controller::initialization_buffer) / sizeof(unsigned char));
}
