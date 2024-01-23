#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


// DHT Sensor
uint8_t DHTPin = 4; 
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(DHTPin, INPUT);

  dht.begin();
}
void loop() {
  
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity
  Serial.print("Temperature:");
  Serial.println(Temperature);
  Serial.print("Humidity:");
  Serial.println(Humidity);
  delay(10000);
}
