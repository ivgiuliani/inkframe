#include <Arduino.h>
#include <HTTPClient.h>
#include <TimeLib.h>

#include "weather.h"
#include "tfl.h"
#include "screen_composer.h"
#include "display.h"
#include "rtc.h"
#include "wikipedia.h"
#include "micro_utils.h"
#include "hw_global.h"
#include "ui.h"

#include "icons/weather.bmp.h"
#include "icons/generic.bmp.h"

// Due to a bug in the GFX library from Adafruit, these must be included *after*
// the include of display.h
#include "fonts/UbuntuMonoBold9pt8b.h"
#include "fonts/UbuntuMonoRegular9pt8b.h"
#include "fonts/UbuntuMonoBold20pt8b.h"
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

  screen.current_time = HW.rtc.now();
  screen.home_temperature = std::lround(HW.rtc.get_temperature());

  screen.current_weather = forecasts[0];
  screen.next_three_days_weather = { forecasts[1], forecasts[2], forecasts[3] };

  std::array<String, 12> lines{
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
    TUBE_LINE_WATERLOO,
  };
  screen.tube_status = tfl_get_status(&client, tfl_line_collection<12>(lines));

  screen.wikipedia_entry = wikipedia_get_onthisday(&client,
    HW.rtc.now().month(), HW.rtc.now().day()
  );

  return screen;
}

  // Tfl data sits in the left half of the screen
void draw_tfl_data(UIBox *root, screen_t screen) {
  auto status_box = root->insert_relative<UIBox>(42, 80, 440, 1000);

  uint16_t start_height = 0;

  std::map<String, String>::iterator it = screen.tube_status.begin();
  while (it != screen.tube_status.end()) {
    auto line_name = status_box->insert_relative<UITextBox>(0, start_height);
    auto line_status = status_box->insert_relative<UITextBox>(185, start_height);

    line_name->set_font(&UbuntuBold9pt8b);
    line_name->set_text(it->first);

    line_name->set_font(&UbuntuMedium9pt8b);
    line_status->set_text(it->second);

    start_height += 34;
    it++;
  }
}

typedef struct {
  const Weather weather;
  const unsigned char *small_day;
  const unsigned char *large_day;
  const unsigned char *small_night;
  const unsigned char *large_night;
} BitmapMapEntry;

BWBitmap get_weather_icon(const Weather weather, const bool large, const bool night) {
  const unsigned char *entry = NULL;
  const BitmapMapEntry entries[] = {
    { DRIZZLE,
      weather_bmp_drizzle_64px, weather_bmp_drizzle_128px,
      weather_bmp_drizzle_64px, weather_bmp_drizzle_128px
    }, // 0x01
    { RAIN,
      weather_bmp_rain_64px, weather_bmp_rain_128px,
      weather_bmp_rain_64px, weather_bmp_rain_128px
    }, // 0x02
    { OVERCAST_CLOUDS,
      weather_bmp_overcast_64px, weather_bmp_overcast_128px,
      weather_bmp_overcast_64px, weather_bmp_overcast_128px
    }, // 0x03
    { CLEAR_SKY,
      weather_bmp_clear_sky_64px, weather_bmp_clear_sky_128px,
      weather_bmp_night_clear_sky_64px, weather_bmp_night_clear_sky_128px
    }, // 0x04
    { BROKEN_CLOUDS,
      weather_bmp_overcast_64px, weather_bmp_overcast_128px,
      weather_bmp_overcast_64px, weather_bmp_overcast_128px
    }, // 0x05
    { SCATTERED_CLOUDS,
      weather_bmp_scattered_clouds_64px, weather_bmp_scattered_clouds_128px,
      weather_bmp_night_scattered_clouds_64px, weather_bmp_night_scattered_clouds_128px
    }, // 0x06
    { THUNDERSTORM,
      weather_bmp_thunderstorm_64px, weather_bmp_thunderstorm_128px,
      weather_bmp_thunderstorm_64px, weather_bmp_thunderstorm_128px
    }, // 0x07
    { FEW_CLOUDS,
      weather_bmp_few_clouds_64px, weather_bmp_few_clouds_128px,
      weather_bmp_night_few_clouds_64px, weather_bmp_night_few_clouds_128px
    }, // 0x08
    { SNOW,
      weather_bmp_snow_64px, weather_bmp_snow_128px,
      weather_bmp_snow_64px, weather_bmp_snow_128px
    }, // 0x09
    { INVALID,
      weather_bmp_unknown_64px, weather_bmp_unknown_128px,
      weather_bmp_unknown_64px, weather_bmp_unknown_128px
    }, // 0x0A
  };

  for (uint8_t i = 0; i < 0x0A; i++) {
    if (entries[i].weather == weather) {
      if (night) {
        entry = large ? entries[i].large_night : entries[i].small_night;
      } else {
        entry = large ? entries[i].large_day : entries[i].small_day;
      }
      break;
    }
  }

  if (entry == NULL) {
    PANIC("Invalid weather request");
  }

  return BWBitmap(entry);
}

void draw_current_weather(Display *display, screen_t screen, weather_t current) {
  const bool night_variant = screen.current_time.hour() >= 19 || screen.current_time.hour() <= 4;
  BWBitmap bmp = get_weather_icon(current.weather, true, night_variant);
  display->draw_bitmap(&bmp, 390, 20);

  BWBitmap home_temperature = BWBitmap(generic_bmp_house_thermometer_32px);
  BWBitmap temperature = BWBitmap(generic_bmp_thermometer_32px);
  BWBitmap humidity = BWBitmap(generic_bmp_humidity_32px);
  BWBitmap wind = BWBitmap(generic_bmp_wind_32px);

  display->draw_bitmap(&home_temperature, 560, 15);
  display->draw_bitmap(&temperature, 560, 51);
  display->draw_bitmap(&humidity, 560, 85);
  display->draw_bitmap(&wind, 560, 119);

  display->set_font(&UbuntuMedium12pt8b);
  display->draw_text(String(screen.home_temperature) + "°", 605, 36);
  display->draw_text(String(current.min_temp_c) + "°", 605, 72);
  display->draw_text(String(current.humidity) + "%", 605, 108);
  display->draw_text(String(current.wind_speed) + "m/s", 605, 144);
}

void draw_secondary_weather(Display *display, std::array<weather_t,3> weather) {
  const weather_t tomorrow = weather[0];

  BWBitmap weather_icon = get_weather_icon(tomorrow.weather, false, false);
  BWBitmap temperature = BWBitmap(generic_bmp_thermometer_24px);
  BWBitmap humidity = BWBitmap(generic_bmp_humidity_24px);
  BWBitmap wind = BWBitmap(generic_bmp_wind_24px);

  display->set_font(&UbuntuBold9pt8b);
  display->draw_text("Tomorrow", 390, 185);

  display->set_font(&UbuntuMedium9pt8b);
  display->draw_bitmap(&weather_icon, 405, 200);

  display->draw_bitmap(&temperature, 490, 197);
  display->draw_text(String("Min: ") + String(tomorrow.min_temp_c) + "° / " +
                     String("Max: " ) + String(tomorrow.max_temp_c) + "°", 515, 214);

  display->draw_bitmap(&humidity, 490, 235);
  display->draw_text(String(tomorrow.humidity) + "%", 515, 250);

  display->draw_bitmap(&wind, 570, 235);
  display->draw_text(String(tomorrow.wind_speed) + "m/s", 599, 250);
}

void draw_date(UIBox *root, uint32_t now_utc_timestamp) {
  char buff[11]; // +1 for the `\0` terminator
  snprintf(buff, sizeof(buff), "%02d/%02d/%d",
    day(now_utc_timestamp), month(now_utc_timestamp), year(now_utc_timestamp));

  auto textbox = root->insert_relative<UITextBox>(40, 45);
  textbox->set_font(&UbuntuMonoBold20pt8b);
  textbox->set_text(buff);
}

void draw_wikipedia(Display *display, struct wikipedia_onthisday_t on_this_day) {
  String header = String("Happened on this day in ");
  if (on_this_day.year >= 0) {
    header += String(on_this_day.year);
  } else {
    header += String(-on_this_day.year) + " B.C";
  }
  display->set_font(&UbuntuBold9pt8b);
  display->draw_text(header, 390, 300);

  display->set_font(&UbuntuMedium9pt8b);
  display->draw_text(on_this_day.text.c_str(), 390, 325, 280);
}

void update_display(Display *display, screen_t screen) {
  UIBox root(display, 0, 0, display->width(), display->height());
  display->new_frame();

  draw_date(&root, screen.current_time.unixtime());

  draw_tfl_data(&root, screen);
  draw_current_weather(display, screen, screen.current_weather);
  draw_secondary_weather(display, screen.next_three_days_weather);
  draw_wikipedia(display, screen.wikipedia_entry);

  root.render();
  display->refresh();
}
