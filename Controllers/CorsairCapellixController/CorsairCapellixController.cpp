/*---------------------------------------------------------*\
|  Processing Code for Corsair Capellix Series              |
|  Based on code by:                                        |
|  Adam Honse (calcprogrammer1@gmail.com), 8/17/2020        |
\*---------------------------------------------------------*/

#include "CorsairCapellixController.h"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std::chrono_literals;

CorsairCapellixController::CorsairCapellixController(hid_device* dev_handle, const char* path)
{
    dev = dev_handle;
    sendKeepalive = 1;
    keepalive_thread_run = 1;
    keepalive_thread = new std::thread(&CorsairCapellixController::KeepaliveThread, this);
}

CorsairCapellixController::~CorsairCapellixController()
{
    keepalive_thread_run = 0;
    keepalive_thread->join();
    delete keepalive_thread;
    hid_close(dev);
}

void CorsairCapellixController::KeepaliveThread()
{
    while(keepalive_thread_run){
        if(sendKeepalive) //only send direct mode packet if it's needed since it resets the color
        {
            if((std::chrono::steady_clock::now() - last_commit_time) > std::chrono::seconds(10))
            {
                SendCommit();
            }
            std::this_thread::sleep_for(1s);
        }
        else
        {
            std::this_thread::sleep_for(1s);
        }
    }
}

void CorsairCapellixController::StartKeepaliveThread()
{
    if(!sendKeepalive){
        sendKeepalive = 1;
        SendCommit();
    }
}

void CorsairCapellixController::PauseKeepaliveThread()
{
    sendKeepalive = 0;
}

void CorsairCapellixController::send_multi_pkt(unsigned char buffarray[][5], int r, int c){
    unsigned char* hidtemp = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    for(int i=0; i < r; i++){
        memset(hidtemp, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
        for(int j=0; j < c; j++){
            hidtemp[j+1] = buffarray[i][j];
        };
        hid_write(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE);
    };
};

void CorsairCapellixController::SetDirectColor(
        unsigned char red,
        unsigned char grn,
        unsigned char blu
        )
{
    unsigned char* usb_buf = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(usb_buf, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xA1;
    usb_buf[8] = 0x12;
    for(int i=10; i<=168; i=i+3){
        usb_buf[i]   = red;
        usb_buf[i+1] = grn;
        usb_buf[i+2] = blu;
    };
    last_commit_time = std::chrono::steady_clock::now();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);

}

void CorsairCapellixController::SendHWMode(
        unsigned int            mode,
        unsigned int            speed,
        unsigned int            direction,
        unsigned int            colormode,
        std::vector<RGBColor> & colors
        )
{
    PauseKeepaliveThread(); //Stop sending direct mode keepalive packets
    char dir;

    /*---------------------------*/
    //Set up hardware mode packet//
    /*---------------------------*/
    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));
    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;

    switch(mode){

    case CORSAIR_CAPELLIX_MODE_STATIC:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE;
        usb_buf[8]  = 0x7E; //Static
        usb_buf[9]  = 0x20; //mode
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_CONFIGURABLE;
        usb_buf[14] = 0x01; //Number of colors
        usb_buf[15] = 0xFF;
        for(std::size_t color_idx = 0; color_idx < colors.size(); color_idx++)
        {
            usb_buf[18] = RGBGetRValue(colors[color_idx]);
            usb_buf[17] = RGBGetGValue(colors[color_idx]);
            usb_buf[16] = RGBGetBValue(colors[color_idx]);
        }
        break;

    case CORSAIR_CAPELLIX_MODE_RAINBOWWAVE:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[8]  = 0x4C; //Rainbow wave
        usb_buf[9]  = 0xB9; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_SPIRALRAINBOW:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[8]  = 0x87; //Spiral rainbow
        usb_buf[9]  = 0xAB; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_RAINBOW:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[8]  = 0x02; //Rainbow
        usb_buf[9]  = 0xA4; //mode
        usb_buf[11] = speed;
        break;

    case CORSAIR_CAPELLIX_MODE_COLORSHIFT:
        usb_buf[8]  = 0xFA; //Color shift
        usb_buf[9]  = 0xA5; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_COLORPULSE:
        usb_buf[8]  = 0x4F; //Color pulse
        usb_buf[9]  = 0xAD; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_COLORWAVE:
        usb_buf[8]  = 0xFF; //Color wave
        usb_buf[9]  = 0x7B; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_SEQUENTTIAL:
        usb_buf[8]  = 0x17; //Sequential
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_STROBING:
        usb_buf[8]  = 0x1A; //Strobing
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_VISOR:
        usb_buf[8]  = 0xC0; //Visor
        usb_buf[9]  = 0x90; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_MARQUEE:
        usb_buf[8]  = 0x19; //Marquee
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_RAIN:
        usb_buf[8]  = 0x7E; //Rain
        usb_buf[9]  = 0xA0; //mode
        usb_buf[11] = speed;
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_COLORWARP:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[8]  = 0x18; //Color warp
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_ARC:
        usb_buf[8]  = 0x1B; //Arc
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf);
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    case CORSAIR_CAPELLIX_MODE_HEARTBEAT:
        usb_buf[8]  = 0x1C; //Heartbeat
        usb_buf[9]  = 0x00; //mode
        usb_buf[11] = speed;
        SetDirection(mode, direction, usb_buf); //outward heartbeat doesn't work for some reason
        SetColors(colormode, colors, CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL, usb_buf);
        break;

    }
    SetHWMode(); //Prepare to accept HW mode change
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE); //Send HW mode packet
    ExitDirectMode();
}

void CorsairCapellixController::SetColors(
        int colormode,
        std::vector<RGBColor> & colors,
        int numcolors,
        unsigned char *usb_buf
        )
{
    switch(colormode){
    case MODE_COLORS_RANDOM:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_RANDOM;
        break;
    case MODE_COLORS_MODE_SPECIFIC:
        usb_buf[4]  = numcolors;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_CONFIGURABLE;
        usb_buf[15] = 0xFF;
        switch(numcolors){
        case CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE:
            usb_buf[14] = 0x01; //number of colors
            break;
        case CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL:
            usb_buf[14] = 0x02; //number of colors
            break;
        }
        for(std::size_t color_idx = 0; color_idx < colors.size(); color_idx++)
        {
            usb_buf[(color_idx * 4) + 18] = RGBGetRValue(colors[color_idx]);
            usb_buf[(color_idx * 4) + 17] = RGBGetGValue(colors[color_idx]);
            usb_buf[(color_idx * 4) + 16] = RGBGetBValue(colors[color_idx]);
            if (color_idx != colors.size()-1)
                usb_buf[(color_idx * 4) + 19] = 0xFF;
        }
        break;
    }
}

void CorsairCapellixController::SetDirection(
        int mode,
        int direction,
        unsigned char *usb_buf
    )
{
    switch(mode){
    case CORSAIR_CAPELLIX_MODE_RAINBOWWAVE:
    case CORSAIR_CAPELLIX_MODE_COLORWAVE:
    case CORSAIR_CAPELLIX_MODE_SEQUENTTIAL:
    case CORSAIR_CAPELLIX_MODE_COLORWARP:
        switch(direction){
        case MODE_DIRECTION_LEFT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_LEFT;
            break;
        case MODE_DIRECTION_RIGHT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_RIGHT;
            break;
        }
        break;

    case CORSAIR_CAPELLIX_MODE_SPIRALRAINBOW:
    case CORSAIR_CAPELLIX_MODE_ARC:
        switch(direction){
        case MODE_DIRECTION_LEFT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_CCW;
            break;
        case MODE_DIRECTION_RIGHT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_CW;
            break;
        }
        break;

    case CORSAIR_CAPELLIX_MODE_HEARTBEAT:
        switch(direction){
        case MODE_DIRECTION_LEFT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_INWARD;
            break;
        case MODE_DIRECTION_RIGHT:
            usb_buf[12] = CORSAIR_CAPELLIX_DIRECTION_OUTWARD;
            break;
        }
        break;
    }
}

void CorsairCapellixController::SetDirectMode(){

    std::cout<<"setdirectmode"<<std::endl;

    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x00, 0x22, 0x00}, //put into direct mode
    };
    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

}

void CorsairCapellixController::SetHWMode()
{
    unsigned char buffarray[][5] = {
        //set hardware mode pre packets
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x01, 0x64, 0x6d}, //required to
        {0x08, 0x09, 0x01, 0x00, 0x00}, //accept new hardware mode setting
    };
    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}

void CorsairCapellixController::ExitDirectMode()
{
    unsigned char buffarray[][5] = {
        //{0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x01}
    };
    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}

void CorsairCapellixController::SendCommit()
{
    unsigned char   usb_buf[1025];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Update last commit time                               |
    \*-----------------------------------------------------*/
    last_commit_time = std::chrono::steady_clock::now();

    /*-----------------------------------------------------*\
    | Set up Commit packet                                  |
    \*-----------------------------------------------------*/
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x00, 0x22, 0x00}, //put into direct mode
    };

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}
