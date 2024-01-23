#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "CHANGE-ME";
const char* password = "CHANGE-ME";
const char* mqtt_server = "10.0.0.1";
#define mqtt_port 1883
#define MQTT_USER "CHANGE-ME"
#define MQTT_PASSWORD "CHANGE-ME"
#define MQTT_PUBLISH_TOPIC_WIND "/pixel/wind/sensor"
#define INPUTPIN 15

volatile int count = 0;
volatile int lastCount = 0;
volatile unsigned long lastEntry;
volatile int endSlice = 0;
hw_timer_t * timer = NULL;
float average = 0;


WiFiClient wifiClient;
PubSubClient client(wifiClient);

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR isr() {
  portENTER_CRITICAL(&synch);
  if (millis() > lastEntry + 500) {
    count++;
    lastEntry = millis();
  }
  portEXIT_CRITICAL(&synch);
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  endSlice = 1;
  lastCount = count;
  count=0;
  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
  //-- MQTT --  
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
  
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  pinMode(INPUTPIN, INPUT_PULLUP);
  attachInterrupt(INPUTPIN, isr, RISING);
  lastEntry = millis();
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 20000000, true);
  timerAlarmEnable(timer);
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
  Serial.println(count);
  if(endSlice == 1) {
    //calc average
    average = lastCount/10;
    Serial.println(average);
    portENTER_CRITICAL_ISR(&timerMux); // início da seção crítica
    endSlice = 0;
    portEXIT_CRITICAL_ISR(&timerMux); // fim da seção crítica
    publishSerialData(MQTT_PUBLISH_TOPIC_WIND, average);
  }
  delay(1000);
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
