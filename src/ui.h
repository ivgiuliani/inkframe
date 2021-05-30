#ifndef __UI_H
#define __UI_H

#include <string>
#include <memory>
#include <Arduino.h>

#include "display.h"

#define __UI_CONSTRUCTOR(name) \
  UI##name(Display *display, const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) : \
  UIItem { display, x, y, w, h }

class UIItem {
public:
  // Coordinates for a UIItem are always absolute.
  UIItem(Display *display,
         const uint16_t x,
         const uint16_t y,
         const uint16_t w,
         const uint16_t h) :
    display(display), x(x), y(y), w(w), h(h) {}

  virtual void render();
protected:
  Display *display;
  const uint16_t x, y, w, h;
};

class UIBox : virtual public UIItem {
public:
  __UI_CONSTRUCTOR(Box) {}

  void render() {
    for (auto it = children.begin(); it != children.end(); it++) {
      std::shared_ptr<UIItem> item = *it;
      item->render();
    }
  }

  template<class T>
  std::shared_ptr<T> insert_relative(const int16_t x, const int16_t y, const int16_t w, const int16_t h) {
    static_assert(std::is_base_of<UIItem, T>::value, "T not derived from UIItem");

    int16_t new_x = this->x + x;
    int16_t new_y = this->y + y;

    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x >= display->width()) new_x = display->width();
    if (new_y >= display->height()) new_y = display->height();

    const int16_t max_width = new_x - display->width();
    const int16_t max_height = new_y - display->height();
    const auto new_w = min(w, max_width);
    const auto new_h = min(h, max_height);

    // TODO: clip width/height if it exceeds display boundaries

    auto t = std::make_shared<T>(display, new_x, new_y, new_w, new_h);

    add(t);
    return t;
  }

protected:
  void add(std::shared_ptr<UIItem> item) {
    children.push_back(item);
  }

private:
  std::vector<std::shared_ptr<UIItem>> children;
};

class UITextBox : virtual public UIItem {
public:
  __UI_CONSTRUCTOR(TextBox) {}

  void set_text(std::string text) {
    this->text = String(text.c_str());
  }

  void set_text(String text) {
    this->text = text;
  }

  void set_text(const char *text) {
    this->set_text(std::string(text));
  }

  void set_font(const GFXfont *font) {
    this->font = font;
  }

  void render() {
    if (font != NULL) {
      // TODO: we should always enforce an explicit font choice
      display->set_font(font);
    }
    display->draw_text(text, x, y);
  }

private:
  const GFXfont *font = NULL;
  String text;
};

#endif // __UI_H
