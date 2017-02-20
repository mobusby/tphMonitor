// Sensors.h
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Part of tphMonitor
// Abstract much of the tedium of working with the sensors

#ifndef SENSORS_HPP
#define SENSORS_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SI7021.h>
#include <RTClib.h>

#define VBATPIN A7

class Sensors {
public:
    Sensors(); // initialize all the sensors
    DateTime getDateTime(); // get the current Date/Time from the RTC
    float getTemperature_C(); // in Celcius
    float getTemperature_F(); // in Farenheit
    float getPressure_hPa();
    float getPressure_inHg();
    float getPressureAltitude_m();
    float getPressureAltitude_ft();
    float getAltitude_m(float seaLevelPressure);
    int getHumidity_percent();
    float getBatteryVoltage();

    float convert_hPa_inHg(float hPa);

    // query specific sensors
    float getSi7021Temperature_C(); // more accurate than BMP280
    float getSi7021Temperature_F();
    float getBMP280Temperature_C(); // less accurate than Si7021
    int getSi7021Humidity_percent(); //only source for humidity
    float getBMP280Pressure_hPa(); // only source for pressure
    float getBMP280PressureAltitude_m(); // only source for Altitude
    float getBMP280Altitude_m(float seaLevelPressure);

private:
    void initSensors();
    static bool initialized; // only need to initialize sensors once.
    static RTC_PCF8523 rtc; // keep track of the time
    static Adafruit_BMP280 bmp280; // temperature and pressure
    static SI7021 si7021; // temperature and humidity
    static float standardPressure; // international standard atmosphere sea level pressure, 1013.25 hPa, ~29.92 inHg
};

bool Sensors::initialized = false;
RTC_PCF8523 Sensors::rtc;// = new RTC_PCF8523();
Adafruit_BMP280 Sensors::bmp280;// = new Adafruit_BMP280();
SI7021 Sensors::si7021;// = new SI7021();
float Sensors::standardPressure = 1013.25;

// constructor also initializes all sensors
Sensors::Sensors() {
    if (! initialized) {
        initSensors();
        initialized = true;
    }
}

void Sensors::initSensors() {
    while (! rtc.begin()) ;

    if (! rtc.initialized()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    while (! si7021.begin()) ;

    while (! bmp280.begin()) ;
}

DateTime Sensors::getDateTime() {
    return rtc.now();
}

float Sensors::getTemperature_C() {
    return getSi7021Temperature_C();
}

float Sensors::getTemperature_F() {
    return getSi7021Temperature_F();
}

float Sensors::getPressure_hPa() {
    return getBMP280Pressure_hPa();
}

float Sensors::getPressure_inHg() {
    return convert_hPa_inHg(getPressure_hPa());
}

float Sensors::getPressureAltitude_m() {
    return getBMP280PressureAltitude_m();
}

float Sensors::getPressureAltitude_ft() {
    return getPressureAltitude_m() * 100. / (2.54 * 12);
}

float Sensors::getAltitude_m(float seaLevelPressure) {
    return getBMP280Altitude_m(seaLevelPressure);
}

int Sensors::getHumidity_percent() {
    return getSi7021Humidity_percent();
}

float Sensors::getBatteryVoltage() {
    // float measuredvbat = analogRead(VBATPIN);
    // measuredvbat *= 2.0;  // board divides by 2, so multiply back
    // measuredvbat *= 3.3;  // Multiply by 3.3V, the reference voltage
    // measuredvbat /= 1024.; // convert to voltage
    return (((double)(analogRead(VBATPIN) * 2)) * 3.3 / 1024.);
}

float Sensors::convert_hPa_inHg(float hPa) {
    return hPa / (3386.39 /* [hPa] / [inHg] */);
}

float Sensors::getSi7021Temperature_C() {
    return ((float) si7021.getCelsiusHundredths() / 100.);
}

float Sensors::getSi7021Temperature_F() {
    return ((float) si7021.getFahrenheitHundredths() / 100.);
}

float Sensors::getBMP280Temperature_C() {
    return bmp280.readTemperature();
}

int Sensors::getSi7021Humidity_percent() {
    return si7021.getHumidityPercent();
}

float Sensors::getBMP280Pressure_hPa() {
    return bmp280.readPressure() / 100.;
}

float Sensors::getBMP280PressureAltitude_m() {
    return bmp280.readAltitude(standardPressure);

}

float Sensors::getBMP280Altitude_m(float seaLevelPressure) {
    return bmp280.readAltitude(seaLevelPressure);
}

#endif // SENSORS_HPP
