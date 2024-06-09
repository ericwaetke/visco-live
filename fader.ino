#define IN1 3       // Motor 1 - Forward
#define IN2 2       // Motor 1 - Reverse
#define ENABLE 9    // Enable pin for both motors (PWM enabled)

void setPWMPrescaler(uint8_t prescaler) {
  TCCR1B = (TCCR1B & B11111000) | prescaler;
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENABLE, OUTPUT);

  Serial.begin(9600); // Initialize serial communication at 9600 baud

  // Set initial PWM frequency for pin 9 (Timer 1)
  setPWMPrescaler(1);
}

void loop() {
  // Example usage of the moveTo function
  moveTo(1024); // Move to the maximum position
  delay(1000);  // Wait for 1 second

  moveTo(512); // Move to the middle position
  delay(1000); // Wait for 1 second

  moveTo(0); // Move to the minimum position
  delay(1000); // Wait for 1 second

  moveTo(512); // Move to the middle position
  delay(1000); // Wait for 1 second
}

// Function to move the fader to a specified position
void moveTo(int targetPosition) {
  int currentPosition = analogRead(A1);

  // Move the fader until it reaches the target position
  while (abs(targetPosition - currentPosition) > 5) { // Allowing some tolerance
    int distance = abs(targetPosition - currentPosition);
    int speed = map(distance, 0, 1023, 170, 220); // Proportional control
    // int speed = 170;

    if (targetPosition > currentPosition) {
      Serial.println("Moving Up");
      // Move fader up
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    } else {
      Serial.println("Moving Down");
      // Move fader down
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }

    // Set speed (adjust value as needed)
    analogWrite(ENABLE, speed); // Use the dynamically calculated speed

    // Read current position again
    currentPosition = analogRead(A1);

    // Debug output
    Serial.print("Current Position: ");
    Serial.println(currentPosition);
    Serial.print("Speed: ");
    Serial.println(speed);
  }

  // Stop the motor once the position is reached
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENABLE, 0);
}
