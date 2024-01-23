#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "secrets.h"

const char* mqttServer = MQTT_HOST;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASS;
const int mqttPort = MQTT_PORT;
const char* wifiNetwork = SECRET_SSID;
const char* wifiPassword = SECRET_PASS;

const int adcPin = 34;
char adcValue[10];

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() 
{
  Serial.begin(115200);
  if(!wifiConnect(15000)) {
    Serial.println("Failed connect to Wifi");
    while(true){
      delay(2000);
    }
  }

  mqttClient.setServer(mqttServer, mqttPort);
  if(!mqttConnect()) {
    Serial.println("Failed connect to MQTT");
    while(true){
      delay(2000);
    }
  }
}

bool mqttConnect() 
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
  mqttClient.publish(MQTT_TOPIC, "test");
  delay(10000);
  //readVoltage();
}

void readVoltage() 
{
    int value = 0, sumValue = 0, average;
    Serial.println("---");
    for (int i = 1; i <= 30; i++) {
      value = analogRead(adcPin);
      // Serial.println(value);
      sumValue += value;
      delay(2000);
    }

    average = round((float)sumValue/60.0);
    
    sprintf(adcValue, "%d", average);
    Serial.println(adcValue);
    mqttClient.publish(MQTT_TOPIC, adcValue);
}
