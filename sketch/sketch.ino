#include <Control_Surface.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// Create MCP23017 objects
Adafruit_MCP23X17 mcp0;
// Adafruit_MCP23X17 mcp1;

#define POT0 A0
#define POT1 A1
#define POT2 A2
#define POT3 A3
#define POT4 A4
#define FADER_POT A5

#define FADER_FORWARD 7
#define FADER_REVERSE 8
#define FADER_SPEED 6

// Leonardo
#define BUTTON_MUTE 4
#define LED_MUTE 5
#define BUTTON_SOLO 6
#define LED_SOLO 7

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

USBMIDI_Interface midi;
CCPotentiometer pot0 { POT0, MIDI_CC::General_Purpose_Controller_1 };
CCPotentiometer pot1 { POT1, MIDI_CC::General_Purpose_Controller_2 };
CCPotentiometer pot2 { POT2, MIDI_CC::General_Purpose_Controller_3 };
CCPotentiometer pot3 { POT3, MIDI_CC::General_Purpose_Controller_4 };
CCPotentiometer pot4 { POT4, MIDI_CC::General_Purpose_Controller_5 };

Bank<4> bank(1);
Bankable::CCPotentiometer fader[] {
    {bank, FADER_POT, 1},
};

void setPWMPrescaler(uint8_t prescaler) {
  TCCR1B = (TCCR1B & B11111000) | prescaler;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {delay(10);}
  Control_Surface.begin();

  if (!mcp0.begin_I2C(0x20)) {
    Serial.println("Error Initializing MCP23017");
    while (1);
  }
  Serial.println("Initialized MCP23017");

  for (int i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      Serial.println("Setting pin " + String(i) + " to INPUT_PULLUP");
      mcp0.pinMode(i, INPUT_PULLUP);
      // mcp1.pinMode(i, INPUT_PULLUP);
    } else {
      Serial.println("Setting pin " + String(i) + " to OUTPUT");
      mcp0.pinMode(i, OUTPUT);
      // mcp1.pinMode(i, OUTPUT);
    }
  }

  pinMode(FADER_FORWARD, OUTPUT);
  pinMode(FADER_REVERSE, OUTPUT);
  pinMode(FADER_SPEED, OUTPUT);

  setPWMPrescaler(1);
  boot();

  pinMode(BUTTON_MUTE, INPUT_PULLUP);
  pinMode(LED_MUTE, OUTPUT);
  pinMode(BUTTON_SOLO, INPUT_PULLUP);
  pinMode(LED_SOLO, OUTPUT);
}

int currentTrack = 1;
bool muted_tracks[8] = {false};
bool soloed_tracks[8] = {false};
int faderValues[8] = {0};

void loop() {
  Control_Surface.loop();

  bool mute_pressed = digitalRead(BUTTON_MUTE) == LOW;
  bool solo_pressed = digitalRead(BUTTON_SOLO) == LOW;

  digitalWrite(LED_MUTE, mute_pressed ? HIGH : LOW);
  digitalWrite(LED_SOLO, solo_pressed ? HIGH : LOW);

  for (int i = 0; i < 4; i++) {
    if (getMcpForTrack(i)->digitalRead((i % 4) * 4) == LOW) {
      Serial.println("Track " + String(i) + " selected");
      updateTrack(i, mute_pressed, solo_pressed);
    }
  }

  lightTrackLed();
}

Adafruit_MCP23X17* getMcpForTrack(int track) {
  // return (trackMcp[track] == 0) ? &mcp0 : &mcp1;
  return &mcp0;
}

void lightTrackLed() {
  selected_led(0);
  mute_led(0);
  solo_led(0);
  selected_led(currentTrack);

  for (int i = 0; i < 8; i++) {
    if (muted_tracks[i]) {
      mute_led(i + 1);
    }
    if (soloed_tracks[i]) {
      solo_led(i + 1);
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
    // moveTo(faderValues[trackId] * 8);
  } else {
    Serial.println("No value");
  }
}

void boot() {
  int speed = 100;

  for (int iter = 0; iter <= 10; iter++) {
    for (int step = 1; step <= 4; step++) {
      selected_led(0);
      mute_led(0);
      solo_led(0);

      selected_led(step);
      // mute_led(step);
      // solo_led(step);

      delay(speed);
    }
  }

  selected_led(0);
  mute_led(0);
  solo_led(0);
}

void selected_led(int selected) {
  for (int i = 0; i < 4; i++) { // Loop only for 4 tracks as there are 4 steps in the boot sequence
    int ledPin = TRACK_1_LED_SELECTED + ((i % 4) * 4); // Correct pin calculation
    int state = (selected == i) ? HIGH : LOW;
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

void mute_led(int track) {
  for (int i = 0; i < 4; i++) {
    int ledPin = TRACK_1_LED_MUTE + ((i % 4) * 4);
    int state = (track == i + 1) ? HIGH : LOW;
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

void solo_led(int track) {
  for (int i = 0; i < 4; i++) {
    int ledPin = TRACK_1_LED_SOLO + ((i % 4) * 4);
    int state = (track == i + 1) ? HIGH : LOW;
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

void moveTo(int targetPosition) {
  Serial.println("Moving fader to " + String(targetPosition));
  int currentPosition = analogRead(FADER_POT);
  
  while (abs(targetPosition - currentPosition) > 50) {
    int distance = abs(targetPosition - currentPosition);
    int speed = map(distance, 0, 1023, 170, 220);

    if (targetPosition > currentPosition) {
      digitalWrite(FADER_FORWARD, HIGH);
      digitalWrite(FADER_REVERSE, LOW);
    } else {
      digitalWrite(FADER_FORWARD, LOW);
      digitalWrite(FADER_REVERSE, HIGH);
    }
    analogWrite(FADER_SPEED, speed);

    currentPosition = analogRead(FADER_POT);
    Serial.print("Current Position: ");
    Serial.println(currentPosition);
  }

  digitalWrite(FADER_FORWARD, LOW);
  digitalWrite(FADER_REVERSE, LOW);
  analogWrite(FADER_SPEED, 0);
}
