#ifndef MIDI_MANAGER_H
#define MIDI_MANAGER_H

#include <Control_Surface.h>

// MIDI Interface
extern USBMIDI_Interface midi;

// Potentiometers for MIDI control
extern CCPotentiometer pot0;
extern CCPotentiometer pot1;
extern CCPotentiometer pot2;
extern CCPotentiometer pot3;
extern CCPotentiometer pot4;

// Bank and Bankable fader for track volume control
extern Bank<4> bank;
extern Bankable::CCPotentiometer fader[];

// Function to set the volume of a specific track
void setTrackVolume(uint8_t track, uint8_t volume);

#endif // MIDI_MANAGER_H
