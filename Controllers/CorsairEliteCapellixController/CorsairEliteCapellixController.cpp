/*---------------------------------------------------------*\
|  Processing Code for Corsair Capellix Series              |
|                                                           |
|  Jeff P.                                                  |
\*---------------------------------------------------------*/

#include "CorsairEliteCapellixController.h"

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std::chrono_literals;

CorsairEliteCapellixController::CorsairEliteCapellixController(hid_device* dev_handle, const char* path)
{
    dev = dev_handle;
    keepalive_thread_run = 1;
    fan_mode_set = 0;
    keepalive_thread = new std::thread(&CorsairEliteCapellixController::KeepaliveThread, this);
}

CorsairEliteCapellixController::~CorsairEliteCapellixController()
{
    keepalive_thread_run = 0;
    keepalive_thread->join();
    delete keepalive_thread;
    hid_close(dev);
}

/*----------------------------*/
//Detects rgb connected to hub//
/*----------------------------*/
std::vector<int> CorsairEliteCapellixController::DetectRGBFans()
{
    unsigned char buffarray[][5] = {
        {0x08, 0x01, 0x03, 0x00, 0x02},
        {0x08, 0x0d, 0x01, 0x20, 0x00},
        {0x08, 0x09, 0x01, 0x00, 0x00},
        //{0x08, 0x08, 0x01, 0x00, 0x00},
    };
    SendMultiPkt(buffarray, sizeof(buffarray)/sizeof(buffarray[0]), sizeof(buffarray)[0]/sizeof(buffarray[0][0]));

    unsigned char* hidtemp = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(hidtemp, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    unsigned char* res = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(res, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    hidtemp[0] = 0x00;
    hidtemp[1] = 0x08;
    hidtemp[2] = 0x08;
    hidtemp[3] = 0x01;
    hid_write(dev, hidtemp, CORSAIR_CAPELLIX_PACKET_SIZE); //send rgb fan detect request
    hid_read(dev, res, CORSAIR_CAPELLIX_PACKET_SIZE); //read response
    for(int i=12; i<=32; i=i+4){
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

void CorsairEliteCapellixController::KeepaliveThread()
{
    while(keepalive_thread_run.load())
    {
        if (fan_mode_set){
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
void CorsairEliteCapellixController::SendMultiPkt(unsigned char buffarray[][5], int r, int c){
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

void CorsairEliteCapellixController::SetDirectColor(
        std::vector<RGBColor> colors
        )
{
    int led_offset=0;
    unsigned char* usb_buf = new unsigned char[CORSAIR_CAPELLIX_PACKET_SIZE];
    memset(usb_buf, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;

    for(int i=0; i<colors.size(); i++){

        /*-------------------*/
        //Set pump led colors//
        /*-------------------*/
        if(i<29){
            led_offset = 10+(3*i);
            usb_buf[led_offset] = RGBGetRValue(colors[i]);
            usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
            usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
        }

        /*------------------*/
        //Set fan led colors//
        /*------------------*/
        else{
            for(int j=0; j<fanleds.size(); j++){

                /*------------------------*/
                //Special case for QL fans//
                /*------------------------*/
                if(fanleds[j] == 34){
                    for(int k=0; k<fanleds[j]; k++){

                        if(k<4){ //front middle 4 LEDs
                            led_offset = CORSAIR_CAPELLIX_PREAMBLE_OFFSET + CORSAIR_CAPELLIX_PUMP_LED_OFFSET + (j*CORSAIR_CAPELLIX_QL_FAN_ZONE_OFFSET) + (k*3);
                            usb_buf[led_offset] = RGBGetRValue(colors[i]);
                            usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
                            usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
                            i++;
                        }

                        else if(k<16){ //back middle 6 LEDs
                            led_offset = CORSAIR_CAPELLIX_PREAMBLE_OFFSET + CORSAIR_CAPELLIX_PUMP_LED_OFFSET + (j*CORSAIR_CAPELLIX_QL_FAN_ZONE_OFFSET) + (k*3) + 18;
                            usb_buf[led_offset] = RGBGetRValue(colors[i]);
                            usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
                            usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
                            i++;
                        }

                        else if(k<22){ //front outer 12 LEDs
                            led_offset = CORSAIR_CAPELLIX_PREAMBLE_OFFSET + CORSAIR_CAPELLIX_PUMP_LED_OFFSET + (j*CORSAIR_CAPELLIX_QL_FAN_ZONE_OFFSET) + (k*3) - 36;
                            usb_buf[led_offset] = RGBGetRValue(colors[i]);
                            usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
                            usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
                            i++;
                        }

                        else{ //back outer 12 LEDs
                            led_offset = CORSAIR_CAPELLIX_PREAMBLE_OFFSET + CORSAIR_CAPELLIX_PUMP_LED_OFFSET + (j*CORSAIR_CAPELLIX_QL_FAN_ZONE_OFFSET) + (k*3);
                            usb_buf[led_offset] = RGBGetRValue(colors[i]);
                            usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
                            usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
                            i++;
                        }
                    }
                }

                /*--------------*/
                //All other fans//
                /*--------------*/
                else{
                    for(int k=0; k<fanleds[j]; k++){
                        led_offset = 10+87+(k*3)+(j*102);
                        usb_buf[led_offset] = RGBGetRValue(colors[i]);
                        usb_buf[led_offset+1] = RGBGetGValue(colors[i]);
                        usb_buf[led_offset+2] = RGBGetBValue(colors[i]);
                        i++;
                    }
                }
            }
        }

    };
    last_commit_time = std::chrono::steady_clock::now(); //sending a direct mode color packet resets the timeout
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    //hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
}

void CorsairEliteCapellixController::SendCommit()
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
    SetDirectColor(std::vector<RGBColor>{RGBColor ToRGBColor(0,0,0)}); //softlocks if color isn't set at least once
}


/*------------------------------------------------------*\
|Set to QL Fan mode as workaround to allow mixed rgb fans|
\*------------------------------------------------------*/
void CorsairEliteCapellixController::SetFanMode()
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
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);

    unsigned char buffarray2[][5] = {
        {0x08, 0x05, 0x01, 0x01, 0x00},
        {0x08, 0x15, 0x01, 0x00, 0x00}
    };
    SendMultiPkt(buffarray2, sizeof(buffarray2)/sizeof(buffarray2[0]), sizeof(buffarray2)[0]/sizeof(buffarray2[0][0]));

    memset(usb_buf, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);

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
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);

    SendMultiPkt(buffarray2, sizeof(buffarray2)/sizeof(buffarray2[0]), sizeof(buffarray2)[0]/sizeof(buffarray2[0][0]));

    memset(usb_buf, 0, CORSAIR_CAPELLIX_PACKET_SIZE);
    usb_buf[0] = 0x00;
    usb_buf[1] = 0x08;
    usb_buf[2] = 0x06;
    usb_buf[4] = 0xF1;
    usb_buf[5] = 0x01;
    usb_buf[8] = 0x12;
    hid_write(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);
    hid_read(dev, usb_buf, CORSAIR_CAPELLIX_PACKET_SIZE);

    fan_mode_set = 1;
}
