#include "esp32-hal.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <pins.h>
#include "i2c_scanner.h"
#include "track_control.h"
#include "led_control.h"
#include "debounce.h"
#include "mcp_manager.h"
// #include "midi_manager/midi_manager.h"
#include <Control_Surface.h>
#include <U8g2lib.h>

USBMIDI_Interface midi;
CCPotentiometer pot0{POT0, MIDI_CC::General_Purpose_Controller_1};
// CCPotentiometer pot1{POT1, MIDI_CC::General_Purpose_Controller_2};
// CCPotentiometer pot2{POT2, MIDI_CC::General_Purpose_Controller_3};
// CCPotentiometer pot3{POT3, MIDI_CC::General_Purpose_Controller_4};
// CCPotentiometer pot4{POT4, MIDI_CC::General_Purpose_Controller_5};

Bank<4> bank(1);
Bankable::CCPotentiometer fader[]{
    {bank, FADER_POT, 1},
};

// Set volume of tracks 1–8
void setTrackVolume(uint8_t track, uint8_t volume)
{
  MIDIAddress controller = {MIDI_CC::Sound_Controller_1 + (track - 1)};
  uint8_t value = volume;
  midi.sendControlChange(controller, value);
}

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  Serial.print(bus);
}

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Starting setup");
  Wire.begin();
  Serial.println("Began wire");
  Control_Surface.begin();
  Serial.println("Control surface started");
  scanI2CBus();

  mcp0.begin_I2C(0x20);
  mcp1.begin_I2C(0x24);

  for (int i = 0; i < 16; i++)
  {
    if (i == 2 || i == 6 || i == 10 || i == 14)
    {
      mcp0.pinMode(i, INPUT_PULLUP);
      mcp1.pinMode(i, INPUT_PULLUP);
    }
    else
    {
      mcp0.pinMode(i, OUTPUT);
      mcp1.pinMode(i, OUTPUT);
    }
  }

  mcp_other.begin_I2C(0x22);
  mcp_other.pinMode(BUTTON_MUTE, INPUT_PULLUP);
  mcp_other.pinMode(LED_MUTE, OUTPUT);
  mcp_other.pinMode(BUTTON_SOLO, INPUT_PULLUP);
  mcp_other.pinMode(LED_SOLO, OUTPUT);

  pinMode(FADER_FORWARD, OUTPUT);
  pinMode(FADER_REVERSE, OUTPUT);
  pinMode(FADER_SPEED, OUTPUT);
  analogWriteFrequency(FADER_SPEED, 1000);

  // Init OLED display on bus number 0
  TCA9548A(0);
  if(!u8g2.begin()) {
    Serial.println(F("OLED allocation failed"));
    for(;;);
  }
  Serial.println("Display 1 initialized");

  // Init OLED display on bus number 1
  TCA9548A(1);
  if(!u8g2.begin()) {
    Serial.println(F("OLED allocation failed"));
    for(;;);
  }
  Serial.println("Display 2 initialized");

  boot();
}

void loop()
{
  TCA9548A(0);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0,15,"Hello World!");
  } while ( u8g2.nextPage() );

  TCA9548A(1);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0,15,"Display 2!");
  } while ( u8g2.nextPage() );

  Control_Surface.loop();
  bool mute_pressed = mcp_other.digitalRead(BUTTON_MUTE) == LOW;
  bool solo_pressed = mcp_other.digitalRead(BUTTON_SOLO) == LOW;

  mcp_other.digitalWrite(LED_MUTE, mute_pressed ? HIGH : LOW);
  mcp_other.digitalWrite(LED_SOLO, solo_pressed ? HIGH : LOW);

  for (int i = 0; i < 8; i++)
  {
    if (debounceButton(i, getMcpForTrack(i)->digitalRead(TRACK_1_BUTTON_SELECT + ((i % 4) * 4))))
    {
      Serial.println("Track " + String(i) + " muted");
      updateTrack(i, mute_pressed, solo_pressed);
    }
  }

  lightTrackLed();
}

