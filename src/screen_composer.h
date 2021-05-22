#ifndef __SCREEN_COMPOSER_H
#define __SCREEN_COMPOSER_H

#include <map>
#include <Arduino.h>

#include "display.h"
#include "weather.h"
#include "rtc.h"
#include "tfl.h"

struct screen_t {
  weather_t current_weather;
  DateTime current_time;
  int16_t home_temperature;
  std::array<weather_t,3> next_three_days_weather;
  std::map<String, String> tube_status;
};

struct screen_t update_screen_data(RTC *);
void update_display(Display *, screen_t);

#endif // __SCREEN_COMPOSER_H
