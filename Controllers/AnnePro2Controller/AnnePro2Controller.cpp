#include "AnnePro2Controller.h"

AnnePro2Controller::AnnePro2Controller(hid_device* dev_handle, const char* path) 
{
    dev = dev_handle;
    location = path;
}

AnnePro2Controller::~AnnePro2Controller() 
{
    hid_close(dev);
}

std::string AnnePro2Controller::GetDeviceLocation() 
{
    return("HID: " + location);
}

std::string AnnePro2Controller::GetSerialString() 
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void AnnePro2Controller::SendDirect(unsigned char frame_count, unsigned char * frame_data) 
{
    //LEDController.service_data = [0, 123, 16, mcu_address];
    //LEDController.static_message = [0, 0, 125];
    //LEDController.command_info = [32, 3, 255];

    const unsigned char real_command_info_length = 3 + 1;
    const unsigned char max_hid_length = 64;

    const unsigned char max_command_length = max_hid_length - 4 - 2 - 3;
    const unsigned char max_message_length = max_command_length - real_command_info_length;
    const unsigned char messages_to_send_amount = frame_count / max_message_length + 1;
    const unsigned char val_1 = frame_count % max_message_length;
    const unsigned char val_2 = (0 == val_1) ? max_message_length : val_1;

    unsigned char hid_command[max_hid_length];

    unsigned char led_data = 0;
    for (unsigned char p = 0; p < messages_to_send_amount; p++) {
        const unsigned char e = (messages_to_send_amount << 4) + p;
        const unsigned char a = ((messages_to_send_amount - 1) == p) ? val_2 + real_command_info_length : max_message_length + real_command_info_length;
        
        // service_data
        hid_command[0] = 0;
        hid_command[1] = 123;
        hid_command[2] = 16;
        hid_command[3] = 65;

        // 
        hid_command[4] = e;
        hid_command[5] = a;

        // static_message
        hid_command[6] = 0;
        hid_command[7] = 0;
        hid_command[8] = 125;

        // command_info
        hid_command[9] = 32;
        hid_command[10] = 3;
        hid_command[11] = 255;
        //
        hid_command[12] = 2;

        // led data
        for(uint8_t i = 0; i < max_message_length; i++){
            hid_command[13 + i] = frame_data[led_data];
            led_data++;
        }

        hid_write(dev, hid_command, max_hid_length);

        // Needed due to Anne Pro 2 ignoring commands when they're sent faster than 50ms apart from each other.
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(40ms);
    }
}
