/*-----------------------------------------*\
|  RazerController.cpp                      |
|                                           |
|  Driver for Razer devices                 |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/22/2021   |
\*-----------------------------------------*/

#include "RazerController.h"
#include "OpenRazerDevices.h"

#include <string.h>

using namespace std::chrono_literals;

RazerController::RazerController(hid_device* dev_handle, const char* path, unsigned short pid, std::string dev_name)
{
    dev             = dev_handle;
    dev_pid         = pid;
    location        = path;
    name            = dev_name;
    device_index    = 0;

    /*-----------------------------------------------------------------*\
    | Loop through all known devices to look for a name match           |
    \*-----------------------------------------------------------------*/
    for (unsigned int i = 0; i < RAZER_NUM_DEVICES; i++)
    {
        if (device_list[i]->name == name)
        {
            /*---------------------------------------------------------*\
            | Set device ID                                             |
            \*---------------------------------------------------------*/
            device_index = i;
        }
    }

    /*-----------------------------------------------------------------*\
    | Set report index                                                  |
    \*-----------------------------------------------------------------*/
    report_index    = 1;
    response_index  = 1;

    /*-----------------------------------------------------------------*\
    | Initialize to full brightness                                     |
    \*-----------------------------------------------------------------*/
    razer_set_brightness(255);
}

RazerController::~RazerController()
{

}

std::string RazerController::GetName()
{
    return(name);
}

unsigned int RazerController::GetDeviceIndex()
{
    return(device_index);
}

device_type RazerController::GetDeviceType()
{
    return(device_list[device_index]->type);
}

std::string RazerController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string RazerController::GetFirmwareString()
{
    return(razer_get_firmware());
}

std::string RazerController::GetSerialString()
{
    return(razer_get_serial());
}

void RazerController::SetLEDs(RGBColor* colors)
{
    /*---------------------------------------------------------*\
    | Get the matrix layout information from the device list    |
    \*---------------------------------------------------------*/
    unsigned int matrix_cols = device_list[device_index]->cols;
    unsigned int matrix_rows = device_list[device_index]->rows;

    /*---------------------------------------------------------*\
    | Create an output array large enough to hold RGB data for  |
    | a single row                                              |
    \*---------------------------------------------------------*/
    unsigned char* output_array = new unsigned char[matrix_cols * 3];

    /*---------------------------------------------------------*\
    | Send one row of the custom frame at a time                |
    \*---------------------------------------------------------*/
    for (unsigned int row = 0; row < matrix_rows; row++)
    {
        unsigned int row_offset = (row * matrix_cols);

        /*-----------------------------------------------------*\
        | Fill the output array with RGB data                   |
        \*-----------------------------------------------------*/
        for(unsigned int col = 0; col < matrix_cols; col++)
        {
            unsigned int color_idx = col + row_offset;
            output_array[(col * 3) + 0] = (char)RGBGetRValue(colors[color_idx]);
            output_array[(col * 3) + 1] = (char)RGBGetGValue(colors[color_idx]);
            output_array[(col * 3) + 2] = (char)RGBGetBValue(colors[color_idx]);
        }

        /*-----------------------------------------------------*\
        | Send the output array to the device                   |
        \*-----------------------------------------------------*/
        std::this_thread::sleep_for(1ms);

        razer_set_custom_frame(row, 0, matrix_cols - 1, output_array);
    }

    std::this_thread::sleep_for(1ms);

    /*---------------------------------------------------------*\
    | Set custom mode to apply frame                            |
    \*---------------------------------------------------------*/
    razer_set_mode_custom();

    /*---------------------------------------------------------*\
    | Delete the output array                                   |
    \*---------------------------------------------------------*/
    delete[] output_array;
}

void RazerController::SetModeSpectrumCycle()
{
    razer_set_mode_spectrum_cycle();
}

/*-------------------------------------------------------------------------------------------------*\
| Private packet sending functions.                                                                 |
\*-------------------------------------------------------------------------------------------------*/

unsigned char RazerController::razer_calculate_crc(razer_report* report)
{
    /*---------------------------------------------------------*\
    | The second to last byte of report is a simple checksum    |
    | Just xor all bytes up with overflow and you are done      |
    \*---------------------------------------------------------*/
    unsigned char   crc         = 0;
    unsigned char*  report_ptr  = (unsigned char*)report;

    /*---------------------------------------------------------*\
    | The start and end checks here have been modified compared |
    | to the original OpenRazer version.  This is due to adding |
    | the report ID field to the razer_report structure for     |
    | compatibility with HIDAPI.                                |
    \*---------------------------------------------------------*/
    for(unsigned int i = 3; i < 89; i++)
    {
        crc ^= report_ptr[i];
    }

    return crc;
}

/*---------------------------------------------------------------------------------*\
| Basic report and response creation functions                                      |
\*---------------------------------------------------------------------------------*/

razer_report RazerController::razer_create_report(unsigned char command_class, unsigned char command_id, unsigned char data_size)
{
    razer_report new_report;

    /*---------------------------------------------------------*\
    | Zero out the new report                                   |
    \*---------------------------------------------------------*/
    memset(&new_report, 0, sizeof(razer_report));

    /*---------------------------------------------------------*\
    | Fill in the new report with the given parameters          |
    \*---------------------------------------------------------*/
    new_report.report_id            = report_index;
    new_report.status               = 0x00;
    new_report.transaction_id.id    = 0xFF;
    new_report.remaining_packets    = 0x00;
    new_report.protocol_type        = 0x00;
    new_report.command_class        = command_class;
    new_report.command_id.id        = command_id;
    new_report.data_size            = data_size;

    return new_report;
}

razer_report RazerController::razer_create_response()
{
    razer_report new_report;

    /*---------------------------------------------------------*\
    | Zero out the new report                                   |
    \*---------------------------------------------------------*/
    memset(&new_report, 0, sizeof(razer_report));

    /*---------------------------------------------------------*\
    | Fill in the new report with the given parameters          |
    \*---------------------------------------------------------*/
    new_report.report_id            = response_index;
    new_report.status               = 0x00;
    new_report.transaction_id.id    = 0x00;
    new_report.remaining_packets    = 0x00;
    new_report.protocol_type        = 0x00;
    new_report.command_class        = 0x00;
    new_report.command_id.id        = 0x00;
    new_report.data_size            = 0x00;

    return new_report;
}

/*---------------------------------------------------------------------------------*\
| Command report creation functions                                                 |
\*---------------------------------------------------------------------------------*/

razer_report RazerController::razer_create_brightness_extended_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char brightness)
{
    razer_report report         = razer_create_report(0x0F, 0x04, 0x03);

    report.transaction_id.id    = 0x3F;
    report.arguments[0]         = variable_storage;
    report.arguments[1]         = led_id;
    report.arguments[2]         = brightness;

    return report;
}

razer_report RazerController::razer_create_brightness_standard_report(unsigned char variable_storage, unsigned char led_id, unsigned char brightness)
{
    razer_report report         = razer_create_report(0x03, 0x03, 0x03);

    report.arguments[0]         = variable_storage;
    report.arguments[1]         = led_id;
    report.arguments[2]         = brightness;

    return report;
}

razer_report RazerController::razer_create_custom_frame_linear_report(unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data)
{
    razer_report report         = razer_create_report(0x03, 0x0C, 0x32);
    size_t       row_length     = (size_t) (((stop_col + 1) - start_col) * 3);

    report.arguments[0]         = start_col;
    report.arguments[1]         = stop_col;

    /*---------------------------------------------------------*\
    | Copy in the RGB data                                      |
    \*---------------------------------------------------------*/
    memcpy(&report.arguments[2], rgb_data, row_length);

    return report;
}

razer_report RazerController::razer_create_custom_frame_extended_matrix_report(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data)
{
    const size_t row_length     = (size_t)(((stop_col + 1) - start_col) * 3);
    const size_t packet_length  = row_length + 5;

    razer_report report         = razer_create_report(0x0F, 0x03, packet_length);
    report.transaction_id.id    = 0x3F;

    report.arguments[2]         = row_index;
    report.arguments[3]         = start_col;
    report.arguments[4]         = stop_col;

    /*---------------------------------------------------------*\
    | Copy in the RGB data                                      |
    \*---------------------------------------------------------*/
    memcpy(&report.arguments[5], rgb_data, row_length);

    return report;
}

razer_report RazerController::razer_create_custom_frame_standard_matrix_report(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data)
{
    const size_t row_length     = (size_t)(((stop_col + 1) - start_col) * 3);
    const size_t packet_length  = row_length + 4;

    razer_report report         = razer_create_report(0x03, 0x0B, packet_length);
    report.transaction_id.id    = 0x3F;

    report.arguments[0]         = 0xFF;
    report.arguments[1]         = row_index;
    report.arguments[2]         = start_col;
    report.arguments[3]         = stop_col;

    /*---------------------------------------------------------*\
    | Copy in the RGB data                                      |
    \*---------------------------------------------------------*/
    memcpy(&report.arguments[4], rgb_data, row_length);

    return report;
}

razer_report RazerController::razer_create_mode_custom_extended_matrix_report()
{
    struct razer_report report = razer_create_report(0x0F, 0x02, 0x0C);
    report.transaction_id.id    = 0x3F;

    report.arguments[0]         = 0x00;
    report.arguments[1]         = 0x00;
    report.arguments[2]         = 0x08;

    return report;
}

razer_report RazerController::razer_create_mode_custom_standard_matrix_report(unsigned char variable_storage)
{
    razer_report report         = razer_create_report(0x03, 0x0A, 0x02);

    report.arguments[0]         = 0x05;
    report.arguments[1]         = variable_storage;

    return report;
}

razer_report RazerController::razer_create_mode_spectrum_cycle_extended_matrix_report(unsigned char variable_storage, unsigned char led_id)
{
    razer_report report         = razer_create_report(0x0F, 0x02, 0x06);
    report.transaction_id.id    = 0x3F;

    report.arguments[0]         = variable_storage;
    report.arguments[1]         = led_id;
    report.arguments[2]         = 0x03;

    return report;
}

razer_report RazerController::razer_create_mode_spectrum_cycle_standard_matrix_report(unsigned char variable_storage, unsigned char led_id)
{
    razer_report report         = razer_create_report(0x03, 0x0A, 0x01);
    report.arguments[0]         = 0x04;

    return report;
}

razer_report RazerController::razer_create_set_led_rgb_report(unsigned char variable_storage, unsigned char led_id, unsigned char* rgb_data)
{
    razer_report report         = razer_create_report(0x03, 0x01, 0x05);

    report.arguments[0]         = variable_storage;
    report.arguments[1]         = led_id;
    report.arguments[2]         = rgb_data[0];
    report.arguments[3]         = rgb_data[1];
    report.arguments[4]         = rgb_data[2];

    return report;
}

razer_report RazerController::razer_create_set_led_effect_report(unsigned char variable_storage, unsigned char led_id, unsigned char effect)
{
    razer_report report         = razer_create_report(0x03, 0x02, 0x03);

    report.arguments[0]         = variable_storage;
    report.arguments[1]         = led_id;

    if(effect > 5)
    {
        report.arguments[2]     = 5;
    }
    else
    {
        report.arguments[2]     = effect;
    }

    return report;
}

/*---------------------------------------------------------------------------------*\
| Get functions (request information from device)                                   |
\*---------------------------------------------------------------------------------*/

std::string RazerController::razer_get_firmware()
{
    std::string         firmware_string         = "";
    struct razer_report report                  = razer_create_report(0x00, RAZER_COMMAND_ID_GET_FIRMWARE_VERSION, 0x02);
    struct razer_report response_report         = razer_create_response();

    switch(dev_pid)
    {
        case RAZER_MAMBA_ELITE_PID:
            report.transaction_id.id            = 0x1F;
            break;
    }

    std::this_thread::sleep_for(1ms);
    razer_usb_send(&report);
    std::this_thread::sleep_for(1ms);
    razer_usb_receive(&response_report);

    firmware_string = "v" + std::to_string(response_report.arguments[0]) + "." + std::to_string(response_report.arguments[1]);

    return firmware_string;
}

std::string RazerController::razer_get_serial()
{
    char                serial_string[64]   = "";
    struct razer_report report              = razer_create_report(0x00, RAZER_COMMAND_ID_GET_SERIAL_STRING, 0x16);
    struct razer_report response_report     = razer_create_response();

    std::this_thread::sleep_for(1ms);
    razer_usb_send(&report);
    std::this_thread::sleep_for(1ms);
    razer_usb_receive(&response_report);

    strncpy(&serial_string[0], (const char*)&response_report.arguments[0], 22);
    serial_string[22] = '\0';

    std::string ret_string = serial_string;
    return ret_string;
}

/*---------------------------------------------------------------------------------*\
| Set functions (send information to device)                                        |
\*---------------------------------------------------------------------------------*/

void RazerController::razer_set_brightness(unsigned char brightness)
{
    razer_report report;

    switch(dev_pid)
    {
        /*-------------------------------------------------*\
        | These devices use an extended matrix report, LED  |
        | ID Zero, and transaction ID 0x1F                  |
        \*-------------------------------------------------*/
        case RAZER_TARTARUS_V2_PID:
        case RAZER_MAMBA_ELITE_PID:
            report                      = razer_create_brightness_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_ZERO, brightness);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report and   |
        | LED ID Backlight                                  |
        \*-------------------------------------------------*/
        case RAZER_BLACKWIDOW_LITE_PID:
        case RAZER_ORNATA_CHROMA_PID:
        case RAZER_HUNTSMAN_ELITE_PID:
        case RAZER_HUNTSMAN_TE_PID:
        case RAZER_BLACKWIDOW_2019_PID:
        case RAZER_HUNTSMAN_PID:
        case RAZER_BLACKWIDOW_ESSENTIAL_PID:
        case RAZER_CYNOSA_CHROMA_PID:
        case RAZER_CYNOSA_LITE_PID:
            report                      = razer_create_brightness_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT, brightness);
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report, LED  |
        | ID Backlight, and transaction ID 0x1F             |
        \*-------------------------------------------------*/
        case RAZER_BLACKWIDOW_ELITE_PID:
        case RAZER_CYNOSA_V2_PID:
        case RAZER_ORNATA_CHROMA_V2_PID:
            report                      = razer_create_brightness_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT, brightness);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and LED|
        | ID Backlight                                      |
        \*-------------------------------------------------*/
        default:
            report                      = razer_create_brightness_standard_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT, brightness);
            break;
    }

    razer_usb_send(&report);
}

void RazerController::razer_set_custom_frame(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data)
{
    razer_report report;

    switch(dev_pid)
    {
        /*-------------------------------------------------*\
        | These devices use an extended matrix report       |
        \*-------------------------------------------------*/
        case RAZER_ORNATA_CHROMA_PID:
        case RAZER_HUNTSMAN_ELITE_PID:
        case RAZER_HUNTSMAN_TE_PID:
        case RAZER_BLACKWIDOW_2019_PID:
        case RAZER_HUNTSMAN_PID:
        case RAZER_CYNOSA_CHROMA_PID:
        case RAZER_DEATHADDER_ELITE_PID:
        case RAZER_NAGA_CHROMA_PID:
        case RAZER_LANCEHEAD_WIRED_PID:
        case RAZER_LANCEHEAD_WIRELESS_PID:
        case RAZER_LANCEHEAD_TE_WIRED_PID:
        case RAZER_LANCEHEAD_WIRELESS_RECEIVER_PID:
        case RAZER_LANCEHEAD_WIRELESS_WIRED_PID:
        case RAZER_MAMBA_WIRELESS_RECEIVER_PID:
        case RAZER_MAMBA_WIRELESS_WIRED_PID:
        case RAZER_BASILISK_PID:
        case RAZER_DEATHADDER_V2_PID:
        case RAZER_DEATHADDER_V2_PRO_WIRED_PID:
        case RAZER_DEATHADDER_V2_PRO_WIRELESS_PID:
        case RAZER_DEATHADDER_V2_MINI_PID:
        case RAZER_VIPER_PID:
        case RAZER_VIPER_MINI_PID:
        case RAZER_VIPER_ULTIMATE_WIRED_PID:
        case RAZER_VIPER_ULTIMATE_WIRELESS_PID:
        case RAZER_FIREFLY_HYPERFLUX_PID:
        case RAZER_GOLIATHUS_CHROMA_PID:
        case RAZER_GOLIATHUS_CHROMA_EXTENDED_PID:
        case RAZER_CHROMA_HDK_PID:
        case RAZER_CHROMA_BASE_PID:
        case RAZER_NOMMO_PRO_PID:
        case RAZER_NOMMO_CHROMA_PID:
            report                      = razer_create_custom_frame_extended_matrix_report(row_index, start_col, stop_col, rgb_data);
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report and   |
        | transaction ID 0x1F                               |
        \*-------------------------------------------------*/
        case RAZER_TARTARUS_V2_PID:
        case RAZER_BLACKWIDOW_ELITE_PID:
        case RAZER_CYNOSA_V2_PID:
        case RAZER_ORNATA_CHROMA_V2_PID:
        case RAZER_MAMBA_ELITE_PID:
        case RAZER_KRAKEN_KITTY_EDITION_PID:
        case RAZER_MOUSE_BUNGEE_V3_CHROMA_PID:
        case RAZER_BASE_STATION_V2_CHROMA_PID:
            report                      = razer_create_custom_frame_extended_matrix_report(row_index, start_col, stop_col, rgb_data);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use a linear report                 |
        \*-------------------------------------------------*/
        case RAZER_DEATHSTALKER_CHROMA_PID:
        case RAZER_MAMBA_TE_WIRED_PID:
        case RAZER_DIAMONDBACK_CHROMA_PID:
        case RAZER_FIREFLY_PID:
        case RAZER_CHROMA_MUG_PID:
            report                      = razer_create_custom_frame_linear_report(start_col, stop_col, rgb_data);
            break;

        /*-------------------------------------------------*\
        | These devices use a linear report and             |
        | transaction ID 0x80                               |
        \*-------------------------------------------------*/
        case RAZER_MAMBA_WIRED_PID:
        case RAZER_MAMBA_WIRELESS_PID:
            report                      = razer_create_custom_frame_linear_report(start_col, stop_col, rgb_data);
            report.transaction_id.id    = 0x80;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and    |
        | transaction ID 0x3F                               |
        \*-------------------------------------------------*/
        case RAZER_BLADE_LATE_2016_PID:
        case RAZER_BLACKWIDOW_CHROMA_V2_PID:
        case RAZER_NAGA_HEX_V2_PID:
            report                      = razer_create_custom_frame_standard_matrix_report(row_index, start_col, stop_col, rgb_data);
            report.transaction_id.id    = 0x3F;
            break;

        /*-------------------------------------------------*\
        | These devices use individual LED effect reports   |
        | and transaction ID 0x1F                           |
        \*-------------------------------------------------*/
        case RAZER_TARTARUS_CHROMA_PID:
            report                      = razer_create_set_led_rgb_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT, rgb_data);
            report.transaction_id.id    = 0x1F;
            break;

        case RAZER_DEATHADDER_CHROMA_PID:
            report                      = razer_create_set_led_rgb_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_SCROLL_WHEEL, rgb_data);
            report.transaction_id.id    = 0x1F;

            razer_usb_send(&report);

            report                      = razer_create_set_led_rgb_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_LOGO, &rgb_data[3]);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | The Orbweaver Chroma has an unusual matrix layout |
        | and the following code allows it to present as a  |
        | 5x5 matrix.  The hardware layout is:              |
        |                                                   |
        |   XX  XX  XX  XX  XX  XX  XX                      |
        |   XX  01  02  03  04  05  XX                      |
        |   XX  06  07  08  09  10  XX                      |
        |   XX  11  12  13  14  15  XX                      |
        |   XX  16  XX  17  18  19  20                      |
        |                                                   |
        | It uses a standard matrix report and transaction  |
        | ID 0x3F                                           |
        \*-------------------------------------------------*/
        case RAZER_ORBWEAVER_CHROMA_PID:
            if(row_index != 3)
            {
                report                  = razer_create_custom_frame_standard_matrix_report(row_index + 1, start_col + 1, stop_col + 1, rgb_data);
            }
            else
            {
                unsigned char rgb_data_adj[6*3];

                memcpy(&rgb_data_adj[0], &rgb_data[0], 3);
                memcpy(&rgb_data_adj[6], &rgb_data[3], 3*4);

                report                  = razer_create_custom_frame_standard_matrix_report(row_index + 1, start_col + 1, stop_col + 2, rgb_data_adj);
            }
            report.transaction_id.id    = 0x3F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and    |
        | transaction ID 0x80                               |
        \*-------------------------------------------------*/
        case RAZER_BLADE_STEALTH_PID:
        case RAZER_BLADE_STEALTH_LATE_2016_PID:
        case RAZER_BLADE_STEALTH_MID_2017_PID:
        case RAZER_BLADE_STEALTH_LATE_2017_PID:
        case RAZER_BLADE_STEALTH_2019_PID:
        case RAZER_BLADE_QHD_PID:
        case RAZER_BLADE_PRO_LATE_2016_PID:
        case RAZER_BLADE_2018_PID:
        case RAZER_BLADE_2018_MERCURY_PID:
        case RAZER_BLADE_2018_BASE_PID:
        case RAZER_BLADE_2019_ADV_PID:
        case RAZER_BLADE_MID_2019_MERCURY_PID:
        case RAZER_BLADE_STUDIO_EDITION_2019_PID:
        case RAZER_BLADE_PRO_2017_PID:
        case RAZER_BLADE_PRO_2017_FULLHD_PID:
        case RAZER_BLADE_PRO_LATE_2019_PID:
        case RAZER_BLADE_15_ADV_2020_PID:
            report                      = razer_create_custom_frame_standard_matrix_report(row_index, start_col, stop_col, rgb_data);
            report.transaction_id.id    = 0x80;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report        |
        \*-------------------------------------------------*/
        case RAZER_CORE_PID:
        default:
            report                      = razer_create_custom_frame_standard_matrix_report(row_index, start_col, stop_col, rgb_data);
            break;
    }

    razer_usb_send(&report);
}

void RazerController::razer_set_mode_custom()
{
    razer_report report;

    switch(dev_pid)
    {
        /*-------------------------------------------------*\
        | These devices use an extended matrix report       |
        \*-------------------------------------------------*/
        case RAZER_ORNATA_CHROMA_PID:
        case RAZER_HUNTSMAN_ELITE_PID:
        case RAZER_HUNTSMAN_TE_PID:
        case RAZER_BLACKWIDOW_2019_PID:
        case RAZER_HUNTSMAN_PID:
        case RAZER_CYNOSA_CHROMA_PID:
        case RAZER_DEATHADDER_ELITE_PID:
        case RAZER_NAGA_CHROMA_PID:
        case RAZER_LANCEHEAD_WIRED_PID:
        case RAZER_LANCEHEAD_WIRELESS_PID:
        case RAZER_LANCEHEAD_TE_WIRED_PID:
        case RAZER_LANCEHEAD_WIRELESS_RECEIVER_PID:
        case RAZER_LANCEHEAD_WIRELESS_WIRED_PID:
        case RAZER_MAMBA_WIRELESS_RECEIVER_PID:
        case RAZER_MAMBA_WIRELESS_WIRED_PID:
        case RAZER_BASILISK_PID:
        case RAZER_DEATHADDER_V2_PID:
        case RAZER_DEATHADDER_V2_PRO_WIRED_PID:
        case RAZER_DEATHADDER_V2_PRO_WIRELESS_PID:
        case RAZER_DEATHADDER_V2_MINI_PID:
        case RAZER_VIPER_PID:
        case RAZER_VIPER_MINI_PID:
        case RAZER_VIPER_ULTIMATE_WIRED_PID:
        case RAZER_VIPER_ULTIMATE_WIRELESS_PID:
        case RAZER_FIREFLY_HYPERFLUX_PID:
        case RAZER_GOLIATHUS_CHROMA_PID:
        case RAZER_GOLIATHUS_CHROMA_EXTENDED_PID:
        case RAZER_CHROMA_HDK_PID:
        case RAZER_CHROMA_BASE_PID:
        case RAZER_NOMMO_PRO_PID:
        case RAZER_NOMMO_CHROMA_PID:
            report                      = razer_create_mode_custom_extended_matrix_report();
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report and   |
        | transaction ID 0x1F                               |
        \*-------------------------------------------------*/
        case RAZER_TARTARUS_V2_PID:
        case RAZER_BLACKWIDOW_ELITE_PID:
        case RAZER_CYNOSA_V2_PID:
        case RAZER_ORNATA_CHROMA_V2_PID:
        case RAZER_MAMBA_ELITE_PID:
        case RAZER_KRAKEN_KITTY_EDITION_PID:
        case RAZER_MOUSE_BUNGEE_V3_CHROMA_PID:
        case RAZER_BASE_STATION_V2_CHROMA_PID:
            report                      = razer_create_mode_custom_extended_matrix_report();
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and    |
        | transaction ID 0x3F                               |
        \*-------------------------------------------------*/
        case RAZER_BLACKWIDOW_CHROMA_V2_PID:
        case RAZER_ORBWEAVER_CHROMA_PID:
        case RAZER_NAGA_HEX_V2_PID:
            report                      = razer_create_mode_custom_standard_matrix_report(RAZER_STORAGE_NO_SAVE);
            report.transaction_id.id    = 0x3F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and    |
        | transaction ID 0x80                               |
        \*-------------------------------------------------*/
        case RAZER_MAMBA_WIRELESS_PID:
            report                      = razer_create_mode_custom_standard_matrix_report(RAZER_STORAGE_NO_SAVE);
            report.transaction_id.id    = 0x80;
            break;

        /*-------------------------------------------------*\
        | These devices use individual LED effect reports   |
        | and transaction ID 0x1F                           |
        \*-------------------------------------------------*/
        case RAZER_TARTARUS_CHROMA_PID:
            report                      = razer_create_set_led_effect_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT, 0);
            report.transaction_id.id    = 0x1F;
            break;

        case RAZER_DEATHADDER_CHROMA_PID:
            report                      = razer_create_set_led_effect_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_SCROLL_WHEEL, 0);
            report.transaction_id.id    = 0x1F;

            razer_usb_send(&report);

            report                      = razer_create_set_led_effect_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_LOGO, 0);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report        |
        \*-------------------------------------------------*/
        case RAZER_FIREFLY_PID:
        case RAZER_CORE_PID:
        case RAZER_CHROMA_MUG_PID:
        default:
            report                      = razer_create_mode_custom_standard_matrix_report(RAZER_STORAGE_NO_SAVE);
            break;
    }

    razer_usb_send(&report);
}

void RazerController::razer_set_mode_spectrum_cycle()
{
    razer_report report;

    switch(dev_pid)
    {
        /*-------------------------------------------------*\
        | These devices use an extended matrix report       |
        \*-------------------------------------------------*/
        case RAZER_ORNATA_CHROMA_PID:
        case RAZER_HUNTSMAN_ELITE_PID:
        case RAZER_HUNTSMAN_TE_PID:
        case RAZER_BLACKWIDOW_2019_PID:
        case RAZER_HUNTSMAN_PID:
        case RAZER_CYNOSA_CHROMA_PID:
        case RAZER_CYNOSA_LITE_PID:
        case RAZER_NAGA_HEX_V2_PID:
        case RAZER_NAGA_CHROMA_PID:
            report                      = razer_create_mode_spectrum_cycle_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT);
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report and   |
        | transaction ID 0x1F                               |
        \*-------------------------------------------------*/
        case RAZER_BLACKWIDOW_ELITE_PID:
        case RAZER_CYNOSA_V2_PID:
        case RAZER_ORNATA_CHROMA_V2_PID:
        case RAZER_TARTARUS_V2_PID:
        case RAZER_MOUSE_BUNGEE_V3_CHROMA_PID:
        case RAZER_BASE_STATION_V2_CHROMA_PID:
            report                      = razer_create_mode_spectrum_cycle_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT);
            report.transaction_id.id    = 0x1F;
            break;

        /*-------------------------------------------------*\
        | These devices use an extended matrix report and   |
        | transaction ID 0x3F                               |
        \*-------------------------------------------------*/
        case RAZER_FIREFLY_HYPERFLUX_PID:
        case RAZER_GOLIATHUS_CHROMA_PID:
        case RAZER_GOLIATHUS_CHROMA_EXTENDED_PID:
        case RAZER_CHROMA_HDK_PID:
        case RAZER_CHROMA_BASE_PID:
        case RAZER_NOMMO_PRO_PID:
        case RAZER_NOMMO_CHROMA_PID:
            report                      = razer_create_mode_spectrum_cycle_extended_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_ZERO);
            report.transaction_id.id    = 0x3F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report and    |
        | transaction ID 0x3F                               |
        \*-------------------------------------------------*/
        case RAZER_BLACKWIDOW_CHROMA_V2_PID:
        case RAZER_FIREFLY_PID:
        case RAZER_CORE_PID:
        case RAZER_CHROMA_MUG_PID:
            report                      = razer_create_mode_spectrum_cycle_standard_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT);
            report.transaction_id.id    = 0x3F;
            break;

        /*-------------------------------------------------*\
        | These devices use a standard matrix report        |
        \*-------------------------------------------------*/
        default:
            report                      = razer_create_mode_spectrum_cycle_standard_matrix_report(RAZER_STORAGE_NO_SAVE, RAZER_LED_ID_BACKLIGHT);
            break;
    }

    razer_usb_send(&report);
}

/*---------------------------------------------------------------------------------*\
| USB transfer functions                                                            |
\*---------------------------------------------------------------------------------*/

int RazerController::razer_usb_receive(razer_report* report)
{
    return hid_get_feature_report(dev, (unsigned char*)report, sizeof(*report));
}

int RazerController::razer_usb_send(razer_report* report)
{
    report->crc = razer_calculate_crc(report);

    return hid_send_feature_report(dev, (unsigned char*)report, sizeof(*report));
}
