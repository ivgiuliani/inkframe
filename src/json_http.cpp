#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "errors.h"
#include "micro_utils.h"

bool json_from_http(HTTPClient *client,
                    const char *url,
                    JsonDocument *json,
                    int32_t connect_timeout,
                    int32_t read_timeout,
                    uint8_t retries,
                    uint32_t retry_delay_backoff_ms) {
  for(uint8_t attempt = 1; attempt <= retries; attempt++) {
    client->useHTTP10(true);

    client->setConnectTimeout(connect_timeout);
    client->setTimeout(read_timeout);

    client->begin(url);

    SERIAL_DEBUG((String("GET ") + url).c_str());
    const int http_code = client->GET();
    if (http_code < 0 || http_code >= 400) {
      Serial.printf("GET failed (attempt %d), error: %d (%s)\n",
        attempt, http_code, client->errorToString(http_code).c_str());
      client->end();
      YIELD(attempt * retry_delay_backoff_ms);
      continue;
    }
    SERIAL_DEBUG((String("-> GET ") + url + " OK " + http_code).c_str());

    deserializeJson(*json, client->getStream());

    client->end();
    return true;
  }

  return false;
}
