#ifndef SENSORS_H
#define SENSORS_H 1

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SI7021.h>
#include <RTClib.h>

#include <Debug.h>

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
    float getAltitude_m(float seaLevelPressure);
    int getHumidity_percent();
    float convert_hPa_inHg(float hPa);
    float getBatteryVoltage();

private:
    void initSensors();
    static bool initialized; // only need to initialize sensors once.
    static RTC_PCF8523 * const rtc; // keep track of the time
    static Adafruit_BMP280 * const bmp280; // temperature and pressure
    static SI7021 * const si7021; // temperature and humidity
    static float standardPressure; // international standard atmosphere sea level pressure, 1013.25 hPa, ~29.92 inHg
};

#endif
