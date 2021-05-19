#include "weather.h"

#include <HTTPClient.h>
#include "json_http.h"
#include "micro_utils.h"
#include "errors.h"

bool owm_call_api(HTTPClient *client, const char *excludes, DynamicJsonDocument *json) {
  String api_url = String(__OPENWEATHERMAP_API_BASE_URL) + \
    "/onecall?lang=en&units=metric" + \
    "&exclude=" + String(excludes) + \
    "&lat=" + String(QUOTE(WEATHER_LAT)) + \
    "&lon=" + String(QUOTE(WEATHER_LON)) + \
    "&appid=" + String(QUOTE(OPENWEATHERMAP_API_KEY));

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

// Return an array of 4 items: the first element is the current weather state,
// the other 3 are the *next* 3 days.
std::array<weather_t, 4> get_weather_forecast(HTTPClient *client) {
  std::array<weather_t, 4> forecasts;
  DynamicJsonDocument json(1024 * 20);
  weather_t tmp;

  bool success = owm_call_api(client, "hourly,minutely,alerts", &json);
  if (!success) {
    tmp.forecast = ForecastType::UNAVAILABLE;
    forecasts[0] = tmp; forecasts[1] = tmp; forecasts[2] = tmp; forecasts[3] = tmp;
    return forecasts;
  } else {
    // Parse the "current" weather
    forecasts[0].forecast = CURRENT;
    forecasts[0].unix_timestamp_utc = json["current"]["dt"].as<uint32_t>();
    forecasts[0].min_temp_c = json["current"]["temp"].as<float>();
    forecasts[0].max_temp_c = forecasts[0].min_temp_c;
    forecasts[0].weather = owm_from_weather_id(json["current"]["weather"][0]["id"].as<uint16_t>());

    // OWM will return a 0-indexed array of days, where 0 is "today", but for
    // "today" we look at the "current" object instead as it's more relevant
    // since "today" refers to the average weather of the day.
    for (uint8_t day = 1; day <= 3; day++) {
      forecasts[day].forecast = DAILY;
      forecasts[day].unix_timestamp_utc = json["daily"][day]["dt"].as<uint32_t>();
      forecasts[day].min_temp_c = std::round(json["daily"][day]["temp"]["min"].as<float_t>());
      forecasts[day].max_temp_c = std::round(json["daily"][day]["temp"]["max"].as<float_t>());
      forecasts[day].weather = owm_from_weather_id(json["daily"][day]["weather"][0]["id"].as<uint16_t>());
    }
  }

  return forecasts;
}

const char *weather_to_description(Weather weather) {
  switch(weather) {
    case CLEAR_SKY:
      return "clear sky";
    case FEW_CLOUDS:
      return "few clouds";
    case SCATTERED_CLOUDS:
      return "scattered clouds";
    case BROKEN_CLOUDS:
      return "broken clouds";
    case OVERCAST_CLOUDS:
      return "overcast";
    case DRIZZLE:
      return "drizzle";
    case RAIN:
      return "rain";
    case THUNDERSTORM:
      return "thunderstorm";
    case SNOW:
      return "snow";
    case MIST:
      return "mist";
    case INVALID:
    default:
      return "invalid";
  }
}
