#ifndef TRACK_CONTROL_H
#define TRACK_CONTROL_H

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

#include "led_control.h"
#include "fader_controller.h"

extern Adafruit_MCP23X17 mcp0;
extern Adafruit_MCP23X17 mcp_other;

void boot();
void updateTrack(int trackId, bool mute_pressed, bool solo_pressed);
void mute_track(int track, bool state);
void solo_track(int track);
void switchToTrack(int trackId);

#endif // TRACK_CONTROL_H
