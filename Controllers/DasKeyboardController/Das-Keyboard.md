The Das Keyboard 4Q and 5Q keyboards are connected via USB
at 24F0:2037 (4Q) or 24F0:2020 (5Q).
The devices use the USB HID protocol and controls the led's over interface 1.

The datatransfer of the original driver is splitted into 8-byte chunks but is possible to send
longer or shorter chunks on linux; On Windows the chunk size has to be 8 bytes.
The first byte of every sending chunk is always '0x01'.

If the command send an response it will send an empty chunk (8 0x00) as end-of-transfer marker

All informations are reverse enginered as best as possible based on firmware version v21.27.0.

:exclamation: There is a known bug in firmware v21.27.0 that could end up to missing commands send to
the keyboard.

(https://qforum.daskeyboard.com/t/4q-key-down-up-loss-debugging/ ; Login necessary)

# Get Version
Get the Identification String from the keyboard containing the firmware version.
### Send
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x02       | Length of data including chk_sum |
| 0x02       | 0xB0       | Command                          |
| 0x03       | 0x58       | checksum (x-or of bytes 0 to 2)  |

### Response
Response Content contains an Identification String (Format of this string is unknown)

| Byte Index  | Byte Value | Description                      |
| ----------- | ---------- | -------------------------------- |
| 0x00        | 0xED       | Signature                        |
| 0x01        | 0x14       | Length of data including chk_sum |
| 0x02        | 0xB0       | Command                          |
| 0x03        | 0x00       | 0x00 for OK ???                  |
| 0x04 - 0x14 | 0xXX       | Ident String                     |
| 0x15        | 0xXX       | checksum (x-or of bytes 0 to 20) |

# Set Whole Keyboard
Switch to a mode where all leds are set by a keyboard internal function (like Q-Button + 1-5).
The settings for single led (see below) are not touched!
### Mode
| Mode Value | Mode Description |
| ---------- | ---------------- |
| 0x01       | Static           |
| 0x08       | Breathe          |
| 0x14       | Color Cycle      |
| 0x1F       | Blink            |

### Send
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x08       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x01       | whole Keyboard                   |
| 0x04       | 0xFF       | ???                              |
| 0x05       | 0xXX       | Mode                             |
| 0x06       | 0xXX       | Color Red                        |
| 0x07       | 0xXX       | Color Green                      |
| 0x08       | 0xXX       | Color Blue                       |
| 0x09       | 0xXX       | checksum (x-or of bytes 0 to 8)  |

### Response
The response has the following format.

if OK

| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xED       | Signature                        |
| 0x01       | 0x03       | Length of data including chk_sum |
| 0x02       | 0x78       | Command                          |
| 0x03       | 0x00       | 0x00 for OK ???                  |
| 0x04       | 0x96       | checksum (x-or of bytes 0 to 2)  |

on Error

| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xED       | Signature                        |
| 0x01       | 0x03       | Length of data including chk_sum |
| 0x02       | 0x00       | Command                          |
| 0x03       | 0xF0       | Error Code ???                   |
| 0x04       | 0x96       | checksum (x-or of bytes 0 to 2)  |

## Specials
Set whole keyboard to fast rainbow wave
### Send
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x05       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x05       | Set color wave                   |
| 0x04       | 0x00       | ???                              |
| 0x05       | 0x00       | ???                              |
| 0x06       | 0x92       | checksum (x-or of bytes 0 to 8)  |

# Set Single Keyboard Key
Set the mode of a single key. The single key modes are activated by an acknowledge sequence.
This acknowledge will switch all key into single led mode (see above)!
### Mode
| Mode Value | Mode Description     | Transfer Format |
| ---------- | -------------------- | --------------- |
| 0x01       | Static               | 1
| 0x08       | Breathe              | 1, 3
| 0x14       | Color Cycle          | 1, 3
| 0x1F       | Blink                | 1, 3
| 0x11       | Ripple               | 2, 3
| 0x21       | Inverse Ripple       | 2, 3
| 0x0B       | Laser                | 3
| 0x1E       | Static (2. color?)   | 3

### Send (Transfer Format 1)
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x08       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x08       | single Keyboard Key              |
| 0x04       | 0xXX       | key code                         |
| 0x05       | 0xXX       | Mode (one mode)                  |
| 0x06       | 0xXX       | Color Red                        |
| 0x07       | 0xXX       | Color Green                      |
| 0x08       | 0xXX       | Color Blue                       |
| 0x09       | 0xXX       | checksum (x-or of bytes 0 to 8)  |

### Send (Transfer Format 2)
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x0A       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x03       | single Keyboard Key              |
| 0x04       | 0xXX       | key code                         |
| 0x05       | 0xXX       | Mode  (multiple mode)            |
| 0x06       | 0xXX       | Color Red                        |
| 0x07       | 0xXX       | Color Green                      |
| 0x08       | 0xXX       | Color Blue                       |
| 0x09       | 0x00       | ???                              |
| 0x0A       | 0x00       | ???                              |
| 0x0B       | 0xXX       | checksum (x-or of bytes 0 to 10) |

### Send (Transfer Format 3)
This mode will be active on key presing.

| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x0B       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x04       | single Keyboard Key              |
| 0x04       | 0xXX       | key code                         |
| 0x05       | 0xXX       | Mode                             |
| 0x06       | 0xXX       | Color Red                        |
| 0x07       | 0xXX       | Color Green                      |
| 0x08       | 0xXX       | Color Blue                       |
| 0x09       | 0x00       | ??? [note]                       |
| 0x0A       | 0x00       | ??? [note]                       |
| 0x0B       | 0x00       | ???                              |
| 0x0C       | 0xXX       | checksum (x-or of bytes 0 to 11) |
note: for Blink (0x1F), Breathe (0x08) and Color Cycle (0x14) these two bytes are
containing diff. values
<pre>
Blink:       0x01, 0xF4
Breathe:     0x03, 0xE8 
Color Cycle: 0xE4, 0x18
</pre>
### Acknowledge
| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x03       | Length of data including chk_sum |
| 0x02       | 0x78       | Command                          |
| 0x02       | 0x0A       | Acknowledge                      |
| 0x04       | 0x9B       | checksum (x-or of bytes 0 to 3)  |

### Response

[see above](Das-Keyboard:response-1)

### Specials
Set color of Q-Button

| Byte Index | Byte Value | Description                      |
| ---------- | ---------- | -------------------------------- |
| 0x00       | 0xEA       | Signature                        |
| 0x01       | 0x06       | Length of data including chk_sum |
| 0x02       | 0x78       | Command: Set                     |
| 0x03       | 0x06       | Q-Button                         |
| 0x04       | 0xXX       | Color Red                        |
| 0x05       | 0xXX       | Color Green                      |
| 0x06       | 0xXX       | Color Blue                       |
| 0x07       | 0xXX       | checksum (x-or of bytes 0 to 6)  |
