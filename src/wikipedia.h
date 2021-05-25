#ifndef __WIKIPEDIA_H
#define __WIKIPEDIA_H

#include <HTTPClient.h>
#include <string>

#define WIKIPEDIA_ON_THIS_DAY_BASE_URI "https://en.wikipedia.org/api/rest_v1/feed/onthisday/events/"

struct wikipedia_onthisday_t {
  int16_t year;
  std::string text;
};

struct wikipedia_onthisday_t wikipedia_get_onthisday(
  HTTPClient *,
  const uint8_t month,
  const uint8_t day
);

#endif // __WIKIPEDIA_H
