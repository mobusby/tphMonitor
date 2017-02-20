// main.cpp
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// tphMonitor -- monitor Temperature, Pressure, Relative Humidity
//     - Built for Adafruit Feather M0 Adalogger (Atmel SAMD chip)
//     - Utilizes a Real Time Clock (RTC), temperature sensor, pressure sensor,
//       and humidity sensor
//     - Data is displayed on an e-paper display
//     - Data is logged to an SD Card

// Comment out this line to remove all the serial prints and other debug-only
// stuff

#include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <SdFat.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BMP280.h>
// #include <SI7021.h>
// #include <RTClib.h>

// everything in DEBUG.h is only included if DEBUG is defined
// #define DEBUG
#include <DEBUG.h>

#include "Sensors.hpp"
#include "LogFile.hpp"
#include "DataPoint.hpp"
#include "Papirus.hpp"
// #include "Gauge.hpp"

// For global constants, save RAM/cache by setting them at compile time
#ifdef DEBUG
// For debugging, shorten up the LOGINTERVAL
#define LOGINTERVAL 10 // seconds
#else
#define LOGINTERVAL 600 // 10 minutes
#endif // DEBUG

// Global variables
DateTime *nextPoint;
LogFile *logFile;
Sensors *sensors;
Papirus *papirus;

// Function Prototypes
void recordDataPoint(const DataPoint &, LogFile *);
void displayDataPoint(const DataPoint &);
void printDateTimeToFile(const DateTime &, ofstream &);

void setup() {
    // initialize the pins
    pinMode(LED_BUILTIN, OUTPUT); // Red LED next to USB (pin 13)

    SETUPSERIALDEBUG();
    DEBUGPRINTLN("setup()");

    DEBUGPRINT("LED_BUILTIN: ");
    DEBUGPRINTLN(LED_BUILTIN);

    // initialize all the sensors
    sensors = new Sensors();

    // initialize the Papirus display
    papirus = new Papirus(sensors->getTemperature_C());

    // initialize the log file -- do this after initilizing the display, but before writing to the display to avoid weird bugs
    logFile = LogFile::initSdLogFile(sensors, NULL, true);

    papirus->addBorder();

    // record the first data point without delay
    DataPoint dp(sensors);
    recordDataPoint(dp, logFile);
    displayDataPoint(dp);

    // wait until just before the top of the minute to continue logging
    // (allow a second to move into the loop() function)
    // blink the LED to indicate waiting
    while ((sensors->getDateTime().secondstime() + 1) % LOGINTERVAL) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
    }

    nextPoint = new DateTime(sensors->getDateTime() + (TimeSpan) 1);

    DEBUGPRINT("nextPoint = ");
    DEBUGPRINTLN(nextPoint->secondstime());
}

void loop() {
    DEBUGPRINTLN("loop()");
    // light the Red LED to indicate taking a measurement
    digitalWrite(LED_BUILTIN, HIGH);

    while (sensors->getDateTime().secondstime() < nextPoint->secondstime())
        delay(100);

    DataPoint dp(sensors);
    recordDataPoint(dp, logFile);
    displayDataPoint(dp);

    // Measurement done, LED off
    digitalWrite(LED_BUILTIN, LOW);

    DateTime *oldPoint = nextPoint;
    nextPoint = new DateTime(*oldPoint + (TimeSpan) LOGINTERVAL);
    delete oldPoint;
    delay(LOGINTERVAL * 1000 - 500); // loop a half-second early
}

void recordDataPoint(const DataPoint &dataPoint, LogFile *logfile) {
    DEBUGPRINTLN("recordDataPoint()");

    // print data to Serial for debugging
    DEBUGPRINT(dataPoint.dateTime.secondstime());
    DEBUGPRINT("  ");
    DEBUGPRINT(dataPoint.batteryVoltage);
    DEBUGPRINT("V    (");
    DEBUGPRINT(dataPoint.bmp280TemperatureC);
    DEBUGPRINT(" ± 1)°C    (");
    DEBUGPRINT(dataPoint.si7021TemperatureC);
    DEBUGPRINT(" ± 0.4)°C    (");
    DEBUGPRINT(dataPoint.bmp280Pressure);
    DEBUGPRINT(" ± 0.12)hPa    (");
    DEBUGPRINT(dataPoint.bmp280PressureAltitudeM);
    DEBUGPRINT(" ± 1)m   (");
    DEBUGPRINT(dataPoint.si7021Humidity);
    DEBUGPRINTLN(" ± 3)% Rel Hum");

    // write data to SD Card, light up the LED during write
    logfile->resetSPI();
    if (logfile->stream.good()) {
        DEBUGPRINTLN("recordDataPoint() -- good stream");
        printDateTimeToFile(dataPoint.dateTime, logfile->stream);
        logfile->stream << " | ";
        logfile->stream << dataPoint.batteryVoltage;
        logfile->stream << " | ";
        logfile->stream << dataPoint.bmp280TemperatureC;
        logfile->stream << " | ";
        logfile->stream << dataPoint.si7021TemperatureC;
        logfile->stream << " | ";
        logfile->stream << dataPoint.bmp280Pressure;
        logfile->stream << " | ";
        logfile->stream << dataPoint.bmp280PressureAltitudeM;
        logfile->stream << " | ";
        logfile->stream << dataPoint.si7021Humidity;
        logfile->stream << endl;

        digitalWrite(SDLED, HIGH);
        logfile->stream << flush; // force writing
        digitalWrite(SDLED, LOW);
    }
    DEBUGPRINTLN("DONE!");
}

void printDateTimeToFile(const DateTime &dt, ofstream &stream) {
    DEBUGPRINTLN("printDateTimeToFile()");

    stream << dt.year() << ".";
    DEBUGPRINTLN("HERE!");

    if (dt.month() < 10) stream << "0";
    stream << (int) dt.month() << ".";

    if (dt.day() < 10) stream << "0";
    stream << (int) dt.day() << " ";

    if (dt.hour() < 10) stream << "0";
    stream << (int) dt.hour() << ":";

    if (dt.minute() < 10) stream << "0";
    stream << (int) dt.minute() << ":";

    if (dt.second() < 10) stream << "0";
    stream << (int) dt.second();
}

void displayDataPoint(const DataPoint &dp) {
    DEBUGPRINTLN("displayDataPoint()");

    static int tempX = 60, presX = 126, humX = 192;
    static float tempMin = 999999, presMin = 999999, humMin = 999999;
    static float tempMax = -999999, presMax = -999999, humMax = -999999;

    if (dp.si7021TemperatureF > tempMax) tempMax = dp.si7021TemperatureF;
    if (dp.bmp280Pressure > presMax) presMax = dp.bmp280Pressure;
    if (dp.si7021Humidity > humMax) humMax = dp.si7021Humidity;

    if (dp.si7021TemperatureF < tempMin) tempMin = dp.si7021TemperatureF;
    if (dp.bmp280Pressure < presMin) presMin = dp.bmp280Pressure;
    if (dp.si7021Humidity < humMin) humMin = dp.si7021Humidity;

    char headerStr[sizeof("YYYY.MM.DD, HH:MM+SS Z || +X.XXV")];
    snprintf(headerStr, sizeof(headerStr), "%04u.%02u.%02u, %02u:%02u+%02u Z || +%1u.%02uV",
        (unsigned int) dp.dateTime.year(),
        (unsigned int) dp.dateTime.month(),
        (unsigned int) dp.dateTime.day(),
        (unsigned int) dp.dateTime.hour(),
        (unsigned int) dp.dateTime.minute(),
        (unsigned int) dp.dateTime.second(),
        (unsigned int) dp.batteryVoltage,
        (unsigned int) ((dp.batteryVoltage * 1000. - ((int) dp.batteryVoltage) * 1000) / 10.));

    papirus->addText(5, 3, headerStr, 1);

    // papirus->addText(5, 5 + 14 * 3, "Temp [F]", 1);
    papirus->addText(1 + 2, 14, " Temp [F]", 1);
    // papirus->addText(66 + 4, 5 + 14 * 3, "Pres [hPa]", 1);
    papirus->addText(66 + 2, 14, "Pres [hPa]", 1);
    // papirus->addText(132 + 4, 5 + 14 * 3, "Rel Hum [%]", 1);
    papirus->addText(132 + 2, 14, "  Hum [%]", 1);

    papirus->addVertScale(tempX, -10., 150., 10., 5., dp.si7021TemperatureF, tempMin, tempMax);
    papirus->addVertScale(presX, 500., 1200., 200., 50., dp.bmp280Pressure, presMin, presMax);
    papirus->addVertScale(humX, 0., 100., 10., 5., dp.si7021Humidity, humMin, humMax);

    papirus->fullUpdate(dp.si7021TemperatureC);
}
