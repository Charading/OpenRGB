
#define CAPABLITIES(subsystem)      {subsystem, 0x01}

// XAP Subsystem
#define XAP_VERSION_REQUEST         {0x00, 0x00}
#define ENABLED_SUBSYSTEMS          {0x00, 0x02}

// QMK Subsystem
#define KB_MANUFACTURER_REQUEST     {0x01, 0x03}
#define KB_NAME_REQUEST             {0x01, 0x04}
#define KB_HWID_REQUEST             {0x01, 0x08}
#define CONFIG_BLOB_LEN             {0x01, 0x05}
#define CONFIG_BLOB_CHUNK           {0x01, 0x06}

// Keymap Subsystem
#define KEYCODE_REQUEST             {0x04, 0x03}

// Lighting Subsystem
