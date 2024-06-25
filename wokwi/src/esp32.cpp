#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "font/unibody_8.h"
#include "font/unibody_16.h"

// Initialize the OLED display
U8G2_SH1107_128X128_1_HW_I2C u8g2(U8G2_R0); // 128x128px, HW IIC connection

void setup()
{
  Wire.begin(); // Initialize I2C communication

  u8g2.begin();          // Begin the u8g2 library
  u8g2.setContrast(255); // Set display contrast/brightness

  // Initialize Serial communication for debugging (optional)
  Serial.begin(115200);
  Serial.println("Display initialized!");
}

void loop()
{
  u8g2.firstPage(); // Select the first page of the display

  do
  {
    u8g2.setFont(unibody_8);            // Choose a font
    u8g2.drawStr(0, 20, "Hello Georg"); // Draw "Hello World!" at position (0, 20)
    // u8g2.setFont(unibody_16);           // Choose a font
    // u8g2.drawStr(0, 20, "lol");         // Draw "Hello World!" at position (0, 20)
  } while (u8g2.nextPage()); // Continue to the next page if available

  delay(1000); // Delay for readability, adjust as necessary
}
