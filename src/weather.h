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

  // This will stop working in 2038...
  uint32_t unix_timestamp_utc;
};

#define __OPENWEATHERMAP_API_BASE_URL "https://api.openweathermap.org/data/2.5"

// Returns the current weather
weather_t get_current_weather_forecast(HTTPClient *client);

// Returns the forecasts for the next 3 days
std::array<weather_t,3> get_daily_weather_forecast(HTTPClient *client);

const char *weather_to_description(Weather weather);

#endif // __WEATHER_H
