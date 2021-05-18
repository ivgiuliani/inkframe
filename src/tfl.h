#ifndef __TFL_H
#define __TFL_H

#include <Arduino.h>
#include <map>

#define __TFL_API_BASE_URL "https://api.tfl.gov.uk/"

#define TUBE_LINE_BAKERLOO "bakerloo"
#define TUBE_LINE_CENTRAL "central"
#define TUBE_LINE_CIRCLE "circle"
#define TUBE_LINE_DISTRICT "district"
#define TUBE_LINE_DLR "dlr"
#define TUBE_LINE_HAMMERSMITH "hammersmith-city"
#define TUBE_LINE_JUBILEE "jubilee"
#define TUBE_LINE_PICCADILLY "piccadilly"
#define TUBE_LINE_METROPOLITAN "metropolitan"
#define TUBE_LINE_NORTHERN "northern"
#define TUBE_LINE_VICTORIA "victoria"
#define TUBE_LINE_WATERLOO "waterloo-city"

template <std::size_t N> String tfl_line_collection(std::array<String, N> lines) {
  String collection("");

  for (uint8_t i = 0; i < N; i++) {
    collection += lines[i];
    if (i < (N - 1)) {
      collection += ",";
    }
  }

  return collection;
}

std::map<String, String> tfl_get_status(HTTPClient *, String);

#endif // __TFL_H
