#ifndef __SCREEN_SETUP_H
#define __SCREEN_SETUP_H

#include "screens/base.h"

namespace Screen {
  class Setup : public BaseScreen {
    void setup(Display *);
    void unload() {};
    void handle() {};
  };
};

#endif // __SCREEN_SETUP_H
