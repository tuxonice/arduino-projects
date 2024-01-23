#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int analogInPin = A0;

// Number of samples to average the reading over
// Change this to make the reading smoother... but beware of buffer overflows!
const int avgSamples = 10;
int sensorValue = 0;

float sensitivity = 1000.0 / 400.0; //1000mA per 400mV = 2.5
float Vref = 2500; // Output voltage with no current: ~ 2500mV or 2.5V


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
  
   // read the analog in value:
  for (int i = 0; i < avgSamples; i++)
  {
    sensorValue += analogRead(analogInPin);

    // wait 2 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    delay(5);

  }

  sensorValue = sensorValue / avgSamples;

  // The on-board ADC is 10-bits -> 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  // The voltage is in millivolts
  float voltage = 4.88 * sensorValue;

  // This will calculate the actual current (in mA)
  // Using the Vref and sensitivity settings you configure
  float current = (voltage - Vref) * sensitivity;

  //display.clearDisplay();
  //displayValue("Current: ", voltage, " mV",1);


  // This is the raw sensor value, not very useful without some calculations
  Serial.println(sensorValue);
  Serial.println(voltage);
  Serial.println(current);
  
  //Serial.println("mV");
 
  delay(5000);
}
