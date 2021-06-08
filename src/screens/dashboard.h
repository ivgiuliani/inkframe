#ifndef __SCREEN_DASHBOARD_H
#define __SCREEN_DASHBOARD_H

#include <map>
#include <Arduino.h>

#include "base.h"
#include "display.h"
#include "weather.h"
#include "rtc.h"
#include "tfl.h"
#include "wikipedia.h"

struct screen_t {
  weather_t current_weather;
  DateTime current_time;
  int16_t home_temperature;
  std::array<weather_t,3> next_three_days_weather;
  std::map<String, String> tube_status;
  struct wikipedia_onthisday_t wikipedia_entry;
};

namespace Screen {
  class Dashboard : public BaseScreen {
    void setup(Display *);
    void unload() {};
    void handle() {};
  };
};

#endif // __SCREEN_DASHBOARD_H
