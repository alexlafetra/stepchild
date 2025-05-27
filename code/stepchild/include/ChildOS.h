/*

  Header file for ChildOS

*/

#define FIRMWARE_VERSION '0.9.2'

#include <vector>
#include <algorithm>

#ifdef HEADLESS
#include "../headless/childOS_headless/headless.h"
#else
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <SoftwareSerial.h>
#include <Wire.h>

//for flash storage
#include <LittleFS.h> // LittleFS is declared
#include "pico/stdlib.h"

//from the pico sdk
extern "C" {
#include "pico.h"
#include "pico/time.h"
#include "pico/bootrom.h"
#include "pico/util/queue.h"
}

#undef CFG_TUH_RPI_PIO_USB
#define CFG_TUH_RPI_PIO_USB 1

#include "hardware.h"   //button/input reading functions
#include "display.h"
using namespace std;
#include "StepchildMIDI.cpp"
#endif

using namespace std;

//reimplementing the map() fn with a new name because it conflicts
long mapVal(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

typedef uint16_t Timestep;
typedef uint8_t TrackID;

/*
-----------------------------
  Class Prototypes
-----------------------------
*/

//Objects for storing data
class Note;
class Track;
class NoteID{
  public:
      NoteID(uint8_t, uint16_t);
      Note getNote();
      uint8_t getPitch();
      uint8_t track;
      uint16_t id;
};

NoteID::NoteID(uint8_t t, uint16_t i){
  track = t;
  id = i;
}

//structs
struct NoteCoords{
  int16_t x1;
  int16_t length;
  int16_t y1;
  int16_t y2;
  void offsetY(int16_t y){
    y1+=y;
    y2+=y;
  }
};

struct CoordinatePair;
struct SequenceRenderSettings;

enum LoopType : uint8_t{
  NORMAL,
  RANDOM,
  RANDOM_SAME,
  RETURN,
  INFINITE
};

LoopType operator++(LoopType &c,int) {
  c = static_cast<LoopType>(static_cast<uint8_t>(c) + 1);
  return c;
}
LoopType operator--(LoopType &c,int) {
  c = static_cast<LoopType>(static_cast<uint8_t>(c) - 1);
  return c;
}

//Stores loop data as start,end,reps,and type
struct Loop{
  //The start of the Loop (in steps)
  uint16_t start;
  //The end of the Loop (in steps)
  uint16_t end;
  //the number of times-1 the loop will play before linking to the next loop. 0 sets the Loop to play once.
  uint8_t reps;
  //how the Loop links to the next Loop
  LoopType type;
  /*
  Type:
  0 = go to next Loop
  1 = go to a random Loop
  2 = go to a random Loop of the same length
  3 = return to the first Loop
  4 = repeat this loop again (infinite repeat)
  */
  Loop(){}
  Loop(uint16_t s, uint16_t e, uint8_t r, uint8_t t){
      this->start = s;
      this->end = e;
      this->reps = r;
      this->type = static_cast<LoopType>(t);
  }
  uint16_t length(){
    return this->end-this->start;
  }
};

uint16_t animOffset = 0;//for animating curves

#include "scales.h"
#include "classes/Curve.h"
#include "graphics/bitmaps.h"            //bitmaps for graphics
#include "functionPrototypes.h" //function prototypes (eventually these should all be refactored into respective files)
#include "clock.h"              //timing functions

/*

  Code for global vars. Most of these should be phased out in future updates!

*/

unsigned const char screenWidth = 128;
unsigned const char screenHeight = 64;

const unsigned char headerHeight = 16;
const unsigned char trackDisplay = 32;

//could probably get rid of these! put them in drawSeq
uint8_t trackHeight;

uint8_t screenBrightness = 255;
bool screenSaverActive = false;
bool core0ready = false;
bool core1ready = false;

bool overwriteRecording = true;
bool overWriteNotesWithEmptiness = false;//flag to set for overwriting notes on tracks that AREN'T receiving notes (overdubbing silence)
bool waitForNoteBeforeRec = true;
bool waitingToReceiveANote = true;//wait to receive note to begin recording
//controls whether or not fragmenting is on
bool isFragmenting = false;

//this could definitely get consumed into the quantize() function (doesn't need to be global)
int8_t quantizeAmount = 100;

//holds all the data for the echo fx
struct EchoData{
  uint8_t delay = 24;
  uint8_t decay = 75;
  uint8_t repeats = 2;
};

EchoData echoData;

struct RandomData{
  int8_t odds = 60;
  int8_t minChance = 100;
  int8_t maxChance = 100;
  uint8_t minLength = 24;
  uint8_t maxLength = 24;
  uint8_t minVel = 100;
  uint8_t maxVel = 127;
  uint8_t everyNSteps = 24;
};

RandomData randomData;

String menuText = "";
String currentFile = "";

//stores recent received note as pitch, vel, channel
volatile bool noteOnReceived = false;
volatile bool noteOffReceived = false;

#include "utils.h"              //common helper functions/utilities
#include "internalCC.h"

// #include "sequence.h"

//classes
#include "graphics/WireFrame.h"//wireframe stuff
#include "graphics/wireframeObjects.h"//wireframe stuff
#include "classes/PlayList.h"
#include "classes/StepchildMenu.h"
#include "classes/Note.h"
#include "classes/Track.h"
#include "classes/Knob.h"
#include "classes/SequenceTemplate.cpp"
#include "classes/AutoTrack.h"
#include "classes/Arp.h"
#include "classes/CV.h"

#include "classes/LiveLooper.h"
#include "sequence.cpp"
#include "classes/LiveLooper.cpp"

#include "midiInputHandlers.cpp"

Note NoteID::getNote(){
  return sequence.noteData[track][id];
}
uint8_t NoteID::getPitch(){
  return sequence.trackData[track].pitch;
}

struct NoteTrackPair{
  public:
      Note note;
      uint8_t trackID;
      NoteTrackPair(Note n, uint8_t t){
          note = n;
          trackID = t;
      }
      uint8_t getPitch(){
        return sequence.trackData[trackID].pitch;
      }
};

//Basic graphic functions
#include "graphics/stepchildGraphics.h"

//16 knobs for the 'controlknobs' instrument
Knob controlKnobs[16];

//These need to be referenced after Autotracks are defined
void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  controls.counterA += (sequence.recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == ENCODER_A)?controls.readEncoder(0)*4:controls.readEncoder(0);
  if(sequence.recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == ENCODER_A)
    waitingToReceiveANote = false;
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  controls.counterB += (sequence.recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == ENCODER_B)?controls.readEncoder(1)*4:controls.readEncoder(1);
  if(sequence.recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == ENCODER_B)
    waitingToReceiveANote = false;
}

#include "classes/SelectionBox.h"
#include "classes/Progression.h"
#include "programChange.h"

//including custom users apps
#include "applications/userApplications.h"
#include "graphics/sequenceRender.h"

#include "helperFunctions.h"

//Instrument apps
#include "applications/applications.h"

#ifndef HEADLESS
#include "webInterface.h"
#else
void webInterface(){}
#endif

//Menus
#include "menus/CVMenu.cpp"
#include "menus/loopMenu.cpp"
#include "menus/consoleMenu.cpp"
#include "menus/instrumentMenu.cpp"
#include "menus/fxMenu.cpp"
#include "menus/arpMenu.cpp"
#include "menus/autotrackMenu.cpp"
#include "menus/trackMenus.cpp"
#include "menus/templateMenu.cpp"
#include "menus/settingsMenu.cpp"
#include "menus/fileMenu.cpp"
#include "menus/noteEditMenu.cpp"
#include "menus/clockMenu.cpp"
#include "menus/liveLoopMenu.cpp"
#include "menus/mainMenu.cpp"
#include "menus/midiMenu.cpp"
#include "menus/quickFXMenu.cpp"

#include "sleep.h"
#include "fileSystem.h"
#include "CCSelector.h"
#include "grooves.h"
#include "superPosition.h"
#include "mainSequence.h"

#include "TBA_Features.h"
