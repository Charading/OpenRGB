#ifndef DEVICETYPE_H
#define DEVICETYPE_H

/*------------------------------------------------------------------*\
| Device Types                                                       |
|   The enum order should be maintained as is for the API however    |
|   DEVICE_TYPE_UNKNOWN needs to remain last. Any new device types   |
|   need to be inserted at the end of the list but before unknown.   |
\*------------------------------------------------------------------*/
typedef int device_type;

enum
{
    DEVICE_TYPE_MOTHERBOARD,
    DEVICE_TYPE_DRAM,
    DEVICE_TYPE_GPU,
    DEVICE_TYPE_COOLER,
    DEVICE_TYPE_LEDSTRIP,
    DEVICE_TYPE_KEYBOARD,
    DEVICE_TYPE_MOUSE,
    DEVICE_TYPE_MOUSEMAT,
    DEVICE_TYPE_HEADSET,
    DEVICE_TYPE_HEADSET_STAND,
    DEVICE_TYPE_GAMEPAD,
    DEVICE_TYPE_LIGHT,
    DEVICE_TYPE_SPEAKER,
    DEVICE_TYPE_VIRTUAL,
    DEVICE_TYPE_STORAGE,
    DEVICE_TYPE_CASE,
    DEVICE_TYPE_MICROPHONE,
    DEVICE_TYPE_ACCESSORY,
    DEVICE_TYPE_KEYPAD,
    DEVICE_TYPE_UNKNOWN,
};

#endif // DEVICETYPE_H
