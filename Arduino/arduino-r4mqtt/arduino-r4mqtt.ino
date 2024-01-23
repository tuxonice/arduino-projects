#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = MQTT_HOST;
int        port     = MQTT_PORT;
const char topic[]  = MQTT_TOPIC;

const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void initSerial() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void connectWifi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("You're connected to the network");
}

void connectMqtt() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  mqttClient.setUsernamePassword(MQTT_USER,MQTT_PASS);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
}

void disconnectWifi() {
  Serial.println("Disconnecting!");
  while(wifiClient.connected())
  {
     wifiClient.flush();
     Serial.print(".");
  }
  
  wifiClient.stop();
  Serial.println("Disconnected!");
}

void disconnectMqtt() {
  Serial.println("Disconnecting!");
  mqttClient.stop();
}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  initSerial();
  connectWifi();
  connectMqtt();
}

void loop() {
    delay(5000);
    Serial.println("-------------START----------");
  
    mqttClient.poll();
    int value = (int)random(1, 100);   
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(value);
    mqttClient.beginMessage(topic);
    mqttClient.print(value);
    mqttClient.endMessage();
}
