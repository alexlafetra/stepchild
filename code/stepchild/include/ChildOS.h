#define FIRMWARE_VERSION '0.9.2'

#include "pins.h"   //pin definitions

#ifndef HEADLESS
#include <vector>
#include <algorithm>
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
}

#undef CFG_TUH_RPI_PIO_USB
#define CFG_TUH_RPI_PIO_USB 1

// #define CAPTURECARD

#include "display.h"

//Don't include this if you're in headless mode
//Headless has it's own midi.h
#include "StepchildMIDI.h"
#endif

using namespace std;

typedef uint16_t Timestep;
typedef uint8_t TrackID;

/*
-----------------------------
  Class Prototypes
-----------------------------
*/

//Objects for storing data
class Knob;
class WireFrame;
class Note;
class Track;
class Autotrack;
class SelectionBox;
class ProgramChange;
class Arp;
class Menu;
class NoteID;

//Objects for accessing stepchild functions
class StepchildCV;
class StepchildSequence;//unfinished
class StepchildGraphics;
class StepchildHardwareInput;
class StepchildUtilities;
class StepchildMIDI;
class LowerBoard;

//objects specifically used for menus, apps
class ConsoleLog;
class PlayList;
class Chord;
class Progression;
class Humanizer;
class HumanizeBlob;
class QChord;
class Moon;
class Planet;
class SolarSystem;
class Raindrop;

//structs
struct CoordinatePair;
struct Coordinate;
struct CCData;
struct NoteData;
struct PolarVertex2D;
struct NoteTrackPair;
struct EchoData;
struct RandomData;
struct NoteCoords;
struct SequenceRenderSettings;


enum ScaleName:uint8_t{
  MAJOR,
  HARMONIC_MINOR,
  MELODIC_MINOR,
  MAJOR_PENTATONIC,
  MINOR_PENTATONIC,
  BLUE,
  DORIAN,
  PHRYGIAN,
  LYDIAN,
  MIXOLYDIAN,
  AEOLIAN,
  LOCRIAN
};

ScaleName& operator++(ScaleName& e) {
    using underlying_type = std::underlying_type<ScaleName>::type;
    e = static_cast<ScaleName>(static_cast<underlying_type>(e) + 1);
    
    // Wrap-around logic if necessary
    if (e > LOCRIAN) {
        e = MAJOR; // or handle wrap-around as per your logic
    }
    
    return e;
}
ScaleName operator++(ScaleName& e, int) {
    ScaleName result = e; // Make a copy of the current value
    ++e;               // Increment the original value
    return result;     // Return the copy (the original value before increment)
}
// Define a free-standing function to overload --
ScaleName& operator--(ScaleName& e) {
    using underlying_type = std::underlying_type<ScaleName>::type;
    e = static_cast<ScaleName>(static_cast<underlying_type>(e) - 1);
    
    // Wrap-around logic if necessary
    if (e < MAJOR) {
        e = LOCRIAN; // or handle wrap-around as per your logic
    }
    
    return e;
}

// Define a free-standing function to overload postfix --
ScaleName operator--(ScaleName& e, int) {
    ScaleName result = e; // Make a copy of the current value
    --e;               // Decrement the original value
    return result;     // Return the copy (the original value before decrement)
}

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

#include "bitmaps.h"            //bitmaps for graphics
#include "functionPrototypes.h" //function prototypes (eventually these should all be refactored into respective files)
#include "clock.h"              //timing functions
#include "global.h"             //program boolean flags and global data, constants
#include "hardware.h"   //button/input reading functions
#include "utils.h"              //common helper functions/utilities
#include "internalCC.h"

//classes
#include "graphics/WireFrame.h"//wireframe stuff
#include "graphics/wireframeObjects.h"//wireframe stuff
#include "classes/PlayList.h"
#include "menus/StepchildMenu.h"
#include "classes/Note.h"
#include "classes/Track.h"
#include "classes/Knob.h"
#include "sequence.h"
#include "classes/AutoTrack.h"

//original ChildOS fonts
#include "graphics/fonts/7_segment.cpp"
#include "graphics/fonts/cursive.cpp"
#include "graphics/fonts/small.cpp"
#include "graphics/fonts/arp.cpp"
#include "graphics/fonts/italic.cpp"
#include "graphics/fonts/chunky.cpp"

struct NoteCoords{
  uint8_t x1;
  int16_t length;
  int16_t y1;
  int16_t y2;
  void offsetY(int16_t y){
    y1+=y;
    y2+=y;
  }
};

//Basic graphic functions
#include "graphics/stepchildGraphics.h"
#include "stepchildGUI.h"

//16 knobs for the 'controlknobs' instrument
Knob controlKnobs[16];

//These need to be referenced after Autotracks are defined
void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  controls.counterA += (recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == 1)?controls.readEncoder(0)*4:controls.readEncoder(0);
  if(recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == 1)
    waitingToReceiveANote = false;
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  controls.counterB += (recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == 2)?controls.readEncoder(1)*4:controls.readEncoder(1);
  if(recordingToAutotrack && sequence.autotrackData[sequence.activeAutotrack].recordFrom == 2)
    waitingToReceiveANote = false;
}

#include "classes/SelectionBox.h"
#include "classes/Arp.h"
#include "classes/NoteID.h"
#include "classes/Progression.h"

#include "CV.h"
#include "playback.h"
#include "programChange.h"

//including custom users apps
#include "applications/userApplications.h"
#include "graphics/sequenceRender.h"

#include "helperFunctions.h"

//FX Apps
#include "fx/randomMenu.cpp"
#include "fx/warpMenu.cpp"
#include "fx/strumMenu.cpp"
#include "fx/reverseMenu.cpp"
#include "fx/quantizeMenu.cpp"
#include "fx/humanizeMenu.cpp"
#include "fx/echo.cpp"

//Instrument apps
#include "applications/rattle.cpp"
#include "applications/chordDJ.cpp"
#include "applications/chordBuilder.cpp"
#include "applications/planets.cpp"
#include "applications/rain.cpp"
#include "applications/liveLoop.cpp"
#include "applications/knobs.cpp"
#include "applications/drumPads.cpp"
#include "applications/xy.cpp"
#include "applications/keyboard.h"

#include "applications/applications.h"

#ifndef HEADLESS
#include "webInterface.h"
#else
void webInterface(){}
#endif

//Menus
#include "menus/loopMenu.cpp"
#include "menus/consoleMenu.cpp"
#include "menus/instrumentMenu.cpp"
#include "menus/fxMenu.cpp"
#include "menus/arpMenu.cpp"
#include "menus/autotrackMenu.cpp"
#include "menus/trackMenus.cpp"
#include "menus/settingsMenu.cpp"
#include "menus/fileMenu.cpp"
#include "menus/noteEditMenu.cpp"
#include "menus/clockMenu.cpp"
#include "menus/mainMenu.cpp"
#include "menus/midiMenu.cpp"

#include "trackEditing.h"
#include "sleep.h"
#include "fileSystem.h"
#include "CCSelector.h"
#include "grooves.h"
#include "mainSequence.h"

#include "TBA_Features.h"