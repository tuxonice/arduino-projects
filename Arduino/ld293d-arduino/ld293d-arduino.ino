/*
 * in1 -> to motor (-)
 * in2 -> to motor (+)
 */


// Motor connections
int enA = 9;
int in1 = 8;
int in2 = 7;

void setup() {
  Serial.begin(9600);
  
	// Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}

void loop() {
	directionControl();
	delay(5000);
	//speedControl();
	//delay(5000);
}

// This function lets you control spinning direction of motor
void directionControl() {
	// Set motor to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(enA, 255);

	// Turn on motor
  Serial.println("Turn on motor (direction)");
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	delay(20000);
	
	// Turn off motor
  Serial.println("Turn off motor (direction)");
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
	// Turn on motor
  Serial.println("Turn on motor speed control");
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	
	// Accelerate from zero to maximum speed
  Serial.println("Accelerate from zero to maximum speed");
	for (int i = 0; i < 256; i++) {
		analogWrite(enA, i);
		delay(10);
	}
	
	// Decelerate from maximum speed to zero
  Serial.println("Decelerate from maximum speed to zero");
	for (int i = 255; i >= 0; --i) {
		analogWrite(enA, i);
		delay(10);
	}
	
	// Now turn off motor
  Serial.println("Turn off motor speed control");
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}
