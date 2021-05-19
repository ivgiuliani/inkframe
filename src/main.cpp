#include <Arduino.h>

#include "display.h"
#include "connectivity.h"

#include "screen_composer.h"

Display display;
Connectivity connectivity;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) { }

  Serial.println(F("booting..."));

  pinMode(LED_BUILTIN, OUTPUT);
  display.begin();
  connectivity.attempt_wifi_connection();

  Serial.println(F("inkframe started."));
}

void refresh() {
  digitalWrite(LED_BUILTIN, HIGH);
  screen_t screen_details = update_screen_data();
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

  refresh();

  Serial.println("about to go to sleep...");
  esp_sleep_enable_timer_wakeup(1000000 * 60 * 10);
  esp_deep_sleep_start();
}
