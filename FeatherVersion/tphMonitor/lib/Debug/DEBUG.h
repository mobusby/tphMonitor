// Debug.h
//
// (c) Mark Busby <mark@BusbyCreations.com>
//
// Debug macros

#ifndef DEBUG_H
#define DEBUG_H

// These exist only for debugging
#ifdef DEBUG
// ArduinoOutStream serial(Serial);
#define SETUPSERIALDEBUG() setupSerialDebug()
// #define SETUPSERIALDEBUG() { Serial.begin(115200); while (! Serial) ; }
#define DEBUGPRINT(TEXT) {Serial.print(TEXT);}
#define DEBUGPRINTLN(TEXT) {Serial.println(TEXT);}
#else
#define SETUPSERIALDEBUG()
#define DEBUGPRINT(TEXT)
#define DEBUGPRINTLN(TEXT)
#endif // DEBUG

#ifdef DEBUG

void setupSerialDebug() {
    Serial.begin(115200);
    while (! Serial) ;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Include debug helper functions here -- they will only be compiled if DEBUG is
// defined in main.cpp (or wherever else, as needed)


#endif // DEBUG
#endif // DEBUG_H
