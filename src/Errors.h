#ifndef __ERRORS_H
#define __ERRORS_H

#if defined(ESP32)
# include <Arduino.h>
#endif

#if !defined(PANIC_DELAY)
# define PANIC_DELAY (5000)
#endif

#if defined(ESP32)
# define PANIC_RESTART() { \
  delay(PANIC_DELAY); \
  ESP.restart(); \
}
#else
# define PANIC_RESTART()
#endif

#define PANIC(msg) { \
  Serial.println(F("!!! PANIC !!!")); \
  Serial.print(msg); \
  Serial.println(); \
  PANIC_RESTART(); \
}

#if defined(ENABLE_SERIAL_DEBUG)
#  define SERIAL_DEBUG(msg) { Serial.print(F("DEBUG: ")); Serial.println(msg); }
#else
#  define SERIAL_DEBUG(msg)
#endif

#endif // __ERRORS_H
