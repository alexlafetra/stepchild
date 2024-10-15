void yControls(){
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1) {
      sequence.setActiveTrack(sequence.activeTrack + 1, !sequence.playing());
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1) {
      sequence.setActiveTrack(sequence.activeTrack - 1, !sequence.playing());
      drawingNote = false;
      lastTime = millis();
    }
  }
}

//moving cursor, loop, and active track. Pass "true" to allow changing the velocity of notes
void defaultJoystickControls(bool velocityEditingAllowed){
  if (utils.itsbeen(100)) {
    if (controls.joystickX == 1 && !controls.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(sequence.cursorPos%sequence.subDivision){
        sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          sequence.moveLoop(-sequence.cursorPos%sequence.subDivision);
      }
      else{
        sequence.moveCursor(-sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          sequence.moveLoop(-sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
    if (controls.joystickX == -1 && !controls.SHIFT()) {
      if(sequence.cursorPos%sequence.subDivision){
        sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          sequence.moveLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
      }
      else{
        sequence.moveCursor(sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          sequence.moveLoop(sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1 && !controls.SHIFT() && !controls.LOOP()) {
      sequence.setActiveTrack(sequence.activeTrack + 1, !sequence.playing()  && !sequence.recording());
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1 && !controls.SHIFT() && !controls.LOOP()) {
      sequence.setActiveTrack(sequence.activeTrack - 1, !sequence.playing()  && !sequence.recording());
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (utils.itsbeen(50)) {
    //moving
    if (controls.joystickX == 1 && controls.SHIFT()) {
      sequence.moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        sequence.moveLoop(-1);
      else if(movingLoop == -1)
        sequence.setLoopPoint(sequence.cursorPos,true);
      else if(movingLoop == 1)
        sequence.setLoopPoint(sequence.cursorPos,false);
    }
    if (controls.joystickX == -1 && controls.SHIFT()) {
      sequence.moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        sequence.moveLoop(1);
      else if(movingLoop == -1)
        sequence.loopData[sequence.activeLoop].start = sequence.cursorPos;
      else if(movingLoop == 1)
        sequence.loopData[sequence.activeLoop].end = sequence.cursorPos;
    }
    //changing vel
    if(velocityEditingAllowed){
      if(sequence.IDAtCursor()){
        if (controls.joystickY == 1 && controls.SHIFT()) {
          sequence.changeVel(-10);
          lastTime = millis();
        }
        if (controls.joystickY == -1 && controls.SHIFT()) {
          sequence.changeVel(10);
          lastTime = millis();
        }
      }
      else{
        if(controls.joystickY == 1 && controls.SHIFT()){
          sequence.defaultVel-=10;
          if(sequence.defaultVel<1)
            sequence.defaultVel = 1;
          lastTime = millis();
        }
        if(controls.joystickY == -1 && controls.SHIFT()){
          sequence.defaultVel+=10;
          if(sequence.defaultVel>127)
            sequence.defaultVel = 127;
          lastTime = millis();
        }
      }
    }
  }
}
void defaultSelectBoxControls(){
  //when controls.SELECT()  is pressed and stick is moved, and there's no selection box yet, start one
  if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
    selBox.begun = true;
    selBox.coords.start.x = sequence.cursorPos;
    selBox.coords.start.y = sequence.activeTrack;
  }
  //if controls.SELECT()  is released, and there's a selection box, end it and select what was in the box
  if(!controls.SELECT()  && selBox.begun){
    selBox.coords.end.x = sequence.cursorPos;
    selBox.coords.end.y = sequence.activeTrack;
    selectBox();
    selBox.begun = false;
  }
}
//default selection behavior
void defaultSelectControls(){
  if(controls.SELECT()  && !selBox.begun){
    uint16_t id = sequence.IDAtCursor();
    if(id == 0){
      clearSelection();
    }
    else{
      //select all
      if(controls.NEW()){
        selectAll();
      }
      //select only one
      else if(controls.SHIFT()){
        clearSelection();
        toggleSelectNote(sequence.activeTrack,id, false);
      }
      //normal select
      else{
        toggleSelectNote(sequence.activeTrack, id, true);          
      }
    }
    lastTime = millis();
  }
}

void defaultLoopControls(){
  if(controls.LOOP()){
      //if you're not moving a loop, start
      if(movingLoop == StepchildSequence::NONE){
        //if you're on the start, move the start
        if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].start){
          movingLoop = StepchildSequence::START;
          menuText = "Moving Loop Start";
        }
        //if you're on the end
        else if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].end){
          movingLoop = StepchildSequence::END;
          menuText = "Moving Loop End";
        }
        //if you're not on either, move the whole loop
        else{
          movingLoop = StepchildSequence::BOTH;
          menuText = "Moving Loop";
        }
        lastTime = millis();
      }
      //if you were moving, stop
      else{
        movingLoop = StepchildSequence::NONE;
        lastTime = millis();
      }
  }
}
//default copy/paste behavior
void defaultCopyControls(){
  if(controls.COPY()){
    if(controls.SHIFT())
      clipboard.paste();
    else{
      clipboard.copy();
    }
    lastTime = millis();
  }
}
void mainSequencerButtons(){
  defaultSelectBoxControls();
  if(!controls.NEW())
    drawingNote = false;

  //del happens a liitle faster (so you can draw/erase fast)
  if(utils.itsbeen(75)){
    //del
    if(controls.DELETE() && !controls.SHIFT()){
      if (sequence.selectionCount > 0){
        sequence.deleteSelected();
        lastTime = millis();
      }
      else if(sequence.IDAtCursor() != 0){
        sequence.deleteNote(sequence.activeTrack,sequence.cursorPos);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //new
    if(controls.NEW() && !drawingNote && !controls.SELECT() ){
      if(controls.SHIFT()){
        //if there's no note starting here, make a new note 1/2 the subdiv
        if(!sequence.IDAtCursor()){
          sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision/2,true);
          sequence.moveCursor(sequence.subDivision/2);
          drawingNote = true;
          lastTime = millis();
        }
        //if there is a note here, cut it in half/thirds (depending on if you're in triplet mode or not)
        else{
          sequence.chopNote(sequence.activeTrack,sequence.cursorPos,sequence.isQuarterGrid()?2:3);
          lastTime = millis();
        }
      }
      else if(!controls.SHIFT()){
        if((sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteAtCursor().startPos)){
          sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision,true);
          sequence.moveCursor(sequence.subDivision);
          drawingNote = true;
          lastTime = millis();
        }
        //if you are on the start pos of a note
        else{
          lastTime = millis();
          setSuperposition(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][sequence.cursorPos]],sequence.activeTrack);
        }
      }
    }
    defaultSelectControls();
    defaultCopyControls();
    if(!controls.SHIFT()){
      defaultLoopControls();
    }
    //shift+loop pushes the loop to to a new loop
    else if(controls.LOOP()){
      lastTime = millis();
      pushToNewLoop();
    }

    if(controls.DELETE() && controls.SHIFT()){
        sequence.muteNote(sequence.activeTrack, sequence.IDAtCursor(), true);
      if(sequence.selectionCount>0){
        sequence.muteSelectedNotes();
      }
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(controls.PLAY() && !controls.SHIFT() && !sequence.recording()){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+controls.SHIFT(), or if play and it's already sequence.recording()
    if((controls.PLAY() && controls.SHIFT()) || (controls.PLAY() && sequence.recording())){
      toggleRecordingMode(waitForNoteBeforeRec);
      lastTime = millis();
    }
    //menu press
    if(controls.MENU()){
      lastTime = millis();
      mainMenu();
      return;
    }
    if(controls.A()){
      lastTime = millis();
      trackMenu();
    }
    if(controls.B()){
      if(controls.SHIFT() && sequence.IDAtCursor()){
        sequence.setCursor(sequence.noteAtCursor().startPos);
      }
      else{
        lastTime = millis();
        editMenu();
      }
    }
  }
}

void mainSequencerStepButtons(){
  //don't need to time debounce the step buttons!
  //DJ loop selector
  if(controls.SHIFT()){
    if(controls.stepButton(15) && sequence.activeTrack){
      sequence.setActiveTrack(sequence.activeTrack-1,!sequence.playing()  && !sequence.recording());
      lastTime = millis();
    }
    else if(controls.stepButton(14)){
      sequence.setActiveTrack(sequence.activeTrack+1,!sequence.playing()  && !sequence.recording());
      lastTime = millis();
    }
    else if(controls.stepButton(0)){
      sequence.makeNoteEveryNDivisions(1);
      lastTime = millis();
    }
    else if(controls.stepButton(1)){
      sequence.makeNoteEveryNDivisions(2);
      lastTime = millis();
    }
    else if(controls.stepButton(2)){
      sequence.makeNoteEveryNDivisions(3);
      lastTime = millis();
    }
    else if(controls.stepButton(3)){
      sequence.makeNoteEveryNDivisions(4);
      lastTime = millis();
    }
  }
  else{
    //if it's in 1/4 mode
    if(!(sequence.subDivision%3)){
      for(int i = 0; i<16; i++){
        if(controls.stepButton(i)){
          uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
          sequence.toggleNote(sequence.activeTrack, sequence.viewStart+i*viewLength/16,viewLength/8);
        }
      }
    }
    //if it's in 1/3 mode, last two buttons do nothing
    else if(!(sequence.subDivision%2)){
      for(int i = 0; i<12; i++){
        if(controls.stepButton(i)){
          uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
          sequence.toggleNote(sequence.activeTrack,sequence.viewStart+i*viewLength/12,viewLength/6);
        }
      }
    }
  }
}

void defaultEncoderControls(){
  while(controls.counterA != 0){
    //changing zoom
    if(controls.counterA >= 1){
      sequence.zoom(true);
    }
    if(controls.counterA <= -1){
      sequence.zoom(false);
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(controls.SHIFT()){
      sequence.toggleTriplets();
    }
    else if(controls.counterB >= 1){
      sequence.changeSubDivInt(true);
    }
    //changing subdivint
    else if(controls.counterB <= -1){
      sequence.changeSubDivInt(false);
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
}

void mainSequencerEncoders(){
  while(controls.counterA != 0){
    //changing pitch
    if(controls.SHIFT()){
      if(controls.counterA >= 1){
        setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch+1,true);
      }
      if(controls.counterA <= -1){
        setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch-1,true);
      }
    }
    //changing zoom
    else{
      if(controls.counterA >= 1 && !controls.A()){
        sequence.zoom(true);
      }
      if(controls.counterA <= -1 && !controls.A()){
        sequence.zoom(false);
      }
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    if(controls.SHIFT()){
      sequence.toggleTriplets();
    }
    else if(controls.counterB >= 1){
      sequence.changeSubDivInt(true,true);
    }
    //changing subdivint
    else if(controls.counterB <= -1){
      sequence.changeSubDivInt(false,true);
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
}

void mainSequence(){
  controls.readJoystick();
  controls.readButtons();
  mainSequencerButtons();
  mainSequencerStepButtons();
  mainSequencerEncoders();
  defaultJoystickControls(true);
  display.clearDisplay();
  drawSeq();
  display.display();
}

class SuperpositionMenu{
  public:
    Note note;
    uint8_t track;
    SuperpositionMenu(Note& n, uint8_t t){
      note = n;
      track = t;
      //first, set the superpos to the track pitch (if it's unset)
      if(note.superposition.pitch == 255)
        note.superposition.pitch = sequence.trackData[track].pitch;
    }
    bool setSuperpositionControls();
    void drawSuperposSelect();
};

bool SuperpositionMenu::setSuperpositionControls(){
  controls.readButtons();
  controls.readJoystick();
  while(controls.counterA){
    if(controls.counterA > 0 && note.superposition.odds > 0){
      note.superposition.odds -= 5;
    }
    if(controls.counterA < 0 && note.superposition.odds < 100){
      note.superposition.odds += 5;
    }
    controls.counterA += controls.counterA<0?1:-1;
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == -1) {
      if(note.superposition.pitch < 127){
        note.superposition.pitch++;
        if(!sequence.playing()){
          MIDI.noteOn(note.superposition.pitch, note.velocity, sequence.trackData[track].channel);
          MIDI.noteOff(note.superposition.pitch, 0, sequence.trackData[track].channel);
        }
        lastTime = millis();
      }
    }
    if (controls.joystickY == 1) {
      if(note.superposition.pitch > 0){
        note.superposition.pitch--;
        if(!sequence.playing()){
          MIDI.noteOn(note.superposition.pitch, note.velocity, sequence.trackData[track].channel);
          MIDI.noteOff(note.superposition.pitch, 0, sequence.trackData[track].channel);
        }
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.NEW() || controls.MENU()){
      lastTime = millis();
      if(note.superposition.pitch == sequence.trackData[track].pitch){
        note.superposition.pitch = 255;//set to 'unset' if it's on the same pitch
      }
      return false;
    }
    if(controls.PLAY()){
      if(controls.SHIFT() || sequence.recording()){
        toggleRecordingMode(waitForNoteBeforeRec);
        lastTime = millis();
        if(sequence.recording())
          menuText = "ready!";
      }
      else{
        togglePlayMode();
        lastTime = millis();
        menuText = sequence.playing()?"playing!":"stopped";
      }
    }
    if(controls.DELETE()){
      lastTime = millis();
      note.superposition.pitch = sequence.trackData[track].pitch;//reset
    }
  }
  return true;
}

void SuperpositionMenu::drawSuperposSelect(){
  SequenceRenderSettings settings;
  NoteCoords nCoords = getNoteCoords(note, track, settings);

  const uint8_t headerHeight = 12;

  // NoteCoords n2Coords = nCoords;
  NoteCoords n2Coords = getNoteCoords(note, track, settings);
  n2Coords.y1 = nCoords.y1+(int16_t(sequence.trackData[track].pitch)-int16_t(note.superposition.pitch)) * trackHeight;
  n2Coords.y2 = n2Coords.y1 + trackHeight;

  if(n2Coords.y1<headerHeight){
    nCoords.offsetY(-n2Coords.y1+headerHeight);
    n2Coords.offsetY(-n2Coords.y1+headerHeight);
  }
  if(n2Coords.y2>(SCREEN_HEIGHT-2)){
    nCoords.offsetY(SCREEN_HEIGHT-n2Coords.y2-2);
    n2Coords.offsetY(SCREEN_HEIGHT-n2Coords.y2-2);
  }

  //drawing horizontal pitch lines
  for(uint8_t i = headerHeight; i<SCREEN_HEIGHT; i++){
    //octave
    if(abs(nCoords.y1 - i)%(12*trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,2);
    //4 semitones
    else if(abs(nCoords.y1 - i)%(4*trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,3);
    else if(abs(nCoords.y1 - i)%(trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,12);
  }

  drawNoteBracket(n2Coords,true);

  String txt = stringify(note.superposition.pitch-sequence.trackData[track].pitch);
  if(note.superposition.pitch>sequence.trackData[track].pitch)
    txt = "+"+txt;

  //draw superposition
  graphics.fillRectWithMissingCorners(n2Coords.x1+1, n2Coords.y1, n2Coords.length-1, trackHeight, SSD1306_BLACK);
  graphics.drawRectWithMissingCorners(n2Coords.x1+1, n2Coords.y1, n2Coords.length-1, trackHeight, SSD1306_WHITE);

  printSmall(n2Coords.x1+n2Coords.length+4,n2Coords.y1+2,txt+"("+pitchToString(note.superposition.pitch,true,true)+")",1);
  drawNote(note, track, nCoords, settings);//draw the note (like normal)

  //draw probability
  graphics.fillSquareDiagonally(0,0,15,note.superposition.odds);
  printSmall(6,5,"%",2);
  printSmall(17,5,stringify(note.superposition.odds)+"%",1);
  printCursive(50,4,"superposition",1);
}

void setSuperposition(Note& note,uint8_t originalTrack){
  SuperpositionMenu superposMenu(note,originalTrack);
  while(superposMenu.setSuperpositionControls()){
    display.clearDisplay();
    superposMenu.drawSuperposSelect();
    display.display();
  }
  note.superposition = superposMenu.note.superposition;
  controls.clearButtons();
}


vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(sequence.trackData.size());
  bool movingBetweenNotes = false;
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawLoopPoints = false;
  settings.trackSelection = false;
  while(true){
    controls.readButtons();
    controls.readJoystick();
    //selectionBox
    //when controls.SELECT()  is pressed and stick is moved, and there's no selection box
    if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
      selBox.begun = true;
      selBox.coords.start.x = sequence.cursorPos;
      selBox.coords.start.y = sequence.activeTrack;
    }
    //if controls.SELECT()  is released, and there's a selection box
    if(!controls.SELECT()  && selBox.begun){
      selBox.coords.end.x = sequence.cursorPos;
      selBox.coords.end.y = sequence.activeTrack;
      if(selBox.coords.start.x>selBox.coords.end.x){
        unsigned short int x1_old = selBox.coords.start.x;
        selBox.coords.start.x = selBox.coords.end.x;
        selBox.coords.end.x = x1_old;
      }
      if(selBox.coords.start.y>selBox.coords.end.y){
        unsigned short int y1_old = selBox.coords.start.y;
        selBox.coords.start.y = selBox.coords.end.y;
        selBox.coords.end.y = y1_old;
      }
      for(int track = selBox.coords.start.y; track<=selBox.coords.end.y; track++){
        for(int time = selBox.coords.start.x; time<=selBox.coords.end.x; time++){
          if(sequence.lookupTable[track][time] != 0){
            //if the note isn't in the vector yet, add it
            if(!isInVector(sequence.lookupTable[track][time],selectedNotes[track]))
              selectedNotes[track].push_back(sequence.lookupTable[track][time]);
            time = sequence.noteData[track][sequence.lookupTable[track][time]].endPos;
          }
        }
      }
      selBox.begun = false;
    }
    if(utils.itsbeen(100)){
      if(!movingBetweenNotes){
        if(controls.joystickY == 1){
          sequence.setActiveTrack(sequence.activeTrack+1,false);
          lastTime = millis();
        }
        if(controls.joystickY == -1){
          sequence.setActiveTrack(sequence.activeTrack-1,false);
          lastTime = millis();
        }
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(!movingBetweenNotes){
          if (controls.joystickX == 1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              sequence.moveCursor(-sequence.subDivision);
              lastTime = millis();
            }
          }
          if (controls.joystickX == -1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              sequence.moveCursor(sequence.subDivision);
              lastTime = millis();
            }
          }
        }
        else{
          if(controls.joystickX == 1){
            sequence.moveToNextNote(false,false);
            lastTime = millis();
          }
          else if(controls.joystickX == -1){
            sequence.moveToNextNote(true,false);
            lastTime = millis();
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      //select
      if(controls.SELECT()  && sequence.IDAtCursor() != 0 && !selBox.begun){
        unsigned short int id;
        id = sequence.IDAtCursor();
        if(controls.SHIFT()){
          //del old vec
          vector<vector<uint8_t>> temp;
          temp.resize(sequence.trackData.size());
          selectedNotes = temp;
          selectedNotes[sequence.activeTrack].push_back(sequence.IDAtCursor());
        }
        else{
          //if the note isn't in the vector yet, add it
          if(!isInVector(sequence.IDAtCursor(),selectedNotes[sequence.activeTrack]))
            selectedNotes[sequence.activeTrack].push_back(sequence.IDAtCursor());
          //if it is, remove it
          else{
            vector<uint8_t> temp;
            for(int i = 0; i<selectedNotes[sequence.activeTrack].size(); i++){
              //push back all the notes that aren't the one the cursor is on
              if(selectedNotes[sequence.activeTrack][i] != sequence.IDAtCursor()){
                temp.push_back(selectedNotes[sequence.activeTrack][i]);
              }
            }
            selectedNotes[sequence.activeTrack] = temp;
          }
        }
        lastTime = millis();
      }
      if(controls.DELETE()){
        lastTime = millis();
        selectedNotes.clear();
        break;
      }
      if(controls.NEW()){
        controls.setNEW(false);
        lastTime = millis();
        break;
      }
      if(controls.LOOP()){
        lastTime = millis();
        movingBetweenNotes = !movingBetweenNotes;
      }
    }
    display.clearDisplay();
    drawSeq(settings);
    printSmall(0,0,text1,SSD1306_WHITE);
    printSmall(0,8,text2,SSD1306_WHITE);
    if(movingBetweenNotes){
      if(millis()%1000 >= 500){
        display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
      }
      else{
        display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
      }
    }
    //draw a note bracket on any note that's been added to the selection
    for(int track = 0; track<selectedNotes.size(); track++){
      for(int note = 0; note<selectedNotes[track].size(); note++){
        drawNoteBracket(sequence.noteData[track][selectedNotes[track][note]],track,settings);
      }
    }
    display.display();
  }
  return selectedNotes;
}
