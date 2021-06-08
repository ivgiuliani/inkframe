#ifndef __SCREEN_COORDINATOR_H
#define __SCREEN_COORDINATOR_H

#include <memory>

#include "micro_utils.h"
#include "screens/base.h"
#include "screens/dashboard.h"
#include "screens/setup.h"

typedef enum {
  SETUP = 0,
  DASHBOARD = 1,
} inkframe_screen;

class ScreenCoordinator {
public:
  ScreenCoordinator(Display *display, inkframe_screen initial_screen = DASHBOARD) {
    this->display = display;
    change_screen(initial_screen);
  }

  void change_screen(inkframe_screen new_screen) {
    auto old_screen = this->screen;

    switch(new_screen) {
      case SETUP:
        screen = std::make_shared<Screen::Setup>();
        break;
      case DASHBOARD:
        screen = std::make_shared<Screen::Dashboard>();
        break;
    }

    current_screen = new_screen;
    screen->setup(display);
    if (old_screen) {
      old_screen->unload();
    }
  }

  void handle() {
    screen->handle();
  }

  __FORCE_INLINE inkframe_screen get_current_screen() const {
    return current_screen;
  }

private:
  Display *display;
  inkframe_screen current_screen = DASHBOARD;
  std::shared_ptr<BaseScreen> screen{};
};

#endif // __SCREEN_COORDINATOR_H
