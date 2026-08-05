#include "Stepchild.h"


;
using namespace std;

void drawLittleQuantCubes(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  graphics.drawBox(13-(millis()/200)%4,(millis()/200)%4,8,8,3,3,4);
  graphics.drawBox(6+(millis()/200)%4,5-(millis()/200)%4,8,8,3,3,0);
}

Note getQuantizedNote(Note& n){
  int16_t moveAmount = 0;
  int16_t subDivOffset = n.startPos%stepchild.subDivision;
  //if offset is closer to the next sub div, move it to the right
  if(subDivOffset>stepchild.subDivision/2){
    moveAmount = stepchild.subDivision - subDivOffset;
  }
  else{
    moveAmount = -subDivOffset;
  }

  moveAmount = float(moveAmount) * float(stepchild.quantizeAmount)/100.0;

  //sloppy lil' bounds check (unthorough)
  if(n.endPos + moveAmount > stepchild.sequenceLength){
    moveAmount = stepchild.sequenceLength - n.endPos;
  }
  if(n.startPos + moveAmount < 0){
    moveAmount = -n.startPos;
  }

  Note newNote = n;
  newNote.setSelected(false);
  newNote.startPos += moveAmount;
  newNote.endPos += moveAmount;
  return newNote;
}


vector<NoteTrackPair> getQuantizedNotes(vector<NoteTrackPair> targetNotes){
  vector<NoteTrackPair> temp = {};
  for(NoteTrackPair n:targetNotes){
    temp.push_back(NoteTrackPair(getQuantizedNote(n.note),n.trackID));
  }
  //check to make sure no two notes have the same start on the same track
  vector<NoteTrackPair> notes = {};
  for(uint8_t i = 0; i<temp.size(); i++){
    bool collision = false;
    for(uint8_t j = 0; j<notes.size(); j++){
      if(j != i && temp[i].note.startPos == notes[j].note.startPos && notes[i].trackID == notes[j].trackID){
        collision = true;
      }
    }
    if(!collision){
      notes.push_back(temp[i]);
    }
  }
  return notes;
}

bool quantize(){
  //set up render settings
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawPram = false;
  settings.drawLoopFlags = false;
  settings.shrinkTopDisplay = false;

  //list of notes to apply fx to
  vector<NoteID> targetNoteIDs = {};
  vector<NoteTrackPair> targetNotes = {};
  vector<NoteTrackPair> previewNotes = {};
  if(stepchild.selectionCount){
    targetNoteIDs = getSelectedNoteIDs();
  }
  else if(stepchild.IDAtCursor()){
    targetNoteIDs.push_back(NoteID(stepchild.activeTrack,stepchild.IDAtCursor()));
  }
  //grab note objects
  for(NoteID n:targetNoteIDs){
    targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
  }

  //delete the targeted notes
  stepchild.deleteNotes_byID(targetNoteIDs);

  bool changed = true;

  //gen
  while(true){
    stepchild.buttons.readInputs();
    //change amount
    while(stepchild.buttons.counterA){
      if(stepchild.buttons.counterA > 0 && stepchild.quantizeAmount < 100){
        stepchild.quantizeAmount+=5;
        changed = true;
      }
      //changing subdivint
      else if(stepchild.buttons.counterA < 0 && stepchild.quantizeAmount > 0){
        stepchild.quantizeAmount-=5;
        changed = true;
      }
      stepchild.buttons.countDownA();
    }
    //change subdivision
    while(stepchild.buttons.counterB != 0){
      //if shifting, toggle between 1/3 and 1/4 mode
      if(stepchild.buttons.SHIFT()){
        stepchild.toggleTriplets();
        changed = true;
      }
      else if(stepchild.buttons.counterB >= 1){
        stepchild.changeSubDivInt(true);
        changed = true;
      }
      //changing subdivint
      else if(stepchild.buttons.counterB <= -1){
        stepchild.changeSubDivInt(false);
        changed = true;
      }
      stepchild.buttons.countDownB();
    }
    if(stepchild.itsbeen(200)){
      //cancelling
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        for(NoteTrackPair n:targetNotes){
          stepchild.makeNote(n.note,n.trackID,false);
        }
        return false;
      }
      //committing
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        for(NoteTrackPair n:previewNotes){
          stepchild.makeNote(n.note,n.trackID,false);
        }
        return true;
      }
      if(stepchild.buttons.UP() && stepchild.quantizeAmount < 100){
        stepchild.quantizeAmount += 5;
        stepchild.lastTime = millis();
        changed = true;
      }
      if(stepchild.buttons.DOWN() && stepchild.quantizeAmount > 0){
        stepchild.quantizeAmount -= 5;
        stepchild.lastTime = millis();
        changed = true;
      }
      if(stepchild.buttons.RIGHT()){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(stepchild.buttons.SHIFT()){
          stepchild.toggleTriplets();
          changed = true;
        }
        else{
          stepchild.changeSubDivInt(true);
          changed = true;
        }
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.LEFT()){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(stepchild.buttons.SHIFT()){
          stepchild.toggleTriplets();
          changed = true;
        }
        else{
          stepchild.changeSubDivInt(false);
          changed = true;
        }
        stepchild.lastTime = millis();
      }
    }
    if(changed){
      previewNotes = getQuantizedNotes(targetNotes);
    }

    stepchild.display.clearDisplay();
    graphics.printSmall(49,1,"$: "+stepchild.stepsToMeasures(stepchild.subDivision),1);
    graphics.drawButton(32,0,"X/B",1);

    graphics.drawButton(81,0,"M",1);
    graphics.printSmall(90,1,"to quit",1);

    graphics.printSmall(49,9,stringify(stepchild.quantizeAmount)+"%",1);
    graphics.drawButton(32,8,"Y/A",1);
    graphics.drawSeq(settings);
    for(NoteTrackPair n:previewNotes){
      graphics.drawNote(n.note,n.trackID,settings);
    }
    drawLittleQuantCubes(0,0,8,true);
    stepchild.display.display();
  }
  return false;
}
