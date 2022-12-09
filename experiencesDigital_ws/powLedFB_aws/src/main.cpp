#include <Arduino.h>
#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// main power supply pin
const int acPin = 13;
// ldr pin
// NOTE: CANNOT USE ADC2 AS WIFI MODE IS ENABLED
const int sensorPin = 36;
// power status
int powStatus;
// light status
int lightStatus;
// initial external light intensity value
int lightInit;
// current light intensity value
int lightVal;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["Power Status"] = powStatus;
  //doc["Light Status"] = lightStatus;
  doc["Light Status"] = lightStatus;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup()
{
  Serial.begin(115200);

  pinMode(acPin, INPUT);

  // reads initial intensity of external light
  lightInit = analogRead(sensorPin);

  connectAWS();
}

void loop()
{
  int pinState = digitalRead(acPin);
  lightVal = analogRead(sensorPin);

  if (pinState == HIGH)
  {
    powStatus = 1;
    // digitalWrite(logPin, LOW);
    Serial.println("Power ON");
  }
  else
  {
    powStatus = 0;
    // digitalWrite(logPin, HIGH);
    Serial.println("Power OFF");
  }

  // 300 is an experimental value 
  if ((lightVal) < 300)
  {
    Serial.print("Billboard ON");
    lightStatus = 1;
  }
  else
  {
    Serial.print("Billboard OFF");
    lightStatus = 0;
  }

  publishMessage();
  client.loop();
  delay(1000);
}