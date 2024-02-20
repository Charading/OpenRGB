/*-----------------------------------------*\
|  TesoroGramXSController.cpp               |
|                                           |
|  RGB Interface for Tesoro Gram XS         |
|  Keyboard                                 |
|                                           |
|  Ondřej Niesner 8/31/2023                 |
\*-----------------------------------------*/

#include "TesoroGramXSController.h"
#include <cstring>

using namespace std::chrono_literals;


/*--------------------------------*\
| Offset map for each key          |
\*_-------------------------------*/
static const uint8_t packet_map[GRAM_XS_KEY_COUNT] =
{
/*     ESC    F1  F2    F3   F4   F5   F6    F7    F8   F9  F10  F11  F12  Prt  Scr  Brk                         */    
        0,    16, 32,   48,  64,  80,  96,  112,  128,  1,  17,  33,   8,  24,  40,  56,
/*       `     1   2     3    4    5    6    7     8    9    0    -    =   BSP  Ins  Hom PgU  NmL   /    *   -   */
        49,   65, 81,   97, 113, 129,   2,   18,   34, 50,  66,  82,  41,  89,  72,  88, 104, 106, 122, 138, 11,
/*      TAB    Q    W    E    R    T    Y     U     I   O    P    [    ]    \  Del  End  PgD    7    8    9   +  */
        98,  114, 130,   3,  19,  35,  51,   67,   83, 99, 115, 131, 120, 136, 105, 121, 137,  59,  75,  91, 27,
/*      CAP    A   S     D    F    G    H     J     K   L    ;    '   Ent   4    5    6                          */
        4,    20, 36,   52,  68,  84, 100,  116,  132,  5,  21,  37,  25, 107, 123, 139,
/*      LSh    Z    X    C    V    B    N     M     ,   .    /   RSh  Up    1    2    3  Ent                     */
        53,   85, 101, 117, 133,   6,  22,   38,   54, 70,  86,  10,  42,  12,  28,  44, 43,
/*      LCt  Win  LAl   Spc  RAl  Fn  Menu   RCt   Lft Dwn  Rgh   0    .                                         */
        102, 118, 134,  23,  71,  87, 103,   26,   58, 74,  90,  92,  76
};

TesoroGramXSController::TesoroGramXSController(hid_device *dev_handle, const char *path)
{
    dev = dev_handle;
    location = path;
}

TesoroGramXSController::~TesoroGramXSController()
{
    hid_close(dev);
}

std::string TesoroGramXSController::GetLocation()
{
    return("HID: " + location);
}

std::string TesoroGramXSController::GetSerialString()
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

void TesoroGramXSController::SetMode(unsigned char mode, unsigned char brightness, unsigned char color_mode, std::vector<RGBColor> colors)
{
    unsigned char usb_buf[GRAM_XS_PACKET_SIZE];

    memset(usb_buf, 0x00, sizeof(usb_buf));

    usb_buf[0]      = 0x07;
    usb_buf[1]      = 0x02;                   
    usb_buf[2]      = GRAM_XS_DEFAULT_PROFILE;
    usb_buf[8]      = 0x01;                   
    usb_buf[12]     = mode;                   
    usb_buf[13]     = brightness;

    if(color_mode == MODE_COLORS_RANDOM)
    {
        /*------------------------------------------------------------------------*\                             
        | Random color mode for each of 5 modes can be set at once for all modes   |
        | because only one mode is active. This is what also the Teosoro app does  |
        \*------------------------------------------------------------------------*/
        usb_buf[19] = 0x02;
        usb_buf[20] = 0x02;
        usb_buf[21] = 0x02;
        usb_buf[22] = 0x02;
        usb_buf[23] = 0x02;
    }

    if(color_mode == MODE_COLORS_MODE_SPECIFIC)
    {
        /*-------------------------------------------------------------------------*\
        | Send color data to all 5 modes. Can be set individually but only one      |
        | mode is active so it's unnecessary. This is also what the Tesoro app does |
        \*-------------------------------------------------------------------------*/
        for(unsigned int count = 0; count < 5; count++)
        {
            usb_buf[count * 6 + 33] = RGBGetRValue(colors[0]);
            usb_buf[count * 6 + 35] = RGBGetGValue(colors[0]);
            usb_buf[count * 6 + 37] = RGBGetBValue(colors[0]);
            if(colors.size() > 1)
            {
                usb_buf[count * 6 + 34] = RGBGetRValue(colors[1]);
                usb_buf[count * 6 + 36] = RGBGetGValue(colors[1]);
                usb_buf[count * 6 + 38] = RGBGetBValue(colors[1]);
            }
        }
    }

    hid_send_feature_report(dev, usb_buf, sizeof(usb_buf));
    std::this_thread::sleep_for(100ms);
}

void TesoroGramXSController::UpdateLeds(std::vector<RGBColor> colors, bool save_to_flash)
{
    unsigned char usb_buf_RG[GRAM_XS_PACKET_SIZE];
    unsigned char usb_buf_GB[GRAM_XS_PACKET_SIZE];

    memset(usb_buf_RG, 0x00, sizeof(usb_buf_RG));
    memset(usb_buf_GB, 0x00, sizeof(usb_buf_GB));

    usb_buf_RG[0] = 0x07;
    usb_buf_RG[1] = 0x06;
    usb_buf_RG[2] = GRAM_XS_DEFAULT_PROFILE;
    usb_buf_RG[3] = 0x01;

    usb_buf_GB[0] = 0x07;
    usb_buf_GB[1] = 0x06;
    usb_buf_GB[2] = GRAM_XS_DEFAULT_PROFILE;
    usb_buf_GB[3] = 0x02;

    if (save_to_flash)
    {
        usb_buf_RG[4] = 0x01;
        usb_buf_GB[4] = 0x01;
    }

    /*----------------------------------------------------------*\
    | Color data for each individual key is send in two packets. |
    | The green color is overlapping to the second packet.       |
    \*----------------------------------------------------------*/
    for(uint8_t i = 0; i < colors.size(); i++)
    {
        uint8_t offset = packet_map[i];

        usb_buf_RG[GRAM_XS_PACKET_R + offset] = RGBGetRValue(colors[i]);

        if((GRAM_XS_PACKET_G_1 + offset) < GRAM_XS_PACKET_SIZE)
        {
            usb_buf_RG[GRAM_XS_PACKET_G_1 + offset] = RGBGetGValue(colors[i]);
        }
        else
        {
            usb_buf_GB[(GRAM_XS_PACKET_G_2 + offset) - (GRAM_XS_PACKET_SIZE - GRAM_XS_PACKET_HEADER_SIZE - GRAM_XS_PACKET_COLOR_SIZE)] = RGBGetGValue(colors[i]);
        }

        usb_buf_GB[GRAM_XS_PACKET_B + offset] = RGBGetBValue(colors[i]);
    }

    hid_send_feature_report(dev, usb_buf_RG, sizeof(usb_buf_RG));
    std::this_thread::sleep_for(10ms);
    hid_send_feature_report(dev, usb_buf_GB, sizeof(usb_buf_GB));
    std::this_thread::sleep_for(10ms);
}
 