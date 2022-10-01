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

enum subsystem_route_t {
    XAP_SUBSYSTEM       = 0x00,
    QMK_SUBSYSTEM       = 0x01,
    KEYMAP_SUBSYSTEM    = 0x04,
    LIGHTING_SUBSYSTEM  = 0x06
};

#define NECESSARY_SUBSYSTEMS 1 << QMK_SUBSYSTEM | 1 << KEYMAP_SUBSYSTEM | 1 << LIGHTING_SUBSYSTEM

typedef uint16_t xap_token_t;
typedef uint8_t xap_response_flags_t;
typedef uint8_t xap_id_t;

#pragma pack(push, 1)
typedef struct
{
    xap_token_t token;
    uint8_t payload_length;
    xap_id_t route;
    xap_id_t sub_route;
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
} XAPLED;


class QMKXAPController
{
public:
    QMKXAPController(hid_device *dev_handle, const char *path);
    ~QMKXAPController();
    std::string                         GetName();
    std::string                         GetManufacturer();
    std::string                         GetVersion();
    std::string                         GetHWID();
    std::string                         GetLocation();
    bool                                CheckKeyboard();
    std::vector<std::vector<uint16_t>>  GetMatrixMask()
    std::vector<XAPLED>                 GetLEDs();


protected:
    hid_device *dev;

private:
    xap_token_t                 GenerateToken();
    void                        SendRequest(subsystem_route_t route, xap_id_t sub_route);
    void                        SendRequest(subsystem_route_t route, xap_id_t sub_route, std::vector<unsigned char> payload);
    XAPResponsePacket           ReceiveResponse();
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
};
