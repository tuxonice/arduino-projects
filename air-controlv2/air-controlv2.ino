/* Air control V1.0 */
#include <Wire.h>
#include <WiFi.h>
#include "SparkFunCCS811.h"
#include <PubSubClient.h>
#include "DHT.h"

// Update these with values suitable for your network.
const char* ssid = "CHANGE-ME";
const char* password = "CHANGE-ME";
const char* mqtt_server = "10.0.0.1";
#define mqtt_port 1883
#define MQTT_USER "CHANGE-ME"
#define MQTT_PASSWORD "CHANGE-ME"
#define MQTT_PUBLISH_TOPIC_TEMPERATURE "/toss/air/quality/temperature"
#define MQTT_PUBLISH_TOPIC_HUMIDITY "/toss/air/quality/humidity"
#define MQTT_PUBLISH_TOPIC_ECO2 "/toss/air/quality/eco2"
#define MQTT_PUBLISH_TOPIC_TVOC "/toss/air/quality/tvoc"
#define MQTT_PUBLISH_TOPIC_STATUS "/toss/air/quality/status"
#define CCS811_ADDR 0x5B //Default I2C Address
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// DHT Sensor
uint8_t DHTPin = 4;
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

WiFiClient wifiClient;

PubSubClient client(wifiClient);
CCS811 airSensor(CCS811_ADDR);

struct AirQuality {
   int  eCO2 = -1; // Equivalent carbon dioxide
   int  tVOC = -1; // Total Volatile Organic Compounds 
};

struct EnvironmentState {
   float  temperature = 25; // Temperature (ºC)
   float  humidity = 50; // Humidity value (%)
};


void setup() {
  //-- DHT22 --
  pinMode(DHTPin, INPUT);
  dht.begin();
  //-- CCS811 --
  Serial.println("CCS811 Basic Example");
  Wire.begin(); //Inialize I2C Hardware
  if (airSensor.begin() == false) {
    Serial.print("CCS811 error. Please check wiring. Freezing...");
    while (1) {
        delay(1000);
    }
  }
  //-- MQTT --  
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish(MQTT_PUBLISH_TOPIC_STATUS, "Air monitoring is online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 10 seconds before retrying
      delay(10000);
    }
  }
}


void loop() {
  char eCO2[12];
  char tVOC[12];
  char temperature[12];
  char humidity[12];
  
  struct EnvironmentState environmentState = getEnvironmentState();
  struct AirQuality airQuality = getAirQuality(environmentState.humidity, environmentState.temperature);
  client.loop();

  Serial.print("Temperature:");
  Serial.println(environmentState.temperature);
  Serial.print("Humidity:");
  Serial.println(environmentState.humidity);

  Serial.print("eCO2:");
  Serial.println(airQuality.eCO2);
  Serial.print("TVOC:");
  Serial.println(airQuality.tVOC);

  sprintf(eCO2, "%d", airQuality.eCO2);
  sprintf(tVOC, "%d", airQuality.tVOC);
  sprintf(temperature, "%.02f", environmentState.temperature);
  sprintf(humidity, "%.02f", environmentState.humidity);
  
  publishSerialData(MQTT_PUBLISH_TOPIC_TEMPERATURE, temperature);
  publishSerialData(MQTT_PUBLISH_TOPIC_HUMIDITY, humidity);
  publishSerialData(MQTT_PUBLISH_TOPIC_ECO2, eCO2);
  publishSerialData(MQTT_PUBLISH_TOPIC_TVOC, tVOC);
  delay(60000); // Wait 60s
}

struct EnvironmentState getEnvironmentState() {
  
  struct EnvironmentState environmentState;
   
  // Gets the values of the temperature
  environmentState.temperature = dht.readTemperature(); 
  // Gets the values of the humidity
  environmentState.humidity = dht.readHumidity(); 
  
  return environmentState;
}

struct AirQuality getAirQuality(float humidity, float temperature) {
    
    struct AirQuality airQuality;
    
    airSensor.setEnvironmentalData(humidity, temperature);
    Serial.println("Environmental data applied!");
    airSensor.readAlgorithmResults(); //Dump a reading and wait
    delay(2000);
    
    if (airSensor.dataAvailable())
    {
        //If so, have the sensor read and calculate the results.
        airSensor.readAlgorithmResults();
        //Returns calculated CO2 reading
        airQuality.eCO2 = airSensor.getCO2();
        //Returns calculated TVOC reading
        airQuality.tVOC = airSensor.getTVOC();
    } else if (airSensor.checkForStatusError())
    {
      //If the CCS811 found an internal error, print it.
      printSensorError();
    }
    return airQuality;
}

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void publishSerialData(char * topic, char * serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(topic, serialData);
}


// printSensorError gets, clears, then prints the errors
// saved within the error register.
void printSensorError()
{
  uint8_t error = airSensor.getErrorRegister();

  if (error == 0xFF) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
  }
  else
  {
    Serial.print("Error: ");
    if (error & 1 << 5)
      Serial.print("HeaterSupply");
    if (error & 1 << 4)
      Serial.print("HeaterFault");
    if (error & 1 << 3)
      Serial.print("MaxResistance");
    if (error & 1 << 2)
      Serial.print("MeasModeInvalid");
    if (error & 1 << 1)
      Serial.print("ReadRegInvalid");
    if (error & 1 << 0)
      Serial.print("MsgInvalid");
    Serial.println();
  }
}
