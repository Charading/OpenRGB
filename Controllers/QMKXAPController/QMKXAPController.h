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

#include "ResourceManager.h"
#include "RGBController.h"


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
#pragma pack(pop)



class QMKXAPController
{
public:
    QMKXAPController(hid_device *dev_handle, const char *path);


protected:
    hid_device *dev;

private:
    uint16_t GenerateToken();
    void SendRequest(xap_id_t route, xap_id_t sub_route);

    xap_token_t last_token;
    std::function<xap_token_t(void)> rng;
};
