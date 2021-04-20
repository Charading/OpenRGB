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
    dev         = dev_handle;
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
        if(sendKeepalive)
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

void CorsairCapellixController::send_multi_pkt(unsigned char buffarray[][10], int r, int c){
    unsigned char* hidtemp = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    for(int i=0; i < r; i++){

        memset(hidtemp, 0, CORSAIR_CAPELLIX_PACKET_SIZE);

        for(int j=0; j < c; j++){
            hidtemp[j+1] = buffarray[i][j];
        };

        hid_write(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

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

void CorsairCapellixController::SetStatic(
        std::vector<RGBColor> & colors
        )
{

    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));
    for(std::size_t color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        usb_buf[18] = RGBGetRValue(colors[color_idx]);
        usb_buf[17] = RGBGetGValue(colors[color_idx]);
        usb_buf[16] = RGBGetBValue(colors[color_idx]);
    }

    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_SINGLE;

    //mode
    usb_buf[8]  = 0x7E;
    usb_buf[9]  = 0x20;

    usb_buf[10] = 0x10;
    usb_buf[14] = 0x01;
    usb_buf[15] = 0xFF;

    SetHWMode();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    EnterHWMode();
}

void CorsairCapellixController::SetRainbowWave(
        unsigned char           speed,
        unsigned char           direction
        )
{
    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));

    char dir;
    switch(direction){
    case MODE_DIRECTION_LEFT:
        dir = CORSAIR_CAPELLIX_DIRECTION_LEFT;
        break;
    case MODE_DIRECTION_RIGHT:
        dir = CORSAIR_CAPELLIX_DIRECTION_RIGHT;
        break;
    }

    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;

    //mode
    usb_buf[8]  = 0x4C;
    usb_buf[9]  = 0xB9;

    usb_buf[11] = speed;
    usb_buf[12] = dir;

    SetHWMode();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    EnterHWMode();
}

void CorsairCapellixController::SetSpiralRainbow(
        unsigned char           speed,
        unsigned char           direction
        )
{
    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));

    char dir;
    switch(direction){
    case MODE_DIRECTION_LEFT:
        std::cout<<"left"<<std::endl;
        dir = CORSAIR_CAPELLIX_DIRECTION_CCW;
        break;
    case MODE_DIRECTION_RIGHT:
        std::cout<<"right"<<std::endl;
        dir = CORSAIR_CAPELLIX_DIRECTION_CW;
        break;
    }

    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;

    //mode
    usb_buf[8]  = 0x87;
    usb_buf[9]  = 0xAB;

    usb_buf[11] = speed;
    usb_buf[12] = dir;

    SetHWMode();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    EnterHWMode();
}

void CorsairCapellixController::SetRainbow(
        unsigned char           speed
        )
{
    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));

    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;

    //mode
    usb_buf[8]  = 0x02;
    usb_buf[9]  = 0xA4;

    usb_buf[11] = speed;

    SetHWMode();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    EnterHWMode();
}

void CorsairCapellixController::SetColorShift(
        unsigned char           speed,
        unsigned char           colormode,
        std::vector<RGBColor> & colors
        )
{
    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));

    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;

    //mode
    usb_buf[8]  = 0xFA;
    usb_buf[9]  = 0xA5;

    usb_buf[11] = speed;
    usb_buf[15] = 0xFF;

    switch(colormode){
    case MODE_COLORS_RANDOM:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_NONE;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_RANDOM;
        usb_buf[14] = 0x00;
        break;
    case MODE_COLORS_MODE_SPECIFIC:
        usb_buf[4]  = CORSAIR_CAPELLIX_COLOR_CONFIG_DUAL;
        usb_buf[10] = CORSAIR_CAPELLIX_COLOR_MODE_CONFIGURABLE;
        usb_buf[14] = 0x02; //number of colors
        break;
    }

    for(std::size_t color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        usb_buf[(color_idx * 4) + 18] = RGBGetRValue(colors[color_idx]);
        usb_buf[(color_idx * 4) + 17] = RGBGetGValue(colors[color_idx]);
        usb_buf[(color_idx * 4) + 16] = RGBGetBValue(colors[color_idx]);
        usb_buf[(color_idx * 4) + 19] = 0xFF;
    }

    SetHWMode();
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    EnterHWMode();
}

void CorsairCapellixController::SetDirectMode(){

    std::cout<<"setdirectmode"<<std::endl;

    unsigned char buffarray[][10] = {
        {0x08, 0x01, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, //required to
        {0x08, 0x0d, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //put into software mode
    };

    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

}

void CorsairCapellixController::SetHWMode()
{
    unsigned char buffarray[][10] = {
        //set hardware mode pre packets
        {0x08, 0x01, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x08, 0x0d, 0x01, 0x64, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    };

    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}

void CorsairCapellixController::EnterHWMode()
{
    unsigned char buffarray[][10] = {
        //{0x08, 0x05, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}
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

    unsigned char buffarray[][10] = {
        {0x08, 0x01, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, //required to
        {0x08, 0x0d, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //put into software mode
    };

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_multi_pkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
}
