
void playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (lookupData[track][timestep] == 0){
    if(trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      if(!isArping || activeArp.source == 1 )
        sendMIDInoteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      sentNotes.subNote(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
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
          if(!isArping || activeArp.source == 1)
            sendMIDInoteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
          trackData[track].noteLastSent = 255;
        }
        //modifying chance value and pitch value and vel
        int16_t chance = seqData[track][lookupData[track][timestep]].chance;
        int16_t pitch = trackData[track].pitch;
        int16_t vel = seqData[track][lookupData[track][timestep]].velocity;
        //if the channel matches, or if the modifier is global
        if(trackData[track].channel == chanceModifier[0] || chanceModifier[0] == 0){
          chance += chanceModifier[1];
          if(chance<0)
            chance = 0;
          else if(chance>100)
            chance = 100;
        }
        if(trackData[track].channel == pitchModifier[0] || pitchModifier[0] == 0){ 
          pitch += pitchModifier[1];
          if(pitch<0)
            pitch = 0;
          else if(pitch>127)
            pitch = 127;
        }
        if(trackData[track].channel == velModifier[0] || velModifier[0] == 0){
          vel += velModifier[1];
          if(vel<0)
            vel = 0;
          else if(vel>127)
            vel = 127;
        }
        //if chance is 100%
        if(chance >= 100 || seqData[track][lookupData[track][timestep]].chance>random(100)){
          //if it's part of a muteGroup
          if(trackData[track].muteGroup!=0){
            muteGroups(track, trackData[track].muteGroup);
          }
          if(!isArping || activeArp.source == 1)
            sendMIDInoteOn(pitch, vel, trackData[track].channel);
          trackData[track].noteLastSent = pitch;
          if(trackData[track].isLatched){
            if(!isArping || activeArp.source == 1)
              sendMIDInoteOff(pitch, 0, trackData[track].channel);
          }
          sentNotes.addNote(pitch,vel,trackData[track].channel);
          return;
        }
      }
    }
  }
}

void playDT(uint8_t dt, uint16_t timestep){
  if(dataTrackData[dt].isActive){
    if(timestep<dataTrackData[dt].data.size()){
      if(dataTrackData[dt].data[timestep] != 255)
        dataTrackData[dt].play(timestep);
    }
  }
}

void playStep(uint16_t timestep) {
    //playing dataTracks too
  for(uint8_t dT = 0; dT < dataTrackData.size(); dT++){
    playDT(dT,timestep);
  }
  //playing each track
  for (int track = 0; track < trackData.size(); track++) {
    //if it's unmuted or solo'd, play it
    if(!trackData[track].isMuted || trackData[track].isSolo)
      playTrack(track,timestep);
  }
  checkCV();
}