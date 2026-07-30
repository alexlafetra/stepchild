#include "Stepchild.h"


;
using namespace std;

bool echoMenuControls(uint8_t* cursor);
void drawEchoMenu(uint8_t cursor);

vector<Note> echoNote(Note& n){
  vector<Note> newNotes = {};
  for(uint8_t i = stepchild.echoFXData.repeats; i > 0; i--){
    Note newNote = n;
    newNote.setSelected(false);
    uint16_t offset = i*stepchild.echoFXData.delay;
    if(newNote.endPos + offset > stepchild.sequenceLength)
      continue;
    newNote.shift(offset);
    newNote.velocity *= pow(float(stepchild.echoFXData.decay)/100.0,i);
    //if the note isn't so quiet it's muted
    if(newNote.velocity)
      newNotes.push_back(newNote);
  }
  return newNotes;
}

vector<NoteTrackPair> getEchoNotes(vector<NoteTrackPair> targetNotes){
  vector<NoteTrackPair> temp = {};
  //add the original notes first
  for(NoteTrackPair n:targetNotes){
    temp.push_back(NoteTrackPair(n.note,n.trackID));
  }
  for(NoteTrackPair n:targetNotes){
    vector<Note> newNotes = echoNote(n.note);
    for(Note newN:newNotes){
      temp.push_back(NoteTrackPair(newN,n.trackID));
    }
  }
  return temp;
}

#define MAX_ECHO_REPEATS 16

bool echo(){
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
    //change repeats
    while(stepchild.buttons.counterA){
      if(stepchild.buttons.counterA > 0 && stepchild.echoFXData.repeats < MAX_ECHO_REPEATS){
        stepchild.echoFXData.repeats++;
        changed = true;
      }
      //changing subdivint
      else if(stepchild.buttons.counterA < 0 && stepchild.echoFXData.repeats > 0){
        stepchild.echoFXData.repeats--;
        changed = true;
      }
      stepchild.buttons.countDownA();
    }
    //change subdivision
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
      stepchild.buttons.countDownB();
    }
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.UP() && stepchild.echoFXData.decay < 100){
        stepchild.lastTime = millis();
        changed = true;
        stepchild.echoFXData.decay += 5;
      }
      else if(stepchild.buttons.DOWN() && stepchild.echoFXData.decay > 0){
        stepchild.lastTime = millis();
        changed = true;
        stepchild.echoFXData.decay -= 5;
      }
    }
    if(stepchild.itsbeen(200)){
      //changing delay by adding/subbing a subdivision using the joystick
      if(stepchild.buttons.RIGHT()){
        if(stepchild.echoFXData.delay < 192-stepchild.subDivision){
          stepchild.echoFXData.delay+=stepchild.subDivision;
        }
        else{
          stepchild.echoFXData.delay = 192;
        }
        stepchild.lastTime = millis();
        changed = true;
      }
      if(stepchild.buttons.LEFT()){
        if(stepchild.echoFXData.delay > stepchild.subDivision){
          stepchild.echoFXData.delay-=stepchild.subDivision;
        }
        else{
          stepchild.echoFXData.delay = 0;
        }
        stepchild.lastTime = millis();
        changed = true;
      }
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
    }
    if(changed){
      previewNotes = getEchoNotes(targetNotes);
      stepchild.display.clearDisplay();
      graphics.printSmall(49,1,"$: "+stepchild.stepsToMeasures(stepchild.echoFXData.delay),1);
      graphics.drawButton(32,0,"X/B",1);
  
      graphics.drawButton(81,0,"M",1);
      graphics.printSmall(90,1,"to quit",1);
  
      graphics.printSmall(41,9,stringify(stepchild.echoFXData.decay)+"%",1);
      graphics.drawButton(32,8,"Y",1);

      graphics.printSmall(66,9,"#:"+stringify(stepchild.echoFXData.repeats),1);
      graphics.drawButton(57,8,"A",1);

      graphics.drawEchoIcon(8,0,12,true);

      graphics.drawSeq(settings);
      for(NoteTrackPair n:previewNotes){
        graphics.drawNote(n.note,n.trackID,settings);
      }
      stepchild.display.display();
    }
  }
  return false;
}
