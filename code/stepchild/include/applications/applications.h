/*
    Contains code for the FX applications and Instrument applications!

   ======================================
            INSTRUMENTS
   ======================================

    Basically, this file catches all undefined instrument application functions, texts, and icons
    and defines them to the default data.

*/


//FX Apps
#include "../fx/randomMenu.cpp"
#include "../fx/warpMenu.cpp"
#include "../fx/strum.cpp"
#include "../fx/reverse.cpp"
#include "../fx/quantize.cpp"
#include "../fx/humanize.cpp"
#include "../fx/echo.cpp"
#include "../fx/chop.cpp"

#include "rattle.cpp"
#include "chordDJ.cpp"
#include "chordBuilder.cpp"
#include "planets.cpp"
#include "rain.cpp"
#include "knobs.cpp"
#include "drumPads.cpp"
#include "xy.cpp"
#include "keyboard.h"

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

// 'chord', 12x12px
const unsigned char chord_dj_bmp []  = {
	0x6d, 0xb0, 0x6d, 0x80, 0x6c, 0x30, 0x61, 0xa0, 0x0d, 0x20, 0x69, 0x60, 0x4b, 0x60, 0x5b, 0x00, 
	0xd8, 0x60, 0xc3, 0x60, 0x1b, 0x60, 0xdb, 0x60
};

//ChildOS instrument applications

//Generators
#define INSTRUMENT_APP1_FUNCTION rain
#define INSTRUMENT_APP1_TEXT "rain"
#define INSTRUMENT_APP1_ICON rain_bmp
#define INSTRUMENT_APP1_INFO "Send notes in a range of pitches using rainfall"

#define INSTRUMENT_APP2_FUNCTION testSolarSystem
#define INSTRUMENT_APP2_TEXT "solar sys"
#define INSTRUMENT_APP2_ICON planets_bmp
#define INSTRUMENT_APP2_INFO "Send notes based on an orbiting solar system"

//Tools
#define INSTRUMENT_APP3_FUNCTION rattle
#define INSTRUMENT_APP3_TEXT "rattle"
#define INSTRUMENT_APP3_ICON rattle_bmp
#define INSTRUMENT_APP3_INFO "Send weird note rolls using the joystick"

#define INSTRUMENT_APP4_TEXT "joystick"
#define INSTRUMENT_APP4_FUNCTION xyGrid
#define INSTRUMENT_APP4_ICON xy_bmp
#define INSTRUMENT_APP4_INFO "Send CC data using the joystick"

#define INSTRUMENT_APP5_FUNCTION knobs
#define INSTRUMENT_APP5_TEXT "knobs"
#define INSTRUMENT_APP5_ICON knobs_bmp
#define INSTRUMENT_APP5_INFO "Use the Stepchild knobs to send CC data"

#define INSTRUMENT_APP6_FUNCTION chordDJ
#define INSTRUMENT_APP6_TEXT "chord dj"
#define INSTRUMENT_APP6_ICON chord_dj_bmp
#define INSTRUMENT_APP6_INFO "Create, edit, and play back full chords using the step buttons"

#define NUMBER_OF_APPLICATIONS 6
#define NUMBER_OF_GENERATIVE_INSTRUMENTS 2

const String instrumentApplicationInfo[NUMBER_OF_APPLICATIONS] = {
    INSTRUMENT_APP1_INFO,
    INSTRUMENT_APP2_INFO,
    INSTRUMENT_APP3_INFO,
    INSTRUMENT_APP4_INFO,
    INSTRUMENT_APP5_INFO,
    INSTRUMENT_APP6_INFO,
};

const unsigned char* instrumentApplicationIcons[NUMBER_OF_APPLICATIONS] = {
    INSTRUMENT_APP1_ICON,
    INSTRUMENT_APP2_ICON,
    INSTRUMENT_APP3_ICON,
    INSTRUMENT_APP4_ICON,
    INSTRUMENT_APP5_ICON,
    INSTRUMENT_APP6_ICON,
};

//loading all the functions
bool (*const instrumentApplicationFunctions[NUMBER_OF_APPLICATIONS])() = {
    INSTRUMENT_APP1_FUNCTION,
    INSTRUMENT_APP2_FUNCTION,
    INSTRUMENT_APP3_FUNCTION,
    INSTRUMENT_APP4_FUNCTION,
    INSTRUMENT_APP5_FUNCTION,
    INSTRUMENT_APP6_FUNCTION,
};

const String instrumentApplicationTitles[NUMBER_OF_APPLICATIONS] = {
    INSTRUMENT_APP1_TEXT,
    INSTRUMENT_APP2_TEXT,
    INSTRUMENT_APP3_TEXT,
    INSTRUMENT_APP4_TEXT,
    INSTRUMENT_APP5_TEXT,
    INSTRUMENT_APP6_TEXT,
};

/* 
   ======================================
            FX
   ======================================

    Basically, this file catches all undefined fx application functions, texts, and icons
    and defines them to the default data.
    
    It also contains the code for the FX Menu

*/

// 'echo', 12x12px
const unsigned char echo_bmp []  = {
	0x1f, 0x80, 0x20, 0x40, 0x4f, 0x20, 0x90, 0x90, 0xa6, 0x50, 0xa9, 0x50, 0xa9, 0x50, 0xa6, 0x50, 
	0x90, 0x90, 0x4f, 0x20, 0x20, 0x40, 0x1f, 0x80
};
// 'humanize', 12x12px
//(same as heart)
const unsigned char humanize_bmp []  = {
	0x79, 0xe0, 0x86, 0x10, 0xb0, 0x10, 0xa0, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x48, 0x20, 
	0x24, 0x40, 0x10, 0x80, 0x09, 0x00, 0x06, 0x00
};
// 'mayhem', 12x12px
const unsigned char mayhem_bmp []  = {
	0x7f, 0xe0, 0xff, 0xf0, 0xff, 0xf0, 0xcf, 0x30, 0x86, 0x10, 0xcf, 0x30, 0xff, 0xf0, 0x79, 0xe0, 
	0x7f, 0xe0, 0x1f, 0x80, 0x16, 0x80, 0x1f, 0x80
};
// 'quant', 12x12px
const unsigned char quantize_bmp []  = {
	0xaa, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8f, 0xf0, 0x08, 0x10, 0x88, 0x90, 0x08, 0x10, 
	0xaa, 0x90, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0
};
// 'reverse', 12x12px
const unsigned char reverse_bmp []  = {
	0x70, 0x80, 0x78, 0xc0, 0x3f, 0xe0, 0x1f, 0xf0, 0x07, 0xe0, 0x10, 0xc0, 0x30, 0x80, 0x7e, 0x00, 
	0xff, 0x80, 0x7f, 0xc0, 0x31, 0xe0, 0x10, 0xe0
};

// 'splinter', 12x12px
const unsigned char splinter_bmp []  = {
	0x04, 0x70, 0x24, 0xf0, 0x11, 0xf0, 0x03, 0xe0, 0x61, 0xc0, 0x00, 0x80, 0x18, 0x30, 0x3c, 0x00, 
	0x7c, 0x40, 0xf9, 0x20, 0xf1, 0x00, 0xe0, 0x00
};
// 'scramble2', 12x12px
const unsigned char scramble_bmp []  = {
	0x06, 0x00, 0x0f, 0x00, 0x17, 0x80, 0x27, 0xc0, 0x2f, 0xc0, 0x5f, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 
	0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0f, 0x00
};
// 'strum', 12x12px
const unsigned char strum_bmp []  = {
	0xc0, 0x00, 0xe0, 0x00, 0x30, 0x00, 0x1b, 0x00, 0x0f, 0x80, 0x1d, 0x80, 0x1f, 0xe0, 0x07, 0x30, 
	0x63, 0x10, 0x43, 0xb0, 0xc1, 0xe0, 0xc0, 0x00
};
// 'warp', 12x12px
const unsigned char warp_bmp []  = {
	0x00, 0x00, 0x08, 0x00, 0x0a, 0x00, 0x2a, 0x20, 0x2a, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0x2a, 0xa0, 
	0x2a, 0x20, 0x0a, 0x00, 0x08, 0x00, 0x00, 0x00
};

//Default ChildOS FX applications

#define FX_APP1_ICON random_bmp
#define FX_APP2_ICON quantize_bmp
#define FX_APP3_ICON humanize_bmp
#define FX_APP4_ICON strum_bmp
#define FX_APP5_ICON echo_bmp
#define FX_APP6_ICON reverse_bmp
#define FX_APP7_ICON warp_bmp
#define FX_APP8_ICON chop2_bmp

#define FX_APP1_FUNCTION randomMenu
#define FX_APP2_FUNCTION quantize
#define FX_APP3_FUNCTION humanize
#define FX_APP4_FUNCTION strum
#define FX_APP5_FUNCTION echo
#define FX_APP6_FUNCTION reverse
#define FX_APP7_FUNCTION warp
#define FX_APP8_FUNCTION getNotesToChop

#define FX_APP1_TEXT "random"
#define FX_APP2_TEXT "quantize"
#define FX_APP3_TEXT "humanize"
#define FX_APP4_TEXT "strum"
#define FX_APP5_TEXT "echo"
#define FX_APP6_TEXT "reverse"
#define FX_APP7_TEXT "warp"
#define FX_APP8_TEXT "chop"

#define FX_APP1_INFO "Create random notes"
#define FX_APP2_INFO "Quantize notes to a grid"
#define FX_APP3_INFO "Add random offsets to note timing/velocity"
#define FX_APP4_INFO "Set notes to cascade up or down"
#define FX_APP5_INFO "Add an echo behind notes"
#define FX_APP6_INFO "Reverse a section of the sequence"
#define FX_APP7_INFO "Shrink/Grow a section of the sequence"
#define FX_APP8_INFO "subdivide notes into equal pieces"

#define NUMBER_OF_FX 8

const String fxApplicationInfo[NUMBER_OF_FX] = {
    FX_APP1_INFO,
    FX_APP2_INFO,
    FX_APP3_INFO,
    FX_APP4_INFO,
    FX_APP5_INFO,
    FX_APP6_INFO,
    FX_APP7_INFO,
    FX_APP8_INFO
};

const unsigned char* fxApplicationIcons[NUMBER_OF_FX] = {
    FX_APP1_ICON,
    FX_APP2_ICON,
    FX_APP3_ICON,
    FX_APP4_ICON,
    FX_APP5_ICON,
    FX_APP6_ICON,
    FX_APP7_ICON,
    FX_APP8_ICON
};

//loading all the functions
bool (*const fxApplicationFunctions[NUMBER_OF_FX])() = {
    FX_APP1_FUNCTION,
    FX_APP2_FUNCTION,
    FX_APP3_FUNCTION,
    FX_APP4_FUNCTION,
    FX_APP5_FUNCTION,
    FX_APP6_FUNCTION,
    FX_APP7_FUNCTION,
    FX_APP8_FUNCTION
};

const String fxApplicationTitles[NUMBER_OF_FX] = {
    FX_APP1_TEXT,
    FX_APP2_TEXT,
    FX_APP3_TEXT,
    FX_APP4_TEXT,
    FX_APP5_TEXT,
    FX_APP6_TEXT,
    FX_APP7_TEXT,
    FX_APP8_TEXT
};
