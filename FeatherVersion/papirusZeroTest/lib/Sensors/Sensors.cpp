#include <Sensors.h>

bool Sensors::initialized = false;
RTC_PCF8523 *const Sensors::rtc = new RTC_PCF8523();
Adafruit_BMP280 *const Sensors::bmp280 = new Adafruit_BMP280();
SI7021 *const Sensors::si7021 = new SI7021();
float Sensors::standardPressure = 1013.25;

// constructor also initializes all sensors
Sensors::Sensors() {
    DEBUGPRINTLN("Sensors::Sensors()");

    if (! initialized) {
        initSensors();
        initialized = true;
    }
}

void Sensors::initSensors() {
    DEBUGPRINTLN("Sensors::initSensors()");
    while (! rtc->begin()) {
        DEBUGPRINTLN("RTC not found, trying again, forever, until found.");
    }

    if (! rtc->initialized()) {
        DEBUGPRINTLN("RTC not running, setting it up with build date/time.");

        rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));

        DEBUGPRINTLN("RTC must be setup with a real computer to have accurate date/time.");
    }

    while (! si7021->begin()) {
        DEBUGPRINTLN("Si7021 not found, trying again, forever, until found.");
    }

    while (! bmp280->begin()) {
        DEBUGPRINTLN("BMP280 not found, trying again, forever, until found.");
    }
}

DateTime Sensors::getDateTime() {
    return rtc->now();
}

float Sensors::getTemperature_C() {
    return ((float) si7021->getCelsiusHundredths() / 100.);
}

float Sensors::getTemperature_F() {
    return ((float) si7021->getFahrenheitHundredths() / 100.);
}

float Sensors::getPressure_hPa() {
    return bmp280->readPressure();
}

float Sensors::getPressure_inHg() {
    return convert_hPa_inHg(bmp280->readPressure());
}

float Sensors::getPressureAltitude_m() {
    return bmp280->readAltitude(standardPressure);
}

float Sensors::getAltitude_m(float seaLevelPressure) {
    return bmp280->readAltitude(seaLevelPressure);
}

int Sensors::getHumidity_percent() {
    return si7021->getHumidityPercent();
}

float Sensors::convert_hPa_inHg(float hPa) {
    return hPa / (3386.39 /* [hPa] / [inHg] */);
}

float Sensors::getBatteryVoltage() {
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2.0;  // board divides by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, the reference voltage
    measuredvbat /= 1024.; // convert to voltage
    return measuredvbat;
    // return (((double)(analogRead(VBATPIN) * 2)) * 3.3 / 1024.);
}
