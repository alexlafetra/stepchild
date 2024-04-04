
void playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (lookupData[track][timestep] == 0){
    if(trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      if(!isArping || activeArp.source == 0 )//if the arp is off, or if it's just listening to notes from outside the seq
        MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      sentNotes.subNote(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
      triggerAutotracks(track,false);
    }
    return;
  }
  //if there's a note there
  else{
    //if it's the start of the note, or if the track wasn't sending already
    if(timestep == seqData[track][lookupData[track][timestep]].startPos || trackData[track].noteLastSent == 255){ //if it's the start
      //if it's not muted
      if(!seqData[track][lookupData[track][timestep]].muted){
        //if the track was already sending a note, send note off
        if(trackData[track].noteLastSent != 255){
          if(!isArping || activeArp.source == 0)
            MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
          trackData[track].noteLastSent = 255;
          triggerAutotracks(track,false);
        }
        //modifying chance value and pitch value and vel
        int16_t chance = seqData[track][lookupData[track][timestep]].chance;
        int16_t pitch = trackData[track].pitch;
        int16_t vel = seqData[track][lookupData[track][timestep]].velocity;
        //if the channel matches, or if the modifier is global

        //adjusting chance
        if(trackData[track].channel == chanceModifier[0] || chanceModifier[0] == 0){
          chance += chanceModifier[1];
          if(chance<0)
            chance = 0;
          else if(chance>100)
            chance = 100;
        }

        //adjusting pitch
        if(trackData[track].channel == pitchModifier[0] || pitchModifier[0] == 0){ 
          pitch += pitchModifier[1];
          if(pitch<0)
            pitch = 0;
          else if(pitch>127)
            pitch = 127;
        }

        //adjusting vel
        if(trackData[track].channel == velModifier[0] || velModifier[0] == 0){
          vel += velModifier[1];
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
          if(!isArping || activeArp.source == 0)
            MIDI.noteOn(pitch, vel, trackData[track].channel);
          trackData[track].noteLastSent = pitch;
          if(trackData[track].isLatched){
            if(!isArping || activeArp.source == 0)
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
  }
 }

void playDT(uint8_t dt, uint16_t timestep){
  //normal Autotracks that use the global timestep
  if(autotrackData[dt].triggerSource == global){
    if(autotrackData[dt].isActive){
      autotrackData[dt].play(timestep);
    }
  }
  //trigger autotracks that use internal playheads
  else{
    if(autotrackData[dt].isActive){
      autotrackData[dt].play(autotrackData[dt].playheadPos);
      autotrackData[dt].playheadPos++;
    }
    else{
      autotrackData[dt].playheadPos = 0;
    }
  }
}

void playStep(uint16_t timestep) {
  playPCData(timestep);
  //playing each track
  for (int track = 0; track < trackData.size(); track++) {
    //if it's unmuted or solo'd, play it
    if(!trackData[track].isMuted || trackData[track].isSolo)
      playTrack(track,timestep);
  }
  //playing autotracks too
  for(uint8_t dT = 0; dT < autotrackData.size(); dT++){
    playDT(dT,timestep);
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
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].noteLastSent != 255){
      MIDI.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      trackData[track].noteLastSent = 255;
    }
    else{
      MIDI.noteOff(trackData[track].pitch, 0, trackData[track].channel);
    }
  }
  sentNotes.clear();
}

