#ifndef DEBUG_H
#define DEBUG_H 1

// These exist only for debugging
#ifdef DEBUG
// ArduinoOutStream serial(Serial);
#define SETUPSERIALDEBUG() setupSerialDebug()
#define DEBUGPRINT(TEXT) Serial.print(TEXT)
#define DEBUGPRINTLN(TEXT) Serial.println(TEXT)

void setupSerialDebug() {
    Serial.begin(115200);
    while (! Serial) ;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Include debug helper functions here -- they will only be compiled if DEBUG is
// defined in main.cpp (or wherever else, as needed)


#else
#define SETUPSERIALDEBUG()
#define DEBUGPRINT(TEXT)
#define DEBUGPRINTLN(TEXT)
#endif // DEBUG
#endif // DEBUG_H
