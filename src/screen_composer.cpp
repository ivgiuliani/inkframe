#include <Arduino.h>
#include <HTTPClient.h>
#include <TimeLib.h>

#include "weather.h"
#include "tfl.h"
#include "screen_composer.h"
#include "display.h"
#include "micro_utils.h"

#include "icons/weather.bmp.h"
#include "icons/generic.bmp.h"

// Due to a bug in the GFX library from Adafruit, these must be included *after*
// the include of display.h
#include "fonts/UbuntuMonoBold9pt8b.h"
#include "fonts/UbuntuMonoRegular9pt8b.h"
#include "fonts/UbuntuMonoBold22pt8b.h"
#include "fonts/UbuntuRegular9pt8b.h"
#include "fonts/UbuntuMedium9pt8b.h"
#include "fonts/UbuntuMedium11pt8b.h"
#include "fonts/UbuntuMedium12pt8b.h"
#include "fonts/UbuntuBold9pt8b.h"
#include "fonts/UbuntuBold12pt8b.h"

struct screen_t update_screen_data() {
  struct screen_t screen;

  HTTPClient client;
  std::array<weather_t, 4> forecasts = get_weather_forecast(&client);

  screen.current_weather = forecasts[0];
  screen.next_three_days_weather = { forecasts[1], forecasts[2], forecasts[3] };

  std::array<String, 11> lines{
    TUBE_LINE_DLR,
    TUBE_LINE_BAKERLOO,
    TUBE_LINE_CENTRAL,
    TUBE_LINE_CIRCLE,
    TUBE_LINE_DISTRICT,
    TUBE_LINE_HAMMERSMITH,
    TUBE_LINE_METROPOLITAN,
    TUBE_LINE_NORTHERN,
    TUBE_LINE_PICCADILLY,
    TUBE_LINE_VICTORIA,
    TUBE_LINE_JUBILEE,
  };
  screen.tube_status = tfl_get_status(&client, tfl_line_collection<11>(lines));

  return screen;
}

void draw_tfl_data(Display *display, screen_t screen) {
  // Tfl data sits in the left half of the screen
  uint16_t start_height = 85;

  std::map<String, String>::iterator it = screen.tube_status.begin();
  while (it != screen.tube_status.end()) {
    display->set_font(&UbuntuBold9pt8b);
    display->draw_text(it->first, 40, start_height);

    display->set_font(&UbuntuMedium9pt8b);
    display->draw_text(it->second, 225, start_height);
    start_height += 32;
    it++;
  }
}

typedef struct {
  Weather weather;
  const unsigned char *small;
  const unsigned char *large;
} BitmapMapEntry;

BWBitmap get_weather_icon(const Weather weather, const bool large) {
  const unsigned char *entry = NULL;
  const BitmapMapEntry entries[] = {
    { DRIZZLE, weather_bmp_drizzle_64px, weather_bmp_drizzle_128px }, // 0x01
    { RAIN, weather_bmp_rain_64px, weather_bmp_rain_128px }, // 0x02
    { OVERCAST_CLOUDS, weather_bmp_overcast_64px, weather_bmp_overcast_128px }, // 0x03
    { CLEAR_SKY, weather_bmp_clear_sky_64px, weather_bmp_clear_sky_128px }, // 0x04
    { BROKEN_CLOUDS, weather_bmp_overcast_64px, weather_bmp_overcast_128px }, // 0x05
    { SCATTERED_CLOUDS, weather_bmp_scattered_clouds_64px, weather_bmp_scattered_clouds_128px }, // 0x06
    { THUNDERSTORM, weather_bmp_thunderstorm_64px, weather_bmp_thunderstorm_128px }, // 0x07
    { FEW_CLOUDS, weather_bmp_few_clouds_64px, weather_bmp_few_clouds_128px }, // 0x08
    { SNOW, weather_bmp_snow_64px, weather_bmp_snow_128px }, // 0x09
    { INVALID, weather_bmp_unknown_64px, weather_bmp_unknown_128px }, // 0x0A
  };

  for (uint8_t i = 0; i < 0x0A; i++) {
    if (entries[i].weather == weather) {
      entry = large ? entries[i].large : entries[i].small;
      break;
    }
  }

  if (entry == NULL) {
    PANIC("Invalid weather request");
  }

  return BWBitmap(entry);
}

void draw_current_weather(Display *display, weather_t current) {
  BWBitmap bmp = get_weather_icon(current.weather, true);
  display->draw_bitmap(&bmp, 390, 10);

  BWBitmap temperature = BWBitmap(generic_bmp_thermometer_32px);
  BWBitmap humidity = BWBitmap(generic_bmp_humidity_32px);
  BWBitmap wind = BWBitmap(generic_bmp_wind_32px);

  display->draw_bitmap(&temperature, 570, 15);
  display->draw_bitmap(&humidity, 570, 51);
  display->draw_bitmap(&wind, 570, 85);

  display->set_font(&UbuntuMedium12pt8b);
  display->draw_text(String(current.min_temp_c) + "°", 610, 36);
  display->draw_text(String(current.humidity) + "%", 610, 72);
  display->draw_text(String(current.wind_speed) + "m/s", 610, 106);
}

void draw_secondary_weather(Display *display, std::array<weather_t,3> weather) {
  const weather_t tomorrow = weather[0];

  BWBitmap weather_icon = get_weather_icon(tomorrow.weather, false);
  BWBitmap temperature = BWBitmap(generic_bmp_thermometer_24px);
  BWBitmap humidity = BWBitmap(generic_bmp_humidity_24px);
  BWBitmap wind = BWBitmap(generic_bmp_wind_24px);

  display->set_font(&UbuntuMedium9pt8b);
  display->draw_bitmap(&weather_icon, 410, 140);

  display->draw_bitmap(&temperature, 490, 142);
  display->draw_text(String("Min: ") + String(tomorrow.min_temp_c) + "° / " +
                     String("Max: " ) + String(tomorrow.max_temp_c) + "°", 515, 159);

  display->draw_bitmap(&humidity, 490, 175);
  display->draw_text(String(tomorrow.humidity) + "%", 515, 190);

  display->draw_bitmap(&wind, 570, 175);
  display->draw_text(String(tomorrow.wind_speed) + "m/s", 599, 190);
}

void draw_date(Display *display, uint32_t now_utc_timestamp) {
  char buff[11]; // +1 for the `\0` terminator
  snprintf(buff, sizeof(buff), "%02d/%02d/%d",
    day(now_utc_timestamp), month(now_utc_timestamp), year(now_utc_timestamp));

  display->set_font(&UbuntuMonoBold22pt8b);
  display->draw_text(buff, 40, 45);
}

void update_display(Display *display, screen_t screen) {
  display->new_frame();

  // As a hack to avoid an extra http request (since there's no RTC on the
  // board), we use the UTC timestamp as returned by the weather call to infer
  // what day we're in. It will be wrong around midnight especially during DST
  // but it's meant to be used in London so will not be too far off normally.
  draw_date(display, screen.current_weather.unix_timestamp_utc);

  draw_tfl_data(display, screen);
  draw_current_weather(display, screen.current_weather);
  draw_secondary_weather(display, screen.next_three_days_weather);

  display->refresh();
}
