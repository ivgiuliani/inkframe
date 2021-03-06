#ifndef __UI_H
#define __UI_H

#include <string>
#include <memory>
#include <Arduino.h>

#include "display.h"
#include "micro_utils.h"

#define __UI_CONSTRUCTOR(name) \
  UI##name(Display *display, const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) : \
  UIItem { display, x, y, w, h }

#define UI_FILL (-1)

typedef struct {
  int32_t x;
  int32_t y;
} ui_coords_t;

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

  __FORCE_INLINE uint16_t _x() { return x; }
  __FORCE_INLINE uint16_t _y() { return y; }
  __FORCE_INLINE uint16_t _w() { return w; }
  __FORCE_INLINE uint16_t _h() { return h; }

  /**
   * Given the offsets, it will return the position of the _new_ object in
   * absolute coordinates relative to the current UIItem.
   **/
  ui_coords_t calc_relative_position(int16_t x_offset, int16_t y_offset) {
    ui_coords_t coords { x, y };

    coords.x += x_offset;
    coords.y += y_offset;

    if (coords.x > display->width()) coords.x = display->width();
    if (coords.y > display->height()) coords.y = display->height();
    if (coords.x < 0) coords.x = 0;
    if (coords.y < 0) coords.y = 0;

    return coords;
  }

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
  std::shared_ptr<T> insert_relative(const int16_t x, const int16_t y,
                                     const int16_t w = UI_FILL, const int16_t h = UI_FILL) {
    static_assert(std::is_base_of<UIItem, T>::value, "T not derived from UIItem");

    ui_coords_t new_coords = calc_relative_position(x, y);

    const int16_t max_width = _x() + _w() - new_coords.x;
    const int16_t max_height = _y() + _h() - new_coords.y;
    const auto new_w = min(w == UI_FILL ? max_width : w, max_width);
    const auto new_h = min(h == UI_FILL ? max_height : h, max_height);

    auto t = std::make_shared<T>(display, new_coords.x, new_coords.y, new_w, new_h);

    add(t);
    return t;
  }

  template<class T> __FORCE_INLINE
  std::shared_ptr<T> insert_below(std::shared_ptr<UIItem> ref, int16_t below, const int16_t w = UI_FILL, const int16_t h = UI_FILL) {
    return insert_relative<T>(ref->_x(), ref->_y() + below, w, h);
  }

  template<class T> __FORCE_INLINE
  std::shared_ptr<T> insert_above(std::shared_ptr<UIItem> ref, int16_t above, const int16_t w = UI_FILL, const int16_t h = UI_FILL) {
    return insert_relative<T>(ref->_x(), ref->_y() - above, w, h);
  }

  template<class T> __FORCE_INLINE
  std::shared_ptr<T> insert_left_of(std::shared_ptr<UIItem> ref, int16_t left, const int16_t w = UI_FILL, const int16_t h = UI_FILL) {
    return insert_relative<T>(ref->_x() - left, ref->_y(), w, h);
  }

  template<class T> __FORCE_INLINE
  std::shared_ptr<T> insert_right_of(std::shared_ptr<UIItem> ref, int16_t right, const int16_t w = UI_FILL, const int16_t h = UI_FILL) {
    return insert_relative<T>(ref->_x() + right, ref->_y(), w, h);
  }

protected:
  void add(std::shared_ptr<UIItem> item) {
    children.push_back(item);
  }

private:
  std::vector<std::shared_ptr<UIItem>> children;
};

/**
 * A special kind of `UIBox` that will also refresh the linked display.
 */
class UIRoot : public UIBox {
public:
  UIRoot(Display *display) :
    UIItem(display, 0, 0, display->width(), display->height()),
    UIBox(display, 0, 0, display->width(), display->height()) {}

  void render() {
    display->new_frame();
    UIBox::render();
    display->refresh();
  }
};

class UITextBox : virtual public UIItem {
public:
  __UI_CONSTRUCTOR(TextBox) {}

  void set(String text, const GFXfont *font) {
    this->text = text;
    this->font = font;
  }

  void render() {
    DisplayT *gfx = display->get_display();

    gfx->setCursor(x, y);
    gfx->setFont(font);
    gfx->setTextColor(GxEPD_BLACK);
    gfx->print(text);
  }

private:
  const GFXfont *font = NULL;
  String text;
};

class UIMultilineTextBox : virtual public UIItem {
public:
  __UI_CONSTRUCTOR(MultilineTextBox) {}

  void set(String text, const GFXfont *font) {
    this->text = text;
    this->font = font;
  }

  void render() {
    display->set_font(font);
    display->draw_text(text, x, y, w);
  }

private:
  const GFXfont *font = NULL;
  String text;
};

class UIBitmap : virtual public UIItem {
public:
  __UI_CONSTRUCTOR(Bitmap) {}

  void set(const unsigned char *bmp) {
    bitmap = std::make_shared<BWBitmap>(bmp);
  }

  void render() {
    display->draw_bitmap(bitmap.get(), x, y);
  }

private:
  std::shared_ptr<BWBitmap> bitmap;
};

#endif // __UI_H
