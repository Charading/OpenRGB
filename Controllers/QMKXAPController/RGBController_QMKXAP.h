/*-------------------------------------------------------------------*\
|  RGBController_QMKXAP.h                                             |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#pragma once

#include <vector>
#include <map>

#include "RGBControllerKeyNames.h"
#include "RGBController.h"
#include "QMKXAPController.h"

#define NO_LED 0xFFFFFFFF

enum led_matrix_flags_t {
    LED_FLAG_NONE       = 0x00,
    LED_FLAG_MODIFIER   = 0x01,
    LED_FLAG_UNDERGLOW  = 0x02,
    LED_FLAG_KEYLIGHT   = 0x04,
    LED_FLAG_INDICATOR  = 0x08,
    LED_FLAG_ALL        = 0xFF,
};

static std::map<uint8_t, std::string> QMKKeycodeToKeynameMap
{
    { 0, KEY_EN_UNUSED          }, { 1, KEY_EN_RIGHT_FUNCTION   }, { 2, KEY_EN_UNUSED           }, { 3, KEY_EN_UNUSED           },
    { 4, KEY_EN_A               }, { 5, KEY_EN_B                }, { 6, KEY_EN_C                }, { 7, KEY_EN_D                },
    { 8, KEY_EN_E               }, { 9, KEY_EN_F                }, { 10, KEY_EN_G               }, { 11, KEY_EN_H               },
    { 12, KEY_EN_I              }, { 13, KEY_EN_J               }, { 14, KEY_EN_K               }, { 15, KEY_EN_L               },
    { 16, KEY_EN_M              }, { 17, KEY_EN_N               }, { 18, KEY_EN_O               }, { 19, KEY_EN_P               },
    { 20, KEY_EN_Q              }, { 21, KEY_EN_R               }, { 22, KEY_EN_S               }, { 23, KEY_EN_T               },
    { 24, KEY_EN_U              }, { 25, KEY_EN_V               }, { 26, KEY_EN_W               }, { 27, KEY_EN_X               },
    { 28, KEY_EN_Y              }, { 29, KEY_EN_Z               }, { 30, KEY_EN_1               }, { 31, KEY_EN_2               },
    { 32, KEY_EN_3              }, { 33, KEY_EN_4               }, { 34, KEY_EN_5               }, { 35, KEY_EN_6               },
    { 36, KEY_EN_7              }, { 37, KEY_EN_8               }, { 38, KEY_EN_9               }, { 39, KEY_EN_0               },
    { 40, KEY_EN_ANSI_ENTER     }, { 41, KEY_EN_ESCAPE          }, { 42, KEY_EN_BACKSPACE       }, { 43, KEY_EN_TAB             },
    { 44, KEY_EN_SPACE          }, { 45, KEY_EN_MINUS           }, { 46, KEY_EN_EQUALS          }, { 47, KEY_EN_LEFT_BRACKET    },
    { 48, KEY_EN_RIGHT_BRACKET  }, { 49, KEY_EN_ANSI_BACK_SLASH }, { 50, KEY_EN_POUND           }, { 51, KEY_EN_SEMICOLON       },
    { 52, KEY_EN_QUOTE          }, { 53, KEY_EN_BACK_TICK       }, { 54, KEY_EN_COMMA           }, { 55, KEY_EN_PERIOD          },
    { 56, KEY_EN_FORWARD_SLASH  }, { 57, KEY_EN_CAPS_LOCK       }, { 58, KEY_EN_F1              }, { 59, KEY_EN_F2              },
    { 60, KEY_EN_F3             }, { 61, KEY_EN_F4              }, { 62, KEY_EN_F5              }, { 63, KEY_EN_F6              },
    { 64, KEY_EN_F7             }, { 65, KEY_EN_F8              }, { 66, KEY_EN_F9              }, { 67, KEY_EN_F10             },
    { 68, KEY_EN_F11            }, { 69, KEY_EN_F12             }, { 70, KEY_EN_PRINT_SCREEN    }, { 71, KEY_EN_SCROLL_LOCK     },
    { 72, KEY_EN_PAUSE_BREAK    }, { 73, KEY_EN_INSERT          }, { 74, KEY_EN_HOME            }, { 75, KEY_EN_PAGE_UP         },
    { 76, KEY_EN_DELETE         }, { 77, KEY_EN_END             }, { 78, KEY_EN_PAGE_DOWN       }, { 79, KEY_EN_RIGHT_ARROW     },
    { 80, KEY_EN_LEFT_ARROW     }, { 81, KEY_EN_DOWN_ARROW      }, { 82, KEY_EN_UP_ARROW        }, { 83, KEY_EN_NUMPAD_LOCK     },
    { 84, KEY_EN_NUMPAD_DIVIDE  }, { 85, KEY_EN_NUMPAD_TIMES    }, { 86, KEY_EN_NUMPAD_MINUS    }, { 87, KEY_EN_NUMPAD_PLUS     },
    { 88, KEY_EN_NUMPAD_ENTER   }, { 89, KEY_EN_NUMPAD_1        }, { 90, KEY_EN_NUMPAD_2        }, { 91, KEY_EN_NUMPAD_3        },
    { 92, KEY_EN_NUMPAD_4       }, { 93, KEY_EN_NUMPAD_5        }, { 94, KEY_EN_NUMPAD_6        }, { 95, KEY_EN_NUMPAD_7        },
    { 96, KEY_EN_NUMPAD_8       }, { 97, KEY_EN_NUMPAD_9        }, { 98, KEY_EN_NUMPAD_0        }, { 99, KEY_EN_NUMPAD_PERIOD   },
    { 100, KEY_EN_ISO_BACK_SLASH}, { 101, KEY_EN_MENU           }, { 102, KEY_EN_UNUSED         }, { 103, KEY_EN_UNUSED         },
    { 104, KEY_EN_UNUSED        }, { 105, KEY_EN_UNUSED         }, { 106, KEY_EN_UNUSED         }, { 107, KEY_EN_UNUSED         },
    { 108, KEY_EN_UNUSED        }, { 109, KEY_EN_UNUSED         }, { 110, KEY_EN_UNUSED         }, { 111, KEY_EN_UNUSED         },
    { 112, KEY_EN_UNUSED        }, { 113, KEY_EN_UNUSED         }, { 114, KEY_EN_UNUSED         }, { 115, KEY_EN_UNUSED         },
    { 116, KEY_EN_UNUSED        }, { 117, KEY_EN_UNUSED         }, { 118, KEY_EN_UNUSED         }, { 119, KEY_EN_UNUSED         },
    { 120, KEY_EN_UNUSED        }, { 121, KEY_EN_UNUSED         }, { 122, KEY_EN_UNUSED         }, { 123, KEY_EN_UNUSED         },
    { 124, KEY_EN_UNUSED        }, { 125, KEY_EN_UNUSED         }, { 126, KEY_EN_UNUSED         }, { 127, KEY_EN_UNUSED         },
    { 128, KEY_EN_UNUSED        }, { 129, KEY_EN_UNUSED         }, { 130, KEY_EN_UNUSED         }, { 131, KEY_EN_UNUSED         },
    { 132, KEY_EN_UNUSED        }, { 133, KEY_EN_UNUSED         }, { 134, KEY_EN_UNUSED         }, { 135, KEY_EN_UNUSED         },
    { 136, KEY_EN_UNUSED        }, { 137, KEY_EN_UNUSED         }, { 138, KEY_EN_UNUSED         }, { 139, KEY_EN_UNUSED         },
    { 140, KEY_EN_UNUSED        }, { 141, KEY_EN_UNUSED         }, { 142, KEY_EN_UNUSED         }, { 143, KEY_EN_UNUSED         },
    { 144, KEY_EN_UNUSED        }, { 145, KEY_EN_UNUSED         }, { 146, KEY_EN_UNUSED         }, { 147, KEY_EN_UNUSED         },
    { 148, KEY_EN_UNUSED        }, { 149, KEY_EN_UNUSED         }, { 150, KEY_EN_UNUSED         }, { 151, KEY_EN_UNUSED         },
    { 152, KEY_EN_UNUSED        }, { 153, KEY_EN_UNUSED         }, { 154, KEY_EN_UNUSED         }, { 155, KEY_EN_UNUSED         },
    { 156, KEY_EN_UNUSED        }, { 157, KEY_EN_UNUSED         }, { 158, KEY_EN_UNUSED         }, { 159, KEY_EN_UNUSED         },
    { 160, KEY_EN_UNUSED        }, { 161, KEY_EN_UNUSED         }, { 162, KEY_EN_UNUSED         }, { 163, KEY_EN_UNUSED         },
    { 164, KEY_EN_UNUSED        },
    /*Space Cadet Left Shift*/     { 216, KEY_EN_LEFT_SHIFT     }, /*Space Cadet Right Shift*/     { 217, KEY_EN_RIGHT_SHIFT    },
    { 224, KEY_EN_LEFT_CONTROL  }, { 225, KEY_EN_LEFT_SHIFT     }, { 226, KEY_EN_LEFT_ALT       }, { 227, KEY_EN_LEFT_WINDOWS   },
    { 228, KEY_EN_RIGHT_CONTROL }, { 229, KEY_EN_RIGHT_SHIFT    }, { 230, KEY_EN_RIGHT_ALT      }, { 231, KEY_EN_RIGHT_WINDOWS  },
};

class RGBController_QMKXAP : public RGBController
{
public:
    RGBController_QMKXAP(QMKXAPController* controller_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    std::vector<unsigned int>   FlattenMatrixMap(VectorMatrix<unsigned int> matrix_map);
    VectorMatrix<unsigned int>  PlaceLEDs(VectorMatrix<uint16_t> keycodes, std::vector<XAPLED>& xap_leds);

    QMKXAPController* controller;
    std::vector<unsigned int>   flat_matrix_map;
};
