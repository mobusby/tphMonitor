// EPD_DEFINES.h
//
// These are copies of the define statements used in the EPD examples.
//

#ifndef EPD_DEFINES_H
#define EPD_DEFINES_H

// pre-processor convert to string
#define MAKE_STRING1(X) #X
#define MAKE_STRING(X) MAKE_STRING1(X)

// other pre-processor magic
// token joining and computing the string for #include
#define ID(X) X
#define MAKE_NAME1(X,Y) ID(X##Y)
#define MAKE_NAME(X,Y) MAKE_NAME1(X,Y)
#define MAKE_JOIN(X,Y) MAKE_STRING(MAKE_NAME(X,Y))

// calculate the include name and variable names
// #define IMAGE_1 text_image
// #define IMAGE_2 cat
// #define IMAGE_3 aphrodite
// #define IMAGE_4 saturn
// #define IMAGE_5 venus
// #define IMAGE_1_FILE MAKE_JOIN(IMAGE_1,EPD_IMAGE_FILE_SUFFIX)
// #define IMAGE_1_BITS MAKE_NAME(IMAGE_1,EPD_IMAGE_NAME_SUFFIX)
// #define IMAGE_2_FILE MAKE_JOIN(IMAGE_2,EPD_IMAGE_FILE_SUFFIX)
// #define IMAGE_2_BITS MAKE_NAME(IMAGE_2,EPD_IMAGE_NAME_SUFFIX)
// #define IMAGE_3_FILE MAKE_JOIN(IMAGE_3,EPD_IMAGE_FILE_SUFFIX)
// #define IMAGE_3_BITS MAKE_NAME(IMAGE_3,EPD_IMAGE_NAME_SUFFIX)
// #define IMAGE_4_FILE MAKE_JOIN(IMAGE_4,EPD_IMAGE_FILE_SUFFIX)
// #define IMAGE_4_BITS MAKE_NAME(IMAGE_4,EPD_IMAGE_NAME_SUFFIX)
// #define IMAGE_5_FILE MAKE_JOIN(IMAGE_5,EPD_IMAGE_FILE_SUFFIX)
// #define IMAGE_5_BITS MAKE_NAME(IMAGE_5,EPD_IMAGE_NAME_SUFFIX)

// read images into flash memory
// PROGMEM const
// #define unsigned
// #define char uint8_t
// #include IMAGE_1_FILE
// #undef char
// #undef unsigned
//
// PROGMEM const
// #define unsigned
// #define char uint8_t
// #include IMAGE_2_FILE
// #undef char
// #undef unsigned
//
// PROGMEM const
// #define unsigned
// #define char uint8_t
// #include IMAGE_3_FILE
// #undef char
// #undef unsigned
//
// PROGMEM const
// #define unsigned
// #define char uint8_t
// #include IMAGE_4_FILE
// #undef char
// #undef unsigned
//
// PROGMEM const
// #define unsigned
// #define char uint8_t
// #include IMAGE_5_FILE
// #undef char
// #undef unsigned

#endif //EPD_DEFINES_H
