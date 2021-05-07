/*---------------------------------------------------------*\
|  Processing Code for Corsair Capellix Series              |
|                                                           |
|  Jeff P.                                                  |
\*---------------------------------------------------------*/

#include "CorsairCommanderCoreController.h"

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std::chrono_literals;

CorsairCommanderCoreController::CorsairCommanderCoreController(hid_device* dev_handle, const char* path)
{
    dev = dev_handle;
    keepalive_thread_run = 1;
    send_keepalive = 0;
    keepalive_thread = new std::thread(&CorsairCommanderCoreController::KeepaliveThread, this);
}

CorsairCommanderCoreController::~CorsairCommanderCoreController()
{
    keepalive_thread_run = 0;
    keepalive_thread->join();
    delete keepalive_thread;
    hid_close(dev);
}

/*----------------------------*/
//Detects rgb connected to hub//
/*----------------------------*/
std::vector<int> CorsairCommanderCoreController::DetectRGBFans()
{
    send_keepalive=0;
    fanleds = {};
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x01, 0x20, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        //{0x08, 0x08, 0x01, 0x00, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    unsigned char* hidtemp = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    unsigned char* res = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    memset(res, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hidtemp[0] = 0x00;
    hidtemp[1] = 0x08;
    hidtemp[2] = 0x08;
    hidtemp[3] = 0x01;
    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE); //send rgb fan detect request
    hid_read(dev, res, CORSAIR_COMMANDER_CORE_PACKET_SIZE); //read response
    for(int i=8; i<=32; i=i+4){
        if(res[i-2] == 0x02)
            fanleds.push_back(res[i]);
        std::cout<<(int) res[i-2]<<":"<<(int) res[i]<<" "<<std::endl;
    }
    if(fanleds.size() == 0){
        fanleds.push_back(0);
    }
    SetFanMode();
    return fanleds;
}

void CorsairCommanderCoreController::KeepaliveThread()
{
    while(keepalive_thread_run.load())
    {
        if (send_keepalive){
            if((std::chrono::steady_clock::now() - last_commit_time) > std::chrono::seconds(10))
            {
                SendCommit();
            }
        }
        std::this_thread::sleep_for(1s);
    }
}

/*-----------------------------------------*/
//Private function to send multiple packets//
/*-----------------------------------------*/
void CorsairCommanderCoreController::SendMultiPkt(unsigned char buffarray[][5], int r, int c){
    unsigned char* hidtemp = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    for(int i=0; i < r; i++){
        memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
        for(int j=0; j < c; j++){
            hidtemp[j+1] = buffarray[i][j];
        };
        hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
        hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    };
}

void CorsairCommanderCoreController::SetDirectColor(
        std::vector<RGBColor> colors,
        std::vector<zone> zones
        )
{
    int packet_offset = CORSAIR_COMMANDER_CORE_PREAMBLE_OFFSET;
    int led_idx = 0;
    unsigned char* usb_buf = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;

    for(int zone_idx=0; zone_idx<zones.size(); zone_idx++){
        std::cout<<"Zone: "<<zone_idx<<std::endl;
        std::cout<<"LED Count: "<< led_idx + zones[zone_idx].leds_count<<std::endl;
        for(int i = led_idx; i < led_idx + zones[zone_idx].leds_count; i++){
            /*--------------*/
            //Set led colors//
            /*--------------*/
            usb_buf[packet_offset]   = RGBGetRValue(colors[i]);
            usb_buf[packet_offset+1] = RGBGetGValue(colors[i]);
            usb_buf[packet_offset+2] = RGBGetBValue(colors[i]);
            packet_offset = packet_offset + 3;
        }
        led_idx = led_idx + zones[zone_idx].leds_count;
        if(zones[zone_idx].leds_count != 29){
            packet_offset = packet_offset + 3 * (34 - zones[zone_idx].leds_count); //Move offset for fans with less than 34 LEDs
        }
    }
    last_commit_time = std::chrono::steady_clock::now(); //sending a direct mode color packet resets the timeout
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    //hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
}

void CorsairCommanderCoreController::SendCommit()
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

    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
}


/*------------------------------------------------------*\
|Set to QL Fan mode as workaround to allow mixed rgb fans|
\*------------------------------------------------------*/
void CorsairCommanderCoreController::SetFanMode()
{
    SendCommit();
    unsigned char buffarray[][5] = {
        {0x08, 0x0D, 0x01, 0x1E, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00}
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    unsigned char usb_buf[1025];
    memset(usb_buf, 0, sizeof(usb_buf));
    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = 0x0F;
    usb_buf[8]  = 0x0D;
    usb_buf[10]  = 0x07;
    usb_buf[11]  = 0x01;
    usb_buf[12]  = 0x08;
    for(int i=13; i<25; i=i+2){
        usb_buf[i]  = 0x01;
        usb_buf[i+1]= 0x06;
    }
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    unsigned char buffarray2[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x15, 0x01, 0x00, 0x00}
    };
    SendMultiPkt(buffarray2, sizeof(buffarray2)/sizeof(buffarray2[0]), sizeof(buffarray2)[0]/sizeof(buffarray2[0][0]));

    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    memset(usb_buf, 0, sizeof(usb_buf));
    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x01;
    usb_buf[4]  = 0x0F;
    usb_buf[8]  = 0x0D;
    usb_buf[10]  = 0x07;
    usb_buf[11]  = 0x01;
    usb_buf[12]  = 0x08;
    for(int i=13; i<25; i=i+2){
        usb_buf[i]  = 0x01;
        usb_buf[i+1]= 0x06;
    }
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    SendMultiPkt(buffarray2, sizeof(buffarray2)/sizeof(buffarray2[0]), sizeof(buffarray2)[0]/sizeof(buffarray2[0][0]));

    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    send_keepalive = 1;
}
