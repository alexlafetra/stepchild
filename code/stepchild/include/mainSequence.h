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
          lastTime = millis();
          chop(sequence.activeTrack,sequence.cursorPos);
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