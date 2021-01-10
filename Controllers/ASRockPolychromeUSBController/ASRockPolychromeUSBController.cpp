/*-----------------------------------------*\
|  ASRockPolychromeUSBController.cpp        |
|                                           |
|  Driver for ASROCK  RGB USB               |
|  lighting controller                      |
|                                           |
|  Ed Kambulow (dredvard) 12/20/2020        |
\*-----------------------------------------*/

#include "RGBController.h"
#include "ASRockPolychromeUSBController.h"
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include "dependencies/dmiinfo.h"

#define POLYCHROME_USB_READ_ZONE_CONFIG 0x11
#define POLYCHROME_USB_READ_HEADER 0x14
#define POLYCHROME_USB_WRITE_HEADER 0x15
#define POLYCHROME_USB_SET 0x10
#define POLYCHROME_USB_INIT 0xa4
#define POLYCHROME_USB_EXIT 0X12

#define POLYCHROME_USB_ADDRESS_CFG 02
#define POLYCHROME_USB_RGSWAP_CFG 03

void display_buf( unsigned char usb_buf[]){

    for (int i=0;i<10;i++){
        printf("%02hhX ", (unsigned char) usb_buf[i]);
        }
    printf("\n");
};




PolychromeUSBController::PolychromeUSBController(hid_device* dev_handle, const char* path)
{

    dev         = dev_handle;
    location    = path;
    unsigned char zone=0;
    DMIInfo dmi;
    //GetFirmwareVersion();
    //GetConfigTable();
    device_name = "ASRock " + dmi.getMainboard();
    asrock_type = ASROCK_TYPE_POLYCHROME_USB;
    // TODO Read all Zone Configs
    zone=1;

    //ReadZoneConfig(zone);
    SetDeviceInfo();
    //ReadLEDConfiguration(zone);

}

unsigned int PolychromeUSBController::GetASRockType()
{
    return(asrock_type);
}

PolychromeUSBController::~PolychromeUSBController()
{

}

unsigned int PolychromeUSBController::GetChannelCount()
{
    return(device_info.size());
}

std::string PolychromeUSBController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string PolychromeUSBController::GetDeviceName()
{
    return(device_name);
}

std::string PolychromeUSBController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
};


    // Don't know how to get Config Table so will just generate if for now
    // TODO Not sure if to implement
void PolychromeUSBController::SetDeviceInfo(){

    // Don't know how to get Config Table so will just generate if for now

    unsigned int max_chns= 6;
    PolychromeDeviceInfo newdev_info;
    for (unsigned int channel_idx = 0; channel_idx < max_chns; channel_idx++)
    {
        switch(channel_idx)
        {
            // Type: Addressable
            case POLYCHROME_USB_ZONE_ARGB1:
            case POLYCHROME_USB_ZONE_ARGB2:
                newdev_info.num_leds = 1;
                //newdev_info.effect_channel=channel_idx;
                newdev_info.device_type=PolychromeDeviceType::ADDRESSABLE;

                break;



            // Might be Addressable
            case POLYCHROME_USB_ZONE_PCH :
            case POLYCHROME_USB_ZONE_IOCOVER:
            case POLYCHROME_USB_ZONE_PCB :
            case POLYCHROME_USB_ZONE_AUDIO:
                newdev_info.num_leds = 1;
                newdev_info.device_type=PolychromeDeviceType::FIXED;
                break;

            // TYpe: Fixed
            case POLYCHROME_USB_ZONE_RGB1:
            case POLYCHROME_USB_ZONE_RGB2:
            default:
                newdev_info.num_leds = 1;
                newdev_info.device_type=PolychromeDeviceType::FIXED;
                break;
        }
        device_info.push_back(newdev_info);

    }

};



// TODO GetFirwareVersion - Requires Reverse Engineering
void PolychromeUSBController::GetFirmwareVersion()
{
    // Don't know how 
    return;
}

void PolychromeUSBController::SendDirect
    (
    unsigned char   device,
    unsigned char   start_led,
    unsigned char   led_count,
    unsigned char*  led_data,
    bool apply /* = false */
    )
{
    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = 0xEC;  // Needs to be written for MB
    usb_buf[0x01]   = 0x00;  // AURA_CONTROL_MODE_DIRECT;
    usb_buf[0x02]   = apply ? 0x80 : 0x00;
    usb_buf[0x02]  |= device;
    usb_buf[0x03]   = start_led;
    usb_buf[0x04]   = led_count;

    /*-----------------------------------------------------*\
    | Copy in color data bytes                              |
    \*-----------------------------------------------------*/
    memcpy(&usb_buf[0x05], led_data, led_count * 3);

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 65);
}

void PolychromeUSBController::SendDirect(
        unsigned char   zone,
        unsigned char   mode,
        unsigned char  speed,
        RGBColor   rgb,  
        bool allzone = false
        )
    {

    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
    

	usb_buf[0x00] = POLYCHROME_USB_SET;
	usb_buf[0x02] = zone;
	usb_buf[0x03] = mode;
	usb_buf[0x04] = RGBGetRValue(rgb);
	usb_buf[0x05] = RGBGetGValue(rgb);
	usb_buf[0x06] = RGBGetBValue(rgb);
	usb_buf[0x07] = speed;
	usb_buf[0x08] = 0xFF;
	usb_buf[0x09] = allzone;

	int written = 0;
    //
	// int ret = libusb_bulk_transfer(dev, (2 | LIBUSB_ENDPOINT_OUT), packet.data(), 64, &written, 1000);
    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 65);
    hid_read(dev, usb_buf, 65);
    

};

void PolychromeUSBController::WriteZone(
        unsigned char   zone,
        unsigned char   mode,
        unsigned char  speed,
        RGBColor   rgb, 
        bool allzone = false
        )
    {

    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
    

	usb_buf[0x00] = POLYCHROME_USB_SET;
	usb_buf[0x02] = zone;
	usb_buf[0x03] = mode;
	usb_buf[0x04] = RGBGetRValue(rgb);
	usb_buf[0x05] = RGBGetGValue(rgb);
	usb_buf[0x06] = RGBGetBValue(rgb);
	usb_buf[0x07] = speed;
	usb_buf[0x08] = 0xFF;
	usb_buf[0x09] = allzone;

	int written = 0;
    //
	// int ret = libusb_bulk_transfer(dev, (2 | LIBUSB_ENDPOINT_OUT), packet.data(), 64, &written, 1000);
    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 64);
     if (PDEBUG)
    {
        printf("Zone write: ");
        display_buf(usb_buf);
    }
    /*-----------------------------------------------------*\
    | Read Response                                         |
    \*-----------------------------------------------------*/
    hid_read(dev, usb_buf, 64);
    
    //if ((usb_buf[0]==0xa6)  || (usb_buf[3]!=07) || (usb_buf[4]!=00)){
    if (PDEBUG)
    {
        printf("Zone Response :  ");
        display_buf(usb_buf);
    }

};



void PolychromeUSBController::ReadLEDConfiguration(unsigned char zone)
{

    //Don't know how to  Read LED Configuraions (e.g. max LED)

};

void PolychromeUSBController::WriteRGSwap(
        bool ahdr1,
        bool ahdr0,
        bool hdr1,
        bool hdr0
        )
    {

    unsigned char usb_buf[65];
    unsigned char RGSwap;

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
    RGSwap=(ahdr1 <<3 &  ahdr0 << 2 & hdr1 << 1  & hdr0) && 0x40;

	usb_buf[0x00] = POLYCHROME_USB_WRITE_HEADER;
    usb_buf[0x03] = POLYCHROME_USB_RGSWAP_CFG;

	usb_buf[0x04] = RGSwap;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 65);
    hid_read(dev, usb_buf, 65);

    
}



void PolychromeUSBController::WriteAddressableHeader(
        unsigned char   zone,
        unsigned char   ahdr0,
        unsigned char   hdr1
        )
    {

    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet                                 |
    \*-----------------------------------------------------*/
    

	usb_buf[0x00] = 0x15;
	usb_buf[0x02] = zone;
	usb_buf[0x03] = 0x01;  
    usb_buf[0x03] = 0x01;

	usb_buf[0x04] = ahdr0;
	usb_buf[0x05] = hdr1;
	usb_buf[0x06] = 0x05;
	usb_buf[0x07] = 0x07;
	usb_buf[0x08] = 0x1e;
    usb_buf[0x09] = 0x1e;


    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 65);
    hid_read(dev, usb_buf, 65);
    
}
PolychromeZoneInfo PolychromeUSBController::GetZoneConfig(unsigned char zone){
    return(ReadZoneConfig(zone));
}


PolychromeZoneInfo PolychromeUSBController::ReadZoneConfig(unsigned char zone)
{
    unsigned char usb_buf[65];
    PolychromeZoneInfo zoneinfo;
    unsigned char spd, all, mode;
    unsigned int rgb;
    unsigned char r,g,b;


    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up config table request packet                    |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = POLYCHROME_USB_READ_ZONE_CONFIG;
    usb_buf[0x02]   = zone; 

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 64);
    if(PDEBUG){
        printf("Writing: ");
        display_buf(usb_buf);
    }
    memset(usb_buf, 0x00, sizeof(usb_buf));
    hid_read(dev, usb_buf, 64);
    if(PDEBUG){
        printf("\n Reading: " );
        display_buf(usb_buf);
    }

    r= usb_buf[0x05];
    g= usb_buf[0x06];
    b = usb_buf[0x07];
    zoneinfo.mode=usb_buf[0x04];
    zoneinfo.color = ToRGBColor(r,g,b);  
    zoneinfo.speed=usb_buf[0x08];
    zoneinfo.zone=usb_buf[0x03];
    all =usb_buf[0x10];
    return(zoneinfo);
}


const std::vector<PolychromeDeviceInfo>& PolychromeUSBController::GetPolychromeDevices() const
{
    return(device_info);
};

// Currently does nothing
void PolychromeUSBController::Commit(){
}

//TODO Sends all Color data.   Only know how to send complete zone change strin
//   Currently just sends first.
void PolychromeUSBController::SetChannelLEDs(unsigned char channel, unsigned char mode, unsigned char speed, RGBColor * colors, unsigned int num_colors)
{
    
    unsigned char   led_data[60];
    unsigned int    leds_sent = 0;

    WriteZone(channel,mode,speed,colors[0],false);
    return;  //  TODO Complete rest of SetChannel LED - Currently only writes 1 led color

    while(leds_sent < num_colors)
    {
        unsigned int leds_to_send = 20;

        if((num_colors - leds_sent) < leds_to_send)
        {
            leds_to_send = num_colors - leds_sent;
        }

        for(unsigned int led_idx = 0; led_idx < leds_to_send; led_idx++)
        {
            led_data[(led_idx * 3) + 0] = RGBGetRValue(colors[led_idx + leds_sent]);
            led_data[(led_idx * 3) + 1] = RGBGetGValue(colors[led_idx + leds_sent]);
            led_data[(led_idx * 3) + 2] = RGBGetBValue(colors[led_idx + leds_sent]);
        }


        // WriteZone(channel,mode,speed,color[0],false);
        /*
        SendDirect
        (
            device_info[channel].direct_channel,
            leds_sent,
            leds_to_send,
            led_data,
            leds_sent + leds_to_send >= num_colors
        );*/

        leds_sent += leds_to_send;
    }

    //SendCommit();
}



void PolychromeUSBController::SetMode(unsigned char zone,unsigned char mode,  unsigned char speed, RGBColor rgb)
{
    //unsigned char led_count_pkt[1]  = { 0x00 };

    bool allzone = false;

    unsigned char r,g,b;
     r=RGBGetRValue(rgb);
     b=RGBGetBValue(rgb);
     g=RGBGetGValue(rgb);

    switch(asrock_type)
    {
        case ASROCK_TYPE_POLYCHROME_USB:
        WriteZone(zone,mode,speed, rgb, allzone);

    }    
}

