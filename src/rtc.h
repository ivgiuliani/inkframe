#ifndef __RTC_H
#define __RTC_H

#include <RTClib.h>
#include <time.h>
#include <cstdlib>

#include "errors.h"
#include "micro_utils.h"

#define NTP_POOL_1 "pool.ntp.org"
#define NTP_POOL_2 "time.google.com"
#define NTP_POOL_3 "time.nist.gov"

#if !defined(TIMEZONE_DEF)
#  error "Configure the timezone definition at build time"
#endif

class RTC {
public:
  void begin() {
    if (!rtc.begin()) WARN("Couldn't find RTC");
  }

  inline bool needs_update() {
    return rtc.lostPower() || rtc.now().year() <= 2000;
  }
  
  /**
   * A blocking call that queries NTP servers to update date and time.
   */
  void adjust(uint32_t timeout_ms = 10 * 1000) {
    configTime(0, 0, NTP_POOL_1, NTP_POOL_2, NTP_POOL_3);
    setenv("TZ", TIMEZONE_DEF, 1);
    tzset();

    time_t now;
    struct tm time_info;

    // Only block for the very first update that would return a totally wrong
    // time in the 1900 (but still honour the timeout).
    const uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &time_info);
      YIELD(100);
    } while (((millis() - start) <= timeout_ms) && (time_info.tm_year < (2021 - 1900)));

    rtc.adjust(DateTime(
      time_info.tm_year + 1900, // Years since 1900
      time_info.tm_mon + 1, // Surprisingly, this is 0 indexed...
      time_info.tm_mday,
      time_info.tm_hour,
      time_info.tm_min,
      time_info.tm_sec));
  }

  inline DateTime now() {
    return rtc.now();
  }

  inline float_t get_temperature() {
    // The DS312 comes with a temperature sensor builtin.
    return rtc.getTemperature();
  }

private:
  RTC_DS3231 rtc;
};

#endif // __RTC_H
