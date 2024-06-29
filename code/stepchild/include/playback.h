void playNote(Note& note, uint8_t track, uint16_t timestep){
  //if it's the start of the note, or if the track wasn't sending already
  if(timestep == note.startPos || sequence.trackData[track].noteLastSent == 255){ //if it's the start
    //if it's not muted
    if(!note.isMuted()){
      //if the track was already sending a note, send note off
      if(sequence.trackData[track].noteLastSent != 255){
        if(!arp.isActive || arp.source == EXTERNAL)
          MIDI.noteOff(sequence.trackData[track].noteLastSent, 0, sequence.trackData[track].channel);
        sequence.trackData[track].noteLastSent = 255;
        triggerAutotracks(track,false);
      }

      //modifying chance value and pitch value and vel
      int16_t chance = note.chance;
      int16_t pitch = sequence.trackData[track].pitch;
      int16_t vel = note.velocity;

      //superposition pitch mod, if there is a superposition
      if(note.superposition.pitch != 255 && (random(100)<note.superposition.odds || note.isSuperpositioned())){
        pitch = note.superposition.pitch;
        note.setSuperpositioned(true);
      }

      //global mods
      //adjusting chance
      if(sequence.trackData[track].channel == globalModifiers.chance[0] || globalModifiers.chance[0] == 0){
        chance += globalModifiers.chance[1];
        if(chance<0)
          chance = 0;
        else if(chance>100)
          chance = 100;
      }

      //adjusting pitch
      if(sequence.trackData[track].channel == globalModifiers.pitch[0] || globalModifiers.pitch[0] == 0){ 
        pitch += globalModifiers.pitch[1];
        if(pitch<0)
          pitch = 0;
        else if(pitch>127)
          pitch = 127;
      }

      //adjusting vel
      if(sequence.trackData[track].channel == globalModifiers.velocity[0] || globalModifiers.velocity[0] == 0){
        vel += globalModifiers.velocity[1];
        if(vel<0)
          vel = 0;
        else if(vel>127)
          vel = 127;
      }
      //if chance is 100%
      if(chance > random(100)){
        //if it's part of a muteGroup
        if(sequence.trackData[track].muteGroup!=0){
          muteGroups(track, sequence.trackData[track].muteGroup);
        }
        if(!arp.isActive || arp.source == EXTERNAL)
          MIDI.noteOn(pitch, vel, sequence.trackData[track].channel);
        sequence.trackData[track].noteLastSent = pitch;
        if(sequence.trackData[track].isLatched()){
          if(!arp.isActive || arp.source == EXTERNAL)
            MIDI.noteOff(pitch, 0, sequence.trackData[track].channel);
        }
        sentNotes.addNote(pitch,vel,sequence.trackData[track].channel);
        triggerAutotracks(track,true);
        return;
      }
      //if the note fails to fire, set the pitch flag anyway so the sequencer knows not to try and play it again
      else{
        sequence.trackData[track].noteLastSent = pitch;
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

void playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (sequence.lookupTable[track][timestep] == 0){
    if(sequence.trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      if(!arp.isActive || arp.source == EXTERNAL)//if the arp is off, or if it's just listening to notes from outside the seq
        MIDI.noteOff(sequence.trackData[track].noteLastSent, 0, sequence.trackData[track].channel);
      sentNotes.subNote(sequence.trackData[track].noteLastSent);
      sequence.trackData[track].noteLastSent = 255;
      triggerAutotracks(track,false);
    }
    return;
  }
  //if there's a note there
  else{
    playNote(sequence.noteData[track][sequence.lookupTable[track][timestep]],track, timestep);
  }
 }

void playDT(uint8_t dt, uint16_t timestep){
  //normal Autotracks that use the global timestep
  if(sequence.autotrackData[dt].triggerSource == global){
    if(sequence.autotrackData[dt].isActive){
      sequence.autotrackData[dt].sendData(timestep);
    }
  }
  //trigger autotracks that use internal playheads
  else{
    if(sequence.autotrackData[dt].isActive){
      sequence.autotrackData[dt].sendData(sequence.autotrackData[dt].playheadPos);
      sequence.autotrackData[dt].playheadPos++;
    }
    else{
      sequence.autotrackData[dt].playheadPos = 0;
    }
  }
}

void playStep(uint16_t timestep) {
  playPCData(timestep);
  //playing each track
  for (int track = 0; track < sequence.trackData.size(); track++) {
    //if it's unmuted or solo'd, play it
    if(!sequence.trackData[track].isMuted() || sequence.trackData[track].isSolo())
      playTrack(track,timestep);
  }
  //playing autotracks too
  for(uint8_t dT = 0; dT < sequence.autotrackData.size(); dT++){
    sequence.autotrackData[dT].play(timestep);
  }
  CV.check();
}


#ifdef HEADLESS
  void sendThruOn(uint8_t c, uint8_t n, uint8_t v){
    return;
  }
  void sendThruOff(uint8_t c, uint8_t n){
    return;
  }
#else
void sendThruOn(uint8_t channel, uint8_t note, uint8_t vel){
  //if it's a valid thru & channel
  if(MIDI.isThru(0) && MIDI.isChannelActive(channel, 0)){
    MIDI0.sendNoteOn(note,vel,channel);
  }
  if(MIDI.isThru(1) && MIDI.isChannelActive(channel, 1)){
    MIDI1.sendNoteOn(note,vel,channel);
  }
  if(MIDI.isThru(2) && MIDI.isChannelActive(channel, 2)){
    MIDI2.sendNoteOn(note,vel,channel);
  }
  if(MIDI.isThru(3) && MIDI.isChannelActive(channel, 3)){
    MIDI3.sendNoteOn(note,vel,channel);
  }
  if(MIDI.isThru(4) && MIDI.isChannelActive(channel, 4)){
    MIDI4.sendNoteOn(note,vel,channel);
  }
}

void sendThruOff(uint8_t channel, uint8_t note){
  //if it's a valid thru & channel
  if(MIDI.isThru(0) && MIDI.isChannelActive(channel, 0)){
    MIDI0.sendNoteOff(note,0,channel);
  }
  if(MIDI.isThru(1) && MIDI.isChannelActive(channel, 1)){
    MIDI1.sendNoteOff(note,0,channel);
  }
  if(MIDI.isThru(2) && MIDI.isChannelActive(channel, 2)){
    MIDI2.sendNoteOff(note,0,channel);
  }
  if(MIDI.isThru(3) && MIDI.isChannelActive(channel, 3)){
    MIDI3.sendNoteOff(note,0,channel);
  }
  if(MIDI.isThru(4) && MIDI.isChannelActive(channel, 4)){
    MIDI4.sendNoteOff(note,0,channel);
  }
}
#endif

//stops all midi sends, clears playlist
void stop(){
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(sequence.trackData[track].noteLastSent != 255){
      MIDI.noteOff(sequence.trackData[track].noteLastSent, 0, sequence.trackData[track].channel);
      sequence.trackData[track].noteLastSent = 255;
      //reset superposition flags
      for(uint8_t i = 1; i<sequence.noteData[track].size(); i++){
        sequence.noteData[track][i].setSuperpositioned(false);
      }
    }
    else{
      MIDI.noteOff(sequence.trackData[track].pitch, 0, sequence.trackData[track].channel);
    }

  }
  sentNotes.clear();
}

void updateLookupData_track(unsigned short int track){
    sequence.lookupTable[track].assign(sequence.sequenceLength,0);//blank the track
    for(int id = 1; id<sequence.noteData[track].size()-1+1; id++){//for each note in data
      unsigned short int start = sequence.noteData[track][id].startPos;
      unsigned short int end = sequence.noteData[track][id].endPos;
      for(int i = start; i<end; i++){
        sequence.lookupTable[track][i] = id;
      }
    }
}

//idrk why this is broken, but this is super broken (but maybe not???)
void updateLookupData(){
  for(int track = 0; track<sequence.trackData.size(); track++){//for each track
    updateLookupData_track(track);
  }
}

void writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value){
  for(uint8_t dt = 0; dt < sequence.autotrackData.size(); dt++){
    //if the track is primed and is recording externally
    if(sequence.autotrackData[dt].isPrimed && sequence.autotrackData[dt].recordFrom == 0){
      //if the channel and control number match
      if(channel == sequence.autotrackData[dt].channel && controller == sequence.autotrackData[dt].control){
        sequence.autotrackData[dt].data[step] = value;
      }
    }
  }
}

void writeNoteOn(unsigned short int step, uint8_t pitch, uint8_t vel, uint8_t channel){
  uint8_t trackID = makeTrackWithPitch(pitch,channel);
  if(sequence.trackData[trackID].isPrimed()){
    Note newNote(step, step, vel);//this constuctor sets the endPos of the note at the same position
    newNote.setSelected(recordedNotesAreSelected);
    if(newNote.isSelected())
      sequence.selectionCount++;
    if(sequence.lookupTable[trackID][step] != 0){
      sequence.deleteNote(trackID,step);
    }
    sequence.noteData[trackID].push_back(newNote);
    sequence.lookupTable[trackID][step] = sequence.noteData[trackID].size()-1;
    sequence.trackData[trackID].noteLastSent = pitch;
  }
}

void writeNoteOff(unsigned short int step, uint8_t pitch, uint8_t channel){
  int8_t track = getTrackWithPitch(pitch,channel);
  if(track == -1)
    return;
  if(sequence.trackData[track].isPrimed() && sequence.trackData[track].noteLastSent != 255){
    unsigned short int note = sequence.noteData[track].size()-1;
    //if the track actually was sending, and exists
    if(sequence.trackData[track].noteLastSent != 255 && track != -1){
      sequence.noteData[track][note].endPos = step;
      sequence.trackData[track].noteLastSent = 255;
    }
  }
}

//continues notes that were started, and dels notes in their way
void continueStep(unsigned short int step){
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(sequence.trackData[track].isPrimed()){
      if(sequence.trackData[track].noteLastSent != 255){
        int id = sequence.noteData[track].size()-1;
        if(id>0){
          //if there's a different note at this step, del it
          if(sequence.lookupTable[track][step] != 0 && sequence.lookupTable[track][step] != id){
            //if it's in overwrite mode, del the conflicting note
            if(overwriteRecording){
              sequence.deleteNote(track, step);
              id = sequence.noteData[track].size()-1;
            }
            //if it's not in overwrite mode, then end the note 
            else{
              sequence.noteData[track][id].endPos = step;
              sequence.trackData[track].noteLastSent = 255;
              continue;
            }
          }
          sequence.lookupTable[track][step] = id;
          sequence.noteData[track][id].endPos = step;
        }
      }
      //if it's not being written to, clear this step out
      else if(overWriteNotesWithEmptiness){
        //if there's something there, and the track isn't sending (make sure it's not a tail note that just got written)
        //AND make sure that the track is primed! if not, then don't overwrite it
        if(sequence.lookupTable[track][step] != 0){
          //if it's not the end of the note (since that note might have been written)
          //i don't think this will cause problems? 
          if(sequence.noteData[track][sequence.lookupTable[track][step]].endPos != step){
            sequence.deleteNote_byID(track, sequence.lookupTable[track][step]);
          }
        }
      }
    }
    //if the track isn't primed, play it normally
    else if(!sequence.trackData[track].isPrimed()){
      playTrack(track,step);
    }
  }
}

//does nothing, just a placeholder
void defaultNoteHandlerFunction(uint8_t channel, uint8_t pitch, uint8_t vel){
  return;
}

//these functions are called whenever notes are sent/received
//you can set them to special functions for different applications
void (* noteOnReceivedHandlerFunc)(uint8_t, uint8_t, uint8_t) = defaultNoteHandlerFunction;
void (* noteOffReceivedHandlerFunc)(uint8_t, uint8_t, uint8_t) = defaultNoteHandlerFunction;
void (* noteOnSentHandlerFunc)(uint8_t, uint8_t, uint8_t) = defaultNoteHandlerFunction;
void (* noteOffSentHandlerFunc)(uint8_t, uint8_t, uint8_t) = defaultNoteHandlerFunction;

void handleStop_playing(){
  hasStarted = false;
  stop();
}

void handleClock_playing(){
  gotClock = true;
}

void handleStart_playing(){
  hasStarted = true;
  sequenceClock.startTime = micros();
}

void handleClock_recording(){
  gotClock = true;
}

void handleStart_recording(){
  hasStarted = true;
  sequenceClock.startTime = micros();
  if(waitForNoteBeforeRec && waitingToReceiveANote){
    waitingToReceiveANote = false;
  }
}

void handleStop_recording(){
  hasStarted = false;
}

void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOn(recheadPos, note, velocity, channel);
  sendThruOn(channel, note, velocity);
  waitingToReceiveANote = false;
  recentNote.pitch = note;
  recentNote.vel = velocity;
  recentNote.channel = channel;
  noteOnReceived = true;
  noteOnReceivedHandlerFunc(channel,note,velocity);

  receivedNotes.addNote(note,velocity,channel);
}

void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOff(recheadPos, note, channel);
  sendThruOff(channel, note);
  waitingToReceiveANote = false;
  noteOffReceived = true;

  //is this a good idea? idk (it messed w/ live loop so i'm disabling it)
  //if you need this, you should have a "recentNoteOff" variable too
  // recentNote.pitch = note;
  // recentNote.vel = velocity;
  // recentNote.channel = channel;

  noteOffReceivedHandlerFunc(channel,note,velocity);
  
  receivedNotes.subNote(note);
}

void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value){
  writeCC(recheadPos,channel,cc,value);
  recentCC.cc = cc;
  recentCC.val = value;
  recentCC.channel = channel;
  waitingToReceiveANote = false;
}

void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value){
  recentCC.cc = cc;
  recentCC.val = value;
  recentCC.channel = channel;
}

void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = getTrackWithPitch(note);
  if(track != -1){
    sequence.trackData[track].noteLastSent = note;
  }
  sendThruOn(channel, note, velocity);
  recentNote.pitch = note;
  recentNote.vel = velocity;
  recentNote.channel = channel;
  noteOnReceived = true;

  receivedNotes.addNote(note,velocity,channel);  
}

void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = getTrackWithPitch(note);
  if(track != -1){
    sequence.trackData[track].noteLastSent = 255;
  }
  sendThruOff(channel, note);
  noteOffReceived = true;

  // recentNote.pitch = note;
  // recentNote.vel = velocity;
  // recentNote.channel = channel;
  receivedNotes.subNote(note);
}

void handleStart_Normal(){
  if(clockSource == EXTERNAL_CLOCK){
    if(!playing && !recording){
      togglePlayMode();
    }
  }
}

void handleStop_Normal(){
  if(clockSource == EXTERNAL_CLOCK){
    if(playing){
      togglePlayMode();
    }
  }
}


//this checks loops bounds, moves to next loop, and cuts loop
void checkLoop(){
  if(playing){
    if (playheadPos > sequence.loopData[sequence.activeLoop].end-1) { //if the timestep is past the end of the loop, loop it to the start
      sequence.loopCount++;
      if(sequence.loopCount > sequence.loopData[sequence.activeLoop].reps){
        sequence.nextLoop();
        //reset all the superposition flags
        for(uint8_t t = 0; t<sequence.trackData.size(); t++){
          for(uint16_t n = 1; n<sequence.noteData[t].size(); n++){
            sequence.noteData[t][n].setSuperpositioned(false);
          }
        }
      }
      playheadPos = sequence.loopData[sequence.activeLoop].start;
      if(!sequence.isLooping)
        togglePlayMode();
    }
  }
  else if(recording){
    //one-shot record to current loop, without looping
    if(recMode == ONESHOT){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        toggleRecordingMode(waitForNoteBeforeRec);
      }
    }
    //record to one loop over and over again
    else if(recMode == LOOP_MODE){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        recheadPos = sequence.loopData[sequence.activeLoop].start;
      }
    }
    //record to loops as they play in sequence
    else if(recMode == LOOPSEQUENCE){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        sequence.cutLoop();
        sequence.loopCount++;
        if(sequence.loopData[sequence.activeLoop].reps>=sequence.loopCount){
         sequence. nextLoop();
        }
        recheadPos = sequence.loopData[sequence.activeLoop].start;
      }
    }
  }
}

void cleanupRecording(uint16_t stopTime){
  for(int8_t i = 0; i<sequence.trackData.size(); i++){
    //if there's a note on this track
    if(sequence.noteData[i].size()-1>0){
      //if the track was sending
      if(sequence.trackData[i].noteLastSent != 255){
        //set the end of the note to the stop time
        sequence.noteData[i][sequence.noteData[i].size()-1].endPos = stopTime;
        sequence.trackData[i].noteLastSent = 255;
      }
      for(uint16_t note = 1; note<sequence.noteData[i].size(); note++){
        //if the note is fucked up
        if(sequence.noteData[i][note].startPos>=sequence.noteData[i][note].endPos){
          sequence.noteData[i][note].endPos = sequence.noteData[i][note].startPos+1;
        }
      }
    }
  }
  updateLookupData();
}

//runs while "recording" is true
void recordingLoop(){
  MIDI.read();
  if(clockSource == INTERNAL_CLOCK){
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
  else if(clockSource == EXTERNAL_CLOCK){
    if(gotClock && hasStarted){
      gotClock = false;
      continueStep(recheadPos);
      recheadPos++;
      checkLoop();
      checkAutotracks();
    }
  }
}

