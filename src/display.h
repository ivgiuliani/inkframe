#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <GxEPD2_BW.h>

#include "bw_bitmap.h"
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
  void begin(const uint32_t serial_baud_rate = 115200, const uint16_t reset_duration = 2) {
    // Reconfigure SPI pins to point at SCK(13), MISO(12), MOSI(14) and SS(15)
    // as that's the configuration recommended from Waveshare for ESP32s.
    SPI.end();
    SPI.begin(13, 12, 14, 15);

    this->display = new DisplayT(__DisplayType(SPI_CS, SPI_DC, SPI_RST, SPI_BUSY));

    // Seems like the only valid configuration for the Waveshare board I have
    // is init(115200, true, 2, false) as recommended by GxEDP2.
    this->display->init(serial_baud_rate, true, reset_duration, false);

    // Set some defaults and clear things out for the initial boot.
    new_frame();
  }

  // Sets some decent defaults for an empty new frame
  inline void new_frame(const uint8_t orientation = DISPLAY_ORIENT_LANDSCAPE) const {
    display->setCursor(0, 0);
    display->setRotation(orientation);
    display->fillScreen(GxEPD_WHITE);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();
    display->setFont(NULL);
    display->setPartialWindow(
      0, 0, display->width(), display->height()
    );

    // Text wrap is disabled by default because if something wraps it messes
    // up everything else, so in that case we try to minimise the broken
    // display artefacts.
    this->display->setTextWrap(false);
  }

  inline DisplayT *get_display() const {
    return this->display;
  }

  void set_font(const GFXfont *font) const {
    display->setFont(font);
  }

  void draw_text(String text, const uint16_t start_x, const uint16_t start_y) const {
    display->setCursor(start_x, start_y);
    display->print(text);
  }

  // Renders the given bitmap at the chosen coordinates.
  // start_x, start_y: top left corner
  void draw_bitmap(BWBitmap *bmp,
                   const int16_t start_x,
                   const uint16_t start_y) const {
    for (uint32_t y = start_y, image_y = 0;
         y < display->height() && image_y < bmp->height();
         y++, image_y++) {
      for (uint32_t x = start_x, image_x = 0;
           x < display->width() && image_x < bmp->width();
           x++, image_x++) {
        display->drawPixel(x, y, bmp->read_pixel(image_x, image_y) > 0 ? GxEPD_WHITE : GxEPD_BLACK);
      }
    }
  }

  inline void refresh() const {
    while(display->nextPage()) {};
  }

private:
  DisplayT *display;
};


#endif // __DISPLAY_H
