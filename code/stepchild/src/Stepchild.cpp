#include <Arduino.h>
#include <vector>
#include <algorithm>
#include "Stepchild.h"
#include "StepchildFileSystem.h"
#include "StepchildArpeggiator.h"
#include "StepchildFileSystem.h"
#include "StepchildClock.h"
#include "StepchildCV.h"
#include "StepchildMIDI.h"
#include "classes/Track.h"
#include "classes/Note.h"
#include "classes/Autotrack.h"
#include "classes/ProgramChange.h"
#include "classes/SequenceTemplate.h"
#include "classes/LiveLooper.h"
#include "classes/Knob.h"
#include "classes/PlayList.h"
#include "commonStructs.h"
#include "commonEnums.h"
#include "pins.h"
#include "StepchildGraphics.h"
#include "stringPatch.h"
#include "guiUtilities.h"
#include "classes/Clipboard.h"

using namespace std;

extern void drawSeq();

// /*
// Generic Keyboard
// tracks on a single channel from pitch 1-127
// */

vector<Track> genericKeyboardTracks(){
    vector<Track> tracks = {};
    for(int8_t i = 127; i>=0; i--){
        tracks.push_back(Track(i,1));
    }
    return tracks;
}

// /*
// SP404mk2
// 16 tracks on one channel, from pitch 36 to 52
// */

vector<Track> sp404mk2Tracks(){
    vector<Track> tracks = {};
    for(uint8_t i = 0; i<16; i++){
        tracks.push_back(Track(51-i,1));
    }
    return tracks;
}

// /*
// Korg Volca
// */

// /*
// Alesis SR16
// */

//constructor
Stepchild::Stepchild() : 
  filesystem(this),
  arpeggiator(this),
  selectionBox(this),
  clipboard(this)
{
}

const SequenceTemplate Stepchild::SP404MK2_TEMPLATE(sp404mk2Tracks);
const SequenceTemplate Stepchild::GENERIC_KEYBOARD_TEMPLATE(genericKeyboardTracks);

void Stepchild::init(){
  //setup MIDI ports/IO
  midi.init();

  //starting serial monitor output @ 9600baud for USB communication
  //do u need this for USB midi?
  Serial.begin(9600);

  //Set USB device info
  // these two strings must be exactly 32 characters long:
  //                                   0123456789ABCDEF0123456789ABCDEF
  USBDevice.setManufacturerDescriptor("Alex LaFetra Thompson           ");
  USBDevice.setProductDescriptor     ("ChildOS V0.9.5                  ");

  //start I^2C bus to communicate with MCP23017's
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }
  
  //setup CV pins, frequency
  cv.init();

  //setting up the pinouts and the lower board
  buttons.init();

  //seeding random number generator
  srand(1);
  //load settings
  filesystem.loadSettings();

  //setting up sequence w/ 16 tracks, 768 steps
  initSequence(SP404MK2_TEMPLATE);

  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
}

bool Stepchild::playing(){
  return (playState == PLAYING);
}
bool Stepchild::recording(){
  return (playState == RECORDING);
}
void Stepchild::stop(bool cleanRecording) {
  
  if(cleanRecording)
    cleanupRecording(recheadPos);

  for(uint16_t track = 0; track<trackData.size(); track++){
    if(trackData[track].noteLastSent != 255){
      // midi.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      trackData[track].noteLastSent = 255;
      //reset superposition flags
      for(uint8_t i = 1; i<noteData[track].size(); i++){
        noteData[track][i].setSuperpositioned(false);
      }
    }
  }

  midi.allOff();
  sentNotes.clear();
}

void Stepchild::stop() {
  stop(false);
}

/*
  For toggling into/out of play states. These should only be used by the this->buttons.
*/
void Stepchild::togglePlay(){
  if(playState == PLAYING){
    setNormalMode();
  }
  else{
    setPlayMode();
  }
}

void Stepchild::toggleRecording(bool waitForANoteBeforeStarting){
  //this check is in here so that starting a rec while the liveLooper is active just starts the liveLooper
  if(liveLoop.active){
    if(playState == RECORDING){
      liveLoop.stop();
    }
    else{
      liveLoop.start(false);
    }
  }
  else{
    if(playState == RECORDING){
      setNormalMode();
    }
    else{
      setRecMode(waitForANoteBeforeStarting);
    }
  }
}

void Stepchild::setRecMode(bool waitForANoteBeforeStarting){
  PlayState previousPlayState = playState;
  playState = RECORDING;

  //if it's recording to the loop
  if(recMode == ONESHOT || recMode == CURRENT_LOOP)
    recheadPos = loopData[activeLoop].start;
  //flag which makes the sequence wait to receive a note
  if(waitForANoteBeforeStarting)
    idlingUntilNoteReceived = true;
  //or, just start recording immediately
  else
    idlingUntilNoteReceived = false;
  
  if(previousPlayState == PLAYING)
    stop();
  #ifndef HEADLESS
  midi.disconnectMIDICallbacks();
  //reconnecting the midi callbacks
  midi.setMIDICallbacks(RECORDING);
  #endif
  clock.startTime = micros();
}


void Stepchild::setNormalMode(){
  PlayState previousPlayState = playState;
  playState = STOPPED;//set this ASAP (or at least before you call stop()) so cpu1 doesn't play any notes

  //stop the seq, and cleanup recording if you're coming from recmode
  stop(previousPlayState == RECORDING);
  if(arpeggiator.isActive){
    arpeggiator.stop();
  }
  if(recordingToAutotrack){
    recordingToAutotrack = false;
    this->buttons.counterA = 0;
    this->buttons.counterB = 0;
  }
  midi.sendStop();
  globalModifiers.velocity.value = 0;
  globalModifiers.chance.value = 0;
  globalModifiers.pitch.value = 0;
  cv.off();

  #ifndef HEADLESS
  midi.disconnectMIDICallbacks();
  midi.setMIDICallbacks(STOPPED);
  #endif
}

void Stepchild::setPlayMode(){
  PlayState previousPlayState = playState;
  playState = PLAYING;

  #ifndef HEADLESS
  midi.disconnectMIDICallbacks();
  midi.setMIDICallbacks(PLAYING);
  #endif

  //if it's looping, set the playhead to the activeLoop start
  if(isLooping)
    playheadPos = loopData[activeLoop].start;
  else
    playheadPos = 0;

  clock.startTime = micros();
  if(arpeggiator.isActive){
    arpeggiator.start();
  }
  midi.sendStart();
}

//looks for autotracks to trigger and triggers them
void Stepchild::triggerAutotracks(uint8_t trackID, bool state){
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

void Stepchild::createAutotrack(CurveType type){
  Autotrack newData(type,this->sequenceLength);
  this->autotrackData.push_back(newData);
}

void Stepchild::createAutotrack(CurveType type, uint8_t cont){
  Autotrack newData(type,this->sequenceLength);
  newData.control = cont;
  this->autotrackData.push_back(newData);
}

void Stepchild::createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType){
  Autotrack newData(LINEAR_CURVE,this->sequenceLength);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  this->autotrackData.push_back(newData);
}

void Stepchild::writeNoteOn(uint16_t step, uint8_t pitch, uint8_t vel, uint8_t channel){
  uint8_t trackID = makeTrackWithPitch(pitch,channel);
  if(trackData[trackID].isPrimed()){
    Note newNote(step, step, vel);//this constuctor sets the endPos of the note at the same position
    if(lookupTable[trackID][step] != 0){
      deleteNote(trackID,step);
    }
    noteData[trackID].push_back(newNote);
    lookupTable[trackID][step] = noteData[trackID].size()-1;
    trackData[trackID].noteLastSent = pitch;
  }
}

void Stepchild::checkCV(){
  //if the CV functionality is turned off, just return immediately
  if(!cv.on){
    return;
  }
  //if a track is sending a pitch
  if(sentNotes.notes.size()){
    cv.checkPitch(sentNotes.notes[sentNotes.notes.size()-1].pitch);
  }
  //this is soooo inefficient, u should just check each jack to see if it's a track jack first
  for(uint8_t i = 0; i < trackData.size(); i++){
    if(trackData[i].noteLastSent != 255){
      cv.checkGate(true,i);
    }
    else{
      cv.checkGate(true,i);
    }
  }
  cv.checkClock();
}


void Stepchild::writeNoteOff(uint16_t step, uint8_t pitch, uint8_t channel){
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

void Stepchild::writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value){
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

void Stepchild::continueStep(uint16_t step){
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
      else if(overwriteNotesWithEmptiness){
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

void Stepchild::updateLookupData(){
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

void Stepchild::cleanupRecording(uint16_t stopTime){
  for(uint8_t i = 0; i<trackData.size(); i++){
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

void Stepchild::recordingLoop(){
  if(clock.clockSource == INTERNAL_CLOCK){
    if(clock.hasItBeenEnoughTime(recheadPos)){
      clock.timeLastStepPlayed = micros();
      checkAutotracks();
      //if it's not in wait mode, or if it is but a note has been received
      if(!waitForNoteBeforeRec || !idlingUntilNoteReceived){
        continueStep(recheadPos);
        midi.sendClock();
        recheadPos++;
        checkLoop();
        playStep(recheadPos);
      }
    }
  }
  else if(clock.clockSource == EXTERNAL_CLOCK){
    if(clock.receivedClockMessage && startedPlaying){
      clock.receivedClockMessage = false;
      continueStep(recheadPos);
      recheadPos++;
      checkLoop();
      checkAutotracks();
      playStep(recheadPos);
    }
  }
}


void Stepchild::checkLoop(){
  //if it's not looping, ignore loop bounds BUT check the sequence length and wrap
  if(!isLooping){
    if((playState == PLAYING && playheadPos >= lookupTable[0].size()) || (playState == RECORDING && recheadPos >= lookupTable[0].size())){
      setNormalMode();
    }
    return;
  }
  //playing
  if(playState == PLAYING){
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
    }
  }
  //recording
  else if(playState == RECORDING){
    //one-shot record to current loop, without looping
    if(recMode == ONESHOT){
      if(recheadPos>=loopData[activeLoop].end){
        setNormalMode();
      }
    }
    //record to one loop over and over again
    else if(recMode == CURRENT_LOOP){
      if(recheadPos>=loopData[activeLoop].end){
        recheadPos = loopData[activeLoop].start;
        cutLoop();
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


void Stepchild::playingLoop(){
  //internal timing
  if(clock.clockSource == INTERNAL_CLOCK){
    if(clock.hasItBeenEnoughTime(playheadPos)){
      midi.sendClock();
      playStep(playheadPos);
      playheadPos++;
      checkLoop();
      if(!playheadPos%24)
        cv.checkClock();
    }
  }
  //external timing
  else if(clock.clockSource == EXTERNAL_CLOCK){
    midi.read();
    if(clock.receivedClockMessage && startedPlaying){
      clock.receivedClockMessage = false;
      playStep(playheadPos);
      playheadPos += 1;
      checkLoop();
      // checkFragment();
    }
  }
}

void Stepchild::checkAutotracks(){
  if(recordingToAutotrack){
    int newVal = 64;
    switch(autotrackData[activeAutotrack].recordFrom){
      //recording externally, so get outta this loop!
      case EXTERNAL_MIDI:
        return;
      //rec from encoder A
      case ENCODER_A:
        if(this->buttons.counterA>127){
          this->buttons.counterA = 127;
        }
        if(this->buttons.counterA<0)
          this->buttons.counterA = 0;
        newVal = this->buttons.counterA;
        break;
      //rec from encoder B
      case ENCODER_B:
        if(this->buttons.counterB>127)
          this->buttons.counterB = 127;
        if(this->buttons.counterB<0)
          this->buttons.counterB = 0;
        newVal = this->buttons.counterB;
        break;
      //rec from joystick X
      case JOY_X:
        newVal = this->buttons.getJoyX();
        if(newVal < 58 || newVal>68)
          idlingUntilNoteReceived = false;
        break;
      //rec from joystick Y
      case JOY_Y:
        newVal = this->buttons.getJoyY();
        if(newVal < 58 || newVal>68)
          idlingUntilNoteReceived = false;
        break;
    }
    //bounds checking the new value before we write it to the DT
    if(newVal>127)
      newVal = 127;
    else if(newVal<0)
      newVal = 0;
    if(idlingUntilNoteReceived){
      return;
    }
    recentCC.val = newVal;
    recentCC.cc = autotrackData[activeAutotrack].control;
    recentCC.channel = autotrackData[activeAutotrack].channel;
    autotrackData[activeAutotrack].data[recheadPos] = newVal;
  }
}

void Stepchild::arpLoop(){
  //if it was active, but hadn't started playing yet
  if(!arpeggiator.playing){
    switch(arpeggiator.source){
      case NOTES_FROM_MIDI_INPUT:
        if(receivedNotes.notes.size()>0)
          arpeggiator.start();
        break;
      case NOTES_FROM_SEQUENCE:
        if(sentNotes.notes.size()>0)
          arpeggiator.start();
        break;
      case NOTES_FROM_SEQUENCE_AND_MIDI_INPUT:
        if(sentNotes.notes.size()>0 || receivedNotes.notes.size()>0)
          arpeggiator.start();
        break;
    }
  }
  if(arpeggiator.playing){
    //if the arp isn't latched and there are no notes for it
    if(!arpeggiator.holding  && ((arpeggiator.source == NOTES_FROM_MIDI_INPUT && !receivedNotes.notes.size()) || (arpeggiator.source == NOTES_FROM_SEQUENCE && !sentNotes.notes.size()))){
      arpeggiator.stop();
    }
    //if it IS latched or there are notes for it, then continue
    else if(arpeggiator.hasItBeenEnoughTime()){
      arpeggiator.playstep();
    }
  }
}


void Stepchild::defaultLoop(){
  playheadPos = loopData[activeLoop].start;
  recheadPos = loopData[activeLoop].start;
}

void Stepchild::playStep(uint16_t timestep) {
  playPCData(timestep);
  //playing each track
  for (uint8_t track = 0; track < trackData.size(); track++) {
    //if it's unmuted or solo'd, or if you're in rec mode but the track isn't primed, play it
    if((playState == PLAYING && (!trackData[track].isMuted() || trackData[track].isSolo())) || (playState == RECORDING && !trackData[track].isPrimed()))
      playTrack(track,timestep);
  }
  //playing autotracks too
  for(uint8_t dT = 0; dT < autotrackData.size(); dT++){
    autotrackData[dT].play(timestep);
  }
  checkCV();
}


void Stepchild::playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (lookupTable[track][timestep] == 0){
    if(trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      if(!arpeggiator.isActive || arpeggiator.source == NOTES_FROM_MIDI_INPUT)//if the arp is off, or if it's just listening to notes from outside the seq
        midi.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
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

void Stepchild::playNote(Note& note, uint8_t track, uint16_t timestep){
  //if it's the start of the note, or if the track wasn't sending already
  if(timestep == note.startPos || trackData[track].noteLastSent == 255){ //if it's the start
    //if it's not muted
    if(!note.isMuted()){
      //if the track was already sending a note, send note off
      if(trackData[track].noteLastSent != 255){
        if(!arpeggiator.isActive || arpeggiator.source == NOTES_FROM_MIDI_INPUT)
          midi.noteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
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
        if(!arpeggiator.isActive || arpeggiator.source == NOTES_FROM_MIDI_INPUT)
          midi.noteOn(pitch, vel, trackData[track].channel);
        trackData[track].noteLastSent = pitch;
        if(trackData[track].isLatched()){
          if(!arpeggiator.isActive || arpeggiator.source == NOTES_FROM_MIDI_INPUT)
            midi.noteOff(pitch, 0, trackData[track].channel);
        }
        sentNotes.addNote(pitch,vel,trackData[track].channel);
        note.setPlaying(true);
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
    //reset playing flag
    note.setPlaying(false);
  }
}

void Stepchild::playPCData(Timestep timestep){
    for(uint8_t port = 0; port<5; port++){
        for(uint16_t event = 0; event<PCData[port].size(); event++){
            if(PCData[port][event].timestep == timestep){
                midi.sendPC(port,PCData[port][event].val,PCData[port][event].channel);
            }
        }
    }
}


/*

  Code for the StepchildSequence class

*/

//creates a sequence object with default values
void Stepchild::initSequence(uint8_t numberOfTracks,uint16_t length){
  //What should happen if length<192?
  this->sequenceLength = length;
  this->viewStart = 0;
  this->viewEnd = 192;

  //make the default loop
  this->loopData = {Loop(0,96,0,0)};

  //reset to 120 bpm
  clock.setBPM(120);

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
  setNormalMode();
}

void Stepchild::initSequence(){
  this->initSequence(16,768);
}

//inits a sequence from a template
void Stepchild::initSequence(SequenceTemplate t){
  this->eraseSequence();
  this->sequenceLength = 768;
  this->viewStart = 0;
  this->viewEnd = 192;
  this->loopData = {Loop(0,96,0,0)};

  clock.setBPM(120);

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
  setNormalMode();
}

bool Stepchild::isQuarterGrid(){
  return !(this->subDivision%3);
}

//swaps all the data vars in the sequence for new, blank data
void Stepchild::eraseSequence(){
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
Note Stepchild::noteAt(uint8_t track, uint16_t step){
  if(this->lookupTable[track][step]<this->noteData[track].size())
      return this->noteData[track][this->lookupTable[track][step]];
  else return this->noteData[track][0];
}

Note Stepchild::noteAtCursor(){
  return this->noteAt(this->activeTrack,this->cursorPos);
}
uint16_t Stepchild::IDAt(uint8_t track, uint16_t step){
  if(step<this->sequenceLength)
      return this->lookupTable[track][step];
  else return 0;
}
uint16_t Stepchild::IDAtCursor(){
  return this->IDAt(this->activeTrack,this->cursorPos);
}

/*
----------------------------------------------------------
                  LOADING NOTES
----------------------------------------------------------
*/
//adds a note w/o checking for overlaps
//Only use this when loading notes from a file into a blank sequence
void Stepchild::loadNote(Note newNote, uint8_t track){
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

//decrements each note ID if it's larger than the last, so that if these notes are 
//deleted in this order their IDs stay referenced
vector<NoteID> Stepchild::crunchNoteIDsForDeletion(vector<NoteID> targetNoteIDs){
  for(uint8_t i = 0; i<targetNoteIDs.size(); i++){
    //start w/ the next note
    for(uint8_t j = i+1; j<targetNoteIDs.size(); j++){
      if(targetNoteIDs[i].id<targetNoteIDs[j].id)
        targetNoteIDs[j].id--;
    }
  }
  return targetNoteIDs;
}

void Stepchild::deleteNotes_byID(vector<NoteID> targetNoteIDs){
  //make sure IDs stay referenced
  targetNoteIDs = crunchNoteIDsForDeletion(targetNoteIDs);
  //delete notes from sequence
  for(NoteID n:targetNoteIDs){
    deleteNote_byID(n.track,n.id);
  }
}
//Deletes a note on a given track with a given ID
void Stepchild::deleteNote_byID(uint8_t track, uint16_t targetNoteID){
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
void Stepchild::deleteNote(uint8_t track, uint16_t time){
  this->deleteNote_byID(track,this->IDAt(track,time));
}
//deletes a note at the current track/cursor position
void Stepchild::deleteNote(){
  this->deleteNote_byID(this->activeTrack,this->IDAtCursor());
}
void Stepchild::deleteSelected(){
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
void Stepchild::makeNote(Note newNoteOn, uint8_t track, bool loudly){
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
          midi.noteOn(this->trackData[track].pitch, newNoteOn.velocity, this->trackData[track].channel);
          midi.noteOff(this->trackData[track].pitch, 0, this->trackData[track].channel);
      }
  }
}
void Stepchild::makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
  this->makeNote(newNoteOn,track,loudly);
}
void Stepchild::makeNote(Note newNoteOn, uint8_t track){
  this->makeNote(newNoteOn,track,false);
}
void Stepchild::makeNote(uint8_t track, uint16_t time, uint16_t length, uint8_t velocity, uint8_t chance, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, false, false);
  this->makeNote(newNoteOn,track,false);
}
//this one is for quickly placing a ntoe at the cursor, on the active track
void Stepchild::makeNote(uint8_t track, uint16_t time, uint16_t length, bool loudly) {
  Note newNote(time,(time+length),this->defaultVel,100,false,false);
  this->makeNote(newNote,track,loudly);
}
void Stepchild::makeNote(uint16_t length, bool loudly) {
  Note newNote(this->cursorPos,this->cursorPos+length,this->defaultVel,100,false,false);
  this->makeNote(newNote,this->activeTrack,false);
}
//draws notes every "count" subDivs, from viewStart to viewEnd
//this is a super useful idea for sequencing, but currently only used by the edit menu
void Stepchild::stencilNotes(uint8_t count){
  for(uint16_t i = this->viewStart; i<this->viewEnd; i+=(this->subDivision*count)){
      //if there's no note there or if it's not the beginning of a note
      if(this->lookupTable[this->activeTrack][i] == 0 || (this->cursorPos != this->noteAtCursor().startPos))
          this->makeNote(this->activeTrack, i, this->subDivision*count+1, this->defaultVel, 100, false);
  }
}

//checks if there's a note first, and if there is it deletes it/if not it places one
void Stepchild::toggleNote(uint8_t track, uint16_t step, uint16_t length){
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
void Stepchild::makeNoteEveryNDivisions(uint8_t n){
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
void Stepchild::editNoteProperty_byID(uint16_t id, uint8_t track, int8_t amount, NoteProperty which){
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
        break;
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
void Stepchild::editNotePropertyOfSelectedNotes(int8_t amount, NoteProperty which){
  for(uint8_t track = 0; track<this->trackData.size(); track++){
      for(uint16_t note = this->noteData[track].size()-1; note>0; note--){
          if(this->noteData[track][note].isSelected()){
              this->editNoteProperty_byID(note,track, amount, which);
          }
      }
  }
}

//edits a note, and all selected notes, checking to make sure it doesn't double-edit
void Stepchild::editNoteAndSelected(int8_t amount, NoteProperty which){
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
void Stepchild::changeVel(int8_t amount){
  this->editNoteAndSelected(amount,VELOCITY);
}
void Stepchild::changeChance(int8_t amount){
  this->editNoteAndSelected(amount,CHANCE);
}

//changes JUST a specific note
void Stepchild::changeChance_byID(uint16_t id, uint8_t track, int8_t amount){
  this->editNoteProperty_byID(id, track, amount, CHANCE);
}
void Stepchild::changeVel_byID(uint16_t id, uint8_t track, int8_t amount){
  this->editNoteProperty_byID(id, track, amount, VELOCITY);
}

void Stepchild::muteNote(uint8_t track, uint16_t id, bool toggle){
  if(id != 0){
      if(toggle)
          this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
      else
          this->noteData[track][id].setMuted(true);
  }
}
void Stepchild::unmuteNote(uint8_t track, uint16_t id, bool toggle){
  if(id != 0){
      if(toggle)
          this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
      else
          this->noteData[track][id].setMuted(false);
  }
}

//mutes/unmutes all selected notes
void Stepchild::setMuteStateOfSelectedNotes(bool state){
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
void Stepchild::muteSelectedNotes(){
  this->setMuteStateOfSelectedNotes(true);
}
void Stepchild::unmuteSelectedNotes(){
  this->setMuteStateOfSelectedNotes(false);
}

//cuts a note short at a specific time
void Stepchild::truncateNote(uint8_t track, uint16_t atTime){
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
bool Stepchild::checkNoteMove(Note& targetNote, uint16_t track, uint16_t newTrack, uint16_t newStart){
  
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
bool Stepchild::checkNoteMove(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
  Note targetNote = this->noteData[track][id];
  return this->checkNoteMove(targetNote,track,newTrack,newStart);
}
//moves a note
bool Stepchild::moveNote(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
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
bool Stepchild::moveSelectedNotes(int16_t xOffset, int8_t yOffset){
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
bool Stepchild::moveNotes(int16_t xAmount, int8_t yAmount){
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
void Stepchild::addTimeToSeq(uint16_t amount, uint16_t insertPoint){
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
void Stepchild::removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
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
void Stepchild::displayMainSequenceLEDs(){
  if(this->buttons.SHIFT()){
      uint16_t ledState = 0b0000000000001111;
      if(millis()/200%2)
          ledState |= 0b1000000000000000;
      if((millis()/200+1)%2)
          ledState |= 0b0100000000000000;
      
      this->buttons.writeLEDs(ledState);
      return;
  }
  uint16_t dat = 0;//00000000
  if(this->buttons.LEDsActive && !screenSaverActive){
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
  this->buttons.writeLEDs(dat);
}
/*
----------------------------------------------------------
                      Loops
----------------------------------------------------------
*/
void Stepchild::setLoopPoint(int32_t start, bool which){
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
      this->tooltipText = "loop start: "+stepsToPosition(this->loopData[this->activeLoop].start,true);
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
      this->tooltipText = "loop end: "+stepsToPosition(this->loopData[this->activeLoop].end,true);
  }
}

void Stepchild::addLoop(Loop newLoop){
  this->loopData.push_back(newLoop);
}
void Stepchild::insertLoop(Loop newLoop, uint8_t index){
  this->loopData.insert(this->loopData.begin()+index,newLoop);
}


void Stepchild::setActiveLoop(unsigned int id){
  if(id<this->loopData.size() && id >=0){
      this->activeLoop = id;
      this->loopCount = 0;
  }
}

void Stepchild::addLoop(){
  Loop newLoop;
  newLoop.start = this->loopData[this->activeLoop].start;
  newLoop.end = this->loopData[this->activeLoop].end;
  newLoop.reps = this->loopData[this->activeLoop].reps;
  newLoop.type = this->loopData[this->activeLoop].type;
  this->loopData.push_back(newLoop);
  setActiveLoop(this->loopData.size()-1);
}

void Stepchild::addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, uint8_t type){
  Loop newLoop;
  newLoop.start = start;
  newLoop.end = end;
  newLoop.reps = iter;
  newLoop.type = static_cast<LoopType>(type);
  this->loopData.push_back(newLoop);
}

void Stepchild::deleteLoop(uint8_t id){
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

void Stepchild::toggleLoop(){
  this->isLooping = !this->isLooping;
}

//moves to the next loop in loopSeq
void Stepchild::nextLoop(){
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
          case RANDOM_SAME_LENGTH:{
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
void Stepchild::cutLoop(){
  for(int i = 0; i<this->trackData.size(); i++){
      if(this->trackData[i].noteLastSent != 255){
          this->noteData[i][this->noteData[i].size()-1].endPos = this->loopData[this->activeLoop].end;
          //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
          if(recMode != ONESHOT)
              writeNoteOn(this->loopData[this->activeLoop].start,this->trackData[i].pitch,this->noteData[i][this->noteData[i].size()-1].velocity,this->trackData[i].channel);
      }
  }
}

//creates a new loop immediately to the right of the current loop and copies the loop over into it (useful for on the fly beatmaking)
//then sets the OG loop to that one
bool Stepchild::pushToNewLoop(){
  Loop newLoop = loopData[activeLoop];
  uint16_t length = newLoop.length();
  //if you're out of sequence length
  if(newLoop.end+length >= sequenceLength){
    alert("extend seq to add more loops!",500);
    return false;
  }
  //else, make a new loop to the right
  else{
    newLoop.start+=length;
    newLoop.end+=length;
    addLoop(newLoop);//add the loop to the chain

    //duplicate the loop contents and paste them next to the current loop
    clipboard.copyLoop(activeLoop);
    clipboard.pasteAt(0,newLoop.start);

    //move the view to the start of the current loop
    setCursor(newLoop.start);
    setViewStart(newLoop.start);
    return true;
  }
}

//true if Stepchild is sending or receiving notes
bool Stepchild::isReceiving(){
  if(receivedNotes.notes.size())
    return true;
  for(uint8_t i = 0; i<this->trackData.size(); i++){
    if(this->trackData[i].noteLastSent != 255)
      return true;
  }
  return false;
}

bool Stepchild::isSending(){
  if(sentNotes.notes.size())
    return true;
  for(uint8_t i = 0; i<this->trackData.size(); i++){
    if(this->trackData[i].noteLastSent != 255)
      return true;
  }
  return false;
}

bool Stepchild::isReceivingOrSending(){
return (isReceiving() || isSending());
}

//returns a vector containing all the pitches being both sent & received, with no duplicates
vector<uint8_t> Stepchild::getAllActivePitches(){
  vector<uint8_t> A = receivedNotes.getUniquePitches();
  vector<uint8_t> B = sentNotes.getUniquePitches();
  vector<uint8_t> C;
  for(uint8_t i = 0; i<A.size(); i++){
    if(find(C.begin(),C.end(),A[i]) == C.end()){
      C.push_back(A[i]);
    }
  }
  for(uint8_t i = 0; i<B.size(); i++){
    if(find(C.begin(),C.end(),B[i]) == C.end()){
      C.push_back(B[i]);
    }
  }
  return C;
}

  vector<uint8_t> Stepchild::getTracksWithNotes(){
    vector<uint8_t> list;
    for(uint8_t track = 0; track<trackData.size(); track++){
        if(noteData[track].size()>1)
        list.push_back(track);
    }
    return list;
  }

  vector<uint8_t> Stepchild::getTracksWithNotesInLoop(uint8_t loop){
    vector<uint8_t> list1 = getTracksWithNotes();
    vector<uint8_t> list2;
    for(uint8_t track = 0; track<list1.size(); track++){
        for(uint16_t i = loopData[loop].start; i<loopData[loop].end; i++){
        if(lookupTable[list1[track]][i] != 0){
            list2.push_back(list1[track]);
            break;
        }
        }
    }
    return list2;
  }



//moves the whole loop
void Stepchild::moveLoop(int16_t amount){
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
void Stepchild::toggleLoopMove(){
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

int16_t Stepchild::changeNoteLength(int val, unsigned short int track, unsigned short int id){
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


void Stepchild::changeNoteLengthSelected(int amount){
for(int track = 0; track<this->trackData.size(); track++){
  for(int note = 1; note <= this->noteData[track].size()-1; note++){
    if(this->noteData[track][note].isSelected()){
      changeNoteLength(amount, track, note);
    }
  }
}
}

int16_t Stepchild::changeNoteLength(int amount){
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
void Stepchild::changeNoteLength_jumpToEnds(int16_t amount){
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
float Stepchild::getDistanceFromNoteToCursor(Note note,uint8_t track){
//if the start of the note is closer than the end
return sqrt(pow(this->activeTrack - track,2)+pow(((abs(note.startPos-this->cursorPos)<abs(note.endPos-this->cursorPos))?(note.startPos-this->cursorPos):(note.endPos-this->cursorPos)),2));
}

void Stepchild::setCursorToNearestNote(){
  const float maxPossibleDist = this->sequenceLength*this->viewScale+this->trackData.size()*trackHeight;
  float minDist = maxPossibleDist;
  uint16_t minTrack = 0;
  uint16_t minNote = 0;
  for(uint8_t track = 0; track<this->noteData.size(); track++){
    for(uint16_t note = 1; note<this->noteData[track].size(); note++){
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

  if(minDist != maxPossibleDist){
    setCursor((this->noteData[minTrack][minNote].startPos<this->cursorPos)?this->noteData[minTrack][minNote].startPos:this->noteData[minTrack][minNote].endPos-1);
    setActiveTrack(minTrack,false);
  }
}


uint16_t Stepchild::getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<this->noteData.size(); track++){
    count+=this->noteData[track].size()-1;
  }
  return count;
}

//counts notes within a range
uint16_t Stepchild::countNotesInRange(uint16_t start, uint16_t end){
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
int16_t Stepchild::getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex){
  for(int i=aboveIndex ; i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return 0;
}
//returns id of track with a specific pitch, returns -1 if track doesn't exist
int16_t Stepchild::getTrackWithPitch(int16_t pitch, uint8_t channel){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch && trackData[i].channel == channel)
      return i;
  }
  return -1;
}
int16_t Stepchild::getTrackWithPitch(int16_t pitch){
  for(uint16_t i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}

int16_t Stepchild::makeTrackWithPitch(int16_t pitch, uint8_t channel){
  int16_t track = getTrackWithPitch(pitch,channel);
  if(track == -1){
    return addTrack_return(pitch, channel, false);
  }
  return track;
}



void Stepchild::addTrack(Track newTrack, bool loudly){
  if(trackData.size()<255){
    insertTrack(newTrack,activeTrack);
    if(loudly){
      midi.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      midi.noteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}

//this function should add a new row to the sequence.noteData, and sequence.lookupTable initialized with 64 zeroes
void Stepchild::addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly){
  if(trackData.size()<255){
    Track newTrack(pitch, channel);
    newTrack.setLatched(latch);
    newTrack.muteGroup = muteGroup;
    newTrack.setPrimed(primed);
    addTrack(newTrack,loudly);
  }
}
void Stepchild::addTrack(unsigned char pitch, unsigned char channel, bool loudly) {
  addTrack(pitch, channel, false, 0, true, loudly);
}

//changes which track is active, changing only to valid tracks
bool Stepchild::setActiveTrack(uint8_t newActiveTrack, bool loudly) {
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
    midi.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
    midi.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
    if(this->trackData[this->activeTrack].isLatched()){
      midi.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
      midi.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
    }
  }
  // this->tooltipText = pitchToString(this->trackData[this->activeTrack].pitch,true,true);
  // this->tooltipText = "tk"+stringify(activeTrack)+" ch"+stringify(trackData[activeTrack].channel)+" $"+pitchToString(trackData[activeTrack].pitch,true,true);
  this->tooltipText = "tk"+stringify(activeTrack);
  return true;
}
return false;
}

void Stepchild::changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    this->trackData[id].channel = newChannel;
  }
}
//unarms any tracks with notes on them
void Stepchild::disarmTracksWithNotes(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    if(noteData[i].size()>1){
      trackData[i].setPrimed(false);
    }
  }
}

void Stepchild::muteTrack(uint16_t id){
  trackData[id].setMuted(true);
}
void Stepchild::unMuteTrack(uint16_t id){
  trackData[id].setMuted(false);
}

void Stepchild::toggleMute(uint16_t id){
  trackData[id].setMuted(!trackData[id].isMuted());
}

void Stepchild::muteMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    muteTrack(ids[track]);
  }
}

void Stepchild::soloTrack(unsigned short int id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id){
      muteTrack(track);
      trackData[track].setSolo(false);
    }
    else 
      trackData[id].setSolo(true);
  }
}

void Stepchild::unSoloTrack(uint16_t id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id)
      unMuteTrack(track);
    else
      trackData[id].setSolo(false);
  }
}

void Stepchild::toggleSolo(uint16_t id){
  if(trackData[id].isSolo())
    unSoloTrack(id);
  else
    soloTrack(id);
}

void Stepchild::eraseMultipleTracks(vector<uint8_t> ids){
  for(uint16_t track = 0; track<ids.size(); track++){
    eraseTrack(ids[track]);
  }
}

void Stepchild::unprimeTracksWithNotes(){
  for(uint16_t track = 0; track<trackData.size(); track++){
    if(noteData[track].size() > 1){
      trackData[track].setPrimed(false);
    }
    else{
      trackData[track].setPrimed(true);
    }
  }
}
void Stepchild::temporarilyUnprimeTracksWithNotes(){
  for(uint16_t track = 0; track<trackData.size(); track++){
    trackData[track].storePrimeState();
    if(noteData[track].size() > 1){
      trackData[track].setPrimed(false);
    }
  }
}
void Stepchild::reprimeTracks(){
  for(uint16_t track = 0; track<trackData.size(); track++){
    trackData[track].reprime();
  }
}


//returns the index of the new track
int16_t Stepchild::addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly) {
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    addTrack(newTrack,loudly);
    return (activeTrack);
  }
  else{
    return -1;
  }
}

void Stepchild::insertTrack(Track newTrack, uint8_t index){
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

int16_t Stepchild::insertTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly, uint8_t loc){
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    insertTrack(newTrack,loc);
    if(loudly){
      midi.noteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      midi.noteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
    return (loc);
  }
  else{
    return -1;
  }
}

void Stepchild::dupeTrack(unsigned short int track){
  if(trackData.size()<256){
    Track newTrack = trackData[track];
    insertTrack(newTrack,track);
  }
}

void Stepchild::eraseTrack(int track) {
  noteData[track].resize(1);
  for (int i = 0; i < sequenceLength; i++) {
    clearSelection(track, i);
    lookupTable[track][i] = 0;
  }
}
//erases notes, but doesn't del track
void Stepchild::eraseTrack() {
  eraseTrack(activeTrack);
}


void Stepchild::deleteDuplicateEmptyTracks(){
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

uint8_t Stepchild::countEmptyTracks(){
  uint8_t count = 0;
  for(auto track:noteData){
    if(track.size()==1){
      count++;
    }
  }
  return count;
}
void Stepchild::deleteEmptyTracks(){
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
void Stepchild::deleteTrack(unsigned short int track){
  deleteTrack(track,false);
}

void Stepchild::deleteTrack(unsigned short int track, bool hard, bool askFirst){
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

void Stepchild::deleteTrack(unsigned short int track, bool hard){
  deleteTrack(track, hard, true);
}
//dels all tracks
void Stepchild::deleteAllTracks(){
  while(trackData.size()>0){
    deleteTrack(0,true,false);
  }
}


void Stepchild::transposeAllChannels(int8_t increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackChannel(i,trackData[i].channel+increment,true);//only the active track makes a noise
    else
      setTrackChannel(i,trackData[i].channel+increment,false);//quiet bc it'd be crazy
  }
}

void Stepchild::setTrackChannel(uint16_t track, uint8_t channel, bool loud){
  if(channel>=1 && channel<=16){
    midi.noteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].channel = channel;
    if(loud){
      midi.noteOn(trackData[track].pitch,63,trackData[track].channel);
      midi.noteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

void Stepchild::transposeAllPitches(int16_t increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackPitch(i,trackData[i].pitch+increment,true);//only the active track makes a noise
    else
      setTrackPitch(i,trackData[i].pitch+increment,false);//quiet bc it'd be crazy
  }
}


//sorting functions passed as params to the sortTracksBy() function
bool sortTracksByPitch(Track t1, Track t2){
  return t1.pitch>t2.pitch;
}
bool sortTracksByChannel(Track t1, Track t2){
  return t1.channel>t2.channel;
}

void Stepchild::sortTrackData(uint8_t type,uint8_t target){
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


void Stepchild::setTrackToNearestPitch(vector<uint8_t>pitches,uint8_t track,bool allowDuplicates){
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

void Stepchild::setTrackPitch(int track, int note, bool loud) {
  if(note>=0 && note<=127){
    midi.noteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].pitch = note;
    if(loud){
      midi.noteOn(trackData[track].pitch,63,trackData[track].channel);
      midi.noteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

//this one won't double up on a pitch, and will instead choose the next closes pitch in the list
void Stepchild::setTrackToNearestUniquePitch(vector<uint8_t>pitches,int track){
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


void Stepchild::swapTracks(unsigned short int track1, unsigned short int track2){
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

void Stepchild::changeAllTrackChannels(int newChannel){
for(int track = 0; track<this->trackData.size(); track++){
  changeTrackChannel(track, newChannel);
}
}

void Stepchild::moveToNextNote_inTrack(bool up){
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
void Stepchild::moveToNextNote(bool forward,bool endSnap){
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

void Stepchild::moveToNextNote(bool forward){
moveToNextNote(forward, false);
}

//View ------------------------------------------------------------------

void Stepchild::setViewStart(uint16_t step){
uint16_t viewLength = this->viewEnd-this->viewStart;
if(viewLength + step > this->sequenceLength){
  step = this->sequenceLength-viewLength;
}
this->viewStart = step;
this->viewEnd = step+viewLength;
}

void Stepchild::moveView(int16_t val){
if(val < 0 && abs(val)>this->viewStart){
  setViewStart(0);
}
else{
  setViewStart(this->viewStart+val);
}
}

//moving the cursor around
int16_t Stepchild::moveCursor(int moveAmount){
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
  this->tooltipText = ((moveAmount>0)?(stepsToPosition(this->cursorPos,true)+">>"):("<<"+stepsToPosition(this->cursorPos,true)));
  return amt;
}

void Stepchild::setCursor(uint16_t loc){
  moveCursor(loc-this->cursorPos);
}

void Stepchild::moveCursorIntoView(){
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
bool Stepchild::isInView(int target){
if(target>=this->viewStart && target<=this->viewEnd)
  return true;
else
  return false;
}


void Stepchild::changeSubDivInt(bool down){
changeSubDivInt(down,false);
}

void Stepchild::changeSubDivInt(bool down, bool limitToView){
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
    this->tooltipText = "~"+stepsToMeasures(this->subDivision);
}

void Stepchild::toggleTriplets(){
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
    this->tooltipText = stepsToMeasures(this->subDivision);
}

//makes sure scale/viewend line up with the display
void Stepchild::checkView(){
if(this->viewEnd>this->sequenceLength){
  this->viewScale = float(96)/float(this->sequenceLength);
  this->viewEnd = this->sequenceLength+1;
}
}
//zooms in/out
void Stepchild::zoom(bool in){
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
this->tooltipText = "~"+stepsToMeasures(this->subDivision);
}
bool Stepchild::areThereAnyNotes(){
for(uint8_t t = 0; t<this->noteData.size(); t++){
  if(this->noteData[t].size()>1){
    return true;
  }
}
return false;
}
//checks for notes above or below a track
bool Stepchild::areThereMoreNotes(bool above){
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

//returns a 2D vector containing a row for each track and a copy of each note that's currently selected on each track
vector<vector<Note>> Stepchild::grabSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<noteData[track].size(); note++){
            if(noteData[track][note].isSelected()){
                selectedNotesOnTrack.push_back(noteData[track][note]);
            }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}
vector<vector<Note>> Stepchild::grabAndDeleteSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<noteData[track].size(); note++){
          if(noteData[track][note].isSelected()){
            selectedNotesOnTrack.push_back(noteData[track][note]);
            deleteNote_byID(track,note);
          }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}

vector<uint16_t> Stepchild::getSelectedNotesBoundingBox(){
  //stored as xStart,yStart,xEnd,yEnd
  //(initially store it as dramatic as possible)
  vector<uint16_t> bounds = {sequenceLength,(uint16_t)trackData.size(),0,0};
  uint16_t checkedNotes = 0;
  for(uint8_t track = 0; track<noteData.size(); track++){
    for(uint16_t note = 1; note<noteData[track].size(); note++){
      if(noteData[track][note].isSelected()){
        //if it's the new highest track
        if(track<bounds[1])
          bounds[1] = track;
        //if it's the new lowest track
        if(track>bounds[3])
          bounds[3] = track;
        //if it's the new earliest note
        if(noteData[track][note].startPos<bounds[0])
          bounds[0] = noteData[track][note].startPos;
        //if it's the new latest note
        if(noteData[track][note].endPos>bounds[2])
          bounds[2] = noteData[track][note].endPos;

        //if you've checked all the selected notes, return
        checkedNotes++;
        if(checkedNotes == selectionCount)
          return bounds;
      }
    }
  }
  return bounds;
}

vector<uint8_t> Stepchild::getTracksWithSelectedNotes(){
  vector<uint8_t> list;
  if(selectionCount>0){
    for(uint8_t track = 0; track<trackData.size(); track++){
      for(uint16_t note = 1; note<noteData[track].size(); note++){
        //once you find a selected note, jump to the next track
        if(noteData[track][note].isSelected()){
          list.push_back(track);
          track++;
          note = 1;
        }
      }
    }
  }
  return list;
}

void Stepchild::clearSelection(int track, int time) {
  if(lookupTable[track][time] != 0 && noteData[track][lookupTable[track][time]].isSelected()){
    selectionCount--;
    noteData[track][lookupTable[track][time]].setSelected(false);
  }
}

void Stepchild::clearSelection(){
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      if(selectionCount<=0)
          return;
      for(int note = noteData[track].size()-1; note>0; note--){
        if(selectionCount<=0)
          return;
        if(noteData[track][note].isSelected()){
          noteData[track][note].setSelected(false);
          selectionCount--;
        }
      }
    }
  }
}

void Stepchild::deselectNote(uint8_t track, uint16_t id){
  if(noteData[track][id].isSelected()){
    selectionCount--;
    noteData[track][id].setSelected(false);
  }
}

void Stepchild::selectNotesInTrack(uint8_t track){
  for(uint16_t note = 1; note<noteData.size(); note++){
    if(!noteData[track][note].isSelected()){
      noteData[track][note].setSelected(true);
      selectionCount++;
    }
  }
}

//select a note
void Stepchild::selectNote(uint8_t track, uint16_t id){
  //if it's already selected
  if(noteData[track][id].isSelected())
    return;
  noteData[track][id].setSelected(true);
  selectionCount++;
}

//togglet a note's selection state  by it's track and ID
void Stepchild::toggleSelectNote(uint8_t track, uint16_t id, bool additive){
    //if id == 0, just return
    if(!id){
      return;
    }
    if(!additive&&!noteData[track][id].isSelected()){
      clearSelection();
      selectNote(track,id);
    }
    else{
      if(noteData[track][id].isSelected() && selectionCount>0)
        deselectNote(track,id);
      else if(!noteData[track][id].isSelected()){
        selectNote(track,id);
      }
    }
}


void Stepchild::selectAllNotesInTrack(){
  for(uint16_t i = 1; i<noteData[activeTrack].size();i++){
    selectNote(activeTrack, i);
  }
}

//selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
void Stepchild::selectAll() {
  for(uint8_t track = 0; track<trackData.size(); track++){
    for(uint16_t id = 1; id<noteData[track].size(); id++){
      selectNote(track,id);
    }
  }
}


//returns the pitch in english AND in either sharp, or flat form
String Stepchild::pitchToString(uint8_t input, bool oct, bool sharps){
  String pitch;
  int octave = (input/12)-2;//idk why this offset is needed
  input = input%12;
  if(!sharps){
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "Db";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "Eb";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "Gb";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "Ab";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "Bb";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  else{
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "C#";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "D#";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "F#";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "G#";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "A#";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  if(oct){
    pitch+=stringify(octave);
  }
  return pitch;
}

String Stepchild::getScaleName(ScaleName scale){
  const String scales[12] = {"major","dorian","phrygian","lydian","mixolydian","aeolian","locrian","melodic minor","harmonic minor","major pentatonic","minor pentatonoic","blue"};
  return scales[static_cast<uint8_t>(scale)];
}

//make sure 'root' isn't passed to this fn as a negative number
vector<uint8_t> Stepchild::makePitchListFromScale(ScaleName scale, uint8_t root){
  vector<uint8_t> newScale;
  switch(scale){
    case MAJOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case DORIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case PHRYGIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+6),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case MIXOLYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case AEOLIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LOCRIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+8),uint8_t(root+10)};
      break;
    case MELODIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case HARMONIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+11)};
    case MAJOR_PENTATONIC:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+7),uint8_t(root+9)};
      break;
    case MINOR_PENTATONIC:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+10)};
      break;
    case BLUE:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+7),uint8_t(root+10)};
      break;
  }
  return newScale;
}

//Functions and definitions for generating scales
vector<uint8_t> Stepchild::makePitchListFromScale(ScaleName scale, uint8_t root, int8_t numOctaves, uint8_t octave){
  vector<uint8_t> newScale;
  for(int8_t i = octave; i<numOctaves+octave; i++){
    vector<uint8_t> temp = makePitchListFromScale(scale,root%12+(i*12));
    for(uint8_t j = 0; j<temp.size(); j++){
      newScale.push_back(temp[j]);
    }
  }
  return newScale;
}

void Stepchild::muteGroups(int callingTrack, int group){
  for(int track = 0; track<trackData.size(); track++){
    if(track != callingTrack && trackData[track].muteGroup == group && trackData[track].noteLastSent != 255){
      midi.noteOff(trackData[track].noteLastSent,0,trackData[track].channel);
      sentNotes.subNote(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
    }
  }
}


String Stepchild::stepsToPosition(int steps,bool verby){
  String text;
  text += stringify(steps/96 + 1);
  text += stringify(":");
  text += stringify((steps%96)/24 + 1);
  return text;
}

String Stepchild::stepsToMeasures(int32_t stepVal){
  String text;
  uint16_t smallest = 0;
  uint16_t steps = stepVal;

  //% steps by each denominator to see if there's no remainder
  //if there's no remainder, then you know that the smallest
  uint16_t denominators[12] = {96,48,32,24,16,12,8,6,4,3,2,1};
  for(uint8_t i = 0; i<12; i++){
    if(smallest == 0){
      steps=stepVal%denominators[i];
      if(steps == 0){
        smallest = i;
        break;
      }
    }
  }
  //steps are each a 1/96
  //this is basically asking "if steps is cleanly divisible by X, with no remainder, then X is the smallest denominator needed"
  //reset steps to the length - wholenotes
  steps = stepVal;
  if(steps>=96){
    text = stringify(steps/96);
    steps %= 96;
  }
  //if there's no numerator
  if(steps/denominators[smallest] == 0){
    if(text == "")
      text = "0";
    return text;
  }
  if(text != "")
    text += " ";
  text += stringify(steps/denominators[smallest])+"/"+stringify(denominators[11-smallest]);
  return text;
}



void Stepchild::leaveSleepMode(){
  lastTime = millis();
}
void Stepchild::enterSleepMode(){
  display.clearDisplay();
  display.display();
  buttons.clearButtons();
  buttons.turnOffLEDs();
  while(true){
    sleep_ms(500);
    if(buttons.anyActiveInputs()){
      leaveSleepMode();
      break;
    }
  }

}

//Deep sleep actually pauses core1 and sleeps core0... not sure how energy saving this is
// tho compared to fully going dormant
#ifdef HEADLESS
  void Stepchild::enterDeepSleepMode(){
    return;
  }
  void Stepchild::leaveDeepSleepMode(){
    return;
  }
#else
void Stepchild::leaveDeepSleepMode(){
  rp2040.resumeOtherCore();
}
void Stepchild::enterDeepSleepMode(){
  //turn off power consuming things
  display.clearDisplay();
  display.display();
  buttons.clearButtons();
  buttons.turnOffLEDs();

  //idle core1
  rp2040.idleOtherCore();
  //sleep until a change is detected on 
  while(true){
    sleep_ms(1000);
    //when input is detected, wake up
    if(buttons.anyActiveInputs()){
      leaveSleepMode();
      return;
    }
  }
}
#endif



Stepchild stepchild;

//These need to be referenced after Autotracks are defined
void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  stepchild.buttons.counterA += (stepchild.recordingToAutotrack && stepchild.autotrackData[stepchild.activeAutotrack].recordFrom == ENCODER_A)?stepchild.buttons.readEncoder(0)*4:stepchild.buttons.readEncoder(0);
  if(stepchild.recordingToAutotrack && stepchild.autotrackData[stepchild.activeAutotrack].recordFrom == ENCODER_A)
    stepchild.idlingUntilNoteReceived = false;
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  stepchild.buttons.counterB += (stepchild.recordingToAutotrack && stepchild.autotrackData[stepchild.activeAutotrack].recordFrom == ENCODER_B)?stepchild.buttons.readEncoder(1)*4:stepchild.buttons.readEncoder(1);
  if(stepchild.recordingToAutotrack && stepchild.autotrackData[stepchild.activeAutotrack].recordFrom == ENCODER_B)
    stepchild.idlingUntilNoteReceived = false;
}