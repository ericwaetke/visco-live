#include "screen.h"

// Encoder setup
ESP32Encoder encoder;

// Initial volume and screen dimensions
int VolumeIn = 10;
const int screenHeight = 128;
const int screenWidth = 128;

// Navigation variables
int previewPreset = 0;
int previewPresetSample = 0;
int selectedPreset = 0;
int selectedPresetSample = 0;

// Sample data
const char *sampleFolder[] = {"1 Kick >", "2 Snare >", "3 Clap >", "4 Closed Hihat >", "5 Open Hihat >", "6 Tom >", "9 Percussions >"};
const char *kickSamples[] = {"1974 Kick", "808 Boom", "Afrofunk Kick", "Big Kick", "Boombap Kick", "Box Kick", "Crusty Kick", "Deep Kick"};
const char *snareSamples[] = {"1974 Snare", "909 Snare", "Afrobeat Snare", "Afrofunk Snare", "Air Snare", "Boombap Snare", "Brostep Snare", "Brush Snare"};
const char *clapSamples[] = {"808 Clap", "909 Clap", "Analog Clap", "Boogie Clap", "Clap Trap", "Disco Clap", "DMX Clap", "Fingersnap"};
const char *closedHihatSamples[] = {"1974 Hihat", "Afrofunk Hihat", "Boombap Hihat", "Crisp Hihat", "Dusty Hihat", "French Hihat", "Funk Hihat", "Hiphop Hihat"};
const char *openHihatSamples[] = {"626 Open", "707 Open", "808 Open", "909 Open", "Bright Open", "Cymbal Open", "DMX Open", "French Open"};
const char *tomSamples[] = {"Analog Tom", "Block Tom", "Brush Tom", "Chip Tom", "Electro Tom", "Room Tom", "Small Tom", "Syndrum"};
const char *percussionsSamples[] = {"Bongo High", "Bongo Low", "Cabasa", "Conga High", "Conga Mid", "Cowbell High", "Cowbell Tight", "Cowbell"};

const char **sampleArrays[] = {kickSamples, snareSamples, clapSamples, closedHihatSamples, openHihatSamples, tomSamples, percussionsSamples};
const int sampleCounts[] = {8, 8, 8, 8, 8, 8, 8};

// Preset data
const char *presetList[] = {"A Clean Start", "808 vs 909", "Bassline Timewarp", "Preset 04", "Preset 05"};
const int presetCount = sizeof(presetList) / sizeof(presetList[0]);

const char *presetSamples[][16] = {
    {"1974 Kick", "808 Boom", "Afrofunk Kick", "Big Kick", "Boombap Kick", "Box Kick", "Crusty Kick", "Deep Kick",
     "1974 Snare", "909 Snare", "Afrobeat Snare", "Afrofunk Snare", "Air Snare", "Boombap Snare", "Brostep Snare", "Brush Snare"},
    {"808 Boom", "909 Snare", "808 Clap", "Crisp Hihat", "French Open", "Small Tom", "Cabasa", "Conga Mid",
     "Box Kick", "Boombap Snare", "Boogie Clap", "Funk Hihat", "Bright Open", "Electro Tom", "Cowbell Tight", "Brush Snare"},
    {"Afrofunk Kick", "Afrobeat Snare", "Analog Clap", "Boombap Hihat", "DMX Open", "Room Tom", "Cowbell High", "Conga High",
     "Big Kick", "Air Snare", "Disco Clap", "Dusty Hihat", "626 Open", "Analog Tom", "Bongo Low", "French Hihat"},
    {"Deep Kick", "Brostep Snare", "DMX Clap", "Dusty Hihat", "Cymbal Open", "Syndrum", "Cowbell", "Bongo High",
     "Boombap Kick", "Boombap Snare", "Fingersnap", "Afrofunk Hihat", "707 Open", "Brush Tom", "Bongo Low", "Funk Hihat"},
    {"Crusty Kick", "Brush Snare", "Analog Clap", "Hiphop Hihat", "808 Open", "Electro Tom", "Cabasa", "Conga High",
     "Big Kick", "Afrobeat Snare", "Clap Trap", "Crisp Hihat", "Bright Open", "Analog Tom", "Cowbell Tight", "French Hihat"}};
const int presetSampleCounts[] = {16, 16, 16, 16, 16};

// Encoder step size
int encoderStepSize = 2;

// Blinking circle variables
unsigned long previousMillis = 0;
const long interval = 2000; // 2 seconds interval for blinking
bool circleVisible = true;

// Screen state variables
ScreenState screenStateRight = SCREEN_RESTING;
ScreenState screenStateLeft = SCREEN_RESTING;

int previewFolder = 0;  // Initial folder selection for navigation
int previewSample = 0;  // Initial sample selection for navigation
int selectedFolder = 0; // Remember selected folder
int selectedSample = 0; // Remember selected sample

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C display_1(U8G2_R1, U8X8_PIN_NONE);
U8G2_SH1107_PIMORONI_128X128_1_HW_I2C display_2(U8G2_R3, U8X8_PIN_NONE);

// Select I2C BUS
void TCA9548A(uint8_t bus) {
    Wire.beginTransmission(0x70); // TCA9548A address
    Wire.write(1 << bus);         // send byte to select bus
    Wire.endTransmission();
    Serial.print(bus);
}

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C* getScreenFromID(int id) {
    switch (id) {
        case 1:
            return &display_1;
        case 2:
            return &display_2;
        default:
            return &display_1;
    }
}

void drawRestingScreen(int screenId) {
    int lineX = 5;
    int lineYTop = 5;
    int lineYBottom = 123;
    int barWidth = 3;
    int barX = lineX - 1;

    getScreenFromID(screenId)->drawLine(lineX, lineYTop, lineX, lineYBottom);

    int totalHeight = lineYBottom - lineYTop;
    int filledHeight = map(VolumeIn, 0, 119, 0, totalHeight);
    filledHeight = constrain(filledHeight, 0, 119);

    getScreenFromID(screenId)->setDrawColor(1);
    getScreenFromID(screenId)->drawBox(barX, lineYBottom - filledHeight, barWidth, filledHeight);

    if (circleVisible) {
        getScreenFromID(screenId)->drawDisc(20, 15, 2);
    }

    getScreenFromID(screenId)->drawBox(115, 0, 2, 5);

    getScreenFromID(screenId)->setFont(unibody_8);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Sample Library"), 18, "Sample Library");

    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth(sampleFolder[selectedFolder]), 128 - 40, sampleFolder[selectedFolder]);

    char displayedSample[12];
    snprintf(displayedSample, sizeof(displayedSample), "%.6s...", sampleArrays[selectedFolder][selectedSample]);
    getScreenFromID(screenId)->setFont(unibody_16);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth(displayedSample), 128 - 20, displayedSample);
}

void drawSampleLibraryFolder(int screenId) {
    getScreenFromID(screenId)->setFont(unibody_8);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

    int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
    int verticalSpacing = 5;
    int baseY = 128 - 6 * (lineHeight + verticalSpacing);

    for (int i = -1; i <= 4; ++i) {
        int index = previewFolder + i;
        if (index >= 0 && index < (sizeof(sampleFolder) / sizeof(sampleFolder[0]))) {
            int yPos = baseY + (i + 1) * (lineHeight + verticalSpacing);
            if (index == previewFolder) {
                getScreenFromID(screenId)->drawStr(2, yPos, sampleFolder[index]);
            } else {
                getScreenFromID(screenId)->drawStr(15, yPos, sampleFolder[index]);
            }
        }
    }
}

void drawSampleLibrarySamples(int screenId) {
    getScreenFromID(screenId)->setFont(unibody_8);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

    int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
    int verticalSpacing = 5;
    int baseY = 128 - 6 * (lineHeight + verticalSpacing);

    for (int i = -1; i <= 4; ++i) {
        int index = previewSample + i;
        if (index >= 0 && index < sampleCounts[selectedFolder]) {
            int yPos = baseY + (i + 1) * (lineHeight + verticalSpacing);
            if (index == previewSample) {
                getScreenFromID(screenId)->drawStr(2, yPos, sampleArrays[selectedFolder][index]);
            } else {
                getScreenFromID(screenId)->drawStr(15, yPos, sampleArrays[selectedFolder][index]);
            }
        }
    }
}

void drawPresetLibraryPresets(int screenId) {
    getScreenFromID(screenId)->setFont(unibody_8);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

    int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
    int verticalSpacing = 5;
    int baseY = 128 - 6 * (lineHeight + verticalSpacing);

    for (int i = -1; i <= 4; ++i) {
        int index = previewPreset + i;
        if (index >= 0 && index < presetCount) {
            int yPos = baseY + (i + 1) * (lineHeight + verticalSpacing);
            if (index == previewPreset) {
                getScreenFromID(screenId)->drawStr(2, yPos, presetList[index]);
            } else {
                getScreenFromID(screenId)->drawStr(15, yPos, presetList[index]);
            }
        }
    }
}

void drawPresetLibrarySamples(int screenId) {
    getScreenFromID(screenId)->setFont(unibody_8);
    getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

    int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
    int verticalSpacing = 5;
    int baseY = 128 - 6 * (lineHeight + verticalSpacing);

    for (int i = -1; i <= 4; ++i) {
        int index = previewPresetSample + i;
        if (index >= 0 && index < presetSampleCounts[previewPreset]) {
            int yPos = baseY + (i + 1) * (lineHeight + verticalSpacing);
            if (index == previewPresetSample) {
                getScreenFromID(screenId)->drawStr(2, yPos, presetSamples[previewPreset][index]);
            } else {
                getScreenFromID(screenId)->drawStr(15, yPos, presetSamples[previewPreset][index]);
            }
        }
    }
}

void drawScreen(int screenId) {
    getScreenFromID(screenId)->firstPage();
    do {
        switch (screenId) {
            case 1:
                switch (screenStateRight) {
                    case SCREEN_SAMPLE_LIBRARY_FOLDER:
                        drawSampleLibraryFolder(screenId);
                        break;
                    case SCREEN_SAMPLE_LIBRARY_SAMPLES:
                        drawSampleLibrarySamples(screenId);
                        break;
                    case SCREEN_PRESET_LIBRARY_PRESETS:
                        drawPresetLibraryPresets(screenId);
                        break;
                    case SCREEN_PRESET_LIBRARY_SAMPLES:
                        drawPresetLibrarySamples(screenId);
                        break;
                    default:
                        drawRestingScreen(screenId);
                        break;
                }
                break;
            case 2:
                switch (screenStateLeft) {
                    case SCREEN_SAMPLE_LIBRARY_FOLDER:
                        drawSampleLibraryFolder(screenId);
                        break;
                    case SCREEN_SAMPLE_LIBRARY_SAMPLES:
                        drawSampleLibrarySamples(screenId);
                        break;
                    case SCREEN_PRESET_LIBRARY_PRESETS:
                        drawPresetLibraryPresets(screenId);
                        break;
                    case SCREEN_PRESET_LIBRARY_SAMPLES:
                        drawPresetLibrarySamples(screenId);
                        break;
                    default:
                        drawRestingScreen(screenId);
                        break;
                }
                break;
            default:
                break;
        }
    } while (getScreenFromID(screenId)->nextPage());
}

void screenSetup() {
    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    encoder.attachHalfQuad(19, 18);
    encoder.clearCount();

    Wire.begin(21, 22);

    TCA9548A(0);
    display_1.begin();
    display_1.setFlipMode(1);
    display_1.setFont(unibody_8);

    TCA9548A(1);
    display_2.begin();
    display_2.setFlipMode(1);
    display_2.setFont(unibody_8);

    encoderStepSize = 2; // Set step size for encoder
}

void updateEncoder() {
    static int lastEncoderValue = 0;
    int newValue = encoder.getCount();
    if (newValue != lastEncoderValue) {
        int delta = (newValue - lastEncoderValue) / encoderStepSize;
        switch (screenStateRight) {
            case SCREEN_SAMPLE_LIBRARY_FOLDER:
                previewFolder = constrain(previewFolder + delta, 0, (sizeof(sampleFolder) / sizeof(sampleFolder[0])) - 1);
                break;
            case SCREEN_SAMPLE_LIBRARY_SAMPLES:
                previewSample = constrain(previewSample + delta, 0, sampleCounts[selectedFolder] - 1);
                break;
            case SCREEN_PRESET_LIBRARY_PRESETS:
                previewPreset = constrain(previewPreset + delta, 0, presetCount - 1);
                break;
            case SCREEN_PRESET_LIBRARY_SAMPLES:
                previewPresetSample = constrain(previewPresetSample + delta, 0, presetSampleCounts[previewPreset] - 1);
                break;
            default:
                break;
        }
        lastEncoderValue = newValue;
    }
}

void screenLoop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        circleVisible = !circleVisible;
    }
    updateEncoder();

    TCA9548A(0);
    drawScreen(1);

    TCA9548A(1);
    drawScreen(2);
}
