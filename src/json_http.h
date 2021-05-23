#ifndef __JSON_HTTP_H
#define __JSON_HTTP_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

// Set a relatively long connect timeout as the cost of attempting a
// reconnection is fairly high.
#if !defined(JSON_HTTP_CONNECT_TIMEOUT_MS)
#  define JSON_HTTP_CONNECT_TIMEOUT_MS 15000
#endif

#if !defined(JSON_HTTP_READ_TIMEOUT_MS)
#  define JSON_HTTP_READ_TIMEOUT_MS 10000
#endif

/** Utility method to return a parsed JSON back from an HTTP response.
 *
 * @param client an instance of the http client that will be used to execute the
 *        http request
 * @param url     the url to execute the request agains
 * @param json    a json document where the response will be parsed into
 * @param filter  a json document that describes the attribute to parse (NULL to parse the whole document)
 * @param retries how many attempts before we give up
 * @param retry_delay_backoff_ms base threshold for exponential backoff for retries
 * @see https://arduinojson.org/v6/how-to/deserialize-a-very-large-document/
 * @return true if the HTTP call returns successfully.
 */
bool json_from_http(HTTPClient *client, const char *url,
  JsonDocument *json, JsonDocument *filter = NULL,
  int32_t connect_timeout = JSON_HTTP_CONNECT_TIMEOUT_MS,
  int32_t read_timeout = JSON_HTTP_READ_TIMEOUT_MS,
  uint8_t retries = 3,
  uint32_t retry_delay_backoff_ms = 1000);

#endif // __JSON_HTTP_H
