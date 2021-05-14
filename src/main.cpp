#include <Arduino.h>

#include "pins.h"


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22, OUTPUT);
}

void loop() {
  Serial.println("YOOO");
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(22, HIGH);
  delay(200);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(22, LOW);
  delay(200);
}
