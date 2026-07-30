#include "Stepchild.h"

#include "mainSequence.h"
#include "guiUtilities.h"

;

using namespace std;

//Fit 'A' into 'B'

// 'into', 11x13px
const unsigned char into_bmp [] = {
	0xe0, 0x00, 0x42, 0x00, 0x47, 0x00, 0xe9, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x20, 0x01, 0xe0, 
	0x00, 0x20, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x20, 0x00, 0xc0
};

void drawWarpIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  w--;
  graphics.drawDottedRect(x1,y1,w,w,2);
  if(anim)
    stepchild.display.fillRect(x1,y1,(millis()/100)%(w)+2,(millis()/100)%(w)+2,SSD1306_WHITE);
  else
    stepchild.display.fillRect(x1,y1,w/2,w/2,SSD1306_WHITE);
}

CoordinatePair selectArea_warp(bool AorB){
  CoordinatePair coords;
  coords.start.x = 0;
  coords.end.x = 0;
  coords.start.y = 0;
  coords.end.y = 0;
  while(true){
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    defaultEncoderControls();
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
      stepchild.selectionBox.begun = true;
      stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
      coords.start.x = stepchild.cursorPos;
      coords.start.y = stepchild.activeTrack;
    }
    //if stepchild.buttons.SELECT()  is released, and there's a selection box
    if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
      stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
      stepchild.selectionBox.begun = false;
      coords.end.x = stepchild.cursorPos;
      coords.end.y = stepchild.activeTrack;
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        return coords;
      }
      if(stepchild.buttons.MENU()){
        coords.start.x = 0;
        coords.end.x = 0;
        coords.start.y = 0;
        coords.end.y = 0;
        stepchild.lastTime = millis();
        return coords;
      }
    }
    if (stepchild.itsbeen(100)) {
      if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(stepchild.cursorPos%stepchild.subDivision){
          stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveCursor(-stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
      if (stepchild.buttons.joystickX == -1 && !stepchild.buttons.SHIFT()) {
        if(stepchild.cursorPos%stepchild.subDivision){
          stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveCursor(stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
      if (stepchild.buttons.joystickY == 1) {
        if(stepchild.recording())
          stepchild.setActiveTrack(stepchild.activeTrack + 1, false);
        else
          stepchild.setActiveTrack(stepchild.activeTrack + 1, true);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickY == -1) {
        if(stepchild.recording())
          stepchild.setActiveTrack(stepchild.activeTrack - 1, false);
        else
          stepchild.setActiveTrack(stepchild.activeTrack - 1, true);
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.itsbeen(50)) {
      if (stepchild.buttons.joystickX == 1 && stepchild.buttons.SHIFT()) {
        stepchild.moveCursor(-1);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickX == -1 && stepchild.buttons.SHIFT()) {
        stepchild.moveCursor(1);
        stepchild.lastTime = millis();
      }
    }
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawPram = false;
    settings.topLabels = false;
    settings.shrinkTopDisplay = false;
    graphics.drawSeq(settings);
    drawCoordinateBox(coords,settings);
    if(coords.start.x == 0 && coords.end.x == 0 && coords.start.y == 0 && coords.end.y == 0){
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH,0,"warp",1);
      if(AorB){
        if(millis()%500>250)
          graphics.printItalic(stepchild.TRACK_LABEL_WIDTH+16,0,"A",1);
        graphics.printItalic(stepchild.TRACK_LABEL_WIDTH+42,0,"B",1);
      }
      else{
        graphics.printItalic(stepchild.TRACK_LABEL_WIDTH+16,0,"A",1);
        if(millis()%500>250)
          graphics.printItalic(stepchild.TRACK_LABEL_WIDTH+42,0,"B",1);
      }
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH+25,0,"into",1);
    }
    else{
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH,0,"[n] to submit",1);
      if(millis()%500>250)
        graphics.printItalic(stepchild.TRACK_LABEL_WIDTH+50,0,AorB?"A":"B",1);
    }
    drawWarpIcon(8,2,11,true);
    stepchild.display.display();
  }
}

//Warps the notes contained within the 'A' coordinates into the 'B' coordinates
bool warpAintoB(CoordinatePair A, CoordinatePair B, bool onlySelected){
  //checking bounds
  if(A.start.x == A.end.x || B.start.x == B.end.x){
      return false;
  }
  if(A.start.x>A.end.x){
      uint16_t temp = A.end.x;
      A.end.x = A.start.x;
      A.start.x = temp;
  }
  if(B.start.x>B.end.x){
      uint16_t temp = B.end.x;
      B.end.x = B.start.x;
      B.start.x = temp;
  }

  //this is the scale of the two areas
  float scaleFactor = float(abs(B.end.x-B.start.x))/float(abs(A.end.x-A.start.x));

  //you need to multiply every note length by that value
  //and also:
  //set the start of the note to be relative
  //to  the start of coordinatePair B * the scaleFactor

  vector<NoteTrackPair> newNotes;
  for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
      for(uint8_t noteID = 1; noteID<stepchild.noteData[track].size(); noteID++){
          //if the note starts SOMEWHERE within A, warp it!
          //you might want to change this/add an option
          //to ONLY warp notes that are entirely contained within A
          if(stepchild.noteData[track][noteID].startPos>=A.start.x && stepchild.noteData[track][noteID].startPos<A.end.x && ((onlySelected && stepchild.noteData[track][noteID].isSelected()) || !onlySelected)){
              Note targetNote = stepchild.noteData[track][noteID];
              uint16_t oldLength = targetNote.getLength();
              uint16_t distanceFromStartOfA = targetNote.startPos - A.start.x;

              Note newNote = targetNote;
              newNote.startPos = distanceFromStartOfA*scaleFactor+B.start.x;
              newNote.endPos = newNote.startPos+(oldLength-1)*scaleFactor;
              //if the note will be less than 1 timestep long, don't warp it
              if(newNote.endPos<=newNote.startPos+1){
                continue;
              }

              //stepchild.buttons.DELETE()eting old note
              stepchild.deleteNote_byID(track,noteID);
              //make sure to decrement noteID! so you don't warp the same note twice or skip a note
              noteID--;

              // stepchild.makeNote(newNote,track,false);
              // newNote.push_back(newNote);
              NoteTrackPair newNT(newNote,track);
              newNotes.push_back(newNT);
          }
      }
  }
  //iterating over the newNotes vector and making each note
  for(uint8_t note = 0; note<newNotes.size(); note++){
    stepchild.makeNote(newNotes[note].note,newNotes[note].trackID,false);
  }
  return true;
}

bool warp(){
  CoordinatePair A;
  CoordinatePair B;
  bool atLeastOnce = false;
  //0 is A, 1 is B, 2 is warp
  while(true){
    A = selectArea_warp(true);
    if(A.isVertical())
      break;
    else
      atLeastOnce = true;
    B = selectArea_warp(false);
    if(B.isVertical())
      break;
    else
      atLeastOnce = true;
    //only warps selected notes, if any are selected
    if(!warpAintoB(A,B,stepchild.selectionCount != 0)){
      break;
    }
    else
      atLeastOnce = true;
  }
  return atLeastOnce;
}
