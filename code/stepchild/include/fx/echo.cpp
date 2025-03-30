bool echoMenuControls(uint8_t* cursor);
void drawEchoMenu(uint8_t cursor);

vector<Note> echoNote(Note& n){
  vector<Note> newNotes = {};
  for(uint8_t i = echoData.repeats; i > 0; i--){
    Note newNote = n;
    newNote.setSelected(false);
    uint16_t offset = i*echoData.delay;
    if(newNote.endPos + offset > sequence.sequenceLength)
      continue;
    newNote.shift(offset);
    newNote.velocity *= pow(float(echoData.decay)/100.0,i);
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
  if(sequence.selectionCount){
    targetNoteIDs = getSelectedNoteIDs();
  }
  else if(sequence.IDAtCursor()){
    targetNoteIDs.push_back(NoteID(sequence.activeTrack,sequence.IDAtCursor()));
  }
  //grab note objects
  for(NoteID n:targetNoteIDs){
    targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
  }

  //delete the targeted notes
  sequence.deleteNotes_byID(targetNoteIDs);

  bool changed = true;

  //gen
  while(true){
    controls.readInputs();
    //change repeats
    while(controls.counterA){
      if(controls.counterA > 0 && echoData.repeats < MAX_ECHO_REPEATS){
        echoData.repeats++;
        changed = true;
      }
      //changing subdivint
      else if(controls.counterA < 0 && echoData.repeats > 0){
        echoData.repeats--;
        changed = true;
      }
      controls.countDownA();
    }
    //change subdivision
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
      controls.countDownB();
    }
    if(utils.itsbeen(100)){
      if(controls.UP() && echoData.decay < 100){
        lastTime = millis();
        changed = true;
        echoData.decay += 5;
      }
      else if(controls.DOWN() && echoData.decay > 0){
        lastTime = millis();
        changed = true;
        echoData.decay -= 5;
      }
    }
    if(utils.itsbeen(200)){
      //changing delay by adding/subbing a subdivision using the joystick
      if(controls.RIGHT()){
        if(echoData.delay < 192-sequence.subDivision){
          echoData.delay+=sequence.subDivision;
        }
        else{
          echoData.delay = 192;
        }
        lastTime = millis();
        changed = true;
      }
      if(controls.LEFT()){
        if(echoData.delay > sequence.subDivision){
          echoData.delay-=sequence.subDivision;
        }
        else{
          echoData.delay = 0;
        }
        lastTime = millis();
        changed = true;
      }
      //cancelling
      if(controls.MENU()){
        lastTime = millis();
        for(NoteTrackPair n:targetNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return false;
      }
      //committing
      if(controls.NEW()){
        lastTime = millis();
        for(NoteTrackPair n:previewNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return true;
      }
    }
    if(changed){
      previewNotes = getEchoNotes(targetNotes);
      display.clearDisplay();
      printSmall(49,1,"$: "+stepsToMeasures(echoData.delay),1);
      graphics.drawButton(32,0,"X/B",1);
  
      graphics.drawButton(81,0,"M",1);
      printSmall(90,1,"to quit",1);
  
      printSmall(41,9,stringify(echoData.decay)+"%",1);
      graphics.drawButton(32,8,"Y",1);

      printSmall(66,9,"#:"+stringify(echoData.repeats),1);
      graphics.drawButton(57,8,"A",1);

      graphics.drawEchoIcon(8,0,12,true);

      drawSeq(settings);
      for(NoteTrackPair n:previewNotes){
        drawNote(n.note,n.trackID,settings);
      }
      display.display();
    }
  }
  return false;
}
