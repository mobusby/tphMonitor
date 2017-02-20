// Gauge.hpp
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Part of tphMonitor
// Create a vertical bar (thermometer style) with low, high, and current markings
// and display it in one of three sections of a 2" Papirus e-paper display

#ifndef GAUGE_HPP
#define GAUGE_HPP

#include "Papirus.hpp"

class Gauge {
public:
    enum POSITION { ONE, TWO, THREE };

    static Gauge *GaugeMaker(Papirus *papirus, POSITION position,
        const char *title, float min, float max, float major, float minor = 0);

    void setTitle(const char *title);
    void setCurrentValue(float value); // also updates low and high as appropriate
    void setLowValue(float value);
    void setHighValue(float value);
    ~Gauge();

private:
    Gauge(POSITION pos, const char *title,
        float min, float max, float major, float minor);

    static Papirus *papirus;
    static const Gauge *gaugeOne;
    static const Gauge *gaugeTwo;
    static const Gauge *gaugeThree;

    const POSITION position;
    int x, y, width, height; // coordinates and bounds
    char *title;
    bool titleSet;
    const float minValue;
    const float maxValue;
    float highValue;
    float currentValue;
    float lowValue;
    const float majorTick;
    const float minorTick;
};

Papirus *Gauge::papirus = NULL;
const Gauge* Gauge::gaugeOne = NULL;
const Gauge* Gauge::gaugeTwo = NULL;
const Gauge* Gauge::gaugeThree = NULL;

Gauge *Gauge::GaugeMaker(Papirus *papirus, Gauge::POSITION position,
    const char *title, float min, float max, float major, float minor) {
    Gauge *nGauge;

    // only one Gauge can exist in each position
    switch (position) {
    case Gauge::POSITION::ONE:
        if (Gauge::gaugeOne == NULL) gaugeOne = nGauge = new Gauge(position, title, min, max, major, minor);
        else return NULL;
        break;
    case Gauge::POSITION::TWO:
        if (Gauge::gaugeTwo == NULL) gaugeTwo = nGauge = new Gauge(position, title, min, max, major, minor);
        else return NULL;
        break;
    case Gauge::POSITION::THREE:
        if (Gauge::gaugeThree == NULL) gaugeThree = nGauge = new Gauge(position, title, min, max, major, minor);
        else return NULL;
        break;
    default:
        return NULL;
    }

    if (Gauge::papirus == NULL) Gauge::papirus = papirus;

    return nGauge;
}

Gauge::Gauge(POSITION pos, const char *title, float min, float max,
    float major, float minor) : position(pos), minValue(min), maxValue(max),
    majorTick(major), minorTick(minor)
{
    titleSet = false;
    setTitle(title);

    width = 66; // panel width minus left and right columns (for border)
    height = 94; // panel height minus top and bottom rows (for border)
    y = 2;

    // TODO: draw a border around the appropriate section
    switch (position) {
    case Gauge::POSITION::ONE:
        x = 1;
        break;
    case Gauge::POSITION::TWO:
        x = 1 + width;
        break;
    case Gauge::POSITION::THREE:
        x = 1 + width + width;
        break;
    }

    papirus->addBorder(x, y, width, height, 1);

    // TODO: Create the gauge, thermometer style, in the appropriate spot
}

void Gauge::setTitle(const char *newTitle) {
    titleSet = true;

    // create a copy of title -- needed for later title changes
    this->title = new char[sizeof(newTitle)];
    memcpy(this->title, title, sizeof(newTitle));

    // TODO: Print the title at the top
    // TODO: Print a line under the title (underline)
}

void Gauge::setCurrentValue(float value) {
    // TODO: print the value to the center-left of the gauge
    // TODO: draw a small filled triangle pointing to the appropriate place on the gauge
    // TODO: draw a black rectangle filling in the gauge to the appropriate height

    if (lowValue > value) setLowValue(value);
    if (highValue < value) setHighValue(value);
}

void Gauge::setLowValue(float value) {
    lowValue = value;

    if (currentValue - lowValue < 0.0001) {
        // TODO: draw a small unfilled triangle point to the appropriate place on the gauge
    }

    // TODO: print the value at the bottom left of the gauge
}

void Gauge::setHighValue(float value) {
    highValue = value;

    if (highValue - currentValue < 0.0001) {
        // TODO: draw a small unfilled triangle point to the appropriate place on the gauge
    }

    // TODO: print the value at the top left of the gauge
}

Gauge::~Gauge() {
    switch (position) {
    case Gauge::POSITION::ONE:
        gaugeOne = NULL;
        break;
    case Gauge::POSITION::TWO:
        gaugeTwo = NULL;
        break;
    case Gauge::POSITION::THREE:
        gaugeThree = NULL;
        break;
    }

    delete[] title;
}

#endif // GAUGE_HPP
