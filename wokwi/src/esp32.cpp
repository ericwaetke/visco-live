// #include <Control_Surface.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#include <pins.h>
#include "i2c_scanner/i2c_scanner.h"

int trackMcp[8] = {0, 0, 0, 0, 1, 1, 1, 1};
Adafruit_MCP23X17 mcp0;
Adafruit_MCP23X17 mcp_other;
Adafruit_MCP23X17 *getMcpForTrack(int track)
{
  // return (trackMcp[track] == 0) ? &mcp0 : &mcp1;
  return &mcp0;
}

int currentTrack = 0;
bool muted_tracks[8] = {false};
bool soloed_tracks[8] = {false};
int faderValues[8] = {0};

const int debounceDelay = 50; // milliseconds

unsigned long lastDebounceTime[8] = {0}; // Array to store the last debounce time for each button
bool lastButtonState[8] = {HIGH};        // Array to store the last button state

// USBMIDI_Interface midi;
// CCPotentiometer pot0{POT0, MIDI_CC::General_Purpose_Controller_1};
// CCPotentiometer pot1{POT1, MIDI_CC::General_Purpose_Controller_2};
// CCPotentiometer pot2{POT2, MIDI_CC::General_Purpose_Controller_3};
// CCPotentiometer pot3{POT3, MIDI_CC::General_Purpose_Controller_4};
// CCPotentiometer pot4{POT4, MIDI_CC::General_Purpose_Controller_5};

// Bank<4> bank(1);
// Bankable::CCPotentiometer fader[]{
//     {bank, FADER_POT, 1},
// };

void lightTrackLed();
void updateTrack(int trackId, bool mute_pressed, bool solo_pressed);
void switchToTrack(int trackId);
void boot();
void selected_led(int selected);
void mute_led(int track);
void solo_led(int track);
void moveTo(int targetPosition);

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Starting setup");
  // Control_Surface.begin();
  Wire.begin();

  // Call the I2C scanner function
  scanI2CBus();

  // Tracks 1-4
  mcp0.begin_I2C(0x20);
  for (int i = 0; i < 16; i++)
  {
    if (i == 2 || i == 6 || i == 10 || i == 14)
    {
      mcp0.pinMode(i, INPUT_PULLUP);
    }
    else
    {
      mcp0.pinMode(i, OUTPUT);
    }
  }

  // Mute, Solo, Select ...
  mcp_other.begin_I2C(0x22);
  mcp_other.pinMode(BUTTON_MUTE, INPUT_PULLUP);
  mcp_other.pinMode(LED_MUTE, OUTPUT);
  mcp_other.pinMode(BUTTON_SOLO, INPUT_PULLUP);
  mcp_other.pinMode(LED_SOLO, OUTPUT);

  // pinMode(FADER_FORWARD, OUTPUT);
  // pinMode(FADER_REVERSE, OUTPUT);
  // pinMode(FADER_SPEED, OUTPUT);

  boot();
}

void loop()
{
  // Serial.print("Current Track: ");
  // Serial.print("\b");
  // Serial.print(currentTrack);
  // Control_Surface.loop();

  bool mute_pressed = mcp_other.digitalRead(BUTTON_MUTE) == LOW;
  bool solo_pressed = mcp_other.digitalRead(BUTTON_SOLO) == LOW;
  // mcp_other.digitalWrite(7, HIGH);

  mcp_other.digitalWrite(LED_MUTE, mute_pressed ? HIGH : LOW);
  mcp_other.digitalWrite(LED_SOLO, solo_pressed ? HIGH : LOW);

  for (int i = 0; i < 4; i++)
  {
    if (getMcpForTrack(i)->digitalRead(TRACK_1_BUTTON_SELECT + (i * 4)) == LOW)
    {
      Serial.println("Track " + String(i) + " selected");
      updateTrack(i, mute_pressed, solo_pressed);
    }
  }

  lightTrackLed();
}

void lightTrackLed()
{
  selected_led(-1);
  mute_led(0);
  solo_led(0);
  selected_led(currentTrack);

  for (int i = 0; i < 8; i++)
  {
    if (muted_tracks[i])
    {
      mute_led(i + 1);
    }
    if (soloed_tracks[i])
    {
      solo_led(i + 1);
    }
  }
}

void updateTrack(int trackId, bool mute_pressed, bool solo_pressed)
{
  if (!mute_pressed && !solo_pressed)
  {
    // faderValues[currentTrack] = fader[0].getValue();
    currentTrack = trackId;
    // bank.select(currentTrack);
    switchToTrack(currentTrack);
    return;
  }

  if (mute_pressed)
  {
    muted_tracks[trackId] = !muted_tracks[trackId];
    Serial.println("Mute Track: " + String(trackId) + " - " + String(muted_tracks[trackId]));
  }
  if (solo_pressed)
  {
    soloed_tracks[trackId] = !soloed_tracks[trackId];
    Serial.println("Solo Track: " + String(trackId) + " - " + String(soloed_tracks[trackId]));
  }
}

void switchToTrack(int trackId)
{
  Serial.println("Switching to Track: " + String(trackId));

  if (faderValues[trackId])
  {
    // moveTo(faderValues[trackId] * 8);
  }
  else
  {
    Serial.println("No value");
  }
}

void boot()
{
  int speed = 100;

  for (int iter = 0; iter <= 10; iter++)
  {
    for (int step = 0; step < 4; step++)
    {
      selected_led(-1);
      mute_led(0);
      solo_led(0);

      selected_led(step);
      // mute_led(step);
      // solo_led(step);

      delay(speed);
    }
  }

  selected_led(-1);
  mute_led(0);
  solo_led(0);
}

void selected_led(int selected)
{
  if (selected == -1)
  {
    for (int i = 0; i < 4; i++)
    {
      int ledPin = TRACK_1_LED_SELECTED + (i * 4);
      getMcpForTrack(i)->digitalWrite(ledPin, LOW);
    }
    return;
  }
  for (int i = 0; i < 4; i++)
  {                                                    // Loop only for 4 tracks as there are 4 steps in the boot sequence
    int ledPin = TRACK_1_LED_SELECTED + ((i % 4) * 4); // Correct pin calculation
    int state = (selected == i) ? HIGH : LOW;
    // Serial.println("Setting pin " + String(ledPin) + " to " + String(state));
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

void mute_led(int track)
{
  for (int i = 0; i < 4; i++)
  {
    int ledPin = TRACK_1_LED_MUTE + ((i % 4) * 4);
    int state = (track == i + 1) ? HIGH : LOW;
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

void solo_led(int track)
{
  for (int i = 0; i < 4; i++)
  {
    int ledPin = TRACK_1_LED_SOLO + ((i % 4) * 4);
    int state = (track == i + 1) ? HIGH : LOW;
    getMcpForTrack(i)->digitalWrite(ledPin, state);
  }
}

// void moveTo(int targetPosition)
// {
//   Serial.println("Moving fader to " + String(targetPosition));
//   int currentPosition = analogRead(FADER_POT);

//   while (abs(targetPosition - currentPosition) > 50)
//   {
//     int distance = abs(targetPosition - currentPosition);
//     int speed = map(distance, 0, 1023, 170, 220);

//     if (targetPosition > currentPosition)
//     {
//       digitalWrite(FADER_FORWARD, HIGH);
//       digitalWrite(FADER_REVERSE, LOW);
//     }
//     else
//     {
//       digitalWrite(FADER_FORWARD, LOW);
//       digitalWrite(FADER_REVERSE, HIGH);
//     }
//     analogWrite(FADER_SPEED, speed);

//     currentPosition = analogRead(FADER_POT);
//     Serial.print("Current Position: ");
//     Serial.println(currentPosition);
//   }

//   digitalWrite(FADER_FORWARD, LOW);
//   digitalWrite(FADER_REVERSE, LOW);
//   analogWrite(FADER_SPEED, 0);
// }
