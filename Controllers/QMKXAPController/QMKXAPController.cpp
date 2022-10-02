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

    LoadConfigBlob();
}

QMKXAPController::~QMKXAPController()
{
    hid_close(dev);
}

xap_token_t QMKXAPController::GenerateToken()
{
    xap_token_t n = rng();
    last_token = n;
    return n;
}

void QMKXAPController::SendRequest(subsystem_route_t route, xap_id_t sub_route)
{
    std::vector<unsigned char> buf(sizeof(XAPRequestHeader));

    XAPRequestHeader header;
    header.token = GenerateToken();
    header.payload_length = 2;
    header.route = route;
    header.sub_route = sub_route;

    memcpy(buf.data(), &header, sizeof(header));

    buf.insert(buf.begin(), 0);

    LOG_TRACE("[QMK XAP] Requesting 0x%02x 0x%02x with token %04X", route, sub_route, header.token);
    int res = hid_write(dev, buf.data(), buf.size());

    if (res < 0) LOG_DEBUG("[QMK XAP] Error writing to device: %ls", hid_error(dev));
}

void QMKXAPController::SendRequest(subsystem_route_t route, xap_id_t sub_route, std::vector<unsigned char> payload)
{
    std::vector<unsigned char> buf(sizeof(XAPRequestHeader));

    XAPRequestHeader header;
    header.token = GenerateToken();
    header.payload_length = 2 + payload.size();
    header.route = route;
    header.sub_route = sub_route;

    memcpy(buf.data(), &header, sizeof(header));
    buf.insert(buf.end(), payload.begin(), payload.end());

    buf.insert(buf.begin(), 0);

    LOG_TRACE("[QMK XAP] Requesting 0x%02x 0x%02x with token %04X", route, sub_route, header.token);
    int res = hid_write(dev, buf.data(), buf.size());

    if (res < 0) LOG_DEBUG("[QMK XAP] Error writing to device: %ls", hid_error(dev));
}

XAPResponsePacket QMKXAPController::ReceiveResponse(int response_length)
{
    XAPResponseHeader header;
    XAPResponsePacket pkt;
    pkt.success = false;
    unsigned char buf[XAP_MAX_PACKET_SIZE];
    int resp;
    int j;

    // This will retry reading a response if the tokens don't match because
    // there could be extra responses from broadcasts or other XAP clients' requests.
    for (j = 0; j < XAP_MAX_RETRIES; j++) {
        std::memset(&header, 0, sizeof(header));
        resp = hid_read_timeout(dev, buf, response_length + sizeof(header), XAP_TIMEOUT);
        LOG_TRACE("[QMK XAP] hid_read_timeout returned %d", resp);

        if (resp < 0)
        {
            LOG_TRACE("[QMK XAP] Error reading from device: %ls", hid_error(dev));
            break;
        }

        std::memcpy(&header, buf, sizeof(XAPResponseHeader));

        LOG_TRACE("[QMK XAP] Received header:\n\ttoken: 0x%04X\n\tresponse_flags: 0x%X\n\tpayload_length: %d", header.token, header.flags, header.payload_length);

        std::string payload_string = "\tpayload_data(hex):";
        for (int i = 2; i < header.payload_length + 2; i++)
        {
            if ((i - 2 ) % 16 == 0)
            {
                payload_string.append("\n\t  ");
            }
            // string.append() does not process substitution so snprintf is used before adding to the payload_string.
            char hex_temp[6];
            snprintf(hex_temp, sizeof(hex_temp),"%02X ", buf[i]);
            payload_string.append(hex_temp);
        }
        LOG_TRACE(payload_string.c_str());

        if (header.token != last_token)
        {
            LOG_DEBUG("[QMK XAP] Received token %04X doesn't match last sent token %04X.  Retrying...", header.token, last_token);
            continue;
        }
        else
        {
            pkt.payload.resize(header.payload_length);
            std::memcpy(pkt.payload.data(), buf + sizeof(XAPResponseHeader), header.payload_length);
            pkt.success = header.flags & XAP_RESPONSE_SUCCESS;
            return pkt;
        }
    }
    if (j == XAP_MAX_RETRIES)
        LOG_DEBUG("[QMK XAP] Max retries exceeded waiting for response with token 0x%04X", header.token);
    return pkt;
}

std::string QMKXAPController::ReceiveString()
{
    XAPResponsePacket pkt = ReceiveResponse();
    if (!pkt.success) return "";

    std::string s(pkt.payload.begin() + 1, pkt.payload.end() - 1);
    return s;
}

template<class T>
T QMKXAPController::ReceiveNumber()
{
    XAPResponsePacket pkt = ReceiveResponse(sizeof(T));

    if (!pkt.success) return 0;

    T n;

    std::memcpy(&n, pkt.payload.data(), pkt.payload.size());
    LOG_TRACE("[QMK XAP] Received number: %d", n);

    return n;
}

std::string QMKXAPController::GetName()
{
    if (!config["keyboard_name"].is_null())
        return config["keyboard_name"];
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
    SendRequest(QMK_SUBSYSTEM, 0x08);

    XAPHWID id;
    XAPResponsePacket pkt = ReceiveResponse();

    if (!pkt.success) return "";

    std::memcpy(&id, pkt.payload.data(), sizeof(id));

    return std::to_string(id.id[0]) + std::to_string(id.id[1]) + std::to_string(id.id[2]) + std::to_string(id.id[3]);
}

std::string QMKXAPController::GetLocation()
{
    return location;
}

bool QMKXAPController::CheckKeyboard()
{
    SendRequest(XAP_SUBSYSTEM, 0x02);
    uint32_t enabled_subsystems = ReceiveNumber<uint32_t>();
    bool subsystems_ok = (NECESSARY_SUBSYSTEMS & enabled_subsystems) == NECESSARY_SUBSYSTEMS;

    bool rgb_matrix_enabled = (!config["features"]["rgb_matrix"].is_null()) && config["features"]["rgb_matrix"];

    return subsystems_ok && rgb_matrix_enabled;
}

void QMKXAPController::LoadConfigBlob()
{
    // Requesting blob length
    SendRequest(QMK_SUBSYSTEM, 0x05);
    uint16_t blob_length = ReceiveNumber<uint16_t>();

    // Config blob data
    std::vector<unsigned char> blob_buf;
    uint16_t received_length = 0;

    while (blob_buf.size() < blob_length)
    {
        std::vector<unsigned char> request_payload(2);
        std::memcpy(request_payload.data(), &received_length, sizeof(received_length));
        SendRequest(QMK_SUBSYSTEM, 0x06, request_payload);

        XAPResponsePacket pkt = ReceiveResponse();
        if (!pkt.success) {
            LOG_DEBUG("[QMK XAP] Error receiving config blob from keyboard");
            return;
        }

        blob_buf.insert(
            blob_buf.end(),
            pkt.payload.begin(),
            (pkt.payload.size() < blob_length - blob_buf.size()) ? pkt.payload.end() : pkt.payload.begin() + blob_length - blob_buf.size());
        received_length = blob_buf.size();
        LOG_TRACE("[QMK XAP] Received %d bytes of config blob, total %u of %u", pkt.payload.size(), (unsigned int)received_length, (unsigned int)blob_length);
    }

    // Unpacking the received bytes from the gzip blob

    std::vector<unsigned char> received = gUncompress(blob_buf);
    if (received.size() == 0)
    {
        LOG_DEBUG("[QMK XAP] Error decompressing config blob");
        return;
    }

    std::string s(received.begin(), received.end());

    json parsed_data = json::parse(received.begin(), received.end(), nullptr, false);

    if (parsed_data.is_discarded())
    {
        LOG_DEBUG("[QMK XAP] Error parsing json data");
        return;
    }

    LOG_TRACE("[QMK XAP] JSON Data:\n%s", parsed_data.dump().c_str());
    config = parsed_data;
}

// Source: https://stackoverflow.com/a/7351507/6226488
std::vector<unsigned char> QMKXAPController::gUncompress(const std::vector<unsigned char> &data)
{
    if (data.size() <= 4)
    {
        LOG_DEBUG("[QMK XAP] gUncompress: Input data is truncated");
        return std::vector<unsigned char>();
    }

    std::vector<unsigned char> result;

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
        return std::vector<unsigned char>();

    // run inflate()
    do
    {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  // state not clobbered

        switch (ret)
        {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return std::vector<unsigned char>();
        }

        result.insert(result.end(), out, out + CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    return result;
}

std::vector<std::vector<uint16_t>> QMKXAPController::GetKeycodeMap()
{
    int height = config["matrix_size"]["rows"];
    int width = config["matrix_size"]["cols"];

    std::vector<std::vector<bool>> mask(height, std::vector<bool> (width, false));

    if (!config["layouts"].empty()) {
        json layout = config["layouts"].begin().value()["layout"];

        for (json key : layout) {
            int x = key["matrix"][0];
            int y = key["matrix"][1];
            mask[x][y] = true;
        }
    }

    VectorMatrix<uint16_t> keycodes(height, std::vector<uint16_t>());

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (mask[i][j])
                keycodes[i].push_back(GetKeycode(0, i, j));
        }
    }

    std::stringstream log;
    log << "[QMK XAP] Key position mask from layout in JSON:\n\t";
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            log << std::setw(3) << mask[i][j] << " ";
        }
        log << "\n\t";
    }
    LOG_TRACE(&log.str()[0]);

    std::stringstream log2;
    log << "[QMK XAP] Keycodes requested for each position:\n\t";
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            log2 << std::setw(3) << keycodes[i][j] << " ";
        }
        log2 << "\n\t";
    }
    LOG_TRACE(&log2.str()[0]);

    return keycodes;
}

std::vector<XAPLED> QMKXAPController::GetLEDs()
{
    XAPLED led;
    std::vector<XAPLED> leds;

    for (json xap_led : config["rgb_matrix"]["layout"])
    {
        led.x = xap_led["x"];
        led.y = xap_led["y"];
        led.flags = xap_led["flags"];
        if (!xap_led["matrix"].is_null())
        {
            led.matrix_y = xap_led["matrix"][0];
            led.matrix_x = xap_led["matrix"][1];
        }
        else
        {
            led.matrix_y = -1;
            led.matrix_x = -1;
        }
        leds.push_back(led);
    }

    return leds;
}

uint16_t QMKXAPController::GetKeycode(uint8_t layer, uint8_t row, uint8_t column)
{
    std::vector<unsigned char> payload = { layer, row, column };

    SendRequest(KEYMAP_SUBSYSTEM, 0x03, payload);
    return ReceiveNumber<uint16_t>();
}
