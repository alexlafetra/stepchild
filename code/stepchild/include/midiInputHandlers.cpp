
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
    if(liveLoop.checkNote(channel,note,velocity))
        return;
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
    if(liveLoop.checkNote(channel,note,velocity))
        return;

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