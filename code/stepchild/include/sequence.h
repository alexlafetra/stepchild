/*

  header file for the StepchildSequence class, contains enums too

*/
enum MovingLoopState{
    MOVING_NO_LOOP_POINTS,
    MOVING_LOOP_END,
    MOVING_LOOP_START,
    MOVING_BOTH_LOOP_POINTS
  };
  
  #ifndef HEADLESS
  enum PlayState{
    STOPPED,
    PLAYING,
    RECORDING,
    LIVELOOPING
  };
  #endif
  
  enum NoteProperty{
    VELOCITY,
    CHANCE,
    PITCH
  };

  //0 is one-shot recording to current loop, 1 is recording to loops as they play in sequence
//2 is recording from seqStart to seqend
enum RecordingMode:uint8_t{
    ONESHOT,
    CURRENT_LOOP,
    LOOP_SEQUENCE,
    FULL_SEQUENCE
};
  
  //MIDI callbacks for different play modes
  void handleStop_playing();
  void handleClock_playing();
  void handleStart_playing();
  void handleClock_recording();
  void handleStart_recording();
  void handleStop_recording();
  void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity);
  void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity);
  void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value);
  void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value);
  void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity);
  void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity);
  void handleStart_Normal();
  void handleStop_Normal();
  void handlePB(uint8_t ch, int val);

  //sorting functions passed as params to the sortTracksBy() function
  bool sortTracksByPitch(Track t1, Track t2){
    return t1.pitch>t2.pitch;
  }
  bool sortTracksByChannel(Track t1, Track t2){
    return t1.channel>t2.channel;
  }
  
  
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
    // uint8_t startTrack = 0;
    // uint8_t endTrack = 4;
    uint16_t startTrack = 0;
    uint16_t endTrack = 5;
    float viewScale = 0.5;
  
    uint8_t subDivision = 24;
  
    MovingLoopState movingLoop = MOVING_NO_LOOP_POINTS;
    PlayState playState = STOPPED;
    RecordingMode recMode = ONESHOT;
    
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

  
    StepchildSequence(){}
    /*
    ----------------------------------------------------------
                        UTILITIES
    ----------------------------------------------------------
    */
    
    //creates a sequence object with default values
    void init(uint8_t numberOfTracks,uint16_t length);
    void init();
    void init(SequenceTemplate t);
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
    void toggleRecording(bool butWait);
    void triggerAutotracks(uint8_t trackID, bool state);
    void setNormalMode();
    void playNote(Note& note, uint8_t track, uint16_t timestep);
    void playTrack(uint8_t track, uint16_t timestep);
    void playStep(uint16_t timestep);
    void stop();
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
    void muteMultipleTracks(vector<uint8_t> ids);
    void soloTrack(uint16_t trackID);
    void unSoloTrack(uint16_t trackID);
    void toggleSolo(uint16_t trackID);
    void eraseMultipleTracks(vector<uint8_t> ids);
    void swapTracks(uint16_t track1, uint16_t track2);
    void setTrackToNearestPitch(vector<uint8_t>pitches,uint8_t track,bool allowDuplicates);
    void setTrackToNearestUniquePitch(vector<uint8_t>pitches,int track);
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
    void changeSubDivInt(bool down);
    void changeSubDivInt(bool down, bool limitToView);
    void toggleTriplets();
  };