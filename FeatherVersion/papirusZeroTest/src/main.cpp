#include <Arduino.h>
#include <inttypes.h>
#include <ctype.h>

// required libraries for E-Paper Display
#define FEATHER 1
#define EPD_ENABLE_EXTRA_SRAM 1
#define SCREEN_SIZE 200
// #include <SPI.h>
#include <EPD_FLASH.h>
#include <EPD_V231_G2.h>
// #include <EPD_PANELS.h>
#include <EPD_PINOUT.h>
// #include <Adafruit_GFX.h>
#include <EPD_GFX.h>
// Add Images library to compiler path
#include <Images.h>  // this is just an empty file
// put all the pre-processor defines and such stuff in a single file
#include <EPD_DEFINES.h>

// #define DEBUG // comment out this line to remove debug prints, etc.
#include <Debug.h>

// Required libraries for sensor suite
// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BMP280.h>
// #include <SI7021.h>
// #include <RTClib.h>
#include <Sensors.h>


// define the E-Ink display
EPD_Class EPD(EPD_SIZE, Pin_PANEL_ON, Pin_BORDER, Pin_DISCHARGE, Pin_RESET,
    Pin_BUSY, Pin_EPD_CS);
EPD_GFX epd_gfx(EPD);

void setupEPD(int);
void displayTempGraphic(int, DateTime, float);
void displayTempGraphic(int);
void displayImage(int); // use in loop() to display image

// I/O setup
void setup() {
    SETUPSERIALDEBUG();
    DEBUGPRINTLN("setup()");

    // initialize the pins
    pinMode(LED_BUILTIN, OUTPUT); // Red LED next to USB (pin 13)

    Sensors sensors;

    setupEPD((int) sensors.getTemperature_C());
}

// main loop
void loop() {
    DEBUGPRINTLN("loop()");
    Sensors sensors;
    DateTime observationTime = sensors.getDateTime();
    int temperature = (int) sensors.getTemperature_C();

    #ifdef DEBUG
	switch (EPD.error()) {
	case EPD_OK:
		// Serial.println("EPD: ok");
		break;
	case EPD_UNSUPPORTED_COG:
		Serial.println("EPD: unsuppported COG");
		break;
	case EPD_PANEL_BROKEN:
		Serial.println("EPD: panel broken");
		break;
	case EPD_DC_FAILED:
		Serial.println("EPD: DC failed");
		break;
	}
    #endif

    // displayImage(temperature);

    displayTempGraphic(temperature, observationTime, sensors.getBatteryVoltage());

    DateTime nextObservation = observationTime + TimeSpan(60 * 1);
    DateTime nextObservationRounded = DateTime(nextObservation.year(), nextObservation.month(), nextObservation.day(), nextObservation.hour(), nextObservation.minute());
    while (sensors.getDateTime().secondstime() < nextObservationRounded.secondstime()) delay(1000);
}

void displayImage(int temperature) {
    static int state = 0;
    EPD.begin(); // power up the EPD panel
    EPD.setFactor(temperature); // adjust for current temperature
    int delay_counts = 50;
    switch(state) {
    default:
    case 0:         // clear the screen
        // EPD.clear();
        state = 1;
        delay_counts = 1;  // reduce delay so first image come up quickly
        break;
    case 1:         // clear -> text
        EPD.image_0(IMAGE_1_BITS);
        ++state;
        break;
    case 2:         // text -> picture
        EPD.image(IMAGE_1_BITS, IMAGE_2_BITS);
        ++state;
        break;
    case 3:         // picture -> picture
        EPD.image(IMAGE_2_BITS, IMAGE_3_BITS);
        ++state;
        break;
    case 4:         // picture -> picture
        EPD.image(IMAGE_3_BITS, IMAGE_4_BITS);
        ++state;
        break;
    case 5:         // picture -> picture
        EPD.image(IMAGE_4_BITS, IMAGE_5_BITS);
        ++state;
        break;
    case 6:        // picture -> text
        EPD.image(IMAGE_5_BITS, IMAGE_1_BITS);
        ++state ;  // back to picture next time
        break;
    case 7:
        displayTempGraphic(temperature);
        // state = 2;
        break;
    }
    EPD.end();   // power down the EPD panel

	// flash LED for 5 seconds
	for (int x = 0; x < delay_counts; ++x) {
		digitalWrite(Pin_RED_LED, HIGH);
		delay(50);
		digitalWrite(Pin_RED_LED, LOW);
		delay(50);
	}
}

void setupEPD(int temperature) {
	pinMode(Pin_RED_LED, OUTPUT);
	pinMode(Pin_BUSY, INPUT);
	pinMode(Pin_RESET, OUTPUT);
	pinMode(Pin_PANEL_ON, OUTPUT);
	pinMode(Pin_DISCHARGE, OUTPUT);
	pinMode(Pin_BORDER, OUTPUT);
	pinMode(Pin_EPD_CS, OUTPUT);
	pinMode(Pin_EPD_FLASH_CS, OUTPUT);

	digitalWrite(Pin_RED_LED, LOW);
	digitalWrite(Pin_RESET, LOW);
	digitalWrite(Pin_PANEL_ON, LOW);
	digitalWrite(Pin_DISCHARGE, LOW);
	digitalWrite(Pin_BORDER, LOW);
	digitalWrite(Pin_EPD_CS, LOW);
	digitalWrite(Pin_EPD_FLASH_CS, HIGH);

    EPD_FLASH.begin(Pin_EPD_FLASH_CS);
    // EPD.clear();

    epd_gfx.begin(temperature);
    epd_gfx.setRotation(2);

    #ifdef DEBUG
	Serial.println();
	Serial.println();
	Serial.println("Display size: " MAKE_STRING(EPD_SIZE));
	Serial.println("Film: V" MAKE_STRING(EPD_FILM_VERSION));
	Serial.println("COG: G" MAKE_STRING(EPD_CHIP_VERSION));
    Serial.print("Rotation: ");
    Serial.println(epd_gfx.getRotation());
	Serial.println();

	Serial.println("Image 1: " IMAGE_1_FILE);
	Serial.println("Image 2: " IMAGE_2_FILE);
    Serial.println("Image 3: " IMAGE_3_FILE);
    Serial.println("Image 4: " IMAGE_4_FILE);
    Serial.println("Image 5: " IMAGE_5_FILE);
	Serial.println();

	if (EPD_FLASH.available()) {
		Serial.println("EPD FLASH chip detected OK");
	} else {
		uint8_t maufacturer;
		uint16_t device;
		EPD_FLASH.info(&maufacturer, &device);
		Serial.print("unsupported EPD FLASH chip: MFG: 0x");
		Serial.print(maufacturer, HEX);
		Serial.print("  device: 0x");
		Serial.print(device, HEX);
		Serial.println();
	}
    #endif
}

void displayTempGraphic(int temperature, DateTime dateTime, float batteryV) {
    DEBUGPRINTLN("displayTempGraphic()");

    int h = epd_gfx.height();
    int w = epd_gfx.width();

    // epd_gfx.setCursor(1,1);

    // create a border
	epd_gfx.drawRect(0, 0, w - 0, h - 0, EPD_GFX::BLACK);
	epd_gfx.drawRect(2, 2, w - 4, h - 4, EPD_GFX::BLACK);

    // create a pointer
	// epd_gfx.fillTriangle(135,20, 186,40, 152,84, EPD_GFX::BLACK);
	// epd_gfx.fillTriangle(139,26, 180,44, 155,68, EPD_GFX::WHITE);

    // time of observation
    char timeStr[sizeof("HH:MM+SS Z")];
    snprintf(timeStr, sizeof(timeStr), "%02u:%02u+%02u Z",
        (unsigned int) dateTime.hour(),
        (unsigned int) dateTime.minute(),
        (unsigned int) dateTime.second());
    int xTime = 5;
    int yTime = 5;
    // short int xBound = 0, yBound = 0;
    // short unsigned int wBound = 0, hBound = 0;
    // epd_gfx.setFont(&FreeSans12pt7b);
    epd_gfx.setFont();
    epd_gfx.setTextColor(EPD_GFX::BLACK, EPD_GFX::WHITE);
    epd_gfx.setTextSize(1);
    epd_gfx.setTextWrap(false);
    // epd_gfx.getTextBounds("HH:MM+SS ZZ", xTime, yTime, &xBound, &yBound, &wBound, &hBound);
    // epd_gfx.fillRect(xBound, yBound, wBound, hBound, EPD_GFX::WHITE);
    epd_gfx.setCursor(xTime, yTime);
    epd_gfx.print(timeStr);
    // for (unsigned int i = 0; i < sizeof(timeStr) - 1; ++i, xTime +=7) {
    //     epd_gfx.drawChar(xTime, yTime, timeStr[i], EPD_GFX::BLACK, EPD_GFX::WHITE, 1);
    // }

    // print battery voltage
    char batV[sizeof("+X.XXV")];
    snprintf(batV, sizeof(batV), "+%1u.%02uV", (unsigned int) batteryV, (unsigned int)((batteryV * 1000. - ((int) batteryV) * 1000) / 10.));
    DEBUGPRINTLN(batV);
    DEBUGPRINTLN(batteryV);
    DEBUGPRINTLN(sizeof(batV));
    int xBat = 5;
    int yBat = 15;
    // epd_gfx.getTextBounds("+X.XXV", xBat, yBat, &xBound, &yBound, &wBound, &hBound);
    // epd_gfx.fillRect(xBound, yBound, wBound, hBound, EPD_GFX::WHITE);
    epd_gfx.setCursor(xBat, yBat);
    epd_gfx.print(batV);
    // for (unsigned int i = 0; i < sizeof(batV) - 1; ++i, xBat +=7) {
    //     epd_gfx.drawChar(xBat, yBat, batV[i], EPD_GFX::BLACK, EPD_GFX::WHITE, 1);
    // }

    // temperature observed
	char temp[sizeof("-999 C")];
	snprintf(temp, sizeof(temp), "%4d C", temperature);
    int xTemp = 5;
    int yTemp = 30;
    epd_gfx.setTextSize(3);
    // epd_gfx.getTextBounds("-999 C", xTemp, yTemp, &xBound, &yBound, &wBound, &hBound);
    // epd_gfx.fillRect(xBound, yBound, wBound, hBound, EPD_GFX::WHITE);
    epd_gfx.setCursor(xTemp, yTemp);
    epd_gfx.print(temp);
	// for (uint i = 0; i < sizeof(temp) - 1; ++i, xTemp += 14) {
	// 	epd_gfx.drawChar(xTemp, yTemp, temp[i], EPD_GFX::BLACK, EPD_GFX::WHITE, 2);
	// }
	// small circle for degrees symbol
    // x coord is cursor + char number * char width + position in char
    // y coord is cursor + circle diameter
	epd_gfx.drawCircle(xTemp + 5 * 14 + 8, yTemp + 4, 3, EPD_GFX::BLACK);
	epd_gfx.drawCircle(xTemp + 5 * 14 + 8, yTemp + 4, 4, EPD_GFX::BLACK);

    // Thermometer Display
    // clip temperature to min or max
    static const int TMAX = 50;
    static const int TMIN = (-10);
    int tempForBar = temperature;
	if (temperature < TMIN) {
		tempForBar = TMIN;
	} else if (temperature > TMAX) {
		tempForBar = TMAX;
	}

    // temperature bar
	int bar_w = tempForBar - TMIN;  // zero based
	int bar_h = 4;
	int bar_x0 = 24;
	int bar_y0 = 60;
	epd_gfx.fillRect(bar_x0, bar_y0, TMAX - TMIN, bar_h, EPD_GFX::WHITE);
	epd_gfx.fillRect(bar_x0, bar_y0, bar_w, bar_h, EPD_GFX::BLACK);

    // scale
    for (int t0 = TMIN; t0 < TMAX; t0 += 5) {
        int t = t0 - TMIN;
        int tick = 8;
        if (0 == t0) {
            tick = 12;
            epd_gfx.drawCircle(bar_x0 + t, bar_y0 + 16, 3, EPD_GFX::BLACK);
        } else if (0 == t0 % 10) {
            tick = 10;
        }
        epd_gfx.drawLine(bar_x0 + t, bar_y0 + tick, bar_x0 + t, bar_y0 + 6, EPD_GFX::BLACK);
        epd_gfx.drawLine(bar_x0 + t, bar_y0 + 6, bar_x0 + t + 5, bar_y0 + 6, EPD_GFX::BLACK);
        epd_gfx.drawLine(bar_x0 + t + 5, bar_y0 + 6, bar_x0 + t + 5, bar_y0 + 8, EPD_GFX::BLACK);
    }

    epd_gfx.display(temperature);
}

void displayTempGraphic(int temperature) {
    DateTime dt;

    displayTempGraphic(temperature, dt, 0.0);
}
