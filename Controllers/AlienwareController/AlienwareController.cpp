/*-----------------------------------------*\
|  AlienwareController.cpp                  |
|                                           |
|  Driver for Alienware lighting controller |
|                                           |
|  Gabriel Marcano (gemarcano) 4/21/2021    |
\*-----------------------------------------*/

#include "RGBController.h"
#include "AlienwareController.h"
#include <cstring>
#include <cstdint>
#include <map>
#include <thread>
#include <chrono>
#include <algorithm>
#include <sstream>

typedef uint32_t alienware_platform_id;

// Some devices appear to report the wrong number of zones. Record that here.
static const std::map<alienware_platform_id, uint8_t> zone_quirks = {
    {0x0c01, 4} // Dell G5 SE 5505
};

// Add zones for devices here, mapping the platform ID to the zone names
static const std::map<alienware_platform_id, std::vector<const char*>> zone_names =
{
    {0x0c01, {"Left", "Middle", "Right", "Numpad"}}
};

template<class Data>
static void SendHIDReport(
    hid_device *dev, const Data& usb_buf, size_t usb_buf_size)
{
    using namespace std::chrono_literals;
    hid_send_feature_report(dev, usb_buf, usb_buf_size);
    // The controller really doesn't like really spammed by too many commands
    // at once... the delay may be command dependent also
    // Delay for longer if the command is changing animation state
    auto command = usb_buf[2];
    auto subcommand = usb_buf[3];
    if (command == ALIENWARE_COMMAND_USER_ANIM &&
            (subcommand == ALIENWARE_COMMAND_USER_ANIM_FINISH_PLAY ||
             subcommand == ALIENWARE_COMMAND_USER_ANIM_FINISH_SAVE))
    {
        std::this_thread::sleep_for(1s);
    }
    else
    {
        std::this_thread::sleep_for(60ms);
    }
}

// Helper function, to help cut back on boilerplate code. Only update variable
// if it is different than zone_data.
template<class Data>
static void SetDirtyData(const Data& data, Data& zone_data, bool &dirty)
{
    if (zone_data != data)
    {
        zone_data = data;
        dirty = true;
    }
}

AlienwareController::AlienwareController(
    hid_device* dev_handle, const hid_device_info& info, std::string name)
{
    dev          = dev_handle;
    device_name_ = std::move(name);
    location_    = info.path;
    std::wstring serial_number = info.serial_number;
    serial_number_ = std::string(serial_number.begin(), serial_number.end());

    // Get zone information by checking firmware configuration
    auto data = Report(ALIENWARE_COMMAND_REPORT_CONFIG);
    alienware_platform_id platform_id = data[4] << 8 | data[5];

    // Get fw version
    data = Report(ALIENWARE_COMMAND_REPORT_FIRMWARE);
    std::stringstream ss;
    ss << static_cast<unsigned>(data[4]) << '.' <<
        static_cast<unsigned>(data[5]) << '.' << static_cast<unsigned>(data[6]);
    version_ = ss.str();

    // Check if the device reports the wrong number of zones
    unsigned number_of_zones =
        zone_quirks.count(platform_id) ? zone_quirks.at(platform_id) : data[6];
    zones_.resize(number_of_zones);

    if (zone_names.count(platform_id))
        zone_names_ = zone_names.at(platform_id);
    else
    {
        // If this is an unknown controller, set the name of all regions to unknown
        for (size_t i = 0; i < number_of_zones; ++i)
    {
            zone_names_.emplace_back("Unknown");
    }
    }

    // Set defaults for all zones
    // It doesn't seem possible to read the controller's current state,
    // hence the default value being set here.
    for (auto& zone : zones_)
    {
        // Set the default color to a light-ish blue, used as a default by ACC
        zone.color = {0x00F0F0, 0x0};
        zone.mode = ALIENWARE_MODE_COLOR;
        // Default period value from ACC
        zone.period = 2000;
        zone.tempo = ALIENWARE_TEMPO_MAX;
        zone.dim = 0;
    }

    dirty_ = true;
    dirty_dim_ = true;
}

unsigned int AlienwareController::GetZoneCount()
{
    return(zones_.size());
}

std::vector<const char*> AlienwareController::GetZoneNames()
{
    return zone_names_;
}

std::string AlienwareController::GetDeviceLocation()
{
    return("HID: " + location_);
}

std::string AlienwareController::GetDeviceName()
{
    return(device_name_);
}

std::string AlienwareController::GetSerialString()
{
    return(serial_number_);
}

std::string AlienwareController::GetFirmwareVersion()
{
    return(version_);
}

AlienwareController::HidapiAlienwareReport AlienwareController::GetResponse()
{
    // Zero init. This is not updated if there's a problem.
    HidapiAlienwareReport result{};
    hid_get_feature_report(dev, result.data(), result.size());
    return result;
}

AlienwareController::HidapiAlienwareReport AlienwareController::Report(
    uint8_t subcommand)
{
    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_REPORT;
    usb_buf[0x03] = subcommand;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    return GetResponse();
}

AlienwareReport AlienwareController::GetStatus(uint8_t subcommand)
{
    auto data = Report(subcommand);
    auto result = AlienwareReport{};
    static_assert(data.size() == HIDAPI_ALIENWARE_REPORT_SIZE &&
        result.size() == (data.size() - 1), "unexpected sizes");
    // Skip first byte, as that's the report number, which should be 0
    std::copy(data.begin() + 1, data.end(), result.begin());
    return result;
}

bool AlienwareController::Dim(
    std::vector<uint8_t> zones, double percent)
{
    // Bail out if there are no zones to update
    if (!zones.size())
        return true;

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_DIM;
    usb_buf[0x03] = static_cast<uint8_t>(percent * 0x64);
    uint16_t num_zones = zones.size();
    usb_buf[0x04] = num_zones >> 8;;
    usb_buf[0x05] = num_zones & 0xFF;
    for (size_t i = 0; i < num_zones; ++i)
    {
        usb_buf[0x06+i] = zones[i];
    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    auto response = GetResponse();
    static_assert(response.size() == sizeof(usb_buf), "Mismatched size");
    // For this command, error is if the output equals the input
    return response[1] == 0x03 && memcmp(usb_buf, response.data(), response.size());
}

bool AlienwareController::UserAnimation(
    uint16_t subcommand, uint16_t animation, uint16_t duration)
{
    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00 per hidapi      |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_USER_ANIM;
    usb_buf[0x03] = subcommand >> 8;
    usb_buf[0x04] = subcommand & 0xFF;
    usb_buf[0x05] = animation >> 8;
    usb_buf[0x06] = animation & 0xFF;
    usb_buf[0x07] = duration >> 8;
    usb_buf[0x08] = duration & 0xFF;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    // Every subcommand appears to report its result on a different byte
    auto response = GetResponse();

    // The only time the 0x03 byte is zero is if the controller has crashed
    if (response[1] == 0)
        return false;

    switch (subcommand)
    {
    case ALIENWARE_COMMAND_USER_ANIM_FINISH_SAVE:
        return !response[7];
    case ALIENWARE_COMMAND_USER_ANIM_FINISH_PLAY:
        return !response[5];
    case ALIENWARE_COMMAND_USER_ANIM_PLAY:
        return !response[7];
    default:
        return true;
    }
}

bool AlienwareController::SelectZones(
    const std::vector<uint8_t>& zones)
{
    // Bail if zones is empty, and return false to indicate nothing has changed
    if (!zones.size())
        return false;

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_SELECT_ZONES;
    usb_buf[0x03] = 1; // loop?
    uint16_t num_zones = zones.size();
    usb_buf[0x04] = num_zones >> 8;
    usb_buf[0x05] = num_zones & 0xFF;
    for (size_t i = 0; i < num_zones; ++i)
    {
        usb_buf[0x06+i] = zones[i];
    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    auto response = GetResponse();
    static_assert(response.size() == sizeof(usb_buf), "Mismatched size");
    // For this command, error is if the output equals the input
    return response[1] == 0x03 && memcmp(usb_buf, response.data(), response.size());
}

bool AlienwareController::ModeAction(
    uint8_t mode, uint16_t duration, uint16_t tempo, RGBColor color)
{
    return ModeAction(&mode, &duration, &tempo, &color, 1);
}

bool AlienwareController::ModeAction(
    const uint8_t *mode,
    const uint16_t *duration,
    const uint16_t *tempo,
    const RGBColor *color,
    unsigned amount)
{
    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    // Amount must be 3 or less, as that's how many subcommands can fit into
    // one report
    if (amount > 3)
        return false;

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_ADD_ACTION;

    for (unsigned i = 0; i < amount; ++i)
    {
        usb_buf[0x03 + 8*i] = mode[i];
        usb_buf[0x04 + 8*i] = duration[i] >> 8;
        usb_buf[0x05 + 8*i] = duration[i] & 0xFF;
        usb_buf[0x06 + 8*i] = tempo[i] >> 8;
        usb_buf[0x07 + 8*i] = tempo[i] & 0xFF;
        usb_buf[0x08 + 8*i] = RGBGetRValue(color[i]);
        usb_buf[0x09 + 8*i] = RGBGetGValue(color[i]);
        usb_buf[0x0A + 8*i] = RGBGetBValue(color[i]);
    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    auto response = GetResponse();
    static_assert(response.size() == sizeof(usb_buf), "Mismatched size");
    // For this command, error is if the output equals the input
    return response[1] == 0x03 && memcmp(usb_buf, response.data(), response.size());
}

bool AlienwareController::MultiModeAction(
    const uint8_t *mode,
    const uint16_t *duration,
    const uint16_t *tempo,
    const RGBColor *color,
    unsigned amount)
{
    bool result = true;
    unsigned left = amount;
    while (left && result)
    {
        unsigned amount_ = std::min(left, 3u);
        result &= ModeAction(mode, duration, tempo, color, amount_);
        mode += amount_;
        duration += amount_;
        tempo += amount_;
        color += amount_;
        left -= amount_;
    }

    return result;
}

bool AlienwareController::SetColorDirect(
    RGBColor color,
    std::vector<uint8_t> zones)
{
    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    // Bail if zones is empty
    if (zones.empty())
        return true;

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_SET_COLOR;

    usb_buf[0x03] = RGBGetRValue(color);
    usb_buf[0x04] = RGBGetGValue(color);
    usb_buf[0x05] = RGBGetBValue(color);
    uint16_t num_zones = zones.size();
    usb_buf[0x06] = num_zones >> 8;
    usb_buf[0x07] = num_zones & 0xFF;
    for (size_t i = 0; i < num_zones; ++i)
    {
        usb_buf[0x08+i] = zones[i];
    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    auto response = GetResponse();
    static_assert(response.size() == sizeof(usb_buf), "Mismatched size");
    // For this command, error is if the output equals the input
    return response[1] == 0x03 && memcmp(usb_buf, response.data(), response.size());
}

bool AlienwareController::Reset()
{
    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    unsigned char usb_buf[HIDAPI_ALIENWARE_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    // Bail if zones is empty
    if (zones_.empty())
        return true;

    /*-----------------------------------------------------*\
    | Set up message packet with leading 00, per hidapi     |
    \*-----------------------------------------------------*/
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = 0x03;
    usb_buf[0x02] = ALIENWARE_COMMAND_RESET;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    SendHIDReport(dev, usb_buf, sizeof(usb_buf));

    auto response = GetResponse();
    static_assert(response.size() == sizeof(usb_buf), "Mismatched size");
    // For this command, error is if the output equals the input
    return response[1] == 0x03;
}

void AlienwareController::SetMode(uint8_t zone, uint8_t mode)
{
    SetDirtyData(mode, zones_[zone].mode, dirty_);
}

void AlienwareController::SetColor(uint8_t zone, RGBColor color)
{
    SetColor(zone, color, zones_[zone].color[1]);
}

void AlienwareController::SetColor(uint8_t zone, RGBColor color1, RGBColor color2)
{
    SetDirtyData(color1, zones_[zone].color[0], dirty_);
    SetDirtyData(color2, zones_[zone].color[1], dirty_);
}

void AlienwareController::SetPeriod(uint8_t zone, uint16_t period)
{
    SetDirtyData(period, zones_[zone].period, dirty_);
}

void AlienwareController::SetTempo(uint8_t zone, uint16_t tempo)
{
    SetDirtyData(tempo, zones_[zone].tempo, dirty_);
}

void AlienwareController::SetDim(uint8_t zone, uint8_t dim)
{
    // Clamp dim to values between 0 and 100
    dim = dim > 100 ? 100 : dim;
    SetDirtyData(dim, zones_[zone].dim, dirty_dim_);
}

void AlienwareController::UpdateDim()
{
    if (!dirty_dim_)
        return;

    // Collect all zones that share dim settings, and update them together
    std::map<uint8_t, std::vector<uint8_t>> dim_zone_map;
    for (size_t i = 0; i < zones_.size(); i++)
    {
        dim_zone_map[zones_[i].dim].emplace_back(i);
    }
    for (auto &pair : dim_zone_map)
    {
        // Bail on an error...
        if (!Dim(pair.second, pair.first))
            return;
    }
    dirty_dim_ = false;
}

bool AlienwareController::UpdateDirect()
{
    // Collect all zones that share dim settings, and update them together
    std::map<RGBColor, std::vector<uint8_t>> color_zone_map;
    for (size_t i = 0; i < zones_.size(); i++)
    {
        color_zone_map[zones_[i].color[0]].emplace_back(i);
    }
    for (auto &pair : color_zone_map)
    {
        // Bail on an error...
        if (!SetColorDirect(pair.first, pair.second))
            return false;
    }
    return true;
}

static const RGBColor rainbow_colors[4][7] =
{
    { 0xFF0000, 0xFFA500, 0xFFFF00, 0x008000, 0x00BFFF, 0x0000FF, 0x800080 },
    { 0x800080, 0xFF0000, 0xFFA500, 0xFFFF00, 0x008000, 0x00BFFF, 0x0000FF },
    { 0x0000FF, 0x800080, 0xFF0000, 0xFFA500, 0xFFFF00, 0x008000, 0x00BFFF },
    { 0x00BFFF, 0x0000FF, 0x800080, 0xFF0000, 0xFFA500, 0xFFFF00, 0x008000 }
};

void AlienwareController::UpdateMode()
{
    // If there are no updates, don't bother running this
    if (!dirty_)
        return;

    bool result = UserAnimation(
        ALIENWARE_COMMAND_USER_ANIM_NEW,
        ALIENWARE_COMMAND_USER_ANIM_KEYBOARD,
        0
    );
    if (!result)
        return;

    for(std::size_t zone_idx = 0; zone_idx < zones_.size(); zone_idx++)
    {
        auto& zone = zones_[zone_idx];
        result = SelectZones({static_cast<uint8_t>(zone_idx)});
        if (!result)
            return;

        // Some modes use 0x07d0 for their duration as sent by AWCC traces,
        // maybe 2000ms?
        switch (zone.mode)
        {
        case ALIENWARE_MODE_COLOR:
            result = ModeAction(
                zone.mode, 2000, ALIENWARE_TEMPO_MAX, zone.color[0]);
            break;
        case ALIENWARE_MODE_PULSE:
            result = ModeAction(
                zone.mode, zone.period, zone.tempo, zone.color[0]);
            break;
        case ALIENWARE_MODE_MORPH:
            {
                uint8_t zones[2] = { zone.mode, zone.mode };
                uint16_t periods[2] = { zone.period, zone.period };
                uint16_t tempos[2] = { zone.tempo, zone.tempo };
                RGBColor colors[2] = { zone.color[0], zone.color[1] };
                result = MultiModeAction(zones, periods, tempos, colors, 2);
            }
            break;
        case ALIENWARE_MODE_SPECTRUM:
            {
                uint8_t zones[7] = {
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH};
                uint16_t periods[7] = {
                    zone.period, zone.period, zone.period, zone.period,
                    zone.period, zone.period, zone.period };
                uint16_t tempos[7] = {
                    zone.tempo, zone.tempo, zone.tempo, zone.tempo, zone.tempo,
                    zone.tempo, zone.tempo };
                result = MultiModeAction(zones, periods, tempos, rainbow_colors[0], 7);
            }
            break;
        case ALIENWARE_MODE_RAINBOW:
            {
                uint8_t zones[7] = {
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH,
                    ALIENWARE_MODE_MORPH};
                uint16_t periods[7] = {
                    zone.period, zone.period, zone.period, zone.period,
                    zone.period, zone.period, zone.period };
                uint16_t tempos[7] = {
                    zone.tempo, zone.tempo, zone.tempo, zone.tempo, zone.tempo,
                    zone.tempo, zone.tempo };
                result = MultiModeAction(zones, periods, tempos, rainbow_colors[zone_idx], 7);
            }
            break;
        case ALIENWARE_MODE_BREATHING:
            {
                uint8_t zones[2] = { ALIENWARE_MODE_MORPH, ALIENWARE_MODE_MORPH };
                uint16_t periods[2] = { zone.period, zone.period };
                uint16_t tempos[2] = { zone.tempo, zone.tempo };
                RGBColor colors[2] = { zone.color[0], 0x0 };
                result = MultiModeAction(zones, periods, tempos, colors, 2);
            }
            break;

        default:
            result = false;
        }
        if (!result)
            return;
    }
    result = UserAnimation(
        ALIENWARE_COMMAND_USER_ANIM_FINISH_PLAY,
        ALIENWARE_COMMAND_USER_ANIM_KEYBOARD,
        0
    );

    // Dont update dirty_ flag if there's an error
    if (!result)
        return;
    dirty_ = false;
}

void AlienwareController::UpdateController()
{
    UpdateMode();
    UpdateDim();
}
