//library changes:
//- added customo graphic to SSD1306 driver
//- switched out the adafruit GFX library for the pico optimized one
//- turned off Sysex in the MIDI library
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
    #include "libraries/Adafruit_SSD1306.h"
    #include "libraries/Adafruit_SSD1306.cpp"
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

  #include "pins.h"

  //setting up screen
  #define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
  // #define i2c_Address 0x3D //initialize with the I2C addr 0x3C Typically eBay OLED's

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
  SoftwareSerial Serial3 = SoftwareSerial(SerialPIO::NOPIN,txPin_3);
  SoftwareSerial Serial4 = SoftwareSerial(SerialPIO::NOPIN,txPin_4);

  //USB MIDI object
  Adafruit_USBD_MIDI usb_midi;

  //1 and 2 are I/0, 3&4 are just O
  MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI0);
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
  MIDI_CREATE_INSTANCE(SoftwareSerial, Serial3, MIDI3);
  MIDI_CREATE_INSTANCE(SoftwareSerial, Serial4, MIDI4);
#endif

using namespace std;

//class prototypes
class Knob;
class WireFrame;
class Note;
class Track;
class CoordinatePair;
class Autotrack;

//Compatability for arduino 'Strings' and c++ 'strings' (some foresight could've avoided this!)
#include "stringUtils.h"

//Data variables -------------------------------------------
unsigned short int copyPos[2];//stores the coordinates of the cursor when copying
vector<vector<uint16_t> > lookupData; //char map of notes; 0 = no note, 1-665,535 = noteID
vector<vector<Note>> seqData;//making a 2D vec, number of rows = tracks, number of columns = usable notes, and stores Note objects
vector<vector<Note>> copyBuffer;//stores copied notes
//holds all the autotracks
vector<Autotrack> autotrackData;
vector<Track> trackData;//holds the tracks in the sequence

//each byte represents the channels an output will send on
//each BIT of each byte is a channel
uint16_t midiChannels[5] = {65535,65535,65535,65535,65535};

unsigned short int animOffset = 0;//for animating curves

//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
int8_t velModifier[2] = {0,0};
int8_t chanceModifier[2] = {0,0};
int8_t pitchModifier[2] = {0,0};

//bitmaps for graphics
#include "bitmaps.h"

//function prototypes
#include "prototypes.h"

//program boolean flags and global data, constants
#include "global.h"

//button defs and reading functions
#include "buttons.h"

//classes
#include "classes/WireFrame.h"//wireframe stuff
#include "classes/PlayList.h"
#include "classes/CoordinatePair.h"
#include "classes/Menu.h"
#include "classes/Note.h"
#include "classes/Track.h"
#include "classes/Knob.h"
//16 knobs for the 'controlknobs' instrument
Knob controlKnobs[16];
#include "classes/AutomationTrack.h"

//These need to be referenced after Autotracks are defined
void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterA += (recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 1)?readEncoder(0)*4:readEncoder(0);
  if(recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 1)
    waiting = false;
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterB += (recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 2)?readEncoder(1)*4:readEncoder(1);
  if(recordingToAutotrack && autotrackData[activeAutotrack].recordFrom == 2)
    waiting = false;
}

#include "classes/SelectionBox.h"
#include "classes/Loop.h"
#include "classes/Arp.h"
#include "classes/NoteID.h"
#include "classes/Progression.h"

//original ChildOS fonts
#include "fonts/7_segment.cpp"
#include "fonts/cursive.cpp"
#include "fonts/small.cpp"
#include "fonts/arp.cpp"
#include "fonts/italic.cpp"
#include "fonts/chunky.cpp"

#include "drawingFunctions.h"
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
#include "fxApps.h"

//Instrument apps
#include "instruments/rattle.cpp"
#include "instruments/chordDJ.cpp"
#include "instruments/chordBuilder.cpp"
#include "instruments/planets.cpp"
#include "instruments/rain.cpp"
#include "instruments/liveLoop.cpp"
#include "instruments/knobs.cpp"
#include "instruments/drumPads.cpp"
#include "instrumentApps.h"

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

#include "helpScreen.h"
#include "trackFunctions.h"
#include "fileSystem.h"
#include "MIDI.h"
#include "setup.h"
#include "screenSavers.h"

