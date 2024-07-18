#include "fader_controller.h"
#include <Arduino.h>

const int POSITION_THRESHOLD = 2;
const int MIN_SPEED = 150;
const int MAX_SPEED = 255;
const int ANALOG_MAX = 4095;
const unsigned long TIMEOUT = 10000; // 10 seconds

void stopMotor() {
    digitalWrite(FADER_FORWARD, LOW);
    digitalWrite(FADER_REVERSE, LOW);
    analogWrite(FADER_SPEED, 0);
}

int getFilteredAnalogRead(int pin) {
    const int numReadings = 5;
    int readings[numReadings];
    int total = 0;
    
    for (int i = 0; i < numReadings; i++) {
        readings[i] = analogRead(pin);
        delay(10);
    }

    for (int i = 0; i < numReadings; i++) {
        total += readings[i];
    }
    
    return total / numReadings;
}

void moveTo(int targetPosition) {
    int currentPosition = analogRead(FADER_POT);
    Serial.println("Initial Target: " + String(targetPosition) + " - " + "Initial Current: " + String(currentPosition));
    bool forward = targetPosition > currentPosition;
    unsigned long startTime = millis();

    while (abs(targetPosition - currentPosition) > POSITION_THRESHOLD) {
        if (millis() - startTime > TIMEOUT) {
            Serial.println("Error: Movement timeout");
            break;
        }

        if ((!forward && currentPosition <= 0) || (forward && currentPosition >= ANALOG_MAX)) {
            Serial.println("Error: Current position out of range: " + String(currentPosition));
            break;
        }

        int distance = abs(targetPosition - currentPosition);
        int speed = map(distance, 30, ANALOG_MAX, MIN_SPEED, MAX_SPEED);
        speed = max(speed, MIN_SPEED);
        if (distance < 30) { speed = 100; }

        if (forward) {
            // Serial.println("Moving Forward");
            digitalWrite(FADER_FORWARD, LOW);
            digitalWrite(FADER_REVERSE, HIGH);
        } else {
            // Serial.println("Moving Reverse");
            digitalWrite(FADER_FORWARD, HIGH);
            digitalWrite(FADER_REVERSE, LOW);
        }
        
        analogWrite(FADER_SPEED, speed);
        currentPosition = analogRead(FADER_POT);

        // Serial.print("Current Position: ");
        // Serial.println(currentPosition);
        forward = targetPosition > currentPosition;
    }

    stopMotor();
}
