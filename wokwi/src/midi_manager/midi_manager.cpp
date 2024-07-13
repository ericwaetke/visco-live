#include "midi_manager.h"
#include <Control_Surface.h>
#include <pins.h>

USBMIDI_Interface midi;
CCPotentiometer pot0{POT0, MIDI_CC::General_Purpose_Controller_1};
CCPotentiometer pot1{POT1, MIDI_CC::General_Purpose_Controller_2};
CCPotentiometer pot2{POT2, MIDI_CC::General_Purpose_Controller_3};
CCPotentiometer pot3{POT3, MIDI_CC::General_Purpose_Controller_4};
CCPotentiometer pot4{POT4, MIDI_CC::General_Purpose_Controller_5};

Bank<4> bank(1);
Bankable::CCPotentiometer fader[]{
    {bank, FADER_POT, 1},
};

// Set volume of tracks 1–8
void setTrackVolume(uint8_t track, uint8_t volume)
{
    MIDIAddress controller = {MIDI_CC::Sound_Controller_1 + (track - 1)};
    uint8_t value = volume;
    midi.sendCC(controller, value);
}