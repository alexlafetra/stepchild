/*
    Contains code for the FX applications and Instrument applications!

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

#define INSTRUMENT_APP7_FUNCTION liveLoop
#define INSTRUMENT_APP7_TEXT "liveloop"
#define INSTRUMENT_APP7_ICON live_loop_bmp
#define INSTRUMENT_APP7_INFO "live looper! record and trigger playback for live performance"


#define NUMBER_OF_APPLICATIONS 7
#define NUMBER_OF_GENERATIVE_INSTRUMENTS 2

const String instrumentApplicationInfo[NUMBER_OF_APPLICATIONS] = {
    INSTRUMENT_APP1_INFO,
    INSTRUMENT_APP2_INFO,
    INSTRUMENT_APP3_INFO,
    INSTRUMENT_APP4_INFO,
    INSTRUMENT_APP5_INFO,
    INSTRUMENT_APP6_INFO,
    INSTRUMENT_APP7_INFO
};

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
bool (*const instrumentApplicationFunctions[16])() = {
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

const String instrumentApplicationTitles[16] = {
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
#define FX_APP2_FUNCTION quantizeMenu
#define FX_APP3_FUNCTION humanizeMenu
#define FX_APP4_FUNCTION strumMenu
#define FX_APP5_FUNCTION echoMenu
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

/*
    Check to see if the user has defined these in their code!
    If not, set them to a default function or bitmap
*/

//Icons
#ifndef FX_APP6_ICON
    #define FX_APP6_ICON default_bmp
#endif
#ifndef FX_APP7_ICON
    #define FX_APP7_ICON default_bmp
#endif
#ifndef FX_APP8_ICON
    #define FX_APP8_ICON default_bmp
#endif
#ifndef FX_APP9_ICON
    #define FX_APP9_ICON default_bmp
#endif
#ifndef FX_APP10_ICON
    #define FX_APP10_ICON default_bmp
#endif
#ifndef FX_APP11_ICON
    #define FX_APP11_ICON default_bmp
#endif
#ifndef FX_APP12_ICON
    #define FX_APP12_ICON default_bmp
#endif
#ifndef FX_APP13_ICON
    #define FX_APP13_ICON default_bmp
#endif
#ifndef FX_APP14_ICON
    #define FX_APP14_ICON default_bmp
#endif
#ifndef FX_APP15_ICON
    #define FX_APP15_ICON default_bmp
#endif
#ifndef FX_APP16_ICON
    #define FX_APP16_ICON default_bmp
#endif
#ifndef FX_APP17_ICON
    #define FX_APP17_ICON default_bmp
#endif
#ifndef FX_APP18_ICON
    #define FX_APP18_ICON default_bmp
#endif
#ifndef FX_APP19_ICON
    #define FX_APP19_ICON default_bmp
#endif
#ifndef FX_APP20_ICON
    #define FX_APP20_ICON default_bmp
#endif
#ifndef FX_APP21_ICON
    #define FX_APP21_ICON default_bmp
#endif
#ifndef FX_APP22_ICON
    #define FX_APP22_ICON default_bmp
#endif
#ifndef FX_APP23_ICON
    #define FX_APP23_ICON default_bmp
#endif
#ifndef FX_APP24_ICON
    #define FX_APP24_ICON default_bmp
#endif

//Functions
#ifndef FX_APP6_FUNCTION
    #define FX_APP6_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP7_FUNCTION
    #define FX_APP7_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP8_FUNCTION
    #define FX_APP8_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP9_FUNCTION
    #define FX_APP9_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP10_FUNCTION
    #define FX_APP10_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP11_FUNCTION
    #define FX_APP11_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP12_FUNCTION
    #define FX_APP12_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP13_FUNCTION
    #define FX_APP13_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP14_FUNCTION
    #define FX_APP14_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP15_FUNCTION
    #define FX_APP15_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP16_FUNCTION
    #define FX_APP16_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP17_FUNCTION
    #define FX_APP17_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP18_FUNCTION
    #define FX_APP18_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP19_FUNCTION
    #define FX_APP19_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP20_FUNCTION
    #define FX_APP20_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP21_FUNCTION
    #define FX_APP21_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP22_FUNCTION
    #define FX_APP22_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP23_FUNCTION
    #define FX_APP23_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP24_FUNCTION
    #define FX_APP24_FUNCTION defaultAppFunction
#endif

//Texts
#ifndef FX_APP6_TEXT
    #define FX_APP6_TEXT ""
#endif
#ifndef FX_APP7_TEXT
    #define FX_APP7_TEXT ""
#endif
#ifndef FX_APP8_TEXT
    #define FX_APP8_TEXT ""
#endif
#ifndef FX_APP9_TEXT
    #define FX_APP9_TEXT ""
#endif
#ifndef FX_APP10_TEXT
    #define FX_APP10_TEXT ""
#endif
#ifndef FX_APP11_TEXT
    #define FX_APP11_TEXT ""
#endif
#ifndef FX_APP12_TEXT
    #define FX_APP12_TEXT ""
#endif
#ifndef FX_APP13_TEXT
    #define FX_APP13_TEXT ""
#endif
#ifndef FX_APP14_TEXT
    #define FX_APP14_TEXT ""
#endif
#ifndef FX_APP15_TEXT
    #define FX_APP15_TEXT ""
#endif
#ifndef FX_APP16_TEXT
    #define FX_APP16_TEXT ""
#endif
#ifndef FX_APP17_TEXT
    #define FX_APP17_TEXT ""
#endif
#ifndef FX_APP18_TEXT
    #define FX_APP18_TEXT ""
#endif
#ifndef FX_APP19_TEXT
    #define FX_APP19_TEXT ""
#endif
#ifndef FX_APP20_TEXT
    #define FX_APP20_TEXT ""
#endif
#ifndef FX_APP21_TEXT
    #define FX_APP21_TEXT ""
#endif
#ifndef FX_APP22_TEXT
    #define FX_APP22_TEXT ""
#endif
#ifndef FX_APP23_TEXT
    #define FX_APP23_TEXT ""
#endif
#ifndef FX_APP24_TEXT
    #define FX_APP24_TEXT ""
#endif

const unsigned char* fxApplicationIcons[24] = {
    FX_APP1_ICON,
    FX_APP2_ICON,
    FX_APP3_ICON,
    FX_APP4_ICON,
    FX_APP5_ICON,
    FX_APP6_ICON,
    FX_APP7_ICON,
    FX_APP8_ICON,
    FX_APP9_ICON,
    FX_APP10_ICON,
    FX_APP11_ICON,
    FX_APP12_ICON,
    FX_APP13_ICON,
    FX_APP14_ICON,
    FX_APP15_ICON,
    FX_APP16_ICON,
    FX_APP17_ICON,
    FX_APP18_ICON,
    FX_APP19_ICON,
    FX_APP20_ICON,
    FX_APP21_ICON,
    FX_APP22_ICON,
    FX_APP23_ICON,
    FX_APP24_ICON
};

//loading all the functions
bool (*const fxApplicationFunctions[24])() = {
    FX_APP1_FUNCTION,
    FX_APP2_FUNCTION,
    FX_APP3_FUNCTION,
    FX_APP4_FUNCTION,
    FX_APP5_FUNCTION,
    FX_APP6_FUNCTION,
    FX_APP7_FUNCTION,
    FX_APP8_FUNCTION,
    FX_APP9_FUNCTION,
    FX_APP10_FUNCTION,
    FX_APP11_FUNCTION,
    FX_APP12_FUNCTION,
    FX_APP13_FUNCTION,
    FX_APP14_FUNCTION,
    FX_APP15_FUNCTION,
    FX_APP16_FUNCTION,
    FX_APP17_FUNCTION,
    FX_APP18_FUNCTION,
    FX_APP19_FUNCTION,
    FX_APP20_FUNCTION,
    FX_APP21_FUNCTION,
    FX_APP22_FUNCTION,
    FX_APP23_FUNCTION,
    FX_APP24_FUNCTION
};

const String fxApplicationTitles[24] = {
    FX_APP1_TEXT,
    FX_APP2_TEXT,
    FX_APP3_TEXT,
    FX_APP4_TEXT,
    FX_APP5_TEXT,
    FX_APP6_TEXT,
    FX_APP7_TEXT,
    FX_APP8_TEXT,
    FX_APP9_TEXT,
    FX_APP10_TEXT,
    FX_APP11_TEXT,
    FX_APP12_TEXT,
    FX_APP13_TEXT,
    FX_APP14_TEXT,
    FX_APP15_TEXT,
    FX_APP16_TEXT,
    FX_APP17_TEXT,
    FX_APP18_TEXT,
    FX_APP19_TEXT,
    FX_APP20_TEXT,
    FX_APP21_TEXT,
    FX_APP22_TEXT,
    FX_APP23_TEXT,
    FX_APP24_TEXT
};
