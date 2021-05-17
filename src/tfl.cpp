#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "http.h"
#include "tfl.h"
#include "Errors.h"

String tfl_api_status_path(String line) {
  if (line.equals(TUBE_LINE_DLR)) {
    // DLR is the only "mode" supported, the rest is all "lines"
    return "line/mode/";
  }
  return "line/";
}

String tfl_get_status(HTTPClient *client, String line) {
  String api_url = String(__TFL_API_BASE_URL) + \
    tfl_api_status_path(line) +
    String(line) + "/status";

  DynamicJsonDocument json(2048);
  bool success = json_from_http(client, api_url.c_str(), &json);

  if (!success) return "";

  return json[0]["lineStatuses"][0]["statusSeverityDescription"].as<String>();
}
