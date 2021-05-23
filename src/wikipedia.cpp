#include <ArduinoJson.h>

#include "wikipedia.h"
#include "string_utils.h"
#include "json_http.h"

boolean replace_invalid_chars(String *text) {
  ascii_extended_remap(text);
  for (uint16_t i = 0; i < text->length(); i++) {
    if ((*text)[i] > 128) {
      return false;
    }
  }
  return true;
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
  bool success = json_from_http(client, api_url.c_str(), &json, &filter);

  if (!success) return result;

  const uint16_t all_events_count = json["events"].size();
  uint8_t attempt = 0;

  // TODO: remove/increase char limit if mounted on a larger panel
  do {
    const uint16_t idx = random(all_events_count);
    result.year = json["events"][idx]["year"].as<uint16_t>();
    result.text = json["events"][idx]["text"].as<String>();
  } while (attempt++ < 5 && !replace_invalid_chars(&result.text) && result.text.length() <= 130);

  return result;
}
