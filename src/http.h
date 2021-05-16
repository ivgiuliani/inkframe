#ifndef __HTTP_H
#define __HTTP_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

// Utility method to return a parsed JSON back from an HTTP response.
//
// @param client an instance of the http client that will be used to execute the
//        http request
// @param url    the url to execute the request agains
// @param json   a json document where the response will be parsed into
// @return true if the HTTP call returns successfully.
bool json_from_http(HTTPClient *client, const char *url, JsonDocument *json);

#endif // __HTTP_H
