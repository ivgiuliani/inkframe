#ifndef __JSON_HTTP_H
#define __JSON_HTTP_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

// Set a relatively long connect timeout as the cost of attempting a
// reconnection is fairly high.
#if !defined(JSON_HTTP_CONNECT_TIMEOUT)
#  define JSON_HTTP_CONNECT_TIMEOUT 15000
#endif

#if !defined(JSON_HTTP_READ_TIMEOUT)
#  define JSON_HTTP_READ_TIMEOUT 10000
#endif

/** Utility method to return a parsed JSON back from an HTTP response.
 *
 * @param client an instance of the http client that will be used to execute the
 *        http request
 * @param url    the url to execute the request agains
 * @param json   a json document where the response will be parsed into
 * @return true if the HTTP call returns successfully.
 */
bool json_from_http(HTTPClient *client, const char *url, JsonDocument *json,
  int32_t connect_timeout = JSON_HTTP_CONNECT_TIMEOUT,
  int32_t read_timeout = JSON_HTTP_READ_TIMEOUT);

#endif // __JSON_HTTP_H
