#include "sequence.h"

void StepchildSequence::writeNoteOn(uint16_t step, uint8_t pitch, uint8_t vel, uint8_t channel){
  uint8_t trackID = makeTrackWithPitch(pitch,channel);
  if(trackData[trackID].isPrimed()){
    Note newNote(step, step, vel);//this constuctor sets the endPos of the note at the same position
    if(liveLooping()){
      newNote.setSelected(true);
      selectionCount++;
    }
    if(lookupTable[trackID][step] != 0){
      deleteNote(trackID,step);
    }
    noteData[trackID].push_back(newNote);
    lookupTable[trackID][step] = noteData[trackID].size()-1;
    trackData[trackID].noteLastSent = pitch;
  }
}


void StepchildSequence::writeNoteOff(uint16_t step, uint8_t pitch, uint8_t channel){
  int8_t track = getTrackWithPitch(pitch,channel);
  if(track == -1)
    return;
  if(trackData[track].isPrimed() && trackData[track].noteLastSent != 255){
    unsigned short int note = noteData[track].size()-1;
    //if the track actually was sending, and exists
    if(trackData[track].noteLastSent != 255 && track != -1){
      noteData[track][note].endPos = step;
      trackData[track].noteLastSent = 255;
    }
  }
}

void StepchildSequence::writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value){
  for(uint8_t dt = 0; dt < autotrackData.size(); dt++){
    //if the track is primed and is recording externally
    if(autotrackData[dt].isPrimed && autotrackData[dt].recordFrom == EXTERNAL_MIDI){
      //if the channel and control number match
      if(channel == autotrackData[dt].channel && controller == autotrackData[dt].control){
        autotrackData[dt].data[step] = value;
      }
    }
  }
}

void StepchildSequence::continueStep(uint16_t step){
  for(uint8_t track = 0; track<trackData.size(); track++){
    if(trackData[track].isPrimed()){
      if(trackData[track].noteLastSent != 255){
        uint16_t id = noteData[track].size()-1;
        if(id>0){
          //if there's a different note at this step, del it
          if(lookupTable[track][step] != 0 && lookupTable[track][step] != id){
            //if it's in overwrite mode, del the conflicting note
            if(overwriteRecording){
              deleteNote(track, step);
              id = noteData[track].size()-1;
            }
            //if it's not in overwrite mode, then end the note 
            else{
              noteData[track][id].endPos = step;
              trackData[track].noteLastSent = 255;
              continue;
            }
          }
          lookupTable[track][step] = id;
          noteData[track][id].endPos = step;
        }
      }
      //if it's not being written to, clear this step out
      else if(overWriteNotesWithEmptiness){
        //if there's something there, and the track isn't sending (make sure it's not a tail note that just got written)
        //AND make sure that the track is primed! if not, then don't overwrite it
        if(lookupTable[track][step] != 0){
          //if it's not the end of the note (since that note might have been written)
          //i don't think this will cause problems? 
          if(noteData[track][lookupTable[track][step]].endPos != step){
            deleteNote_byID(track, lookupTable[track][step]);
          }
        }
      }
    }
    //if the track isn't primed, play it normally
    else if(!trackData[track].isPrimed()){
      playTrack(track,step);
    }
  }
}

void StepchildSequence::updateLookupData(){
  for(uint8_t track = 0; track<trackData.size(); track++){
    lookupTable[track].assign(sequenceLength,0);//blank the track
    for(uint16_t id = 1; id<noteData[track].size()-1+1; id++){//for each note in data
      uint16_t start = noteData[track][id].startPos;
      uint16_t end = noteData[track][id].endPos;
      for(uint16_t i = start; i<end; i++){
        lookupTable[track][i] = id;
      }
    }
  }
}

void StepchildSequence::cleanupRecording(uint16_t stopTime){
  for(int8_t i = 0; i<trackData.size(); i++){
    //if there's a note on this track
    if(noteData[i].size()-1>0){
      //if the track was sending
      if(trackData[i].noteLastSent != 255){
        //set the end of the note to the stop time
        noteData[i][noteData[i].size()-1].endPos = stopTime;
        trackData[i].noteLastSent = 255;
      }
      for(uint16_t note = 1; note<noteData[i].size(); note++){
        //if the note is fucked up
        if(noteData[i][note].startPos>=noteData[i][note].endPos){
          noteData[i][note].endPos = noteData[i][note].startPos+1;
        }
      }
    }
  }
  updateLookupData();
}

void StepchildSequence::recordingLoop(){
  MIDI.read();
  if(sequenceClock.clockSource == INTERNAL_CLOCK){
    if(sequenceClock.hasItBeenEnoughTime(recheadPos)){
      sequenceClock.timeLastStepPlayed = micros();
      checkAutotracks();
      //if it's not in wait mode, or if it is but a note has been received
      if(!waitForNoteBeforeRec || !waitingToReceiveANote){
        continueStep(recheadPos);
        MIDI.sendClock();
        recheadPos++;
        checkLoop();
      }
    }
  }
  else if(sequenceClock.clockSource == EXTERNAL_CLOCK){
    if(sequenceClock.receivedClockMessage && startedPlaying){
      sequenceClock.receivedClockMessage = false;
      continueStep(recheadPos);
      recheadPos++;
      checkLoop();
      checkAutotracks();
    }
  }
}


void StepchildSequence::checkLoop(){
  if(playing()){
    if (playheadPos > loopData[activeLoop].end-1) { //if the timestep is past the end of the loop, loop it to the start
      loopCount++;
      if(loopCount > loopData[activeLoop].reps){
        nextLoop();
        //reset all the superposition flags
        for(uint8_t t = 0; t<trackData.size(); t++){
          for(uint16_t n = 1; n<noteData[t].size(); n++){
            noteData[t][n].setSuperpositioned(false);
          }
        }
      }
        playheadPos = loopData[activeLoop].start;
      if(!isLooping)
        togglePlay();
    }
  }
  else if(recording()){
    //one-shot record to current loop, without looping
    if(recMode == ONESHOT){
      if(recheadPos>=loopData[activeLoop].end){
        toggleRecording(waitForNoteBeforeRec);
      }
    }
    //record to one loop over and over again
    else if(recMode == CURRENT_LOOP){
      if(recheadPos>=loopData[activeLoop].end){
        recheadPos = loopData[activeLoop].start;
      }
    }
    //record to loops as they play in sequence
    else if(recMode == LOOP_SEQUENCE){
      if(recheadPos>=loopData[activeLoop].end){
        cutLoop();
        loopCount++;
        if(loopData[activeLoop].reps>=loopCount){
          nextLoop();
        }
        recheadPos = loopData[activeLoop].start;
      }
    }
  }
}


void StepchildSequence::playingLoop(){
  //internal timing
  if(sequenceClock.clockSource == INTERNAL_CLOCK){
    if(sequenceClock.hasItBeenEnoughTime(playheadPos)){
      MIDI.sendClock();
      playStep(playheadPos);
      playheadPos++;
      checkLoop();
      if(!playheadPos%24)
        CV.writeClock();
    }
  }
  //external timing
  else if(sequenceClock.clockSource == EXTERNAL_CLOCK){
    MIDI.read();
    if(sequenceClock.receivedClockMessage && startedPlaying){
      sequenceClock.receivedClockMessage = false;
      playStep(playheadPos);
      playheadPos += 1;
      checkLoop();
      // checkFragment();
    }
  }
}

void StepchildSequence::checkAutotracks(){
  if(recordingToAutotrack){
    int newVal = 64;
    switch(autotrackData[activeAutotrack].recordFrom){
      //recording externally, so get outta this loop!
      case EXTERNAL_MIDI:
        return;
      //rec from encoder A
      case ENCODER_A:
        if(controls.counterA>127){
          controls.counterA = 127;
        }
        if(controls.counterA<0)
          controls.counterA = 0;
        newVal = controls.counterA;
        break;
      //rec from encoder B
      case ENCODER_B:
        if(controls.counterB>127)
          controls.counterB = 127;
        if(controls.counterB<0)
          controls.counterB = 0;
        newVal = controls.counterB;
        break;
      //rec from joystick X
      case JOY_X:
        newVal = controls.getJoyX();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
      //rec from joystick Y
      case JOY_Y:
        newVal = controls.getJoyY();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
    }
    //bounds checking the new value before we write it to the DT
    if(newVal>127)
      newVal = 127;
    else if(newVal<0)
      newVal = 0;
    if(waitingToReceiveANote){
      return;
    }
    recentCC.val = newVal;
    recentCC.cc = autotrackData[activeAutotrack].control;
    recentCC.channel = autotrackData[activeAutotrack].channel;
    autotrackData[activeAutotrack].data[recheadPos] = newVal;
  }
}

void StepchildSequence::arpLoop(){
  //if it was active, but hadn't started playing yet
  if(!arp.playing){
    switch(arp.source){
      case NOTES_FROM_MIDI_INPUT:
        if(receivedNotes.notes.size()>0)
          arp.start();
        break;
      case NOTES_FROM_SEQUENCE:
        if(sentNotes.notes.size()>0)
          arp.start();
        break;
      case NOTES_FROM_SEQUENCE_AND_MIDI_INPUT:
        if(sentNotes.notes.size()>0 || receivedNotes.notes.size()>0)
          arp.start();
        break;
    }
  }
  if(arp.playing){
    //if the arp isn't latched and there are no notes for it
    if(!arp.holding  && ((arp.source == NOTES_FROM_MIDI_INPUT && !receivedNotes.notes.size()) || (arp.source == NOTES_FROM_SEQUENCE && !sentNotes.notes.size()))){
      arp.stop();
    }
    //if it IS latched or there are notes for it, then continue
    else if(arp.hasItBeenEnoughTime()){
      arp.playstep();
    }
  }
}


void StepchildSequence::defaultLoop(){
  playheadPos = loopData[activeLoop].start;
  recheadPos = loopData[activeLoop].start;
  MIDI.read();
}

void StepchildSequence::stop() {
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].noteLastSent != 255){
      MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      trackData[track].noteLastSent = 255;
      //reset superposition flags
      for(uint8_t i = 1; i<noteData[track].size(); i++){
        noteData[track][i].setSuperpositioned(false);
      }
    }
    else{
      MIDI.noteOff(trackData[track].pitch, 0, trackData[track].channel);
    }

  }
  sentNotes.clear();
}

void StepchildSequence::playStep(uint16_t timestep) {
  playPCData(timestep);
  //playing each track
  for (uint8_t track = 0; track < trackData.size(); track++) {
    //if it's unmuted or solo'd, play it
    if(!trackData[track].isMuted() || trackData[track].isSolo())
      playTrack(track,timestep);
  }
  //playing autotracks too
  for(uint8_t dT = 0; dT < autotrackData.size(); dT++){
    autotrackData[dT].play(timestep);
  }
  CV.check();
}


void StepchildSequence::playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (lookupTable[track][timestep] == 0){
    if(trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      if(!arp.isActive || arp.source == NOTES_FROM_MIDI_INPUT)//if the arp is off, or if it's just listening to notes from outside the seq
        MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      sentNotes.subNote(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
      triggerAutotracks(track,false);
    }
    return;
  }
  //if there's a note there
  else{
    playNote(noteData[track][lookupTable[track][timestep]],track, timestep);
  }
}

void StepchildSequence::playNote(Note& note, uint8_t track, uint16_t timestep){
  //if it's the start of the note, or if the track wasn't sending already
  if(timestep == note.startPos || trackData[track].noteLastSent == 255){ //if it's the start
    //if it's not muted
    if(!note.isMuted()){
      //if the track was already sending a note, send note off
      if(trackData[track].noteLastSent != 255){
        if(!arp.isActive || arp.source == NOTES_FROM_MIDI_INPUT)
          MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
        trackData[track].noteLastSent = 255;
        triggerAutotracks(track,false);
      }

      //modifying chance value and pitch value and vel
      int16_t chance = note.chance;
      int16_t pitch = trackData[track].pitch;
      int16_t vel = note.velocity;

      //superposition pitch mod, if there is a superposition
      if(note.superposition.pitch != 255 && (random(100)<note.superposition.odds || note.isSuperpositioned())){
        pitch = note.superposition.pitch;
        note.setSuperpositioned(true);
      }

      //global mods
      //adjusting chance
      if(trackData[track].channel == globalModifiers.chance.channel || globalModifiers.chance.channel == 0){
        chance += globalModifiers.chance.value;
        if(chance<0)
          chance = 0;
        else if(chance>100)
          chance = 100;
      }

      //adjusting pitch
      if(trackData[track].channel == globalModifiers.pitch.channel || globalModifiers.pitch.channel == 0){ 
        pitch += globalModifiers.pitch.value;
        if(pitch<0)
          pitch = 0;
        else if(pitch>127)
          pitch = 127;
      }

      //adjusting vel
      if(trackData[track].channel == globalModifiers.velocity.channel || globalModifiers.velocity.channel == 0){
        vel += globalModifiers.velocity.value;
        if(vel<0)
          vel = 0;
        else if(vel>127)
          vel = 127;
      }
      //if chance is 100%
      if(chance > random(100)){
        //if it's part of a muteGroup
        if(trackData[track].muteGroup!=0){
          muteGroups(track, trackData[track].muteGroup);
        }
        if(!arp.isActive || arp.source == NOTES_FROM_MIDI_INPUT)
          MIDI.noteOn(pitch, vel, trackData[track].channel);
        trackData[track].noteLastSent = pitch;
        if(trackData[track].isLatched()){
          if(!arp.isActive || arp.source == NOTES_FROM_MIDI_INPUT)
            MIDI.noteOff(pitch, 0, trackData[track].channel);
        }
        sentNotes.addNote(pitch,vel,trackData[track].channel);
        triggerAutotracks(track,true);
        return;
      }
      //if the note fails to fire, set the pitch flag anyway so the sequencer knows not to try and play it again
      else{
        trackData[track].noteLastSent = pitch;
      }
    }
  }
  //if it's the end of the note (just for visual effect)
  else if(timestep == note.endPos-1){
    //reset superposition flag
    if(note.isSuperpositioned()){
      note.setSuperpositioned(false);
    }
  }
}

/*

  Code for the StepchildSequence class

*/

//creates a sequence object with default values
void StepchildSequence::init(uint8_t numberOfTracks,uint16_t length){
  //What should happen if length<192?
  this->sequenceLength = length;
  this->viewStart = 0;
  this->viewEnd = 192;

  //make the default loop
  this->loopData = {Loop(0,96,0,0)};

  //reset to 120 bpm
  sequenceClock.setBPM(120);

  //resize data arrays to number of tracks
  this->lookupTable.resize(numberOfTracks);
  this->noteData.resize(numberOfTracks);

  //clear out trackData
  this->trackData = {};
  //pitch var so that all the tracks count up from this (weird bc the order the tracks are drawn in is high->low)
  uint8_t pitch = this->defaultPitch + numberOfTracks-1;
  for(uint8_t i = 0; i<numberOfTracks; i++){
      //add a track on the lookupdata
      this->lookupTable[i].resize(sequenceLength+1,0);
      this->noteData[i] = {Note()};
      this->trackData.push_back(Track(pitch,this->defaultChannel));
      pitch--;
  }
}
void StepchildSequence::init(){
  this->init(16,768);
}
//inits a sequence from a template
void StepchildSequence::init(SequenceTemplate t){
  this->erase();
  this->sequenceLength = 768;
  this->viewStart = 0;
  this->viewEnd = 192;
  this->loopData = {Loop(0,96,0,0)};

  sequenceClock.setBPM(120);

  trackData = t.loadTemplate();

  //resize data arrays to number of tracks
  this->lookupTable.resize(trackData.size());
  this->noteData.resize(trackData.size());

  //load in tracks

  //set up sequence data
  for(uint8_t i = 0; i<trackData.size(); i++){
      this->lookupTable[i].resize(769,0);
      this->noteData[i] = {Note()};
  }
}

bool StepchildSequence::isQuarterGrid(){
  return !(this->subDivision%3);
}

//swaps all the data vars in the sequence for new, blank data
void StepchildSequence::erase(){
  this->selectionCount = 0;
  vector<vector<uint16_t>> newLookupData;
  newLookupData.swap(this->lookupTable);
  vector<vector<Note>> newSeqData;
  newSeqData.swap(this->noteData);
  vector<Track> newTrackData;
  newTrackData.swap(this->trackData);
  vector<Autotrack> newAutotrackData;
  newAutotrackData.swap(this->autotrackData);
  vector<Loop> newLoopData;
  newLoopData.swap(this->loopData);
}
Note StepchildSequence::noteAt(uint8_t track, uint16_t step){
  if(this->lookupTable[track][step]<this->noteData[track].size())
      return this->noteData[track][this->lookupTable[track][step]];
  else return this->noteData[track][0];
}

Note StepchildSequence::noteAtCursor(){
  return this->noteAt(this->activeTrack,this->cursorPos);
}
uint16_t StepchildSequence::IDAt(uint8_t track, uint16_t step){
  if(step<this->sequenceLength)
      return this->lookupTable[track][step];
  else return 0;
}
uint16_t StepchildSequence::IDAtCursor(){
  return this->IDAt(this->activeTrack,this->cursorPos);
}

/*
----------------------------------------------------------
                  LOADING NOTES
----------------------------------------------------------
*/
//adds a note w/o checking for overlaps
//Only use this when loading notes from a file into a blank sequence
void StepchildSequence::loadNote(Note newNote, uint8_t track){
  this->noteData[track].push_back(newNote);
  if(newNote.isSelected())
      this->selectionCount++;
  //adding to lookupData
  for (uint16_t i =  newNote.startPos; i < newNote.endPos; i++) { //sets id
      this->lookupTable[track][i] = this->noteData[track].size()-1;
  }
}
/*
----------------------------------------------------------
                  DELETING NOTES
----------------------------------------------------------
*/

void StepchildSequence::deleteNotes_byID(vector<NoteID> targetNoteIDs){
  //make sure IDs stay referenced
  targetNoteIDs = crunchNoteIDsForDeletion(targetNoteIDs);
  //delete notes from sequence
  for(NoteID n:targetNoteIDs){
    deleteNote_byID(n.track,n.id);
  }
}
//Deletes a note on a given track with a given ID
void StepchildSequence::deleteNote_byID(uint8_t track, uint16_t targetNoteID){
  //if there's a note/something here, and it's in data
  if (targetNoteID > 0 && targetNoteID < this->noteData[track].size()) {
      //clearing note from this->lookupTable
      for (uint16_t i = this->noteData[track][targetNoteID].startPos; i < this->noteData[track][targetNoteID].endPos; i++) {
          this->lookupTable[track][i] = 0;
      }
      //lowering selectionCount
      if(this->noteData[track][targetNoteID].isSelected() && this->selectionCount>0)
          this->selectionCount--;
      //erasing note from this->noteData
      //make a copy of the this->noteData[track] vector which excludes the note
      //hopefully, this does a better job of freeing memory
      //swapping it like this! this is so the memory is free'd up again
      vector<Note> temp = {Note()};
      for(uint16_t i = 1; i<=this->noteData[track].size()-1; i++){
      if(i != targetNoteID){//if it's not the target note, or an empty spot, copy it to the temp vector
          temp.push_back(this->noteData[track][i]);
      }
      }
      temp.swap(this->noteData[track]);
      //since we del'd it from this->noteData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the del'd note's ID should be decremented.
      if(this->noteData[track].size()-1>0){
          for (uint16_t step = 0; step < this->lookupTable[track].size(); step++) {
              if (this->lookupTable[track][step] > targetNoteID) //if there's a higher note and if there are still notes to be changed
                  this->lookupTable[track][step] -= 1;
          }
      }
  }
}

//deletes a note at a specific time/place
void StepchildSequence::deleteNote(uint8_t track, uint16_t time){
  this->deleteNote_byID(track,this->IDAt(track,time));
}
//deletes a note at the current track/cursor position
void StepchildSequence::deleteNote(){
  this->deleteNote_byID(this->activeTrack,this->IDAtCursor());
}
void StepchildSequence::deleteSelected(){
  if(this->selectionCount>0){
      if(binarySelectionBox(64,32,"nah","yea","del "+stringify(selectionCount)+((selectionCount == 1)?stringify(" note?"):stringify(" notes?")),drawSeq) == 1){
          for(uint8_t track = 0; track<this->trackData.size(); track++){
              for(uint16_t note = 0; note<this->noteData[track].size(); note++){
                  if(this->noteData[track][note].isSelected()){
                      this->deleteNote_byID(track, note);
                      (note == 0) ? note = 0: note--;
                  }
                  //if you've already checked all the selected notes
                  if(this->selectionCount == 0)
                      break;
              }
          }
      }
  }
}
/*
----------------------------------------------------------
                  CREATING NOTES
----------------------------------------------------------
Lots of these are redundant/deprecated overloads... go thru em and get rid of them!
*/
void StepchildSequence::makeNote(Note newNoteOn, uint8_t track, bool loudly){
  //if you're placing it on the end of the seq, just return
  if(newNoteOn.startPos == this->sequenceLength)
      return;
  //if there's a 0 where the note is going to go, or if there's not a zero BUT it's also not the start of that other note
  if (this->lookupTable[track][newNoteOn.startPos] == 0 || newNoteOn.startPos != this->noteAt(this->activeTrack,newNoteOn.startPos).startPos) { //if there's no note there
      if (this->lookupTable[track][newNoteOn.startPos] != 0)
          this->truncateNote(track, newNoteOn.startPos);
      if(newNoteOn.isSelected())
          this->selectionCount++;
      uint16_t id = this->noteData[track].size();
      this->lookupTable[track][newNoteOn.startPos] = id;//set noteID in this->lookupTable to the index of the new note
      for (uint16_t i = newNoteOn.startPos+1; i < newNoteOn.endPos; i++) { //sets id
          if (this->lookupTable[track][i] == 0 && i<this->sequenceLength)
              this->lookupTable[track][i] = id;
          else { //if there's something there, then set the end to the step before it
              newNoteOn.endPos = i;
              break;
          }
      }
      this->noteData[track].push_back(newNoteOn);
      if (loudly) {
          MIDI.noteOn(this->trackData[track].pitch, newNoteOn.velocity, this->trackData[track].channel);
          MIDI.noteOff(this->trackData[track].pitch, 0, this->trackData[track].channel);
      }
  }
}
void StepchildSequence::makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
  this->makeNote(newNoteOn,track,loudly);
}
void StepchildSequence::makeNote(Note newNoteOn, uint8_t track){
  this->makeNote(newNoteOn,track,false);
}
void StepchildSequence::makeNote(uint8_t track, uint16_t time, uint16_t length, uint8_t velocity, uint8_t chance, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, false, false);
  this->makeNote(newNoteOn,track,false);
}
//this one is for quickly placing a ntoe at the cursor, on the active track
void StepchildSequence::makeNote(uint8_t track, uint16_t time, uint16_t length, bool loudly) {
  Note newNote(time,(time+length),this->defaultVel,100,false,false);
  this->makeNote(newNote,track,loudly);
}
void StepchildSequence::makeNote(uint16_t length, bool loudly) {
  Note newNote(this->cursorPos,this->cursorPos+length,this->defaultVel,100,false,false);
  this->makeNote(newNote,this->activeTrack,false);
}
//draws notes every "count" subDivs, from viewStart to viewEnd
//this is a super useful idea for sequencing, but currently only used by the edit menu
void StepchildSequence::stencilNotes(uint8_t count){
  for(uint16_t i = this->viewStart; i<this->viewEnd; i+=(this->subDivision*count)){
      //if there's no note there or if it's not the beginning of a note
      if(this->lookupTable[this->activeTrack][i] == 0 || (this->cursorPos != this->noteAtCursor().startPos))
          this->makeNote(this->activeTrack, i, this->subDivision*count+1, this->defaultVel, 100, false);
  }
}

//checks if there's a note first, and if there is it deletes it/if not it places one
void StepchildSequence::toggleNote(uint8_t track, uint16_t step, uint16_t length){
  //if there's no note/no start there, make a note
  if(this->lookupTable[track][step] == 0 || (this->lookupTable[track][step] != 0 && this->noteAt(track,step).startPos != step)){
      if(this->playing() && this->recording())
          this->makeNote(track, step, length, true);
      else
          this->makeNote(track, step, length, false);
  }
  //if there IS a note there, delete it
  else if(step == this->noteAt(track,step).startPos)
      this->deleteNote(track, step);
}
void StepchildSequence::makeNoteEveryNDivisions(uint8_t n){
  for(uint16_t step = viewStart; step<viewEnd; step+=(n*subDivision)){
      makeNote(activeTrack,step,subDivision,false);
  }
}
/*
----------------------------------------------------------
                  EDITING NOTES
----------------------------------------------------------
*/

//edits a single note accessed via its ID
void StepchildSequence::editNoteProperty_byID(uint16_t id, uint8_t track, int8_t amount, NoteProperty which){
  switch(which){
      case VELOCITY:{
          uint8_t vel = this->noteData[track][id].velocity;
          vel += amount;
          if(vel>=127)
              this->noteData[track][id].velocity = 127;
          else if(vel<0)
              this->noteData[track][id].velocity = 0;
          else
              this->noteData[track][id].velocity = vel;
          break;
      }
      case PITCH:{

      }
      case CHANCE:{
          int8_t chance = this->noteData[track][id].chance;
          chance += amount;
          if(chance>=100)
              this->noteData[track][id].chance = 100;
          else if(chance<0)
              this->noteData[track][id].chance = 0;
          else
              this->noteData[track][id].chance = chance;
          break;
      }
  }
}
//edits all selected notes
void StepchildSequence::editNotePropertyOfSelectedNotes(int8_t amount, NoteProperty which){
  for(uint8_t track = 0; track<this->trackData.size(); track++){
      for(uint16_t note = this->noteData[track].size()-1; note>0; note--){
          if(this->noteData[track][note].isSelected()){
              this->editNoteProperty_byID(note,track, amount, which);
          }
      }
  }
}

//edits a note, and all selected notes, checking to make sure it doesn't double-edit
void StepchildSequence::editNoteAndSelected(int8_t amount, NoteProperty which){
  if(this->selectionCount>0){
      this->editNotePropertyOfSelectedNotes(amount,which);
      //only edit this note if it's not selected, so you don't double-edit it
      if(!this->noteAtCursor().isSelected())
          this->editNoteProperty_byID(this->IDAtCursor(), this->activeTrack, amount, which);
  }
  else
      this->editNoteProperty_byID(this->IDAtCursor(), this->activeTrack, amount, which);
}

//called by main controls, edits all selected notes
void StepchildSequence::changeVel(int8_t amount){
  this->editNoteAndSelected(amount,VELOCITY);
}
void StepchildSequence::changeChance(int8_t amount){
  this->editNoteAndSelected(amount,CHANCE);
}

//changes JUST a specific note
void StepchildSequence::changeChance_byID(uint16_t id, uint8_t track, int8_t amount){
  this->editNoteProperty_byID(id, track, amount, CHANCE);
}
void StepchildSequence::changeVel_byID(uint16_t id, uint8_t track, int8_t amount){
  this->editNoteProperty_byID(id, track, amount, VELOCITY);
}

void StepchildSequence::muteNote(uint8_t track, uint16_t id, bool toggle){
  if(id != 0){
      if(toggle)
          this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
      else
          this->noteData[track][id].setMuted(true);
  }
}
void StepchildSequence::unmuteNote(uint8_t track, uint16_t id, bool toggle){
  if(id != 0){
      if(toggle)
          this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
      else
          this->noteData[track][id].setMuted(false);
  }
}

//mutes/unmutes all selected notes
void StepchildSequence::setMuteStateOfSelectedNotes(bool state){
  uint16_t count = 0;
  for(uint8_t track = 0; track<this->noteData.size(); track++){
      for(uint16_t note = 1; note<this->noteData[track].size(); note++){
          if(this->noteData[track][note].isSelected()){
              this->noteData[track][note].setMuted(state);
              count++;
          }
          if(count>=this->selectionCount){
              return;
          }
      }
  }
}
void StepchildSequence::muteSelectedNotes(){
  this->setMuteStateOfSelectedNotes(true);
}
void StepchildSequence::unmuteSelectedNotes(){
  this->setMuteStateOfSelectedNotes(false);
}

//cuts a note short at a specific time
void StepchildSequence::truncateNote(uint8_t track, uint16_t atTime){
  uint16_t id = this->lookupTable[track][atTime];
  if(id == 0 || id >= this->noteData[track].size())
      return;
  //if the note is only 1 step long, j del it
  if(this->noteData[track][id].endPos == this->noteData[track][id].startPos+1){
      this->deleteNote_byID(track,id);
      return;
  }

  //remove note from the lookupTable
  for(uint16_t i = atTime; i<this->noteData[track][id].endPos; i++){
      this->lookupTable[track][i] = 0;
  }
  //set the note end to the new cut point
  this->noteData[track][id].endPos = atTime;
}
bool StepchildSequence::checkNoteMove(Note& targetNote, uint16_t track, uint16_t newTrack, uint16_t newStart){
  
  uint16_t length = targetNote.endPos-targetNote.startPos;
  //checking bounds
  if((newStart+length)>this->sequenceLength || newTrack>=this->trackData.size())
      return false;
  //checking lookupData
  for(uint16_t start = newStart; start < newStart+length; start++){
      //for moving horizontally within one track
      if(track == newTrack && this->lookupTable[newTrack][start] != 0)
          return false;
      //for vertical kinds of movement, where you won't collide with yourself
      else if(this->lookupTable[newTrack][start] != 0)
          return false;
  }
  return true;
}
bool StepchildSequence::checkNoteMove(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
  Note targetNote = this->noteData[track][id];
  return this->checkNoteMove(targetNote,track,newTrack,newStart);
}
//moves a note
bool StepchildSequence::moveNote(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
  Note targetNote = this->noteData[track][id];
  //remove note
  deleteNote_byID(track,id);
  //if there's room
  if(checkNoteMove(targetNote, track, newTrack, newStart)){
      uint16_t length = targetNote.endPos-targetNote.startPos;
      targetNote.startPos = newStart;
      targetNote.endPos = newStart+length;
      makeNote(targetNote,newTrack, false);
      return true;
  }
  else{
    makeNote(targetNote,track,false);
    return false;
  }
}
bool StepchildSequence::moveSelectedNotes(int16_t xOffset, int8_t yOffset){
  //to temporarily store all the notes
  vector<vector<Note>> selectedNotes;
  selectedNotes.resize(this->noteData.size());

  //grab all the selected notes
  for(uint8_t track = 0; track<this->noteData.size(); track++){
      for(uint16_t note = 1; note<this->noteData[track].size(); note++){
          //if the note is selected, push it into the buffer and then del it
          if(this->noteData[track][note].isSelected()){
              selectedNotes[track].push_back(this->noteData[track][note]);
              this->deleteNote(track, this->noteData[track][note].startPos);
              note--;
          }
          if(this->selectionCount==0)
              break;
      }
      if(this->selectionCount==0)
          break;
  }
  //check each note in the buffer to see if it's a valid move
  for(uint8_t track = 0; track<selectedNotes.size(); track++){
      for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
          //if it hits a single bad note, then remake all the notes and exit
          if(!this->checkNoteMove(selectedNotes[track][note],track,track+yOffset,selectedNotes[track][note].startPos+xOffset)){
              for(uint8_t track2 = 0; track2<selectedNotes.size(); track2++){
                  for(uint16_t note2  = 0; note2<selectedNotes[track2].size(); note2++){
                      //remake old note
                      this->makeNote(selectedNotes[track2][note2],track2,false);
                  }
              }
              return false;
          }
      }
  }
  //if all notes pass the check... move em!
  for(uint8_t track = 0; track<selectedNotes.size(); track++){
      for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
          unsigned short int length = selectedNotes[track][note].endPos-selectedNotes[track][note].startPos;
          this->makeNote(track+yOffset, selectedNotes[track][note].startPos+xOffset, length+1, selectedNotes[track][note].velocity, selectedNotes[track][note].chance, selectedNotes[track][note].isMuted(), selectedNotes[track][note].isSelected(), false);
      }
  }
  return true;
}
//this should move the note the cursor is on (if any)
bool StepchildSequence::moveNotes(int16_t xAmount, int8_t yAmount){
  if(!selectionCount){
      if(this->IDAtCursor()){
          return this->moveNote(this->IDAtCursor(),this->activeTrack,this->activeTrack+yAmount,this->noteData[this->activeTrack][this->IDAtCursor()].startPos+xAmount);
      }
      else return false;
  }
  else{
      return this->moveSelectedNotes(xAmount,yAmount);
  }
}
/*
----------------------------------------------------------
                  EDITING SEQ
----------------------------------------------------------
*/
void StepchildSequence::addTimeToSeq(uint16_t amount, uint16_t insertPoint){
  //make sure it doesn't overflow the uint16_t length
  if(this->sequenceLength+amount>65535)
      return;
  this->sequenceLength+=amount;
  //move through notes that appear AFTER the insert point and move them back
  //sweeping from old seq end (there should be no notes after that)-->insertPoint
  for(uint8_t t = 0; t<this->trackData.size(); t++){
      //extend lookupdata
      this->lookupTable[t].resize(this->sequenceLength,0);
      for(uint16_t i = this->sequenceLength-amount; i>insertPoint; i--){
      uint16_t id = this->IDAt(t,i);
      //if there's a note there
      if(id != 0){
          //if it starts after the insert point, move the whole note
          if(this->noteData[t][id].startPos>insertPoint){
              //set i to old beginning of note (so you definitely don't hit it twice)
              i = this->noteData[t][id].startPos;
              this->moveNote(id,t,t,this->noteData[t][id].startPos+amount);
          }
          //if the insert point intersects it somehow, truncate it
          else{
              this->truncateNote(t,insertPoint);
              //break bc that was the last note
              break;
          }
      }
      }
  }
}
void StepchildSequence::removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
  //if you're trying to del more than (or as much as) exists! just return
  if(amount>=this->sequenceLength)
      return;
  //if you're trying to del more than exists between insertpoint --> this->sequenceLength,
  //then set amount to everything there
  if(insertPoint+amount>this->sequenceLength){
      amount = this->sequenceLength-insertPoint;
  }
  //move through 'deld' area and clear out notes
  for(uint8_t t = 0; t<trackData.size(); t++){
      //if there are no notes, skip this track
      if(this->noteData[t].size() == 1)
          continue;
      for(uint16_t i = insertPoint+1; i<insertPoint+amount; i++){
          uint16_t id = this->lookupTable[t][i];
          //if there's a note there
          if(id != 0){
              //if it starts before/at insert point, truncate it
              if(this->noteData[t][id].startPos<=insertPoint){
                  truncateNote(t,insertPoint);
                  //break bc you know that was the last note
                  break;
              }
              //if it's in the del area, del it
              else if(this->noteData[t][id].startPos>insertPoint && this->noteData[t][id].startPos<=insertPoint+amount){
                  deleteNote_byID(t,id);
              }
          }
      }
  }
  //move notes that fall beyond the del area
  for(uint8_t t = 0; t<trackData.size(); t++){
      //if there're no notes, skip this track
      if(this->noteData[t].size() == 1)
          continue;
      for(uint16_t i = insertPoint+amount; i<this->sequenceLength; i++){
          uint16_t id = this->lookupTable[t][i];
          //if there's a note here, move it back by amount
          if(id != 0){
              this->moveNote(id,t,t,this->noteData[t][id].startPos-amount);
              i = this->noteData[t][id].endPos-1;
          }
      }
      //resize lookupData
      this->lookupTable[t].resize(this->sequenceLength-amount);
  }
  this->sequenceLength -= amount;
  //fixing loop point positions
  for(uint8_t loop = 0; loop<loopData.size(); loop++){
      //if start or end are past seqend, set to seqend
      if(loopData[loop].start>this->sequenceLength)
          loopData[loop].start = this->sequenceLength;
      if(loopData[loop].end>this->sequenceLength)
          loopData[loop].end = this->sequenceLength;
  }
  //make sure view stays within seq
  checkView();
}
/*
----------------------------------------------------------
                      Graphics??
----------------------------------------------------------
*/
//displays notes on LEDs
void StepchildSequence::displayMainSequenceLEDs(){
  if(controls.SHIFT()){
      uint16_t ledState = 0b0000000000001111;
      if(millis()/200%2)
          ledState |= 0b1000000000000000;
      if((millis()/200+1)%2)
          ledState |= 0b0100000000000000;
      
      controls.writeLEDs(ledState);
      return;
  }
  uint16_t dat = 0;//00000000
  if(controls.LEDsActive && !screenSaverActive){
      uint16_t viewLength = this->viewEnd-this->viewStart;
      //move through the view, check every this->subDivision
      const uint16_t jump = this->isQuarterGrid()?(viewLength/16):(viewLength/12);
      // const uint16_t jump = viewLength/16;
      //if there are any notes, check
      if(this->noteData[this->activeTrack].size()>1){
          for(uint8_t i = 0; i<16; i++){
              uint16_t step = this->viewStart+i*jump;
              if(this->lookupTable[this->activeTrack][step] != 0){
                  //not sure if it should only light up if it's on the start step or nah
                  if(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][step]].startPos == step){
                      //if playing or recording, and the head isn't on that step, it should be on
                      //if it is on that step, then the step should blink
                      if(!this->playing() || ((playheadPos < this->noteAt(this->activeTrack,step).startPos) || (playheadPos > this->noteAt(this->activeTrack,step).endPos))){
                          dat |= (1<<i);
                      }
                  }
              }
          }
      }
  }
  controls.writeLEDs(dat);
}
/*
----------------------------------------------------------
                      Loops
----------------------------------------------------------
*/
void StepchildSequence::setLoopPoint(int32_t start, bool which){
  //set start
  if(which){
      if(start>=loopData[activeLoop].end)
          return;
      if(start<=this->loopData[this->activeLoop].end && start>=0)
          this->loopData[this->activeLoop].start = start;
      else if(start < 0)
          this->loopData[this->activeLoop].start = 0;
      else if(start>this->loopData[this->activeLoop].end)
          this->loopData[this->activeLoop].start = this->loopData[this->activeLoop].end;
      this->loopData[this->activeLoop].start = this->loopData[this->activeLoop].start;
      menuText = "loop start: "+stepsToPosition(this->loopData[this->activeLoop].start,true);
  }
  //set end
  else{
      if(start <= loopData[activeLoop].start)
          return;
      if(start>=this->loopData[this->activeLoop].start && start <= this->sequenceLength)
          this->loopData[this->activeLoop].end = start;
      else if(start>this->sequenceLength)
          this->loopData[this->activeLoop].end = this->sequenceLength;
      else if(start<this->loopData[this->activeLoop].start)
          this->loopData[this->activeLoop].end = this->loopData[this->activeLoop].start;
      this->loopData[this->activeLoop].end = this->loopData[this->activeLoop].end;
      menuText = "loop end: "+stepsToPosition(this->loopData[this->activeLoop].end,true);
  }
}
void StepchildSequence::addLoop(Loop newLoop){
  this->loopData.push_back(newLoop);
}
void StepchildSequence::insertLoop(Loop newLoop, uint8_t index){
  this->loopData.insert(this->loopData.begin()+index,newLoop);
}


void StepchildSequence::setActiveLoop(unsigned int id){
  if(id<this->loopData.size() && id >=0){
      this->activeLoop = id;
      this->loopCount = 0;
  }
}

void StepchildSequence::addLoop(){
  Loop newLoop;
  newLoop.start = this->loopData[this->activeLoop].start;
  newLoop.end = this->loopData[this->activeLoop].end;
  newLoop.reps = this->loopData[this->activeLoop].reps;
  newLoop.type = this->loopData[this->activeLoop].type;
  this->loopData.push_back(newLoop);
  setActiveLoop(this->loopData.size()-1);
}

void StepchildSequence::addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, uint8_t type){
  Loop newLoop;
  newLoop.start = start;
  newLoop.end = end;
  newLoop.reps = iter;
  newLoop.type = static_cast<LoopType>(type);
  this->loopData.push_back(newLoop);
}

void StepchildSequence::deleteLoop(uint8_t id){
  if(this->loopData.size() > 1 && this->loopData.size()>id){//if there's more than one loop, and id is in this->loopData
      vector<Loop> tempVec;
      for(int i = 0; i<this->loopData.size(); i++){
          if(i!=id){
              tempVec.push_back(this->loopData[i]);
          }
      }
      this->loopData.swap(tempVec);
      //if this->activeLoop was the loop that got deld, or above it
      //decrement it's id so it reads correct (and existing) data
      if(this->activeLoop>=this->loopData.size()){  
          this->activeLoop = this->loopData.size()-1;
      }
  }
  setActiveLoop(this->activeLoop);
}

void StepchildSequence::toggleLoop(){
  this->isLooping = !this->isLooping;
}

//moves to the next loop in loopSeq
void StepchildSequence::nextLoop(){
  this->loopCount = 0;
  if(this->loopData.size()>1){
      switch(this->loopData[this->activeLoop].type){
          case NORMAL:
              //move to next loop
              if(this->activeLoop < this->loopData.size()-1)
                  this->activeLoop++;
              else
                  this->activeLoop = 0;
              if(this->playing())
                  playheadPos = this->loopData[this->activeLoop].start;
              if(this->recording())
                  recheadPos = this->loopData[this->activeLoop].start;
              break;
          case RANDOM:{
              this->activeLoop = random(0,this->loopData.size());
              if(this->playing())
                  playheadPos = this->loopData[this->activeLoop].start;
              if(this->recording())
                  recheadPos = this->loopData[this->activeLoop].start;
              break;}
          case RANDOM_SAME:{
              //move to next loop
              if(this->activeLoop < this->loopData.size()-1)
                  this->activeLoop++;
              else
                  this->activeLoop = 0;
              //if rnd of same size mode, choose a random loop
              int currentLength = this->loopData[this->activeLoop].end - this->loopData[this->activeLoop].start;
              vector<uint8_t> similarLoops;
              for(int i = 0; i<this->loopData.size(); i++){
                  int len = this->loopData[i].end-this->loopData[i].start;
                  if(len == currentLength){
                      similarLoops.push_back(i);
                  }
              }
              this->activeLoop = similarLoops[random(0,similarLoops.size())];
              if(this->playing())
                  playheadPos = this->loopData[this->activeLoop].start;
              if(this->recording())
                  recheadPos = this->loopData[this->activeLoop].start;
              break;}
          case RETURN:{
              this->activeLoop = 0;
              if(this->playing())
                  playheadPos = this->loopData[this->activeLoop].start;
              if(this->recording())
                  recheadPos = this->loopData[this->activeLoop].start;
              break;}
          case INFINITE:{
              if(this->playing())
                  playheadPos = this->loopData[this->activeLoop].start;
              if(this->recording())
                  recheadPos = this->loopData[this->activeLoop].start;
              break;}
          }
  }
}
//cuts notes off when loop repeats, then starts new note at beginning
void StepchildSequence::cutLoop(){
  for(int i = 0; i<this->trackData.size(); i++){
      if(this->trackData[i].noteLastSent != 255){
          this->noteData[i][this->noteData[i].size()-1].endPos = this->loopData[this->activeLoop].end;
          //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
          if(recMode != ONESHOT)
              writeNoteOn(this->loopData[this->activeLoop].start,this->trackData[i].pitch,this->noteData[i][this->noteData[i].size()-1].velocity,this->trackData[i].channel);
      }
  }
}

//true if Stepchild is sending or receiving notes
bool StepchildSequence::isReceiving(){
for(uint8_t i = 0; i<this->trackData.size(); i++){
  if(this->trackData[i].noteLastSent != 255)
    return true;
}
return receivedNotes.notes.size();
}
bool StepchildSequence::isSending(){
if(sentNotes.notes.size())
  return true;
for(uint8_t i = 0; i<this->trackData.size(); i++){
  if(this->trackData[i].noteLastSent != 255)
    return true;
}
return false;
}

bool StepchildSequence::isReceivingOrSending(){
return (isReceiving() || isSending());
}


//moves the whole loop
void StepchildSequence::moveLoop(int16_t amount){
uint16_t length = this->loopData[this->activeLoop].end-this->loopData[this->activeLoop].start;
//if it's being moved back
if(amount<0){
  //if amount is larger than start, meaning start would be moved before 0
  if(this->loopData[this->activeLoop].start<=amount)
    this->setLoopPoint(0,true);
  else
    this->setLoopPoint(this->loopData[this->activeLoop].start+amount,true);
  this->setLoopPoint(this->loopData[this->activeLoop].start+length,false);
}
//if it's being moved forward
else{
  //if amount is larger than the gap between seqend and this->loopData[this->activeLoop].end
  if((this->sequenceLength-this->loopData[this->activeLoop].end)<=amount)
    this->setLoopPoint(this->sequenceLength,false);
  else
    this->setLoopPoint(this->loopData[this->activeLoop].end+amount,false);
  this->setLoopPoint(this->loopData[this->activeLoop].end - length,true);
}
}
void StepchildSequence::toggleLoopMove(){
switch(movingLoop){
  case MOVING_NO_LOOP_POINTS:
    movingLoop = MOVING_LOOP_START;
    moveCursor(this->loopData[this->activeLoop].start-this->cursorPos);
    break;
  case MOVING_LOOP_END:
    movingLoop = MOVING_LOOP_START;
    moveCursor(this->loopData[this->activeLoop].end-this->cursorPos);
    break;
  case MOVING_LOOP_START:
    movingLoop = MOVING_BOTH_LOOP_POINTS;
    break;
  case MOVING_BOTH_LOOP_POINTS:
    movingLoop = MOVING_NO_LOOP_POINTS;
    break;
}
}

int16_t StepchildSequence::changeNoteLength(int val, unsigned short int track, unsigned short int id){
if(id!=0){
  int newEnd;
  //if it's 1 step long and being increased by this->subDivision, make it this->subDivision steps long instead of this->subDivision+1 steps
  //(just to make editing more intuitive)
  if(this->noteData[track][id].endPos - this->noteData[track][id].startPos == 1 && val == this->subDivision){
    newEnd = this->noteData[track][id].endPos + val - 1;
  }
  else{
    newEnd = this->noteData[track][id].endPos + val;
  }
  //check and see if there's a different note there
  //if there is, set the new end to be 
  for(uint16_t step = 1; step<=val; step++){
    if(this->lookupTable[track][step+this->noteData[track][id].endPos] != 0 && this->lookupTable[track][step+this->noteData[track][id].endPos] != id){
      //if it's the first step, just fail to save time
      if(step == 1)
        return 0;
      //if it's not the first step, set the new end to right before the other note
      else{
        newEnd = this->noteData[track][id].endPos+step;
        break;
      }
    }
  }
  //if the new end is before/at the start, don't do anything
  if(newEnd<=this->noteData[track][id].startPos)
    return 0;
    // newEnd = this->noteData[track][id].startPos+1;
  //if the new end is past/at the end of the seq
  if(newEnd>this->sequenceLength){
    newEnd = this->sequenceLength;
  }
  Note note = this->noteData[track][id];
  int16_t amount = newEnd-note.endPos;
  note.endPos = newEnd;
  this->deleteNote_byID(track, id);
  this->makeNote(note, track, false);
  return amount;
}
return 0;
}


void StepchildSequence::changeNoteLengthSelected(int amount){
for(int track = 0; track<this->trackData.size(); track++){
  for(int note = 1; note <= this->noteData[track].size()-1; note++){
    if(this->noteData[track][note].isSelected()){
      changeNoteLength(amount, track, note);
    }
  }
}
}

int16_t StepchildSequence::changeNoteLength(int amount){
if(this->selectionCount > 0){
  changeNoteLengthSelected(amount);
  vector<uint16_t> bounds = getSelectedNotesBoundingBox();
  //move the cursor to the end/beginning of the selection box
  if(amount>0)
    setCursor(bounds[2]);
  else
    setCursor(bounds[0]);
  return 0;
}
else{
  return changeNoteLength(amount, this->activeTrack, this->IDAtCursor());
}
}

//this one jumps the cursor to the end or start of the note
void StepchildSequence::changeNoteLength_jumpToEnds(int16_t amount){
if(this->selectionCount > 0){
  changeNoteLengthSelected(amount);
  vector<uint16_t> bounds = getSelectedNotesBoundingBox();
  //move the cursor to the end/beginning of the selection box
  if(amount>0)
    setCursor(bounds[2]);
  else
    // setCursor(bounds[0]);
    setCursor(bounds[2]-this->subDivision);//testing this
}
else{
  //if the note was changed
  if(changeNoteLength(amount, this->activeTrack, this->IDAtCursor()) != 0){
    //if you're shrinking the note
    if(amount<0){
      setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-this->subDivision);//testing this
      // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].startPos);
      //if it's out of view
      // else
        // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos+amount);
    }
    //if you're growing it
    else
      // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-this->subDivision);
      setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-1);//testing this

      // setCursor(this->noteData[this->activeTrack][this->IDAtCursor()].endPos - amount);
  }
}
}

//sets cursor to the visually nearest note
//steps to pixels = steps*scale
//for a note to be "visually" closer, it needs to have a smaller pixel
//distance from the cursor than another note
//compare trackDistance * trackHeight to stepDistance * scale
float StepchildSequence::getDistanceFromNoteToCursor(Note note,uint8_t track){
//if the start of the note is closer than the end
return sqrt(pow(this->activeTrack - track,2)+pow(((abs(note.startPos-this->cursorPos)<abs(note.endPos-this->cursorPos))?(note.startPos-this->cursorPos):(note.endPos-this->cursorPos)),2));
}

void StepchildSequence::setCursorToNearestNote(){
const float maxPossibleDist = this->sequenceLength*this->viewScale+this->trackData.size()*trackHeight;
float minDist = maxPossibleDist;
uint16_t minTrack = 0;
uint16_t minNote = 0;
for(int track = 0; track<this->noteData.size(); track++){
  for(int note = 1; note<this->noteData[track].size(); note++){
    // //Serial.println("checking n:"+stringify(note)+" t:"+stringify(track));
    // Serial.flush();
    float distance = getDistanceFromNoteToCursor(this->noteData[track][note],track);
    if(distance<minDist){
      minTrack = track;
      minNote = note;
      minDist = distance;
      if(minDist == 0)
        break;
    }
  }
  if(minDist == 0)
    break;
}
// //Serial.println("setting cursor...");
// Serial.flush();
if(minDist != maxPossibleDist){
  setCursor((this->noteData[minTrack][minNote].startPos<this->cursorPos)?this->noteData[minTrack][minNote].startPos:this->noteData[minTrack][minNote].endPos-1);
  setActiveTrack(minTrack,false);
}
}


uint16_t StepchildSequence::getNoteCount(){
uint16_t count = 0;
for(uint8_t track = 0; track<this->noteData.size(); track++){
  count+=this->noteData[track].size()-1;
}
return count;
}

float StepchildSequence::getNoteDensity(uint16_t timestep){
float density = 0;
for(int track = 0; track<this->trackData.size(); track++){
  if(this->lookupTable[track][timestep] != 0){
    density++;
  }
}
return density/float(this->trackData.size());
}
float StepchildSequence::getNoteDensity(uint16_t start, uint16_t end){
float density = 0;
for(int i = start; i<= end; i++){
  density+=getNoteDensity(i);
}
return density/float(end-start+1);
}

//counts notes within a range
uint16_t StepchildSequence::countNotesInRange(uint16_t start, uint16_t end){
uint16_t count = 0;
for(uint8_t t = 0; t<this->trackData.size(); t++){
  //if there are no notes, ignore it
  if(this->noteData[t].size() == 1)
    continue;
  else{
    //move over each note
    for(uint16_t i = 1; i<this->noteData.size(); i++){
      if(this->noteData[t][i].startPos>=start && this->noteData[t][i].startPos<end)
        count++;
    }
  }
}
return count;
}


//above index is an INCLUSIVE lower bound!
int16_t StepchildSequence::getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex){
  for(int i=aboveIndex ; i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return 0;
}
//returns id of track with a specific pitch, returns -1 if track doesn't exist
int16_t StepchildSequence::getTrackWithPitch(int16_t pitch, uint8_t channel){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch && trackData[i].channel == channel)
      return i;
  }
  return -1;
}
int16_t StepchildSequence::getTrackWithPitch(int16_t pitch){
  for(uint16_t i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}

int16_t StepchildSequence::makeTrackWithPitch(int16_t pitch, uint8_t channel){
  int16_t track = getTrackWithPitch(pitch,channel);
  if(track == -1){
    return addTrack_return(pitch, channel, false);
  }
  return track;
}



void StepchildSequence::addTrack(Track newTrack, bool loudly){
  if(trackData.size()<255){
    insertTrack(newTrack,activeTrack);
    if(loudly){
      MIDI.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      MIDI.noteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}

//this function should add a new row to the sequence.noteData, and sequence.lookupTable initialized with 64 zeroes
void StepchildSequence::addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly){
  if(trackData.size()<255){
    Track newTrack(pitch, channel);
    newTrack.setLatched(latch);
    newTrack.muteGroup = muteGroup;
    newTrack.setPrimed(primed);
    addTrack(newTrack,loudly);
  }
}
void StepchildSequence::addTrack(unsigned char pitch, unsigned char channel, bool loudly) {
  addTrack(pitch, channel, false, 0, true, loudly);
}

//changes which track is active, changing only to valid tracks
bool StepchildSequence::setActiveTrack(uint8_t newActiveTrack, bool loudly) {
if (newActiveTrack >= 0 && newActiveTrack < this->trackData.size()) {
  //if you're about to run off the screen for the first time, shrink the top area
  if((this->activeTrack == this->maxTracksShown-1) && (newActiveTrack == this->maxTracksShown)){
    this->shrinkTopDisplay = true;
  }
  //if you're about to come back up to the within the first few tracks, unshrink the top area
  else if((this->activeTrack == 1) && (newActiveTrack == 0)){
    this->shrinkTopDisplay = false;
  }
  this->activeTrack = newActiveTrack;
  if (loudly) {
    MIDI.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
    MIDI.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
    if(this->trackData[this->activeTrack].isLatched()){
      MIDI.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
      MIDI.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
    }
  }
  menuText = pitchToString(this->trackData[this->activeTrack].pitch,true,true);
  return true;
}
return false;
}

void StepchildSequence::changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    this->trackData[id].channel = newChannel;
  }
}
//unarms any tracks with notes on them
void StepchildSequence::disarmTracksWithNotes(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    if(noteData[i].size()>1){
      trackData[i].setPrimed(false);
    }
  }
}

void StepchildSequence::muteTrack(uint16_t id){
  trackData[id].setMuted(true);
}
void StepchildSequence::unMuteTrack(uint16_t id){
  trackData[id].setMuted(false);
}

void StepchildSequence::toggleMute(uint16_t id){
  trackData[id].setMuted(!trackData[id].isMuted());
}

void StepchildSequence::muteMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    muteTrack(ids[track]);
  }
}

void StepchildSequence::soloTrack(unsigned short int id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id){
      muteTrack(track);
      trackData[track].setSolo(false);
    }
    else 
      trackData[id].setSolo(true);
  }
}

void StepchildSequence::unSoloTrack(uint16_t id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id)
      unMuteTrack(track);
    else
      trackData[id].setSolo(false);
  }
}

void StepchildSequence::toggleSolo(uint16_t id){
  if(trackData[id].isSolo())
    unSoloTrack(id);
  else
    soloTrack(id);
}

void StepchildSequence::eraseMultipleTracks(vector<uint8_t> ids){
  for(uint16_t track = 0; track<ids.size(); track++){
    eraseTrack(ids[track]);
  }
}


//returns the index of the new track
int16_t StepchildSequence::addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly) {
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    addTrack(newTrack,loudly);
    return (activeTrack);
  }
  else{
    return -1;
  }
}

void StepchildSequence::insertTrack(Track newTrack, uint8_t index){
  //if you're trying to insert past the end!
  if(index>=trackData.size()){
    //inserting new track
    trackData.push_back(newTrack);
    //inserting new lookupTable lane
    vector<uint16_t> blankLookupData(sequenceLength,0);
    lookupTable.push_back(blankLookupData);
    //inserting new noteData lane
    noteData.push_back({Note()});
  }
  else{
    //inserting new track
    trackData.insert(trackData.begin()+index,newTrack);
    //inserting new lookupTable lane
    vector<uint16_t> blankLookupData(sequenceLength,0);
    lookupTable.insert(lookupTable.begin()+index,blankLookupData);
    //inserting new noteData lane
    noteData.insert(noteData.begin()+index,{Note()});
  }
}

int16_t StepchildSequence::insertTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly, uint8_t loc){
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    insertTrack(newTrack,loc);
    if(loudly){
      MIDI.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      MIDI.noteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
    return (loc);
  }
  else{
    return -1;
  }
}

void StepchildSequence::dupeTrack(unsigned short int track){
  if(trackData.size()<256){
    Track newTrack = trackData[track];
    insertTrack(newTrack,track);
  }
}

void StepchildSequence::eraseTrack(int track) {
  noteData[track].resize(1);
  for (int i = 0; i < sequenceLength; i++) {
    clearSelection(track, i);
    lookupTable[track][i] = 0;
  }
}
//erases notes, but doesn't del track
void StepchildSequence::eraseTrack() {
  eraseTrack(activeTrack);
}


void StepchildSequence::deleteDuplicateEmptyTracks(){
  for(uint8_t t = 0; t<trackData.size(); t++){
    for(uint8_t t2 = 0; t2<trackData.size(); t2++){
      if(t2 == t)
        continue;
      //if a track has the same pitch, channel, and is empty, del it
      if(trackData[t].pitch == trackData[t2].pitch &&
        trackData[t].channel == trackData[t2].channel &&
        noteData[t2].size() == 1){
        deleteTrack(t2);
      }
    }
  }
}

uint8_t StepchildSequence::countEmptyTracks(){
  uint8_t count = 0;
  for(auto track:noteData){
    if(track.size()==1){
      count++;
    }
  }
  return count;
}
void StepchildSequence::deleteEmptyTracks(){
  if(binarySelectionBox(64,32,"nah","yea","delete "+stringify(countEmptyTracks())+" tracks?",drawSeq) != 1){
    return;
  }
  for(uint8_t i = 0; i<trackData.size(); i++){
    //leave at least 1 track
    if(noteData[i].size()-1 == 0 && trackData.size()>1){
      deleteTrack(i);
      i--;
    }
  }
  if(activeTrack>=trackData.size())
    activeTrack = trackData.size()-1;
}

//CHECK do you need to update trackID's??? idk if this will cause problems
//dels the track AND notes stored within it (from noteData and lookupTable)
//as long as you del tracks from the back, i think this is okay
void StepchildSequence::deleteTrack(unsigned short int track){
  deleteTrack(track,false);
}

void StepchildSequence::deleteTrack(unsigned short int track, bool hard, bool askFirst){
  int choice = 1;
  if(askFirst && noteData[track].size()-1>0){
    vector<String> ops = {"nay","yeah"};                                                                                    //this is == 2 instead of 1 because noteData[track] always has the default note
    choice = binarySelectionBox(64,32,"nay","yeah","del track w/"+stringify(noteData[track].size()-1)+(noteData[track].size() == 2?" note?":" notes?"),drawSeq);
  }
  if(choice == 1){
    if(trackData.size() == 1 && !hard){
      eraseTrack(track);
      return;
    }
    //if the end track is within view
    if(endTrack == trackData.size()){
      endTrack--;
      if(startTrack>0)
        startTrack--;
    }
    if(activeTrack == track && activeTrack>0){
      activeTrack--;
    }

    eraseTrack(track);

    //making new data without the deld track
    vector<Track> tempTrackData;
    vector<vector<Note>> tempSeqData;
    vector<vector<uint16_t>> tempLookupData;
    for(uint8_t t = 0; t<trackData.size(); t++){
      if(t != track){
        tempTrackData.push_back(trackData[t]);
        tempSeqData.push_back(noteData[t]);
        tempLookupData.push_back(lookupTable[t]);
      }
    }
    trackData.swap(tempTrackData);
    noteData.swap(tempSeqData);
    lookupTable.swap(tempLookupData);
  }
}

void StepchildSequence::deleteTrack(unsigned short int track, bool hard){
  deleteTrack(track, hard, true);
}
//dels all tracks
void StepchildSequence::deleteAllTracks(){
  while(trackData.size()>0){
    deleteTrack(0,true,false);
  }
}


void StepchildSequence::transposeAllChannels(int8_t increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackChannel(i,trackData[i].channel+increment,true);//only the active track makes a noise
    else
      setTrackChannel(i,trackData[i].channel+increment,false);//quiet bc it'd be crazy
  }
}

void StepchildSequence::setTrackChannel(uint16_t track, uint8_t channel, bool loud){
  if(channel>=1 && channel<=16){
    MIDI.noteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].channel = channel;
    if(loud){
      MIDI.noteOn(trackData[track].pitch,63,trackData[track].channel);
      MIDI.noteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

void StepchildSequence::transposeAllPitches(int16_t increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackPitch(i,trackData[i].pitch+increment,true);//only the active track makes a noise
    else
      setTrackPitch(i,trackData[i].pitch+increment,false);//quiet bc it'd be crazy
  }
}


void StepchildSequence::sortTrackData(uint8_t type,uint8_t target){
  //type is either 0 (ascending) or 1 (descending)
  //target is either pitch, channel, or the number of notes
  vector<Track> tempData = trackData;
  switch(target){
    case 0:
      sort(tempData.begin(),tempData.end(),sortTracksByPitch);
      break;
    case 1:
      sort(tempData.begin(),tempData.end(),sortTracksByChannel);
      break;
  }
  if(type)
    reverse(tempData.begin(),tempData.end());
  trackData = tempData;
}


void StepchildSequence::setTrackToNearestPitch(vector<uint8_t>pitches,uint8_t track,bool allowDuplicates){
  int oldPitch = trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch = 0;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the track is already that pitch, return
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if no duplicates are allowed, check to see if there are any other tracks
  //with this pitch
  if(!allowDuplicates){
    for(uint8_t t = 0; t<trackData.size(); t++){
      if(t!=track && trackData[t].pitch == pitches[closestPitch]+octaveOffset){
        return;
      }
    }
  }
  setTrackPitch(track,pitches[closestPitch]+octaveOffset,false);
}

void StepchildSequence::setTrackPitch(int track, int note, bool loud) {
  if(note>=0 && note<=127){
    MIDI.noteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].pitch = note;
    if(loud){
      MIDI.noteOn(trackData[track].pitch,63,trackData[track].channel);
      MIDI.noteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

//this one won't double up on a pitch, and will instead choose the next closes pitch in the list
void StepchildSequence::setTrackToNearestUniquePitch(vector<uint8_t>pitches,int track){
  int oldPitch = trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch = 0;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the distance is 0, then the track is already in tune
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if there's no track already with this pitch, set the track to the pitch
  if(getTrackWithPitch(pitches[closestPitch]+octaveOffset) == -1){
    setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
  }
  //if there is, run it again without this pitch (as long as there're still pitches left)
  else{
    if(pitches.size()>1){
      vector<uint8_t>newPitches;
      for(uint8_t i = 0; i<pitches.size(); i++){
        if(i != closestPitch)
          newPitches.push_back(pitches[i]);
      }
      pitches.swap(newPitches);
      //recursively run the function again, just without the already-occupied pitch as an option
      setTrackToNearestUniquePitch(pitches, track);
    }
    //if there's only one pitch,set it to it
    else{
      setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
    }
  }
}


void StepchildSequence::swapTracks(unsigned short int track1, unsigned short int track2){
  //making sure the tracks are real
  if(track1 < trackData.size() && track2 < trackData.size() && track1>=0 && track2 >= 0){
    //swapping track data
    Track old_activeTrack = trackData[track1];
    trackData[track1] = trackData[track2];
    trackData[track2] = old_activeTrack;
    //swapping lookupData
    vector<unsigned short int> old_lookupTable = lookupTable[track1];
    lookupTable[track1] = lookupTable[track2];
    lookupTable[track2] = old_lookupTable;
    //swapping sequence data
    vector<Note> old_noteData = noteData[track1];
    noteData[track1] = noteData[track2];
    noteData[track2] = old_noteData;
  }
}

void StepchildSequence::changeAllTrackChannels(int newChannel){
for(int track = 0; track<this->trackData.size(); track++){
  changeTrackChannel(track, newChannel);
}
}

void StepchildSequence::moveToNextNote_inTrack(bool up){
uint8_t track = this->activeTrack;
bool foundTrack = false;
//moving the track up/down until it hits a track with notes
//and checking bounds
if(up){
  while(track<this->trackData.size()-1){
    track++;
    if(this->noteData[track].size()>1){
      foundTrack = true;
      break;
    }
  }
}
else{
  while(track>0){
    track--;
    if(this->noteData[track].size()>1){
      foundTrack = true;
      break;
    }
  }
}
//if you couldn't find a track with a note on it, just return
if(!foundTrack){
  return;
}
for(uint16_t dist = 0; dist<this->sequenceLength; dist++){
  bool stillValid = false;
  //if the new position is in bounds
  if(this->cursorPos+dist<=this->sequenceLength){
    stillValid = true;
    //and if there's something there!
    if(this->lookupTable[track][this->cursorPos+dist] != 0){
      //move to it
      moveCursor(dist);
      setActiveTrack(track,false);
      return;
    }
  }
  if(this->cursorPos>=dist){
    stillValid = true;
    if(this->lookupTable[track][this->cursorPos-dist] != 0){
      moveCursor(-dist);
      setActiveTrack(track,false);
      return;
    }
  }
  //if it's reached the bounds
  if(!stillValid){
    return;
  }
}
}

//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void StepchildSequence::moveToNextNote(bool forward,bool endSnap){
//if there's a note on this track at all
if(this->noteData[this->activeTrack].size()>1){
  unsigned short int id = this->IDAtCursor();
  if(forward){
    for(int i = this->cursorPos; i<this->sequenceLength; i++){
      if(this->lookupTable[this->activeTrack][i] !=id && this->lookupTable[this->activeTrack][i] != 0){
        moveCursor(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][i]].startPos-this->cursorPos);
        return;
      }
    }
    if(endSnap){
      moveCursor(this->sequenceLength-this->cursorPos);
    }
    return;
  }
  else{
    for(int i = this->cursorPos; i>0; i--){
      if(this->lookupTable[this->activeTrack][i] !=id && this->lookupTable[this->activeTrack][i] != 0){
        moveCursor(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][i]].startPos-this->cursorPos);
        return;
      }
    }
    if(endSnap){
      moveCursor(-this->cursorPos);
    }
    return;
  }
}
}

void StepchildSequence::moveToNextNote(bool forward){
moveToNextNote(forward, false);
}

//View ------------------------------------------------------------------

void StepchildSequence::setViewStart(uint16_t step){
uint16_t viewLength = this->viewEnd-this->viewStart;
if(viewLength + step > this->sequenceLength){
  step = this->sequenceLength-viewLength;
}
this->viewStart = step;
this->viewEnd = step+viewLength;
}

void StepchildSequence::moveView(int16_t val){
if(val < 0 && abs(val)>this->viewStart){
  setViewStart(0);
}
else{
  setViewStart(this->viewStart+val);
}
}

//moving the cursor around
int16_t StepchildSequence::moveCursor(int moveAmount){
  int16_t amt = 0;
  //if you're trying to move back at the start
  if(this->cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && this->cursorPos+moveAmount<0){
    amt = this->cursorPos;
    this->cursorPos = 0;
  }
  else{
    this->cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(this->cursorPos > this->sequenceLength) {
    amt += (this->sequenceLength-this->cursorPos);
    this->cursorPos = this->sequenceLength;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos<this->cursorPos)
      changeNoteLength(this->cursorPos-this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos,this->activeTrack,this->noteData[this->activeTrack].size()-1);
  }
  //Move the view along with the cursor
  if(this->cursorPos<this->viewStart+this->subDivision && this->viewStart>0){
    moveView(this->cursorPos - (this->viewStart+this->subDivision));
  }
  else if(this->cursorPos > this->viewEnd-this->subDivision && this->viewEnd<this->sequenceLength){
    moveView(this->cursorPos - (this->viewEnd-this->subDivision));
  }
  //update the LEDs
  menuText = ((moveAmount>0)?(stepsToPosition(this->cursorPos,true)+">>"):("<<"+stepsToPosition(this->cursorPos,true)));
  return amt;
}

void StepchildSequence::setCursor(uint16_t loc){
  moveCursor(loc-this->cursorPos);
}

void StepchildSequence::moveCursorIntoView(){
  if (this->cursorPos < 0) {
    this->cursorPos = 0;
  }
  if (this->cursorPos > this->sequenceLength-1) {
    this->cursorPos = this->sequenceLength-1;
  }
  if (this->cursorPos < this->viewStart) {
    moveView(this->cursorPos-this->viewStart);
  }
  if (this->cursorPos >= this->viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(this->cursorPos-this->viewEnd);
  }
}

//------------------------------------------------------------------------------------------------------------------------------
bool StepchildSequence::isInView(int target){
if(target>=this->viewStart && target<=this->viewEnd)
  return true;
else
  return false;
}


void StepchildSequence::changeSubDivInt(bool down){
changeSubDivInt(down,false);
}

void StepchildSequence::changeSubDivInt(bool down, bool limitToView){
if(down){
  if(this->subDivision>3 && (!limitToView || (this->subDivision*this->viewScale)>2))
    this->subDivision /= 2;
  else if(this->subDivision == 3)
    this->subDivision = 1;
}
else{
  if(this->subDivision == 1)//if it's one, set it to 3
    this->subDivision = 3;
  else if(this->subDivision !=  96 && this->subDivision != 32){
    //if triplet mode
    if(!(this->subDivision%2))
      this->subDivision *= 2;
    else if(!(this->subDivision%3))
      this->subDivision *=2;
  }
}
menuText = "~"+stepsToMeasures(this->subDivision);
}

void StepchildSequence::toggleTriplets(){
//this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
if(this->subDivision == 192){
  this->subDivision = 32;
}
else if(!(this->subDivision%3)){//if it's in 1/4 mode...
  this->subDivision = 2*this->subDivision/3;//set it to triplet mode
}
else if(!(this->subDivision%2)){//if it was in triplet mode...
  this->subDivision = 3*this->subDivision/2;//set it to 1/4 mode
}
menuText = stepsToMeasures(this->subDivision);
}

//makes sure scale/viewend line up with the display
void StepchildSequence::checkView(){
if(this->viewEnd>this->sequenceLength){
  this->viewScale = float(96)/float(this->sequenceLength);
  this->viewEnd = this->sequenceLength+1;
}
}
//zooms in/out
void StepchildSequence::zoom(bool in){
uint16_t viewLength = this->viewEnd-this->viewStart;
if(!in && viewLength<this->sequenceLength){
  this->viewScale /= 2;
}
else if(in && viewLength/2>1){
  this->viewScale *= 2;
}  
this->viewStart = 0;
this->viewEnd = 96/this->viewScale;
checkView();
changeSubDivInt(in);
moveCursorIntoView();
menuText = stepsToMeasures(this->viewStart)+"<-->"+stepsToMeasures(this->viewEnd)+"(~"+stepsToMeasures(this->subDivision)+")";
}
bool StepchildSequence::areThereAnyNotes(){
for(uint8_t t = 0; t<this->noteData.size(); t++){
  if(this->noteData[t].size()>1){
    return true;
  }
}
return false;
}
//checks for notes above or below a track
bool StepchildSequence::areThereMoreNotes(bool above){
if(this->trackData.size()>this->maxTracksShown){
  if(!above){
    for(int track = this->endTrack+1; track<this->trackData.size();track++){
      if(this->noteData[track].size()-1>0)
      return true;
    }
  }
  else if(above){
    for(int track = this->startTrack-1; track>=0; track--){
      if(this->noteData[track].size()-1>0)
      return true;
    }
  }
}
return false;
}

bool StepchildSequence::playing(){
  return (playState == PLAYING);
}
bool StepchildSequence::recording(){
  return (playState == RECORDING);
}
bool StepchildSequence::liveLooping(){
  return (playState == LIVELOOPING);
}
void StepchildSequence::setNormalMode(){
  stop();
  if(arp.isActive){
    arp.stop();
  }
  if(recordingToAutotrack){
    recordingToAutotrack = false;
    controls.counterA = 0;
    controls.counterB = 0;
  }
  #ifndef HEADLESS
  MIDI1.disconnectCallbackFromType(midi::Clock);
  MIDI1.disconnectCallbackFromType(midi::Start);
  MIDI1.disconnectCallbackFromType(midi::Stop);
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.disconnectCallbackFromType(midi::Clock);
  MIDI0.disconnectCallbackFromType(midi::Start);
  MIDI0.disconnectCallbackFromType(midi::Stop);
  MIDI0.disconnectCallbackFromType(midi::NoteOn);
  MIDI0.disconnectCallbackFromType(midi::NoteOff);
  MIDI0.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.setHandleNoteOn(handleNoteOn_Normal);
  MIDI0.setHandleNoteOff(handleNoteOff_Normal);
  MIDI0.setHandleStart(handleStart_Normal);
  MIDI0.setHandleStop(handleStop_Normal);

  MIDI1.setHandleNoteOn(handleNoteOn_Normal);
  MIDI1.setHandleNoteOff(handleNoteOff_Normal);
  MIDI1.setHandleStart(handleStart_Normal);
  MIDI1.setHandleStop(handleStop_Normal);

  MIDI0.setHandleControlChange(handleCC_Normal);
  MIDI1.setHandleControlChange(handleCC_Normal);

  MIDI0.setHandlePitchBend(handlePB);
  MIDI1.setHandlePitchBend(handlePB);
  #endif
}

void StepchildSequence::togglePlay(){
  if(playing()){
    playState = STOPPED;
  }
  else{
    playState = PLAYING;
  }
  //if it's looping, set the playhead to the activeLoop start
  if(isLooping)
    playheadPos = loopData[activeLoop].start;
  else
    playheadPos = 0;
  if(playing()){
    if(recording()){
      toggleRecording(waitForNoteBeforeRec);
    }
    #ifndef HEADLESS
    MIDI1.setHandleNoteOn(handleNoteOn_Normal);
    MIDI1.setHandleNoteOff(handleNoteOff_Normal);
    MIDI1.setHandleClock(handleClock_playing);
    MIDI1.setHandleStart(handleStart_playing);
    MIDI1.setHandleStop(handleStop_playing);

    MIDI0.setHandleNoteOn(handleNoteOn_Normal);
    MIDI0.setHandleNoteOff(handleNoteOff_Normal);
    MIDI0.setHandleClock(handleClock_playing);
    MIDI0.setHandleStart(handleStart_playing);
    MIDI0.setHandleStop(handleStop_playing);
    #endif

    sequenceClock.startTime = micros();
    if(arp.isActive){
      arp.start();
    }
    MIDI.sendStart();
  }
  else{
    stop();
    setNormalMode();
    MIDI.sendStop();
    globalModifiers.velocity.value = 0;
    globalModifiers.chance.value = 0;
    globalModifiers.pitch.value = 0;
    CV.off();
  }
}
void StepchildSequence::toggleRecording(bool butWait){
  if(recording()){
    playState = STOPPED;
  }
  else{
    playState = RECORDING;
  }
  //if it stopped recording
  if(!recording())
    cleanupRecording(recheadPos);
  //if it's recording to the loop
  if(recMode == ONESHOT || recMode == CURRENT_LOOP)
    recheadPos = loopData[activeLoop].start;
  // else
  //   recheadPos = ONESHOT;
  if(butWait)
    waitingToReceiveANote = true;
  else
    waitingToReceiveANote = false;
  if(recording()){
    if(playing()){
      togglePlay();
    }
    stop();
    #ifndef HEADLESS
    //disconnecting all the midi callbacks!
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);
    MIDI1.disconnectCallbackFromType(midi::Clock);
    MIDI1.disconnectCallbackFromType(midi::Start);
    MIDI1.disconnectCallbackFromType(midi::Stop);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
    MIDI0.disconnectCallbackFromType(midi::Clock);
    MIDI0.disconnectCallbackFromType(midi::Start);
    MIDI0.disconnectCallbackFromType(midi::Stop);

    //reconnecting the midi callbacks
    MIDI1.setHandleNoteOn(handleNoteOn_Recording);
    MIDI1.setHandleNoteOff(handleNoteOff_Recording);
    MIDI1.setHandleClock(handleClock_recording);
    MIDI1.setHandleStart(handleStart_recording);
    MIDI1.setHandleStop(handleStop_recording);
    MIDI1.setHandleControlChange(handleCC_Recording);

    MIDI0.setHandleNoteOn(handleNoteOn_Recording);
    MIDI0.setHandleNoteOff(handleNoteOff_Recording);
    MIDI0.setHandleClock(handleClock_recording);
    MIDI0.setHandleStart(handleStart_recording);
    MIDI0.setHandleStop(handleStop_recording);
    MIDI0.setHandleControlChange(handleCC_Recording);
    #endif
    sequenceClock.startTime = micros();
  }
  else{//go back to normal mode
    setNormalMode();
  }
}

//looks for autotracks to trigger and triggers them
void StepchildSequence::triggerAutotracks(uint8_t trackID, bool state){
  for(uint8_t i = 0; i<autotrackData.size(); i++){
    switch(autotrackData[i].triggerSource){
      case GLOBAL_TRIGGER:
        break;
      case TRACK_TRIGGER:
        //if it's a targeted autotrack
        if(autotrackData[i].triggerTarget == trackID){
          //triggering it on
          if(state){
            autotrackData[i].isActive = true;
            autotrackData[i].playheadPos = 0;
          }
          //triggering it off
          else if(autotrackData[i].gated){
            autotrackData[i].isActive = false;
            autotrackData[i].playheadPos = 0;
          }
        }
        break;
      case CHANNEL_TRIGGER:
        //if it's a targeted autotrack
        if(autotrackData[i].triggerTarget == trackData[trackID].channel){
          //triggering it on
          if(state){
            autotrackData[i].isActive = true;
            autotrackData[i].playheadPos = 0;
          }
          //triggering it off
          else if(autotrackData[i].gated){
            autotrackData[i].isActive = false;
            autotrackData[i].playheadPos = 0;
          }
        }
        break;
    }
  }
}

StepchildSequence sequence;

  /*
  ----------------------------------------------------------
                    MIDI Input Handlers
  ----------------------------------------------------------
  Fns outside the Sequence class which are used as callbacks
  */

void handleStop_playing(){
  sequence.startedPlaying = false;
  sequence.stop();
}

void handleClock_playing(){
  sequenceClock.receivedClockMessage = true;
}

void handleStart_playing(){
  sequence.startedPlaying = true;
  sequenceClock.startTime = micros();
}

void handleClock_recording(){
  sequenceClock.receivedClockMessage = true;
}

void handleStart_recording(){
  sequence.startedPlaying = true;
  sequenceClock.startTime = micros();
  if(waitForNoteBeforeRec && waitingToReceiveANote){
    waitingToReceiveANote = false;
  }
}

void handleStop_recording(){
  sequence.startedPlaying = false;
}

void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  sequence.writeNoteOn(sequence.recheadPos, note, velocity, channel);
  MIDI.sendThruOn(channel, note, velocity);
  waitingToReceiveANote = false;
  recentNote.pitch = note;
  recentNote.vel = velocity;
  recentNote.channel = channel;
  noteOnReceived = true;
  receivedNotes.addNote(note,velocity,channel);
}

void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  sequence.writeNoteOff(sequence.recheadPos, note, channel);
  MIDI.sendThruOff(channel, note);
  waitingToReceiveANote = false;
  noteOffReceived = true;

  //is this a good idea? idk (it messed w/ live loop so i'm disabling it)
  //if you need this, you should have a "recentNoteOff" variable too
  // recentNote.pitch = note;
  // recentNote.vel = velocity;
  // recentNote.channel = channel;
  receivedNotes.subNote(note);
}

void handlePB(uint8_t ch, int val){
  MIDI.sendThruPB(ch,val);
}

void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value){
  sequence.writeCC(sequence.recheadPos,channel,cc,value);
  MIDI.sendThruCC(channel,cc,value);
  recentCC.cc = cc;
  recentCC.val = value;
  recentCC.channel = channel;
  waitingToReceiveANote = false;
}

void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value){
  MIDI.sendThruCC(channel,cc,value);
  recentCC.cc = cc;
  recentCC.val = value;
  recentCC.channel = channel;
}

void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = sequence.getTrackWithPitch(note);
  if(track != -1){
    sequence.trackData[track].noteLastSent = note;
  }
  MIDI.sendThruOn(channel, note, velocity);
  recentNote.pitch = note;
  recentNote.vel = velocity;
  recentNote.channel = channel;
  noteOnReceived = true;

  receivedNotes.addNote(note,velocity,channel);  
}

void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = sequence.getTrackWithPitch(note);
  if(track != -1){
    sequence.trackData[track].noteLastSent = 255;
  }
  MIDI.sendThruOff(channel, note);
  noteOffReceived = true;
  receivedNotes.subNote(note);
}

void handleStart_Normal(){
  if(sequenceClock.clockSource == EXTERNAL_CLOCK){
    if(!sequence.playing() && !sequence.recording()){
      sequence.togglePlay();
    }
  }
}

void handleStop_Normal(){
  if(sequenceClock.clockSource == EXTERNAL_CLOCK){
    if(sequence.playing()){
      sequence.togglePlay();
    }
  }
}

