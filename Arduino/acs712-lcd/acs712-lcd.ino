/*
ACS712 Sensor current
Adafruit SSD1306 LCD display
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
Measuring AC Current Using ACS712
*/
const int sensorIn = A0;
int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
float voltage = 0;


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

float getVoltage()
{
  float result;

  int readValue; //value read from the sensor
  int totalValue = 0;
  float avgValue = 0.0;

  readValue = analogRead(sensorIn);

  result = (readValue * 5.0)/1024.0;

 return result;
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
}


void loop() {
  
 voltage = getVoltage();
 
 float current = (voltage * 1000)/mVperAmp;
 Serial.print(current);
 Serial.println(" Amps");
 delay(5000);
  
}
