#include "fader_controller.h"

void moveTo(int targetPosition)
{
	Serial.println("Moving fader to " + String(targetPosition));
	int currentPosition = analogRead(FADER_POT);

	while (abs(targetPosition - currentPosition) > 50)
	{
		int distance = abs(targetPosition - currentPosition);
		int speed = map(distance, 0, 1023, 170, 220);

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
		analogWrite(FADER_SPEED, speed);

		currentPosition = analogRead(FADER_POT);
		Serial.print("Current Position: ");
		Serial.println(currentPosition);
	}

	digitalWrite(FADER_FORWARD, LOW);
	digitalWrite(FADER_REVERSE, LOW);
	analogWrite(FADER_SPEED, 0);
}
