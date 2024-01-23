#include <WiFi.h>
#include <PubSubClient.h>


const char* mqttServer = "10.0.0.1";
const char* mqttUser = "CHANGE-ME";
const char* mqttPassword = "CHANGE-ME";
const int mqttPort = 1883;
const char* wifiNetwork = "CHANGE-ME";
const char* wifiPassword = "CHANGE-ME";
const int potPin = 34;

int potValue = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() 
{
  Serial.begin(9600);
  /*
  if(!wifiConnect(15000)) {
    Serial.println("Failed connect to Wifi");
    while(true){
        delay(2000);
    }
  }

  mqttClient.setServer(mqttServer, mqttPort);
  mqttReconnect();
  */
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
  potValue = analogRead(potPin);
  Serial.println(potValue);
  delay(50);
}
