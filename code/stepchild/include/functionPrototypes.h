/*
    This is the big list of function prototypes! Not all functions are here.
*/

//Overloaded stringify function to create strings so that this code is compatible with both the Arduino String class and the C++ string class
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
#else
string stringify(int a){
    return to_string(a);
}
string stringify(string s){
    return s;
}
int toInt(string s){
    if(s == "")
        return 0;
    else
        return stoi(s);
}
#endif


//Debug
void displaySeqSerial();
void keyListen();
void debugNoteCount();
void debugPrint();
void debugPrintLookup();

//WireFrame
void viewPram();

//CV
void writeCVGate(bool state);
void writeCVPitch(uint8_t pitch);
void writeCVClock();

//Automation
void moveAutotrackCursor(int moveAmount);
void changeDataPoint(int8_t);
void smoothAutotrack(uint16_t, uint16_t, uint8_t);
bool isModulated(uint8_t ch);
void autotrackViewer();
String getCCParam(uint8_t param, uint8_t channel, uint8_t type);

//Rec/play
void writeNoteOn(unsigned short int step, uint8_t pitch, uint8_t vel, uint8_t channel);

//Drawing functions
void printItalic(int16_t x, int16_t y, char character, uint16_t c);
void printItalic(int16_t x, int16_t y, String text, uint16_t c);
void drawLogo(uint8_t x1, uint8_t y1);
void displaySeq();
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection);
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, bool shadeOutsideLoop, uint16_t start, uint16_t end);
void drawDisplay();
void drawBox(uint8_t cornerX, uint8_t cornerY, uint8_t width, uint8_t height, uint8_t depth, int8_t xSlant, uint8_t fill);
void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation);
void drawTetra(uint8_t h, uint8_t k, uint8_t height, uint8_t width, int xDepth, int yDepth, uint8_t style, uint16_t c);
void drawBracket(uint8_t h, uint8_t k, uint8_t height, uint8_t width, uint8_t style, uint16_t c);
void drawKeys(uint8_t xStart,uint8_t yStart,uint8_t octave,uint8_t numberOfKeys, bool fromPlaylist);
void drawDrumPads(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads);
void drawArpStepLengths(uint8_t xStart, uint8_t yStart, uint8_t startNote, uint8_t xCursor, bool selected);
void drawEchoMenu(uint8_t cursor);
void drawMoon(int phase, bool forward);
void drawSeq();
void drawCurlyBracket(int16_t x1, uint8_t y1, uint8_t length,uint8_t height,bool start, bool end);
void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation);
void drawHumanizeIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim);
void drawTrackLabel(uint8_t xCoord, uint8_t yCoord, String pitch, bool bigOct, bool channel, uint16_t c);
uint8_t printPitch(uint8_t xCoord, uint8_t yCoord, String pitch, bool bigOct, bool channel, uint16_t c);
void printSmall_centered(int x, int y, String t, uint16_t c);
void printSmall(int, int, String, uint16_t);
void fillSquareVertically(uint8_t x0, uint8_t y0, uint8_t width, uint8_t fillAmount);
void fillSquareDiagonally(uint8_t x0, uint8_t y0, uint8_t width,uint8_t fillAmount);
void keyboardAnimation(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeys, bool into);
void printParam_centered(uint8_t which, uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC);
void printParam(uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC);
void drumPadAnimation(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads, bool into);

//Icons
void drawEchoIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim);
void drawQuantIcon(uint8_t,uint8_t,uint8_t,bool);
void drawDrumIcon(uint8_t x1, uint8_t y1, uint8_t note);
void drawArpModeIcon(uint8_t x1, int8_t y1, uint8_t which,uint16_t c);
void drawWarpIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim);
void drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch);

//Menus
void saveMenu();
void settingsMenu();
void editMenu();
void trackMenu();
void trackEditMenu();
void clockMenu();
void loopMenu();
void fragmentMenu();
void fileMenu();
void recMenu();
void xyGrid();
void displayMiniMenu();
void routeMenu();
void warp();
void reverseNotes();
void inputMenu();
void midiMenu();
void chordBuilder();
void echoMenu();
void humanizeMenu();
void quantizeMenu();
void strumMenu();

//Note editing
void echoNote(int track, int id);
void echoTrack(int track);
void changeNoteLength_jumpToEnds(int16_t amount);
void quantize(bool move_the_cursor,bool deleteNote);
void humanize(bool move);
int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id);
int16_t changeNoteLength(int amount);
void changeNoteLengthSelected(int amount);
void setSuperposition(Note& note,uint8_t t);

void quantizeSelectedNotes(bool deleteNote);
bool quantizeNote(uint8_t track, uint16_t id, bool deleteNote);
bool quantizeNote(uint8_t track, uint16_t id,bool move,bool deleteNote);

vector<vector<Note>> grabAndDeleteSelectedNotes();
void selectAll(bool, bool, bool);
void selectAll();
void selectAll(bool);
void selectNote(int track, int time, bool additive);
void clearSelection();
void clearSelection(int, int);
void selectNote(uint8_t track, uint16_t id);
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool));
void toggleSelectNote(uint8_t, uint16_t, bool); 
vector<vector<uint8_t>> selectMultipleNotes(String,String);
vector<uint8_t> selectSeqArea(String);
bool warpAintoB(CoordinatePair A, CoordinatePair B, bool onlySelected);
void reverse();
void rattle();

//Track editing
int16_t makeTrackWithPitch(int,int);
void deleteAllTracks();
int getTrackWithPitch(int);
int getTrackWithPitch(int,int);
int getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex);
int16_t addTrack_return(unsigned short int,unsigned short int,bool);
int16_t insertTrack_return(unsigned short int,unsigned short int,bool,uint8_t);
void addTrack(Track,bool);
void addTrack(unsigned char);
void addTrack(unsigned char, bool);
void eraseTrack();
void deleteTrack(unsigned short int);
void deleteTrack(unsigned short int track, bool hard);
void setTrackPitch(int, int, bool);
bool setActiveTrack(uint8_t, bool);
vector<uint8_t> selectMultipleTracks(String);
void muteMultipleTracks(vector<uint8_t> ids);
void toggleMute(uint16_t id);
void swapTracks(unsigned short int track1, unsigned short int track2);
void swapTracks();
void eraseMultipleTracks(vector<uint8_t> ids);
void eraseTrack();
void eraseTrack(int track);
void toggleSolo(uint16_t id);
void deleteEmptyTracks();
void dupeTrack(unsigned short int track);
void transposeAllPitches(int increment);
void transposeAllChannels(int increment);
void setTrackChannel(int track, int channel, bool loud);
void sortTracks();
void tuneTracksToScale();

//controls
void clearButtons();
void mainSequencerStepButtons();
void mainSequencerEncoders();
bool echoMenuControls(uint8_t* cursor);
void saveMenuControls();
bool quantizeMenuControls(uint8_t* whichParam, bool* deleteNote);
bool humanizeMenuControls(uint8_t* cursor);
bool mainMenuControls();
void yControls();
void defaultEncoderControls();
void defaultSelectBoxControls();
void defaultJoystickControls(bool);
void defaultSelectControls();
void defaultLoopControls();
void defaultCopyControls();
void rotaryActionA_Handler();
void rotaryActionB_Handler();


//sequence
void zoom(bool);
bool isInView(int);
void updateLookupData_track(unsigned short int);
int16_t moveCursor(int moveAmount);
void selectBox();
void togglePlayMode();
void toggleRecordingMode(bool);
void setCursor(uint16_t loc);
// void cutInsertTime();
void moveToNextNote_inTrack(bool up);
void moveToNextNote(bool forward,bool endSnap);
bool areThereAnyNotes();
bool areThereMoreNotes(bool);
void setCursorToNearestNote();
void checkView();
void checkAutotracks();

//Loops
void moveLoop(int16_t amount);
void setLoopToInfinite(uint8_t targetL);

//RP2040 Hardware
float getBattLevel();
// float analogReadTemp();
void enterBootsel();

//filesystem
void writeSeqFile(String filename);
vector<String> loadFiles();
vector<String> loadFilesAndSizes();
bool loadBackup();
void quickSave();
void loadSeqFile(String filename);
void renameSeqFile(String filename);
bool deleteSeqFile(String filename);
void duplicateSeqFile(String filename);
uint32_t getByteCount(String filename);
void loadSettings();
void loadSavedSettingsFromFile();
void writeCurrentSettingsToFile();
uint32_t getByteCount_standAlone(String filename);

//Misc. helper functions
vector<uint16_t> getSelectedNotesBoundingBox();
vector<uint8_t> selectKeys(uint8_t startRoot);
vector<uint8_t> genScale(uint8_t, uint8_t);
vector<uint8_t> genScale(uint8_t, uint8_t, int8_t,uint8_t);
float swingOffset(unsigned short int step);
String stepsToMeasures(int32_t stepVal);
void changeSubDivInt(bool);
String pitchToString(uint8_t input, bool oct, bool sharps);
bool isInVector(int val, vector<uint8_t> vec);
uint8_t getLowestVal(vector<uint8_t> vec);
String stepsToPosition(int steps,bool verby);
int getOctave(int val);
void changeFragmentSubDivInt(bool down);
void toggleFragmentTriplets();
void keyboard();
void drumPad();
String getMKIIParameterName(uint8_t param,  uint8_t channel);
String getCCParameterName(uint8_t param);
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title);
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*displayFunction)());
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*displayFunction)());
uint8_t countSpaces(String text);
void toggleTriplets();
uint16_t toggleTriplets(uint16_t subDiv);
void changeSubDivInt(bool down);
void changeSubDivInt(bool down, bool limitToView);
uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero);
String convertVectorToPitches(vector<uint8_t> intervals);
uint16_t countNotesInRange(uint16_t start, uint16_t end);
bool isReceiving();
bool isSending();
bool isReceivingOrSending();
String getInterval(int root, int pitch);
void moveView(int16_t val);
void setViewStart(uint16_t step);
void setNormalMode();
String enterText(String title);
void bootscreen();
uint8_t countChar(String text,unsigned char c);
void filterOutUnisonNotes(vector<uint8_t>& notes);
bool quantizeNote(uint8_t track, uint16_t id, bool move, bool deleteNote);
void disarmTracksWithNotes();
uint8_t countDigits_byte(uint8_t number);
uint16_t getNoteCount();
String bytesToString(uint32_t bytes);
void printTrackPitch(uint8_t xCoord, uint8_t yCoord, uint8_t trackID,bool bigOct, bool channel, uint16_t c);

//Graphics
void bootscreen_2();
void alert(String text, int time);
void selectKeysAnimation(bool);
void printItalic_wave(int16_t x, int16_t y, string text, int8_t height, uint8_t timing, uint16_t c);

//MIDI
void handleInternalCC(uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t moveToNextCCParam(uint8_t param, bool up, uint8_t whichList);
uint8_t selectCCParam_autotrack(uint8_t which);

//Playback
void playPCData(uint16_t timestep);
void muteGroups(int callingTrack, int group);
