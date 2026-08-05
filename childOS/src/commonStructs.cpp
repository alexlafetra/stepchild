#include "Arduino.h"
#include "commonStructs.h"
#include "Stepchild.h"

;

using namespace std;

NoteID::NoteID(uint8_t t, uint16_t i){
  track = t;
  id = i;
}


Note NoteID::getNote(){
  return stepchild.noteData[track][id];
}
uint8_t NoteID::getPitch(){
  return stepchild.trackData[track].pitch;
}


CCData::CCData(){
  cc = 0;
  val = 0;
  channel = 0;
}
CCData::CCData(uint8_t param, uint8_t v, uint8_t c){
  cc = param;
  val = v;
  channel = c;
}

NoteData::NoteData(){
  pitch = 255;
  vel = 0;
  channel = 0;
}

NoteData::NoteData(uint8_t p, uint8_t v, uint8_t c){
  pitch = p;
  vel = v;
  channel = c;
}

NoteTrackPair::NoteTrackPair(Note n, uint8_t t){
  note = n;
  trackID = t;
}
uint8_t NoteTrackPair::getPitch(){
  return stepchild.trackData[trackID].pitch;
}


