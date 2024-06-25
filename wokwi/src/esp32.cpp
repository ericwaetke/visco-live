#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

/*
  Fontname: -*-Unibody 8 Pro Reg-Medium-R-Normal--21-160-96-96-c-18--fontspecific
  Copyright: You may wish to put your copyright here
  Glyphs: 96/224
  BBX Build Mode: 0
*/

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
    // u8g2.setFont(unibody_16_reg); // Choose a font
    u8g2.drawStr(0, 20, "Hello World!"); // Draw "Hello World!" at position (0, 20)
  } while (u8g2.nextPage()); // Continue to the next page if available

  delay(1000); // Delay for readability, adjust as necessary
}
