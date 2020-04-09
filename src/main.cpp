#include <Arduino.h>

#define R1_PIN A2
#define R2_PIN A3
#define VIBRO_PIN 2
#define SW1_PIN 5
#define SW2_PIN 10

void setup() {
  Serial.begin (19200); //  9600, 115200
  pinMode(A1, OUTPUT), digitalWrite(A1, LOW); // use as GND
  pinMode(9, OUTPUT), digitalWrite(9, HIGH); // use as VCC

  pinMode(R1_PIN, INPUT_PULLUP);
  pinMode(R2_PIN, INPUT);
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(SW1_PIN, OUTPUT);
  pinMode(SW2_PIN, OUTPUT);
}

void loop() {
  Serial.print(analogRead(R1_PIN)); // 235 — 397; max 1024
  Serial.print(" ");
  Serial.print(analogRead(R2_PIN));
  Serial.println();

  delay(200);
}
