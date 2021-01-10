/*-----------------------------------------*\
|  ASRockPolychromeUSBController.h          |
|                                           |
|  Driver for ASROCK  RGB USB               |
|  lighting controller                      |
|                                           |
|  Ed Kambulow (dredvard) 12/20/2020        |
\*-----------------------------------------*/

#include "RGBController.h"
#include <hidapi/hidapi.h>
#include <string>

#pragma once

#define PDEBUG false

typedef unsigned char	polychrome_dev_id;

enum
{
    ASROCK_TYPE_UNKNOWN                     = 0x00,     /* Unknown Type or Not ASRock Device    */
    ASROCK_TYPE_ASRLED                      = 0x01,     /* ASRock Firmware 1.x - ASR LED        */
    ASROCK_TYPE_POLYCHROME_V1               = 0x02,     /* ASRock Firmware 2.x - Polychrome V1  */
    ASROCK_TYPE_POLYCHROME_V2               = 0x03,     /* ASRock Firmware 3.x - Polychrome V2  */
    ASROCK_TYPE_POLYCHROME_USB              = 0x04,     /* ASRock Firmware 3.x - Polychrome V2  */
};

enum
{
    // Remenents from SMBusController - For Reference in Reverse Engineering only
    // Can be deleted
    /*------------------------------------------------------------------------------------------*\
    | ASRock Common Registers                                                                    |
    \*------------------------------------------------------------------------------------------*/
    ASROCK_REG_FIRMWARE_VER                 = 0x00,     /* Firmware version Major.Minor         */
    ASROCK_REG_MODE                         = 0x30,     /* Mode selection register              */
    ASROCK_REG_LED_SELECT                   = 0x31,     /* LED selection register               */
    /*------------------------------------------------------------------------------------------*\
    | ASRock Polychrome V1/V2 Common Registers                                                   |
    \*------------------------------------------------------------------------------------------*/
    POLYCHROME_REG_LED_CONFIG               = 0x33,     /* LED configuration register           */
    /*------------------------------------------------------------------------------------------*\
    | ASRock Polychrome V1 (Firmware 2.x) Registers                                              |
    \*------------------------------------------------------------------------------------------*/
    POLYCHROME_V1_REG_SET_ALL               = 0x32,     /* Set All register 0x1 = set all       */
    POLYCHROME_V1_REG_ARGB_GRB              = 0x35,     /* ARGB bistream reversing register     */
    /*------------------------------------------------------------------------------------------*\
    | ASRock Polychrome V2 (Firmware 3.x) Registers                                              |
    \*------------------------------------------------------------------------------------------*/
    POLYCHROME_V2_REG_LED_COUNT             = 0x32,     /* Additional LED count register        */
    POLYCHROME_V2_REG_COLOR                 = 0x34,     /* Color register: Red, Green, Blue     */
    POLYCHROME_V2_REG_ARGB_GRB              = 0x35,     /* ARGB bistream reversing register     */
};



/*----------------------------------------------------------------------------------------------*\
| Definitions for Polychrome USB                                                                 |
\*----------------------------------------------------------------------------------------------*/
#define POLYCHROME_USB_NUM_MODES             15          /* Number of Polychrome USB modes        */

enum
{
    POLYCHROME_USB_MODE_OFF                  = 0x00,     /* OFF mode                             */
    POLYCHROME_USB_MODE_STATIC               = 0x01,     /* Static color mode                    */
    POLYCHROME_USB_MODE_BREATHING            = 0x02,     /* BreatHing effect mode                 */
    POLYCHROME_USB_MODE_STROBE               = 0x03,     /* Strobe effect mode                   */
    POLYCHROME_USB_MODE_SPECTRUM_CYCLE       = 0x04,     /* Spectrum Cycle effect mode           */
    POLYCHROME_USB_MODE_RANDOM               = 0x05,     /* Random effect mode                   */
    POLYCHROME_USB_MODE_MUSIC                = 0x06,     /* Random effect mode                   */
    POLYCHROME_USB_MODE_WAVE                 = 0x07,     /* Wave effect mode                     */
    POLYCHROME_USB_MODE_SPRING               = 0x08,     /* Spring effect mode                   */
    POLYCHROME_USB_MODE_STACK                = 0x09,     /* Stack effect mode                    */
    POLYCHROME_USB_MODE_CRAM                 = 0x0A,     /* Cram effect mode                     */
    POLYCHROME_USB_MODE_SCAN                 = 0x0B,     /* Scan effect mode                     */
    POLYCHROME_USB_MODE_NEON                 = 0x0C,     /* Neon effect mode                     */
    POLYCHROME_USB_MODE_WATER                = 0x0D,     /* Water effect mode                    */
    POLYCHROME_USB_MODE_RAINBOW              = 0x0E,     /* Rainbow effect mode                  */
};

enum
{
    POLYCHROME_USB_SPEED_MIN                 = 0xFF,     /* Slowest speed                        */
    POLYCHROME_USB_SPEED_DEFAULT             = 0xE0,     /* Default speed                        */
    POLYCHROME_USB_SPEED_MAX                 = 0x00,     /* Fastest speed                        */
};

enum
{
    POLYCHROME_USB_ZONE_COUNT                   = 0x07,     /* Total number of zones                */
    POLYCHROME_USB_ZONE_ADDRESSABLE_MAX         = 0x64,     /* Maxinum number of ARGB LEDs  ???         */
};


static const char* polychrome_USB_zone_names[] =
{
    "RGB LED 1 Header",
    "RGB LED 2 Header",
    "Addressable Header 1",
    "Addressable Header 2",
    "PCH",
    "IO Cover",
    "PCB",
    "Audio",
};

enum {
    POLYCHROME_USB_ZONE_RGB1         = 0x00,  // RGB Header 1
    POLYCHROME_USB_ZONE_RGB2         = 0X01,  // RGB Header 2
    POLYCHROME_USB_ZONE_ARGB1        = 0X02,  // ARGB Header 1
    POLYCHROME_USB_ZONE_ARGB2        = 0X03,  // ARGB Header 2
    POLYCHROME_USB_ZONE_PCH          = 0X04,  // PCH
    POLYCHROME_USB_ZONE_IOCOVER      = 0X05,  // IOCOVER
    POLYCHROME_USB_ZONE_PCB          = 0X06,  // PCB - Could be mixed swapped with 0x07
    POLYCHROME_USB_ZONE_AUDIO        = 0X07   // AUDIO
};


enum class PolychromeDeviceType
{
    FIXED,
    ADDRESSABLE,
};

struct PolychromeZoneInfo
{
    unsigned char mode;
    unsigned char zone;
    unsigned char speed;
    RGBColor color;
};

struct PolychromeDeviceInfo
{
     unsigned char effect_channel;
    // unsigned char direct_channel;
    unsigned char num_leds;
    PolychromeDeviceType device_type;
};

// TODO Clean up Definition of USBController
class PolychromeUSBController
{
public:
    PolychromeUSBController(hid_device* dev_handle, const char* path);
    virtual ~PolychromeUSBController();
    unsigned int GetChannelCount();
    std::string GetDeviceLocation();
    std::string GetDeviceName();
    std::string GetSerialString();
    void  SetColorsAndSpeed(unsigned char led, unsigned char red, unsigned char green, unsigned char blue);
    
    void  SetMode(unsigned char zone, unsigned char mode, unsigned char speed, RGBColor rgb);
    //void SetMode (unsigned char channel,unsigned char mode,unsigned char red,unsigned char grn,unsigned char blu);
    unsigned char           zone_led_count[8];
    //void SetChannelLEDs (unsigned char channel,RGBColor *  colors,unsigned int  num_colors);
    void SetChannelLEDs(unsigned char channel, unsigned char mode, unsigned char speed, RGBColor * colors, unsigned int num_colors);

    PolychromeZoneInfo GetZoneConfig (unsigned char zone);
    unsigned int GetASRockType();
    const std::vector<PolychromeDeviceInfo>& GetPolychromeDevices() const;
   
protected:
    hid_device*                 dev;
    std::vector<PolychromeDeviceInfo> device_info;
    std::string                 location;

    // TODO implmement send direct - requires Reverse engineering
    void SendDirect(unsigned char device, unsigned char start_led, unsigned char led_count, unsigned char*led_data, bool apply = false);
    void SendDirect(
        unsigned char   zone,
        unsigned char   mode,
        unsigned char  speed,
        RGBColor   rgb,  
        bool allzone
        );

    void ReadLEDConfiguration(unsigned char zone);
    PolychromeZoneInfo ReadZoneConfig(unsigned char zone);
    // unsigned short  ReadFirmwareVersion();
    void WriteRGSwap( bool ahdr1, bool ahdr0, bool hdr1, bool hdr0 );
    void WriteAddressableHeader(unsigned char zone,unsigned char ahdr0,unsigned char hdr1);
    void WriteZone(unsigned char   zone, unsigned char   mode, unsigned char  speed, RGBColor rgb,  bool allzone);

    void Commit();

private:
    //char          device_name[16];
    unsigned int  led_count;
    unsigned int  asrock_type;
    std::string   device_name;

    void SetDeviceInfo();
    void GetConfigTable();
    void GetFirmwareVersion();
};
