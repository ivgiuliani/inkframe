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

#if defined(ESP32)
#define PANIC(msg) { \
  Serial.println(F("!!! PANIC !!!")); \
  Serial.print(msg); \
  Serial.println(); \
  Serial.flush(); \
  PANIC_RESTART(); \
}
#else
#define PANIC(msg)
#endif

#if defined(ESP32)
#  define WARN(msg) { Serial.print(F("[WARN] ")); Serial.println(msg); Serial.flush(); }
#else
#  define WARN(msg)
#endif

#if defined(ENABLE_SERIAL_DEBUG) && defined(ESP32)
#  define SERIAL_DEBUG(msg) { Serial.print(F("[DEBUG] ")); Serial.println(msg); }
#else
#  define SERIAL_DEBUG(msg)
#endif

#endif // __ERRORS_H
