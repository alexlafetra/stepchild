/*
    This is the big list of function prototypes! Not all functions are here.
*/

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

//Drawing functions
void drawLogo(uint8_t x1, uint8_t y1);
void drawEllipse(uint8_t h, uint8_t k, int a, int b, uint16_t c);
void shadeArea(int16_t, int16_t, int16_t, int16_t, uint8_t);
void shadeRect(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade);
void drawNote_vel(unsigned short int, unsigned short int, unsigned short int, unsigned short int, unsigned short int, bool,bool);
void drawDottedLineH(unsigned short int, unsigned short int, unsigned short int, unsigned short int);
void displaySeq();
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection);
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, bool shadeOutsideLoop, uint16_t start, uint16_t end);
void drawDisplay();
void drawNote(unsigned short int, unsigned short int, unsigned short int, unsigned short int, unsigned short int, bool, bool);
void drawBox(uint8_t cornerX, uint8_t cornerY, uint8_t width, uint8_t height, uint8_t depth, int8_t xSlant, uint8_t fill);
void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation);
void drawPram(uint8_t x1, uint8_t y1);
void drawVU(int8_t x1, int8_t y1, float val);
void drawTetra(uint8_t h, uint8_t k, uint8_t height, uint8_t width, int xDepth, int yDepth, uint8_t style, uint16_t c);
void drawBracket(uint8_t h, uint8_t k, uint8_t height, uint8_t width, uint8_t style, uint16_t c);
void drawCenteredBracket(int x1, int y1, int length, int height);
void drawKeys(uint8_t xStart,uint8_t yStart,uint8_t octave,uint8_t numberOfKeys, bool fromPlaylist);
void drawDrumPads(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads);
void drawLabel(uint8_t x1, uint8_t y1, String text, bool wOrB);
void drawSlider(uint8_t x1, uint8_t y1, uint8_t w, uint8_t h, bool state);
void drawSlider(uint8_t x1, uint8_t y1, String a, String b, bool state);
void drawBanner(int8_t x1, int8_t y1, String text);
void drawArpStepLengths(uint8_t xStart, uint8_t yStart, uint8_t startNote, uint8_t xCursor, bool selected);
void drawArpModeIcon(uint8_t x1, int8_t y1, uint8_t which,uint16_t c);
void drawWarpIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim);
void drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch);
void drawFullKeyBed(vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave);
void drawFullKeyBed(uint8_t y1, vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave);
void drawEchoMenu(uint8_t cursor);
void drawDrumIcon(uint8_t x1, uint8_t y1, uint8_t note);
void drawWormhole();
void drawMoon(int phase, bool forward);
void drawPlayIcon(int8_t x1, int8_t y1);
void drawSeq();
void drawCurlyBracket(int16_t x1, uint8_t y1, uint8_t length,uint8_t height,bool start, bool end);
void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation);
void drawQuantIcon(uint8_t,uint8_t,uint8_t,bool);
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
void printChannel(uint8_t xPos, uint8_t yPos, uint8_t channel, bool withBox);
void drumPadAnimation(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads, bool into);
void drawPendulum(int16_t x2, int16_t y2, int8_t length, float val,uint8_t r);

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
void slideMenuIn(int fromWhere, int speed);
void slideMenuOut(int fromWhere, int speed);
void strumMenu();

//Note editing
void eraseSeq();
void makeNote(int, bool);
void makeNote(int, int, int, bool);
void makeNote(int, int, int, int, int, bool, bool, bool);
void deleteNote();
void deleteNote_byID(int track, int targetNoteID);
bool moveNote(int, int, int, int);
bool moveNotes(int xAmount, int yAmount);

unsigned short int getRecentNoteID();
unsigned short int getRecentNoteID(int, int);
void changeVel(int amount);
void deleteNote(int track, int time);

void muteNote(unsigned short int track, unsigned short int id, bool toggle);
void echoNote(int track, int id);
void echoTrack(int track);
void changeChance(int amount);
void changeNoteLength_jumpToEnds(int16_t amount);
void quantize(bool move_the_cursor,bool deleteNote);
void humanize(bool move);
void stencilNotes(uint8_t count);
int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id);
int16_t changeNoteLength(int amount);
void changeNoteLengthSelected(int amount);
void loadNote(int id, int track, int start, int velocity, bool isMuted, int chance, int end, bool selected);
void changeVelSelected(int amount);
void changeVel(int amount);
void changeVel(int id, int track, int amount);
void changeChance(int amount);
void changeChance(int noteID, int track, int amount);
void changeChanceSelected(int amount);
void reverseTrack(uint8_t track, uint16_t start, uint16_t end);
bool warpChunk(uint16_t start, uint16_t end, uint16_t trackStart, uint16_t trackEnd, int16_t amount);
bool checkNoteMove(int id, int track, int newTrack, int newStart);
void quantizeSelectedNotes(bool deleteNote);
bool quantizeNote(uint8_t track, uint16_t id, bool deleteNote);
bool quantizeNote(uint8_t track, uint16_t id,bool move,bool deleteNote);
void makeNote(Note newNoteOn, int track, bool loudly);
void makeNote(Note newNoteOn, int track);
void loadNote(Note newNote, uint8_t track);
bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart);
vector<vector<Note>> grabAndDeleteSelectedNotes();
void deleteSelected();
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
void deleteAllTracks();
int getTrackWithPitch(int);
int getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex);
int16_t addTrack_return(unsigned short int,unsigned short int,bool);
void addTrack(unsigned char);
void addTrack(unsigned char, bool);
void eraseTrack();
void deleteTrack(unsigned short int);
void deleteTrack(unsigned short int track, bool hard);
void setTrackPitch(int, int, bool);
void setActiveTrack(uint8_t, bool);
vector<uint8_t> selectMultipleTracks(String);
String getTrackPitch(int id);
String getTrackPitchOctave(int id);
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
void stepButtons();
void readJoystick();
void readButtons();
void mainSequencerEncoders();
bool fxMenuControls();
bool echoMenuControls(uint8_t* cursor);
void saveMenuControls();
bool quantizeMenuControls(uint8_t* whichParam, bool* deleteNote);
bool humanizeMenuControls(uint8_t* cursor);
bool mainMenuControls();
bool anyActiveInputs();
void yControls();
void defaultEncoderControls();
void defaultSelectBoxControls();
void defaultJoystickControls(bool);
void defaultSelectControls();
void defaultLoopControls();
void defaultCopyControls();
int8_t readEncoder(bool encoder);
void rotaryActionA_Handler();
void rotaryActionB_Handler();


//sequence
void zoom(bool);
void initSeq(int tracks, int length);
void initSeq();
bool isInView(int);
void updateLookupData_track(unsigned short int);
int16_t moveCursor(int moveAmount);
void selectBox();
void togglePlayMode();
void toggleRecordingMode(bool);
void copy();
void paste();
void setCursor(uint16_t loc);
void removeTimeFromSeq(uint16_t amount, uint16_t insertPoint);
void addTimeToSeq(uint16_t amount, uint16_t insertPoint);
void cutInsertTime();
void moveToNextNote_inTrack(bool up);
void moveToNextNote(bool forward,bool endSnap);
bool areThereAnyNotes();
void setCursorToNearestNote();
void setBpm(int newBpm);
void checkView();
void turnOffLEDs();

//Loops
void moveLoop(int16_t amount);
void setActiveLoop(unsigned int id);
void deleteLoop(uint8_t id);
void setLoopToInfinite(uint8_t targetL);

//RP2040 Hardware
float getBattLevel();
float analogReadTemp();
void enterBootsel();

//filesystem
void writeSeqFile(String filename);
vector<String> loadFiles();
vector<String> loadFilesAndSizes();
void loadBackup();
void quickSave();
void loadSeqFile(String filename);
void renameSeqFile(String filename);
bool deleteSeqFile(String filename);
void duplicateSeqFile(String filename);
void exportSeqFileToSerial(String filename);
void exportSeqFileToSerial_standAlone(String filename);
void dumpFilesToSerial();
uint32_t getByteCount(String filename);
void loadSettings();
void loadSavedSettingsFromFile();
void writeCurrentSettingsToFile();
uint32_t getByteCount_standAlone(String filename);

//Misc. helper functions
vector<uint16_t> getSelectionBounds2();
bool itsbeen(int);
vector<uint8_t> selectKeys();
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
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*displayFunction)());
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*displayFunction)());
uint8_t countSpaces(String text);
void toggleTriplets();
uint16_t toggleTriplets(uint16_t subDiv);
void changeSubDivInt(bool down);
void changeSubDivInt(bool down, bool limitToView);
uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero);
String getPitchList(vector<uint8_t> intervals);
uint16_t countNotesInRange(uint16_t start, uint16_t end);
bool isReceiving();
bool isSending();
bool isReceivingOrSending();
String getInterval(int root, int pitch);

bool onBeat(int subDiv, int fudge);
void setNormalMode();
String enterText(String title);
void bootscreen();
uint16_t getIDAtCursor();
uint8_t countChar(String text,unsigned char c);
void filterOutUnisonNotes(vector<uint8_t>& notes);
bool quantizeNote(uint8_t track, uint16_t id, bool move, bool deleteNote);
void disarmTracksWithNotes();
uint8_t countDigits_byte(uint8_t number);
uint16_t getNoteCount();
String bytesToString(uint32_t bytes);
void newSeq();

//Graphics
void bootscreen_2();
void alert(String text, int time);
void drawWebLink();
void displayHelpText(uint8_t);
void updateLEDs();
void writeLEDs(uint8_t led, bool state);
void writeLEDs(uint8_t first, uint8_t last);
void writeLEDs(bool leds[8]);
void drawBinarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, bool state);
void selectKeysAnimation(bool);
void printFraction_small_centered(uint8_t x1, uint8_t y1, String fraction);
void drawArrow(uint8_t pointX, uint8_t pointY, uint8_t size, uint8_t direction, bool full);
uint8_t printFraction_small(uint8_t x1, uint8_t y1, String fraction);

//MIDI
void sendMIDIallOff();
void sendMIDInoteOn(uint8_t,uint8_t,uint8_t);
void sendMIDInoteOff(uint8_t,uint8_t,uint8_t);
void handleInternalCC(uint8_t, uint8_t, uint8_t, uint8_t);
void sendMIDICC(uint8_t,uint8_t,uint8_t);
uint8_t moveToNextCCParam(uint8_t param, bool up, uint8_t whichList);
uint8_t selectCCParam_autotrack(uint8_t which);
bool isActiveChannel(uint8_t channel, uint8_t output);
void sendMIDIProgramChange(uint8_t port, uint8_t val, uint8_t channel);
bool isThru(uint8_t i);
void setThru(uint8_t i, bool state);

//Playback
void playPCData(uint16_t timestep);
void muteGroups(int callingTrack, int group);