unsigned const char screenWidth = 128;
unsigned const char screenHeight = 64;

uint8_t screenBrightness = 255;

//sequence note editor help text
const String helptext[2][23] = {{"n   : draw note","+sh : create track","sel : add note to selection","+sh : select single note",
                                "del : delete note","+sh : mute note","loop: move loop points","+sh : open loop editor","pl  : play/stop",
                                "+sh : record","cpy : copy selection","+sh : paste selection","menu: open menu","</> : move cursor","+sh : move cursor by 1 step",
                                "^/& : change active track","+sh : change velocity","A   : zoom","+sh : change track pitch","+clk: open track menu",
                                "B   : change subdivision","+sh : toggle 1/3 subdivisions","+clk: open note menu"},
                                {"test2","test","test"}};

const String infotext[2] = {"Hey! This is the stepchild sequence editor. This screen is the base note editor where you can add, delete, and see notes in your sequence. [SHIFT] to see the control list.",
                            "This is the track editor."};

const String OSversionNumber = "V0.1";
/*
CC numbers:
1 -- Modulation
7 -- Volume
10 -- Pan
11 -- Expression
64 -- Sustain
121 -- Reset All Controllers
123 -- All Notes Off
*/
const vector<String> CCparameters = {"Bank Select","Mod Wheel","Breath Controller","Undefined","Foot Pedal","Portamento Time",
                                      "Data Entry","Volume","Balance","Undefined","Pan","Expression","Effect 1","Effect 2","Undefined",
                                      "Undefined","Gen. Purpose","Undefined","LSB Cont. for ","Damper Pedal","Portamento","Sostenuto Pedal",
                                      "Soft Pedal","Legato Switch","Hold 2","Sound Cont. 1","Sound Cont. 2","Sound Cont. 3",
                                      "Sound Cont. 4","Sound Cont. 5","Sound Cont. 6","Sound Cont. 7","Sound Cont. 8",
                                      "Sound Cont. 9","Sound Cont. 10","Gen. Purpose","Portamento","Undefined","High Res. Vel. Prefix",
                                      "Undefined","Effect 1 Depth","Effect 2 Depth","Effect 3 Depth", "Effect 4 Depth", "Effect 5 Depth",
                                      "Increment +1","Increment -1","Non-Reg. Param. LSB","Non-Reg. Param. MSB",
                                      "Reg. Param. LSB","Reg. Param. MSB","Undefined"};
const vector<String> MKIICCparameters = {"Ctrl ","EFX On/Off","EFX Selector"};
const vector<String> stepChildCCParameters = {"Velocity","Probability","Pitch","BPM [Exp]","Swing [Exp]"};

#define IDLE 0
#define PLAYING 1
#define RECORDING 2

//Providing all the data and functions that the stepchild needs to run the sequence, but none of the graphics
class StepchildSequence{
  public:
  vector<Note> noteData;
  vector<vector<uint16_t>> lookupData;
  uint16_t activeTrack;
  uint16_t bpm;
  uint16_t cursorPos;
  uint16_t playheadPos;
  uint16_t recheadPos;

  uint16_t viewStart;
  uint16_t viewEnd;

  uint8_t playState;
  StepchildSequence(){

  }
};

#define NEW 0
#define SHIFT 1
#define SELECT 2
#define DELETE 3
#define LOOP 4
#define PLAY 5
#define COPY 6
#define MENU 7

class StepchildInput{
  public:

  //this should probably do all the hardware inits, and get passed a "settings" struct with the pins
  StepchildInput(){

  }
  //stores buttons 1-8
  uint8_t topButtons = 0;
  //stores the 13 step buttons
  uint16_t stepButtons = 0;

  //reads in inputs
  void readTopButtons(){

  }
  void readStepButtons(){

  }

  bool buttonState(uint8_t which){
    return topButtons>>which & 1;
  }

  bool stepButtonState(uint8_t which){
    return stepButtons>>which & 1;
  }
};


uint16_t bpm = 120;

#ifndef HEADLESS
bool playing = false;
bool recording = false;
#endif

bool gotClock = false;
bool hasStarted = false;
bool isArping = false;
bool menuIsActive = false;
bool displayingVel = true;
bool drawingNote = false;
bool pitchesOrNumbers = true;

bool pramOffset = 1;

#define INTERNAL 1
#define EXTERNAL 0
uint8_t clockSource = INTERNAL;
bool swung = false;
bool overwriteRecording = true;
bool onlyRecToPrimedTracks = true;//Not implemented yet
bool waitForNoteBeforeRec = true;
bool waitingToReceiveANote = true;//wait to receive note to begin recording

bool isShrunk = false;

bool screenSaverActive = false;
bool LEDsOn = true;

//when true, new notes that are recorded will start off selected
bool recordedNotesAreSelected = false;

//true when recording in the autotrack editor
bool recordingToAutotrack = false;

//controls whether or not fragmenting is on
bool isFragmenting = false;

bool editingNote = false;

uint16_t cursorPos; //cursor position (before playing)
uint8_t activeLoop;//loop that's currently active
uint8_t activeTrack; //sets which track you're editing, you'll only be able to edit one at a time
uint8_t subDivInt;//sets where the divider bars are in the console output
//cursor jump is locked to this division

uint8_t activeAutotrack;

//counts up for each iteration of a loop
uint8_t loopCount;//controls how many times sequence has looped
bool isLooping = true;//controls whether or not the sequence loops at all

//lets you drag the loop indicators around
//0 is off, 1 is start, -1 is end, 2 is both
int8_t movingLoop = 0;

unsigned short int viewStart;//where the view ends, usually moves by measures but shift lets it move one at a time
unsigned short int viewEnd;//where the view ends

//you could get rid of this! just use lookupData[0].size()
unsigned short int seqEnd;

float scale = 0.5;//HEY this needs to match the initial viewEnd call, otherwise it'll be all fucked up
//basically, viewEnd * scale = 96;

unsigned short int playheadPos;
unsigned short int recheadPos;

uint16_t selectionCount = 0;

const unsigned char headerHeight = 16;
const unsigned char trackDisplay = 32;

//0 is one-shot recording to current loop, 1 is recording to loops as they play in sequence
//2 is recording from seqStart to seqend
#define ONESHOT 0
#define LOOP 1
#define LOOPSEQUENCE 2
#define FULL 3

int8_t recMode = ONESHOT;

//number of steps to waiting for
uint16_t recCountIn = 0;
//number of steps to record for (0 is infinite)
uint16_t recForNSteps = 0;

//post-rec behavior (0 is stop, 1 is play from where you stopped, 2 is play from first loop)
int8_t postRec = 0;
//stop playing behavior. 0 is reset to start of current loop, 1 is reset to first loop, 2 is leave in place
int8_t onStop = 0;

//could probably get rid of these! put them in drawSeq
uint8_t trackHeight;
uint8_t maxTracksShown = 5;
uint8_t startTrack = 0;
uint8_t endTrack;

//timing vars
unsigned long timeLastStepPlayed,MicroSperTimeStep,startTime;
long offBy,timeElapsed;
#ifndef HEADLESS
    bool core0ready = false;
#endif
bool core1ready = false;

//30000/1000 = 30 seconds
const uint16_t sleepTime = 30000;
//120,000ms = 2min
const uint32_t deepSleepTime = 120000;

//swing vars
//holds the amount that the notes swing
int32_t swingVal = 4000;
//holds the subdiv the notes are swung to
uint16_t swingSubDiv = 96;

unsigned char defaultPitch;
unsigned char defaultChannel;
unsigned char defaultVel;//default velocity;

unsigned char keyboardPitch = 36;//holds the lowest key the keyboard is playing

uint16_t timestep;

//humanize values are timing, velocity, and chance
//i very well might go back to using this array instead of the humanizer object
// int8_t humanizeParameters[3] = {0,0,0};
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
  uint16_t minLength = 24;
  uint16_t maxLength = 48;
  uint8_t minVel = 64;
  uint8_t maxVel = 127;
  bool onlyOnGrid = true;//true is on the grid, false is off grid
  bool target = 0;//0 is all, 1 is selected
};

RandomData randomData;

String menuText;
String currentFile = "";

//stores recent received note as pitch, vel, channel
volatile bool noteOnReceived = false;
volatile bool noteOffReceived = false;
