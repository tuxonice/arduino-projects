
volatile int count = 0;
unsigned long lastEntry, lastDisplay;
int frequency, tst;

#define INPUTPIN 15

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR isr() {
  portENTER_CRITICAL(&synch);
  count++;
  Serial.println(count);
  portEXIT_CRITICAL(&synch);
}


void setup() {
  Serial.begin(115200);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  pinMode(INPUTPIN, INPUT_PULLUP);
  attachInterrupt(INPUTPIN, isr, RISING);
  lastEntry = millis();
}

void loop() {
  /*
  if (millis() > lastEntry + 1000) {
    frequency = count;
    // display(0, "frequency=", frequency);
    lastEntry = millis();
    count = 0;
  }
  if (millis() > lastDisplay + 500) {
    lastDisplay = millis();
    // display(2, "Other=", tst++);
  }
  */
}
