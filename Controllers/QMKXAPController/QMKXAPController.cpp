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

void QMKXAPController::SendRequest(subsystem_route_t route, xap_id_t sub_route)
{
    XAPRequestHeader header;
    header.token = GenerateToken();
    header.payload_length = 2;
    header.route = route;
    header.sub_route = sub_route;

    LOG_TRACE("[QMK XAP] Requesting 0x%02x 0x%02x with token %d", route, sub_route, header.token);
    hid_write(dev, (unsigned char *)(&header), sizeof(XAPRequestHeader));
}

int QMKXAPController::ReceiveResponse()
{
    XAPResponseHeader header;

    // This will retry reading a response if the tokens don't match because
    // there could be extra responses from broadcasts or other XAP clients' requests.
    for (;;) {
        hid_read_timeout(dev, (unsigned char *)(&header), sizeof(header), XAP_TIMEOUT);

        // If something was wrong with the response, discard the payload
        if (!(header.flags & XAP_RESPONSE_SUCCESS) || header.token != last_token)
        {
            unsigned char* temp = (unsigned char *)malloc(header.payload_length);
            hid_read_timeout(dev, temp, header.payload_length, XAP_TIMEOUT);
            free(temp);
        }
        else
        {
            LOG_TRACE("[QMK XAP] Received sucessful packet with token %d", header.token);
            return header.payload_length;
        }

        if (!(header.flags & XAP_RESPONSE_SUCCESS))
        {
            LOG_TRACE("");
            return -1;
        }
        LOG_DEBUG("[QMK XAP] Received token %d doesn't match last sent token %d.  Retrying...", header.token, last_token);
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

std::string QMKXAPController::GetVersion()
{
    SendRequest(XAP_SUBSYSTEM, 0x00);

    uint32_t version = ReceiveU32();
    return std::to_string((version >> 24) & 0x000000FF) + "." + std::to_string((version >> 16) & 0x000000FF) + "." + std::to_string(version & 0x0000FFFF);
}

std::string QMKXAPController::GetHWID()
{
    SendRequest(XAP_SUBSYSTEM, 0x08);

    XAPHWID id;
    int data_len = ReceiveResponse();

    if (data_len != sizeof(XAPHWID)) return "";

    hid_read(dev, (unsigned char *)(&id), data_len);

    return std::to_string(id.id[0]) + std::to_string(id.id[1]) + std::to_string(id.id[2]) + std::to_string(id.id[3]);
}

bool QMKXAPController::CheckSubsystems()
{
    SendRequest(XAP_SUBSYSTEM, 0x02);
    uint32_t enabled_subsystems = ReceiveU32();

    return (NECESSARY_SUBSYSTEMS & enabled_subsystems) == NECESSARY_SUBSYSTEMS;
}