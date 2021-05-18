#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "Display.h"
#include "Connectivity.h"
#include "Bitmap.h"

#include "screen_composer.h"

Display display;
Connectivity connectivity;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) { }

  delay(1000);
  Serial.println(F("booting..."));

  pinMode(LED_BUILTIN, OUTPUT);
  display.begin();
  connectivity.attempt_wifi_connection();

  Serial.println(F("inkframe started."));
}

bool executed = false;
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  connectivity.update_state();

  if (!connectivity.is_connected()) return;
  if (executed) return;

  screen_t screen_details = update_screen_data();
  update_display(&display, screen_details);

  executed = true;

  Serial.println("about to go to sleep...");
  digitalWrite(LED_BUILTIN, LOW);

  esp_sleep_enable_timer_wakeup(1000000 * 60 * 10);
  esp_deep_sleep_start();
}
