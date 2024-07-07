// #include <Arduino.h>
// #include <Adafruit_MCP23X17.h>

// // Create MCP23017 objects
// Adafruit_MCP23X17 mcp;
// Adafruit_MCP23X17 mcp2;

// #define LED_PIN 15

// void setup()
// {
// 	// Initialize serial communication
// 	Serial.begin(9600);
// 	while (!Serial)
// 	{
// 		delay(10); // Wait for serial port to connect (useful for Leonardo)
// 	}

// 	// Initialize I2C communication
// 	Wire.begin();

// 	// Initialize MCP23017 at address 0x20
// 	if (!mcp.begin_I2C(0x20))
// 	{
// 		Serial.println("Error initializing MCP23017 at 0x20");
// 		while (1)
// 			;
// 	}

// 	mcp.pinMode(2, INPUT_PULLUP);
// 	mcp.pinMode(6, INPUT_PULLUP);
// 	mcp.pinMode(10, INPUT_PULLUP);
// 	mcp.pinMode(14, INPUT_PULLUP);
// }

// void loop()
// {
// 	// Control LEDs on mcp
// 	if (mcp.digitalRead(2) == LOW)
// 	{
// 		Serial.println("Button 2 pressed");
// 	}
// 	if (mcp.digitalRead(6) == LOW)
// 	{
// 		Serial.println("Button 6 pressed");
// 	}
// 	if (mcp.digitalRead(10) == LOW)
// 	{
// 		Serial.println("Button 10 pressed");
// 	}
// 	if (mcp.digitalRead(14) == LOW)
// 	{
// 		Serial.println("Button 14 pressed");
// 	}
// 	delay(100);
// }
