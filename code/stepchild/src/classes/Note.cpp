#include "classes/Note.h"

//sets a flag
void Note::setFlag(NoteFlagBit b, bool state){
  if(state)
    flags |= (1<<b);
  else
    flags &= ~(1<<b);
}
bool Note::checkFlag(NoteFlagBit b){
  return flags&(1<<b);
}
void Note::setSelected(bool state){
  setFlag(SELECTED,state);
}
void Note::setMuted(bool state){
  setFlag(MUTED,state);
}
void Note::setSuperpositioned(bool state){
  setFlag(SUPERPOSITIONED,state);
}
void Note::setPlaying(bool state){
  setFlag(PLAYING,state);
}
bool Note::isSelected(){
  return checkFlag(SELECTED);
}
bool Note::isMuted(){
  return checkFlag(MUTED);
}
bool Note::isSuperpositioned(){
  return checkFlag(SUPERPOSITIONED);
}
bool Note::isPlaying(){
  return checkFlag(PLAYING);
}
//default constructor
Note::Note(){
  startPos = 0;
  endPos = 24;
  velocity = 127;
  chance = 100;
  superposition = Superposition(255,0);
}
Note::Note(uint16_t s, uint16_t e){
  startPos = s;
  endPos = e;
  velocity = 127;
  chance = 100;
  superposition = Superposition(255,0);
}
Note::Note(uint16_t start, uint16_t end, uint8_t vel, uint8_t chnce, uint8_t flgs){
  startPos = start;
  endPos = end;
  velocity = vel;
  chance = chnce;
  flags = flgs;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel, unsigned char odds, bool mute, bool select) {
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  chance = odds;
  setSelected(select);
  setMuted(mute);
  superposition = Superposition(255,0);
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel){
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  chance = 100;
  superposition = Superposition(255,0);
}
uint16_t Note::getLength(){
  return endPos-startPos+1;
}
//moves both timepoints by the same amount -- DOESN'T check bounds!
void Note::shift(int16_t amount){
  endPos += amount;
  startPos += amount;
}
