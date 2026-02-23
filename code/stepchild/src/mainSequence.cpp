#include "Stepchild.h"
#include "menus/superpositionMenu.h"
#include "StepchildGraphics.h"
#include "menus.h"

extern Stepchild stepchild;
extern StepchildGraphics graphics;

extern bool (*const fxApplicationFunctions[])();

using namespace std;

void yControls(){
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == 1) {
      stepchild.setActiveTrack(stepchild.activeTrack + 1, !stepchild.playing());
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1) {
      stepchild.setActiveTrack(stepchild.activeTrack - 1, !stepchild.playing());
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
  }
}
void defaultJoystickXControls(){
  if (stepchild.itsbeen(100)) {
    if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(stepchild.cursorPos%stepchild.subDivision){
        stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
        stepchild.lastTime = millis();
        //moving entire loop
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(-stepchild.cursorPos%stepchild.subDivision);
      }
      else{
        stepchild.moveCursor(-stepchild.subDivision);
        stepchild.lastTime = millis();
        //moving entire loop
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(-stepchild.subDivision);
      }
      //moving loop start/end
      if(stepchild.movingLoop == MOVING_LOOP_END){
        stepchild.setLoopPoint(stepchild.cursorPos,true);
      }
      else if(stepchild.movingLoop == MOVING_LOOP_START){
        stepchild.setLoopPoint(stepchild.cursorPos,false);
      }
    }
    if (stepchild.buttons.joystickX == -1 && !stepchild.buttons.SHIFT()) {
      if(stepchild.cursorPos%stepchild.subDivision){
        stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
        stepchild.lastTime = millis();
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
      }
      else{
        stepchild.moveCursor(stepchild.subDivision);
        stepchild.lastTime = millis();
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(stepchild.subDivision);
      }
      //moving loop start/end
      if(stepchild.movingLoop == MOVING_LOOP_END){
        stepchild.setLoopPoint(stepchild.cursorPos,true);
      }
      else if(stepchild.movingLoop == MOVING_LOOP_START){
        stepchild.setLoopPoint(stepchild.cursorPos,false);
      }
    }
  }
  if (stepchild.itsbeen(50)) {
    //moving
    if (stepchild.buttons.joystickX == 1 && stepchild.buttons.SHIFT()) {
      stepchild.moveCursor(-1);
      stepchild.lastTime = millis();
      if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
        stepchild.moveLoop(-1);
      else if(stepchild.movingLoop == MOVING_LOOP_END)
        stepchild.setLoopPoint(stepchild.cursorPos,true);
      else if(stepchild.movingLoop == MOVING_LOOP_START)
        stepchild.setLoopPoint(stepchild.cursorPos,false);
    }
    if (stepchild.buttons.joystickX == -1 && stepchild.buttons.SHIFT()) {
      stepchild.moveCursor(1);
      stepchild.lastTime = millis();
      if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
        stepchild.moveLoop(1);
      else if(stepchild.movingLoop == MOVING_LOOP_END)
        stepchild.setLoopPoint(stepchild.cursorPos,true);
      else if(stepchild.movingLoop == MOVING_LOOP_START)
        stepchild.setLoopPoint(stepchild.cursorPos,false);
    }
  }
}

//moving cursor, loop, and active track. Pass "true" to allow changing the velocity of notes
void defaultJoystickControls(bool velocityEditingAllowed){
  defaultJoystickXControls();
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == 1 && !stepchild.buttons.SHIFT() && !stepchild.buttons.LOOP()) {
      stepchild.setActiveTrack(stepchild.activeTrack + 1, !stepchild.playing()  && !stepchild.recording());
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1 && !stepchild.buttons.SHIFT() && !stepchild.buttons.LOOP()) {
      stepchild.setActiveTrack(stepchild.activeTrack - 1, !stepchild.playing()  && !stepchild.recording());
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(50)){
    //changing vel
    if(velocityEditingAllowed){
      if(stepchild.IDAtCursor()){
        if (stepchild.buttons.joystickY == 1 && stepchild.buttons.SHIFT()) {
          stepchild.changeVel(-10);
          stepchild.lastTime = millis();
        }
        if (stepchild.buttons.joystickY == -1 && stepchild.buttons.SHIFT()) {
          stepchild.changeVel(10);
          stepchild.lastTime = millis();
        }
      }
      else{
        if(stepchild.buttons.joystickY == 1 && stepchild.buttons.SHIFT()){
          stepchild.defaultVel-=10;
          if(stepchild.defaultVel<1)
            stepchild.defaultVel = 1;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.joystickY == -1 && stepchild.buttons.SHIFT()){
          stepchild.defaultVel+=10;
          if(stepchild.defaultVel>127)
            stepchild.defaultVel = 127;
          stepchild.lastTime = millis();
        }
      }
    }
  }
}
void defaultSelectBoxControls(){
  //when stepchild.buttons.SELECT()  is pressed and stick is moved, and there's no selection box yet, start one
  if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
    stepchild.selectionBox.begun = true;
    stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
  }
  //if stepchild.buttons.SELECT()  is released, and there's a selection box, end it and select what was in the box
  if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
    stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
    stepchild.selectionBox.select();
    stepchild.selectionBox.begun = false;
  }
}
//default selection behavior
void defaultSelectControls(){
  if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun){
    uint16_t id = stepchild.IDAtCursor();
    if(id == 0){
      stepchild.clearSelection();
    }
    else{
      //select all
      if(stepchild.buttons.NEW()){
        stepchild.selectAll();
      }
      //select only one
      else if(stepchild.buttons.SHIFT()){
        stepchild.clearSelection();
        stepchild.toggleSelectNote(stepchild.activeTrack,id, false);
      }
      //normal select
      else{
        stepchild.toggleSelectNote(stepchild.activeTrack, id, true);          
      }
    }
    stepchild.lastTime = millis();
  }
}

void defaultLoopControls(){
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.LOOP()){
        //if you're not moving a loop, start
        if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS){
          //if you're on the start, move the start
          if(stepchild.cursorPos == stepchild.loopData[stepchild.activeLoop].start){
            stepchild.movingLoop = MOVING_LOOP_END;
            stepchild.tooltipText = "Moving Loop Start";
          }
          //if you're on the end
          else if(stepchild.cursorPos == stepchild.loopData[stepchild.activeLoop].end){
            stepchild.movingLoop = MOVING_LOOP_START;
            stepchild.tooltipText = "Moving Loop End";
          }
          //if you're not on either, move the whole loop
          else{
            stepchild.movingLoop = MOVING_BOTH_LOOP_POINTS;
            stepchild.tooltipText = "Moving Loop";
          }
          stepchild.lastTime = millis();
        }
        //if you were moving, stop
        else{
          stepchild.movingLoop = MOVING_NO_LOOP_POINTS;
          stepchild.lastTime = millis();
        }
    }
  }
}
//default copy/paste behavior
void defaultCopyControls(){
  if(stepchild.buttons.COPY()){
    if(stepchild.buttons.SHIFT())
      stepchild.clipboard.paste();
    else{
      stepchild.clipboard.copy();
    }
    stepchild.lastTime = millis();
  }
}
void mainSequencerButtons(){
  defaultSelectBoxControls();
  if(!stepchild.buttons.NEW())
    stepchild.drawingNote = false;
  //del happens a liitle faster (so you can draw/erase fast)
  if(stepchild.itsbeen(75)){
    //del
    if(stepchild.buttons.DELETE() && !stepchild.buttons.SHIFT()){
      if (stepchild.selectionCount > 0){
        stepchild.deleteSelected();
        stepchild.lastTime = millis();
      }
      else if(stepchild.IDAtCursor() != 0){
        stepchild.deleteNote(stepchild.activeTrack,stepchild.cursorPos);
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    //new
    if(stepchild.buttons.NEW() && !stepchild.drawingNote && !stepchild.buttons.SELECT() ){
      if(stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        uint8_t selectedFX = quickFX();
        if(selectedFX != 255){
          fxApplicationFunctions[selectedFX]();
          return;
        }
      }
      else if(!stepchild.buttons.SHIFT()){
        if((stepchild.IDAtCursor() == 0 || stepchild.cursorPos != stepchild.noteAtCursor().startPos)){
          stepchild.makeNote(stepchild.activeTrack,stepchild.cursorPos,stepchild.subDivision,true);
          stepchild.moveCursor(stepchild.subDivision);
          stepchild.drawingNote = true;
          stepchild.lastTime = millis();
        }
        //if you are on the start pos of a note
        else{
          stepchild.lastTime = millis();
          setSuperpositionMenu(stepchild.noteData[stepchild.activeTrack][stepchild.lookupTable[stepchild.activeTrack][stepchild.cursorPos]],stepchild.activeTrack);
          return;
        }
      }
    }
    defaultSelectControls();
    defaultCopyControls();
    if(!stepchild.buttons.SHIFT()){
      defaultLoopControls();
    }
    //shift+loop pushes the loop to to a new loop
    else if(stepchild.buttons.LOOP()){
      stepchild.lastTime = millis();
      stepchild.pushToNewLoop();
    }

    if(stepchild.buttons.DELETE() && stepchild.buttons.SHIFT()){
        stepchild.muteNote(stepchild.activeTrack, stepchild.IDAtCursor(), true);
      if(stepchild.selectionCount>0){
        stepchild.muteSelectedNotes();
      }
      stepchild.lastTime = millis();
    }

    //Modes: play, listen, and record
    if(stepchild.buttons.PLAY() && !stepchild.buttons.SHIFT() && !stepchild.recording()){
      stepchild.togglePlay();
      stepchild.lastTime = millis();
    }
    //if play+stepchild.buttons.SHIFT(), or if play and it's already stepchild.recording()
    if((stepchild.buttons.PLAY() && stepchild.buttons.SHIFT()) || (stepchild.buttons.PLAY() && stepchild.recording())){
      stepchild.toggleRecording(stepchild.waitForNoteBeforeRec);
      stepchild.lastTime = millis();
    }
    //menu press
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      if(stepchild.buttons.SHIFT()){
        fxMenu();
      }
      else{
        mainMenu();
      }
      return;
    }
    if(stepchild.buttons.A()){
      stepchild.lastTime = millis();
      trackMenu();
      return;
    }
    if(stepchild.buttons.B()){
      stepchild.lastTime = millis();
      editMenu();
      return;
    }
  }
}

void mainSequencerStepButtons(){
  //don't need to time debounce the step buttons!
  //DJ loop selector
  if(stepchild.buttons.SHIFT()){
    if(stepchild.buttons.stepButton(15) && stepchild.activeTrack){
      stepchild.setActiveTrack(stepchild.activeTrack-1,!stepchild.playing()  && !stepchild.recording());
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.stepButton(14)){
      stepchild.setActiveTrack(stepchild.activeTrack+1,!stepchild.playing()  && !stepchild.recording());
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.stepButton(0)){
      stepchild.makeNoteEveryNDivisions(1);
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.stepButton(1)){
      stepchild.makeNoteEveryNDivisions(2);
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.stepButton(2)){
      stepchild.makeNoteEveryNDivisions(3);
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.stepButton(3)){
      stepchild.makeNoteEveryNDivisions(4);
      stepchild.lastTime = millis();
    }
  }
  else{
    //if it's in 1/4 mode
    if(!(stepchild.subDivision%3)){
      for(int i = 0; i<16; i++){
        if(stepchild.buttons.stepButton(i)){
          uint16_t viewLength = stepchild.viewEnd-stepchild.viewStart;
          stepchild.toggleNote(stepchild.activeTrack, stepchild.viewStart+i*viewLength/16,viewLength/8);
        }
      }
    }
    //if it's in 1/3 mode, last two buttons do nothing
    else if(!(stepchild.subDivision%2)){
      for(int i = 0; i<12; i++){
        if(stepchild.buttons.stepButton(i)){
          uint16_t viewLength = stepchild.viewEnd-stepchild.viewStart;
          stepchild.toggleNote(stepchild.activeTrack,stepchild.viewStart+i*viewLength/12,viewLength/6);
        }
      }
    }
  }
}

void defaultEncoderControls(){
  while(stepchild.buttons.counterA != 0){
    //changing zoom
    if(stepchild.buttons.counterA >= 1){
      stepchild.zoom(true);
    }
    if(stepchild.buttons.counterA <= -1){
      stepchild.zoom(false);
    }
    stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
  }
  while(stepchild.buttons.counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(stepchild.buttons.SHIFT()){
      stepchild.toggleTriplets();
    }
    else if(stepchild.buttons.counterB >= 1){
      stepchild.changeSubDivInt(true);
    }
    //changing subdivint
    else if(stepchild.buttons.counterB <= -1){
      stepchild.changeSubDivInt(false);
    }
    stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
  }
}

void mainSequencerEncoders(){
  while(stepchild.buttons.counterA != 0){
    //changing pitch
    if(stepchild.buttons.SHIFT()){
      if(stepchild.buttons.counterA >= 1){
        stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch+1,true);
      }
      if(stepchild.buttons.counterA <= -1){
        stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch-1,true);
      }
    }
    //changing zoom
    else{
      if(stepchild.buttons.counterA >= 1 && !stepchild.buttons.A()){
        stepchild.zoom(true);
      }
      if(stepchild.buttons.counterA <= -1 && !stepchild.buttons.A()){
        stepchild.zoom(false);
      }
    }
    stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
  }
  while(stepchild.buttons.counterB != 0){
    if(stepchild.buttons.SHIFT()){
      stepchild.toggleTriplets();
    }
    else if(stepchild.buttons.counterB >= 1){
      stepchild.changeSubDivInt(true,true);
    }
    //changing subdivint
    else if(stepchild.buttons.counterB <= -1){
      stepchild.changeSubDivInt(false,true);
    }
    stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
  }
}

void mainSequence(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  mainSequencerButtons();
  mainSequencerStepButtons();
  mainSequencerEncoders();
  defaultJoystickControls(true);
  stepchild.display.clearDisplay();
  graphics.drawSeq();
  stepchild.display.display();
}
