#ifndef __JSON_HTTP_H
#define __JSON_HTTP_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "errors.h"
#include "micro_utils.h"

// Set a relatively long connect timeout as the cost of attempting a
// reconnection is fairly high.
#if !defined(JSON_HTTP_CONNECT_TIMEOUT_MS)
#  define JSON_HTTP_CONNECT_TIMEOUT_MS 15000
#endif

#if !defined(JSON_HTTP_READ_TIMEOUT_MS)
#  define JSON_HTTP_READ_TIMEOUT_MS 10000
#endif

typedef struct {
  int http_status_code;
  bool success;
} __json_http_response;

class JsonHttp {
public:
  int32_t connect_timeout = JSON_HTTP_CONNECT_TIMEOUT_MS;
  int32_t read_timeout = JSON_HTTP_READ_TIMEOUT_MS;
  uint8_t retries = 3;
  uint32_t retry_delay_backoff_ms = 1000;

  JsonHttp(HTTPClient *client, const char *url) :
    client(client), url(url) {}

  ~JsonHttp() {
    client = NULL;
  }

  bool execute(JsonDocument *json, JsonDocument *filter = NULL) {
    __json_http_response response;
    for(uint8_t attempt = 1; attempt <= retries; attempt++) {
      response = request(json, filter);

      if (!response.success) {
        Serial.printf("GET failed (attempt %d), error: %d (%s)\n",
          attempt,
          response.http_status_code,
          client->errorToString(response.http_status_code).c_str()
        );

        YIELD(attempt * retry_delay_backoff_ms);

        continue;
      }

      return true;
    }

    return false;
  }

private:
  HTTPClient *client;
  const char *url;

  __json_http_response request(JsonDocument *json, JsonDocument *filter = NULL) {
    __json_http_response result;

    client->useHTTP10(true);
    client->setConnectTimeout(connect_timeout);
    client->setTimeout(read_timeout);
    client->begin(url);

    SERIAL_DEBUG((String("GET ") + url).c_str());
    const int http_code = client->GET();
    if (http_code < 0 || http_code >= 400) {
      result.http_status_code = http_code;
      result.success = false;

      client->end();

      return result;
    }
    SERIAL_DEBUG((String("-> GET ") + url + " OK " + http_code).c_str());

    if (filter == NULL) {
      deserializeJson(*json, client->getStream());
    } else {
      deserializeJson(*json, client->getStream(),
                      DeserializationOption::Filter(*filter));
    }

    result.http_status_code = http_code;
    result.success = true;

    client->end();

    return result;
  }
};

#endif // __JSON_HTTP_H
