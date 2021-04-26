/*---------------------------------------------------------*\
|  Processing Code for Corsair Capellix Series              |
|                                                           |
|  Jeff P.                                                  |
\*---------------------------------------------------------*/

#include "CorsairCapellixController.h"

#include <cstring>
#include <iomanip>
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

/*----------------------------*/
//Detects rgb connected to hub//
/*----------------------------*/
std::vector<int> CorsairCapellixController::DetectFans()
{
    std::vector<int> fanleds;
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x01, 0x20, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        //{0x08, 0x08, 0x01, 0x00, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    unsigned char* hidtemp = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(hidtemp, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    hidtemp[0] = 0x00;
    hidtemp[1] = 0x08;
    hidtemp[2] = 0x08;
    hidtemp[3] = 0x01;
    hid_write(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE); //send rgb fan detect request
    hid_read(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE); //read response
    for(int i=12; i<=32; i=i+4){
        if(hidtemp[i-2] == 0x02)
            fanleds.push_back(hidtemp[i]);
    }
    return fanleds;
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

void CorsairCapellixController::RestartKeepaliveThread()
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

/*-----------------------------------------*/
//Private function to send multiple packets//
/*-----------------------------------------*/
void CorsairCapellixController::SendMultiPkt(unsigned char buffarray[][5], int r, int c){
    unsigned char* hidtemp = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    for(int i=0; i < r; i++){
        memset(hidtemp, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
        for(int j=0; j < c; j++){
            hidtemp[j+1] = buffarray[i][j];
        };
        hid_write(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE);
        hid_read(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE);
    };
}

void CorsairCapellixController::SetDirectColor(
        std::vector<RGBColor> colors
        )
{
    unsigned char* usb_buf = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(usb_buf, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xA1;
    usb_buf[8] = 0x12;

    for(int i=0; i<=colors.size(); i++){
        usb_buf[10+3*i] = RGBGetRValue(colors[i]);
        usb_buf[11+3*i] = RGBGetGValue(colors[i]);
        usb_buf[12+3*i] = RGBGetBValue(colors[i]);
    };

    last_commit_time = std::chrono::steady_clock::now(); //sending a direct mode color packet resets the timeout
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    //hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
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
    hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE); //Read response
    ExitDirectMode(); //Exit direct mode, allow hardware mode to play
}

/*-------------------------------------------------------*/
//Function to build color portion of hardware mode packet//
/*-------------------------------------------------------*/
void CorsairCapellixController::SetColors(
        int colormode,
        std::vector<RGBColor> & colors,
        int colorconfig,
        unsigned char *usb_buf
        )
{
    switch(colormode){
    case MODE_COLORS_RANDOM:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_RANDOM;
        break;
    case MODE_COLORS_MODE_SPECIFIC:
        usb_buf[4]  = colorconfig;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_CONFIGURABLE;
        usb_buf[15] = 0xFF;
        switch(colorconfig){
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
                usb_buf[(color_idx * 4) + 19] = 0xFF; //separator byte for hardware mode packets
        }
        break;
    }
}

/*------------------------------------------------------*/
//Function to set direction byte of hardware mode packet//
/*------------------------------------------------------*/
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

/*--------------------*/
//put into direct mode//
/*--------------------*/
void CorsairCapellixController::SetDirectMode()
{
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x0d, 0x00, 0x22, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}

/*-----------------------------*/
//send hardware mode pre packets//
/*-----------------------------*/
void CorsairCapellixController::SetHWMode()
{
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        //{0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x0d, 0x01, 0x64, 0x6d},
        {0x08, 0x09, 0x01, 0x00, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}

void CorsairCapellixController::ExitDirectMode()
{
    unsigned char buffarray[][5] = {
        //{0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x01},
        {0x08, 0x0d, 0x01, 0x17, 0x00}
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
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
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
    SetDirectColor(std::vector<RGBColor> {ToRGBColor(0,0,0)}); //softlocks if color isn't set at least once
}
