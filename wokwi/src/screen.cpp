#include <U8g2lib.h>
#include <Wire.h>
#include <ESP32Encoder.h>

#include "font/unibody_8.h"
#include "font/unibody_16.h"

U8G2_SH1107_128X128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define ENCODER_DT 2
#define ENCODER_CLK 4
#define ENCODER_SW 15
#define BUTTON_PIN 5

ESP32Encoder encoder;

int VolumeIn = 10; // Start at X% (X out of 128)
int screenHeight = 128;
int screenWidth = 128;

enum ScreenState
{
	SCREEN_VOLUME,
	SCREEN_SAMPLE_LIBRARY_FOLDER,
	SCREEN_SAMPLE_LIBRARY_SAMPLES,
	SCREEN_PRESET_LIBRARY_PRESETS,
	SCREEN_PRESET_LIBRARY_SAMPLES
};

int previewPreset = 0;		  // Initial preset selection for navigation
int previewPresetSample = 0;  // Initial preset sample selection for navigation
int selectedPreset = 0;		  // Remember selected preset
int selectedPresetSample = 0; // Remember selected preset sample

ScreenState screenStateLeft = SCREEN_RESTING;
ScreenState screenStateRight = SCREEN_RESTING;

const char *sampleFolder[] = {"1 Kick >", "2 Snare >", "3 Clap >", "4 Closed Hihat >", "5 Open Hihat >", "6 Tom >", "9 Percussions >"};
const char *kickSamples[] = {"1974 Kick", "808 Boom", "Afrofunk Kick", "Big Kick", "Boombap Kick", "Box Kick", "Crusty Kick", "Deep Kick"};
const char *snareSamples[] = {"1974 Snare", "909 Snare", "Afrobeat Snare", "Afrofunk Snare", "Air Snare", "Boombap Snare", "Brostep Snare", "Brush Snare"};
const char *clapSamples[] = {"808 Clap", "909 Clap", "Analog Clap", "Boogie Clap", "Clap Trap", "Disco Clap", "DMX Clap", "Fingersnap"};
const char *closedHihatSamples[] = {"1974 Hihat", "Afrofunk Hihat", "Boombap Hihat", "Crisp Hihat", "Dusty Hihat", "French Hihat", "Funk Hihat", "Hiphop Hihat"};
const char *openHihatSamples[] = {"626 Open", "707 Open", "808 Open", "909 Open", "Bright Open", "Cymbal Open", "DMX Open", "French Open"};
const char *tomSamples[] = {"Analog Tom", "Block Tom", "Brush Tom", "Chip Tom", "Electro Tom", "Room Tom", "Small Tom", "Syndrum"};
const char *percussionsSamples[] = {"Bongo High", "Bongo Low", "Cabasa", "Conga High", "Conga Mid", "Cowbell High", "Cowbell Tight", "Cowbell"};

const char **sampleArrays[] = {kickSamples, snareSamples, clapSamples, closedHihatSamples, openHihatSamples, tomSamples, percussionsSamples};
const int sampleCounts[] = {8, 8, 8, 8, 8, 8, 8}; // Number of samples in each category

const char *presetList[] = {"A Clean Start", "808 vs 909", "Bassline Timewarp", "Preset 04", "Preset 05"};
const int presetCount = sizeof(presetList) / sizeof(presetList[0]);

int previewFolder = 0;	// Initial folder selection for navigation
int previewSample = 0;	// Initial sample selection for navigation
int selectedFolder = 0; // Remember selected folder
int selectedSample = 0; // Remember selected sample

bool updateScreen = true; // Flag to control screen update

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

int encoderStepSize = 2; // Adjust the encoder step size for sensitive navigation

unsigned long previousMillis = 0;
const long interval = 2000; // 2 seconds interval for blinking
bool circleVisible = true;

void volumeChange()
{
	// Volume screen logic
	int encoderChange = encoder.getCount();
	if (encoderChange != 0)
	{
		VolumeIn += encoderChange * 4;
		VolumeIn = constrain(VolumeIn, 0, 128); // Constrain volume between 0 and 128
		encoder.setCount(VolumeIn);				// Keep the encoder in sync with the volume
		encoder.clearCount();					// Clear encoder change after applying

		updateScreen = true; // Ensure screen update when volume changes
	}
}

void folderSelection()
{
	// Folder selection screen logic
	int encoderChange = encoder.getCount();
	if (encoderChange != 0)
	{
		int steps = encoderChange / encoderStepSize;
		previewFolder += steps;

		// Ensure previewFolder stays within valid range
		previewFolder = constrain(previewFolder, 0, sizeof(sampleFolder) / sizeof(sampleFolder[0]) - 1);

		// Reset encoder count after processing movement
		encoder.clearCount();

		updateScreen = true; // Trigger screen update on folder selection change
	}
}
void drawUI(int screenId, ScreenState screenState)
{
	switch (screenState)
	{
	case SCREEN_VOLUME:
		// Draw the vertical line and the volume progress bar
		int lineX = 5;
		int lineYTop = 5;
		int lineYBottom = 123;
		int barWidth = 3;
		int barX = lineX - 1;

		u8g2.drawLine(lineX, lineYTop, lineX, lineYBottom);

		int totalHeight = lineYBottom - lineYTop;
		int filledHeight = map(VolumeIn, 0, 119, 0, totalHeight);
		filledHeight = constrain(filledHeight, 0, 119); // Ensure the height does not exceed 118px

		u8g2.setDrawColor(1);
		u8g2.drawBox(barX, lineYBottom - filledHeight, barWidth, filledHeight);

		// Draw the blinking circle
		if (circleVisible)
		{
			u8g2.drawDisc(20, 15, 2);
		}

		// Draw a small bar in the top right corner
		u8g2.drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top

		u8g2.setFont(unibody_8);
		u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Sample Library"), 18, "Sample Library");

		// Display folder name in full
		u8g2.drawStr(128 - 5 - u8g2.getStrWidth(sampleFolder[selectedFolder]), 128 - 40, sampleFolder[selectedFolder]);

		// Truncate sample name if it is longer than 8 characters
		char displayedSample[12];
		snprintf(displayedSample, sizeof(displayedSample), "%.6s...", sampleArrays[selectedFolder][selectedSample]);
		u8g2.setFont(unibody_16);
		u8g2.drawStr(128 - 5 - u8g2.getStrWidth(displayedSample), 128 - 20, displayedSample);
		break;
	case SCREEN_SAMPLE_LIBRARY_FOLDER:
		// Draw
		u8g2.setFont(unibody_8);
		u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Back"), 18, "Back");

		int lineHeight = u8g2.getAscent() - u8g2.getDescent();
		int verticalSpacing = 5;
		int baseY = 128 - 6 * (lineHeight + verticalSpacing);

		for (int i = -1; i <= 4; ++i)
		{
			int index = previewFolder + i;
			if (index >= 0 && index < (sizeof(sampleFolder) / sizeof(sampleFolder[0])))
			{
				int y = baseY + (i + 1) * (lineHeight + verticalSpacing);
				u8g2.drawStr(5, y, sampleFolder[index]);
				if (i == 0)
				{
					u8g2.drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
				}
			}
		}

		// Draw a small bar in the top right corner
		u8g2.drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
	}
}

void sampleSelection()
{
	// Sample selection screen logic
	int encoderChange = encoder.getCount();
	if (encoderChange != 0)
	{
		int steps = encoderChange / encoderStepSize;
		previewSample += steps;

		// Ensure previewSample stays within valid range
		int sampleCount = sampleCounts[previewFolder];
		previewSample = constrain(previewSample, 0, sampleCount - 1);

		// Reset encoder count after processing movement
		encoder.clearCount();

		updateScreen = true; // Trigger screen update on sample selection change
	}

	// Draw
	u8g2.setFont(unibody_8);
	u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Back"), 18, "Back");

	int lineHeight = u8g2.getAscent() - u8g2.getDescent();
	int verticalSpacing = 5;
	int baseY = 128 - 6 * (lineHeight + verticalSpacing);

	const char **currentSamples = sampleArrays[previewFolder];
	int sampleCount = sampleCounts[previewFolder];

	for (int i = -1; i <= 4; ++i)
	{
		int index = previewSample + i;
		if (index >= 0 && index < sampleCount)
		{
			int y = baseY + (i + 1) * (lineHeight + verticalSpacing);
			u8g2.drawStr(5, y, currentSamples[index]);
			if (i == 0)
			{
				u8g2.drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
			}
		}
	}

	// Draw a small bar in the top right corner
	u8g2.drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
}

void presetSelection()
{
	// Sample selection screen logic
	int encoderChange = encoder.getCount();
	if (encoderChange != 0)
	{
		int steps = encoderChange / encoderStepSize;
		previewSample += steps;

		// Ensure previewSample stays within valid range
		int sampleCount = sampleCounts[previewFolder];
		previewSample = constrain(previewSample, 0, sampleCount - 1);

		// Reset encoder count after processing movement
		encoder.clearCount();

		updateScreen = true; // Trigger screen update on sample selection change
	}
}

void blinkingCircle()
{
	// Handle blinking circle
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval)
	{
		previousMillis = currentMillis;
		circleVisible = !circleVisible;
		updateScreen = true; // Ensure screen update for blinking circle
	}
}

void setup()
{
	Wire.begin();
	u8g2.begin();
	u8g2.setContrast(255);

	Serial.begin(115200);
	Serial.println("Display initialized!");

	ESP32Encoder::useInternalWeakPullResistors = puType::up;

	encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
	encoder.clearCount();

	pinMode(ENCODER_SW, INPUT_PULLUP);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	// Initialize encoder with the current volume
	encoder.setCount(VolumeIn);
}

void loop()
{
	// Handle button press to toggle between screens
	if (digitalRead(BUTTON_PIN) == LOW)
	{
		if (screenState == SCREEN_RESTING)
		{
			screenState = SCREEN_SAMPLE_LIBRARY_FOLDER;
			encoder.clearCount(); // Clear encoder count when switching screens
			updateScreen = true;  // Ensure screen update when switching to folder view
		}
		else if (screenState == SCREEN_SAMPLE_LIBRARY_FOLDER)
		{
			screenState = SCREEN_RESTING;
			encoder.setCount(VolumeIn); // Ensure encoder reflects the current volume
			updateScreen = true;		// Ensure screen update when switching back to volume screen
		}
		else if (screenState == SCREEN_SAMPLE_LIBRARY_SAMPLES)
		{
			screenState = SCREEN_SAMPLE_LIBRARY_FOLDER;
			encoder.clearCount(); // Clear encoder count when switching screens
			updateScreen = true;  // Ensure screen update when switching to folder view
		}
		delay(300); // Debounce delay
	}

	// Handle encoder button press for sample selection and switch back to volume screen
	if (digitalRead(ENCODER_SW) == LOW)
	{
		if (screenState == SCREEN_SAMPLE_LIBRARY_FOLDER)
		{
			// Move to sample list screen
			screenState = SCREEN_SAMPLE_LIBRARY_SAMPLES;
			encoder.clearCount();
			updateScreen = true;
		}
		else if (screenState == SCREEN_SAMPLE_LIBRARY_SAMPLES)
		{
			// Confirm selection and switch back to volume screen
			screenState = SCREEN_RESTING;
			selectedFolder = previewFolder; // Store selected folder
			selectedSample = previewSample; // Store selected sample
			updateScreen = true;			// Trigger screen update on return to volume screen
			encoder.setCount(VolumeIn);		// Ensure encoder reflects the current volume
			delay(300);						// Debounce delay
		}
	}

	// ___SCREENS___
	if (screenState == SCREEN_RESTING)
	{
		// Volume screen logic
		volumeChange();
	}
	else if (screenState == SCREEN_SAMPLE_LIBRARY_FOLDER)
	{
		// Folder selection screen logic
		folderSelection();
	}
	else if (screenState == SCREEN_SAMPLE_LIBRARY_SAMPLES)
	{
		// Sample selection screen logic
		sampleSelection();
	}

	// Handle blinking circle
	blinkingCircle();

	// Render the display based on screen state and update condition
	u8g2.firstPage();
	// ___DRAW___

	do
	{
		// Update based on screen state
		if (screenState == SCREEN_RESTING)
		{
			// Volume screen logic
			drawRestingScreen(0);
			drawRestingScreen(1);
		}
		else if (screenState == SCREEN_SAMPLE_LIBRARY_FOLDER)
		{
			// Folder selection screen logic
			folderSelection();
		}
		else if (screenState == SCREEN_SAMPLE_LIBRARY_SAMPLES)
		{
			// Sample selection screen logic
			sampleSelection();
		}
	} while (u8g2.nextPage());

	if (updateScreen)
	{
		updateScreen = false; // Reset update flag
	}
}
