#ifndef __TFL_H
#define __TFL_H

#include <Arduino.h>

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

String tfl_get_status(HTTPClient *client, String line);

#endif // __TFL_H
