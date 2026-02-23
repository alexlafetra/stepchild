#include "Stepchild.h"
extern Stepchild stepchild;
/*
----------------------------------------------------------
                  MIDI Input Handlers
----------------------------------------------------------
Fns outside the Sequence class which are used as callbacks
*/

void handleStop_playing(){
  stepchild.startedPlaying = false;
  stepchild.stop();
}

void handleClock_playing(){
  stepchild.clock.receivedClockMessage = true;
}

void handleStart_playing(){
  stepchild.startedPlaying = true;
  stepchild.clock.startTime = micros();
}

void handleClock_recording(){
  stepchild.clock.receivedClockMessage = true;
}

void handleStart_recording(){
  stepchild.startedPlaying = true;
  stepchild.clock.startTime = micros();
  if(stepchild.waitForNoteBeforeRec && stepchild.idlingUntilNoteReceived){
    stepchild.idlingUntilNoteReceived = false;
  }
}

void handleStop_recording(){
  stepchild.startedPlaying = false;
}

void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
    if(stepchild.liveLoop.checkNote(channel,note,velocity))
        return;
    stepchild.writeNoteOn(stepchild.recheadPos, note, velocity, channel);
    stepchild.midi.sendThruOn(channel, note, velocity);
    stepchild.idlingUntilNoteReceived = false;
    stepchild.recentNote.pitch = note;
    stepchild.recentNote.vel = velocity;
    stepchild.recentNote.channel = channel;
    stepchild.receivedNotes.addNote(note,velocity,channel);
}

void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  stepchild.writeNoteOff(stepchild.recheadPos, note, channel);
  stepchild.midi.sendThruOff(channel, note);
  stepchild.idlingUntilNoteReceived = false;

  //is this a good idea? idk (it messed w/ live loop so i'm disabling it)
  //if you need this, you should have a "stepchild.recentNoteOff" variable too
  stepchild.recentNote.pitch = note;
  stepchild.recentNote.vel = velocity;
  stepchild.recentNote.channel = channel;
  stepchild.receivedNotes.subNote(note);
}

void handlePB(uint8_t ch, int val){
  stepchild.midi.sendThruPB(ch,val);
}

void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value){
  stepchild.writeCC(stepchild.recheadPos,channel,cc,value);
  stepchild.midi.sendThruCC(channel,cc,value);
  stepchild.recentCC.cc = cc;
  stepchild.recentCC.val = value;
  stepchild.recentCC.channel = channel;
  stepchild.idlingUntilNoteReceived = false;
}

void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value){
  stepchild.midi.sendThruCC(channel,cc,value);
  stepchild.recentCC.cc = cc;
  stepchild.recentCC.val = value;
  stepchild.recentCC.channel = channel;
}

void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
    if(stepchild.liveLoop.checkNote(channel,note,velocity))
        return;

    int track = stepchild.getTrackWithPitch(note);
    if(track != -1){
        stepchild.trackData[track].noteLastSent = note;
    }
    stepchild.midi.sendThruOn(channel, note, velocity);
    stepchild.recentNote.pitch = note;
    stepchild.recentNote.vel = velocity;
    stepchild.recentNote.channel = channel;
    stepchild.receivedNotes.addNote(note,velocity,channel);
}

void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = stepchild.getTrackWithPitch(note);
  if(track != -1){
    stepchild.trackData[track].noteLastSent = 255;
  }
  stepchild.midi.sendThruOff(channel, note);

  stepchild.recentNote.pitch = note;
  stepchild.recentNote.vel = 0;
  stepchild.recentNote.channel = channel;
  
  stepchild.receivedNotes.subNote(note);
}

void handleStart_Normal(){
  if(stepchild.clock.clockSource == EXTERNAL_CLOCK){
    if(!stepchild.playing() && !stepchild.recording()){
      stepchild.togglePlay();
    }
  }
}

void handleStop_Normal(){
  if(stepchild.clock.clockSource == EXTERNAL_CLOCK){
    if(stepchild.playing()){
      stepchild.togglePlay();
    }
  }
}

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      stepchild.globalModifiers.velocity.channel = channel;
      stepchild.globalModifiers.velocity.value = val-63;
      break;
    //chance
    case 1:
      stepchild.globalModifiers.chance.channel = channel;
      stepchild.globalModifiers.chance.value = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      stepchild.globalModifiers.pitch.channel = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      stepchild.globalModifiers.pitch.value = float(val)/float(127) * 48 - 24;
      break;
    //stepchild.clock.BPM
    case 3:
      stepchild.clock.setBPM(float(val)*2);
      break;
    //swing amount
    case 4:
      stepchild.clock.swingCurve.amplitude += val-63;
    //track channel
    case 5:
      break;
  }
}