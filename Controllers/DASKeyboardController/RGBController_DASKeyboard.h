/*-----------------------------------------*\
|  RGBController_DASKeyboard.h              |
|                                           |
|  Generic RGB Interface for DAS RGB        |
|  keyboard devices                         |
|                                           |
|  Frank Niessen (denk_mal) 12/16/2020      |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "DASKeyboardController.h"

/*
<start_token> <length> <78> <cmd> <key_code> <mode> <red> <green> <blue> <chk_sum>
Set key 'Space' to (red):
ea 08 78 08 24 01 ff 00 00 48 (static)
ea 08 78 08 24 1f ff 00 00 56 (blink)
ea 08 78 08 24 08 ff 00 00 41 (breathe)
ea 08 78 08 24 14 ff 00 00 5d (color_cycle)

ea 0a 78 03 24 11 ff 00 00 00 00 51 (ripple)
ea 0a 78 03 24 21 ff 00 00 00 00 61 (inv_ripple)

ea 0b 78 04 24 1e ff 00 00 00 00 00 58 (bg_set)
ea 0b 78 04 24 1f ff 00 00 01 f4 00 ac (bg_blink)
ea 0b 78 04 24 08 ff 00 00 03 e8 00 a5 (bg_breathe)
ea 0b 78 04 24 14 00 00 00 e4 18 00 51 (bg_color_cycle)
ea 0b 78 04 24 11 ff 00 00 00 00 00 57 (bg_ripple)
ea 0b 78 04 24 21 ff 00 00 00 00 00 67 (bg_inv_ripple)
ea 0b 78 04 24 0b ff 00 00 00 00 00 4d (bg_laser)

set whole keyboard to (red):
ea 08 78 01 ff 01 ff 00 00 9a (set)
ea 08 78 01 ff 1f ff 00 00 84 (blink)
ea 08 78 01 ff 08 ff 00 00 93 (breathe)
ea 08 78 01 ff 14 00 00 00 70 (color_cycle)

ea 05 78 05 00 00 92 (color_cloud)
*/

enum {
  DAS_MODE_STATIC      = 0x01,
  DAS_MODE_BLINKING    = 0x1f,
  DAS_MODE_BREATHING   = 0x08,
  DAS_MODE_COLOR_CYCLE = 0x14
};


class RGBController_DASKeyboard : public RGBController {
 public:
  explicit RGBController_DASKeyboard(DASKeyboardController *das_ptr);

  ~RGBController_DASKeyboard() override;

  void SetupZones() override;

  void ResizeZone(int zone, int new_size) override;

  void DeviceUpdateLEDs() override;

  void UpdateZoneLEDs(int zone) override;

  void UpdateSingleLED(int led) override;

  void SetCustomMode() override;

  void DeviceUpdateMode() override;

 private:
  std::vector<int>   mode_index;         /* mode of led             */
  std::vector<RGBColor>   double_buffer;

  DASKeyboardController *das;
  bool updateDevice;
};
