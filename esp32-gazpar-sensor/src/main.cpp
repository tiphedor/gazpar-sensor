/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include "secrets.h"

const char *mqttServer = "192.168.1.73";
const char *mqttUsername = "homeassistant";
const char *mqttClientName = "ESP32_gaz";
const int mqttPort = 1883;

const char *ssid = "bonjour";

int updateFrequencySeconds = 30;

WiFiClient espClient;
PubSubClient client(espClient);
bool valuePublished = false;

void goToSleep()
{
  client.disconnect();
  WiFi.disconnect();

  Serial.print("Going to sleep for ");
  Serial.print(updateFrequencySeconds);
  Serial.println(" seconds.");
  ESP.deepSleep(updateFrequencySeconds * 1000000);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("[ incoming mqtt payload ]");
  Serial.print("topic= ");
  Serial.println(topic);
  Serial.print("payload= ");

  // Isn't C fun ? if we don't do that we can an unterminated buffer which causes issues.
  // Yeah.
  char *payloadString = (char *)malloc(sizeof(char) * (length + 1));
  memcpy(payloadString, payload, length); // copy the data
  payloadString[length] = 0;              // terminate with \0
  Serial.println(payloadString);
  Serial.println();

  // We only care about the retained value from the broker, so filter other events.
  // Note: not really needed since we only subscribe to the one topic, but i'm keeping it to be safe.
  if (strcmp("energy/gaz_pulse_10_liters", topic) != 0)
  {
    return;
  }

  // Little (bad) trick here - we only want to send once, but since we're sending to a topic we've subscribed to, we need
  // to avoid an infinite loop situation.
  if (valuePublished)
  {
    return;
  }
  valuePublished = true;

  // Parse and increment the old value with the newly counted pulses
  int oldValue = atoi(payloadString);
  int newPulses = 5; // todo fetch from attiny
  int newValue = oldValue + newPulses;

  // We need a string, not an int. Yes, 64 is too much, sue me
  char bufferNewValue[64];
  sprintf(bufferNewValue, "%d", newValue);

  Serial.print("sending energy/gaz_pulse_10_liters with value: ");
  Serial.println(bufferNewValue);

  client.publish("energy/gaz_pulse_10_liters", bufferNewValue, true);
  goToSleep();
}

void setup()
{
  // Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();

  // WiFi
  Serial.print("Connecting to WiFi.");
  WiFi.begin(ssid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Ok!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // MQTT
  Serial.print("Connecting to MQTT server...");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  if (client.connect(mqttClientName, mqttUsername, mqttPassword))
  {
    Serial.println(" Ok!");

    // Subscribe to get the previous value
    client.subscribe("energy/gaz_pulse_10_liters");
  }
  else
  {
    Serial.println(" Error:(");
    Serial.println(client.state());
  }

  Serial.println();
  Serial.println();
  Serial.println();
}

void loop()
{
  // MQTT event handler
  client.loop();
}
