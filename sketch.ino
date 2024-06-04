// #include "MIDIUSB.h"
// #include "PitchToNote.h"
#include <Control_Surface.h>  // Include the library

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use
CCPotentiometer pot0 { A0, MIDI_CC::General_Purpose_Controller_1 };
CCPotentiometer pot1 { A1, MIDI_CC::General_Purpose_Controller_2 };
CCPotentiometer pot2 { A2, MIDI_CC::General_Purpose_Controller_3 };
CCPotentiometer pot3 { A3, MIDI_CC::General_Purpose_Controller_4 };
CCPotentiometer pot4 { A4, MIDI_CC::General_Purpose_Controller_5 };

// The setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  // pinMode(7, INPUT);

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
}

int lit = 0;

// The loop function runs over and over again forever
void loop() {
  Control_Surface.loop();

  // Check if the button is pressed
  if (digitalRead(2) == LOW) {
    lit = 12;
  } else if (digitalRead(3) == LOW) {
    lit = 11;
  } else if (digitalRead(4) == LOW) {
    lit = 10;
  } else if (digitalRead(5) == LOW) {
    lit = 9;
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

  for (int iter = 0; iter < 4; iter++) {
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
