// Papirus.h
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Part of tphMonitor
// Abstract much of the tedium of working with the Paparius Zero e-paper display
// with the Adafruit M0 Adalogger

#ifndef PAPIRUS_HPP
#define PAPIRUS_HPP

#define FEATHER 1
#define EPD_ENABLE_EXTRA_SRAM 1
#define SCREEN_SIZE 200

#include <Arduino.h>
#include <inttypes.h>
#include <ctype.h>
#include <EPD_FLASH.h>
#include <EPD_V231_G2.h>
#include <EPD_PANELS.h>
#include <EPD_PINOUT.h> // requires modified version for Adafruit Feather M0 Adalogger
// #include <Adafruit_GFX.h> // must include this in main.cpp before Papirus.h
#include <EPD_GFX.h>
// #include <Fonts/FreeSans9pt7b.h>
// #include <Fonts/FreeSans12pt7b.h>

// Add Images library to compiler path
#include <Images.h>  // this is just an empty file

#include <EPD_DEFINES.h>

#include <DEBUG.h>

class Papirus {
public:
    Papirus(int temperature);
    void addBorder(int x = 0, int y = 0, int width = 0, int height = 0, int numPixels = 1);
    void addText(const int x, int y, const char *text, int fontSize = Papirus::SMALL);
    void rmText(int x, int y, char *text, int fontSize = Papirus::SMALL);
    void addVertScale(int x, float min, float max, float major, float minor,
        float value, float lowVal, float highVal);
    void partialUpdate();
    void fullUpdate(int temperature);
    void clear(int temperature);

    enum { SMALL, MEDIUM, LARGE };

private:
    static EPD_Class EPD;
    static EPD_GFX epd_gfx;
    // static const GFXfont *smallFont;
    // static const GFXfont *bigFont;

    void setupEPD(int temperature);
    char *valToString(float val);
};

EPD_Class Papirus::EPD(EPD_SIZE, Pin_PANEL_ON, Pin_BORDER, Pin_DISCHARGE, Pin_RESET,
    Pin_BUSY, Pin_EPD_CS);
EPD_GFX Papirus::epd_gfx(Papirus::EPD);

Papirus::Papirus(int temperature) {
    setupEPD(temperature);
}

void Papirus::setupEPD(int temperature) {
    DEBUGPRINTLN("Papirus::setupEPD(int temperature)");
    DEBUGPRINTLN(temperature);

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

    // clear(temperature);

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
    #endif // DEBUG
}

void Papirus::addBorder(int x, int y, int w, int h, int bw) {
    DEBUGPRINTLN("Papirus::addBorder(int x, int y, int w, int h, int bw)");
    DEBUGPRINTLN(x);
    DEBUGPRINTLN(y);
    DEBUGPRINTLN(w);
    DEBUGPRINTLN(h);
    DEBUGPRINTLN(bw);

    if (! w) w = epd_gfx.width();
    if (! h) h = epd_gfx.height();

    for (int i = 0; i < bw; i++) {
    	epd_gfx.drawRect(x + i, y + i, w - i, h - i, EPD_GFX::BLACK);
    }
}

void Papirus::addText(int x, int y, const char *text, int fontSize) {
    // drawing character by character won't work well with non fixed-width fotns
    // for (unsigned int i = 0; i < sizeof(text) - 1; i++, x += 3 * fontSize) {
    //     epd_gfx.drawChar(x, y, text[i], EPD_GFX::BLACK, EPD_GFX::WHITE, fontSize);
    // }
    DEBUGPRINTLN("Papirus::addText(int x, int y, const char *text, int fontSize)");
    DEBUGPRINTLN(x);
    DEBUGPRINTLN(y);
    DEBUGPRINTLN(text);
    DEBUGPRINTLN(fontSize);

    epd_gfx.setFont();
    epd_gfx.setCursor(x, y);
    epd_gfx.setTextColor(EPD_GFX::BLACK, EPD_GFX::WHITE);
    epd_gfx.setTextSize(fontSize);
    epd_gfx.setTextWrap(false);
    epd_gfx.print(text);
}

void Papirus::rmText(int x, int y, char *text, int fontSize) {
    DEBUGPRINTLN("Papirus::rmText(int x, int y, char *text, int fontSize)");
    DEBUGPRINTLN(x);
    DEBUGPRINTLN(y);
    DEBUGPRINTLN(text);
    DEBUGPRINTLN(fontSize);

    int16_t x1 = 0, y1 = 0;
    uint16_t width = 0, height = 0;

    epd_gfx.setTextSize(fontSize);
    epd_gfx.setCursor(x, y);
    epd_gfx.getTextBounds(text, x, y, &x1, &y1, &width, &height);
    epd_gfx.fillRect(x1, y1, width, height, EPD_GFX::BLACK);
    epd_gfx.fillRect(x1, y1, width, height, EPD_GFX::WHITE);
}

void Papirus::addVertScale(const int x, float min, float max, float major, float minor,
    float value, float lowVal, float highVal) {
    DEBUGPRINTLN("Papirus::addVertScale(int x, float min, float max, float major, float minor, float value, float lowVale, float highVal)");
    DEBUGPRINTLN(x);
    DEBUGPRINTLN(min);
    DEBUGPRINTLN(max);
    DEBUGPRINTLN(major);
    DEBUGPRINTLN(minor);
    DEBUGPRINTLN(value);
    DEBUGPRINTLN(lowVal);
    DEBUGPRINTLN(highVal);

    if (min > max) {
        DEBUGPRINTLN("min > max");
        DEBUGPRINT("min = ");
        DEBUGPRINTLN(min);
        DEBUGPRINT("max = ");
        DEBUGPRINTLN(max);
        return;
    }

    const int bottom = EPD_PIXEL_HEIGHT - 5; // 5 px from bottom
    const int top = 25; // Two lines of text at top
    const int heightPx = bottom - top;
    const int barWidth = 4;
    const float pxScale = heightPx / (max - minor);
    const int majorPx = major * pxScale;
    const int minorPx = minor * pxScale;

    if (majorPx % minorPx > 1) {
        DEBUGPRINTLN("majorPx is not evenly divisible by minorPx");
        return;
    }

    if (value > max) value = max;
    else if (value < min) value = min;

    int barHeight = (value - min) * pxScale;
    DEBUGPRINT("barHeight = ");
    DEBUGPRINTLN(barHeight);

    epd_gfx.fillRect(x + 1, bottom - heightPx, barWidth, heightPx, EPD_GFX::WHITE); // empty the thermometer
    epd_gfx.fillRect(x + 1, bottom - barHeight, barWidth, barHeight, EPD_GFX::BLACK); // fill the thermometer
    epd_gfx.drawLine(x, bottom, x, bottom - heightPx, EPD_GFX::BLACK);

    for (int tick = 0; tick <= heightPx; tick += 1) {
        if (tick % minorPx == 0) {
            if (tick % majorPx == 0)
                epd_gfx.drawLine(x - 4, bottom - tick, x, bottom - tick, EPD_GFX::BLACK);
            else
                epd_gfx.drawLine(x - 2, bottom - tick, x, bottom - tick, EPD_GFX::BLACK);
        }
    }

    char *lowStr = valToString(lowVal);
    char *currentStr = valToString(value);
    char *highStr = valToString(highVal);

    if (lowVal < value) addText(x - 50, bottom - 7, lowStr, 1);
    addText(x - 50, (top + bottom) / 2, currentStr, 1);
    if (highVal > value) addText(x - 50, top + 7, highStr, 1);

    delete[] lowStr;
    delete[] currentStr;
    delete[] highStr;
}

char *Papirus::valToString(float val) {
    DEBUGPRINTLN("Papirus::valToString(float val)");
    DEBUGPRINTLN(val);

    int strSize = sizeof("-XXX.XXX");
    char *valStr = new char[strSize];
    int valWhole = (int) val;
    unsigned int valFrac = (int)(val * 1000.) - valWhole * 1000;
    snprintf(valStr, strSize, "%3d.%03u", valWhole, valFrac);
    return valStr;
}

void Papirus::partialUpdate() {
    DEBUGPRINTLN("Papirus::partialUpdate()");
}

void Papirus::fullUpdate(int temperature) {
    DEBUGPRINTLN("Papirus::fullUpdate()");

    epd_gfx.display(temperature);
}

void Papirus::clear(int temperature) {
    EPD.begin();
    EPD.setFactor(temperature);
    EPD.clear();
    EPD.end();
}

#endif // PAPIRUS_HPP
