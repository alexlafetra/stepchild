void updateLookupData_track(unsigned short int track){
    lookupData[track].assign(seqEnd,0);//blank the track
    for(int id = 1; id<seqData[track].size()-1+1; id++){//for each note in data
      unsigned short int start = seqData[track][id].startPos;
      unsigned short int end = seqData[track][id].endPos;
      for(int i = start; i<end; i++){
        lookupData[track][i] = id;
      }
    }
}

//idrk why this is broken, but this is super broken (but maybe not???)
void updateLookupData(){
  for(int track = 0; track<trackData.size(); track++){//for each track
    updateLookupData_track(track);
  }
}

void writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value){
  for(uint8_t dt = 0; dt < autotrackData.size(); dt++){
    //if the track is primed and is recording externally
    if(autotrackData[dt].isPrimed && autotrackData[dt].recordFrom == 0){
      //if the channel and control number match
      if(channel == autotrackData[dt].channel && controller == autotrackData[dt].control){
        autotrackData[dt].data[step] = value;
      }
    }
  }
}

void writeNoteOn(unsigned short int step, uint8_t pitch, uint8_t vel, uint8_t channel){
  int track = makeTrackWithPitch(pitch,channel);
  if(trackData[track].isPrimed){
    Note newNote(step, step, vel);//this constuctor sets the endPos of the note at the same position
    newNote.isSelected = recordedNotesAreSelected;
    if(newNote.isSelected)
      selectionCount++;
    if(lookupData[track][step] != 0){
      deleteNote(track,step);
    }
    seqData[track].push_back(newNote);
    lookupData[track][step] = seqData[track].size()-1;
    trackData[track].noteLastSent = pitch;
  }
}

void writeNoteOff(unsigned short int step, uint8_t pitch, uint8_t channel){
  int8_t track = getTrackWithPitch(pitch,channel);
  if(track == -1)
    return;
  if(trackData[track].isPrimed && trackData[track].noteLastSent != 255){
    unsigned short int note = seqData[track].size()-1;
    //if the track actually was sending, and exists
    if(trackData[track].noteLastSent != 255 && track != -1){
      seqData[track][note].endPos = step;
      trackData[track].noteLastSent = 255;
    }
  }
}

//continues notes that were started, and deletes notes in their way
void continueStep(unsigned short int step){
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].isPrimed){
      if(trackData[track].noteLastSent != 255){
        int id = seqData[track].size()-1;
        if(id>0){
          //if there's a different note at this step, delete it
          if(lookupData[track][step] != 0 && lookupData[track][step] != id){
            //if it's in overwrite mode, delete the conflicting note
            if(overwriteRecording){
              deleteNote(track, step);
              id = seqData[track].size()-1;
            }
            //if it's not in overwrite mode, then end the note 
            else{
              seqData[track][id].endPos = step;
              trackData[track].noteLastSent = 255;
              continue;
            }
          }
          lookupData[track][step] = id;
          seqData[track][id].endPos = step;
        }
      }
      //if it's not being written to, clear this step out
      else if(overwriteRecording){
        //if there's something there, and the track isn't sending (make sure it's not a tail note that just got written)
        //AND make sure that the track is primed! if not, then don't overwrite it
        if(lookupData[track][step] != 0){
          //if it's not the end of the note (since that note might have been written)
          //i don't think this will cause problems? 
          if(seqData[track][lookupData[track][step]].endPos != step){
            deleteNote_byID(track, lookupData[track][step]);
          }
        }
      }
    }
    //if the track isn't primed, play it normally
    else if(!trackData[track].isPrimed){
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
  startTime = micros();
}

void handleClock_recording(){
  gotClock = true;
}

void handleStart_recording(){
  hasStarted = true;
  startTime = micros();
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
    trackData[track].noteLastSent = note;
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
    trackData[track].noteLastSent = 255;
  }
  sendThruOff(channel, note);
  noteOffReceived = true;

  // recentNote.pitch = note;
  // recentNote.vel = velocity;
  // recentNote.channel = channel;
  receivedNotes.subNote(note);
}

void handleStart_Normal(){
  if(clockSource == EXTERNAL){
    if(!playing && !recording){
      togglePlayMode();
    }
  }
}

void handleStop_Normal(){
  if(clockSource == EXTERNAL){
    if(playing){
      togglePlayMode();
    }
  }
}

void cleanupRecording(uint16_t stopTime){
  for(int8_t i = 0; i<trackData.size(); i++){
    //if there's a note on this track
    if(seqData[i].size()-1>0){
      //if the track was sending
      if(trackData[i].noteLastSent != 255){
        //set the end of the note to the stop time
        seqData[i][seqData[i].size()-1].endPos = stopTime;
        trackData[i].noteLastSent = 255;
      }
      for(uint16_t note = 1; note<seqData[i].size(); note++){
        //if the note is fucked up
        if(seqData[i][note].startPos>=seqData[i][note].endPos){
          seqData[i][note].endPos = seqData[i][note].startPos+1;
        }
      }
    }
  }
  updateLookupData();
}

//runs while "recording" is true
void recordingLoop(){
  readMIDI();
  if(clockSource == INTERNAL){
    if(hasItBeenEnoughTime()){
      timeLastStepPlayed = micros();
      checkAutotracks();
      //if it's not in wait mode, or if it is but a note has been received
      if(!waitForNoteBeforeRec || !waitingToReceiveANote){
        continueStep(recheadPos);
        sendClock();
        recheadPos++;
        checkLoop();
      }
    }
  }
  else if(clockSource == EXTERNAL){
    if(gotClock && hasStarted){
      gotClock = false;
      continueStep(recheadPos);
      recheadPos++;
      checkLoop();
      checkAutotracks();
    }
  }
}

