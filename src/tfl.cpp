#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <map>

#include "http.h"
#include "tfl.h"
#include "errors.h"

std::map<String, String> tfl_get_status(HTTPClient *client, String lines) {
  std::map<String, String> result;
  String api_url = String(__TFL_API_BASE_URL) + \
    "line/" + lines + "/status";

  DynamicJsonDocument json(1024 * 10);
  bool success = json_from_http(client, api_url.c_str(), &json);

  if (!success) return result;

  for (size_t i = 0; i < json.size(); i++) {
    result[json[i]["name"].as<String>()] = json[i]["lineStatuses"][0]["statusSeverityDescription"].as<String>();
  }

  return result;
}
