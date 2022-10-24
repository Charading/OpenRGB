/*-------------------------------------------------------------------*\
|  QMKXAPController.h                                                 |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/


#pragma once

#include <hidapi/hidapi.h>
#include <cstdint>
#include <random>
#include <functional>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <zlib.h>

#include "ResourceManager.h"
#include "LogManager.h"
#include "RGBController.h"

#define XAP_RESPONSE_SUCCESS 1
#define XAP_TIMEOUT 200
#define XAP_MAX_PACKET_SIZE 128
#define XAP_MAX_RETRIES 5

#define CAPABILITIES(subsystem)     {subsystem, 0x01}

// XAP Subsystem
#define XAP_VERSION_REQUEST         {0x00, 0x00}
#define ENABLED_SUBSYSTEMS          {0x00, 0x02}

// QMK Subsystem
#define KB_MANUFACTURER_REQUEST     {0x01, 0x03}
#define KB_NAME_REQUEST             {0x01, 0x04}
#define KB_HWID_REQUEST             {0x01, 0x08}
#define CONFIG_BLOB_LEN             {0x01, 0x05}
#define CONFIG_BLOB_CHUNK           {0x01, 0x06}

// Keymap Subsystem
#define KEYCODE_REQUEST             {0x04, 0x03}

// Lighting Subsystem


enum subsystem_route_t {
    XAP_SUBSYSTEM       = 0x00,
    QMK_SUBSYSTEM       = 0x01,
    KEYMAP_SUBSYSTEM    = 0x04,
    LIGHTING_SUBSYSTEM  = 0x06
};

enum lighting_type {
    RGBLIGHT    = 0x03,
    RGBMATRIX   = 0x04,
    NONE        = -1
};

#define NECESSARY_SUBSYSTEMS ((1 << QMK_SUBSYSTEM) | (1 << KEYMAP_SUBSYSTEM) | (1 << LIGHTING_SUBSYSTEM))
#define MIN_XAP_VERSION 0, 3, 0

template <class T>
using VectorMatrix = std::vector<std::vector<T>>;

typedef uint16_t xap_token_t;
typedef uint8_t xap_response_flags_t;
typedef uint8_t xap_id_t;
typedef std::tuple<uint8_t, uint8_t, uint16_t> xap_version;

#pragma pack(push, 1)
typedef struct
{
    xap_token_t token;
    uint8_t payload_length;
} XAPRequestHeader;

typedef struct
{
    xap_token_t token;
    xap_response_flags_t flags;
    uint8_t payload_length;
} XAPResponseHeader;

typedef struct {
    uint32_t id[4];
} XAPHWID;
#pragma pack(pop)

typedef struct {
    bool success;
    std::vector<unsigned char> payload;
} XAPResponsePacket;

typedef struct {
    std::string label;
    unsigned int x;
    unsigned int y;
    unsigned int flags;
    int matrix_x;
    int matrix_y;
} XAPLED;


class QMKXAPController
{
public:
    QMKXAPController(hid_device *dev_handle, const char *path);
    ~QMKXAPController();
    std::string             GetName();
    std::string             GetManufacturer();
    std::string             GetVersion();
    std::string             GetHWID();
    std::string             GetLocation();
    lighting_type           CheckKeyboard();
    VectorMatrix<uint16_t>  GetKeycodeMap();
    std::vector<XAPLED>     GetLEDs();


protected:
    hid_device *dev;

private:
    xap_token_t                 GenerateToken();
    void                        SendRequest(std::vector<xap_id_t> route);
    void                        SendRequest(std::vector<xap_id_t> route, std::vector<unsigned char> payload);
    XAPResponsePacket           ReceiveResponse(int response_length = XAP_MAX_PACKET_SIZE - sizeof(XAPResponseHeader));
    std::string                 ReceiveString();
    template<class T>
    T                           ReceiveNumber();
    void                        LoadConfigBlob();
    std::vector<unsigned char>  gUncompress(const std::vector<unsigned char> &data);
    uint16_t                    GetKeycode(uint8_t layer, uint8_t row, uint8_t column);

    std::string     location;

    xap_token_t last_token;
    std::function<xap_token_t(void)> rng;
    json config;
    xap_version version;
};
