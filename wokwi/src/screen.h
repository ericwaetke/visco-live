#ifndef SCREEN_H
#define SCREEN_H

#include <U8g2lib.h>
#include <Wire.h>
#include <ESP32Encoder.h>
#include "font/unibody_8.h"
#include "font/unibody_16.h"

// Screen states enumeration
enum ScreenState {
    SCREEN_SAMPLE_LIBRARY_FOLDER,
    SCREEN_SAMPLE_LIBRARY_SAMPLES,
    SCREEN_PRESET_LIBRARY_PRESETS,
    SCREEN_PRESET_LIBRARY_SAMPLES,
    SCREEN_RESTING
};

// Function declarations
void screenSetup();
void screenLoop();
U8G2_SH1107_PIMORONI_128X128_1_HW_I2C* getScreenFromID(int id);

#endif
