#include <Arduino.h>
#include <HTTPClient.h>

#include "weather.h"
#include "tfl.h"
#include "screen_composer.h"
#include "Display.h"

#include "icons/weather.bmp.h"

// Due to a bug in the GFX library from Adafruit, these must be included *after*
// the include of display.h
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


struct screen_t update_screen_data() {
  struct screen_t screen;

  HTTPClient client;
  screen.current_weather = get_current_weather_forecast(&client);
  screen.next_three_days_weather = get_daily_weather_forecast(&client);

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
  uint16_t height = 20;

  display->set_font(&FreeSansBold12pt7b);
  std::map<String, String>::iterator it = screen.tube_status.begin();
  while (it != screen.tube_status.end()) {
    display->draw_text(it->first, 5, height);
    display->draw_text(it->second, 190, height);
    height += 32;
    it++;
  }
}

Bitmap get_weather_icon_large(Weather weather) {
  switch (weather) {
    case DRIZZLE:
      return Bitmap(weather_bmp_drizzle_large, true);
    case RAIN:
      return Bitmap(weather_bmp_rain_large, true);
    case OVERCAST_CLOUDS:
      return Bitmap(weather_bmp_overcast_large, true);
    case CLEAR_SKY:
      return Bitmap(weather_bmp_clear_sky_large, true);
    case BROKEN_CLOUDS:
      return Bitmap(weather_bmp_overcast_large, true);
    case SCATTERED_CLOUDS:
      return Bitmap(weather_bmp_scattered_clouds_large, true);
    case THUNDERSTORM:
      return Bitmap(weather_bmp_thunderstorm_large, true);
    case FEW_CLOUDS:
      return Bitmap(weather_bmp_few_clouds_large, true);
    case SNOW:
      return Bitmap(weather_bmp_snow_large, true);
    case INVALID:
    default:
      return Bitmap(weather_bmp_unknown_large, true);
  }
}

Bitmap get_weather_icon_small(Weather weather) {
  switch (weather) {
    case DRIZZLE:
      return Bitmap(weather_bmp_drizzle_small, true);
    case RAIN:
      return Bitmap(weather_bmp_rain_small, true);
    case OVERCAST_CLOUDS:
      return Bitmap(weather_bmp_overcast_small, true);
    case CLEAR_SKY:
      return Bitmap(weather_bmp_clear_sky_small, true);
    case BROKEN_CLOUDS:
      return Bitmap(weather_bmp_overcast_small, true);
    case SCATTERED_CLOUDS:
      return Bitmap(weather_bmp_scattered_clouds_small, true);
    case THUNDERSTORM:
      return Bitmap(weather_bmp_thunderstorm_small, true);
    case FEW_CLOUDS:
      return Bitmap(weather_bmp_few_clouds_small, true);
    case SNOW:
      return Bitmap(weather_bmp_snow_small, true);
    case INVALID:
    default:
      return Bitmap(weather_bmp_unknown_small, true);
  }
}

void draw_current_weather(Display *display, weather_t current) {
  Bitmap bmp = get_weather_icon_large(current.weather);
  display->draw_bitmap(&bmp, 400, 10);

  display->set_font(&FreeMonoBold24pt7b);
  display->draw_text(String(current.min_temp_c) + String("C"), 550, 60);

  display->set_font(&FreeSansBold12pt7b);
  display->draw_text(weather_to_description(current.weather), 550, 100);
}

void draw_next_3_days_weather(Display *display, std::array<weather_t,3> weather) {
  for (uint8_t i = 0; i < 3; i++) {
    Bitmap bmp = get_weather_icon_small(weather[i].weather);

    display->draw_bitmap(&bmp, 420 + (i * 110), 150);

    display->set_font(&FreeMono9pt7b);
    const String temp_ind = String(weather[i].min_temp_c) + "/" + String(weather[i].max_temp_c) + String("C");
    display->draw_text(temp_ind, 425 + (i * 110), 240);
  }
}

void update_display(Display *display, screen_t screen) {
  display->new_frame();

  draw_tfl_data(display, screen);
  draw_current_weather(display, screen.current_weather);
  draw_next_3_days_weather(display, screen.next_three_days_weather);

  display->refresh();
}
