/*---------------------------------------------------------*\
|  Processing Code for Corsair Commander Core               |
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
    InitController();
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

void CorsairCommanderCoreController::InitController()
{
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x00, 0x22, 0x00}, //put into direct mode
    };

    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));
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


    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
}

/*-----------------------------------------*\
| Private function to send multiple packets |
\*-----------------------------------------*/
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

void CorsairCommanderCoreController::SendMultiPktLg(unsigned char buffarray[][15], int r, int c){
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
    int channel_idx = 0;
    unsigned char* usb_buf = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    memset(usb_buf, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    /*--------------------------------*\
    | Prepare color information packet |
    \*--------------------------------*/
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;

    std::cout<<"Zone setup done: "<<zones.size()<<std::endl;

    for(int zone_idx = 0; zone_idx < zones.size(); zone_idx++){
        //std::cout<<"channel: "<<channel_idx<<": "<<fanleds[channel_idx]<<std::endl;

        /*--------------*\
        | Add led colors |
        \*--------------*/
        for(int i = led_idx; i < led_idx + zones[zone_idx].leds_count; i++){
            usb_buf[packet_offset]   = RGBGetRValue(colors[i]);
            usb_buf[packet_offset+1] = RGBGetGValue(colors[i]);
            usb_buf[packet_offset+2] = RGBGetBValue(colors[i]);
            packet_offset = packet_offset + 3;
        }
        led_idx = led_idx + zones[zone_idx].leds_count;

        /*-------------------------------------------*\
        | Move offset for fans with less than 34 LEDs |
        \*-------------------------------------------*/
        if(zone_idx != 0){
            packet_offset = packet_offset + 3 * (34 - zones[zone_idx].leds_count);
        }

        channel_idx++;
    }

    /*-----------------------------------------------------*\
    | sending a direct mode color packet resets the timeout |
    \*-----------------------------------------------------*/
    last_commit_time = std::chrono::steady_clock::now();

    for(int i=15; i<900; i=i+3){
        usb_buf[i]   = 0xFF;
        usb_buf[i+1] = 0;
        usb_buf[i+2] = 0;
    }

    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    //hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
}




std::vector<int> CorsairCommanderCoreController::DetectRGBFans()
{
    unsigned char* hidtemp = new unsigned char[CORSAIR_COMMANDER_CORE_PACKET_SIZE];
    send_keepalive = 0;
    fanleds={};

    unsigned char buff[][5] = {

        {0x08, 0x02, 0x03, 0x00, 0x00},
        {0x08, 0x02, 0x13, 0x00, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x02, 0x03, 0x00, 0x00},
        {0x08, 0x02, 0x5f, 0x00, 0x00},
        {0x08, 0x02, 0x03, 0x00, 0x00},
        {0x08, 0x02, 0x13, 0x00, 0x00},
        {0x08, 0x02, 0x14, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x27, 0x00},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x20, 0x00},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x18, 0x00},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},
        {0x08, 0x02, 0x65, 0x00, 0x00},
        {0x08, 0x02, 0x55, 0x00, 0x00},
        {0x08, 0x02, 0x56, 0x00, 0x00},
        {0x08, 0x02, 0x57, 0x00, 0x00},
        {0x08, 0x02, 0x58, 0x00, 0x00},
        {0x08, 0x02, 0x59, 0x00, 0x00},
        {0x08, 0x02, 0x5A, 0x00, 0x00},
        {0x08, 0x02, 0x5B, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x07, 0x00},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x65, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x65, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x64, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x63, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x60, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x61, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x62, 0x6D},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x00, 0x1A, 0x00},
        {0x08, 0x09, 0x00, 0x00, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x00, 0x00},
        {0x08, 0x02, 0x65, 0x00, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x02},

        {0x08, 0x0d, 0x00, 0x22, 0x00}
    };

        SendMultiPkt(buff, sizeof(buff)/sizeof(buff[0]), sizeof(buff)[0]/sizeof(buff[0][0]));

        unsigned char buff2[][15] = {

        {0x08, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
        };

        SendMultiPktLg(buff2, sizeof(buff2)/sizeof(buff2[0]), sizeof(buff2)[0]/sizeof(buff2[0][0]));

        unsigned char buff3[][5] = {
        {0x08, 0x0d, 0x01, 0x1e, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        };

        SendMultiPkt(buff3, sizeof(buff3)/sizeof(buff3[0]), sizeof(buff3)[0]/sizeof(buff3[0][0]));

        unsigned char buff4[][15] = {

        {0x08, 0x06, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}
        };

        SendMultiPktLg(buff4, sizeof(buff4)/sizeof(buff4[0]), sizeof(buff4)[0]/sizeof(buff4[0][0]));

                unsigned char buff5[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x15, 0x01, 0x00, 0x00},

        {0x08, 0x0d, 0x01, 0x21, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        {0x08, 0x08, 0x01, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x01, 0x00},

        {0x08, 0x0d, 0x01, 0x17, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        {0x08, 0x08, 0x01, 0x00, 0x00},
        {0x08, 0x05, 0x01, 0x01, 0x00},

        {0x08, 0x0d, 0x01, 0x18, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
                };
                SendMultiPkt(buff5, sizeof(buff5)/sizeof(buff5[0]), sizeof(buff5)[0]/sizeof(buff5[0][0]));

                memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hidtemp[1] = 0x08;
                hidtemp[2] = 0x06;
                hidtemp[3] = 0x01;
                hidtemp[4] = 0x1F;
                hidtemp[8] = 0x07;
                hidtemp[10] = 0x07;
                hidtemp[15] = 0x01;
                hidtemp[19] = 0x02;
                hidtemp[23] = 0x03;
                hidtemp[27] = 0x04;
                hidtemp[29] = 0x19;
                hidtemp[31] = 0x05;
                hidtemp[35] = 0x06;
                hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);



                unsigned char buff6[][5] = {

        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x0d, 0x01, 0x18, 0x00},
                    {0x08, 0x09, 0x01, 0x00, 0x00},
                };
                SendMultiPkt(buff6, sizeof(buff6)/sizeof(buff6[0]), sizeof(buff6)[0]/sizeof(buff6[0][0]));


                memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hidtemp[1] = 0x08;
                hidtemp[2] = 0x06;
                hidtemp[3] = 0x01;
                hidtemp[4] = 0x1F;
                hidtemp[8] = 0x07;
                hidtemp[10] = 0x07;
                hidtemp[15] = 0x01;
                hidtemp[17] = 0x19;
                hidtemp[19] = 0x02;
                hidtemp[23] = 0x03;
                hidtemp[27] = 0x04;
                hidtemp[29] = 0x19;
                hidtemp[31] = 0x05;
                hidtemp[35] = 0x06;
                hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);





                    SendMultiPkt(buff6, sizeof(buff6)/sizeof(buff6[0]), sizeof(buff6)[0]/sizeof(buff6[0][0]));


                memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hidtemp[1] = 0x08;
                hidtemp[2] = 0x06;
                hidtemp[3] = 0x01;
                hidtemp[4] = 0x1F;
                hidtemp[8] = 0x07;
                hidtemp[10] = 0x07;
                hidtemp[15] = 0x01;
                hidtemp[17] = 0x19;
                hidtemp[19] = 0x02;
                hidtemp[21] = 0x19;
                hidtemp[23] = 0x03;
                hidtemp[27] = 0x04;
                hidtemp[29] = 0x19;
                hidtemp[31] = 0x05;
                hidtemp[35] = 0x06;
                hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);


                    SendMultiPkt(buff6, sizeof(buff6)/sizeof(buff6[0]), sizeof(buff6)[0]/sizeof(buff6[0][0]));

                    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hidtemp[1] = 0x08;
                    hidtemp[2] = 0x06;
                    hidtemp[3] = 0x01;
                    hidtemp[4] = 0x1F;
                    hidtemp[8] = 0x07;
                    hidtemp[10] = 0x07;
                    hidtemp[15] = 0x01;
                    hidtemp[17] = 0x19;
                    hidtemp[19] = 0x02;
                    hidtemp[21] = 0x19;
                    hidtemp[23] = 0x03;
                    hidtemp[25] = 0x19;
                    hidtemp[27] = 0x04;
                    hidtemp[29] = 0x19;
                    hidtemp[31] = 0x05;
                    hidtemp[35] = 0x06;
                    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

                    SendMultiPkt(buff6, sizeof(buff6)/sizeof(buff6[0]), sizeof(buff6)[0]/sizeof(buff6[0][0]));

                    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hidtemp[1] = 0x08;
                    hidtemp[2] = 0x06;
                    hidtemp[3] = 0x01;
                    hidtemp[4] = 0x1F;
                    hidtemp[8] = 0x07;
                    hidtemp[10] = 0x07;
                    hidtemp[13] = 0x55;
                    hidtemp[15] = 0x01;
                    hidtemp[17] = 0x19;
                    hidtemp[19] = 0x02;
                    hidtemp[21] = 0x19;
                    hidtemp[23] = 0x03;
                    hidtemp[25] = 0x19;
                    hidtemp[27] = 0x04;
                    hidtemp[29] = 0x19;
                    hidtemp[31] = 0x05;
                    hidtemp[35] = 0x06;
                    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);


                unsigned char buff8[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x14, 0x00, 0x01, 0x00},
        {0x08, 0x14, 0x01, 0x01, 0x00},
        {0x08, 0x14, 0x02, 0x01, 0x00},
        {0x08, 0x14, 0x03, 0x01, 0x00},
        {0x08, 0x14, 0x04, 0x01, 0x00},
        {0x08, 0x14, 0x05, 0x01, 0x00},
        {0x08, 0x14, 0x06, 0x01, 0x00},

        {0x08, 0x0d, 0x01, 0x1f, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        {0x08, 0x08, 0x01, 0x00, 0x00},

        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x0d, 0x01, 0x60, 0x6d},             //change 64 - 60
        {0x08, 0x09, 0x01, 0x00, 0x00},
                    };

                    SendMultiPkt(buff8, sizeof(buff8)/sizeof(buff8[0]), sizeof(buff8)[0]/sizeof(buff8[0][0]));


                    unsigned char buff9[][15] = {

                        {0x08, 0x06, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0xa4, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x0d, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x0d, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                    };

                    SendMultiPktLg(buff9, sizeof(buff9)/sizeof(buff9[0]), sizeof(buff9)[0]/sizeof(buff9[0][0]));


                    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hidtemp[1] = 0x08;
                    hidtemp[2] = 0x06;
                    hidtemp[3] = 0x01;
                    hidtemp[4] = 0x0F;
                    hidtemp[8] = 0x0d;
                    hidtemp[10] = 0x07;
                    hidtemp[11] = 0x01;
                    hidtemp[12] = 0x08;
                    hidtemp[13] = 0x01;
                    hidtemp[14] = 0x06;
                    hidtemp[15] = 0x01;
                    hidtemp[16] = 0x06;
                    hidtemp[17] = 0x01;
                    hidtemp[18] = 0x06;
                    hidtemp[20] = 0x01;
                    hidtemp[21] = 0x06;
                    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

                    unsigned char buff10[][5] = {


        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x15, 0x01, 0x00, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        {0x08, 0x08, 0x01, 0x00, 0x00},
};
                    SendMultiPkt(buff10, sizeof(buff10)/sizeof(buff10[0]), sizeof(buff10)[0]/sizeof(buff10[0][0]));

                    unsigned char buff11[][15] = {

                        {0x08, 0x06, 0x00, 0xf1, 0x01, 0x00, 0x00, 0x12, 0x00, 0xe8, 0x00, 0x2e, 0xe8, 0x00, 0x2e},
                        {0x08, 0x0d, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                        {0x08, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

                    };

                    SendMultiPktLg(buff11, sizeof(buff11)/sizeof(buff11[0]), sizeof(buff11)[0]/sizeof(buff11[0][0]));

                    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hidtemp[1] = 0x08;
                    hidtemp[2] = 0x06;
                    hidtemp[3] = 0x01;
                    hidtemp[4] = 0x0F;
                    hidtemp[8] = 0x07;
                    hidtemp[10] = 0x07;
                    hidtemp[13] = 0x55;
                    hidtemp[15] = 0x01;
                    hidtemp[17] = 0x20;
                    hidtemp[19] = 0x02;
                    hidtemp[21] = 0x19;
                    hidtemp[23] = 0x03;
                    hidtemp[25] = 0x19;
                    hidtemp[27] = 0x04;
                    hidtemp[29] = 0x19;
                    hidtemp[31] = 0x05;
                    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
                    hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

                    unsigned char buff12[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x0d, 0x01, 0x18, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
    };
                    SendMultiPkt(buff12, sizeof(buff12)/sizeof(buff12[0]), sizeof(buff12)[0]/sizeof(buff12[0][0]));



    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hidtemp[0] = 0x00;
    hidtemp[1] = 0x08;
    hidtemp[2] = 0x06;
    hidtemp[4] = 0x02;
    hidtemp[8] = 0x12;
    //hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    /*-------------------------------------*\
    | Prepare controller for detect command |
    \*-------------------------------------*/
    unsigned char buffarray[][5] = {
        {0x08, 0x02, 0x03, 0x00, 0x00},
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x01, 0x20, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    /*----------------------------*\
    | Send rgb port detect request |
    \*----------------------------*/

    memset(hidtemp, 0, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hidtemp[0] = 0x00;
    hidtemp[1] = 0x08;
    hidtemp[2] = 0x08;
    hidtemp[3] = 0x01;
    hid_write(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    /*-------------*\
    | Read response |
    \*-------------*/
    hid_read(dev, hidtemp, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    for(int i=8; i<=32; i=i+4){

        /*------------------------------------*\
        | Save number of LEDs in each rgb port |
        \*------------------------------------*/
        fanleds.push_back((int) hidtemp[i]);
        std::cout<<(int) hidtemp[i-2]<<":"<<(int) hidtemp[i]<<" "<<std::endl;
    }

    SetFanMode();
    //send_keepalive = 1;

    return fanleds;
}



/*------------------------------------------------------*\
|Set to QL Fan mode as workaround to allow mixed rgb fans|
\*------------------------------------------------------*/
void CorsairCommanderCoreController::SetFanMode()
{
    /*-----------------------------------------------------------------------*\
    | Controller needs to be in software mode before setting the rgb fan mode |
    \*-----------------------------------------------------------------------*/
    SendCommit();

    /*--------------------------------------------------------------------------------------------------*\
    | Force controller to 6 QL fan mode to expose maximum number of LEDs per rgb port (34 LEDs per port) |
    \*--------------------------------------------------------------------------------------------------*/

    unsigned char usb_buf[1025];
    unsigned char buffarray[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    memset(usb_buf, 0, sizeof(usb_buf));
    usb_buf[0]  = 0x00;
    usb_buf[1]  = 0x08;
    usb_buf[2]  = 0x06;
    usb_buf[3]  = 0x00;
    usb_buf[4]  = 0x02;
    usb_buf[8]  = 0x12;
    hid_write(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_COMMANDER_CORE_PACKET_SIZE);

    unsigned char buffarray4[][5] = {
        {0x08, 0x0D, 0x01, 0x1E, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00}
    };
    SendMultiPkt(buffarray4, sizeof(buffarray4)/sizeof(buffarray4[0]), sizeof(buffarray4)[0]/sizeof(buffarray4[0][0]));

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

    send_keepalive = 1;
    //fanleds = DetectRGBFans();
}
