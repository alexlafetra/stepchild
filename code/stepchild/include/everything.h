//for no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //bool for breaking from the loop
  bool notChosenYet = true;
  //storing the state
  bool state = false;
  lastTime = millis();
  
  while(true){
    display.clearDisplay();
    drawingFunction();
    graphics.drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      //x to select option
      if(controls.joystickX != 0){
        if(controls.joystickX == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(controls.joystickX == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/del to cancel
      if(controls.MENU() || controls.DELETE()){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(controls.NEW() || controls.SELECT() ){
        lastTime = millis();
          controls.setNEW(false);
        controls.setSELECT(false);
        return state;
      }
    }
  }
  return false;
}
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0 || controls.joystickY != 0){
        if(controls.joystickX == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(controls.joystickX == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(controls.SELECT() ){
        selected = true;
        time = millis();
      }
    }
  }
  controls.setSELECT(false);
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}

uint16_t getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<sequence.noteData.size(); track++){
    count+=sequence.noteData[track].size()-1;
  }
  return count;
}

float getNoteDensity(int timestep){
  float density = 0;
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(sequence.lookupTable[track][timestep] != 0){
      density++;
    }
  }
  return density/float(sequence.trackData.size());
}
float getNoteDensity(int start, int end){
  float density;
  for(int i = start; i<= end; i++){
    density+=getNoteDensity(i);
  }
  return density/float(end-start+1);
}

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      globalModifiers.velocity[0] = channel;
      globalModifiers.velocity[1] = val-63;
      break;
    //chance
    case 1:
      globalModifiers.chance[0] = channel;
      globalModifiers.chance[1] = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      globalModifiers.pitch[0] = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      globalModifiers.pitch[1] = float(val)/float(127) * 48 - 24;
      break;
    //sequenceClock.BPM
    case 3:
      sequenceClock.setBPM(float(val)*2);
      break;
    //swing amount
    case 4:
      sequenceClock.swingAmplitude += val-63;
    //track channel
    case 5:
      break;
  }
}

// //gets a note from data, returns a note object
// Note getNote(int track, int timestep) {
//   unsigned short int noteID;
//   if (sequence.lookupTable[track][timestep] != 0) {
//     noteID = sequence.lookupTable[track][timestep];
//     Note targetNote = sequence.noteData[track][noteID];
//     return targetNote;
//   }
//   else {
//     return Note();
//   }
// }

//counts notes
uint16_t countNotesInRange(uint16_t start, uint16_t end){
  uint16_t count = 0;
  for(uint8_t t = 0; t<sequence.trackData.size(); t++){
    //if there are no notes, ignore it
    if(sequence.noteData[t].size() == 1)
      continue;
    else{
      //move over each note
      for(uint16_t i = 1; i<sequence.noteData.size(); i++){
        if(sequence.noteData[t][i].startPos>=start && sequence.noteData[t][i].startPos<end)
          count++;
      }
    }
  }
  return count;
}

//changes which track is active, changing only to valid tracks
void setActiveTrack(uint8_t newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < sequence.trackData.size()) {
    if(sequence.activeTrack == 4 && newActiveTrack == 5 && maxTracksShown == 5){
      maxTracksShown = 6;
    }
    else if(sequence.activeTrack == 1 && newActiveTrack == 0 && maxTracksShown == 6){
      maxTracksShown = 5;
    }
    sequence.activeTrack = newActiveTrack;
    if (loudly) {
      MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
      MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, 0, sequence.trackData[sequence.activeTrack].channel);
      if(sequence.trackData[sequence.activeTrack].isLatched){
        MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
        MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, 0, sequence.trackData[sequence.activeTrack].channel);
      }
    }
  }
  menuText = pitchToString(sequence.trackData[sequence.activeTrack].pitch,true,true);
}

void changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    sequence.trackData[id].channel = newChannel;
  }
}

void changeAllTrackChannels(int newChannel){
  for(int track = 0; track<sequence.trackData.size(); track++){
    changeTrackChannel(track, newChannel);
  }
}

void moveToNextNote_inTrack(bool up){
  uint8_t track = sequence.activeTrack;
  uint16_t currentID = sequence.IDAtCursor();
  bool foundTrack = false;
  //moving the track up/down until it hits a track with notes
  //and checking bounds
  if(up){
    while(track<sequence.trackData.size()-1){
      track++;
      if(sequence.noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  else{
    while(track>0){
      track--;
      if(sequence.noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  //if you couldn't find a track with a note on it, just return
  if(!foundTrack){
    return;
  }
  for(uint16_t dist = 0; dist<sequence.sequenceLength; dist++){
    bool stillValid = false;
    //if the new position is in bounds
    if(sequence.cursorPos+dist<=sequence.sequenceLength){
      stillValid = true;
      //and if there's something there!
      if(sequence.lookupTable[track][sequence.cursorPos+dist] != 0){
        //move to it
        moveCursor(dist);
        setActiveTrack(track,false);
        return;
      }
    }
    if(sequence.cursorPos>=dist){
      stillValid = true;
      if(sequence.lookupTable[track][sequence.cursorPos-dist] != 0){
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
  if(sequence.noteData[sequence.activeTrack].size()>1){
    unsigned short int id = sequence.IDAtCursor();
    if(forward){
      for(int i = sequence.cursorPos; i<sequence.sequenceLength; i++){
        if(sequence.lookupTable[sequence.activeTrack][i] !=id && sequence.lookupTable[sequence.activeTrack][i] != 0){
          moveCursor(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][i]].startPos-sequence.cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(sequence.sequenceLength-sequence.cursorPos);
      }
      return;
    }
    else{
      for(int i = sequence.cursorPos; i>0; i--){
        if(sequence.lookupTable[sequence.activeTrack][i] !=id && sequence.lookupTable[sequence.activeTrack][i] != 0){
          moveCursor(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][i]].startPos-sequence.cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(-sequence.cursorPos);
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
// void moveView(int16_t val) {
//   int oldViewLength = sequence.viewEnd-sequence.viewStart;
//   if((sequence.viewStart+val)<0){
//     sequence.viewStart = 0;
//     sequence.viewEnd = sequence.viewStart+oldViewLength;
//   }
//   if(sequence.viewEnd+val>sequence.sequenceLength){
//     sequence.viewEnd = sequence.sequenceLength;
//     sequence.viewStart = sequence.viewEnd - oldViewLength;
//   }
//   if(sequence.viewEnd+val<=sequence.sequenceLength && sequence.viewStart+val>=0){
//     sequence.viewStart += val;
//     sequence.viewEnd += val;
//   }
// }
void setViewStart(uint16_t step){
  uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
  if(viewLength + step > sequence.sequenceLength){
    step = sequence.sequenceLength-viewLength;
  }
  sequence.viewStart = step;
  sequence.viewEnd = step+viewLength;
}

void moveView(int16_t val){
  if(val < 0 && abs(val)>sequence.viewStart){
    setViewStart(0);
  }
  else{
    setViewStart(sequence.viewStart+val);
  }
}

//moving the cursor around
int16_t moveCursor(int moveAmount){
  int16_t amt;
  //if you're trying to move back at the start
  if(sequence.cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && sequence.cursorPos+moveAmount<0){
    amt = sequence.cursorPos;
    sequence.cursorPos = 0;
  }
  else{
    sequence.cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(sequence.cursorPos > sequence.sequenceLength) {
    amt += (sequence.sequenceLength-sequence.cursorPos);
    sequence.cursorPos = sequence.sequenceLength;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos<sequence.cursorPos)
      changeNoteLength(sequence.cursorPos-sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos,sequence.activeTrack,sequence.noteData[sequence.activeTrack].size()-1);
  }
  //Move the view along with the cursor
  if(sequence.cursorPos<sequence.viewStart+sequence.subDivision && sequence.viewStart>0){
    moveView(sequence.cursorPos - (sequence.viewStart+sequence.subDivision));
  }
  else if(sequence.cursorPos > sequence.viewEnd-sequence.subDivision && sequence.viewEnd<sequence.sequenceLength){
    moveView(sequence.cursorPos - (sequence.viewEnd-sequence.subDivision));
  }
  //update the LEDs
  menuText = ((moveAmount>0)?(stepsToPosition(sequence.cursorPos,true)+">>"):("<<"+stepsToPosition(sequence.cursorPos,true)));
  return amt;
}

void setCursor(uint16_t loc){
  moveCursor(loc-sequence.cursorPos);
}

void moveCursorIntoView(){
  if (sequence.cursorPos < 0) {
    sequence.cursorPos = 0;
  }
  if (sequence.cursorPos > sequence.sequenceLength-1) {
    sequence.cursorPos = sequence.sequenceLength-1;
  }
  if (sequence.cursorPos < sequence.viewStart) {
    moveView(sequence.cursorPos-sequence.viewStart);
  }
  if (sequence.cursorPos >= sequence.viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(sequence.cursorPos-sequence.viewEnd);
  }
}

//------------------------------------------------------------------------------------------------------------------------------
bool isInView(int target){
  if(target>=sequence.viewStart && target<=sequence.viewEnd)
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
    if(sequence.subDivision>3 && (!limitToView || (sequence.subDivision*sequence.viewScale)>2))
      sequence.subDivision /= 2;
    else if(sequence.subDivision == 3)
      sequence.subDivision = 1;
  }
  else{
    if(sequence.subDivision == 1)//if it's one, set it to 3
      sequence.subDivision = 3;
    else if(sequence.subDivision !=  96 && sequence.subDivision != 32){
      //if triplet mode
      if(!(sequence.subDivision%2))
        sequence.subDivision *= 2;
      else if(!(sequence.subDivision%3))
        sequence.subDivision *=2;
    }
  }
  menuText = "~"+stepsToMeasures(sequence.subDivision);
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
  menuText = stepsToMeasures(sequence.subDivision);
  return subDiv;
}

void toggleTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(sequence.subDivision == 192){
    sequence.subDivision = 32;
  }
  else if(!(sequence.subDivision%3)){//if it's in 1/4 mode...
    sequence.subDivision = 2*sequence.subDivision/3;//set it to triplet mode
  }
  else if(!(sequence.subDivision%2)){//if it was in triplet mode...
    sequence.subDivision = 3*sequence.subDivision/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(sequence.subDivision);
}

//makes sure scale/viewend line up with the display
void checkView(){
  if(sequence.viewEnd>sequence.sequenceLength){
    sequence.viewScale = float(96)/float(sequence.sequenceLength);
    sequence.viewEnd = sequence.sequenceLength+1;
  }
}
//zooms in/out
void zoom(bool in){
  uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
  if(!in && viewLength<sequence.sequenceLength){
    sequence.viewScale /= 2;
  }
  else if(in && viewLength/2>1){
    sequence.viewScale *= 2;
  }  
  sequence.viewStart = 0;
  sequence.viewEnd = 96/sequence.viewScale;
  checkView();
  changeSubDivInt(in);
  moveCursorIntoView();
  menuText = stepsToMeasures(sequence.viewStart)+"<-->"+stepsToMeasures(sequence.viewEnd)+"(~"+stepsToMeasures(sequence.subDivision)+")";
}
bool areThereAnyNotes(){
  for(uint8_t t = 0; t<sequence.noteData.size(); t++){
    if(sequence.noteData[t].size()>1){
      return true;
    }
  }
  return false;
}
//checks for notes above or below a track
bool areThereMoreNotes(bool above){
  if(sequence.trackData.size()>maxTracksShown){
    if(!above){
      for(int track = endTrack+1; track<sequence.trackData.size();track++){
        if(sequence.noteData[track].size()-1>0)
        return true;
      }
    }
    else if(above){
      for(int track = startTrack-1; track>=0; track--){
        if(sequence.noteData[track].size()-1>0)
        return true;
      }
    }
  }
  return false;
}
//checks to see if a channel is currently being modulated by an autotrack
bool isModulated(uint8_t ch){
  for(uint8_t dt = 0; dt<sequence.autotrackData.size(); dt++){
    if(sequence.autotrackData[dt].parameterType == 2){
      if(sequence.autotrackData[dt].channel == ch && sequence.autotrackData[dt].isActive)
        return true;
    }
  }
  return false;
}
//sort function for sorting tracks by channel
bool compareChannels(Track t1, Track t2){
  return t1.channel>t2.channel;
}

void togglePlayMode(){
  playing = !playing;
  //if it's looping, set the playhead to the sequence.activeLoop start
  if(sequence.isLooping)
    playheadPos = sequence.loopData[sequence.activeLoop].start;
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

    sequenceClock.startTime = micros();
    if(arp.isActive){
      arp.start();
    }
    MIDI.sendStart();
  }
  else if(!playing){
    stop();
    setNormalMode();
    MIDI.sendStop();
    globalModifiers.velocity[1] = 0;
    globalModifiers.chance[1] = 0;
    globalModifiers.pitch[1] = 0;
    CV.off();

  }
}
void setNormalMode(){
  stop();
  if(arp.isActive){
    arp.stop();
  }
  if(recordingToAutotrack){
    recordingToAutotrack = false;
    controls.counterA = 0;
    controls.counterB = 0;
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
  if(recMode == ONESHOT || recMode == LOOP_MODE)
    recheadPos = sequence.loopData[sequence.activeLoop].start;
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
    sequenceClock.startTime = micros();
  }
  else{//go back to normal mode
    setNormalMode();
  }
}

//runs while "playing" is true
void playingLoop(){
  //internal timing
  if(clockSource == INTERNAL_CLOCK){
    if(sequenceClock.hasItBeenEnoughTime(playheadPos)){
      MIDI.sendClock();
      playStep(playheadPos);
      playheadPos++;
      checkLoop();
    }
  }
  //external timing
  else if(clockSource == EXTERNAL_CLOCK){
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
    switch(sequence.autotrackData[sequence.activeAutotrack].recordFrom){
      //recording externally, so get outta this loop!
      case 0:
        return;
      //rec from encoder A
      case 1:
        if(controls.counterA>127){
          controls.counterA = 127;
        }
        if(controls.counterA<0)
          controls.counterA = 0;
        newVal = controls.counterA;
        break;
      //rec from encoder B
      case 2:
        if(controls.counterB>127)
          controls.counterB = 127;
        if(controls.counterB<0)
          controls.counterB = 0;
        newVal = controls.counterB;
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
    recentCC.cc = sequence.autotrackData[sequence.activeAutotrack].control;
    recentCC.channel = sequence.autotrackData[sequence.activeAutotrack].channel;
    sequence.autotrackData[sequence.activeAutotrack].data[recheadPos] = newVal;
  }
}

void defaultLoop(){
  playheadPos = sequence.loopData[sequence.activeLoop].start;
  recheadPos = sequence.loopData[sequence.activeLoop].start;
  // fragmentStep = 0;
  MIDI.read();
}

void arpLoop(){
  //if it was active, but hadn't started playing yet
  if(!arp.playing){
    switch(arp.source){
      case 0:
        if(receivedNotes.notes.size()>0)
          arp.start();
        break;
      case 1:
        if(sentNotes.notes.size()>0)
          arp.start();
        break;
    }
  }
  if(arp.playing){
    //if the arp isn't latched and there are no notes for it
    if(!arp.holding  && ((arp.source == 0 && !receivedNotes.notes.size()) || (arp.source == 1 && !sentNotes.notes.size()))){
      arp.stop();
    }
    //if it IS latched or there are notes for it, then continue
    else if(arp.hasItBeenEnoughTime()){
      arp.playstep();
    }
  }
}
