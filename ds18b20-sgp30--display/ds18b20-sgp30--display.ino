#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_SGP30.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define DHTPIN 7     // DHT pin
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SGP30 sgp;
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

// Setup a oneWire instance to communicate with any OneWire devices (DS18b20)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

float hum;  //Stores humidity value
float temp; //Stores temperature value
int sgpCounter = 0;


void initSGP30() {
  if (! sgp.begin()){
    printMessage("SGP30 sensor not found :(");
    while (1);
  }
  printMessage("Found SGP30");
  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  // sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!
}


void initDHT22() {
  dht.begin();
  printMessage("Found DHT22");
}

void initDS18B20() {
  sensors.begin();
  printMessage("Found DS18B20");
}

void initSSD1306() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  printMessage("Found SSD1306");
}


void readDS18b20() {
  sensors.requestTemperatures(); 
  float temperature = sensors.getTempCByIndex(0);
  display.clearDisplay();
  displayValue("DS ", temperature, " C", 1);
}

void readDHT22() {
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  display.clearDisplay();
  displayValue("DHT ", hum, " %", 1);
  displayValue("DHT ", temp, " C", 2);
}

void readSGP30() {
  sgp.setHumidity(getAbsoluteHumidity(temp, hum));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  
  display.clearDisplay();
  displayValue("SGP ", sgp.TVOC, " (TVOC)", 1);
  displayValue("SGP ", sgp.eCO2, " (eCO2)", 2);
  delay(5000);
  
  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }
  
  display.clearDisplay();
  displayValue("SGP ", sgp.rawH2, " H2",1);
  displayValue("SGP ", sgp.rawEthanol, " Eth",2);
  delay(5000);

  sgpCounter++;
  if (sgpCounter == 30) {
    sgpCounter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      printMessage("Failed to get baseline readings");
      return;
    }
    Serial.print("****Baseline values: eCO2: 0x"); 
    Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); 
    Serial.println(TVOC_base, HEX);
  }
}

void displayValue(String type, float value, String unit, uint8_t line) {
  if(line == 1) {
    display.setCursor(5, 5);
  } else {
    display.setCursor(5, 21);
  }
  display.print(type);
  display.print(value);
  display.print(unit);
  display.display();
  Serial.print(type);
  Serial.print(value);
  Serial.println(unit);
}

void printMessage(String message) {
  display.clearDisplay();
  display.setCursor(15, 10);
  display.print(message);
  display.display();
  Serial.println(message);
}


void setup() {
  Serial.begin(9600);

  while (!Serial) { delay(10); } // Wait for serial console to open!

  initSSD1306();
  delay(2000);
  initSGP30();
  delay(2000);
  initDHT22();
  delay(2000);
  initDS18B20();
  delay(2000);
}


void loop() {

  readDS18b20();
  delay(5000);
  readDHT22();
  delay(5000);
  readSGP30();
}
