#ifndef HEADLESS
void startMIDI(){
  MIDI0.begin(MIDI_CHANNEL_OMNI);
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI4.begin(MIDI_CHANNEL_OMNI);

  MIDI0.turnThruOff();
  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  MIDI4.turnThruOff();
}


void sendMIDICC(uint8_t controller, uint8_t val, uint8_t channel){
  if(isActiveChannel(channel, 0)){
    MIDI0.sendControlChange(controller, val, channel);
  }
  if(isActiveChannel(channel, 1))
    MIDI1.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendControlChange(controller, val, channel);
}

void sendMIDIallOff(){
  MIDI0.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI1.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI2.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI3.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI4.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
}


void sendMIDInoteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
  if(isActiveChannel(channel, 0))
    MIDI0.sendNoteOn(pitch,vel,channel);
  if(isActiveChannel(channel, 1))
    MIDI1.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendNoteOn(pitch, vel, channel);
}

void sendMIDInoteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
  if(isActiveChannel(channel, 0))
    MIDI0.sendNoteOff(pitch,vel,channel);
  if(isActiveChannel(channel, 1))
    MIDI1.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendNoteOff(pitch, vel, channel);
}

void readMIDI(){
  MIDI0.read();
  MIDI1.read();
}

void sendClock(){
  MIDI0.sendRealTime(midi::Clock);
  MIDI1.sendRealTime(midi::Clock);
  MIDI2.sendRealTime(midi::Clock);
  MIDI3.sendRealTime(midi::Clock);
  MIDI4.sendRealTime(midi::Clock);
}

void sendMIDIStart(){
  MIDI0.sendRealTime(midi::Start);
  MIDI1.sendRealTime(midi::Start);
  MIDI2.sendRealTime(midi::Start);
  MIDI3.sendRealTime(midi::Start);
  MIDI4.sendRealTime(midi::Start);
}

void sendMIDIStop(){
  MIDI0.sendRealTime(midi::Stop);
  MIDI1.sendRealTime(midi::Stop);
  MIDI2.sendRealTime(midi::Stop);
  MIDI3.sendRealTime(midi::Stop);
  MIDI4.sendRealTime(midi::Stop);
}

void sendMIDIProgramChange(uint8_t port, uint8_t val, uint8_t channel){
  switch(port){
    case 0:
        MIDI0.sendProgramChange(val,channel);
        break;
    case 1:
        MIDI1.sendProgramChange(val,channel);
        break;
    case 2:
        MIDI2.sendProgramChange(val,channel);
        break;
    case 3:
        MIDI3.sendProgramChange(val,channel);
        break;
    case 4:
        MIDI4.sendProgramChange(val,channel);
        break;
  }
}
#endif