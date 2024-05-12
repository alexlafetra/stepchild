//library changes:
//- added customo graphic to SSD1306 driver
//- switched out the adafruit GFX library for the pico optimized one
//- turned off Sysex in the MIDI library

const uint8_t Screen_SDA = 8;
const uint8_t Screen_SCL = 9;

//ENCODERS
const unsigned char encoderA_Button = 17;
const unsigned char note_clk_Pin = 18;
const unsigned char note_data_Pin = 19;

const unsigned char encoderB_Button = 20;
const unsigned char track_clk_Pin = 21;
const unsigned char track_data_Pin = 22;

//SHIFT REGISTERS
// const unsigned char dataPin_LEDS = 9;//V0.4
const unsigned char dataPin_LEDS = 7;
const unsigned char latchPin_LEDS = 10;
const unsigned char clockPin_LEDS = 11;

const unsigned char buttons_clockEnable = 16;
const unsigned char buttons_dataIn = 13;
const unsigned char buttons_load = 14;
const unsigned char buttons_clockIn = 15;
//doubling up on load, clockin, and clockenable (since i'll read from them simultaneously)
const unsigned char stepButtons_dataIn = 12;

//JOYSTICK
const unsigned char y_Pin = 26;
const unsigned char x_Pin = 27;

//MISC. HARDWARE
const unsigned char Vpin = 29;
const unsigned char usbPin = 24;

const unsigned char ledPin = 6;
const unsigned char onboard_ledPin = 25;

#ifndef HEADLESS
  #include <vector>
  #include <algorithm>
  #include <Arduino.h>
  #include <Adafruit_TinyUSB.h>
  #include <MIDI.h>
  #include <SoftwareSerial.h>

  //for communicating w screen
  #include <Wire.h>

  //for drawing
  #include <Adafruit_GFX.h>//using one optimized for the pico
  #ifdef CAPTURECARD
    #include <SPI.h>
    #include "libraries/Adafruit_SSD1306_CaptureCard.h"
    #include "libraries/Adafruit_SSD1306_CaptureCard.cpp"
  #else
    #include <Adafruit_SSD1306.h>
  #endif

  //for a nice font
  #include <Fonts/FreeSerifItalic9pt7b.h>
  //you only use a few glyphs from these, you don't need to include the whole thing
  #include <Fonts/FreeSerifItalic12pt7b.h>
  #include <Fonts/FreeSerifItalic24pt7b.h>

  //for flash storage
  #include <LittleFS.h> // LittleFS is declared
  #include "pico/stdlib.h"

  //from the pico sdk
  extern "C" {
    #include "pico.h"
    #include "pico/time.h"
    #include "pico/bootrom.h"
  }
  
  //setting up screen
  #define SCREEN_ADDR 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
  // #define SCREEN_ADDR 0x3D

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define OLED_RESET -1   //   QT-PY / XIAO
  // #define OLED_RESET 7   //   QT-PY / XIAO

  #undef SSD1306_NO_SPLASH
  #define SSD1306_NO_SPLASH 1

  #undef CFG_TUH_RPI_PIO_USB
  #define CFG_TUH_RPI_PIO_USB 1

  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  #define UPRIGHT 2
  #define UPSIDEDOWN 0
  #define SIDEWAYS_R 3
  #define SIDEWAYS_L 1
#endif

using namespace std;

//Don't include this if you're in headless mode
//Headless has it's own midi.h
#ifndef HEADLESS
#include "StepchildMIDI.h"
#endif

//class prototypes
class Knob;
class WireFrame;
class Note;
class Track;
class Autotrack;
class SelectionBox;
struct CoordinatePair;

//Data variables -------------------------------------------
unsigned short int copyPos[2];//stores the coordinates of the cursor when copying
vector<vector<uint16_t> > lookupData; //char map of notes; 0 = no note, 1-665,535 = noteID
vector<vector<Note>> seqData;//making a 2D vec, number of rows = tracks, number of columns = usable notes, and stores Note objects
vector<vector<Note>> copyBuffer;//stores copied notes
//holds all the autotracks
vector<Autotrack> autotrackData;
vector<Track> trackData;//holds the tracks in the sequence

//Stores loop data as start,end,reps,and type
struct Loop{
  //The start of the Loop (in steps)
  uint16_t start;
  //The end of the Loop (in steps)
  uint16_t end;
  //the number of times-1 the loop will play before linking to the next loop. 0 sets the Loop to play once.
  uint8_t reps;
  //how the Loop links to the next Loop
  uint8_t type;
  /*
  Type:
  0 = go to next Loop
  1 = go to a random Loop
  2 = go to a random Loop of the same length
  3 = return to the first Loop
  4 = repeat this loop again (infinite repeat)
  */
};
vector<Loop> loopData;

unsigned short int animOffset = 0;//for animating curves

//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
struct GlobalModifiers{
  int8_t velocity[2] = {0,0};
  int8_t chance[2] = {0,0};
  int8_t pitch[2] = {0,0};
};

GlobalModifiers globalModifiers;

//bitmaps for graphics
#include "bitmaps.h"

//function prototypes
#include "functionPrototypes.h"

//program boolean flags and global data, constants
#include "global.h"

//button defs and reading functions
#include "hardware.h"
// #include "buttons.h"

//classes
#include "classes/WireFrame.h"//wireframe stuff
#include "classes/PlayList.h"
#include "classes/CoordinatePair.h"
#include "classes/Menu.h"
#include "classes/Note.h"
#include "classes/Track.h"
#include "classes/Knob.h"

#include "lowerBoard.h"

//string utilities for parsing musical data
#include "stringUtils.h"
#include "utils.h"

//original ChildOS fonts
#include "fonts/7_segment.cpp"
#include "fonts/cursive.cpp"
#include "fonts/small.cpp"
#include "fonts/arp.cpp"
#include "fonts/italic.cpp"
#include "fonts/chunky.cpp"

//Basic graphic functions
#include "graphics.h"

//16 knobs for the 'controlknobs' instrument
Knob controlKnobs[16];

#include "classes/AutoTrack.h"

//These need to be referenced after Autotracks are defined
void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterA += (recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 1)?readEncoder(0)*4:readEncoder(0);
  if(recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 1)
    waitingToReceiveANote = false;
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterB += (recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 2)?readEncoder(1)*4:readEncoder(1);
  if(recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 2)
    waitingToReceiveANote = false;
}

#include "classes/SelectionBox.h"
#include "classes/Loop.h"
#include "classes/Arp.h"
#include "classes/NoteID.h"
#include "classes/Progression.h"

#include "scales.h"
#include "CV.h"
#include "playback.h"
#include "interface.h"
#include "programChange.h"

//including custom users apps
#include "user/userApplications.h"

//FX Apps
#include "fx/randomMenu.cpp"
#include "fx/warpMenu.cpp"
#include "fx/strumMenu.cpp"
#include "fx/reverseMenu.cpp"
#include "fx/quantizeMenu.cpp"
#include "fx/humanizeMenu.cpp"
#include "fx/echo.cpp"
//Instrument apps
#include "instruments/rattle.cpp"
#include "instruments/chordDJ.cpp"
#include "instruments/chordBuilder.cpp"
#include "instruments/planets.cpp"
#include "instruments/rain.cpp"
#include "instruments/liveLoop.cpp"
#include "instruments/knobs.cpp"
#include "instruments/drumPads.cpp"
#include "instruments/xy.cpp"

#include "applications.h"

//Menus
#include "menus/consoleMenu.cpp"
#include "menus/InstrumentMenu.cpp"
#include "menus/arpMenu.cpp"
#include "menus/autotrackMenu.cpp"
#include "menus/trackMenu.cpp"
#include "menus/settingsMenu.cpp"
#include "menus/fileMenu.cpp"
#include "menus/editMenu.cpp"
#include "menus/clockMenu.cpp"
#include "menus/mainMenu.cpp"
#include "menus/midiMenu.cpp"

#include "trackEditing.h"
#include "fileSystem.h"
#include "setup.h"
#include "screenSavers.h"
#include "recording.h"
#include "keyboard.h"
#include "CCSelector.h"
#include "noteEditing.h"

#include "TBA_Features.h"
#include "deprecatedDebugFunctions.h"
