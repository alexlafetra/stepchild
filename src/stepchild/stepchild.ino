//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

// #define CAPTURECARD

#include "ChildOS.h"

uint16_t getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    count+=seqData[track].size()-1;
  }
  return count;
}

float getNoteDensity(int timestep){
  float density = 0;
  for(int track = 0; track<trackData.size(); track++){
    if(lookupData[track][timestep] != 0){
      density++;
    }
  }
  return density/float(trackData.size());
}
float getNoteDensity(int start, int end){
  float density;
  for(int i = start; i<= end; i++){
    density+=getNoteDensity(i);
  }
  return density/float(end-start+1);
}

uint16_t getLookupID(uint8_t track, uint16_t pos){
  return lookupData[track][pos];
}

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      velModifier[0] = channel;
      velModifier[1] = val-63;
      break;
    //chance
    case 1:
      chanceModifier[0] = channel;
      chanceModifier[1] = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      pitchModifier[0] = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      pitchModifier[1] = float(val)/float(127) * 48 - 24;
      break;
    //bpm
    case 3:
      setBpm(float(val)*2);
      break;
    //swing amount
    case 4:
      swingVal += val-63;
    //track channel
    case 5:
      break;
  }
}

//gets a note from data, returns a note object
Note getNote(int track, int timestep) {
  unsigned short int noteID;
  if (lookupData[track][timestep] != 0) {
    noteID = lookupData[track][timestep];
    Note targetNote = seqData[track][noteID];
    return targetNote;
  }
  else {
    return Note();
  }
}

//counts notes
uint16_t countNotesInRange(uint16_t start, uint16_t end){
  uint16_t count = 0;
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there are no notes, ignore it
    if(seqData[t].size() == 1)
      continue;
    else{
      //move over each note
      for(uint16_t i = 1; i<seqData.size(); i++){
        if(seqData[t][i].startPos>=start && seqData[t][i].startPos<end)
          count++;
      }
    }
  }
  return count;
}

//Sequence parameters -----------------------------------------------------------
void initSeq(int tracks, int length) {
  defaultChannel = 1;
  defaultPitch = 36;

  bpm = 120;
  // seqStart = 0;
  //default is 4 measures = 24*4*4=384
  //1 measure  = 96
  seqEnd = length;
  //6144 = 64 measures
  // seqEnd = 6144;
  // seqEnd = 28800;//<--can it run this?? theoretical max is 65,000 before overflow

  Loop firstLoop;
  firstLoop.start = 0;
  firstLoop.end = 96;
  firstLoop.reps = 0;
  firstLoop.type = 0;
  loopData.push_back(firstLoop);
  activeLoop = 0;
  isLooping = 1;

  viewStart = 0;
  viewEnd = 192;
  // viewLength = viewEnd-viewStart;
  subDivInt = subDivInt; //default note length

  counterA = 0;
  counterB = 0;

  cursorPos = viewStart; //cursor position (before playing)
  activeTrack = 0; //sets which track you're editing, you'll only be able to edit one at a time
  subDivInt = 24;//sets where the divider bars are in the console output
  //cursor jump is locked to this division
  defaultVel = 127;

  MicroSperTimeStep = round(2500000/bpm);

  seqData.resize(tracks);
  lookupData.resize(tracks);
  trackData = {};

  //this is so we can count down, instead of up
  defaultPitch += tracks-1;

  for(int i = 0; i < tracks; i++){
    lookupData[i].resize(seqEnd+1, 0);
    seqData[i] = {Note()};
    Track newTrack;
    trackData.push_back(newTrack);
    defaultPitch--;
  }

  loadSettings();
}

void initSeq(){
  initSeq(4,768);
}


void newSeq(){
  selectionCount = 0;
  vector<vector<uint16_t>> newLookupData;
  newLookupData.swap(lookupData);
  vector<vector<Note>> newSeqData;
  newSeqData.swap(seqData);
  vector<Track> newTrackData;
  newTrackData.swap(trackData);
}

//changes which track is active, changing only to valid tracks
void setActiveTrack(uint8_t newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < trackData.size()) {
    if(activeTrack == 4 && newActiveTrack == 5 && maxTracksShown == 5){
      maxTracksShown = 6;
    }
    else if(activeTrack == 1 && newActiveTrack == 0 && maxTracksShown == 6){
      maxTracksShown = 5;
    }
    activeTrack = newActiveTrack;
    if (loudly) {
      MIDI.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      MIDI.noteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
      if(trackData[activeTrack].isLatched){
        MIDI.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
        MIDI.noteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
      }
    }
  }
  updateLEDs();
  menuText = pitchToString(trackData[activeTrack].pitch,true,true);
}

void changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    trackData[id].channel = newChannel;
  }
}

void changeAllTrackChannels(int newChannel){
  for(int track = 0; track<trackData.size(); track++){
    changeTrackChannel(track, newChannel);
  }
}

void moveToNextNote_inTrack(bool up){
  uint8_t track = activeTrack;
  uint16_t currentID = lookupData[activeTrack][cursorPos];
  bool foundTrack = false;
  //moving the track up/down until it hits a track with notes
  //and checking bounds
  if(up){
    while(track<trackData.size()-1){
      track++;
      if(seqData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  else{
    while(track>0){
      track--;
      if(seqData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  //if you couldn't find a track with a note on it, just return
  if(!foundTrack){
    return;
  }
  for(uint16_t dist = 0; dist<seqEnd; dist++){
    bool stillValid = false;
    //if the new position is in bounds
    if(cursorPos+dist<=seqEnd){
      stillValid = true;
      //and if there's something there!
      if(lookupData[track][cursorPos+dist] != 0){
        //move to it
        moveCursor(dist);
        setActiveTrack(track,false);
        return;
      }
    }
    if(cursorPos>=dist){
      stillValid = true;
      if(lookupData[track][cursorPos-dist] != 0){
        moveCursor(-dist);
        setActiveTrack(track,false);
        return;
      }
    }
    //if it's reached the bounds
    if(!stillValid){
      return;
    }
  }
}

//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void moveToNextNote(bool forward,bool endSnap){
  //if there's a note on this track at all
  if(seqData[activeTrack].size()>1){
    unsigned short int id = lookupData[activeTrack][cursorPos];
    if(forward){
      for(int i = cursorPos; i<seqEnd; i++){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          moveCursor(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(seqEnd-cursorPos);
      }
      return;
    }
    else{
      for(int i = cursorPos; i>0; i--){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          moveCursor(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(-cursorPos);
      }
      return;
    }
  }
}

void moveToNextNote(bool forward){
  moveToNextNote(forward, false);
}

//View ------------------------------------------------------------------
//view start is inclusive, starts at 0
//view end is inclusive, 127
//handles making sure the view is correct
void moveView(int16_t val) {
  int oldViewLength = viewEnd-viewStart;
  if((viewStart+val)<0){
    viewStart = 0;
    viewEnd = viewStart+oldViewLength;
  }
  if(viewEnd+val>seqEnd){
    viewEnd = seqEnd;
    viewStart = viewEnd - oldViewLength;
  }
  if(viewEnd+val<=seqEnd && viewStart+val>=0){
    viewStart += val;
    viewEnd += val;
  }
}

//moving the cursor around
int16_t moveCursor(int moveAmount){
  int16_t amt;
  //if you're trying to move back at the start
  if(cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && cursorPos+moveAmount<0){
    amt = cursorPos;
    cursorPos = 0;
  }
  else{
    cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(cursorPos > seqEnd) {
    amt += (seqEnd-cursorPos);
    cursorPos = seqEnd;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(seqData[activeTrack][seqData[activeTrack].size()-1].endPos<cursorPos)
      changeNoteLength(cursorPos-seqData[activeTrack][seqData[activeTrack].size()-1].endPos,activeTrack,seqData[activeTrack].size()-1);
  }
  //Move the view along with the cursor
  if(cursorPos<viewStart+subDivInt && viewStart>0){
    moveView(cursorPos - (viewStart+subDivInt));
  }
  else if(cursorPos > viewEnd-subDivInt && viewEnd<seqEnd){
    moveView(cursorPos - (viewEnd-subDivInt));
  }
  //update the LEDs
  updateLEDs();
  menuText = (moveAmount>0)?(stepsToPosition(cursorPos,true)+">>"):("<<"+stepsToPosition(cursorPos,true));
  return amt;
}

void setCursor(uint16_t loc){
  moveCursor(loc-cursorPos);
}

void moveCursorIntoView(){
  if (cursorPos < 0) {
    cursorPos = 0;
  }
  if (cursorPos > seqEnd-1) {
    cursorPos = seqEnd-1;
  }
  if (cursorPos < viewStart) {
    moveView(cursorPos-viewStart);
  }
  if (cursorPos >= viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(cursorPos-viewEnd);
  }
}

unsigned short int getNoteLength(unsigned short int track, unsigned short int id){
  return seqData[track][id].getLength();
}

int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    int newEnd;
    //if it's 1 step long and being increased by subDivInt, make it subDivInt steps long instead of subDivInt+1 steps
    //(just to make editing more intuitive)
    if(seqData[track][id].endPos - seqData[track][id].startPos == 1 && val == subDivInt){
      newEnd = seqData[track][id].endPos + val - 1;
    }
    else{
      newEnd = seqData[track][id].endPos + val;
    }
    //check and see if there's a different note there
    //if there is, set the new end to be 
    for(uint16_t step = 1; step<=val; step++){
      if(lookupData[track][step+seqData[track][id].endPos] != 0 && lookupData[track][step+seqData[track][id].endPos] != id){
        //if it's the first step, just fail to save time
        if(step == 1)
          return 0;
        //if it's not the first step, set the new end to right before the other note
        else{
          newEnd = seqData[track][id].endPos+step;
          break;
        }
      }
    }
    //if the new end is before/at the start, don't do anything
    if(newEnd<=seqData[track][id].startPos)
      return 0;
      // newEnd = seqData[track][id].startPos+1;
    //if the new end is past/at the end of the seq
    if(newEnd>seqEnd){
      newEnd = seqEnd;
    }
    Note note = seqData[track][id];
    int16_t amount = newEnd-note.endPos;
    note.endPos = newEnd;
    deleteNote_byID(track, id);
    makeNote(note, track, false);
    return amount;
  }
  return 0;
}


void changeNoteLengthSelected(int amount){
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note <= seqData[track].size()-1; note++){
      if(seqData[track][note].isSelected){
        changeNoteLength(amount, track, note);
      }
    }
  }
}

int16_t changeNoteLength(int amount){
  if(selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      setCursor(bounds[0]);
    return 0;
  }
  else{
    return changeNoteLength(amount, activeTrack, lookupData[activeTrack][cursorPos]);
  }
}

//this one jumps the cursor to the end or start of the note
void changeNoteLength_jumpToEnds(int16_t amount){
  if(selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      // setCursor(bounds[0]);
      setCursor(bounds[2]-subDivInt);//testing this
  }
  else{
    //if the note was changed
    if(changeNoteLength(amount, activeTrack, lookupData[activeTrack][cursorPos]) != 0){
      //if you're shrinking the note
      if(amount<0){
        setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos-subDivInt);//testing this
        // setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].startPos);
        //if it's out of view
        // else
          // setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos+amount);
      }
      //if you're growing it
      else
        // setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos-subDivInt);
        setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos-1);//testing this

        // setCursor(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos - amount);
    }
  }
}

//gets the closest pitch of a scale
uint8_t getNearestPitch(int pitch, int scale){
  uint8_t closestPitch;
  return closestPitch;
}

//------------------------------------------------------------------------------------------------------------------------------
bool isInView(int target){
  if(target>=viewStart && target<=viewEnd)
    return true;
  else
    return false;
}

uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero){
  //down
  if(!direction){
    if(subDiv == 1 && allowZero)
      subDiv = 0;
    else if(subDiv>3)
      subDiv /= 2;
    else if(subDiv == 3)
      subDiv = 1;
  }
  else{
    if(subDiv == 0)
      subDiv = 1;
    else if(subDiv == 1)//if it's one, set it to 3
      subDiv = 3;
    else if(subDiv !=  96 && subDiv != 32){
      //if triplet mode
      if(!(subDiv%2))
        subDiv *= 2;
      else if(!(subDiv%3))
        subDiv *=2;
    }
  }
  return subDiv;
}

void changeSubDivInt(bool down){
  changeSubDivInt(down,false);
}

void changeSubDivInt(bool down, bool limitToView){
  if(down){
    if(subDivInt>3 && (!limitToView || (subDivInt*scale)>2))
      subDivInt /= 2;
    else if(subDivInt == 3)
      subDivInt = 1;
  }
  else{
    if(subDivInt == 1)//if it's one, set it to 3
      subDivInt = 3;
    else if(subDivInt !=  96 && subDivInt != 32){
      //if triplet mode
      if(!(subDivInt%2))
        subDivInt *= 2;
      else if(!(subDivInt%3))
        subDivInt *=2;
    }
  }
  menuText = "~"+stepsToMeasures(subDivInt);
}

uint16_t toggleTriplets(uint16_t subDiv){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDiv == 192){
    subDiv = 32;
  }
  else if(!(subDiv%3)){//if it's in 1/4 mode...
    subDiv = 2*subDiv/3;//set it to triplet mode
  }
  else if(!(subDiv%2)){//if it was in triplet mode...
    subDiv = 3*subDiv/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(subDivInt);
  return subDiv;
}

void toggleTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDivInt == 192){
    subDivInt = 32;
  }
  else if(!(subDivInt%3)){//if it's in 1/4 mode...
    subDivInt = 2*subDivInt/3;//set it to triplet mode
  }
  else if(!(subDivInt%2)){//if it was in triplet mode...
    subDivInt = 3*subDivInt/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(subDivInt);
}

//makes sure scale/viewend line up with the display
void checkView(){
  if(viewEnd>seqEnd){
    scale = float(96)/float(seqEnd);
    viewEnd = seqEnd+1;
  }
}
//zooms in/out
void zoom(bool in){
  uint16_t viewLength = viewEnd-viewStart;
  if(!in && viewLength<seqEnd){
    scale /= 2;
  }
  else if(in && viewLength/2>1){
    scale *= 2;
  }  
  viewStart = 0;
  viewEnd = 96/scale;
  checkView();

  changeSubDivInt(in);

  moveCursorIntoView();
  updateLEDs();

  menuText = stepsToMeasures(viewStart)+"<-->"+stepsToMeasures(viewEnd)+"(~"+stepsToMeasures(subDivInt)+")";
}
bool areThereAnyNotes(){
  for(uint8_t t = 0; t<seqData.size(); t++){
    if(seqData[t].size()>1){
      return true;
    }
  }
  return false;
}
//checks for notes above or below a track
bool areThereMoreNotes(bool above){
  if(trackData.size()>maxTracksShown){
    if(!above){
      for(int track = endTrack+1; track<trackData.size();track++){
        if(seqData[track].size()-1>0)
        return true;
      }
    }
    else if(above){
      for(int track = startTrack-1; track>=0; track--){
        if(seqData[track].size()-1>0)
        return true;
      }
    }
  }
  return false;
}
//checks to see if a channel is currently being modulated by an autotrack
bool isModulated(uint8_t ch){
  for(uint8_t dt = 0; dt<autotrackData.size(); dt++){
    if(autotrackData[dt].parameterType == 2){
      if(autotrackData[dt].channel == ch && autotrackData[dt].isActive)
        return true;
    }
  }
  return false;
}
//sort function for sorting tracks by channel
bool compareChannels(Track t1, Track t2){
  return t1.channel>t2.channel;
}

uint16_t getIDAtCursor(){
  return lookupData[activeTrack][cursorPos];
}

uint8_t getLowestVal(vector<uint8_t> vec){
  uint8_t lowest = 255;
  for(uint8_t i = 0; i<vec.size(); i++){
    if(vec[i]<lowest)
      lowest = vec[i];
  }
  return lowest;
}

bool isInVector(int val, vector<uint8_t> vec){
  if(vec.size() == 0) 
    return false;
  for(int i = 0; i<vec.size(); i++){
    if(val == vec[i]){
      return true;
    }
  }
  return false;
}

void drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch){
  display.drawBitmap(x1,y1,ch_tiny,6,3,SSD1306_WHITE);
  printSmall(x1+7,y1,stringify(ch),1);
}

//true if Stepchild is sending or receiving notes
bool isReceiving(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    if(trackData[i].noteLastSent != 255)
      return true;
  }
  return receivedNotes.notes.size();
}
bool isSending(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    if(trackData[i].noteLastSent != 255)
      return true;
  }
  return sentNotes.notes.size();
}
bool isReceivingOrSending(){
  return (isReceiving() || isSending());
}
void filterOutUnisonNotes(vector<uint8_t>& notes){
  vector<uint8_t> uniqueNotes;
  for(uint8_t i = 0; i<notes.size(); i++){
    if(!uniqueNotes.size())
      uniqueNotes.push_back(notes[i]%12);
    else{
      if(find(uniqueNotes.begin(),uniqueNotes.end(),notes[i]%12) == uniqueNotes.end()){
        uniqueNotes.push_back(notes[i]%12);
      }
    }
  }
}
void togglePlayMode(){
  playing = !playing;
  //if it's looping, set the playhead to the activeLoop start
  if(isLooping)
    playheadPos = loopData[activeLoop].start;
  else
    playheadPos = 0;
  if(playing){
    if(recording){
      toggleRecordingMode(waitForNoteBeforeRec);
    }
    #ifndef HEADLESS
    MIDI1.setHandleNoteOn(handleNoteOn_Normal);
    MIDI1.setHandleNoteOff(handleNoteOff_Normal);
    MIDI1.setHandleClock(handleClock_playing);
    MIDI1.setHandleStart(handleStart_playing);
    MIDI1.setHandleStop(handleStop_playing);
    MIDI0.setHandleNoteOn(handleNoteOn_Normal);
    MIDI0.setHandleNoteOff(handleNoteOff_Normal);
    MIDI0.setHandleClock(handleClock_playing);
    MIDI0.setHandleStart(handleStart_playing);
    MIDI0.setHandleStop(handleStop_playing);
    #endif

    startTime = micros();
    if(isArping){
      activeArp.start();
    }
    MIDI.sendStart();
  }
  else if(!playing){
    stop();
    setNormalMode();
    MIDI.sendStop();
    velModifier[1] = 0;
    chanceModifier[1] = 0;
    pitchModifier[1] = 0;
    currentCVPitch = -1;
    CVGate = false;
  }
}
void setNormalMode(){
  stop();
  if(isArping){
    activeArp.stop();
  }
  if(recordingToAutotrack){
    recordingToAutotrack = false;
    counterA = 0;
    counterB = 0;
  }
  #ifndef HEADLESS
  MIDI1.disconnectCallbackFromType(midi::Clock);
  MIDI1.disconnectCallbackFromType(midi::Start);
  MIDI1.disconnectCallbackFromType(midi::Stop);
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.disconnectCallbackFromType(midi::Clock);
  MIDI0.disconnectCallbackFromType(midi::Start);
  MIDI0.disconnectCallbackFromType(midi::Stop);
  MIDI0.disconnectCallbackFromType(midi::NoteOn);
  MIDI0.disconnectCallbackFromType(midi::NoteOff);
  MIDI0.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.setHandleNoteOn(handleNoteOn_Normal);
  MIDI0.setHandleNoteOff(handleNoteOff_Normal);
  MIDI0.setHandleStart(handleStart_Normal);
  MIDI0.setHandleStop(handleStop_Normal);

  MIDI1.setHandleNoteOn(handleNoteOn_Normal);
  MIDI1.setHandleNoteOff(handleNoteOff_Normal);
  MIDI1.setHandleStart(handleStart_Normal);
  MIDI1.setHandleStop(handleStop_Normal);

  MIDI0.setHandleControlChange(handleCC_Normal);
  MIDI1.setHandleControlChange(handleCC_Normal);
  #endif
}

void toggleRecordingMode(bool butWait){
  recording = !recording;
  //if it stopped recording
  if(!recording)
    cleanupRecording(recheadPos);
  //if it's recording to the loop
  if(recMode == ONESHOT || recMode == LOOP)
    recheadPos = loopData[activeLoop].start;
  // else
  //   recheadPos = ONESHOT;
  if(butWait)
    waitingToReceiveANote = true;
  else
    waitingToReceiveANote = false;
  if(recording){
    if(playing){
      togglePlayMode();
    }
    stop();
    #ifndef HEADLESS
    //disconnecting all the midi callbacks!
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);
    MIDI1.disconnectCallbackFromType(midi::Clock);
    MIDI1.disconnectCallbackFromType(midi::Start);
    MIDI1.disconnectCallbackFromType(midi::Stop);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
    MIDI0.disconnectCallbackFromType(midi::Clock);
    MIDI0.disconnectCallbackFromType(midi::Start);
    MIDI0.disconnectCallbackFromType(midi::Stop);

    //reconnecting the midi callbacks
    MIDI1.setHandleNoteOn(handleNoteOn_Recording);
    MIDI1.setHandleNoteOff(handleNoteOff_Recording);
    MIDI1.setHandleClock(handleClock_recording);
    MIDI1.setHandleStart(handleStart_recording);
    MIDI1.setHandleStop(handleStop_recording);
    MIDI1.setHandleControlChange(handleCC_Recording);

    MIDI0.setHandleNoteOn(handleNoteOn_Recording);
    MIDI0.setHandleNoteOff(handleNoteOff_Recording);
    MIDI0.setHandleClock(handleClock_recording);
    MIDI0.setHandleStart(handleStart_recording);
    MIDI0.setHandleStop(handleStop_recording);
    MIDI0.setHandleControlChange(handleCC_Recording);
    #endif
    startTime = micros();
  }
  else{//go back to normal mode
    setNormalMode();
  }
}

unsigned short int getLowestPitch(){
  unsigned short int pitch = trackData[activeTrack].pitch;
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].pitch<pitch)
      pitch = trackData[i].pitch;
  }
  return pitch;
}

bool itsbeen(int time){
  if(millis()-lastTime >= time)
    return true;
  else
    return false;
}

//moves the whole loop
void moveLoop(int16_t amount){
  uint16_t length = loopData[activeLoop].end-loopData[activeLoop].start;
  //if it's being moved back
  if(amount<0){
    //if amount is larger than start, meaning start would be moved before 0
    if(loopData[activeLoop].start<=amount)
      setLoopPoint(0,true);
    else
      setLoopPoint(loopData[activeLoop].start+amount,true);
    setLoopPoint(loopData[activeLoop].start+length,false);
  }
  //if it's being moved forward
  else{
    //if amount is larger than the gap between seqend and loopData[activeLoop].end
    if((seqEnd-loopData[activeLoop].end)<=amount)
      setLoopPoint(seqEnd,false);
    else
      setLoopPoint(loopData[activeLoop].end+amount,false);
    setLoopPoint(loopData[activeLoop].end - length,true);
  }
}
void toggleLoopMove(){
  switch(movingLoop){
    case 0:
      movingLoop = 1;
      moveCursor(loopData[activeLoop].start-cursorPos);
      break;
    case -1:
      movingLoop = 1;
      moveCursor(loopData[activeLoop].end-cursorPos);
      break;
    case 1:
      movingLoop = 2;
      break;
    case 2:
      movingLoop = 0;
      break;
  }
}

void yControls(){
  if(itsbeen(100)){
    if (y == 1) {
      setActiveTrack(activeTrack + 1, false);
      drawingNote = false;
      lastTime = millis();
    }
    if (y == -1) {
      setActiveTrack(activeTrack - 1, false);
      drawingNote = false;
      lastTime = millis();
    }
  }
}

//moving cursor, loop, and active track. Pass "true" to allow changing the velocity of notes
void defaultJoystickControls(bool velocityEditingAllowed){
  if (itsbeen(100)) {
    if (x == 1 && !shift) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(cursorPos%subDivInt){
        moveCursor(-cursorPos%subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-cursorPos%subDivInt);
      }
      else{
        moveCursor(-subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
    if (x == -1 && !shift) {
      if(cursorPos%subDivInt){
        moveCursor(subDivInt-cursorPos%subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt-cursorPos%subDivInt);
      }
      else{
        moveCursor(subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
  }
  if(itsbeen(100)){
    if (y == 1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack + 1, false);
      else
        setActiveTrack(activeTrack + 1, false);
      drawingNote = false;
      lastTime = millis();
    }
    if (y == -1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack - 1, false);
      else
        setActiveTrack(activeTrack - 1, false);
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (itsbeen(50)) {
    //moving
    if (x == 1 && shift) {
      moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(-1);
      else if(movingLoop == -1)
        setLoopPoint(cursorPos,true);
      else if(movingLoop == 1)
        setLoopPoint(cursorPos,false);
    }
    if (x == -1 && shift) {
      moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(1);
      else if(movingLoop == -1)
        loopData[activeLoop].start = cursorPos;
      else if(movingLoop == 1)
        loopData[activeLoop].end = cursorPos;
    }
    //changing vel
    if(velocityEditingAllowed){
      if (y == 1 && shift) {
        changeVel(-10);
        lastTime = millis();
      }
      if (y == -1 && shift) {
        changeVel(10);
        lastTime = millis();
      }

      if(getIDAtCursor()==0){
        if(y == 1 && shift){
          defaultVel-=10;
          if(defaultVel<1)
            defaultVel = 1;
          lastTime = millis();
        }
        if(y == -1 && shift){
          defaultVel+=10;
          if(defaultVel>127)
            defaultVel = 127;
          lastTime = millis();
        }
      }
    }
  }
}

//prints out each bit of the byte
void printByte(uint8_t b){
  //Serial.print("\n");
  uint8_t i = 0;
  while(i<8){
    //Serial.print(b&1);
    b = b>>1;
    i++;
  }
}

void defaultSelectBoxControls(){
  //when sel is pressed and stick is moved, and there's no selection box yet, start one
  if(sel && !selBox.begun && (x != 0 || y != 0)){
    selBox.begun = true;
    selBox.x1 = cursorPos;
    selBox.y1 = activeTrack;
  }
  //if sel is released, and there's a selection box, end it and select what was in the box
  if(!sel && selBox.begun){
    selBox.x2 = cursorPos;
    selBox.y2 = activeTrack;
    selectBox();
    selBox.begun = false;
  }
}

//default selection behavior
void defaultSelectControls(){
  if(sel && !selBox.begun){
    uint16_t id = getIDAtCursor();
    if(id == 0){
      clearSelection();
    }
    else{
      //select all
      if(n){
        selectAll();
      }
      //select only one
      else if(shift){
        clearSelection();
        toggleSelectNote(activeTrack,id, false);
      }
      //normal select
      else{
        toggleSelectNote(activeTrack, id, true);          
      }
    }
    lastTime = millis();
  }
}

void defaultLoopControls(){
  if(loop_Press){
      //if you're not moving a loop, start
      if(movingLoop == 0){
        //if you're on the start, move the start
        if(cursorPos == loopData[activeLoop].start){
          movingLoop = -1;
        }
        //if you're on the end
        else if(cursorPos == loopData[activeLoop].end){
          movingLoop = 1;
        }
        //if you're not on either, move the whole loop
        else{
          movingLoop = 2;
        }
        lastTime = millis();
      }
      //if you were moving, stop
      else{
        movingLoop = 0;
        lastTime = millis();
      }
  }
}

//default copy/paste behavior
void defaultCopyControls(){
  if(copy_Press){
    if(shift)
      paste();
    else{
      copy();
    }
    lastTime = millis();
  }
}
void mainSequencerButtons(){
  defaultSelectBoxControls();
  if(!n)
    drawingNote = false;

  //delete happens a liitle faster (so you can draw/erase fast)
  if(itsbeen(75)){
    //delete
    if(del && !shift){
      if (selectionCount > 0){
        deleteSelected();
        lastTime = millis();
      }
      else if(getIDAtCursor() != 0){
        deleteNote(activeTrack,cursorPos);
        updateLEDs();
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    //new
    if(n && !drawingNote && !sel){
      if((!shift)&& (getIDAtCursor() == 0 || cursorPos != seqData[activeTrack][getIDAtCursor()].startPos)){
        makeNote(activeTrack,cursorPos,subDivInt,true);
        moveCursor(subDivInt);
        drawingNote = true;
        lastTime = millis();
      }
      if(shift){
        addTrack(defaultPitch, defaultChannel);
        lastTime = millis();
      }
    }
    defaultSelectControls();
    defaultCopyControls();
    if(!shift){
      defaultLoopControls();
    }
    //special case for the main sequence; loop+shift jumps into the loop menu
    else if(loop_Press){
      isLooping = !isLooping;
      lastTime = millis();
      menuText = isLooping?"loop on":"loop off";
    }

    if(del && shift){
      muteNote(activeTrack, getIDAtCursor(), true);
      if(selectionCount>0){
        muteSelectedNotes();
      }
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(play && !shift && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+shift, or if play and it's already recording
    if(play && shift || play && recording){
      toggleRecordingMode(waitForNoteBeforeRec);
      lastTime = millis();
    }
 

    //menu press
    if(menu_Press){
      if(shift){
        lastTime = millis();
        return;
      }
      else{
        lastTime = millis();
        menuIsActive = true;
        constructMenu("MENU");
        mainMenu();
        return;
      }
    }
    if(track_Press){
      menuIsActive = true;
      lastTime = millis();
      track_Press = false;
      constructMenu("TRK");
    }
    if(note_Press){
      lastTime = millis();
      note_Press = false;
      menuIsActive = true;
      constructMenu("EDIT");
    }
  }
}

//sets all the buttons to 0 (useful for making the animations transition nicely)
void clearButtons(){
  for(int i = 0; i < 8; i++){
    step_buttons[i] = false;
  }
  counterA = 0;
  counterB = 0;
  x = false;
  y = false;
  joy_Press = false;
  n = false;
  sel = false;
  shift = false;
  del = false;
  play = false;
  track_Press = false;
  note_Press = false;
  loop_Press = false;
  copy_Press = false;
  menu_Press = false;
}

#ifndef HEADLESS
//checks all inputs
bool anyActiveInputs(){
  //normal buttons
  digitalWrite(buttons_load,LOW);
  digitalWrite(buttons_load,HIGH);
  digitalWrite(buttons_clockIn, HIGH);
  digitalWrite(buttons_clockEnable,LOW);
  unsigned char bits_buttons = shiftIn(buttons_dataIn, buttons_clockIn, LSBFIRST);
  digitalWrite(buttons_clockEnable, HIGH);

  for(int digit = 0; digit<8; digit++){
    if(!((bits_buttons>>digit)&1))//if any of these are high, return true
      return true;
  }

  //stepButtons
  if(LEDsOn){
    digitalWrite(buttons_load,LOW);
    digitalWrite(buttons_load,HIGH);
    digitalWrite(buttons_clockIn, HIGH);
    digitalWrite(buttons_clockEnable,LOW);
    unsigned char bits_stepButtons = shiftIn(stepButtons_dataIn, buttons_clockIn, LSBFIRST);
    digitalWrite(buttons_clockEnable, HIGH);
  
    for(int digit = 0; digit<8; digit++){
      if(!((bits_stepButtons>>digit)&1))
        return true;
    }
  }

  readJoystick();
  if(x || y)
    return true;
  //encoder presses
  if(digitalRead(encoderA_Button) || digitalRead(encoderB_Button)){
    return true;
  }
  //encoder clicks
  if(counterA || counterB){
    counterA = 0;
    counterB = 0;
    return true;
  }

  return false;
}
#else
bool anyActiveInputs(){
  glfwPollEvents();
  if(x || y || n || shift || sel || del || loop_Press || play || copy_Press || menu_Press || counterA || counterB || note_Press || track_Press)
    return true;
  else
    return false;
}
#endif

//CHECK
//true if pitch is being sent or received
bool isBeingPlayed(uint8_t pitch){
  if(sentNotes.containsPitch(pitch) || receivedNotes.containsPitch(pitch)){
    return true;
  }
  return false;
}

void stepButtons(){
  if(itsbeen(200)){
    //DJ loop selector
    if(shift){
      for(uint8_t i = 0; i<loopData.size(); i++){
        if(step_buttons[i]){
          setActiveLoop(i);
          lastTime = millis();
          break;
        }
      }
    }
    else{
      bool atLeastOne = false;
      //if it's in 1/4 mode
      if(!(subDivInt%3)){
        for(int i = 0; i<8; i++){
          if(step_buttons[i]){
            uint16_t viewLength = viewEnd-viewStart;
            toggleNote(activeTrack, viewStart+i*viewLength/8, viewLength/8);
            atLeastOne = true;
          }
        }
      }
      //if it's in 1/3 mode, last two buttons do nothing
      else if(!(subDivInt%2)){
        for(int i = 0; i<6; i++){
          if(step_buttons[i]){
            uint16_t viewLength = viewEnd-viewStart;
            toggleNote(activeTrack,viewStart+i*viewLength/6,viewLength/6);
            atLeastOne = true;
          }
        }
      }
      if(atLeastOne){
        lastTime = millis();
      }
    }
  }
}

void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}

void defaultEncoderControls(){
  while(counterA != 0){
    //changing zoom
    if(counterA >= 1){
      zoom(true);
    }
    if(counterA <= -1){
      zoom(false);
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(shift){
      toggleTriplets();
      break;
    }
    else if(counterB >= 1){
      changeSubDivInt(true);
    }
    //changing subdivint
    else if(counterB <= -1){
      changeSubDivInt(false);
    }
    counterB += counterB<0?1:-1;;
  }
}

void mainSequencerEncoders(){
  while(counterA != 0){
    //changing pitch
    if(shift){
      if(counterA >= 1){
        setTrackPitch(activeTrack,trackData[activeTrack].pitch+1,true);
      }
      if(counterA <= -1){
        setTrackPitch(activeTrack,trackData[activeTrack].pitch-1,true);
      }
    }
    //changing zoom
    else{
      if(counterA >= 1 && !track_Press){
        zoom(true);
      }
      if(counterA <= -1 && !track_Press){
        zoom(false);
      }
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    if(shift){
      toggleTriplets();
    }
    else if(counterB >= 1){
      changeSubDivInt(true,true);
    }
    //changing subdivint
    else if(counterB <= -1){
      changeSubDivInt(false,true);
    }
    counterB += counterB<0?1:-1;;
  }
}

String enterText(String title){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  alphabet = alpha1;
  while(!done){
    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(shift){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }

      if(del && text.length()>0){
        del = false;
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(sel){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<10){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        lastTime = millis();
      }
      if(menu_Press){
        text = "";
        done = true;
        lastTime = millis();
      }
    }
    if(itsbeen(100)){
      if(x == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(x == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(y == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(y == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    //title
    display.clearDisplay();
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();

    //text tooltip
    printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      display.print(text+alphabet[highlight]);
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,15,10,SSD1306_WHITE);
      }
    }
    else
      display.print(text);
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // display.print(alphabet[count]);
              // display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              display.print(alphabet[count]);
              display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    display.display();
  }
  if(text == ""){
    text = "default";
  }
  return text;
}

//this should move the note the cursor is on (if any)
bool moveNotes(int xAmount, int yAmount){
  if(selectionCount == 0){
    if(lookupData[activeTrack][cursorPos] != 0){
      bool worked = moveNote(lookupData[activeTrack][cursorPos],activeTrack,activeTrack+yAmount,seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos+xAmount);
      return worked;
    }
    else
      return false;
  }
  else{
    bool worked = moveSelectedNotes(xAmount,yAmount);
    return worked;
  }
}

void sequenceLEDs(){
  int length = loopData[activeLoop].end-loopData[activeLoop].start;
  int val = playheadPos/(length/8);
  bool leds[8] = {0,0,0,0,0,0,0,0};
  leds[val] = true;
  writeLEDs(leds);
}

//pulses the onboard LED
void ledPulse(uint8_t speed){
  //use abs() so that it counts DOWN when millis() overflows into the negatives
  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
  analogWrite(onboard_ledPin,4*abs(int8_t(millis()/speed)));
}

void loop() {
  readJoystick();
  readButtons();

  mainSequencerButtons();
  stepButtons();
  mainSequencerEncoders();
  defaultJoystickControls(true);

  displaySeq();
}

//the closer the step is to the subDiv (both forward and backward), the shorter the time val
unsigned long getTimeValue(unsigned short int step){
  step%=swingSubDiv;
  //if it's closer to the next subDiv, move it forward
  if(step>=swingSubDiv/2){
  }
  //if it's closer to the last subDiv, move it backward
  else if(step<swingSubDiv/2){
  }
  if(swingVal != 0){
    return MicroSperTimeStep;
  }
  else
    return MicroSperTimeStep;
}

//returns the offset in microseconds at a given step
float swingOffset(unsigned short int step){
  return swingVal*sin(2*PI/swingSubDiv * (step-swingSubDiv/4));
}

//this is a sloppy lil function that returns true if the time is within (x) of the subDiv
bool onBeat(int subDiv, int fudge){
  long msPerB = (240000/(bpm*subDiv));
  long offVal = millis()%msPerB;
  //if it's within fudge of this beat or the next
  if(offVal<=fudge || offVal>= msPerB - fudge ){
    return true;
  }
  else 
    return false;
}

//old,working one without floats (uses %)
bool hasItBeenEnoughTime_swing(){
  timeElapsed = micros()-timeLastStepPlayed;
  if(timeElapsed >= MicroSperTimeStep+swingOffset(playheadPos)){
    if(!(playheadPos%swingSubDiv)){//if it's a multiple of the swing subDiv, it should be perfectly on time, so grab the offset from here
     offBy = (micros()-startTime)%(MicroSperTimeStep);
    }
    // //Serial.println(timeElapsed);
    if(offBy == 0)
      startTime = micros();
    timeLastStepPlayed = micros();
    return true;
  }
  else
    return false;
}

//old,working one without floats (uses %)
bool hasItBeenEnoughTime(){
  if(swung){
    return hasItBeenEnoughTime_swing();
  }
  else{
    timeElapsed = micros()-timeLastStepPlayed;
    if(timeElapsed + offBy >= MicroSperTimeStep){
      offBy = (micros()-startTime)%MicroSperTimeStep;
      if(offBy == 0)
        startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    }
    else
      return false;
  }
}
bool hasItBeenEnoughTime_cycleCount(){
  timeElapsed = rp2040.getCycleCount()-timeLastStepPlayed;
  if(timeElapsed + offBy >= MicroSperTimeStep){
    offBy = (micros()-startTime)%MicroSperTimeStep;
    timeLastStepPlayed = rp2040.getCycleCount();
    return true;
  }
  else
    return false;
}
//this one is for the clockMenu
bool hasItBeenEnoughTime_clock(int timeStep){
  timeElapsed = micros()-timeLastStepPlayed;
  if(swung){
    if(timeElapsed >= MicroSperTimeStep+swingOffset(timeStep)){
      timeLastStepPlayed = micros();
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(timeElapsed >= MicroSperTimeStep){
      timeLastStepPlayed = micros();
      return true;
    }
    else{
      return false;
    }
  }
}

//runs while "playing" is true
void playingLoop(){
  //internal timing
  if(clockSource == INTERNAL){
    if(hasItBeenEnoughTime()){
      MIDI.sendClock();
      playStep(playheadPos);
      playheadPos++;
      checkLoop();
      // checkFragment();
      if(!(playheadPos%12)){
        pramOffset = !pramOffset;
      }
    }
  }
  //external timing
  else if(clockSource == EXTERNAL){
    MIDI.read();
    if(gotClock && hasStarted){
      gotClock = false;
      playStep(playheadPos);
      playheadPos += 1;
      checkLoop();
      // checkFragment();
    }
  }
}

//this records CC to the activeDT (when you're in the )
void checkAutotracks(){
  if(recordingToAutotrack){
    int newVal = 64;
    switch(autotrackData[activeAutotrack].recordFrom){
      //recording externally, so get outta this loop!
      case 0:
        return;
      //rec from encoder A
      case 1:
        if(counterA>127){
          counterA = 127;
        }
        if(counterA<0)
          counterA = 0;
        newVal = counterA;
        break;
      //rec from encoder B
      case 2:
        if(counterB>127)
          counterB = 127;
        if(counterB<0)
          counterB = 0;
        newVal = counterB;
        break;
      //rec from joystick X
      case 3:
        newVal = getJoyX();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
      //rec from joystick Y
      case 4:
        newVal = getJoyY();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
    }
    //bounds checking the new value before we write it to the DT
    if(newVal>127)
      newVal = 127;
    else if(newVal<0)
      newVal = 0;
    if(waitingToReceiveANote){
      return;
    }
    recentCC.val = newVal;
    recentCC.cc = autotrackData[activeAutotrack].control;
    recentCC.channel = autotrackData[activeAutotrack].channel;
    autotrackData[activeAutotrack].data[recheadPos] = newVal;
  }
}

void defaultLoop(){
  playheadPos = loopData[activeLoop].start;
  recheadPos = loopData[activeLoop].start;
  // fragmentStep = 0;
  MIDI.read();
}

void arpLoop(){
  //if it was active, but hadn't started playing yet
  if(!activeArp.playing){
    switch(activeArp.source){
      case 0:
        if(receivedNotes.notes.size()>0)
          activeArp.start();
        break;
      case 1:
        if(sentNotes.notes.size()>0)
          activeArp.start();
        break;
    }
  }
  if(activeArp.playing){
    //if the arp isn't latched and there are no notes for it
    if(!activeArp.holding  && ((activeArp.source == 0 && !receivedNotes.notes.size()) || (activeArp.source == 1 && !sentNotes.notes.size()))){
      activeArp.stop();
    }
    //if it IS latched or there are notes for it, then continue
    else if(activeArp.hasItBeenEnoughTime()){
      activeArp.playstep();
    }
  }
}

//this cpu handles time-sensitive things
void loop1(){
  //play mode
  if(playing){
    playingLoop();
  }
  //record mode
  else if(recording){
    recordingLoop();
  }
  //default state
  else{
    defaultLoop();
  }
  if(isArping){
    arpLoop();
  }
  ledPulse(16);
}
