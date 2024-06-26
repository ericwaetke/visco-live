#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ESP32Encoder.h>

// Include custom fonts
#include "font/unibody_8.h"
#include "font/unibody_16.h"

// Initialize the OLED display (128x128px, HW I2C connection)
U8G2_SH1107_128X128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Define the rotary encoder pins
#define ENCODER_DT 2
#define ENCODER_CLK 4
#define ENCODER_SW 15

// Create Encoder object
ESP32Encoder encoder;

// Define the VolumeIn input
int VolumeIn = 51; // Start at 40% (51 out of 128)


void setup()
{
  Wire.begin(); // Initialize I2C communication

  u8g2.begin();          // Begin the u8g2 library
  u8g2.setContrast(255); // Set display contrast/brightness

  // Initialize Serial communication for debugging (optional)
  Serial.begin(115200);
  Serial.println("Display initialized!");

  // Enable the weak pull up resistors
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // Attach the encoder
  encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
  encoder.clearCount();

  // Set the encoder button pin
  pinMode(ENCODER_SW, INPUT_PULLUP);
}

void loop()
{
  // Read the encoder value
  VolumeIn = encoder.getCount();

  u8g2.firstPage(); // Select the first page of the display

  do
  {
    int numberOfRectangles = 16;
    int rectHeight = 4;
    int screenHeight = 128;
    int verticalSpacing = (screenHeight - (numberOfRectangles * rectHeight)) / (numberOfRectangles - 1);

    // Map VolumeIn to the number of rectangles to be drawn
    int activeRectangles = map(VolumeIn, -64, 64, 0, numberOfRectangles);

    for (int i = 0; i < numberOfRectangles; i++)
    {
      int rectY = screenHeight - ((i + 1) * (rectHeight + verticalSpacing) - verticalSpacing);
      int rectWidth = 0 + i * 1.3;
      if (i < activeRectangles)
      {
        // Draw active rectangles as solid boxes
        u8g2.setDrawColor(1);
        u8g2.drawBox(0, rectY, rectWidth, rectHeight);
      }
      else
      {
        // Draw inactive rectangles with a striped pattern
        for (int j = 0; j < rectWidth; j += 3)
        {
          u8g2.drawLine(j, rectY, j, rectY + rectHeight);
        }
      }
    }

    // Draw the three lines of text and top right rectangle
    u8g2.setFont(unibody_8);
    u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Sample Library"), 15, "Sample Library");  // First line, top right corner

    u8g2.drawStr(128 - 5 - u8g2.getStrWidth("1 Kick >"), 128 - 40, "1 Kick >");  // Second line, bottom right corner, over third line

    u8g2.setFont(unibody_16);
    u8g2.drawStr(128 - 5 - u8g2.getStrWidth("1974 Kick"), 128 - 20, "1974 Kick");  // Third line, bottom right corner

    // Draw the top right rectangle (width 2px, height 5px)
    u8g2.drawBox(128 - 20, 0, 2, 5);

  } while (u8g2.nextPage()); // Continue to the next page if available

  delay(100); // Delay for readability, adjust as necessary
}
