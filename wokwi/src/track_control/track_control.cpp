#include "track_control.h"
#include "../led_control/led_control.h"

int currentTrack = 0;
bool muted_tracks[8] = {false};
bool soloed_tracks[8] = {false};
int faderValues[8] = {0};

void boot()
{
	int speed = 100;

	for (int iter = 0; iter <= 10; iter++)
	{
		for (int step = 0; step < 4; step++)
		{
			selected_led(-1);
			mute_led(0, LOW);
			solo_led(0, LOW);
			selected_led(step);
			delay(speed);
		}
	}

	selected_led(-1);
	mute_led(0, LOW);
	solo_led(0, LOW);
}

void updateTrack(int trackId, bool mute_pressed, bool solo_pressed)
{
	if (!mute_pressed && !solo_pressed)
	{
		currentTrack = trackId;
		switchToTrack(currentTrack);
		return;
	}

	if (mute_pressed)
	{
		muted_tracks[trackId] = !muted_tracks[trackId];
		Serial.println("Mute Track: " + String(trackId) + " - " + String(muted_tracks[trackId]));
	}
	if (solo_pressed)
	{
		soloed_tracks[trackId] = !soloed_tracks[trackId];
		Serial.println("Solo Track: " + String(trackId) + " - " + String(soloed_tracks[trackId]));
	}
}

void switchToTrack(int trackId)
{
	Serial.println("Switching to Track: " + String(trackId));
	if (faderValues[trackId])
	{
		// moveTo(faderValues[trackId] * 8);
	}
	else
	{
		Serial.println("No value");
	}
}
