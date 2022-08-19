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
    std::uniform_int_distribution<xap_token_t> distribution(0x0100, 0xFFFF);
    rng = std::bind(distribution, generator);
}

QMKXAPController::~QMKXAPController()
{
    hid_close(dev);
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
    int res = hid_write(dev, (unsigned char *)(&header), sizeof(XAPRequestHeader));

    if (res < 0) LOG_TRACE("[QMK XAP] Error writing to device: %ls", hid_error(dev));
    LOG_TRACE("[QMK XAP] Sent request");
}

int QMKXAPController::ReceiveResponse()
{
    unsigned char buf[sizeof(XAPResponseHeader)];
    XAPResponseHeader header;
    int resp;

    // This will retry reading a response if the tokens don't match because
    // there could be extra responses from broadcasts or other XAP clients' requests.
    for (;;) {
        LOG_TRACE("[QMK XAP] Receiving response...");
        resp = hid_read_timeout(dev, buf, sizeof(XAPResponseHeader), XAP_TIMEOUT);
        LOG_TRACE("[QMK XAP] hid_read_timeout returned %d", resp);

        if (resp < 0) {
            LOG_TRACE("[QMK XAP] Error reading from device: %ls", hid_error(dev));
            return -1;
        }

        std::stringstream log;
        log << "[QMK XAP] Data received:\n\t";
        for (unsigned int i = 0; i < sizeof(XAPResponseHeader); i++) {
            log << "0x" << std::hex << static_cast<int>(buf[i]) << " ";
        }
        LOG_TRACE(&log.str()[0]);

        std::memcpy(&header, buf, sizeof(XAPResponseHeader));

        LOG_TRACE("[QMK XAP] Received header:\n\ttoken: %d\n\tflags: 0x%08x\n\tpayload_length: %d\n\t", header.token, header.flags, header.payload_length);

        // If something was wrong with the response, discard the payload
        if ((!(header.flags & XAP_RESPONSE_SUCCESS) || header.token != last_token) && header.payload_length > 0)
        {
            unsigned char* temp = (unsigned char *)malloc(header.payload_length);
            hid_read_timeout(dev, temp, header.payload_length, XAP_TIMEOUT);
            free(temp);
        }
        
        if (header.token != last_token)
        {
            LOG_DEBUG("[QMK XAP] Received token %d doesn't match last sent token %d.  Retrying...", header.token, last_token);
            continue;   
        }
        else if (header.flags & XAP_RESPONSE_SUCCESS)
        {
            return header.payload_length;
        }
        else
        {
            LOG_DEBUG("[QMK XAP] Received unsuccessfull response with token %d", header.token);
            return -1;
        }
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
    LOG_TRACE("[QMK XAP] Receiving U32");
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
