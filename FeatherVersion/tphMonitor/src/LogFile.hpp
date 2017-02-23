// LogFile.h
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Part of tphMonitor
// Abstract much of the tedium of working with the SD Card
// Requires Sensors.h (requires RTC)

#ifndef LOGFILE_HPP
#define LOGFILE_HPP

// Pin Selections for Adafruit M0 Adalogger
#define CARDSELECT 4
#define SDLED 8

#include <SPI.h>
#include <SdFat.h>
#include "Sensors.hpp"
#include <DEBUG.h>

class LogFile {
public:
    ofstream stream; // make a stream available
    const char *getFileName () const;

    // use a static function to create new LogFile objects
    static LogFile *initSdLogFile(Sensors *sensors, SdFat *sd, bool useLongFileName = true);
    static void sdDateTimeCallback(uint16_t *date, uint16_t *time);
    static void resetSPI();
    ~LogFile();

private:
    // constructor requires a working SdFat object, so only allow construction
    // via static function LogFile::initSdLogFile(...)
    LogFile(const DateTime &dt, SdFat *sd, bool useLongFileName = true);
    char fileName[23]; // allocate enough space for long filename
    static bool callbackSet; // only need to set SdFile::dateTimeCallback once
    // static SdFat sd;
    // static Sensors sensors;
    SdFat *sd;
    static Sensors *sensors;
    void longFileName(const DateTime &dt);
    void shortFileName(const DateTime &dt);
};

// Static members
// SdFat LogFile::sd;
Sensors *LogFile::sensors = NULL;
bool LogFile::callbackSet = false;

// Static function that creates the file
LogFile *LogFile::initSdLogFile(Sensors *sensors, SdFat *sd, bool useLongFileName) {
    DEBUGPRINTLN("LogFile::initSdLogFile()");
    LogFile::sensors = sensors;

    if (sd == NULL) {
        pinMode(SDLED, OUTPUT); // LED for SD Card (pin 8)
        pinMode(CARDSELECT, OUTPUT); // The cardSelect pin must be set for output
        pinMode(SS, OUTPUT);
        sd = new SdFat();

        #ifdef DEBUG
        sd->errorPrint();
        DEBUGPRINTLN("0x19 (init not called) is ok here, will be called later");
        #endif // DEBUG
    }

    if (! LogFile::callbackSet) {
        // Set the SdFat DateTime callback function so we get good timestamps
        SdFile::dateTimeCallback(LogFile::sdDateTimeCallback);
    }

    while (! sd->begin(CARDSELECT, SPI_HALF_SPEED)) {
        #ifdef DEBUG
        static int attempts = 1;
        DEBUGPRINT("Could not intialize SD Card after ");
        DEBUGPRINT(attempts++);
        DEBUGPRINTLN(" attempt(s), trying again, forever, until intitialized.");
        sd->errorPrint();
        DEBUGPRINTLN("    0x1 = timeout error for command CMD0 (initialize card in SPI mode)");
        DEBUGPRINTLN("    0x5 = card returned an error response for CMD18 (read multiple block)");
        #endif // DEBUG
    }

    // SdFat.h library can use long file names!  Use the short filename with the older "SD.h" library
    return (new LogFile(sensors->getDateTime(), sd, useLongFileName));
}

LogFile::LogFile(const DateTime &dt, SdFat *sd, bool useLongFileName) {
    this->sd = sd;

    if (useLongFileName) {
        longFileName(dt);
    }
    else {
        shortFileName(dt);
    }

    // create/open the file for writing
    DEBUGPRINT("Attempting to open log file with name: ");
    DEBUGPRINTLN(getFileName());

    bool printHeader = false; // don't print a header on the file, if it already exists
    if (! sd->exists(getFileName())) printHeader = true;
    else DEBUGPRINTLN("File exists, will not print header.");

    //this->logfile = sd.open(filename, O_WRITE | O_APPEND | O_CREAT);
    while (! (this->stream = ofstream(getFileName(), ios::out | ios::app))) {
        DEBUGPRINTLN("Could not create/open log file, trying again, forever, until opened.");
        DEBUGPRINTLN(getFileName());
    }

    if (printHeader) {
        this->stream << F("Date | Time | Battery Voltage [V] | BMP280 Temperature [°C, ±1] | Si7021 Temperature [°C, ±0.4] | BMP280 Pressure [hPa, ±0.12] | BMP280 Pressure Altitude [m, ±1] | Si7021 Relative Humidity [%, ±3]")
            << endl << flush;
    }
}

LogFile::~LogFile() {

}

const char *LogFile::getFileName() const {
    return fileName;
}

void LogFile::shortFileName(const DateTime &dt) {
    // create the filename -- this is a hack to turn numbers into ASCII notation
    // "/YYMMDDHH.LOG" -- must use 8.3 file-names (SD library limitation)
    // fileName[0] = '/';
    // fileName[1] = '0' + ((dt.year()%1000)%100)/10;
    // fileName[2] = '0' + ((dt.year()%1000)%100)%10;
    // fileName[3] = '0' + dt.month()/10;
    // fileName[4] = '0' + dt.month()%10;
    // fileName[5] = '0' + dt.day()/10;
    // fileName[6] = '0' + dt.day()%10;
    // fileName[7] = '0' + dt.hour()/10;
    // fileName[8] = '0' + dt.hour()%10;
    // fileName[9] = '.';
    // fileName[10] = 'L';
    // fileName[11] = 'O';
    // fileName[12] = 'G';
    // fileName[13] = '\0';

    snprintf(fileName, sizeof("/YYMMDDHH.LOG"), "/%02u%02u%02u%02u.LOG",
        (dt.year() % 1000) % 100, dt.month(), dt.day(), dt.hour());
}

void LogFile::longFileName(const DateTime &dt) {
    // fileName = "/YYYY.MM.DD-HHMM_SS.log";
    // fileName[0] = '/';
    // fileName[1] = '0' + dt.year()/1000;
    // fileName[2] = '0' + (dt.year()%1000)/100;
    // fileName[3] = '0' + ((dt.year()%1000)%100)/10;
    // fileName[4] = '0' + ((dt.year()%1000)%100)%10;
    // fileName[5] = '.';
    // fileName[6] = '0' + dt.month()/10;
    // fileName[7] = '0' + dt.month()%10;
    // fileName[8] = '.';
    // fileName[9] = '0' + dt.day()/10;
    // fileName[10] = '0' + dt.day()%10;
    // fileName[11] = '-';
    // fileName[12] = '0' + dt.hour()/10;
    // fileName[13] = '0' + dt.hour()%10;
    // fileName[14] = '0' + dt.minute()/10;
    // fileName[15] = '0' + dt.minute()%10;
    // fileName[16] = '_';
    // fileName[17] = '0' + dt.second()/10;
    // fileName[18] = '0' + dt.second()%10;
    // fileName[19] = '.';
    // fileName[20] = 'l';
    // fileName[21] = 'o';
    // fileName[22] = 'g';
    // fileName[23] = '\0';

    snprintf(fileName, sizeof("/YYYY.MM.DD-HHMM_SS.log"),
        "/%04u.%02u.%02u-%02u%02u_%02u.log",
        dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
}

void LogFile::sdDateTimeCallback(uint16_t *dateptr, uint16_t *timeptr) {
    DateTime now = sensors->getDateTime();
    *dateptr = FAT_DATE(now.year(), now.month(), now.day());
    *timeptr = FAT_TIME(now.hour(), now.minute(), now.second());
}

void LogFile::resetSPI() {
    SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
}

#endif // LOGFILE_HPP
