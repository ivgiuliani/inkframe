#include <Arduino.h>

#include "pins.h"
#include "Display.h"

Display display;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) { }

  Serial.println(F("inkframe started."));

  pinMode(LED_BUILTIN, OUTPUT);
  display.begin();
}

int idx = 1;
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("in loop");

  DisplayT *d = display.get_display();

  d->clearScreen();

  d->fillScreen(GxEPD_WHITE);
  d->print("Hello World!");
  d->printf("%d", idx++);
  d->nextPage();
  digitalWrite(LED_BUILTIN, LOW);

  delay(180000);
}
