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
const vector<String> stepChildCCParameters = {"Velocty","Probability","Pitch","BPM [Exp]","Swing [Exp]"};

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

bool stepButtonsAreActive = true;

bool internalClock = true;
bool overwriteRecording = true;
bool recToPrimedTracks = true;
bool screenSaving = false;
bool keys = false;
bool drumPads = false;
bool LEDsOn = true;
bool waitForNote = true;
bool waiting = true;//wait to receive note to begin recording
bool swung = false;

bool isShrunk = false;

//controls whether or not fragmenting is on
bool isFragmenting = false;

bool editingNote = false;

uint16_t cursorPos; //cursor position (before playing)
uint8_t activeLoop;//loop that's currently active
uint8_t activeTrack; //sets which track you're editing, you'll only be able to edit one at a time
uint8_t subDivInt;//sets where the divider bars are in the console output
//cursor jump is locked to this division

uint8_t activeDataTrack;

//counts up for each iteration of a loop
uint8_t loopCount;//controls how many times sequence has looped
bool isLooping = true;//controls whether or not the sequence loops at all
//lets you drag the loop indicators around
//0 is off, 1 is start, -1 is end, 2 is both
int8_t movingLoop = 0;

unsigned short int viewStart;//where the view ends, usually moves by measures but shift lets it move one at a time
unsigned short int viewEnd;//where the view ends

//you could get rid of these! just use lookupData[0].size()
unsigned short int seqStart;
unsigned short int seqEnd;

float scale = 0.5;//HEY this needs to match the initial viewEnd call, otherwise it'll be all fucked up
//basically, viewEnd * scale = 96;

unsigned short int playheadPos;
unsigned short int recheadPos;

uint16_t selectionCount = 0;

const unsigned char debugHeight = 16;
const unsigned char trackDisplay = 32;

//0 is one-shot recording to current loop, 1 is recording to loops as they play in sequence
//2 is recording from seqStart to seqend
int8_t recMode = 0;
//number of steps to waiting for
uint16_t recCountIn = 0;
//number of steps to record for (0 is infinite)
uint16_t recForNSteps = 0;
//rec loop behavior (0 is current loop only, 1 is loops in sequence, 2 is ignore loop points)
int8_t recLoopBehavior = 0;
//post-rec behavior (0 is stop, 1 is play from where you stopped, 2 is play from first loop)
int8_t postRec = 0;
//stop playing behavior. 0 is reset to start of current loop, 1 is reset to first loop, 2 is leave in place
int8_t onStop = 0;
//making new tracks or nah
bool makeNewTracks = true;

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

unsigned short int timestep;

unsigned int passiveTimer;

//humanize values are timing, velocity, and chance
//i very well might go back to using this array instead of the humanizer object
// int8_t humanizeParameters[3] = {0,0,0};
int8_t quantizeAmount = 100;
//holds all the data for the echo fx
uint8_t echoData[3] = {24,75,2};//delay, decay, repeats, target (0 for note, 1 for track, 2 for selection, and 3 for seq)

String menuText;
String currentFile = "";

//stores recent received note as pitch, vel, channel
volatile bool noteOnReceived = false;
volatile bool noteOffReceived = false;
