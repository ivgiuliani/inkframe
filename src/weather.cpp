#include "weather.h"

#include <HTTPClient.h>
#include "http.h"
#include "micro_utils.h"
#include "Errors.h"

bool owm_call_api(HTTPClient *client, const char *excludes, DynamicJsonDocument *json) {
  String api_url = String(__OPENWEATHERMAP_API_BASE_URL) + \
    "/onecall?lang=en&units=metric" + \
    "&exclude=" + String(excludes) + \
    "&lat=" + String(QUOTE(WEATHER_LAT)) + \
    "&lon=" + String(QUOTE(WEATHER_LON)) + \
    "&appid=" + String(QUOTE(OPENWEATHERMAP_API_KEY));

  SERIAL_DEBUG(api_url);

  return json_from_http(client, api_url.c_str(), json);
}

Weather owm_from_weather_id(const int id) {
  // https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
  switch((int)(id / 100)) {
    case 2:
      return THUNDERSTORM;
    case 3:
      return DRIZZLE;
    case 5:
      return RAIN;
    case 6:
      return SNOW;
    case 7:
      return MIST;
    case 8:
      switch (id) {
        case 800:
          return CLEAR_SKY;
        case 801:
          return FEW_CLOUDS;
        case 802:
          return SCATTERED_CLOUDS;
        case 803:
          return BROKEN_CLOUDS;
        case 804:
          return OVERCAST_CLOUDS;
      }
      break;
  }

  return Weather::INVALID;
}

weather_t get_current_weather_forecast(HTTPClient *client) {
  weather_t forecast;
  DynamicJsonDocument json(4096);

  bool success = owm_call_api(client, "hourly,daily,minutely,alerts", &json);
  if (!success) {
    forecast.forecast = ForecastType::UNAVAILABLE;
    return forecast;
  } else {
    forecast.forecast = CURRENT;

    forecast.unix_timestamp_utc = json["current"]["dt"].as<uint32_t>();
    forecast.min_temp_c = json["current"]["temp"].as<float>();
    forecast.max_temp_c = forecast.min_temp_c;
    forecast.weather = owm_from_weather_id(json["current"]["weather"][0]["id"].as<uint16_t>());
  }

  return forecast;
}

std::array<weather_t,3> get_daily_weather_forecast(HTTPClient *client) {
  std::array<weather_t,3> forecasts;
  DynamicJsonDocument json(4096);

  bool success = owm_call_api(client, "current,hourly,minutely,alerts", &json);
  if (!success) {
    weather_t tmp;
    tmp.forecast = ForecastType::UNAVAILABLE;
    forecasts[0] = tmp;
    forecasts[1] = tmp;
    forecasts[2] = tmp;
    return forecasts;
  } else {
    // OWM will return a 0-indexed array of days, where 0 is today
    for (uint8_t day = 1; day <= 3; day++) {
      forecasts[day - 1].forecast = DAILY;
      forecasts[day - 1].unix_timestamp_utc = json["daily"][day]["dt"].as<uint32_t>();

      forecasts[day - 1].min_temp_c = std::round(json["daily"][day]["temp"]["min"].as<float_t>());
      forecasts[day - 1].max_temp_c = std::round(json["daily"][day]["temp"]["max"].as<float_t>());
      forecasts[day - 1].weather = owm_from_weather_id(json["daily"][day]["weather"][0]["id"].as<uint16_t>());
    }
  }
  return forecasts;
}
