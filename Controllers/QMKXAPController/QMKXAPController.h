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
#include <sstream>
#include <chrono>
#include <thread>
#include <QByteArray>
#include <zlib.h>

#include "ResourceManager.h"
#include "LogManager.h"
#include "RGBController.h"

#define XAP_RESPONSE_SUCCESS 1
#define XAP_TIMEOUT 200
#define XAP_MAX_PACKET_SIZE 128

enum subsystem_route_t {
    XAP_SUBSYSTEM       = 0x00,
    QMK_SUBSYSTEM       = 0x01,
    KEYMAP_SUBSYSTEM    = 0x04,
    LIGHTING_SUBSYSTEM  = 0x06
};

#define NECESSARY_SUBSYSTEMS 1 << QMK_SUBSYSTEM // | 1 << KEYMAP_SUBSYSTEM | 1 << LIGHTING_SUBSYSTEM

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



class QMKXAPController
{
public:
    QMKXAPController(hid_device *dev_handle, const char *path);
    ~QMKXAPController();
    std::string     GetName();
    std::string     GetManufacturer();
    std::string     GetVersion();
    std::string     GetHWID();
    std::string     GetLocation();
    bool            CheckSubsystems();
    json            GetConfigBlob();


protected:
    hid_device *dev;

private:
    uint16_t        GenerateToken();
    void            SendRequest(subsystem_route_t route, xap_id_t sub_route);
    int             ReceiveResponse(unsigned char **data);
    std::string     ReceiveString();
    template<class T>
    T              ReceiveNumber();
    QByteArray     gUncompress(const QByteArray &data);

    std::string     location;

    xap_token_t last_token;
    std::function<xap_token_t(void)> rng;
    json config;
};
