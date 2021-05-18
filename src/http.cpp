#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "errors.h"

bool json_from_http(HTTPClient *client,
                    const char *url,
                    JsonDocument *json) {
  client->useHTTP10(true);
  client->begin(url);

  SERIAL_DEBUG((String("GET ") + url).c_str());

  const int http_code = client->GET();
  if (http_code < 0) {
    Serial.printf("GET failed, error: %s\n", client->errorToString(http_code).c_str());
    return false;
  }

  deserializeJson(*json, client->getStream());

  client->end();

  return true;
}
