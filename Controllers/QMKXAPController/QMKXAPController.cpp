/*-------------------------------------------------------------------*\
|  QMKXAPController.cpp                                               |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#include "QMKXAPController.h"


QMKXAPController::QMKXAPController(hid_device *dev_handle, const char *path)
{
    dev         = dev_handle;
    location    = path;
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<xap_token_t> distribution(0x0100, 0xFFFD);
    rng = std::bind(distribution, generator);
}

QMKXAPController::~QMKXAPController()
{
    hid_close(dev);
}

uint16_t QMKXAPController::GenerateToken()
{
    xap_token_t n = rng();
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

    LOG_TRACE("[QMK XAP] Requesting 0x%02x 0x%02x with token %04X", route, sub_route, header.token);
    int res = hid_write(dev, (unsigned char *)(&header), sizeof(XAPRequestHeader));

    if (res < 0) LOG_TRACE("[QMK XAP] Error writing to device: %ls", hid_error(dev));
    LOG_TRACE("[QMK XAP] Sent request");
}

int QMKXAPController::ReceiveResponse(unsigned char **data)
{
    unsigned char buf[XAP_MAX_PACKET_SIZE];
    XAPResponseHeader header;
    int resp;

    // This will retry reading a response if the tokens don't match because
    // there could be extra responses from broadcasts or other XAP clients' requests.
    for (;;) {
        LOG_TRACE("[QMK XAP] Receiving response...");
        resp = hid_read_timeout(dev, buf, XAP_MAX_PACKET_SIZE, XAP_TIMEOUT);
        LOG_TRACE("[QMK XAP] hid_read_timeout returned %d", resp);

        if (resp < 0) {
            LOG_TRACE("[QMK XAP] Error reading from device: %ls", hid_error(dev));
            return -1;
        }

        std::stringstream log;
        log << "[QMK XAP] Data received:\n\t";
        for (unsigned int i = 0; i < resp; i++) {
            log << "0x" << std::hex << static_cast<int>(buf[i]) << " ";
        }
        LOG_TRACE(&log.str()[0]);

        std::memcpy(&header, buf, sizeof(XAPResponseHeader));

        LOG_TRACE("[QMK XAP] Received header:\n\ttoken: 0x%04X\n\tflags: 0x%08X\n\tpayload_length: %d\n\t", header.token, header.flags, header.payload_length);

        if (header.token != last_token)
        {
            LOG_DEBUG("[QMK XAP] Received token %04X doesn't match last sent token %04X.  Retrying...", header.token, last_token);
            continue;
        }
        else if (header.flags & XAP_RESPONSE_SUCCESS)
        {
            if ((resp - sizeof(XAPResponseHeader)) < header.payload_length) return -1;
            unsigned char* payload = new unsigned char[header.payload_length];
            std::memcpy(payload, buf + sizeof(XAPResponseHeader), header.payload_length);
            *data = payload;
            return header.payload_length;
        }
        else
        {
            LOG_DEBUG("[QMK XAP] Received unsuccessful response with token %d", header.token);
            return -1;
        }
    }
}

std::string QMKXAPController::ReceiveString()
{
    unsigned char* data;
    int data_length = ReceiveResponse(&data);

    if (data_length < 0) return "";

    std::string s((const char *)data);

    delete [] data;
    return s;
}

uint32_t QMKXAPController::ReceiveU32()
{
    LOG_TRACE("[QMK XAP] Receiving U32");
    unsigned char* data;

    int data_length = ReceiveResponse(&data);

    if (data_length < 4) return 0;

    uint32_t n;

    std::memcpy(&n, data, sizeof(n));
    LOG_TRACE("[QMK XAP] Received U32: %d", n);

    delete [] data;
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
    unsigned char* data;
    int data_length = ReceiveResponse(&data);

    if (data_length != sizeof(XAPHWID)) return "";

    std::memcpy(&id, data, data_length);
    delete [] data;

    return std::to_string(id.id[0]) + std::to_string(id.id[1]) + std::to_string(id.id[2]) + std::to_string(id.id[3]);
}

std::string QMKXAPController::GetLocation()
{
    return location;
}

bool QMKXAPController::CheckSubsystems()
{
    SendRequest(XAP_SUBSYSTEM, 0x02);
    uint32_t enabled_subsystems = ReceiveU32();

    return (NECESSARY_SUBSYSTEMS & enabled_subsystems) == NECESSARY_SUBSYSTEMS;
}
