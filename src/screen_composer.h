#ifndef __SCREEN_COMPOSER_H
#define __SCREEN_COMPOSER_H

#include <map>
#include <Arduino.h>

#include "Display.h"
#include "weather.h"
#include "tfl.h"

struct screen_t {
  weather_t current_weather;
  std::array<weather_t,3> next_three_days_weather;
  std::map<String, String> tube_status;
};

struct screen_t update_screen_data();
void update_display(Display *, screen_t);

#endif // __SCREEN_COMPOSER_H
