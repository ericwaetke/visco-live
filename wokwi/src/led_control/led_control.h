#ifndef LED_CONTROL_H
#define LED_CONTROL_H

void lightTrackLed();
void selected_led(int selected);
void mute_led(int track, bool state);
void solo_led(int track, bool state);

#endif // LED_CONTROL_H
