/*-------------------------------------------------------------------*\
|  QMKXAPController.cpp                                               |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#include "QMKXAPController.h"


QMKXAPController::QMKXAPController(hid_device *dev_handle)
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

int QMKXAPController::ReceiveResponse()
{
    XAPResponseHeader header;

    // This will retry reading a response if the tokens don't match because
    // there could be extra responses from broadcasts or other XAP clients' requests.
    for (;;) {
        hid_read(dev, (unsigned char *)(&header), sizeof(header));

        // If something was wrong with the response, discard the payload
        if (!(header.flags & XAP_RESPONSE_SUCCESS) || header.token != last_token)
        {
            hid_read(dev, nullptr, header.payload_length);
        }
        else
        {
            return header.payload_length;
        }

        if (!(header.flags & XAP_RESPONSE_SUCCESS)) return -1;
    }
}

std::string QMKXAPController::ReceiveString()
{
    int data_length = ReceiveResponse();

    if (data_length < 0) return "";

    std::vector<char> data(data_length);

    hid_read(dev, reinterpret_cast<unsigned char*>(&data[0]), data_length);

    std::string s(data.begin(), data.end());
    return s;
}

uint32_t QMKXAPController::ReceiveU32()
{
    int data_length = ReceiveResponse();

    if (data_length != 4) return 0;

    uint32_t n;

    hid_read(dev, (unsigned char *)(&n), data_length);

    return n;
}

std::string QMKXAPController::GetName()
{
    SendRequest(QMK_SUBSYSTEM, 0x04);
    return ReceiveString();
}

std::string QMKXAPController::GetManufacturer()
{
    SendRequest(QMK_SUBSYSTEM, 0x03);
    return ReceiveString();
}

bool QMKXAPController::CheckSubsystems()
{
    SendRequest(XAP_SUBSYSTEM, 0x02);
    uint32_t enabled_subsystems = ReceiveU32();

    return (NECESSARY_SUBSYSTEMS & enabled_subsystems) == NECESSARY_SUBSYSTEMS;
}
