#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "pins.h"

#define DISPLAY_ORIENT_LANDSCAPE 0
#define DISPLAY_ORIENT_PORTRAIT 1

#ifndef DisplayT
#  define DisplayT GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>
#endif

class Display {
public:
  void begin(uint32_t serial_baud_rate = 115200) {
    this->display = new DisplayT(GxEPD2_750_T7(
      SPI_CS,
      SPI_DC,
      SPI_RST,
      SPI_BUSY
    ));
    this->display->init(serial_baud_rate, true, 2); 

    // Release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
    SPI.end();

    // Map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15) - adjusted to the
    // recommended PIN settings from Waveshare - note that this is not the default
    // for most screens
    SPI.begin(13, 12, 14, 15);

    // Set some defaults and clear things out for the first page.
    set_display_defaults();
    this->display->firstPage();
  }

  inline void set_display_defaults() {
    this->display->setRotation(DISPLAY_ORIENT_PORTRAIT);
    this->display->fillScreen(GxEPD_WHITE);
    this->display->setTextColor(GxEPD_BLACK);
    this->display->setTextWrap(true);
  }

  inline DisplayT *get_display() {
    return this->display;
  }

private:
  DisplayT *display;
};


#endif // __DISPLAY_H
