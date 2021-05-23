#include <RTClib.h>
#include <time.h>
#include <cstdlib>
#include <esp_sntp.h>

#include "rtc.h"
#include "errors.h"
#include "micro_utils.h"

// This will force an NTP update after every cold boot.
RTC_DATA_ATTR uint64_t RTC_LAST_UPDATE = 0;

void RTC::begin() {
  if (!rtc.begin()) WARN("Couldn't find RTC");
}

bool RTC::needs_adjustment() {
  #if defined(ENABLE_SERIAL_DEBUG)
    Serial.printf("Last RTC update: %llu\n", RTC_LAST_UPDATE);
  #endif
  return rtc.lostPower() ||
    rtc.now().year() <= 2000 ||
    (rtc.now().unixtime() - RTC_LAST_UPDATE) > update_interval_seconds;
}

/**
 * A blocking call that queries NTP servers to update date and time.
 */
bool RTC::adjust(const uint32_t timeout_ms) {
  time_t now;
  struct tm time_info;

  sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
  configTime(0, 0, NTP_POOL_1, NTP_POOL_2, NTP_POOL_3);
  setenv("TZ", TIMEZONE_DEF, 1);
  tzset();

  // Best effort at blocking while the update is in progress.
  const uint64_t start = millis();
  bool sync_complete = false;
  do {
    time(&now);
    localtime_r(&now, &time_info);
    sync_complete = sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED;
    if (!sync_complete) {
      YIELD(100);
    }
  } while (((millis() - start) <= timeout_ms) && !sync_complete);

  rtc.adjust(DateTime(
    time_info.tm_year + 1900, // Years since 1900
    time_info.tm_mon + 1, // Surprisingly, this is 0 indexed...
    time_info.tm_mday,
    time_info.tm_hour,
    time_info.tm_min,
    time_info.tm_sec));

  if (sync_complete) {
    RTC_LAST_UPDATE = rtc.now().unixtime();
  }

  return sync_complete;
}
