#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "secrets.h"

#define ONE_WIRE_BUS 32
const char* mqttServer = MQTT_HOST;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASS;
const int mqttPort = MQTT_PORT;
const char* wifiNetwork = SECRET_SSID;
const char* wifiPassword = SECRET_PASS;
float lastValue = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() 
{
  sensors.begin();
  Serial.begin(115200);
  if(!wifiConnect(15000)) {
    Serial.println("Failed connect to Wifi");
    while(true){
        delay(2000);
    }
  }

  mqttClient.setServer(mqttServer, mqttPort);
  mqttReconnect();
}

bool mqttReconnect() 
{
  if (!mqttClient.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (!mqttClient.connect(clientId.c_str(),mqttUser,mqttPassword)) {
      Serial.print("Fail MQTT connection...");
      Serial.println("failed, rc=" + mqttClient.state());
      return false;
    }
  }
  return true;
}

bool mqttPublish(char * topic, char * serialData)
{
  if (!mqttReconnect()) {
      return false;
  }
  mqttClient.publish(topic, serialData);
  return true;
}

bool wifiConnect(int timeout)
{
    unsigned long startTime = millis();
    // Connect to WI-FI
    Serial.print("Connecting to ");
    Serial.println(wifiNetwork);
    // connect to your local wi-fi network
    WiFi.begin(wifiNetwork, wifiPassword);
    // check wi-fi is connected to wi-fi network
    while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      if((millis() - startTime) > timeout) {
        WiFi.disconnect(); //Not sure if we can do this here
        return false;
      }
    }
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
    randomSeed(micros());
    return true;
}


void loop()
{
  char temperature[10];
  float currentValue = 0;
  sensors.requestTemperatures();
  currentValue = sensors.getTempCByIndex(0);
  sprintf(temperature, "%.02f", currentValue);
  //Serial.print("Celsius temperature: ");
  //Serial.println(temperature);
  
  if(abs(lastValue - currentValue) >= 0.2) {
    mqttPublish(MQTT_TOPIC, temperature);  
    lastValue = currentValue;
  }
  delay(60000);
}
