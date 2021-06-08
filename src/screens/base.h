#ifndef __SCREEN_BASE_H
#define __SCREEN_BASE_H

#include "display.h"

class BaseScreen {
public:
  virtual void setup(Display *);
  virtual void unload();
  virtual void handle();
};

#endif // __SCREEN_BASE_H
