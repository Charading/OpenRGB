/*-------------------------------------------------------------------*\
|  ThermaltakeRiingQuadController.cpp                                 |
|                                                                     |
|  Driver for Thermaltake Riing Quad Controller                       |
|                                                                     |
|  Chris M (Dr_No)          15th Feb 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "ThermaltakeRiingQuadController.h"
#include <cstring>

ThermaltakeRiingQuadController::ThermaltakeRiingQuadController(hid_device* dev_handle, const char* path)
{
    wchar_t tmpName[HID_MAX_STR];

    dev         = dev_handle;
    location    = path;

    hid_get_manufacturer_string(dev, tmpName, HID_MAX_STR);
    std::wstring wName = std::wstring(tmpName);
    device_name = std::string(wName.begin(), wName.end());

    hid_get_product_string(dev, tmpName, HID_MAX_STR);
    wName = std::wstring(tmpName);
    device_name.append(" ").append(std::string(wName.begin(), wName.end()));

    hid_get_serial_number_string(dev, tmpName, HID_MAX_STR);
    wName = std::wstring(tmpName);
    serial = std::string(wName.begin(), wName.end());

    SendInit();

    /*-----------------------------------------------------*\
    | The Riing Quad only seems to run in direct mode and   |
    | requires a packet within seconds to remain in the     |
    | set mode (similar to Corsair Node Pro. Start a thread |
    | to send a packet every TT_QUAD_KEEPALIVE seconds      |
    \*-----------------------------------------------------*/
    memset(tt_quad_buffer, 0x00, sizeof(tt_quad_buffer));       //zero out entire buffer
    keepalive_thread_run = 1;
    keepalive_thread = new std::thread(&ThermaltakeRiingQuadController::KeepaliveThread, this);
}

ThermaltakeRiingQuadController::~ThermaltakeRiingQuadController()
{
    keepalive_thread_run = 0;
    keepalive_thread->join();
    delete keepalive_thread;

    hid_close(dev);
}

void ThermaltakeRiingQuadController::KeepaliveThread()
{
    while(keepalive_thread_run.load())
    {
        if((std::chrono::steady_clock::now() - last_commit_time) > std::chrono::seconds(TT_QUAD_KEEPALIVE))
        {
            SendBuffer();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string ThermaltakeRiingQuadController::GetDeviceName()
{
    return device_name;
}

std::string ThermaltakeRiingQuadController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string ThermaltakeRiingQuadController::GetSerial()
{
    return(serial);
}

void ThermaltakeRiingQuadController::SetChannelLEDs(uint8_t zone, RGBColor * colors, unsigned int num_colors)
{
    unsigned char* color_data = new unsigned char[3 * num_colors];

    //This for loop seems to be the bottleneck in the controller and potentially
    //would allow for higher FPS if the struct could be optimised for speed
    for(std::size_t color = 0; color < num_colors; color++)
    {
        //int color_idx = color * 3;
        //color_data[color_idx + 0] = RGBGetGValue(colors[color]);    //Green First
        //color_data[color_idx + 1] = RGBGetRValue(colors[color]);
        //color_data[color_idx + 2] = RGBGetBValue(colors[color]);
        color_data[color * 3]       = ToGRBRaw(colors[color]);
    }

<<<<<<< HEAD
    PrepareBuffer(zone, num_colors, color_data);

    //delete[] color_data;
=======
    tt_quad_buffer[zone][TT_QUAD_ZONE_BYTE]     = zone + 1;
    tt_quad_buffer[zone][TT_QUAD_MODE_BYTE]     = current_mode + ( current_speed & 0x03 );
    memcpy(&tt_quad_buffer[zone][TT_QUAD_DATA_BYTE], color_data, (num_colors * 3));
    SendBuffer();

    delete[] color_data;
>>>>>>> Final code polish prior to merge
}

void ThermaltakeRiingQuadController::SetMode(unsigned char mode, unsigned char speed)
{
    current_mode        = mode;
    current_speed       = speed;
}

void ThermaltakeRiingQuadController::SendInit()
{
    unsigned char buffer[TT_QUAD_PACKET_SIZE]   = { 0x00, 0xFE, 0x33};
    int buffer_size                             = (sizeof(buffer) / sizeof(buffer[0]));

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, TT_QUAD_INTERRUPT_TIMEOUT);
}

<<<<<<< HEAD
void ThermaltakeRiingQuadController::PrepareBuffer(unsigned char zone, unsigned char num_colors, unsigned char* color_data)
{
    unsigned char buffer[TT_QUAD_PACKET_SIZE]   = { 0x00, 0x32, 0x52};  // 0x33, 0x51, Zone ??

    buffer[TT_QUAD_ZONE_BYTE]                   = zone + 1;
    buffer[TT_QUAD_MODE_BYTE]                   = current_mode + ( current_speed & 0x03 );
    //memcpy(&buffer[TT_QUAD_DATA_BYTE], color_data, (num_colors * 3));   //Copy in GRB color data

    //Copy the prepared buffer to the module buffer then send
    //memcpy(&tt_quad_buffer[zone][0], buffer, TT_QUAD_PACKET_SIZE);
    memcpy(&tt_quad_buffer[zone][TT_QUAD_DATA_BYTE], color_data, (num_colors * 3));
    SendBuffer();
}

void ThermaltakeRiingQuadController::SendBuffer()
{
    unsigned char buffer[TT_QUAD_PACKET_SIZE]   = { 0x00 };     //Temporary read buffer

    for(std::size_t zone_index = 0; zone_index < TT_QUAD_ZONES; zone_index++)
    {
        hid_write(dev, tt_quad_buffer[zone_index], TT_QUAD_PACKET_SIZE);
        hid_read_timeout(dev, buffer, TT_QUAD_PACKET_SIZE, TT_QUAD_INTERRUPT_TIMEOUT);
=======
void ThermaltakeRiingQuadController::SendBuffer()
{
    for(std::size_t zone_index = 0; zone_index < TT_QUAD_ZONES; zone_index++)
    {
        hid_write(dev, tt_quad_buffer[zone_index], TT_QUAD_PACKET_SIZE);
>>>>>>> Final code polish prior to merge
    }
    //Update the last commit time
    last_commit_time = std::chrono::steady_clock::now();
}
