// #include "MIDIUSB.h"
// #include "PitchToNote.h"
#include <Control_Surface.h>  // Include the library
 
USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use
CCPotentiometer pot0 { POT0, MIDI_CC::General_Purpose_Controller_1 };
CCPotentiometer pot1 { POT1, MIDI_CC::General_Purpose_Controller_2 };
CCPotentiometer pot2 { POT2, MIDI_CC::General_Purpose_Controller_3 };
CCPotentiometer pot3 { POT3, MIDI_CC::General_Purpose_Controller_4 };
CCPotentiometer pot4 { POT4, MIDI_CC::General_Purpose_Controller_5 };

#define POT0 A0
#define POT1 A1
#define POT2 A2
#define POT3 A3
#define POT4 A4
#define FADER_POT A5

#define FADER_FORWARD 7       // Motor 1 - Forward
#define FADER_REVERSE 8       // Motor 1 - Reverse
#define FADER_SPEED 6    // Enable pin for both motors (PWM enabled)

#define BUTTON0 2
#define BUTTON1 3
#define BUTTON2 4
#define BUTTON3 5

#define LED0 12
#define LED1 11
#define LED2 10
#define LED3 9

Bank<4> bank(1); // 3 banks with an offset of 4 tracks per bank (a = 4)
 
Bankable::CCPotentiometer fader[] {
    {bank, FADER_POT, 1}, // base address 1 (b = 1)
};

void setPWMPrescaler(uint8_t prescaler) {
  TCCR1B = (TCCR1B & B11111000) | prescaler;
}


// The setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  // Initialize pins 10-13 as outputs
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  boot();
  Control_Surface.begin();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  // Set initial PWM frequency for pin 9 (Timer 1)
  setPWMPrescaler(1);
}

int lit = 0;

// The loop function runs over and over again forever
void loop() {
  Control_Surface.loop();

  Serial.println(fader[bank.getSelection()].getRawValue());

  // Check if the button is pressed
  if (digitalRead(2) == LOW) {
    lit = 12;
    bank.select(0);
    // moveTo(fader[0].getValue());
    Serial.println(fader[0].getValue());
  } else if (digitalRead(3) == LOW) {
    lit = 11;
    bank.select(1);
    // moveTo(fader[1].getValue());
    Serial.println(fader[1].getValue());
  } else if (digitalRead(4) == LOW) {
    lit = 10;
    bank.select(2);
    // moveTo(fader[2].getValue());
    Serial.println(fader[2].getValue());
  } else if (digitalRead(5) == LOW) {
    lit = 9;
    bank.select(3);
    // moveTo(fader[3].getValue());
    Serial.println(fader[3].getValue());
  }

  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);

  // Turn on the LED at the current position
  digitalWrite(lit, HIGH);
}

void boot() {
  int speed = 100;

  for (int iter = 0; iter <= 2; iter++) {
    for (int step = 1; step <= 4; step++) {
      selected_led(step);
      delay(speed);
    }
  }

  // Turn off all LEDs
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);

  lit = 12;
}

void selected_led(int selected) {
  switch (selected) {
    case 1:
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH);
      break;
    case 2:
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
      break;
    case 3:
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      break;
    case 4:
      digitalWrite(9, HIGH);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      break;
  }
}


// Function to move the fader to a specified position
void moveTo(int targetPosition) {
  int currentPosition = analogRead(FADER_POT);

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
    currentPosition = analogRead(FADER_POT);

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