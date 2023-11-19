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
  #include <Adafruit_SSD1306.h>

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

//Overloaded stringify function to create strings
#ifndef HEADLESS
  //This works for the Stepchild version of ChildOS
  String stringify(int a){
    return String(a);
  }
  String stringify(uint8_t a){
    return String(a);
  }
  String stringify(int8_t a){
    return String(a);
  }
  String stringify(uint16_t a){
    return String(a);
  }
  String stringify(int16_t a){
    return String(a);
  }
  String stringify(uint32_t a){
    return String(a);
  }
  String stringify(int32_t a){
    return String(a);
  }
  String stringify(std::vector<Note>::size_type a){
    return String(a);
  }
  String stringify(float a){
    return String(a);
  }
  String stringify(const char * a){
    return String(a);
  }
  int toInt(String s){
    return s.toInt();
  }
#endif

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

const Note offNote; //default note, always goes in element 0 of seqData for each track
const vector<Note> defaultVec = {offNote};//default vector for a track, holds offNote at 0
vector<vector<Note>> seqData;//making a 2D vec, number of rows = tracks, number of columns = usable notes, and stores Note objects
vector<vector<Note>> copyBuffer;//stores copied notes

#include "classes/Track.h"

vector<Track> trackData;//holds the tracks in the sequence

#include "classes/Knob.h"
#include "classes/AutomationTrack.h"
#include "classes/Loop.h"
#include "classes/SelectionBox.h"

//These classes need to talk to seqData or trackData
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

//scope this into the "knobs" instrument
Knob controlKnobs[16];

#include "drawingFunctions.h"
#include "scales.h"

//Data variables -------------------------------------------

unsigned short int copyPos[2];//stores the coordinates of the cursor when copying

//stores cursor position in copy pos, makes a copy of all selected notes (or the target note)
vector<vector<uint16_t> > lookupData; //char map of notes; 0 = no note, 1-665,535 = noteID

#include "CV.h"
#include "instruments/planets.cpp"
#include "instruments/rain.cpp"
#include "instruments/liveLoop.cpp"
#include "instruments/knobs.cpp"
#include "instruments/drumPads.cpp"

unsigned short int animOffset = 0;//for animating curves

//Menus
#include "menus/loopMenu.cpp"
#include "menus/consoleMenu.cpp"
