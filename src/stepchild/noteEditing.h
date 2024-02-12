//Notes ------------------------------------------------------------------------
//truncates any note in this position
void truncateNote(int track, int time) {
  int id = lookupData[track][time];
  if(id == 0 || id >= seqData[track].size())
    return;
  //if the note is only 1 step long, j delete it
  if(seqData[track][id].endPos == seqData[track][id].startPos+1){
    deleteNote_byID(track,id);
    return;
  }
  for(uint16_t i = time; i<seqData[track][id].endPos; i++){
    lookupData[track][i] = 0;
  }
  seqData[track][id].endPos = time;
}

//Deletes a note
void deleteNote_byID(int track, int targetNoteID){
  //if there's a note/something here, and it's in data
  if (targetNoteID != 0 && targetNoteID <= seqData[track].size()) {
    //clearing note from lookupData
    for (int i = seqData[track][targetNoteID].startPos; i < seqData[track][targetNoteID].endPos; i++) {
      if (lookupData[track][i] != 0) {
        lookupData[track][i] = 0;
      }
    }
    //lowering selectionCount
    if(seqData[track][targetNoteID].isSelected){
      selectionCount--;
    }
    //erasing note from seqData
    //make a copy of the seqData[track] vector which excludes the note
    //hopefully, this does a better job of freeing memory
    //swapping it like this! this is so the memory is free'd up again
    vector<Note> temp = {Note()};
    for(int i = 1; i<=seqData[track].size()-1; i++){
      if(i != targetNoteID){//if it's not the target note, or an empty spot, copy it to the temp vector
        temp.push_back(seqData[track][i]);
      }
    }
    temp.swap(seqData[track]);
    //since we deleted it from seqData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the deleted note's ID should be decremented.
    if(seqData[track].size()-1>0){
      for (uint16_t step = 0; step < lookupData[track].size(); step++) {
        if (lookupData[track][step] > targetNoteID) { //if there's a higher note and if there are still notes to be changed
          lookupData[track][step] -= 1;
        }
      }
    }
  }
}

void deleteNote(int track, int time) {
  deleteNote_byID(track, lookupData[track][time]);
}

void deleteNote() {
  deleteNote(activeTrack, cursorPos);
}


void deleteSelected(){
  if(selectionCount>0){
    if(binarySelectionBox(64,32,"nah","yea","delete "+stringify(selectionCount)+((selectionCount == 1)?stringify(" note?"):stringify(" notes?")),drawSeq)){
      for(uint8_t track = 0; track<trackData.size(); track++){
        for(uint16_t note = 0; note<seqData[track].size(); note++){
          if(seqData[track][note].isSelected){
            deleteNote_byID(track, note);
            (note == 0) ? note = 0: note--;
          }
          if(selectionCount == 0)
            break;
        }
      }
    }
  }
  updateLEDs();
}

void makeNote(Note newNoteOn, int track, bool loudly){
  //if you're placing it on the end of the seq, just return
  if(newNoteOn.startPos == seqEnd)
    return;
  //if there's a 0 where the note is going to go, or if there's not a zero BUT it's also not the start of that other note
  if (lookupData[track][newNoteOn.startPos] == 0 || newNoteOn.startPos != seqData[activeTrack][lookupData[activeTrack][newNoteOn.startPos]].startPos) { //if there's no note there
    if (lookupData[track][newNoteOn.startPos] != 0) {
      truncateNote(track, newNoteOn.startPos);
    }
    if(newNoteOn.isSelected){
      selectionCount++;
    }
    unsigned short int id = seqData[track].size();
    lookupData[track][newNoteOn.startPos] = id;//set noteID in lookupData to the index of the new note
    for (int i = newNoteOn.startPos+1; i < newNoteOn.endPos; i++) { //sets id
      if (lookupData[track][i] == 0 && i<seqEnd)
        lookupData[track][i] = id;
      else { //if there's something there, then set the end to the step before it
        newNoteOn.endPos = i;
        break;
      }
    }
    seqData[track].push_back(newNoteOn);
    if (loudly) {
      MIDI.noteOn(trackData[track].pitch, newNoteOn.velocity, trackData[track].channel);
      MIDI.noteOff(trackData[track].pitch, 0, trackData[track].channel);
    }
  }
}
void makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
  makeNote(newNoteOn,track,loudly);
}

void makeNote(Note newNoteOn, int track){
  makeNote(newNoteOn,track,false);
}

void makeNote(int track, int time, int length, int velocity, int chance, bool loudly){
  makeNote(track,time,length,velocity,chance,false,false,loudly);
}
//this one is for quickly placing a ntoe at the cursor, on the active track
void makeNote(int track, int time, int length, bool loudly) {
  Note newNote(time,time+length,defaultVel,100,false,false);
  makeNote(newNote,track,loudly);
}

void makeNote(int length, bool loudly) {
  makeNote(activeTrack, cursorPos, length, defaultVel, 100, false, false, loudly);
}

//draws notes every "count" subDivs, from viewStart to viewEnd
void stencilNotes(uint8_t count){
  for(uint16_t i = viewStart; i<viewEnd; i+=(subDivInt*count)){
    if(lookupData[activeTrack][i] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos)
      makeNote(activeTrack, i, subDivInt*count+1, defaultVel, 100, false);
  }
}

void toggleNote(int track, int time, int length){
  if(lookupData[track][time] == 0 || (lookupData[track][time] != 0 && seqData[track][lookupData[track][time]].startPos != time)){
    if(!playing && !recording)
      makeNote(track, time, length, true);
    else
      makeNote(track, time, length, false);
  }
  else if(time == seqData[track][lookupData[track][time]].startPos){
    deleteNote(track, time);
  }
  updateLEDs();
}


void changeVel(int amount){
  if(selectionCount>0){
    changeVelSelected(amount);
    //see "changeChance() for explanation"
    if(!seqData[activeTrack][lookupData[activeTrack][cursorPos]].isSelected){
      changeVel(lookupData[activeTrack][cursorPos], activeTrack, amount);
    }
  }
  else
    changeVel(lookupData[activeTrack][cursorPos], activeTrack, amount);
}

void changeVelSelected(int amount){
  for(int track = 0; track<trackData.size(); track++){
    for(int note = seqData[track].size()-1; note>0; note--){
      if(seqData[track][note].isSelected){
        changeVel(note,track, amount);
      }
    }
  }
}

void changeVel(int id, int track, int amount) {
  int vel = seqData[track][id].velocity;
  vel += amount;
  if(vel>=127)
    seqData[track][id].velocity = 127;
  else if(vel<0)
    seqData[track][id].velocity = 0;
  else
    seqData[track][id].velocity = vel;
}

void changeChance(int amount){
  if(selectionCount>0){
    changeChanceSelected(amount);
    //if the cursor note isn't selected
    //it still needs to be changed (but don't change it twice!)
    if(!seqData[activeTrack][lookupData[activeTrack][cursorPos]].isSelected)
      changeChance(lookupData[activeTrack][cursorPos],activeTrack,amount);
  }
  else{
    changeChance(lookupData[activeTrack][cursorPos],activeTrack,amount);
  }
}

void changeChanceSelected(int amount){
  for(uint16_t track = 0; track<trackData.size(); track++){
    for(uint16_t note = seqData[track].size()-1; note>0; note--){
      if(seqData[track][note].isSelected){
        changeChance(note,track, amount);
      }
    }
  }
}

void changeChance(int noteID, int track, int amount){
  int8_t chance = seqData[track][noteID].chance;
  chance += amount;
  if(chance>=100)
    seqData[track][noteID].chance = 100;
  else if(chance<0)
    seqData[track][noteID].chance = 0;
  else
    seqData[track][noteID].chance = chance;
}


//Add time/remove time should be changed to one "add/remove" time function
//It would be nice if this could be accessed from somewhere other than the settings menu?
//insert or cut time from the seq
//cursor+new inserts time
//cursor+del deletes time
//selbox+new duplicates time
//selbox+del deletes time
//selbox stays until new selbox is made
void cutInsertTime(){
  while(true){
    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        return;
      }
    }
  }
}
//adds an amount of time to the sequence (used in the 'SEQUENCE' submenu)
void addTimeToSeq(uint16_t amount, uint16_t insertPoint){
  //if it'll cause a wrap around
  if(seqEnd+amount>65535)
    return;
  //extend seq
  seqEnd+=amount;

  //move through notes that appear AFTER the insert point and move them back
  //sweeping from old seq end (there should be no notes after that)-->insertPoint
  for(uint8_t t = 0; t<trackData.size(); t++){
    //extend lookupdata
    lookupData[t].resize(seqEnd,0);
    for(uint16_t i = seqEnd-amount; i>insertPoint; i--){
      uint16_t id = lookupData[t][i];
      //if there's a note there
      if(id != 0){
        //if it starts after the insert point, move the whole note
        if(seqData[t][id].startPos>insertPoint){
          //set i to old beginning of note (so you definitely don't hit it twice)
          i = seqData[t][id].startPos;
          moveNote(id,t,t,seqData[t][id].startPos+amount);
        }
        //if the insert point intersects it somehow, truncate it
        else{
          truncateNote(t,insertPoint);
          //break bc that was the last note
          break;
        }
      }
    }
  }
}

void removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
  //if you're trying to delete more than (or as much as) exists! just return
  if(amount>=seqEnd)
    return;
  //if you're trying to delete more than exists between insertpoint --> seqEnd,
  //then set amount to everything there
  if(insertPoint+amount>seqEnd){
    amount = seqEnd-insertPoint;
  }
  //move through 'deleted' area and clear out notes
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there are no notes, skip this track
    if(seqData[t].size() == 1)
      continue;
    for(uint16_t i = insertPoint+1; i<insertPoint+amount; i++){
      uint16_t id = lookupData[t][i];
      //if there's a note there
      if(id != 0){
        //if it starts before/at insert point, truncate it
        if(seqData[t][id].startPos<=insertPoint){
          truncateNote(t,insertPoint);
          //break bc you know that was the last note
          break;
        }
        //if it's in the delete area, delete it
        else if(seqData[t][id].startPos>insertPoint && seqData[t][id].startPos<=insertPoint+amount){
          deleteNote_byID(t,id);
        }
      }
    }
  }
  //move notes that fall beyond the delete area
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there're no notes, skip this track
    if(seqData[t].size() == 1)
      continue;
    for(uint16_t i = insertPoint+amount; i<seqEnd; i++){
      uint16_t id = lookupData[t][i];
      //if there's a note here, move it back by amount
      if(id != 0){
        moveNote(id,t,t,seqData[t][id].startPos-amount);
        i = seqData[t][id].endPos-1;
      }
    }
    //resize lookupData
    lookupData[t].resize(seqEnd-amount);
  }
  seqEnd -= amount;
  //fixing loop point positions
  for(uint8_t loop = 0; loop<loopData.size(); loop++){
    //if start or end are past seqend, set to seqend
    if(loopData[loop].start>seqEnd)
      loopData[loop].start = seqEnd;
    if(loopData[loop].end>seqEnd)
      loopData[loop].end = seqEnd;
  }
  //make sure view stays within seq
  // if(viewEnd > seqEnd){
  //   viewEnd = seqEnd;
  // }
  checkView();
}