#include <Arduino.h>

// this will assign the name PushButton to pin numer 13
const int acPin = 13;
// This Setup function is used to initialize everything
void setup()
{
  Serial.begin(115200);
  // This statement will declare pin 13 as digital input
  pinMode(acPin, INPUT);
}

void loop()

{
  int pinState = digitalRead(acPin);
  if (pinState == HIGH)
  {
    Serial.println("Power ON");
  }
  else
  {
    Serial.println("Power OFF");
  }
}