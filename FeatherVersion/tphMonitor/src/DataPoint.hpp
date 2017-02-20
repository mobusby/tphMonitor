// DataPoint.hpp
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Part of tphMonitor
// class to hold Data for a single point

#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP

#include "Sensors.hpp"

class DataPoint {
public:
    DateTime dateTime;
    float batteryVoltage;
    float bmp280TemperatureC;
    float si7021TemperatureC;
    float si7021TemperatureF;
    float bmp280Pressure;
    float bmp280PressureAltitudeM;
    float bmp280PressureAltitudeFt;
    int si7021Humidity;
    DataPoint(Sensors *s) {
        dateTime = s->getDateTime();
        batteryVoltage = s->getBatteryVoltage();
        bmp280TemperatureC = s->getBMP280Temperature_C();
        si7021TemperatureC = s->getSi7021Temperature_C();
        si7021TemperatureF = s->getSi7021Temperature_F();
        bmp280Pressure = s->getBMP280Pressure_hPa();
        bmp280PressureAltitudeM = s->getBMP280PressureAltitude_m();
        bmp280PressureAltitudeFt = s->getPressureAltitude_ft();
        si7021Humidity = s->getSi7021Humidity_percent();
    }
};

#endif // DATAPOINT_HPP
