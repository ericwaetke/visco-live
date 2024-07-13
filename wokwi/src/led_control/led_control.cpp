#include "led_control.h"
#include <Adafruit_MCP23X17.h>
#include <pins.h>
#include "../mcp_manager/mcp_manager.h"

extern int currentTrack;
extern bool muted_tracks[8];
extern bool soloed_tracks[8];

void lightTrackLed()
{
	selected_led(currentTrack);

	for (int i = 0; i < 8; i++)
	{
		mute_led(i, muted_tracks[i]);
		solo_led(i, soloed_tracks[i]);
	}
}

void selected_led(int selected)
{
	for (int i = 0; i < 8; i++)
	{
		int ledPin = TRACK_1_LED_SELECTED + ((i % 4) * 4);
		int state = (selected == i) ? HIGH : LOW;
		getMcpForTrack(i)->digitalWrite(ledPin, state);
	}
}

void mute_led(int track, bool state)
{
	int ledPin = TRACK_1_LED_MUTE + (track % 4) * 4;
	getMcpForTrack(track)->digitalWrite(ledPin, state ? HIGH : LOW);
}

void solo_led(int track, bool state)
{
	int ledPin = TRACK_1_LED_SOLO + (track % 4) * 4;
	getMcpForTrack(track)->digitalWrite(ledPin, state ? HIGH : LOW);
}
