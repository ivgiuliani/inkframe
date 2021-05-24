#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <GxEPD2_BW.h>
#include <string>
#include <vector>

#include "string_utils.h"
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
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);

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

  void draw_text(String text,
                 const uint16_t start_x,
                 const uint16_t start_y,
                 const int16_t max_width = -1,
                 const uint8_t v_padding = 3) {
    if (max_width > 0) {
      const int16_t max_x = min(display->width(), static_cast<int16_t>(max_width));
      std::vector<std::string> words = split_words(from_arduino_str(&text));
      const auto space_width = text_width(" ");
      uint16_t line_y = start_y;
      String line;

      // TODO: special case one long word case
      uint16_t space_left = max_x;
      for (uint16_t i = 0; i < words.size(); i++) {
        const String word = String(words[i].c_str());
        auto new_width = (text_width(word) + space_width);

        if (new_width <= space_left) {
          line += word + " ";
          space_left -= new_width;
        } else {
          // Exhausted space in the current line, print whatever is in the buffer
          // and move to the next line.
          display->setCursor(start_x, line_y);
          display->print(line);

          line_y += text_height(line) + v_padding;
          line = word + " ";
          space_left = max_x - (text_width(word) + space_width);
        }
      }

      if (!line.isEmpty()) {
        display->setCursor(start_x, line_y);
        display->print(line);
      }
    } else {
      display->setCursor(start_x, start_y);
      display->print(text);
    }
  }

  inline uint16_t text_width(String s) const {
    int16_t bound_x, bound_y;
    uint16_t bound_w, bound_h;
    display->getTextBounds(s, 0, 0, &bound_x, &bound_y, &bound_w, &bound_h);
    return bound_w;
  }

  inline uint16_t text_height(String s) const {
    int16_t bound_x, bound_y;
    uint16_t bound_w, bound_h;
    display->getTextBounds(s, 0, 0, &bound_x, &bound_y, &bound_w, &bound_h);
    return bound_h;
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
