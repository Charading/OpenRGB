/*-----------------------------------------*\
|  DASKeyboardController.cpp                |
|                                           |
|  Driver for DAS RGB keyboardlighting      |
|  controller                               |
|                                           |
|  Frank Niessen (denk_mal) 12/16/2020      |
\*-----------------------------------------*/

#include <cstring>
#include "DASKeyboardController.h"

DASKeyboardController::DASKeyboardController(hid_device *dev_handle, const char *path) {
    dev = dev_handle;
    location = path;

    SendInitialize();
}

DASKeyboardController::~DASKeyboardController() = default;

std::string DASKeyboardController::GetDeviceLocation() {
    return "HID: " + location;
}

std::string DASKeyboardController::GetSerialString() {
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return return_string;
}

std::string DASKeyboardController::GetVersionString() {
    return version;
}

void DASKeyboardController::SendColors(unsigned char key_id, unsigned char mode,
                                       unsigned char red, unsigned char green, unsigned char blue) {
    if (key_id < 130) {
        unsigned char usb_buf[] = {0xEA,
                                   0x08,
                                   0x78,
                                   0x08,
                                   static_cast<unsigned char>(key_id),
                                   mode,
                                   red,
                                   green,
                                   blue};
        SendData(usb_buf, sizeof(usb_buf));
    } else {  // special handling for the Q-Button; only color, no mode
        unsigned char usb_buf[] = {0xEA,
                                   0x06,
                                   0x78,
                                   0x06,
                                   red,
                                   green,
                                   blue};
        SendData(usb_buf, sizeof(usb_buf));
    }
}


void DASKeyboardController::SendInitialize() {
    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Initialize connection                          |
    \*-----------------------------------------------------*/
    unsigned char usb_init[] = {0xEA, 0x02, 0xB0};
    SendData(usb_init, sizeof(usb_init));

    /*-----------------------------------------------------*\
    | get Version String                                    |
    \*-----------------------------------------------------*/
    auto status = ReceiveData(usb_buf);
    if (status == -1) {
        // data was not correct;
        //return;
    }
    std::string fw_version(reinterpret_cast<char *>(&usb_buf[2]));
    version = fw_version;

    /*-----------------------------------------------------*\
    | Set up Initialize Color packet                        |
    \*-----------------------------------------------------*/
    // unsigned char usb_buf2[] = {0xEA, 0x08, 0x78, 0x01, 0xFF, 0x01, 0x00, 0x00, 0x00};
    // SendData(usb_buf2, sizeof(usb_buf2));
    //*status =*/ ReceiveData(usb_buf);
}

void DASKeyboardController::SendApply() {
    /*-----------------------------------------------------*\
    | Set up Terminate Color packet                         |
    \*-----------------------------------------------------*/
    unsigned char usb_buf2[] = {0xEA, 0x03, 0x78, 0x0a};
    SendData(usb_buf2, sizeof(usb_buf2));

    unsigned char usb_buf[256] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    /*auto status =*/ ReceiveData(usb_buf);
}

void DASKeyboardController::SendData(const unsigned char *data, int length) {
    unsigned char usb_buf[65];

    /*-----------------------------------------------------*\
    | Fill data into send buffer                            |
    \*-----------------------------------------------------*/
    unsigned int chk_sum = 0;
    usb_buf[0] = 1;
    for (auto idx = 0; idx < length; idx++) {
        usb_buf[idx + 1] = data[idx];
        chk_sum ^= data[idx];
    }
    usb_buf[++length] = chk_sum;
    length++;

    hid_send_feature_report(dev, usb_buf, length);
}

int DASKeyboardController::ReceiveData(unsigned char *data) {
    int idx = 0;
    unsigned char usb_buf[9];

    /*-----------------------------------------------------*\
    | Fill data from receive buffer                         |
    \*-----------------------------------------------------*/
    unsigned int chk_sum = 0;
    do {
        memset(usb_buf, 0x00, sizeof(usb_buf));
        usb_buf[0x00] = 0x01;
        hid_get_feature_report(dev, usb_buf, 8);
        if (usb_buf[0]) {
            for (auto ii = 0; ii < 8; ii++) {
                data[idx++] = usb_buf[ii];
                chk_sum ^= usb_buf[ii];
            }
        }
    } while (usb_buf[0]);

    // if checksum not correct cleanup databuffer
    if (chk_sum) {
        for (auto ii = 0; ii < idx; ii++) {
            data[ii] = 0;
        }
        return -1;
    }

    if (idx) {
        idx = data[1];
        // remove first two bytes (signature?? and content length
        for (auto ii = 0; ii < idx - 1; ii++) {
            data[ii] = data[ii + 2];
        }
        // remove checksum
        data[idx + 1] = 0;
        // remove duplicate bytes at the end
        data[idx--] = 0;
        data[idx] = 0;
    }

    return idx;
}
