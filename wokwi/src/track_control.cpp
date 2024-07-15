#include "track_control.h"
#include "led_control.h"
// #include "../midi_manager/midi_manager.h"

int currentTrack = 0;
bool muted_tracks[8] = {false};
bool soloed_tracks[8] = {false};
int faderValues[8] = {0};
// Midi Volume (0-127)
uint8_t track_volumes[8] = {0};

extern void setTrackVolume(uint8_t track, uint8_t volume);

void boot()
{
	int speed = 100;

	for (int iter = 0; iter <= 4; iter++)
	{
		for (int step = 0; step < 8; step++)
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
		mute_track(trackId, !muted_tracks[trackId]);
	}
	if (solo_pressed)
	{
		soloed_tracks[trackId] = !soloed_tracks[trackId];
	}
}

void mute_track(int track, bool state)
{
	muted_tracks[track] = state;
	if (state)
	{
		// Set volume to 0
		setTrackVolume(track, 0);
	}
	else
	{
		// Set volume to previous value
		setTrackVolume(track, track_volumes[track]);
	}
}

void solo_track(int track, bool state)
{
	soloed_tracks[track] = state;
	if (state)
	{
		// Set volume to previous value
		// setTrackVolume(track, track_volumes[track]);
	}
	else
	{
		// Set volume to 0
		// setTrackVolume(track, 0);
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
