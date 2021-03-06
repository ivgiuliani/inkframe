#include <ArduinoJson.h>

#include "wikipedia.h"
#include "string_utils.h"
#include "json_http.h"

std::string remove_newlines(std::string in) {
  std::string out;
  for (auto it = in.begin(); it != in.end(); it++) {
    if (*it != '\n' && *it != '\r') out += *it;
  }
  return out;
}

struct wikipedia_onthisday_t wikipedia_get_onthisday(HTTPClient *client,
                                                     const uint8_t month,
                                                     const uint8_t day) {
  struct wikipedia_onthisday_t result = { 0, "" };

  // https://arduinojson.org/v6/how-to/deserialize-a-very-large-document/
  StaticJsonDocument<64> filter;
  filter["events"][0]["year"] = true;
  filter["events"][0]["text"] = true;

  String api_url = WIKIPEDIA_ON_THIS_DAY_BASE_URI +
    String(month) + "/" + String(day);

  DynamicJsonDocument json(1024 * 10);
  JsonHttp http(client, api_url.c_str());
  bool success = http.execute(&json, &filter);

  if (!success) return result;

  const uint16_t all_events_count = json["events"].size();
  uint8_t attempt = 0;

  // TODO: remove/increase char limit if mounted on a larger panel
  do {
    const uint16_t idx = random(all_events_count);
    result.year = json["events"][idx]["year"].as<int16_t>();
    result.text = json["events"][idx]["text"].as<std::string>();
    result.text = remove_newlines(ascii_remap(&result.text));
  } while (++attempt < 20 && result.text.length() > 150);

  return result;
}
