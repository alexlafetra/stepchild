#define FIRMWARE_VERSION 0.9.2

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
  Loop(){}
  Loop(uint16_t s, uint16_t e, uint8_t r, uint8_t t){
      this->start = s;
      this->end = e;
      this->reps = r;
      this->type = t;
  }
  uint16_t length(){
    return this->end-this->start;
  }
};

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

uint16_t animOffset = 0;//for animating curves

#include "bitmaps.h"            //bitmaps for graphics
#include "functionPrototypes.h" //function prototypes (eventually these should all be refactored into respective files)
#include "clock.h"              //timing functions
#include "global.h"             //program boolean flags and global data, constants
#include "hardwareControls.h"   //button/input reading functions
#include "utils.h"              //common helper functions/utilities

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

//Basic graphic functions
#include "graphics/stepchildGraphics.h"

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

#include "scales.h"
#include "CV.h"
#include "playback.h"
#include "interface.h"
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

#include "applications/applications.h"

#include "webInterface.h"

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
#include "recording.h"
#include "keyboard.h"
#include "CCSelector.h"
#include "grooves.h"
#include "mainSequence.h"

#include "TBA_Features.h"
#include "everything.h"