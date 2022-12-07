#include <Arduino.h>

// contants for the pins where sensors are plugged into.
const int sensorPin = 2;

// set up some global variables for the light level an initial value.
int lightInit;  // initial value
int lightVal;   // light reading

void setup()
{
  Serial.begin(115200);
  // reads initial intensity of external light 
  lightInit = analogRead(sensorPin); 
}

void loop()
{
  lightVal = analogRead(sensorPin); // read the current light levels

  if((lightVal - lightInit) < 50)
    Serial.print("Billboard ON");
  else
    Serial.print("Billboard OFF");

  // Serial.print(lightVal);
  Serial.print("\n");

  /*
  // if lightVal is less than our initial reading withing a threshold then it is dark.
  if(lightVal - lightInit <  50)
  {
      digitalWrite (ledPin, HIGH); // turn on light
  }
  //otherwise, it is bright
  else
  {
    digitalWrite (ledPin, LOW); // turn off light
  }
  */
 delay(2000);
}