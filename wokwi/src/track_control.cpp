#include "track_control.h"

int currentTrack = 0;
bool muted_tracks[8] = {false};
bool soloed_tracks[8] = {false};
int faderValues[8] = {0};
// Midi Volume (0-127)
uint8_t track_volumes[8] = {64, 64, 64, 64, 64, 64, 64, 64};

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
		faderValues[currentTrack] = analogRead(FADER_POT);
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
		solo_track(trackId);
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

void solo_track(int track)
{
	bool state = !soloed_tracks[track];
	soloed_tracks[track] = state;

	// If no track is soloed, unmute all tracks
	if (std::none_of(std::begin(soloed_tracks), std::end(soloed_tracks), [](bool b)
					 { return b; }))
	{
		for (int i = 0; i < 8; i++)
		{
			if (!muted_tracks[i])
			{
				setTrackVolume(i, track_volumes[i]);
			}
		}
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		Serial.println("Soloing Track: " + String(i));
		if (!soloed_tracks[i] || muted_tracks[i])
		{
			Serial.println("Muting Track: " + String(i));
			setTrackVolume(i, 0);
			// return;
		}
		else
		{
			Serial.println("Unmuting Track: " + String(i));
			setTrackVolume(i, track_volumes[i]);
		}
	}
}

void switchToTrack(int trackId)
{
	Serial.println("Switching to Track: " + String(trackId));
  moveTo(faderValues[trackId]);
}
