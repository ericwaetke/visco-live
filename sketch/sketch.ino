// #include "MIDIUSB.h"
// #include "PitchToNote.h"
#include <Control_Surface.h>  // Include the library

#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// Create an MCP23017 object
Adafruit_MCP23X17 mcp0;
Adafruit_MCP23X17 mcp1;

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

// Leonardo
#define BUTTON_MUTE 2
#define LED_MUTE 3
#define BUTTON_SOLO 4
#define LED_SOLO 5

// MCP 0
#define TRACK_1_BUTTON_SELECT 0
#define TRACK_1_LED_SELECTED 1
#define TRACK_1_LED_MUTE 2
#define TRACK_1_LED_SOLO 3

#define TRACK_2_BUTTON_SELECT 4
#define TRACK_2_LED_SELECTED 5
#define TRACK_2_LED_MUTE 6
#define TRACK_2_LED_SOLO 7

#define TRACK_3_BUTTON_SELECT 8
#define TRACK_3_LED_SELECTED 9
#define TRACK_3_LED_MUTE 10
#define TRACK_3_LED_SOLO 11

#define TRACK_4_BUTTON_SELECT 12
#define TRACK_4_LED_SELECTED 13
#define TRACK_4_LED_MUTE 14
#define TRACK_4_LED_SOLO 15

// MCP 1
#define TRACK_5_BUTTON_SELECT 0
#define TRACK_5_LED_SELECTED 1
#define TRACK_5_LED_MUTE 2
#define TRACK_5_LED_SOLO 3

#define TRACK_6_BUTTON_SELECT 4
#define TRACK_6_LED_SELECTED 5
#define TRACK_6_LED_MUTE 6
#define TRACK_6_LED_SOLO 7

#define TRACK_7_BUTTON_SELECT 8
#define TRACK_7_LED_SELECTED 9
#define TRACK_7_LED_MUTE 10
#define TRACK_7_LED_SOLO 11

#define TRACK_8_BUTTON_SELECT 12
#define TRACK_8_LED_SELECTED 13
#define TRACK_8_LED_MUTE 14
#define TRACK_8_LED_SOLO 15

int trackMcp[8] = {0, 0, 0, 0, 1, 1, 1, 1};

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

 // Initialize the MCP23017 with the default I2C address (0x20)
  if (!mcp0.begin_I2C()) {
    Serial.println("Error Initializing MCP23017");
    while (1);
  }
  Serial.println("Initialized MCP23017");

  // Interate over all the pins and set them as inputs or outputs
  for (int i = 0; i < 16; i++) {
    switch(trackMcp[i]) {
      case 0:
        if (i % 4 == 0) {
          mcp0.pinMode(i, INPUT);
        } else {
          mcp0.pinMode(i, OUTPUT);
        }
        break;
      case 1:
        if (i % 4 == 0) {
          mcp1.pinMode(i, INPUT_PULLUP);
        } else {
          mcp1.pinMode(i, OUTPUT);
        }
        break;
    }
  }

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
bool muted_tracks[8] = {false}; // Initialize all tracks as not muted
bool soloed_tracks[8] = {false}; // Initialize all tracks as not soloed

int faderValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop() {
  Control_Surface.loop();

  // Check if Mute and/or Solo Button is pressed
  bool mute_pressed = digitalRead(BUTTON_MUTE) == low
  bool solo_pressed = digitalRead(BUTTON_SOLO) == low

  // Light the Solo or Mute LEDs depending on the button state
  if (mute_pressed) { digitalWrite(LED_MUTE, HIGH); }
  else { digitalWrite(LED_MUTE, LOW); }
  if (solo_pressed) { digitalWrite(LED_SOLO, HIGH); }
  else { digitalWrite(LED_SOLO, LOW); }

  // Interate over all the buttons and check if any is pressed
  for (int i = 0; i < 8; i++) {
    if (selectedMcp(i).digitalRead((i % 4) * 4) == LOW) {
      updateTrack(i, mute_pressed, solo_pressed);
    }
  }

  // Light the correct tracks based on states:
  // - Muted
  // - Soloed
  // - Selected
  lightTrackLed();
}

Adafruit_MCP23X17 selectedMcp(int track) {
  switch (trackMcp[track]) {
    case 0:
      return mcp0;
    case 1:
      return mcp1;
  }
}

void lightTrackLed() {
  selected_led(0);
  mute_led(0);
  selected_led(0);
  selected_led(currentTrack);

  for (int i = 0; i < 4; i++) {
    if (muted_tracks[i]) {
      mute_led(i);
    }
    if (soloed_tracks[i]) {
      solo_led(i);
    }
  }
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
      mute_led(0);
      selected_led(0);

      selected_led(step);
      mute_led(step);
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
      selectedMcp(selected).digitalWrite(TRACK_1_LED_SELECTED, LOW);
      selectedMcp(selected).digitalWrite(TRACK_2_LED_SELECTED, LOW);
      selectedMcp(selected).digitalWrite(TRACK_3_LED_SELECTED, LOW);
      selectedMcp(selected).digitalWrite(TRACK_4_LED_SELECTED, LOW);
      break;
    case 1:
      selectedMcp(selected).digitalWrite(TRACK_1_LED_SELECTED, HIGH);
      break;
    case 2:
      selectedMcp(selected).digitalWrite(TRACK_2_LED_SELECTED, HIGH);
      break;
    case 3:
      selectedMcp(selected).digitalWrite(TRACK_3_LED_SELECTED, HIGH);
      break;
    case 4:
      selectedMcp(selected).digitalWrite(TRACK_4_LED_SELECTED, HIGH);
      break;
  }
}

void mute_led(int track) {
  switch (track) {
    case 0:
      selectedMcp(selected).digitalWrite(TRACK_1_LED_MUTE, LOW);
      selectedMcp(selected).digitalWrite(TRACK_2_LED_MUTE, LOW);
      selectedMcp(selected).digitalWrite(TRACK_3_LED_MUTE, LOW);
      selectedMcp(selected).digitalWrite(TRACK_4_LED_MUTE, LOW);
      break;
    case 1:
      selectedMcp(selected).digitalWrite(TRACK_1_LED_MUTE, HIGH);
      break;
    case 2:
      selectedMcp(selected).digitalWrite(TRACK_2_LED_MUTE, HIGH);
      break;
    case 3:
      selectedMcp(selected).digitalWrite(TRACK_3_LED_MUTE, HIGH);
      break;
    case 4:
      selectedMcp(selected).digitalWrite(TRACK_4_LED_MUTE, HIGH);
      break;
  }
}

void solo_led(int track) {
  switch (track) {
    case 0:
      selectedMcp(selected).digitalWrite(TRACK_1_LED_SOLO, LOW);
      selectedMcp(selected).digitalWrite(TRACK_2_LED_SOLO, LOW);
      selectedMcp(selected).digitalWrite(TRACK_3_LED_SOLO, LOW);
      selectedMcp(selected).digitalWrite(TRACK_4_LED_SOLO, LOW);
      break;
    case 1:
      selectedMcp(selected).digitalWrite(TRACK_1_LED_SOLO, HIGH);
      break;
    case 2:
      selectedMcp(selected).digitalWrite(TRACK_2_LED_SOLO, HIGH);
      break;
    case 3:
      selectedMcp(selected).digitalWrite(TRACK_3_LED_SOLO, HIGH);
      break;
    case 4:
      selectedMcp(selected).digitalWrite(TRACK_4_LED_SOLO, HIGH);
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