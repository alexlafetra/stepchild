//Providing all the data and functions that the stepchild needs to run the sequence, but none of the graphics
class StepchildSequence{
  public:
  vector<vector<uint16_t>> lookupTable;
  vector<vector<Note>> noteData;
  vector<Track> trackData;

  uint16_t activeTrack = 0;
  uint16_t cursorPos = 0;

  //Not implemented yet! Still using global vars
  uint16_t recheadPos = 0;
  uint16_t playheadPos = 0;

  vector<Loop> loopData;
  uint8_t activeLoop = 0;
  uint8_t isLooping = true;
  uint8_t loopCount = 0;

  vector<Autotrack> autotrackData;
  uint8_t activeAutotrack = 0;

  uint16_t sequenceLength = 192;
  uint16_t viewStart = 0;
  uint16_t viewEnd = 192;
  bool shrinkTopDisplay = false;
  uint8_t maxTracksShown = 5;
  uint8_t startTrack = 0;
  uint8_t endTrack = 4;
  float viewScale = 0.5;

  uint8_t subDivision = 24;

  enum MovingLoopState{
    NONE,
    START,
    END,
    BOTH
  };

  MovingLoopState movingLoop;

  enum PlayState{
      STOPPED,
      PLAYING,
      RECORDING,
      LIVELOOPING
  };

  PlayState playState = STOPPED;

  enum SequenceTemplate{
    DEFAULT,
    SP404MKII,
    VOLCA,
    SR_16
  };
  
  uint8_t defaultChannel = 1;
  uint8_t defaultPitch = 36;
  uint8_t defaultVel = 127;
  uint8_t defaultChance = 100;

  uint16_t selectionCount = 0;

  enum NoteProperty{
      VELOCITY,
      CHANCE,
      PITCH
  };

  StepchildSequence(){}
  /*
  ----------------------------------------------------------
                      UTILITIES
  ----------------------------------------------------------
  */
  
  //creates a sequence object with default values
  void init(uint8_t numberOfTracks,uint16_t length);
  void init();
  bool isQuarterGrid();
  //swaps all the data vars in the sequence for new, blank data
  void erase();
  Note noteAt(uint8_t track, uint16_t step);
  Note noteAtCursor();
  uint16_t IDAt(uint8_t track, uint16_t step);
  uint16_t IDAtCursor();

    /*
  ----------------------------------------------------------
                      PLAYBACK/RECORDING
  ----------------------------------------------------------
  */
  void togglePlay();
  void toggleRecording();
  /*
  ----------------------------------------------------------
                      LOADING NOTES
  ----------------------------------------------------------
  */
  //adds a note w/o checking for overlaps
  //Only use this when loading notes from a file into a blank sequence
  void loadNote(Note newNote, uint8_t track);
  void loadNote(uint16_t id, uint8_t track, uint16_t start, uint8_t velocity, bool isMuted, uint8_t chance, uint16_t end, bool selected);
  void loadNote(uint8_t whichTrack, uint16_t start, uint16_t end, uint8_t velocity, uint8_t chance, uint8_t flags);
  /*
  ----------------------------------------------------------
                      DELETING NOTES
  ----------------------------------------------------------
  */
  //Deletes a note on a given track with a given ID
  void deleteNote_byID(uint8_t track, uint16_t targetNoteID);
  //deletes a note at a specific time/place
  void deleteNote(uint8_t track, uint16_t time);
  //deletes a note at the current track/cursor position
  void deleteNote();
  void deleteSelected();
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
  bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart);
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
  /*
  ----------------------------------------------------------
                      Editing Tracks
  ----------------------------------------------------------
  */
  //sort function for sorting tracks by channel
  bool compareChannels(Track t1, Track t2);
  void changeTrackChannel(int id, int newChannel);
  void changeAllTrackChannels(int newChannel);
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
  float getNoteDensity(uint16_t timestep);
  float getNoteDensity(uint16_t start, uint16_t end);
  //counts notes within a range
  uint16_t countNotesInRange(uint16_t start, uint16_t end);
  bool playing();
  bool recording();
  bool liveLooping();
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
  uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero);
  void changeSubDivInt(bool down);
  void changeSubDivInt(bool down, bool limitToView);
  uint16_t toggleTriplets(uint16_t subDiv);
  void toggleTriplets();
};

#include "sequence.cpp"

StepchildSequence sequence;