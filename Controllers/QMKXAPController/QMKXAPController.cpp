/*-------------------------------------------------------------------*\
|  QMKXAPController.h                                                 |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#include "QMKXAPController.h"


QMKXAPController::QMKXAPController(hid_device *dev_handle, const char *path)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<xap_token_t> distribution(0x0100, 0xFFFF);
    rng = std::bind(distribution, generator);
}

uint16_t QMKXAPController::GenerateToken()
{
    xap_token_t n;
    do
    {
        n = rng();
    } while (n == 0xFFFE || n == 0xFFFF); // Reserved
    last_token = n;
    return n;
}

void QMKXAPController::SendRequest(xap_id_t route, xap_id_t sub_route)
{
    XAPRequestHeader header;
    header.token = GenerateToken();
    header.payload_length = 2;
    header.route = route;
    header.sub_route = sub_route;
    hid_write(dev, (unsigned char *)(&header), sizeof(XAPRequestHeader));
}
