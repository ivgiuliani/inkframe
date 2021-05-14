#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#include "pins.h"

#define DISPLAY_ORIENT_LANDSCAPE 0
#define DISPLAY_ORIENT_PORTRAIT 1

#ifndef DisplayT
   // Can be overridden at compile time, but by default it configures the
   // display to be a black&white Waveshare 7.5 inches (800x480).
   // https://www.waveshare.com/wiki/7.5inch_e-Paper_HAT
#  define __DisplayType GxEPD2_750_T7
#  define DisplayT GxEPD2_BW<__DisplayType, __DisplayType::HEIGHT>
#endif

class Display {
public:
  void begin(uint32_t serial_baud_rate = 115200, uint reset_duration = 2) {
    // Reconfigure SPI pins to point at SCK(13), MISO(12), MOSI(14) and SS(15)
    // as that's the configuration recommended from Waveshare for ESP32s.
    SPI.end();
    SPI.begin(13, 12, 14, 15);

    this->display = new DisplayT(__DisplayType(SPI_CS, SPI_DC, SPI_RST, SPI_BUSY));
    this->display->init(serial_baud_rate, true, reset_duration);

    // Set some defaults and clear things out for the initial boot.
    new_frame();
    this->display->firstPage();
  }

  // Sets some decent defaults for an empty new frame
  inline void new_frame() {
    this->display->setCursor(0, 0);
    this->display->setRotation(DISPLAY_ORIENT_PORTRAIT);
    this->display->fillScreen(GxEPD_WHITE);
    this->display->setTextColor(GxEPD_BLACK);
    this->display->setFullWindow();
    this->display->setPartialWindow(
      0, 0, this->display->width(), this->display->height()
    );

    // Text wrap is disabled by default because if something wraps it messes
    // up everything else, so in that case we try to minimise the broken
    // display artefacts.
    this->display->setTextWrap(false);
  }

  inline DisplayT *get_display() {
    return this->display;
  }

private:
  DisplayT *display;
};


#endif // __DISPLAY_H
