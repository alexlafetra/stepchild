void drawLittleQuantCubes(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  graphics.drawBox(13-(millis()/200)%4,(millis()/200)%4,8,8,3,3,4);
  graphics.drawBox(6+(millis()/200)%4,5-(millis()/200)%4,8,8,3,3,0);
}

Note getQuantizedNote(Note& n){
  int16_t moveAmount = 0;
  int16_t subDivOffset = n.startPos%sequence.subDivision;
  //if offset is closer to the next sub div, move it to the right
  if(subDivOffset>sequence.subDivision/2){
    moveAmount = sequence.subDivision - subDivOffset;
  }
  else{
    moveAmount = -subDivOffset;
  }

  moveAmount = float(moveAmount) * float(quantizeAmount)/100.0;

  //sloppy lil' bounds check (unthorough)
  if(n.endPos + moveAmount > sequence.sequenceLength){
    moveAmount = sequence.sequenceLength - n.endPos;
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
  //check to make sure no two notes have the same start
  vector<NoteTrackPair> notes = {};
  for(uint8_t i = 0; i<temp.size(); i++){
    bool collision = false;
    for(uint8_t j = 0; j<notes.size(); j++){
      if(j != i && temp[i].note.startPos == notes[j].note.startPos){
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
    //change amount
    while(controls.counterA){
      if(controls.counterA > 0 && quantizeAmount < 100){
        quantizeAmount+=5;
        changed = true;
      }
      //changing subdivint
      else if(controls.counterA < 0 && quantizeAmount > 0){
        quantizeAmount-=5;
        changed = true;
      }
      controls.countDownA();
    }
    //change subdivision
    while(controls.counterB != 0){
      //if shifting, toggle between 1/3 and 1/4 mode
      if(controls.SHIFT()){
        sequence.toggleTriplets();
        changed = true;
      }
      else if(controls.counterB >= 1){
        sequence.changeSubDivInt(true);
        changed = true;
      }
      //changing subdivint
      else if(controls.counterB <= -1){
        sequence.changeSubDivInt(false);
        changed = true;
      }
      controls.countDownB();
    }
    if(utils.itsbeen(200)){
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
      if(controls.UP() && quantizeAmount < 100){
        quantizeAmount += 5;
        lastTime = millis();
        changed = true;
      }
      if(controls.DOWN() && quantizeAmount > 0){
        quantizeAmount -= 5;
        lastTime = millis();
        changed = true;
      }
      if(controls.RIGHT()){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(controls.SHIFT()){
          sequence.toggleTriplets();
          changed = true;
        }
        else{
          sequence.changeSubDivInt(true);
          changed = true;
        }
        lastTime = millis();
      }
      else if(controls.LEFT()){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(controls.SHIFT()){
          sequence.toggleTriplets();
          changed = true;
        }
        else{
          sequence.changeSubDivInt(false);
          changed = true;
        }
        lastTime = millis();
      }
    }
    if(changed){
      previewNotes = getQuantizedNotes(targetNotes);
    }

    display.clearDisplay();
    printSmall(49,1,"$: "+stepsToMeasures(sequence.subDivision),1);
    graphics.drawButton(32,0,"X/B",1);

    graphics.drawButton(81,0,"M",1);
    printSmall(90,1,"to quit",1);

    printSmall(49,9,stringify(quantizeAmount)+"%",1);
    graphics.drawButton(32,8,"Y/A",1);
    drawSeq(settings);
    for(NoteTrackPair n:previewNotes){
      drawNote(n.note,n.trackID,settings);
    }
    drawLittleQuantCubes(0,0,8,true);
    display.display();
  }
  return false;
}
