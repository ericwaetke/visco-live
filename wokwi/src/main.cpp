#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <pins.h>
#include "i2c_scanner/i2c_scanner.h"
#include "track_control/track_control.h"
#include "led_control/led_control.h"
#include "debounce/debounce.h"
#include "mcp_manager/mcp_manager.h"

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Starting setup");
  Wire.begin();
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

  boot();
}

void loop()
{
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
