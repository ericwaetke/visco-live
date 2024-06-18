#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// Create an MCP23017 object
Adafruit_MCP23X17 mcp;

#define HEARTBEAT 13

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Arduino");
  // Start the I2C communication
  Wire.begin();

  // Initialize the MCP23017 with the default I2C address (0x20)
  if (!mcp.begin_I2C()) {
    Serial.println("Error Initializing MCP23017");
    while (1);
  }
  Serial.println("Initialized MCP23017");

  // Set GPA7 as an output pin
  mcp.pinMode(7, OUTPUT);
  mcp.pinMode(8, OUTPUT);

  pinMode(HEARTBEAT, OUTPUT);
}

void loop() {
  // Turn the LED on (GPA7 high)
  mcp.digitalWrite(7, HIGH);
  mcp.digitalWrite(8, LOW);
  delay(1000); // Wait for 1 second

  // Turn the LED off (GPA7 low)
  mcp.digitalWrite(7, LOW);
  mcp.digitalWrite(8, HIGH);
  delay(1000); // Wait for 1 second
}
