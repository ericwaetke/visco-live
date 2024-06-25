#include <Adafruit_MCP23X17.h>

// Create MCP23017 objects
Adafruit_MCP23X17 mcp;
Adafruit_MCP23X17 mcp2;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // Wait for serial port to connect (useful for Leonardo)
  }

  // Initialize I2C communication
  Wire.begin();

  // Initialize MCP23017 at address 0x20
  if (!mcp.begin_I2C(0x20)) {
    Serial.println("Error initializing MCP23017 at 0x20");
    while(1);
  }

  // Initialize MCP23017 at address 0x21
  if (!mcp2.begin_I2C(0x21)) {
    Serial.println("Error initializing MCP23017 at 0x21");
    while(1);
  }

  // Set MCP23017 pins as outputs
  for (uint8_t pin = 8; pin <= 13; ++pin) {
    mcp.pinMode(pin, OUTPUT);
    mcp2.pinMode(pin, OUTPUT);
  }
}

void loop() {
  // Control LEDs on mcp
  mcp.digitalWrite(8, HIGH);
  delay(1000);  // Keep LED on for 1 second
  mcp.digitalWrite(8, LOW);
  
  mcp.digitalWrite(9, HIGH);
  delay(1000);  // Keep LED on for 1 second
  mcp.digitalWrite(9, LOW);
  delay(1000);  // Keep LED off for 1 second

  // Control LEDs on mcp2
  mcp2.digitalWrite(8, HIGH);
  delay(1000);  // Keep LED on for 1 second
  mcp2.digitalWrite(8, LOW);
  
  mcp2.digitalWrite(9, HIGH);
  delay(1000);  // Keep LED on for 1 second
  mcp2.digitalWrite(9, LOW);
  delay(1000);  // Keep LED off for 1 second
}
