#ifndef __RTC_H
#define __RTC_H

#include <RTClib.h>

#include "micro_utils.h"

#if !defined(NTP_POOL_1)
#  define NTP_POOL_1 "pool.ntp.org"
#endif

#if !defined(NTP_POOL_2)
#  define NTP_POOL_2 "time.google.com"
#endif

#if !defined(NTP_POOL_3)
#  define NTP_POOL_3 "time.nist.gov"
#endif

#if !defined(TIMEZONE_DEF)
#  error "Configure the timezone definition at build time"
#endif

class RTC {
public:
  RTC(const uint64_t update_interval_seconds = 60 * 60 * 24) :
    update_interval_seconds(update_interval_seconds) {}

  void begin();

  bool needs_adjustment();

  /**
   * A blocking call that queries NTP servers to update date and time.
   */
  bool adjust(const uint32_t timeout_ms = 10 * 1000);

  inline DateTime now() {
    return rtc.now();
  }

  inline float_t get_temperature() {
    // The DS312 comes with a temperature sensor builtin.
    return rtc.getTemperature();
  }

private:
  RTC_DS3231 rtc;
  const uint64_t update_interval_seconds;
};

#endif // __RTC_H
