#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <map>

#include "json_http.h"
#include "tfl.h"
#include "errors.h"

std::map<String, String> tfl_get_status(HTTPClient *client, String lines) {
  std::map<String, String> result;
  String api_url = String(__TFL_API_BASE_URL) + \
    "line/" + lines + "/status";

  StaticJsonDocument<64> filter;
  filter[0]["name"] = true;
  filter[0]["lineStatuses"] = true;

  DynamicJsonDocument json(1024 * 10);
  JsonHttp http(client, api_url.c_str());
  bool success = http.execute(&json, &filter);

  if (!success) return result;

  for (size_t i = 0; i < json.size(); i++) {
    result[json[i]["name"].as<String>()] = json[i]["lineStatuses"][0]["statusSeverityDescription"].as<String>();
  }

  return result;
}
