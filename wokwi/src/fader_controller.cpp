#include "fader_controller.h"

void moveTo(int targetPosition)
{
	Serial.println("Moving fader to " + String(targetPosition));
	int currentPosition = analogRead(FADER_POT);

	while (abs(targetPosition - currentPosition) > 50)
	{
		int distance = abs(targetPosition - currentPosition);

		if (targetPosition > currentPosition)
		{
			digitalWrite(FADER_FORWARD, HIGH);
			digitalWrite(FADER_REVERSE, LOW);
		}
		else
		{
			digitalWrite(FADER_FORWARD, LOW);
			digitalWrite(FADER_REVERSE, HIGH);
		}
		// analogWrite(FADER_SPEED, 3000);
		digitalWrite(FADER_SPEED, HIGH);

		currentPosition = analogRead(FADER_POT);
		Serial.print("Current Position: ");
		Serial.println(currentPosition);
	}

	digitalWrite(FADER_FORWARD, LOW);
	digitalWrite(FADER_REVERSE, LOW);
	// analogWrite(FADER_SPEED, 0);
	digitalWrite(FADER_SPEED, LOW);
}
