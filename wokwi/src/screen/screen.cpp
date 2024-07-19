#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
// #include <ESP32Encoder.h>

#include "font/unibody_8.h"
#include "font/unibody_16.h"

U8G2_SH1107_PIMORONI_128X128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define ENCODER_DT 2
#define ENCODER_CLK 4
#define ENCODER_SW 15
#define BUTTON_PIN 5

ESP32Encoder encoder;

int VolumeIn = 51; // Start at 40% (51 out of 128)
int screenHeight = 128;
int screenWidth = 128;

enum ScreenState
{
	SCREEN_VOLUME,
	SCREEN_SAMPLE_FOLDER,
	SCREEN_SAMPLE_LIST
};

ScreenState screenState = SCREEN_VOLUME;

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

int previewFolder = 0;	// Initial folder selection for navigation
int previewSample = 0;	// Initial sample selection for navigation
int selectedFolder = 0; // Remember selected folder
int selectedSample = 0; // Remember selected sample

bool updateScreen = true; // Flag to control screen update

int encoderStepSize = 2; // Adjust the encoder step size for sensitive navigation

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
}

void loop()
{
	// Handle button press to toggle between screens
	if (digitalRead(BUTTON_PIN) == LOW)
	{
		if (screenState == SCREEN_VOLUME)
		{
			screenState = SCREEN_SAMPLE_FOLDER;
		}
		else if (screenState == SCREEN_SAMPLE_FOLDER)
		{
			screenState = SCREEN_VOLUME;
			previewFolder = selectedFolder; // Reset preview to last selected
			previewSample = selectedSample; // Reset preview to last selected
		}
		else if (screenState == SCREEN_SAMPLE_LIST)
		{
			screenState = SCREEN_SAMPLE_FOLDER;
		}
		encoder.clearCount(); // Reset encoder count when switching screens
		updateScreen = true;  // Ensure screen update when switching back to volume screen
		delay(300);			  // Debounce delay
	}

	// Handle encoder button press for sample selection and switch back to volume screen
	if (digitalRead(ENCODER_SW) == LOW)
	{
		if (screenState == SCREEN_SAMPLE_FOLDER)
		{
			// Move to sample list screen
			screenState = SCREEN_SAMPLE_LIST;
			encoder.clearCount();
			updateScreen = true;
		}
		else if (screenState == SCREEN_SAMPLE_LIST)
		{
			// Confirm selection and switch back to volume screen
			screenState = SCREEN_VOLUME;
			selectedFolder = previewFolder; // Store selected folder
			selectedSample = previewSample; // Store selected sample
			updateScreen = true;			// Trigger screen update on return to volume screen
			delay(300);						// Debounce delay
		}
	}

	// Update based on screen state
	if (screenState == SCREEN_VOLUME)
	{
		// Volume screen logic
		VolumeIn = encoder.getCount();
	}
	else if (screenState == SCREEN_SAMPLE_FOLDER)
	{
		// Folder selection screen logic
		int encoderChange = encoder.getCount();
		if (encoderChange != 0)
		{
			int steps = encoderChange / encoderStepSize;
			previewFolder += steps;

			// Ensure previewFolder stays within valid range
			if (previewFolder < 0)
				previewFolder = 0;
			else if (previewFolder >= sizeof(sampleFolder) / sizeof(sampleFolder[0]))
				previewFolder = sizeof(sampleFolder) / sizeof(sampleFolder[0]) - 1;

			// Reset encoder count after processing movement
			encoder.clearCount();

			updateScreen = true; // Trigger screen update on folder selection change
		}
	}
	else if (screenState == SCREEN_SAMPLE_LIST)
	{
		// Sample selection screen logic
		int encoderChange = encoder.getCount();
		if (encoderChange != 0)
		{
			int steps = encoderChange / encoderStepSize;
			previewSample += steps;

			// Ensure previewSample stays within valid range
			int sampleCount = sampleCounts[previewFolder];
			if (previewSample < 0)
				previewSample = 0;
			else if (previewSample >= sampleCount)
				previewSample = sampleCount - 1;

			// Reset encoder count after processing movement
			encoder.clearCount();

			updateScreen = true; // Trigger screen update on sample selection change
		}
	}

	// Render the display based on screen state and update condition
	u8g2.firstPage();

	do
	{
		u8g2.drawBox(116, 0, 2, 5);

		if (screenState == SCREEN_VOLUME)
		{
			int numberOfRectangles = 16;
			int rectHeight = 4;
			int verticalSpacing = (screenHeight - (numberOfRectangles * rectHeight)) / (numberOfRectangles - 1);

			int activeRectangles = map(VolumeIn, -64, 64, 0, numberOfRectangles);

			for (int i = 0; i < numberOfRectangles; i++)
			{
				int rectY = screenHeight - ((i + 1) * (rectHeight + verticalSpacing) - verticalSpacing);
				int rectWidth = 0 + i * 1.3;
				if (i < activeRectangles)
				{
					u8g2.setDrawColor(1);
					u8g2.drawBox(0, rectY, rectWidth, rectHeight);
				}
				else
				{
					for (int j = 0; j < rectWidth; j += 3)
					{
						u8g2.drawLine(j, rectY, j, rectY + rectHeight);
					}
				}
			}

			u8g2.setFont(unibody_8);
			u8g2.drawStr(128 - 5 - u8g2.getStrWidth("Sample Library"), 18, "Sample Library");
			u8g2.drawStr(128 - 5 - u8g2.getStrWidth(sampleFolder[selectedFolder]), 128 - 40, sampleFolder[selectedFolder]);
			u8g2.setFont(unibody_16);
			u8g2.drawStr(128 - 5 - u8g2.getStrWidth(sampleArrays[selectedFolder][selectedSample]), 128 - 20, sampleArrays[selectedFolder][selectedSample]);
		}
		else if (screenState == SCREEN_SAMPLE_FOLDER)
		{
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
		}
		else if (screenState == SCREEN_SAMPLE_LIST)
		{
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
		}

	} while (u8g2.nextPage());

	if (updateScreen)
	{
		updateScreen = false; // Reset update flag
	}

	delay(100);
}
