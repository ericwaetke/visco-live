#ifndef FADER_CONTROLLER_H
#define FADER_CONTROLLER_H

#include <Arduino.h>
#include "pins.h"

// extern int PWM_CHANNEL;         // PWM channel
// extern int PWM_FREQUENCY;   // PWM frequency in Hz
// extern int PWM_RESOLUTION;      // PWM resolution in bits

// Function to move the fader to the target position
void moveTo(int targetPosition);
void stopMotor();

#endif // FADER_CONTROLLER_H