#include "guiUtilities.h"
#include <vector>
#include "Arduino.h"
#include "Stepchild.h"

#include "graphics/WireFrame.h"
#include "mainSequence.h"

;

using namespace std;

// 'loop_next_reverse', 13x16px
const unsigned char loop_next_reverse_bmp [] = {
	0x0f, 0xf8, 0x37, 0xf8, 0x43, 0xf8, 0x25, 0xf8, 0xa2, 0xf8, 0xd5, 0x78, 0xcf, 0xf8, 0xe0, 0x00, 
	0xff, 0x80, 0xff, 0xd8, 0xff, 0xe8, 0x7f, 0xf0, 0x7f, 0xe0, 0x3f, 0xc8, 0x0f, 0x98, 0x00, 0x38
};
// 'loop_next_reverse_overlay', 14x16px
const unsigned char loop_next_reverse_overlay_bmp [] = {
	0x07, 0xfc, 0x1b, 0xfc, 0x21, 0xfc, 0x12, 0xfc, 0x51, 0x7c, 0x6a, 0xbc, 0x67, 0xfc, 0x70, 0x00, 
	0x7f, 0xc0, 0x7f, 0xec, 0x7f, 0xf4, 0xbf, 0xf8, 0xbf, 0xf0, 0xdf, 0xe4, 0xe7, 0xcc, 0xf8, 0x1c
};
// 'loop_next', 13x16px
const unsigned char loop_next_bmp [] = {
	0xff, 0x80, 0xff, 0x60, 0xfe, 0x10, 0xfd, 0x20, 0xfa, 0x28, 0xf5, 0x58, 0xff, 0x98, 0x00, 0x38, 
	0x0f, 0xf8, 0xdf, 0xf8, 0xbf, 0xf8, 0x7f, 0xf0, 0x3f, 0xf0, 0x9f, 0xe0, 0xcf, 0x80, 0xe0, 0x00
};
// 'corner_bottom', 8x8px
const unsigned char loop_corner_bottom_bmp []  = {
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x0f
};
// 'arrow_top', 15x8px
const unsigned char loop_arrow_top_bmp []  = {
	0x0f, 0xe0, 0x3f, 0xf6, 0x7f, 0xfa, 0x7f, 0xfc, 0xff, 0xf8, 0xff, 0xf2, 0xff, 0xe6, 0xff, 0x0e
};
// 'loop_arrow_workaround', 8x16px
const unsigned char loop_arrow_workaround_bmp [] = {
	0xf0, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf0
};
// 'loop_point_right', 3x5px
const unsigned char loop_point_right_bmp [] = {
	0x80, 0xc0, 0xe0, 0xc0, 0x80
};
// 'corner_bottom_overlay', 8x9px
const unsigned char corner_bottom_overlay_bmp [] = {
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0xbf, 0x8f, 0xe0
};
//loop icons
// 'inf', 9x5px
const unsigned char inf_bmp []  = {
	0x63, 0x00, 0x94, 0x80, 0x88, 0x80, 0x94, 0x80, 0x63, 0x00
};
// 'down_arrow', 7x7px
const unsigned char down_arrow_bmp []  = {
	0x38, 0x38, 0x38, 0xba, 0x7c, 0x38, 0x10
};
// 'rnd_equal', 11x7px
const unsigned char rnd_equal_bmp []  = {
	0xfe, 0x00, 0x82, 0x00, 0xa2, 0xe0, 0x82, 0x00, 0x8a, 0xe0, 0x82, 0x00, 0xfe, 0x00
};
// 'rnd', 7x7px
const unsigned char rnd_bmp []  = {
	0xfe, 0x82, 0xa2, 0x82, 0x8a, 0x82, 0xfe
};
// 'return', 7x7px
const unsigned char return_bmp []  = {
	0x08, 0x1c, 0x3e, 0x1c, 0x1c, 0xfc, 0xf8
};


//loop title --------------
// 'L', 7x9px
const unsigned char loop_L []  = {
	0x30, 0x10, 0x10, 0x10, 0x30, 0x30, 0x20, 0x66, 0xfc
};
// 'O', 7x9px
const unsigned char loop_O []  = {
	0x18, 0x3c, 0x66, 0x42, 0xc6, 0x84, 0xcc, 0x7e, 0x30
};
// 'P', 7x9px
const unsigned char loop_P []  = {
	0x3c, 0x16, 0x12, 0x32, 0x3c, 0x20, 0x20, 0x60, 0xf0
};
// 'parenth', 5x9px
const unsigned char loop_parenth []  = {
	0x20, 0x10, 0x18, 0x18, 0x18, 0x30, 0x30, 0x60, 0xc0
};


void moveCursorWithinLoop(int amount, uint8_t whichLoop){
  //if the cursor is going to move before the loop
  if(amount<0 && (stepchild.cursorPos + amount)<stepchild.loopData[whichLoop].start)
    stepchild.moveCursor(stepchild.cursorPos - stepchild.loopData[whichLoop].start);
  //if the cursor is going to move past the loop
  else if(amount>0 && (stepchild.cursorPos+amount)>=stepchild.loopData[whichLoop].end)
    stepchild.moveCursor(stepchild.loopData[whichLoop].end-stepchild.cursorPos);
  //if not, then it's moving within the loop
  else
    stepchild.moveCursor(amount);
}

bool isWithinLoop(int val, int loop, int amount, bool inclusive){
  if(!inclusive){
    if(val<(stepchild.loopData[loop].end-amount) && val>(stepchild.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(val<=(stepchild.loopData[loop].end-amount) && val>=(stepchild.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
}

void insertLoop(int afterThis, Loop newLoop){
  vector<Loop> tempData;
  for(int i = 0; i<stepchild.loopData.size(); i++){
    tempData.push_back(stepchild.loopData[i]);
    if(i == afterThis)
      tempData.push_back(newLoop);
  }
  stepchild.loopData.swap(tempData);
}

void dupeLoop(int loop){
  vector<Loop> tempData;
  for(int i = 0; i<stepchild.loopData.size(); i++){
    tempData.push_back(stepchild.loopData[i]);
    if(i == loop)
      tempData.push_back(stepchild.loopData[i]);
  }
  stepchild.loopData.swap(tempData);
}

uint16_t getLongestLoop(){
  uint16_t longestLength = 0;
  for(uint8_t i = 0; i<stepchild.loopData.size(); i++){
    uint16_t l = stepchild.loopData[i].end - stepchild.loopData[i].start;
    if(l>longestLength)
      longestLength = l;
  }
  return longestLength;
}

//just your normal editing controls, but you can't move the view past the loop
bool viewLoopControls(uint8_t which){
  //selectionBox
  //when stepchild.buttons.SELECT()  is pressed and stick is moved, and there's no selection box
  if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
    stepchild.selectionBox.begun = true;
    stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
  }
  //if stepchild.buttons.SELECT()  is released, and there's a selection box
  if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
    stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
    stepchild.selectionBox.select();
    stepchild.selectionBox.begun = false;
  }
  if(!stepchild.buttons.NEW())
    stepchild.drawingNote = false;
  mainSequencerEncoders();
  if (stepchild.itsbeen(100)) {
    if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(stepchild.cursorPos%stepchild.subDivision){
        if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(-stepchild.cursorPos%stepchild.subDivision,which);
        else
          stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
        stepchild.lastTime = millis();
        //moving entire loop
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(-stepchild.cursorPos%stepchild.subDivision);
      }
      else{
        if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(-stepchild.subDivision,which);
        else
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
        if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision,which);
        else
          stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
        stepchild.lastTime = millis();
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
          stepchild.moveLoop(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
      }
      else{
        if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(stepchild.subDivision,which);
        else
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
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == 1 && !stepchild.buttons.SHIFT() && !stepchild.buttons.LOOP()) {
      if(stepchild.recording())//if you're not in normal mode, you don't want it to be loud
        stepchild.setActiveTrack(stepchild.activeTrack + 1, false);
      else
        stepchild.setActiveTrack(stepchild.activeTrack + 1, true);
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1 && !stepchild.buttons.SHIFT() && !stepchild.buttons.LOOP()) {
      if(stepchild.recording())//if you're not in normal mode, you don't want it to be loud
        stepchild.setActiveTrack(stepchild.activeTrack - 1, false);
      else
        stepchild.setActiveTrack(stepchild.activeTrack - 1, true);
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
  }
  if (stepchild.itsbeen(50)) {
    //moving
    if (stepchild.buttons.joystickX == 1 && stepchild.buttons.SHIFT()) {
      if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
        moveCursorWithinLoop(-1,which);
      else
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
      if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS)
        moveCursorWithinLoop(1,which);
      else
        stepchild.moveCursor(1);
      stepchild.lastTime = millis();
      if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
        stepchild.moveLoop(1);
      else if(stepchild.movingLoop == MOVING_LOOP_END)
        stepchild.loopData[stepchild.activeLoop].start = stepchild.cursorPos;
      else if(stepchild.movingLoop == MOVING_LOOP_START)
        stepchild.loopData[stepchild.activeLoop].end = stepchild.cursorPos;
    }
    //changing vel
    if (stepchild.buttons.joystickY == 1 && stepchild.buttons.SHIFT()) {
      stepchild.changeVel(-10);
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1 && stepchild.buttons.SHIFT()) {
      stepchild.changeVel(10);
      stepchild.lastTime = millis();
    }

    if(stepchild.IDAtCursor()==0){
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
    if(stepchild.buttons.NEW() && !stepchild.buttons.A() && !stepchild.drawingNote && !stepchild.buttons.SELECT() ){
      if((!stepchild.buttons.SHIFT())&&(stepchild.IDAtCursor() == 0 || stepchild.cursorPos != stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos)){
        stepchild.makeNote(stepchild.activeTrack,stepchild.cursorPos,stepchild.subDivision,true);
        stepchild.drawingNote = true;
        stepchild.lastTime = millis();
        moveCursorWithinLoop(stepchild.subDivision,which);
      }
      if(stepchild.buttons.SHIFT()){
        stepchild.addTrack(stepchild.defaultPitch, stepchild.defaultChannel,false);
        stepchild.lastTime = millis();
      }
    }
    //select
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun){
      uint16_t id = stepchild.IDAtCursor();
      //select all
      if(stepchild.buttons.NEW()){
        stepchild.selectAll();
      }
      //select only one
      else if(stepchild.buttons.SHIFT()){
        stepchild.clearSelection();
        stepchild.toggleSelectNote(stepchild.activeTrack, id, false);
      }
      //normal select
      else{
        stepchild.toggleSelectNote(stepchild.activeTrack, id, true);          
      }
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.DELETE() && stepchild.buttons.SHIFT()){
      stepchild.muteNote(stepchild.activeTrack, stepchild.IDAtCursor(), true);
      stepchild.lastTime = millis();
    }
 
    //loop
    if(stepchild.buttons.LOOP()){
      //if you're not moving a loop, start
      if(stepchild.movingLoop == MOVING_NO_LOOP_POINTS){
        //if you're on the start, move the start
        if(stepchild.cursorPos == stepchild.loopData[stepchild.activeLoop].start){
          stepchild.movingLoop = MOVING_LOOP_END;
        }
        //if you're on the end
        else if(stepchild.cursorPos == stepchild.loopData[stepchild.activeLoop].end){
          stepchild.movingLoop = MOVING_LOOP_START;
        }
        //if you're not on either, move the whole loop
        else{
          stepchild.movingLoop = MOVING_BOTH_LOOP_POINTS;
        }
        stepchild.lastTime = millis();
      }
      //if you were moving, stop
      else{
        stepchild.movingLoop = MOVING_NO_LOOP_POINTS;
        stepchild.lastTime = millis();
      }
    }
    //menu press
    if(stepchild.buttons.MENU()){
      if(stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        return false;
      }
      else{
        stepchild.lastTime = millis();
        stepchild.buttons.setMENU(false) ;
        return false;
      }
    }
    //copy/pasate
    if(stepchild.buttons.COPY()){
      if(stepchild.buttons.SHIFT())
        stepchild.clipboard.paste();
      else{
        stepchild.clipboard.copy();
      }
      stepchild.lastTime = millis();
    }
  }
  return true;
}

void viewLoop(uint8_t which){
  stepchild.setCursor(stepchild.loopData[which].start);
  stepchild.setActiveLoop(which);
  String tempText;
  while(true){
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    if(!viewLoopControls(which))
      return;
    tempText = stepchild.tooltipText;
    stepchild.tooltipText = "lp"+stringify(which+1)+" "+stepchild.tooltipText;
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.shadeOutsideLoop = true;
    graphics.drawSeq(settings);
    stepchild.display.display();
    stepchild.tooltipText = tempText;
  }
}

void setLoopToInfinite(uint8_t targetL){
  //if it's already a 3, set it to 0
  if(stepchild.loopData[targetL].type == INFINITE){
    stepchild.loopData[targetL].type = NORMAL;
  }
  //if not, set this loop to 3
  else{
    stepchild.loopData[targetL].type = INFINITE;
  }
  //set all other inf loops to 0
  for(uint8_t l = 0; l<stepchild.loopData.size(); l++){
    if(l != targetL){
      if(stepchild.loopData[l].type == INFINITE)
        stepchild.loopData[l].type = NORMAL;
    }
  }
}

String getLoopType(LoopType type){
  switch(type){
    case NORMAL:
      return "normal";
    case RANDOM:
      return "rnd";
    case RANDOM_SAME_LENGTH:
      return "rnd same";
    case RETURN:
      return "reset";
    case INFINITE:
      return "infinite";
    default:
      return "idk bruh";
  }
}

String getLoopBehavior(LoopType type){
  switch(type){
    case NORMAL:
      return "the next loop";
    case RANDOM:
      return "a random loop";
    case RANDOM_SAME_LENGTH:
      return "a random loop of same length";
    case RETURN:
      return "the 1st loop";
    case INFINITE:
      return "this loop";
    default:
      return "idk bruh";
  }
}

class LoopMenu:public StepchildMenu{
  public:
  WireFrame icon;
  uint8_t menuStart = 0;
  uint8_t numberOfLoopsShown = 5;
  int8_t yCoordOffset = 0;
  const uint8_t loopHeight = 9;

  LoopMenu(){
    icon = makeLoopArrows(0);
    icon.offset.x = 10;
    icon.offset.y = 5;
    // icon.scale = 1.5;
    icon.scale = 1;
    coords = CoordinatePair(0,0,128,64);
  }
  uint8_t lastReturnLoopID(){
    uint8_t id = 0;
    for(uint8_t i = 0; i<stepchild.loopData.size(); i++){
      if(stepchild.loopData[i].type == RETURN || (stepchild.loopData[i].type == NORMAL && (i == (stepchild.loopData.size()-1)))){
        id = i;
      }
    }
    return id;
  }
  bool isThisATrickyLoop(uint8_t loopID){
    return !(loopID%2) && (stepchild.loopData[loopID].type == RETURN || (stepchild.loopData[loopID].type == NORMAL && stepchild.loopData.size()-1 == loopID));
  }
  bool thatOneGraphicalCondition(uint8_t loopID){
    if(loopID == stepchild.loopData.size()-1)
      return false;
    return isThisATrickyLoop(loopID+1);
  }
  bool loopAboveIsPointingToThisLoop(uint8_t id){
    if(id){
      return (stepchild.loopData[id - 1].type == NORMAL);
    }
    return false;
  }
  void drawLoopChunk(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t loopID, bool selected){
    if(stepchild.playing() && stepchild.activeLoop == loopID){
      uint8_t progress = float(w) * float(stepchild.playheadPos - stepchild.loopData[loopID].start)/float(stepchild.loopData[loopID].end - stepchild.loopData[loopID].start);
      stepchild.display.drawRect(x,y,w,h,1);
      stepchild.display.fillRect(x,y,progress,h,1);
    }
    else{
      stepchild.display.fillRect(x,y,w,h,1);
    }
    if(!loopID && stepchild.loopData.size() == 1 && (stepchild.loopData[loopID].type == NORMAL || stepchild.loopData[loopID].type == RETURN)){
    }
    else if(!loopID && stepchild.loopData[loopID].type == RETURN){
    }
    else
    switch(stepchild.loopData[loopID].type){
      case NORMAL:
        //if it's the last loop, treat it like a return loop instead
        if(!(loopID == stepchild.loopData.size()-1)){
          if(loopID%2){
            if(thatOneGraphicalCondition(loopID))
              stepchild.display.drawBitmap(x-14,y,loop_next_reverse_overlay_bmp,14,16,1,0);
            else
              stepchild.display.drawBitmap(x-13,y,loop_next_reverse_bmp,13,16,1,0);
            if(!loopAboveIsPointingToThisLoop(loopID)){
              stepchild.display.drawFastVLine(x+w,y,h,1);
              stepchild.display.drawFastVLine(x+w+1,y+1,h-2,1);
              stepchild.display.drawFastVLine(x+w+2,y+2,h-4,1);
            }
          }
          else{
            stepchild.display.drawBitmap(x+w,y,loop_next_bmp,13,16,1,0);
            if(!loopAboveIsPointingToThisLoop(loopID)){
              stepchild.display.drawFastVLine(x-1,y,h,1);
              stepchild.display.drawFastVLine(x-2,y+1,h-2,1);
              stepchild.display.drawFastVLine(x-3,y+2,h-4,1);
            }
          }
          break;
        }
      case RETURN:
        //if you need to snake underneath the loop to draw the arrow
        if(isThisATrickyLoop(loopID) || !loopAboveIsPointingToThisLoop(loopID)){
          stepchild.display.drawFastVLine(x+w,y+1,h-2,1);
          stepchild.display.drawFastVLine(x+w+1,y+2,h-4,1);
        }
        //tube out to bottom corner
        stepchild.display.fillRect(x-14,y,14,h,1);
        //bottom corner
        if(!(lastReturnLoopID() == loopID))
          stepchild.display.drawBitmap(x-22,y-1,corner_bottom_overlay_bmp,8,9,1,0);
        else
          stepchild.display.drawBitmap(x-22,y-1,loop_corner_bottom_bmp,8,8,1);
        break;
      case RANDOM_SAME_LENGTH:
        if(loopID%2){
          stepchild.display.drawFastHLine(x-14,y+2,3,1);
          stepchild.display.drawFastHLine(x-14,y+4,3,1);
        }
        else{
          stepchild.display.drawFastHLine(x+w+11,y+2,3,1);
          stepchild.display.drawFastHLine(x+w+11,y+4,3,1);
        }
      case RANDOM:
        if(loopID%2){
          stepchild.display.drawBitmap(x-10,y,rnd_bmp,7,7,SSD1306_WHITE);
          stepchild.display.drawFastVLine(x-1,y+1,h-2,1);
          stepchild.display.drawFastVLine(x-2,y+2,h-4,1);
          stepchild.display.drawPixel(x-3,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            stepchild.display.drawFastVLine(x+w,y,h,1);
            stepchild.display.drawFastVLine(x+w+1,y+1,h-2,1);
            stepchild.display.drawFastVLine(x+w+2,y+2,h-4,1);
          }
        }
        else{
          stepchild.display.drawBitmap(x+w+3,y,rnd_bmp,7,7,SSD1306_WHITE);
          stepchild.display.drawFastVLine(x+w,y+1,h-2,1);
          stepchild.display.drawFastVLine(x+w+1,y+2,h-4,1);
          stepchild.display.drawPixel(x+w+2,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            stepchild.display.drawFastVLine(x-1,y,h,1);
            stepchild.display.drawFastVLine(x-2,y+1,h-2,1);
            stepchild.display.drawFastVLine(x-3,y+2,h-4,1);
          }
        }
        
        break;
      case INFINITE:
        if(loopID%2){
          stepchild.display.drawBitmap(x-10,y+1,inf_bmp,9,5,SSD1306_WHITE);
          stepchild.display.drawFastVLine(x-1,y+1,h-2,1);
          stepchild.display.drawFastVLine(x-2,y+2,h-4,1);
          stepchild.display.drawPixel(x-3,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            stepchild.display.drawFastVLine(x+w,y,h,1);
            stepchild.display.drawFastVLine(x+w+1,y+1,h-2,1);
            stepchild.display.drawFastVLine(x+w+2,y+2,h-4,1);
          }
        }
        else{
          stepchild.display.drawBitmap(x+w+1,y+1,inf_bmp,9,5,SSD1306_WHITE);
          stepchild.display.drawFastVLine(x+w,y+1,h-2,1);
          stepchild.display.drawFastVLine(x+w+1,y+2,h-4,1);
          stepchild.display.drawPixel(x+w+2,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            stepchild.display.drawFastVLine(x-1,y,h,1);
            stepchild.display.drawFastVLine(x-2,y+1,h-2,1);
            stepchild.display.drawFastVLine(x-3,y+2,h-4,1);
          }
        }
        break;
    }
    graphics.printSmall(x+w/2-2,y+1,stringify(loopID+1),2);
  }
  void drawReturnBar(uint8_t yStart, uint8_t firstLoopToDraw){
    uint8_t lastReturnID = lastReturnLoopID();
    //if there's just one loop
    if(stepchild.loopData.size() == 1 && lastReturnID == 0 && (stepchild.loopData[0].type == NORMAL || stepchild.loopData[0].type == RETURN)){
      stepchild.display.drawBitmap(coords.start.x+2,yStart-1,loop_arrow_top_bmp,15,8,1);
      stepchild.display.drawBitmap(coords.start.x+2,yStart+8,loop_corner_bottom_bmp,8,8,1);
      stepchild.display.drawBitmap(coords.start.x+32,yStart,loop_arrow_workaround_bmp,8,16,1);
      stepchild.display.fillRect(coords.start.x+10,yStart+9,22,7,1);
      stepchild.display.fillRect(coords.start.x+17,yStart,7,7,1);
      stepchild.display.drawFastHLine(coords.start.x+2,yStart+7,7,1);
    }
    //if there's a return loop below & offscreen
    else if(lastReturnID > (firstLoopToDraw+menuStart)){
      //if the start is onscreen, draw the corner arrow and connect it
      if(menuStart == 0){
        stepchild.display.drawBitmap(2,yStart-1,loop_arrow_top_bmp,15,8,1);
        stepchild.display.fillRect(17,yStart,7,7,1);
      }
      else{
        stepchild.display.fillRect(2,yStart,7,8,1);
      }
      stepchild.display.fillRect(2,yStart+7,7,57-yStart,1);
    }
    //if the return loop is onscreen
    else if(lastReturnID <= (menuStart+firstLoopToDraw) && lastReturnID){
      //if the first loop is onscreen too
      if(menuStart == 0){
        stepchild.display.drawBitmap(2,yStart-1,loop_arrow_top_bmp,15,8,1);
        stepchild.display.fillRect(17,yStart,7,7,1);
        stepchild.display.fillRect(2,yStart+7,7,(lastReturnID-1) * loopHeight+1,1);
      }
      //if not
      else{
        stepchild.display.fillRect(2,yStart,7,(lastReturnID-menuStart) * loopHeight,1);
      }
    }
    //if neither the return loop or the start is onscreen
    else{
      stepchild.display.fillRect(2,yStart,7,64-yStart,1);
    }
  }
  void displayMenu(){
    icon.rotate(-1,2);
    stepchild.display.clearDisplay();

    //if the last loop drawn is a return loop, and it's a snake-around loop,
    //then move it up and draw one less loop
    uint8_t firstLoopToDraw = min(numberOfLoopsShown,stepchild.loopData.size()-1);
    uint8_t yStart = coords.start.y + 12 + yCoordOffset;

    //drawing return bar
    drawReturnBar(yStart,firstLoopToDraw);

    //draw the loop blocks
    for(int8_t i = firstLoopToDraw; i>=0; i--){
      drawLoopChunk(coords.start.x+24,yStart+(i)*loopHeight,8,7,i+menuStart,cursor == (i+menuStart));
    }
    //draw one extra above, just for visual continuity
    if(menuStart || (!menuStart && yCoordOffset)){
      if(menuStart)
        drawLoopChunk(coords.start.x+24,yStart-loopHeight,8,7,menuStart-1,false);
      //crop it
      stepchild.display.fillRect(coords.start.x,coords.start.y,48,12,0);
    }
    stepchild.display.drawFastHLine(coords.start.x+16,coords.start.y+10,48,1);

    //title stuff
    stepchild.display.drawBitmap(coords.start.x+20,coords.start.y,loop_L,7,9,SSD1306_WHITE);
    stepchild.display.drawBitmap(coords.start.x+27,coords.start.y,loop_O,7,9,SSD1306_WHITE);
    stepchild.display.drawBitmap(coords.start.x+34,coords.start.y,loop_O,7,9,SSD1306_WHITE);
    stepchild.display.drawBitmap(coords.start.x+41,coords.start.y,loop_P,7,9,SSD1306_WHITE);
    graphics.printSmall(coords.start.x+51,coords.start.y+2,"menu",1);
    icon.render();

    //draw the highlight arrow
    if(cursor <=  menuStart + firstLoopToDraw && cursor >= menuStart)
      graphics.drawArrow(coords.start.x+24+millis()/200%2,yStart + 3 + (cursor-menuStart)*loopHeight,2,ARROW_RIGHT,0,0,false);

    //decrement the interpolate distance var
    if(yCoordOffset)
      yCoordOffset += ((yCoordOffset < 0)?1:-1);

    //drawing loop info
    #define LOOPINFO coords.start.x+70

    //arrow for loops above
    if(menuStart)
      graphics.drawArrow(LOOPINFO,millis()/200%2,2,ARROW_UP,1);
    if(stepchild.loopData.size() > menuStart + 6)
      graphics.drawArrow(LOOPINFO,7-millis()/200%2,2,ARROW_DOWN,1);
    
    //loop number
    graphics.printSmall(LOOPINFO+8,coords.start.y,"loop #"+stringify(cursor),1);
    //loop type
    graphics.drawButton(LOOPINFO,coords.start.y+7,"type",1);
    graphics.printSmall(LOOPINFO+23,coords.start.y+8,getLoopType(stepchild.loopData[cursor].type),1);
    uint8_t height = graphics.printSmall_overflow(LOOPINFO,coords.start.y+15,0,"leads to "+getLoopBehavior(stepchild.loopData[cursor].type),1);
    //# of reps
    graphics.drawButton(LOOPINFO,coords.start.y+15+height*7,"reps",1);
    // graphics.drawButton(LOOPINFO+28,coords.start.y+15+height*7,stringify(stepchild.loopData[cursor].reps),1);
    //length

    //buttons
    //set active/is active
    //delete
    //duplicate

    stepchild.display.display();
  }
  bool loopMenuControls(){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        return false;
      }
      if(stepchild.buttons.SELECT()){
        //should trigger a dropdown menu for: set active, move loop points, delete
        stepchild.lastTime = millis();
        stepchild.activeLoop = cursor;
        return false;
      }
      if(stepchild.buttons.DELETE() && stepchild.loopData.size()>1){
        if(binarySelectionBox(64,32,"NO","YEA","delete loop?")==1){
          stepchild.deleteLoop(cursor);
          if(cursor >= stepchild.loopData.size()){
            cursor = stepchild.loopData.size()-1;
            if(menuStart + numberOfLoopsShown > stepchild.loopData.size()-1 && menuStart)
              menuStart--;
          }
        }
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.NEW()){
        dupeLoop(cursor);
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.DOWN() && cursor > 0){
        cursor--;
        if(cursor<menuStart){
          yCoordOffset = -9;
          menuStart--;
        }
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.UP() && cursor < stepchild.loopData.size()-1){
        cursor++;
        if(cursor > (menuStart+numberOfLoopsShown) && (menuStart + numberOfLoopsShown < stepchild.loopData.size()-1)){
          menuStart++;
          yCoordOffset = 9;
        }
        stepchild.lastTime = millis();
      }
    }
    //changing the loop type
    while(stepchild.buttons.counterB != 0){
      if(stepchild.buttons.counterB>0){
        if(stepchild.loopData[cursor].type<INFINITE)
          stepchild.loopData[cursor].type++;
        else
          stepchild.loopData[cursor].type = NORMAL;
      }
      else{
        if(stepchild.loopData[cursor].type>NORMAL)
          stepchild.loopData[cursor].type--;
        else
          stepchild.loopData[cursor].type = INFINITE;
      }
      stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
    }
    return true;
  }
};

void loopMenu(){
  LoopMenu menu;
  while(menu.loopMenuControls()){
    menu.displayMenu();
  }
}
