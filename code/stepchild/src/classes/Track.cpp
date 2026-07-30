#include "classes/Track.h"
#include "Stepchild.h"

;

//constructor for each track
Track::Track() {
  muteGroup = 0;
  noteLastSent = 255;
}

Track::Track(unsigned char p, unsigned char c){
  pitch = p;
  channel = c;
  muteGroup = 0;
  noteLastSent = 255;
}
//Returns a formatted string containing the track's pitch and octave
String Track::getPitchAndOctave(){
  return stepchild.pitchToString(this->pitch,true, true);
}
//Returns a formatted string containing the track's pitch
String Track::getPitch(){
  return stepchild.pitchToString(this->pitch, false, true);
}
void Track::reprime(){
  setPrimed(checkFlag(PRIME_BACKUP_WHILE_LIVELOOPING));
}
void Track::storePrimeState(){
  setFlag(PRIME_BACKUP_WHILE_LIVELOOPING,isPrimed());
}
void Track::setFlag(TrackFlagBit b, bool state){
  if(state)
    flags |= (1<<b);
  else
    flags &= ~(1<<b);
}
bool Track::checkFlag(TrackFlagBit b){
  return flags&(1<<b);
}
bool Track::isSelected(){
  return checkFlag(SELECTED);
}
bool Track::isPrimed(){
  return checkFlag(PRIMED);
}
bool Track::isSolo(){
  return checkFlag(SOLO);
}
bool Track::isMuted(){
  return checkFlag(MUTED);
}
bool Track::isLatched(){
  return checkFlag(LATCHED);
}
void Track::setPrimed(bool state){
  setFlag(PRIMED,state);
}
void Track::setSolo(bool state){
  setFlag(SOLO,state);
}
void Track::setSelected(bool state){
  setFlag(SELECTED,state);
}
void Track::setMuted(bool state){
  setFlag(MUTED,state);
}
void Track::setLatched(bool state){
  setFlag(LATCHED,state);
}