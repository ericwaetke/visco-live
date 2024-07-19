#include "debounce.h"

const int debounceDelay = 50;			  // milliseconds
unsigned long lastDebounceTime[10] = {0}; // Array to store the last debounce time for each button
bool lastButtonState[10] = {HIGH};		  // Array to store the last button state
bool buttonState[10] = {HIGH};			  // Array to store the current button state

bool debounceButton(int index, bool currentState)
{
	bool reading = currentState == LOW;

	if (reading != lastButtonState[index])
	{
		lastDebounceTime[index] = millis();
	}

	if ((millis() - lastDebounceTime[index]) > debounceDelay)
	{
		if (reading != buttonState[index])
		{
			buttonState[index] = reading;
			if (buttonState[index])
			{
				lastButtonState[index] = reading;
				return true;
			}
		}
	}

	lastButtonState[index] = reading;
	return false;
}
