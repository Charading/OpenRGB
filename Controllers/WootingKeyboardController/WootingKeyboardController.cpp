/*-----------------------------------------*\
|  WootingKeyboardController.cpp            |
|                                           |
|  Driver for Wooting RGB keyboardlighting  |
|  controller                               |
|                                           |
|  Diogo Trindade (diogotr7)    3/4/2021    |
\*-----------------------------------------*/

#include <cstring>
#include "WootingKeyboardController.h"
#define WOOTING_COMMAND_SIZE 8
#define WOOTING_REPORT_SIZE 129
#define WOOTING_RAW_COLORS_REPORT 11
#define WOOTING_SINGLE_COLOR_COMMAND 30
#define WOOTING_SINGLE_RESET_COMMAND 31
#define WOOTING_RESET_ALL_COMMAND 32
#define WOOTING_COLOR_INIT_COMMAND 33
#define WOOTING_ONE_KEY_CODE_LIMIT 95
#define WOOTING_TWO_KEY_CODE_LIMIT 116
#define RGB_RAW_BUFFER_SIZE 96

static uint16_t getCrc16ccitt(const uint8_t* buffer, uint16_t size)
{
    uint16_t crc = 0;

    while (size--) {
        crc ^= (*buffer++ << 8);

        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            }
            else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

WootingKeyboardController::WootingKeyboardController(hid_device* dev_handle)
{
    dev = dev_handle;

    SendInitialize();
}

WootingKeyboardController::~WootingKeyboardController()
{
    
}

void WootingKeyboardController::SendColors
    (
    unsigned char*  buf0,
    unsigned char*  buf1,
    unsigned char*  buf2,
    unsigned char*  buf3
    )
{
    wooting_usb_send_buffer(RGB_PARTS::PART0, buf0);
    wooting_usb_send_buffer(RGB_PARTS::PART1, buf1);
    wooting_usb_send_buffer(RGB_PARTS::PART2, buf2);
    wooting_usb_send_buffer(RGB_PARTS::PART3, buf3);
}

void WootingKeyboardController::SendInitialize()
{
    wooting_usb_send_feature(WOOTING_COLOR_INIT_COMMAND, 0,0,0,0);
    unsigned char stub = 0;
    hid_read(dev, &stub, 0);
    hid_read_timeout(dev, &stub, 0, 50);
}

bool WootingKeyboardController::wooting_usb_send_feature(uint8_t commandId, uint8_t parameter0, uint8_t parameter1, uint8_t parameter2, uint8_t parameter3) {
    // prevent sending unnecessary data to the wootings if the index exceedes it's capabilities
    if ((commandId == WOOTING_SINGLE_COLOR_COMMAND && parameter0 > WOOTING_ONE_KEY_CODE_LIMIT)
    || (commandId == WOOTING_SINGLE_RESET_COMMAND && parameter3 > WOOTING_ONE_KEY_CODE_LIMIT)) {
        // this is not a usb error so let's return true. wooting_rgb_direct_set_key would also behave differently otherwise.
        return true;
    }

    unsigned char report_buffer[WOOTING_COMMAND_SIZE];

    report_buffer[0] = 0; // HID report index (unused)
    report_buffer[1] = 0xD0; // Magic word
    report_buffer[2] = 0xDA; // Magic word
    report_buffer[3] = commandId;
    report_buffer[4] = parameter3;
    report_buffer[5] = parameter2;
    report_buffer[6] = parameter1;
    report_buffer[7] = parameter0;
    int command_size = hid_send_feature_report(dev, report_buffer, WOOTING_COMMAND_SIZE);
}

bool WootingKeyboardController::wooting_usb_send_buffer(RGB_PARTS part_number, uint8_t rgb_buffer[]) {
    unsigned char report_buffer[WOOTING_REPORT_SIZE] = { 0 };

    report_buffer[0] = 0; // HID report index (unused)
    report_buffer[1] = 0xD0; // Magicword
    report_buffer[2] = 0xDA; // Magicword
    report_buffer[3] = WOOTING_RAW_COLORS_REPORT; // Report ID
    switch (part_number) {
        case PART0: {
            report_buffer[4] = 0; // Slave nr
            report_buffer[5] = 0; // Reg start address
            break;
        }
        case PART1: {
            report_buffer[4] = 0; // Slave nr
            report_buffer[5] = RGB_RAW_BUFFER_SIZE; // Reg start address
            break;
        }
        case PART2: {
            report_buffer[4] = 1; // Slave nr
            report_buffer[5] = 0; // Reg start address
            break;
        }
        case PART3: {
            report_buffer[4] = 1; // Slave nr
            report_buffer[5] = RGB_RAW_BUFFER_SIZE; // Reg start address
            break;
        }
        default: {
            return false;
        }
    }

    memcpy(&report_buffer[6], rgb_buffer, RGB_RAW_BUFFER_SIZE);

    unsigned short crc = getCrc16ccitt((unsigned char*)&report_buffer, WOOTING_REPORT_SIZE - 2);
    report_buffer[127] = (unsigned char)crc;
    report_buffer[128] = crc >> 8;
    int ret = hid_write(dev, report_buffer, WOOTING_REPORT_SIZE);
    int a;
}
