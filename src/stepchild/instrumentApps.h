/* 
   ======================================
            INSTRUMENTS
   ======================================

    Basically, this file catches all undefined instrument application functions, texts, and icons
    and defines them to the default data.

*/

// 'xy', 12x12px
const unsigned char xy_bmp []  = {
	0x00, 0x00, 0x50, 0xe0, 0x20, 0x20, 0x50, 0xa0, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 
	0x50, 0xa0, 0x40, 0x40, 0x70, 0x80, 0x00, 0x00
};
// 'planets', 12x12px
const unsigned char planets_bmp []  = {
	0x00, 0x00, 0x31, 0x00, 0x48, 0x00, 0x68, 0x60, 0x33, 0xa0, 0x04, 0x40, 0x08, 0x60, 0x48, 0xa0, 
	0x09, 0x20, 0x16, 0x40, 0x1b, 0x80, 0x00, 0x00
};
// 'rain', 12x12px
const unsigned char rain_bmp []  = {
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x09, 0x00, 0x10, 0x80, 0x22, 0x40, 0x28, 0x40, 0x28, 0x40, 
	0x24, 0x40, 0x10, 0x80, 0x0f, 0x00, 0x00, 0x00
};
// 'knobs', 12x12px
const unsigned char knobs_bmp []  = {
	0x00, 0x00, 0x0f, 0x00, 0x10, 0x80, 0x23, 0x40, 0x30, 0xc0, 0x4f, 0x20, 0x50, 0xa0, 0x50, 0xa0, 
	0x50, 0xa0, 0x20, 0x40, 0x1f, 0x80, 0x00, 0x00
};
// 'keys', 12x12px
const unsigned char keys_bmp []  = {
	0x00, 0x00, 0x6f, 0x60, 0x69, 0x60, 0x69, 0x60, 0x69, 0x60, 0x69, 0x60, 0x6f, 0x60, 0x60, 0x60, 
	0x76, 0xe0, 0x76, 0xe0, 0x76, 0xe0, 0x00, 0x00
};
// 'drum', 12x12px
const unsigned char drum_bmp []  = {
	0x00, 0x00, 0x42, 0x20, 0x28, 0x40, 0x00, 0x00, 0x1f, 0x80, 0x60, 0x60, 0x60, 0x60, 0x5f, 0xa0, 
	0x40, 0x20, 0x20, 0x40, 0x1f, 0x80, 0x00, 0x00
};
// 'cv', 12x12px
const unsigned char cv_icon_bmp[] = {
	0x00, 0x00, 0x1c, 0xe0, 0x22, 0x20, 0x5f, 0xc0, 0x52, 0xa0, 0x5e, 0xa0, 0x22, 0xa0, 0x1e, 0xa0, 
	0x42, 0xa0, 0x41, 0x40, 0x70, 0x80, 0x00, 0x00
};
// 'rattle_bmp', 12x12px
const unsigned char rattle_bmp [] = {
	0x00, 0x00, 0x26, 0x40, 0x0d, 0x00, 0x52, 0xa0, 0x19, 0x80, 0x54, 0xa0, 0x0b, 0x00, 0x26, 0x40, 
	0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00
};
// 'live_loop', 12x12px
const unsigned char live_loop_bmp [] = {
	0x00, 0x00, 0x5e, 0x00, 0x2f, 0x80, 0x03, 0xc0, 0x01, 0xa0, 0x00, 0x60, 0x11, 0xe0, 0x3f, 0xc0, 
	0x7f, 0x80, 0x3e, 0x00, 0x10, 0x00, 0x00, 0x00
};

//----------------TBD----------------

// 'chord', 12x12px
const unsigned char epd_bitmap_chord []  = {
	0x6d, 0xb0, 0x6d, 0x80, 0x6c, 0x30, 0x61, 0xa0, 0x0d, 0x20, 0x69, 0x60, 0x4b, 0x60, 0x5b, 0x00, 
	0xd8, 0x60, 0xc3, 0x60, 0x1b, 0x60, 0xdb, 0x60
};

//Default ChildOS instrument applications

#define INSTRUMENT_APP1_ICON xy_bmp
#define INSTRUMENT_APP2_ICON rain_bmp
#define INSTRUMENT_APP3_ICON knobs_bmp
#define INSTRUMENT_APP4_ICON keys_bmp
#define INSTRUMENT_APP5_ICON drum_bmp
#define INSTRUMENT_APP6_ICON cv_icon_bmp
#define INSTRUMENT_APP7_ICON rattle_bmp
#define INSTRUMENT_APP8_ICON live_loop_bmp

#define INSTRUMENT_APP1_FUNCTION xyGrid
#define INSTRUMENT_APP2_FUNCTION rain
#define INSTRUMENT_APP3_FUNCTION knobs
#define INSTRUMENT_APP4_FUNCTION keyboard
#define INSTRUMENT_APP5_FUNCTION drumPad
#define INSTRUMENT_APP6_FUNCTION MIDItoCV
#define INSTRUMENT_APP7_FUNCTION rattle
#define INSTRUMENT_APP8_FUNCTION liveLoop

//max 8 characters! otherwise it'll run off the screen
#define INSTRUMENT_APP1_TEXT "joystick"
#define INSTRUMENT_APP2_TEXT "rain"
#define INSTRUMENT_APP3_TEXT "knobs"
#define INSTRUMENT_APP4_TEXT "keys"
#define INSTRUMENT_APP5_TEXT "drumpad"
#define INSTRUMENT_APP6_TEXT "midi cv"
#define INSTRUMENT_APP7_TEXT "rattle"
#define INSTRUMENT_APP8_TEXT "liveloop"

/*
    Check to see if the user has defined these in their code!
    If not, set them to a default function or bitmap
*/

//Icons

#ifndef INSTRUMENT_APP7_ICON
    #define INSTRUMENT_APP7_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP8_ICON
    #define INSTRUMENT_APP8_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP9_ICON
    #define INSTRUMENT_APP9_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP10_ICON
    #define INSTRUMENT_APP10_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP11_ICON
    #define INSTRUMENT_APP11_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP12_ICON
    #define INSTRUMENT_APP12_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP13_ICON
    #define INSTRUMENT_APP13_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP14_ICON
    #define INSTRUMENT_APP14_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP15_ICON
    #define INSTRUMENT_APP15_ICON default_bmp
#endif
#ifndef INSTRUMENT_APP16_ICON
    #define INSTRUMENT_APP16_ICON default_bmp
#endif

//Functions

#ifndef INSTRUMENT_APP7_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP8_FUNCTION
    #define INSTRUMENT_APP8_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP9_FUNCTION
    #define INSTRUMENT_APP9_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP10_FUNCTION
    #define INSTRUMENT_APP10_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP11_FUNCTION
    #define INSTRUMENT_APP11_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP12_FUNCTION
    #define INSTRUMENT_APP12_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP13_FUNCTION
    #define INSTRUMENT_APP13_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP14_FUNCTION
    #define INSTRUMENT_APP14_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP15_FUNCTION
    #define INSTRUMENT_APP15_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP16_FUNCTION
    #define INSTRUMENT_APP16_FUNCTION defaultAppFunction
#endif

//Texts

#ifndef INSTRUMENT_APP7_TEXT
    #define INSTRUMENT_APP7_TEXT ""
#endif
#ifndef INSTRUMENT_APP8_TEXT
    #define INSTRUMENT_APP8_TEXT ""
#endif
#ifndef INSTRUMENT_APP9_TEXT
    #define INSTRUMENT_APP9_TEXT ""
#endif
#ifndef INSTRUMENT_APP10_TEXT
    #define INSTRUMENT_APP10_TEXT ""
#endif
#ifndef INSTRUMENT_APP11_TEXT
    #define INSTRUMENT_APP11_TEXT ""
#endif
#ifndef INSTRUMENT_APP12_TEXT
    #define INSTRUMENT_APP12_TEXT ""
#endif
#ifndef INSTRUMENT_APP13_TEXT
    #define INSTRUMENT_APP13_TEXT ""
#endif
#ifndef INSTRUMENT_APP14_TEXT
    #define INSTRUMENT_APP14_TEXT ""
#endif
#ifndef INSTRUMENT_APP15_TEXT
    #define INSTRUMENT_APP15_TEXT ""
#endif
#ifndef INSTRUMENT_APP16_TEXT
    #define INSTRUMENT_APP16_TEXT ""
#endif

const unsigned char* instrumentApplicationIcons[16] = {
    INSTRUMENT_APP1_ICON,
    INSTRUMENT_APP2_ICON,
    INSTRUMENT_APP3_ICON,
    INSTRUMENT_APP4_ICON,
    INSTRUMENT_APP5_ICON,
    INSTRUMENT_APP6_ICON,
    INSTRUMENT_APP7_ICON,
    INSTRUMENT_APP8_ICON,
    INSTRUMENT_APP9_ICON,
    INSTRUMENT_APP10_ICON,
    INSTRUMENT_APP11_ICON,
    INSTRUMENT_APP12_ICON,
    INSTRUMENT_APP13_ICON,
    INSTRUMENT_APP14_ICON,
    INSTRUMENT_APP15_ICON,
    INSTRUMENT_APP16_ICON
};

//loading all the functions
void (*const instrumentApplicationFunctions[16])() = {
    INSTRUMENT_APP1_FUNCTION,
    INSTRUMENT_APP2_FUNCTION,
    INSTRUMENT_APP3_FUNCTION,
    INSTRUMENT_APP4_FUNCTION,
    INSTRUMENT_APP5_FUNCTION,
    INSTRUMENT_APP6_FUNCTION,
    INSTRUMENT_APP7_FUNCTION,
    INSTRUMENT_APP8_FUNCTION,
    INSTRUMENT_APP9_FUNCTION,
    INSTRUMENT_APP10_FUNCTION,
    INSTRUMENT_APP11_FUNCTION,
    INSTRUMENT_APP12_FUNCTION,
    INSTRUMENT_APP13_FUNCTION,
    INSTRUMENT_APP14_FUNCTION,
    INSTRUMENT_APP15_FUNCTION,
    INSTRUMENT_APP16_FUNCTION
};

const String instrumentApplicationTexts[16] = {
    INSTRUMENT_APP1_TEXT,
    INSTRUMENT_APP2_TEXT,
    INSTRUMENT_APP3_TEXT,
    INSTRUMENT_APP4_TEXT,
    INSTRUMENT_APP5_TEXT,
    INSTRUMENT_APP6_TEXT,
    INSTRUMENT_APP7_TEXT,
    INSTRUMENT_APP8_TEXT,
    INSTRUMENT_APP9_TEXT,
    INSTRUMENT_APP10_TEXT,
    INSTRUMENT_APP11_TEXT,
    INSTRUMENT_APP12_TEXT,
    INSTRUMENT_APP13_TEXT,
    INSTRUMENT_APP14_TEXT,
    INSTRUMENT_APP15_TEXT,
    INSTRUMENT_APP16_TEXT
};



