#include <Arduino.h>

#include "screens/setup.h"

#include "fonts/UbuntuMonoBold20pt8b.h"
#include "ui.h"

void Screen::Setup::setup(Display *display) {
  UIRoot root(display);

  auto setup_header = root.insert_relative<UITextBox>(300, 220);
  setup_header->set("Welcome", &UbuntuMonoBold20pt8b);

  root.render();
}

