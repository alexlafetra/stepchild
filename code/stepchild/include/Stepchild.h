/*
so much consolidation to do! i think the whole sequence class should be turned into this class
since the main sequence is really the core operation of the stepchild

also menus need a more standard API, so that subclasses can be run from some basic menu.run()

run(){
    this.updateControls()
    this.display()
}

methods. This should help not have such crazy ass functions like "PCEditingEncoderControls," the PCEditor should
just be its own class with its own control method


wireframes should be inside graphics object
maybe all drawing/screen functions should be a part of the graphics object
and then the screen/display class is a member of it too
*/

#pragma once

#ifdef HEADLESS
#include "../headless/childOS_headless/headless.h"
#else
// #include <Arduino.h>
// #include "pico/stdlib.h"
//from the pico sdk
// extern "C" {
// #include "pico.h"
// #include "pico/time.h"
// #include "pico/bootrom.h"
// #include "pico/util/queue.h"
// }

#endif

#include <vector>
#include <Arduino.h>
#include "commonStructs.h"
#include "commonEnums.h"
#include "classes/Track.h"
#include "classes/Note.h"
#include "classes/Autotrack.h"
#include "classes/ProgramChange.h"
#include "classes/SequenceTemplate.h"
#include "classes/LiveLooper.h"
#include "classes/Knob.h"
#include "classes/PlayList.h"
#include "classes/SelectionBox.h"
#include "classes/StepchildMenu.h"
#include "StepchildFileSystem.h"
#include "StepchildClock.h"
#include "StepchildArpeggiator.h"
#include "StepchildCV.h"
#include "StepchildMIDI.h"
#include "StepchildDisplay.h"
#include "StepchildIO.h"
#include "stringPatch.h"
#include "utils.h"
// #include "graphics/WireFrame.h"
#include "classes/Clipboard.h"

class Stepchild{
  public:
    const uint8_t SCREEN_WIDTH = 128;
    const uint8_t SCREEN_HEIGHT = 64;
    const uint8_t HEADER_HEIGHT = 16;
    const uint8_t TRACK_LABEL_WIDTH = 32;
    
    uint32_t lastTime = 0;

    StepchildCV cv;
    StepchildIO buttons;

    StepchildDisplay display = StepchildDisplay(SCREEN_WIDTH, SCREEN_HEIGHT,&SPI1, OLED_DC, OLED_RESET, OLED_CS);
    // I2C variant vv
    // StepchildDisplay display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    StepchildMIDI midi;
    StepchildFileSystem filesystem;
    StepchildClock clock;
    StepchildArpeggiatior arpeggiator;
    LiveLooper liveLoop;
    SelectionBox selectionBox;
    Clipboard clipboard;

    //FX data that should persist after menus are closed
    EchoData echoFXData;
    RandomData randomFXData;
    int8_t quantizeAmount = 100;
    //16 knobs for the 'controlknobs' instrument
    Knob controlKnobs[16];

    GlobalModifiers globalModifiers;

    //tooltip displayed above sequence
    String tooltipText = "";

    std::vector<std::vector<uint16_t>> lookupTable;
    std::vector<std::vector<Note>> noteData;
    std::vector<Track> trackData;

    uint16_t activeTrack = 0;
    uint16_t cursorPos = 0;

    //Not implemented yet! Still using global vars
    uint16_t recheadPos = 0;
    uint16_t playheadPos = 0;

    std::vector<Loop> loopData;
    uint8_t activeLoop = 0;
    uint8_t isLooping = true;
    uint8_t loopCount = 0;

    bool liveLooping = false;
    //not implemented yet!
    bool isFragmenting = false;

    std::vector<Autotrack> autotrackData;
    uint8_t activeAutotrack = 0;

    std::vector<ProgramChange> PCData[5];

    uint16_t sequenceLength = 192;
    uint16_t viewStart = 0;
    uint16_t viewEnd = 192;
    bool shrinkTopDisplay = false;
    uint8_t maxTracksShown = 5;
    // uint8_t startTrack = 0;
    // uint8_t endTrack = 4;
    uint16_t startTrack = 0;
    uint16_t endTrack = 5;
    float viewScale = 0.5;

    uint8_t subDivision = 24;

    uint8_t trackHeight;

    MovingLoopState movingLoop = MOVING_NO_LOOP_POINTS;
    PlayState playState = STOPPED;
    // RecordingMode recMode = ONESHOT;
    RecordingMode recMode = CURRENT_LOOP;
    
    uint8_t defaultChannel = 1;
    uint8_t defaultPitch = 36;
    uint8_t defaultVel = 127;
    uint8_t defaultChance = 100;

    uint16_t selectionCount = 0;

    //true while recording to an autotrack in the autotrack editor
    bool recordingToAutotrack;

    //true when the sequence has received a start trigger
    bool startedPlaying = false;

    //flag set while holding down NEW() in the main editor so you can draw notes
    bool drawingNote = false;
    //setting controlling whether track pitches are rendered as note letter names or note numbers
    bool pitchesOrNumbers = true;

    bool overwriteRecording = false;
    bool overwriteNotesWithEmptiness = false;
    bool waitForNoteBeforeRec = true;

    //Stores a list of all notes that are either being sent or received as NoteSourcePair structs
    //Used to access things like currently sent notes (for the arpeggiator or instruments)
    PlayList sentNotes;
    PlayList receivedNotes;

    //stores the recently received note / cc datapoint
    volatile NoteData recentNote;
    volatile CCData recentCC;

    //flag set to true when the stepchild is waiting for a note in order to start playing or recording
    //and set to false when the note is received
    volatile bool idlingUntilNoteReceived = false;

    //Startup templates/templates you can swap to for automatically 
    const static SequenceTemplate GENERIC_KEYBOARD_TEMPLATE;
    const static SequenceTemplate SP404MK2_TEMPLATE;

    //system utility stuff
    uint8_t screenBrightness = 255;

    //these is barely used/not implemented very consistently
    uint16_t sleepTime = 30000;//10s
    const uint32_t deepSleepTime = 600000;

    bool screenSaverActive = false;

    Stepchild();

    /*
    ----------------------------------------------------------
                        UTILITIES
    ----------------------------------------------------------
    */
    
    void init();
    //creates a sequence object with default values
    void initSequence(uint8_t numberOfTracks,uint16_t length);
    void initSequence();
    void initSequence(SequenceTemplate t);
    bool isQuarterGrid();
    //swaps all the data vars in the sequence for new, blank data
    void eraseSequence();
    Note noteAt(uint8_t track, uint16_t step);
    Note noteAtCursor();
    uint16_t IDAt(uint8_t track, uint16_t step);
    uint16_t IDAtCursor();

    String stepsToMeasures(int32_t stepVal);
    String stepsToPosition(int steps,bool verby);
    String pitchToString(uint8_t input, bool oct, bool sharps);

    int16_t getOctave(int val){return val/12;}
    bool itsbeen(unsigned long time){return ((millis()-lastTime)>=time);}


    /*
    ----------------------------------------------------------
                        PLAYBACK/RECORDING
    ----------------------------------------------------------
    */
    void setPlayMode();
    void setRecMode(bool);
    void setNormalMode();
    void stop();
    void stop(bool);
    void togglePlay();
    void toggleRecording(bool butWait);
    void triggerAutotracks(uint8_t trackID, bool state);
    void playNote(Note& note, uint8_t track, uint16_t timestep);
    void playTrack(uint8_t track, uint16_t timestep);
    void playStep(uint16_t timestep);
    void defaultLoop();
    void arpLoop();
    void checkAutotracks();
    void playingLoop();
    void recordingLoop();
    void checkLoop();
    void cleanupRecording(uint16_t stopTime);
    void updateLookupData();
    void continueStep(uint16_t step);
    void writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value);
    void writeNoteOn(uint16_t step, uint8_t pitch, uint8_t vel, uint8_t channel);
    void writeNoteOff(uint16_t step, uint8_t pitch, uint8_t channel);
    void checkCV();
    void playPCData(Timestep timestep);

    /*
    ----------------------------------------------------------
                        LOADING NOTES
    ----------------------------------------------------------
    */
    //adds a note w/o checking for overlaps
    //Only use this when loading notes from a file into a blank sequence
    void loadNote(Note newNote, uint8_t track);
    /*
    ----------------------------------------------------------
                        DELETING NOTES
    ----------------------------------------------------------
    */
    //Deletes a note on a given track with a given ID
    void deleteNote_byID(uint8_t track, uint16_t targetNoteID);
    void deleteNotes_byID(std::vector<NoteID> targetNoteIDs);
    //deletes a note at a specific time/place
    void deleteNote(uint8_t track, uint16_t time);
    //deletes a note at the current track/cursor position
    void deleteNote();
    void deleteSelected();
    std::vector<NoteID> crunchNoteIDsForDeletion(std::vector<NoteID> targetNoteIDs);
    /*
    ----------------------------------------------------------
                        CREATING NOTES
    ----------------------------------------------------------
    Lots of these are redundant/deprecated overloads... go thru em and get rid of them!
    */
    void makeNote(Note newNoteOn, uint8_t track, bool loudly);
    void makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly);
    void makeNote(Note newNoteOn, uint8_t track);
    void makeNote(uint8_t track, uint16_t time, uint16_t length, uint8_t velocity, uint8_t chance, bool loudly);
    //this one is for quickly placing a ntoe at the cursor, on the active track
    void makeNote(uint8_t track, uint16_t time, uint16_t length, bool loudly);
    void makeNote(uint16_t length, bool loudly);
    //draws notes every "count" subDivs, from viewStart to viewEnd
    //this is a super useful idea for sequencing, but currently only used by the edit menu
    void stencilNotes(uint8_t count);

    //checks if there's a note first, and if there is it deletes it/if not it places one
    void toggleNote(uint8_t track, uint16_t step, uint16_t length);
    void makeNoteEveryNDivisions(uint8_t n);
    /*
    ----------------------------------------------------------
                          SCALES
    ----------------------------------------------------------
    */
    String getScaleName(ScaleName scale);
    std::vector<uint8_t> makePitchListFromScale(ScaleName scale, uint8_t root);
    std::vector<uint8_t> makePitchListFromScale(ScaleName scale, uint8_t root, int8_t numOctaves, uint8_t octave);
    /*
    ----------------------------------------------------------
                        EDITING NOTES
    ----------------------------------------------------------
    */
    //edits a single note
    void editNoteProperty_byID(uint16_t id, uint8_t track, int8_t amount, NoteProperty which);
    //edits all selected notes
    void editNotePropertyOfSelectedNotes(int8_t amount, NoteProperty which);
    //edits a note, and all selected notes, checking to make sure it doesn't double-edit
    void editNoteAndSelected(int8_t amount, NoteProperty which);
    //called by main controls, edits all selected notes
    void changeVel(int8_t amount);
    void changeChance(int8_t amount);
    //changes JUST a specific note
    void changeChance_byID(uint16_t id, uint8_t track, int8_t amount);
    void changeVel_byID(uint16_t id, uint8_t track, int8_t amount);
    void muteNote(uint8_t track, uint16_t id, bool toggle);
    void unmuteNote(uint8_t track, uint16_t id, bool toggle);
    //mutes/unmutes all selected notes
    void setMuteStateOfSelectedNotes(bool state);
    void muteSelectedNotes();
    void unmuteSelectedNotes();
    //cuts a note short at a specific time
    void truncateNote(uint8_t track, uint16_t atTime);
    bool checkNoteMove(Note& targetNote, uint16_t track, uint16_t newTrack, uint16_t newStart);
    bool checkNoteMove(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart);
    //moves a note
    bool moveNote(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart);
    bool moveSelectedNotes(int16_t xOffset, int8_t yOffset);
    //this should move the note the cursor is on (if any)
    bool moveNotes(int16_t xAmount, int8_t yAmount);
    int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id);
    int16_t changeNoteLength(int amount);
    //this one jumps the cursor to the end or start of the note
    void changeNoteLength_jumpToEnds(int16_t amount);
    void changeNoteLengthSelected(int amount);

    //returns a 2D vector containing a row for each track and a copy of each note that's currently selected on each track
    std::vector<std::vector<Note>> grabSelectedNotes();
    std::vector<std::vector<Note>> grabAndDeleteSelectedNotes();
    std::vector<uint16_t> getSelectedNotesBoundingBox();
    std::vector<uint8_t> getTracksWithSelectedNotes();
    void clearSelection(int track, int time);
    void clearSelection();
    void deselectNote(uint8_t track, uint16_t id);
    void selectNotesInTrack(uint8_t track);
    //select a note
    void selectNote(uint8_t track, uint16_t id);
    //togglet a note's selection state  by it's track and ID
    void toggleSelectNote(uint8_t track, uint16_t id, bool additive);
    void selectAllNotesInTrack();
    //selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
    void selectAll();

    /*
    ----------------------------------------------------------
                        Editing Tracks
    ----------------------------------------------------------
    */
    void changeTrackChannel(int id, int newChannel);
    void changeAllTrackChannels(int newChannel);
    void disarmTracksWithNotes();
    void muteTrack(uint16_t trackID);
    void unMuteTrack(uint16_t trackID);
    void toggleMute(uint16_t trackID);
    void muteMultipleTracks(std::vector<uint8_t> ids);
    void soloTrack(uint16_t trackID);
    void unSoloTrack(uint16_t trackID);
    void toggleSolo(uint16_t trackID);
    void eraseMultipleTracks(std::vector<uint8_t> ids);
    void swapTracks(uint16_t track1, uint16_t track2);
    void setTrackToNearestPitch(std::vector<uint8_t>pitches,uint8_t track,bool allowDuplicates);
    void setTrackToNearestUniquePitch(std::vector<uint8_t>pitches,int track);
    void deleteDuplicateEmptyTracks();
    void eraseTrack();
    void eraseTrack(int track);
    void dupeTrack(unsigned short int track);
    int16_t insertTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly, uint8_t loc);
    int16_t addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly);
    void insertTrack(Track newTrack, uint8_t index);
    void sortTrackData(uint8_t type,uint8_t target);
    void addTrack(Track newTrack, bool loudly);
    void addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly);
    void addTrack(unsigned char pitch, unsigned char channel, bool loudly);
    int16_t makeTrackWithPitch(int16_t pitch, uint8_t channel);
    int16_t getTrackWithPitch(int16_t pitch);
    int16_t getTrackWithPitch(int16_t pitch, uint8_t channel);
    //above index is an INCLUSIVE lower bound!
    int16_t getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex);
    uint8_t countEmptyTracks();
    void deleteEmptyTracks();
    void deleteAllTracks();
    void deleteTrack(unsigned short int track);
    void deleteTrack(unsigned short int track, bool hard);
    void deleteTrack(unsigned short int track, bool hard, bool askFirst);
    void setTrackPitch(int track, int note, bool loud);
    void transposeAllChannels(int8_t increment);
    void setTrackChannel(uint16_t track, uint8_t channel, bool loud);
    void transposeAllPitches(int16_t increment);
    void unprimeTracksWithNotes();
    void temporarilyUnprimeTracksWithNotes();
    void reprimeTracks();
    void muteGroups(int callingTrack, int group);

    /*
    ----------------------------------------------------------
                        AUTOTRACKS
    ----------------------------------------------------------
    */
    void createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, std::vector<uint8_t> points,uint8_t pType);
    void createAutotrack(CurveType type, uint8_t cont);
    void createAutotrack(CurveType type);


    /*
    ----------------------------------------------------------
                        EDITING SEQ
    ----------------------------------------------------------
    */
    void addTimeToSeq(uint16_t amount, uint16_t insertPoint);
    void removeTimeFromSeq(uint16_t amount, uint16_t insertPoint);
    /*
    ----------------------------------------------------------
                            Graphics??
    ----------------------------------------------------------
    */
    //displays notes on LEDs
    void displayMainSequenceLEDs();
    /*
    ----------------------------------------------------------
                            Loops
    ----------------------------------------------------------
    */
    void setLoopPoint(int32_t start, bool which);
    void addLoop(Loop newLoop);
    void insertLoop(Loop newLoop, uint8_t index);
    void setActiveLoop(unsigned int id);
    void addLoop();
    void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, uint8_t type);
    void deleteLoop(uint8_t id);
    void toggleLoop();
    //moves to the next loop in loopSeq
    void nextLoop();
    //cuts notes off when loop repeats, then starts new note at beginning
    void cutLoop();
    //moves the whole loop
    void moveLoop(int16_t amount);
    void toggleLoopMove();
    bool pushToNewLoop();
    /*
    ----------------------------------------------------------
                            Status/info
    ----------------------------------------------------------
    */
    //true if Stepchild is sending or receiving notes
    bool isReceiving();
    bool isSending();
    bool isReceivingOrSending();
    bool areThereAnyNotes();
    //checks for notes above or below a track
    bool areThereMoreNotes(bool above);
    uint16_t getNoteCount();
    //counts notes within a range
    uint16_t countNotesInRange(uint16_t start, uint16_t end);
    bool playing();
    bool recording();
    std::vector<uint8_t> getAllActivePitches();
    std::vector<uint8_t> getTracksWithNotes();
    std::vector<uint8_t> getTracksWithNotesInLoop(uint8_t loop);

    /*
    ----------------------------------------------------------
                            Cursor
    ----------------------------------------------------------
    */
    //sets cursor to the visually nearest note
    //steps to pixels = steps*scale
    //for a note to be "visually" closer, it needs to have a smaller pixel
    //distance from the cursor than another note
    //compare trackDistance * trackHeight to stepDistance * scale
    float getDistanceFromNoteToCursor(Note note,uint8_t track);
    void setCursorToNearestNote();
    //changes which track is active, changing only to valid tracks
    bool setActiveTrack(uint8_t newActiveTrack, bool loudly);
    void moveToNextNote_inTrack(bool up);
    //moves thru each step, forward or backward, and moves the cursor to the first note it finds
    void moveToNextNote(bool forward,bool endSnap);
    void moveToNextNote(bool forward);
    //moving the cursor around
    int16_t moveCursor(int moveAmount);
    void setCursor(uint16_t loc);
    void moveCursorIntoView();
    /*
    ----------------------------------------------------------
                            View
    ----------------------------------------------------------
    */
    void setViewStart(uint16_t step);
    void moveView(int16_t val);
    bool isInView(int target);
    //makes sure scale/viewend line up with the display
    void checkView();
    //zooms in/out
    void zoom(bool in);
    /*
    ----------------------------------------------------------
                            Subdiv
    ----------------------------------------------------------
    */
    void changeSubDivInt(bool down);
    void changeSubDivInt(bool down, bool limitToView);
    void toggleTriplets();

    /*
    ----------------------------------------------------------
                            SLEEP
    ----------------------------------------------------------
    RP2040 has both a sleep mode AND a dormant mode, dormant is even more low power.
    Sleep example:
    https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_sleep/hello_sleep.c
    Dormant example:
    https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant.c

    normal sleep mode keeps core 1 running as usual (so midi i/o still works)
    turns off LEDs and the screen, and sleeps core0 while intermittently checking buttons
    this is kind of a "lite" sleep mode, only saves power by turning off screen and LEDs
    but both CPU's are still running at full speed

    This COULD actually "turn off" the screen via SSD1306_command but it looks like I'd need to physically reset it
    using the reset pin... which isn't always exposed on the breakout boards
    */
   void leaveSleepMode();
   void enterSleepMode();
   void leaveDeepSleepMode();
   void enterDeepSleepMode();
};