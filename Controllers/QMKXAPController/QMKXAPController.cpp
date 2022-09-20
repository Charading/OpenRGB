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

    config = GetConfigBlob();
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
    unsigned char buf[sizeof(XAPRequestHeader) + 1];
    buf[0] = 0;

    XAPRequestHeader header;
    header.token = GenerateToken();
    header.payload_length = 2;
    header.route = route;
    header.sub_route = sub_route;

    memcpy(&buf[1], &header, sizeof(header));

    LOG_TRACE("[QMK XAP] Requesting 0x%02x 0x%02x with token %04X", route, sub_route, header.token);
    int res = hid_write(dev, buf, sizeof(XAPRequestHeader));

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
    for (int j = 0; j < XAP_MAX_RETRIES; j++) {
        std::memset(&header, 0, sizeof(header));
        LOG_TRACE("[QMK XAP] Receiving response...");
        resp = hid_read_timeout(dev, buf, XAP_MAX_PACKET_SIZE, XAP_TIMEOUT);
        LOG_TRACE("[QMK XAP] hid_read_timeout returned %d", resp);

        if (resp < 0)
        {
            LOG_TRACE("[QMK XAP] Error reading from device: %ls", hid_error(dev));
            return -1;
        } 

        std::stringstream log;
        log << "[QMK XAP] Data received:\n\t";
        for (int i = 0; i < resp; i++) {
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
            LOG_DEBUG("[QMK XAP] Received unsuccessful response with token 0x%04X", header.token);
            return -1;
        }
    }
    LOG_DEBUG("[QMK XAP] Max retries exceeded waiting for response with token 0x%04X", header.token);
    return -1;
}

std::string QMKXAPController::ReceiveString()
{
    unsigned char* data;
    int data_length = ReceiveResponse(&data);

    if (data_length < 0) return "";

    std::string s;
    s.resize(data_length);
    std::memcpy(s.data(), data, data_length);

    delete [] data;
    return s;
}

template<class T>
T QMKXAPController::ReceiveNumber()
{
    LOG_TRACE("[QMK XAP] Receiving number");
    unsigned char* data;

    int data_length = ReceiveResponse(&data);

    if (data_length < (int)sizeof(T))
    {
        if (data_length != -1) 
            delete [] data;
        return 0;
    }
    
    T n;

    std::memcpy(&n, data, sizeof(n));
    LOG_TRACE("[QMK XAP] Received number: %d", n);

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

    uint32_t version = ReceiveNumber<uint32_t>();
    return std::to_string((version >> 24) & 0x000000FF) + "." + std::to_string((version >> 16) & 0x000000FF) + "." + std::to_string(version & 0x0000FFFF);
}

std::string QMKXAPController::GetHWID()
{
    SendRequest(XAP_SUBSYSTEM, 0x08);

    XAPHWID id;
    unsigned char* data;
    int data_length = ReceiveResponse(&data);

    if (data_length < (int)sizeof(XAPHWID))
    {
        if (data_length != -1)
            delete [] data;
        return "";
    }

    std::memcpy(&id, data, sizeof(id));
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
    uint32_t enabled_subsystems = ReceiveNumber<uint32_t>();

    return (NECESSARY_SUBSYSTEMS & enabled_subsystems) == NECESSARY_SUBSYSTEMS;
}

json QMKXAPController::GetConfigBlob()
{
    // Requesting blob length
    SendRequest(QMK_SUBSYSTEM, 0x05);
    uint16_t blob_length = ReceiveNumber<uint16_t>();

    // Config blob data
    unsigned char* buf = new unsigned char[blob_length];
    unsigned char* chunk_buf;

    uint16_t received_length = 0;
    int bytes_read;

    // header + request offset + zero prefix
    int request_length = sizeof(XAPRequestHeader) + sizeof(uint16_t) + 1;
    unsigned char* request_buf = new unsigned char[request_length];

    request_buf[0] = 0;

    XAPRequestHeader *header = (XAPRequestHeader*)&request_buf[1];
    header->payload_length = sizeof(uint16_t);
    header->route = QMK_SUBSYSTEM;
    header->sub_route = 0x06;

    while (received_length < blob_length)
    {
        header->token = GenerateToken();
        memcpy(&request_buf[sizeof(XAPRequestHeader)], &received_length, sizeof(received_length));
        hid_write(dev, request_buf, request_length);

        bytes_read = ReceiveResponse(&chunk_buf);
        if (bytes_read == -1) {
            delete [] buf;
            delete [] request_buf;
            LOG_WARNING("[QMK XAP] Error receiving config blob from keyboard");
            return "{}"_json;
        }
        received_length += bytes_read;
        LOG_TRACE("[QMK XAP] Received %d bytes of config blob, total %u of %u", bytes_read, (unsigned int)received_length, (unsigned int)blob_length);
    }
    delete [] request_buf;

    // Unpacking the received bytes from the gzip blob
    QByteArray received = QByteArray((char*)buf, blob_length);

    received = gUncompress(received);
    if (received.size() == 0) {
        LOG_WARNING("[QMK XAP] Error decompressing config blob");
        return "{}"_json;
    }

    json parsed_data = json::parse(received.begin(), received.end(), nullptr, false);

    if (parsed_data.is_discarded()) {
        LOG_WARNING("[QMK XAP] Error parsing json data");
        return "{}"_json;
    }
    return parsed_data;
}

QByteArray QMKXAPController::gUncompress(const QByteArray &data)
{
    if (data.size() <= 4)
    {
        qWarning("gUncompress: Input data is truncated");
        return QByteArray();
    }

    QByteArray result;

    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in = (Bytef*)(data.data());

    ret = inflateInit2(&strm, 15 +  32); // gzip decoding
    if (ret != Z_OK)
        return QByteArray();

    // run inflate()
    do
    {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered

        switch (ret)
        {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return QByteArray();
        }

        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    return result;
}
