#ifndef __WEATHER_H
#define __WEATHER_H

#if !defined(OPENWEATHERMAP_API_KEY)
#  error "Configure the OpenWeatherMap API key at build time"
#endif

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

enum Weather {
  CLEAR_SKY,
  FEW_CLOUDS,
  SCATTERED_CLOUDS,
  BROKEN_CLOUDS,
  OVERCAST_CLOUDS,
  DRIZZLE,
  RAIN,
  THUNDERSTORM,
  SNOW,
  MIST,

  INVALID,
};

enum ForecastType {
  UNAVAILABLE,
  CURRENT,
  HOURLY,
  DAILY,
};

struct weather_t {
  Weather weather;
  ForecastType forecast;

  // Will be the same for both min and max for 'current' forecasts.
  int8_t min_temp_c;
  int8_t max_temp_c;

  int8_t wind_speed;

  // uint8_t should be enough, but who knows what will happen with climate change
  int16_t humidity;

  // This will stop working in 2038...
  uint32_t unix_timestamp_utc;
};

#define __OPENWEATHERMAP_API_BASE_URL "https://api.openweathermap.org/data/2.5"

std::array<weather_t, 4> get_weather_forecast(HTTPClient *client);

const char *weather_to_description(Weather weather);

#endif // __WEATHER_H
