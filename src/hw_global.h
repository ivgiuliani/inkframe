#ifndef __HW_GLOBAL_H
#define __HW_GLOBAL_H

#include <buttonctrl.h>

#include "connectivity.h"
#include "display.h"
#include "pins.h"
#include "rotary_encoder.h"
#include "rtc.h"

// A common place to aggregate all HW access.
struct {
  RTC rtc;
  ButtonCtrl<PIN_RE_BUTTON, HIGH, INPUT_PULLUP> button;
  RotaryEncoder<PIN_RE_CLK, PIN_RE_DT> rotary_encoder;
} HW;

#endif // __HW_GLOBAL_H
