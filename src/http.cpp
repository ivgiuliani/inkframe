#include <HTTPClient.h>
#include <ArduinoJson.h>

bool json_from_http(HTTPClient *client,
                    const char *url,
                    JsonDocument *json) {
  client->useHTTP10(true);
  client->begin(url);

  const int http_code = client->GET();
  if (http_code < 0) {
    Serial.printf("GET failed, error: %s\n", client->errorToString(http_code).c_str());
    return false;
  }

  deserializeJson(*json, client->getStream());

  client->end();

  return true;
}
