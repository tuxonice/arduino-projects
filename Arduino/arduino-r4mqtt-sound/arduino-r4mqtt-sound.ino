#include <ArduinoMqttClient.h>
#include <WiFiS3.h>

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

#define PIN_ANALOG_IN A0

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  analogReadResolution(14); //change to 12-bit resolution
//
  wifiConnect();
//   mqttConnect();  
}

void loop() {

  int value = 0;
  float avg = 0.0;
  float values[1000];
  String bucket = "{";


  
for(int j=0;j<100;j++) {

  // sample for 500ms
  for(int i=1;i<=5;i++) {
    value += analogRead(PIN_ANALOG_IN);  
    delay(100);
  }

  avg = value/5.0;

  bucket.concat(avg);
  if(j!=99) {
    bucket.concat(",");  
  }
  
  
  Serial.print("Status: ");
  Serial.println(avg);

  value = 0;
  avg = 0.0;
}

bucket.concat("}");
  
Serial.print(bucket);

//wifiConnect();
//delay(5000);
//mqttConnect();  

//mqttClient.poll();
//mqttClient.beginMessage(MQTT_TOPIC);
//mqttClient.print(bucket.c_str());
//mqttClient.endMessage();  


delay(2000);
//mqttClient.stop();
//WiFi.disconnect();


  
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  /*
  mqttClient.poll();
    
   
  mqttClient.beginMessage(MQTT_TOPIC);
  mqttClient.print(avg);
  mqttClient.endMessage();
  */

}

void wifiConnect() {

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
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

void mqttConnect() {
  
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(MQTT_HOST);

  mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);
  if (!mqttClient.connect(MQTT_HOST, MQTT_PORT)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
}
