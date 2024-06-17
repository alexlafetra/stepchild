
//Enter note selection screen, and display a custom icon in the corner
//Used for the FX a lot
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
  while(true){
    controls.readJoystick();
    controls.readButtons();
    defaultEncoderControls();
    if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
      selBox.begun = true;
      selBox.coords.start.x = sequence.cursorPos;
      selBox.coords.start.y = sequence.activeTrack;
    }
    //if controls.SELECT()  is released, and there's a selection box
    if(!controls.SELECT()  && selBox.begun){
      selBox.coords.end.x = sequence.cursorPos;
      selBox.coords.end.y = sequence.activeTrack;
      selBox.begun = false;
      selectBox();
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        return true;
      }
      if(controls.MENU()){
        clearSelection();
        lastTime = millis();
        return false;
      }
      if(controls.SELECT() ){
        if(controls.SHIFT()){
          clearSelection();
          toggleSelectNote(sequence.activeTrack,sequence.IDAtCursor(),false);
        }
        else{
          toggleSelectNote(sequence.activeTrack,sequence.IDAtCursor(),true);
        }
        lastTime = millis();
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !controls.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(sequence.cursorPos%sequence.subDivision){
          moveCursor(-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          moveCursor(-sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickX == -1 && !controls.SHIFT()) {
        if(sequence.cursorPos%sequence.subDivision){
          moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          moveCursor(sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickY == 1) {
        if(recording)
          setActiveTrack(sequence.activeTrack + 1, false);
        else
          setActiveTrack(sequence.activeTrack + 1, false);
        lastTime = millis();
      }
      if (controls.joystickY == -1) {
        if(recording)
          setActiveTrack(sequence.activeTrack - 1, false);
        else
          setActiveTrack(sequence.activeTrack - 1, false);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(50)) {
      if (controls.joystickX == 1 && controls.SHIFT()) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && controls.SHIFT()) {
        moveCursor(1);
        lastTime = millis();
      }
    }
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    drawSeq(settings);
    if(!sequence.selectionCount){
      printSmall(trackDisplay,0,"select notes to "+text,1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to "+text+" "+stringify(sequence.selectionCount)+(sequence.selectionCount == 1?" note":" notes"),1);
    }
    iconFunction(7,1,14,true);
    display.display();
  }
}
