#include <Arduino.h>

#include "Display.h"
#include "Connectivity.h"
#include "Bitmap.h"

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

void loop() {
  connectivity.update_state();
}
