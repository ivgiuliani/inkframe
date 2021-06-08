#include <Arduino.h>

#include "display.h"
#include "connectivity.h"
#include "rtc.h"
#include <buttonctrl.h>
#include "rotary_encoder.h"

#include "hw_global.h"

#include "screen_coordinator.h"

Display display;
Connectivity connectivity;

void handleInputTask(void *pvTaskArgs) {
  for(;;) {
    const ButtonEvent btn_ev = HW.button.handle();

    if (btn_ev != None) {
      Serial.printf("Button: %s\n", HW.button.str_from_event(btn_ev));
    }

    const int8_t offset = HW.rotary_encoder.read_offset();
    if (offset != 0) {
      Serial.printf("Rotary encoder: %d\n", offset);
    }

    YIELD(10);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) { }

  Serial.println(F("booting..."));

  pinMode(LED_BUILTIN, OUTPUT);
  HW.rtc.begin();
  HW.button.begin();
  HW.rotary_encoder.begin();

  display.begin();
  connectivity.attempt_wifi_connection();

  Serial.println(F("inkframe started."));

  xTaskCreatePinnedToCore(
    handleInputTask,
    "Handle input",
    4096,
    NULL,
    tskIDLE_PRIORITY,
    NULL,
    1
  );
}

void refresh() {
  digitalWrite(LED_BUILTIN, HIGH);
  auto sc = ScreenCoordinator(&display);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if (connectivity.update_state() == CI_CONNECTING) {
    // Let the builtin LED blink for 0.5s while connecting to the wifi, this way
    // it becomes easier to tell if the problem is with the wifi without having
    // to look at the serial output.
    digitalWrite(LED_BUILTIN, 
      (millis() % 1000) < 500 ? HIGH : LOW);
  }

  if (!connectivity.is_connected()) return;

  if (HW.rtc.needs_adjustment()) {
    SERIAL_DEBUG("RTC needs to be updated");
    HW.rtc.adjust();
  }

  refresh();

  Serial.println("about to go to sleep...");
  esp_sleep_enable_ext0_wakeup(PIN_RE_BUTTON, 0);
  esp_sleep_enable_timer_wakeup(1000000 * 60 * 10);
  esp_deep_sleep_start();
}
