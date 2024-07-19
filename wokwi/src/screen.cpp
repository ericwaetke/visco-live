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

int previewFolder = 0;	// Initial folder selection for navigation
int previewSample = 0;	// Initial sample selection for navigation
int selectedFolder = 0; // Remember selected folder
int selectedSample = 0; // Remember selected sample

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C display_1(U8G2_R1, U8X8_PIN_NONE);
U8G2_SH1107_PIMORONI_128X128_1_HW_I2C display_2(U8G2_R3, U8X8_PIN_NONE);
// Select I2C BUS
void TCA9548A(uint8_t bus)
{
	Wire.beginTransmission(0x70); // TCA9548A address
	Wire.write(1 << bus);		  // send byte to select bus
	Wire.endTransmission();
	Serial.print(bus);
}

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C *getScreenFromID(int id)
{
	switch (id)
	{
	case 1:
		return &display_1;
	case 2:
		return &display_2;
	default:
		return &display_1;
	}
}

void drawUI(int screenId, ScreenState screenState)
{
	switch (screenState)
	{
	case SCREEN_RESTING:
		// Draw the vertical line and the volume progress bar
		int lineX = 5;
		int lineYTop = 5;
		int lineYBottom = 123;
		int barWidth = 3;
		int barX = lineX - 1;

		getScreenFromID(screenId)->drawLine(lineX, lineYTop, lineX, lineYBottom);

		int totalHeight = lineYBottom - lineYTop;
		int filledHeight = map(VolumeIn, 0, 119, 0, totalHeight);
		filledHeight = constrain(filledHeight, 0, 119); // Ensure the height does not exceed 118px

		getScreenFromID(screenId)->setDrawColor(1);
		getScreenFromID(screenId)->drawBox(barX, lineYBottom - filledHeight, barWidth, filledHeight);

		// Draw the blinking circle
		if (circleVisible)
		{
			getScreenFromID(screenId)->drawDisc(20, 15, 2);
		}

		// Draw a small bar in the top right corner
		getScreenFromID(screenId)->drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top

		getScreenFromID(screenId)->setFont(unibody_8);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Sample Library"), 18, "Sample Library");

		// Display folder name in full
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth(sampleFolder[selectedFolder]), 128 - 40, sampleFolder[selectedFolder]);

		// Truncate sample name if it is longer than 8 characters
		char displayedSample[12];
		snprintf(displayedSample, sizeof(displayedSample), "%.6s...", sampleArrays[selectedFolder][selectedSample]);
		getScreenFromID(screenId)->setFont(unibody_16);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth(displayedSample), 128 - 20, displayedSample);
		break;
	case SCREEN_SAMPLE_LIBRARY_FOLDER:
		// Draw
		getScreenFromID(screenId)->setFont(unibody_8);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

		int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
		int verticalSpacing = 5;
		int baseY = 128 - 6 * (lineHeight + verticalSpacing);

		for (int i = -1; i <= 4; ++i)
		{
			int index = previewFolder + i;
			if (index >= 0 && index < (sizeof(sampleFolder) / sizeof(sampleFolder[0])))
			{
				int y = baseY + (i + 1) * (lineHeight + verticalSpacing);
				getScreenFromID(screenId)->drawStr(5, y, sampleFolder[index]);
				if (i == 0)
				{
					getScreenFromID(screenId)->drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
				}
			}
		}

		// Draw a small bar in the top right corner
		getScreenFromID(screenId)->drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
		break;
	case SCREEN_SAMPLE_LIBRARY_SAMPLES:
		// Draw
		getScreenFromID(screenId)->setFont(unibody_8);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

		int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
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
				getScreenFromID(screenId)->drawStr(5, y, currentSamples[index]);
				if (i == 0)
				{
					getScreenFromID(screenId)->drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
				}
			}
		}

		// Draw a small bar in the top right corner
		getScreenFromID(screenId)->drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
		break;
	case SCREEN_PRESET_LIBRARY_PRESETS:
		// Draw
		getScreenFromID(screenId)->setFont(unibody_8);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

		int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
		int verticalSpacing = 5;
		int baseY = 128 - 6 * (lineHeight + verticalSpacing);

		for (int i = -1; i <= 4; ++i)
		{
			int index = previewFolder + i;
			if (index >= 0 && index < (sizeof(sampleFolder) / sizeof(sampleFolder[0])))
			{
				int y = baseY + (i + 1) * (lineHeight + verticalSpacing);
				getScreenFromID(screenId)->drawStr(5, y, sampleFolder[index]);
				if (i == 0)
				{
					getScreenFromID(screenId)->drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
				}
			}
		}

		// Draw a small bar in the top right corner
		getScreenFromID(screenId)->drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
		break;
	case SCREEN_PRESET_LIBRARY_SAMPLES:
		// Draw
		getScreenFromID(screenId)->setFont(unibody_8);
		getScreenFromID(screenId)->drawStr(128 - 5 - getScreenFromID(screenId)->getStrWidth("Back"), 18, "Back");

		int lineHeight = getScreenFromID(screenId)->getAscent() - getScreenFromID(screenId)->getDescent();
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
				getScreenFromID(screenId)->drawStr(5, y, currentSamples[index]);
				if (i == 0)
				{
					getScreenFromID(screenId)->drawFrame(0, y - lineHeight - 2, 128, lineHeight + 6);
				}
			}
		}

		// Draw a small bar in the top right corner
		getScreenFromID(screenId)->drawBox(115, 0, 2, 5); // 10px margin from the right, no margin from the top
		break;
	}
}

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
	}
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
	}
}

void screenSetup()
{
	// Init OLED display on bus number 0
	TCA9548A(0);
	if (!display_1.begin())
	{
		Serial.println(F("OLED allocation failed"));
		for (;;)
			;
	}
	Serial.println("Display 1 initialized");

	// Init OLED display on bus number 1
	TCA9548A(1);
	if (!display_2.begin())
	{
		Serial.println(F("OLED allocation failed"));
		for (;;)
			;
	}
	Serial.println("Display 2 initialized");

	ESP32Encoder::useInternalWeakPullResistors = puType::up;

	encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
	encoder.clearCount();

	pinMode(ENCODER_SW, INPUT_PULLUP);
	pinMode(SAMPLE_A, INPUT_PULLUP);
	pinMode(SAMPLE_B, INPUT_PULLUP);

	// Initialize encoder with the current volume
	encoder.setCount(VolumeIn);
}

void screenLoop()
{
	// // Handle button press to toggle between screens
	// if (debounceButton(9, mcp->digitalRead(SAMPLE_A)))
	// {
	// 	// Safety check to prevent switching screens while the other screen is active
	// 	if (screenStateRight != = SCREEN_RESTING)
	// 	{
	// 		return;
	// 	}

	// 	if (screenStateLeft == SCREEN_RESTING)
	// 	{
	// 		screenStateLeft = SCREEN_SAMPLE_LIBRARY_FOLDER;
	// 		encoder.clearCount(); // Clear encoder count when switching screens
	// 	}
	// 	else if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_FOLDER)
	// 	{
	// 		screenStateLeft = SCREEN_RESTING;
	// 		encoder.setCount(VolumeIn); // Ensure encoder reflects the current volume
	// 	}
	// 	else if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_SAMPLES)
	// 	{
	// 		screenStateLeft = SCREEN_SAMPLE_LIBRARY_FOLDER;
	// 		encoder.clearCount(); // Clear encoder count when switching screens
	// 	}
	// }

	// // Handle encoder button press for sample selection and switch back to volume screen
	// if (digitalRead(ENCODER_SW) == LOW)
	// {
	// 	if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_FOLDER)
	// 	{
	// 		// Move to sample list screen
	// 		screenStateLeft = SCREEN_SAMPLE_LIBRARY_SAMPLES;
	// 		encoder.clearCount();
	// 	}
	// 	else if (screenStateRight == SCREEN_SAMPLE_LIBRARY_FOLDER)
	// 	{
	// 		// Move to sample list screen
	// 		screenStateRight = SCREEN_SAMPLE_LIBRARY_SAMPLES;
	// 		encoder.clearCount();
	// 	}
	// 	else if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_SAMPLES)
	// 	{
	// 		// Confirm selection and switch back to volume screen
	// 		screenStateLeft = SCREEN_RESTING;
	// 		selectedFolder = previewFolder; // Store selected folder
	// 		selectedSample = previewSample; // Store selected sample
	// 		encoder.setCount(VolumeIn);		// Ensure encoder reflects the current volume
	// 		delay(300);						// Debounce delay
	// 	}
	// 	else if (screenStateRight == SCREEN_SAMPLE_LIBRARY_SAMPLES)
	// 	{
	// 		// Confirm selection and switch back to volume screen
	// 		screenStateRight = SCREEN_RESTING;
	// 		selectedFolder = previewFolder; // Store selected folder
	// 		selectedSample = previewSample; // Store selected sample
	// 		encoder.setCount(VolumeIn);		// Ensure encoder reflects the current volume
	// 		delay(300);						// Debounce delay
	// 	}
	// }

	// ___SCREENS___
	if (screenStateLeft == SCREEN_RESTING && screenStateRight == SCREEN_RESTING)
	{
		// Volume screen logic
		volumeChange();
	}
	else if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_FOLDER || screenStateRight == SCREEN_SAMPLE_LIBRARY_FOLDER)
	{
		// Folder selection screen logic
		folderSelection();
	}
	else if (screenStateLeft == SCREEN_SAMPLE_LIBRARY_SAMPLES || screenStateLeft == SCREEN_SAMPLE_LIBRARY_SAMPLES)
	{
		// Sample selection screen logic
		sampleSelection();
	}

	// Handle blinking circle
	blinkingCircle();

	// Render the display based on screen state and update condition
	getScreenFromID(0)->firstPage();
	getScreenFromID(1)->firstPage();
	do
	{
		drawUI(0, screenStateLeft);
		drawUI(1, screenStateRight);
	} while (getScreenFromID(0)->nextPage());
}
