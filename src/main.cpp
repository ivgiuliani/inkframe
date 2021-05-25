#include <Arduino.h>

#include "display.h"
#include "connectivity.h"
#include "rtc.h"
#include <buttonctrl.h>
#include "rotary_encoder.h"

#include "screen_composer.h"

Display display;
Connectivity connectivity;
RTC rtc;
ButtonCtrl<PIN_RE_BUTTON, HIGH, INPUT_PULLUP> button;
RotaryEncoder<PIN_RE_CLK, PIN_RE_DT> rotary_encoder;

void handleInputTask(void *pvTaskArgs) {
  for(;;) {
    const ButtonEvent btn_ev = button.handle();

    if (btn_ev != None) {
      Serial.printf("Button: %s\n", button.str_from_event(btn_ev));
    }

    const int8_t offset = rotary_encoder.read_offset();
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
  rtc.begin();
  display.begin();
  button.begin();
  rotary_encoder.begin();
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
  screen_t screen_details = update_screen_data(&rtc);
  update_display(&display, screen_details);
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

  if (rtc.needs_adjustment()) {
    SERIAL_DEBUG("RTC needs to be updated");
    rtc.adjust();
  }

  refresh();

  Serial.println("about to go to sleep...");
  esp_sleep_enable_ext0_wakeup(PIN_RE_BUTTON, 0);
  esp_sleep_enable_timer_wakeup(1000000 * 60 * 10);
  esp_deep_sleep_start();
}
