// #include "MIDIUSB.h"
// #include "PitchToNote.h"
#include <Control_Surface.h>  // Include the library

#define POT0 A0
#define POT1 A1
#define POT2 A2
#define POT3 A3
#define POT4 A4
#define FADER_POT A5

#define FADER_FORWARD 7      
// Motor 1 - Reverse — WHITE, further away from Cutout

#define FADER_REVERSE 8       
 // Motor 1 - Forward — RED, closer to Cutout

#define FADER_SPEED 6    // Enable pin for both motors (PWM enabled)

#define BUTTON_MUTE 0
#define BUTTON_SOLO 0

#define BUTTON0 2
#define BUTTON1 3
#define BUTTON2 4
#define BUTTON3 5

#define LED0 9
#define LED1 10
#define LED2 11
#define LED3 12

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use
CCPotentiometer pot0 { POT0, MIDI_CC::General_Purpose_Controller_1 };
CCPotentiometer pot1 { POT1, MIDI_CC::General_Purpose_Controller_2 };
CCPotentiometer pot2 { POT2, MIDI_CC::General_Purpose_Controller_3 };
CCPotentiometer pot3 { POT3, MIDI_CC::General_Purpose_Controller_4 };
CCPotentiometer pot4 { POT4, MIDI_CC::General_Purpose_Controller_5 };

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
  Control_Surface.begin();

  // Initialize pins 10-13 as outputs
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  pinMode(FADER_FORWARD, OUTPUT);
  pinMode(FADER_REVERSE, OUTPUT);
  pinMode(FADER_SPEED, OUTPUT);

  // Set initial PWM frequency for pin 9 (Timer 1)
  setPWMPrescaler(1);

  // Start Boot Animation
  boot();
}

// Int showing what LED/Track is currently selected
int currentTrack = 0;
bool muted_tracks[4] = {false}; // Initialize all tracks as not muted
bool soloed_tracks[4] = {false}; // Initialize all tracks as not soloed

int faderValues[4] = {0, 0, 0, 0};

void loop() {
  Control_Surface.loop();

  bool mute_pressed = digitalRead(BUTTON_MUTE) == low
  bool solo_pressed = digitalRead(BUTTON_SOLO) == low

  // Check if the button is pressed
  if (digitalRead(BUTTON0) == LOW) {
    updateTrack(0, mute_pressed, solo_pressed);
  } else if (digitalRead(BUTTON1) == LOW) {
    updateTrack(1, mute_pressed, solo_pressed);
  } else if (digitalRead(BUTTON2) == LOW) {
    updateTrack(2, mute_pressed, solo_pressed);
  } else if (digitalRead(BUTTON3) == LOW) {
    updateTrack(3, mute_pressed, solo_pressed);
  }

  lightTrackLed();
}

void lightTrackLed() {
  selected_led(0);
  selected_led(currentTrack);
}

void updateTrack(int trackId, bool mute_pressed, bool solo_pressed) {
  if (!mute_pressed && !solo_pressed) {
    faderValues[currentTrack] = fader[0].getValue();
    currentTrack = trackId;

    bank.select(currentTrack);
    switchToTrack(currentTrack);
    return;
  }

  if (mute_pressed) {
    muted_tracks[trackId] = !muted_tracks[trackId];
    Serial.println("Mute Track: " + String(trackId) + " - " + String(muted_tracks[trackId]));
  }
  if (solo_pressed) {
    soloed_tracks[trackId] = !soloed_tracks[trackId];
    Serial.println("Solo Track: " + String(trackId) + " - " + String(soloed_tracks[trackId]));
  }
}

void switchToTrack(int trackId) {
  Serial.println("Switching to Track: " + String(trackId));

  if (faderValues[trackId]) {
    moveTo(faderValues[trackId] * 8);
  } else {
    Serial.println("no value");
  }
}

void boot() {
  int speed = 100;

  for (int iter = 0; iter <= 2; iter++) {
    for (int step = 1; step <= 4; step++) {
      selected_led(0);
      selected_led(step);
      delay(speed);
    }
  }

  // Turn off all LEDs
  selected_led(0);
}

void selected_led(int selected) {
  switch (selected) {
    case 0:
      digitalWrite(LED0, LOW);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      break;
    case 1:
      digitalWrite(LED3, HIGH);
      break;
    case 2:
      digitalWrite(LED2, HIGH);
      break;
    case 3:
      digitalWrite(LED1, HIGH);
      break;
    case 4:
      digitalWrite(LED0, HIGH);
      break;
  }
}

// Function to move the fader to a specified position
void moveTo(int targetPosition) {
  Serial.println("Moving fader to " + String(targetPosition));
  int currentPosition = analogRead(FADER_POT);
  
  // Move the fader until it reaches the target position
  while (abs(targetPosition - currentPosition) > 50) { // Allowing some tolerance
    int distance = abs(targetPosition - currentPosition);
    int speed = map(distance, 0, 1023, 170, 220); // Proportional control

    if (targetPosition > currentPosition) {
      Serial.print("Moving Up ");
      // Move fader up
      digitalWrite(FADER_FORWARD, HIGH);
      digitalWrite(FADER_REVERSE, LOW);
    } else {
      Serial.print("Moving Down ");
      // Move fader down
      digitalWrite(FADER_FORWARD, LOW);
      digitalWrite(FADER_REVERSE, HIGH);
    }
    analogWrite(FADER_SPEED, speed);

     // Debug outputSerial.print("Current Position: ");
    Serial.println(currentPosition);
  }

  // Stop the motor once the position is reached
  digitalWrite(FADER_FORWARD, LOW);
  digitalWrite(FADER_REVERSE, LOW);
  analogWrite(FADER_SPEED, 0);
}