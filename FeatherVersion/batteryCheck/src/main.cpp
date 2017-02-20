#include <Arduino.h>
#include <spi.h>
#include <Adafruit_BMP280.h>

#define VBATPIN A7

void printBatVoltageToSerial();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    printBatVoltageToSerial();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}

void printBatVoltageToSerial() {
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Serial.print("VBat: " );
    Serial.println(measuredvbat);
}
