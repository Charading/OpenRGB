/*-----------------------------------------*\
|  DasKeyboardController.cpp                |
|                                           |
|  Driver for Das RGB keyboardlighting      |
|  controller                               |
|                                           |
|  Frank Niessen (denk_mal) 12/16/2020      |
\*-----------------------------------------*/

#include <cstring>
#include "DasKeyboardController.h"

DasKeyboardController::DasKeyboardController(hid_device *dev_handle, const char *path) {
    dev = dev_handle;
    location = path;

    SendInitialize();
}

DasKeyboardController::~DasKeyboardController() = default;

std::string DasKeyboardController::GetDeviceLocation() {
    return "HID: " + location;
}

std::string DasKeyboardController::GetSerialString() {
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    if (return_string.empty()) {
        return_string = version;
    }

    return return_string;
}

std::string DasKeyboardController::GetVersionString() {
    std::string fw_version = "V";
    fw_version += version.substr(6, 2);
    fw_version += ".";
    fw_version += version.substr(15, 2);
    fw_version += ".0";

    return fw_version;
}

// Todo: experimental for now; should be '16' for US and '28' for EU layout
std::string DasKeyboardController::GetLayoutString() {
    std::string layout_id = version.substr(3, 2);
    if (layout_id == "16") {
        return "US";
    }

    return "EU";
}

void DasKeyboardController::SendColors(unsigned char key_id, unsigned char mode,
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


void DasKeyboardController::SendInitialize() {
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

void DasKeyboardController::SendApply() {
    /*-----------------------------------------------------*\
    | Set up Terminate Color packet                         |
    \*-----------------------------------------------------*/
    unsigned char usb_buf2[] = {0xEA, 0x03, 0x78, 0x0a};
    SendData(usb_buf2, sizeof(usb_buf2));

    unsigned char usb_buf[256] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    /*auto status =*/ ReceiveData(usb_buf);
}

void DasKeyboardController::SendData(const unsigned char *data, int length) {
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

int DasKeyboardController::ReceiveData(unsigned char *data) {
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
