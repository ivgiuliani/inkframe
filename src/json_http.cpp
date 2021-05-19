#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "errors.h"

bool json_from_http(HTTPClient *client,
                    const char *url,
                    JsonDocument *json,
                    int32_t connect_timeout,
                    int32_t read_timeout) {
  client->useHTTP10(true);

  client->setConnectTimeout(connect_timeout);
  client->setTimeout(read_timeout);

  client->begin(url);

  SERIAL_DEBUG((String("GET ") + url).c_str());
  const int http_code = client->GET();
  if (http_code < 0 || http_code >= 400) {
    Serial.printf("GET failed, error: %d (%s)\n", http_code, client->errorToString(http_code).c_str());
    client->end();
    return false;
  }
  SERIAL_DEBUG((String("-> GET ") + url + " OK " + http_code).c_str());

  deserializeJson(*json, client->getStream());

  client->end();

  return true;
}