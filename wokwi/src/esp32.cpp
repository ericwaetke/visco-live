#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ESP32Encoder.h>

#include "font/unibody_8.h"
#include "font/unibody_16.h"

U8G2_SH1107_128X128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define ENCODER_DT 2
#define ENCODER_CLK 4
#define ENCODER_SW 15
#define BUTTON_PIN 5

ESP32Encoder encoder;

int VolumeIn = 51; // Start at 40% (51 out of 128)
int screenHeight = 128;
int screenWidth = 128;

enum ScreenState {
  SCREEN_VOLUME,
  SCREEN_SAMPLE
};

ScreenState screenState = SCREEN_VOLUME;

const char* sampleList[] = {"808 Boom", "High-Energy Clap", "1974 Kick", "808ish Cymbal", "Ringing Snare"};
const char* sampleAttributes[] = {"808 >", "Clap >", "Kick >", "808 >", "Snare >"};
int selectedSample = 2; // "1974 Kick" is active first
int lastSelectedSample = selectedSample;
bool updateScreen = true; // Flag to control screen update

int encoderStepSize = 2; // Adjust the encoder step size for sensitive navigation

void setup()
{
  Wire.begin(); 
  u8g2.begin();          
  u8g2.setContrast(255); 

  Serial.begin(115200);
  Serial.println("Display initialized!");

  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
  encoder.clearCount();

  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
  // Handle button press to toggle between screens
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    screenState = (screenState == SCREEN_VOLUME) ? SCREEN_SAMPLE : SCREEN_VOLUME;
    encoder.clearCount(); // Reset encoder count when switching screens
    updateScreen = true; // Ensure screen update when switching back to volume screen
    delay(300); // Debounce delay
  }

  // Handle encoder button press for sample selection and switch back to volume screen
  if (digitalRead(ENCODER_SW) == LOW && screenState == SCREEN_SAMPLE)
  {
    // Confirm selection and switch back to volume screen
    screenState = SCREEN_VOLUME;
    lastSelectedSample = selectedSample; // Store last selected sample
    updateScreen = true; // Trigger screen update on return to volume screen
    // Activate selected sample here if needed
    delay(300); // Debounce delay
  }

  // Update based on screen state
  if (screenState == SCREEN_VOLUME)
  {
    // Volume screen logic
    VolumeIn = encoder.getCount();
  }
  else if (screenState == SCREEN_SAMPLE)
  {
    // Sample selection screen logic
    int encoderChange = encoder.getCount();
    if (encoderChange != 0)
    {
      int steps = encoderChange / encoderStepSize;
      selectedSample += steps;

      // Ensure selectedSample stays within valid range
      if (selectedSample < 0)
        selectedSample = 0;
      else if (selectedSample >= sizeof(sampleList) / sizeof(sampleList[0]))
        selectedSample = sizeof(sampleList) / sizeof(sampleList[0]) - 1;

      // Reset encoder count after processing movement
      encoder.clearCount();
      
      updateScreen = true; // Trigger screen update on sample selection change
    }
  }

  // Render the display based on screen state and update condition
  u8g2.firstPage();

  do
  {
    u8g2.drawBox(116, 0, 2, 5);

    if (screenState == SCREEN_VOLUME)
    {
      int numberOfRectangles = 16;
      int rectHeight = 4;
      int verticalSpacing = (screenHeight - (numberOfRectangles * rectHeight)) / (numberOfRectangles - 1);

      int activeRectangles = map(VolumeIn, -64, 64, 0, numberOfRectangles);

      for (int i = 0; i < numberOfRectangles; i++)
      {
        int rectY = screenHeight - ((i + 1) * (rectHeight + verticalSpacing) - verticalSpacing);
        int rectWidth = 0 + i * 1.3;
        if (i < activeRectangles)
        {
          u8g2.setDrawColor(1);
          u8g2.drawBox(0, rectY, rectWidth, rectHeight);
        }
        else
        {
          for (int j = 0; j < rectWidth; j += 3)
          {
            u8g2.drawLine(j, rectY, j, rectY + rectHeight);
          }
        }
      }

      u8g2.setFont(unibody_8);
      u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Sample Library"), 18, "Sample Library");  
      u8g2.drawStr(128 - 5 - u8g2.getStrWidth(sampleAttributes[selectedSample]), 128 - 40, sampleAttributes[selectedSample]);  
      u8g2.setFont(unibody_16);

      String displayString = sampleList[selectedSample];
      if (displayString.length() > 9)
      {
          displayString = displayString.substring(0, 6) + "...";
      }

      u8g2.drawStr(128 - 5 - u8g2.getStrWidth(displayString.c_str()), 128 - 20, displayString.c_str());  
    }
    else if (screenState == SCREEN_SAMPLE)
    {
      u8g2.setFont(unibody_8);
      u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Back"), 18, "Back");
      u8g2.setFont(unibody_8);

      int lineHeight = u8g2.getAscent() - u8g2.getDescent();
      int verticalSpacing = 5;
      int baseY = 128 - (4 * (lineHeight + verticalSpacing + 2));

      for (int i = -1; i <= 2; ++i)
      {
        int index = selectedSample + i;
        if (index >= 0 && index < (sizeof(sampleList) / sizeof(sampleList[0])))
        {
          int y = baseY + (i + 1) * (lineHeight + verticalSpacing);
          u8g2.drawStr(5, y, sampleList[index]);
          if (i == 0)
          {
            u8g2.drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6); 
          }
        }
      }
    }

  } while (u8g2.nextPage());

  if (updateScreen)
  {
    lastSelectedSample = selectedSample; // Update last selected sample
    updateScreen = false; // Reset update flag
  }

  delay(100);
}
