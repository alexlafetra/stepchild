struct Superposition{
  //for now, it's always at the same pos but in a different track
  // Timestep position;
  uint8_t pitch;
  uint8_t odds;
  Superposition(){
    pitch = 255;
    odds = 50;
  }
  Superposition(uint8_t t, uint8_t o){
    // position = p;
    pitch = t;
    odds = 50;
  }
};

//storage efficient Note class:
class Note {
  public:
    uint8_t flags = 0;//byte that combines selected, muted, superpositioned, and playing into one flag
    uint8_t velocity;
    uint8_t chance;
    Timestep startPos;
    Timestep endPos;
    Superposition superposition;//note superposition
    Note();
    Note(uint16_t s, uint16_t e);
    Note(unsigned short int, unsigned short int, unsigned char, unsigned char, bool, bool);
    Note(unsigned short int, unsigned short int, unsigned char);
    Note(uint16_t, uint16_t, uint8_t, uint8_t, uint8_t);
    enum NoteFlagBit:uint8_t{
      SELECTED = 0,
      MUTED = 1,
      SUPERPOSITIONED = 2,
      PLAYING = 3
    };
    uint16_t getLength();
    bool isSelected();
    void setSelected(bool);
    void setMuted(bool);
    void setSuperpositioned(bool);
    void setPlaying(bool);
    bool isMuted();
    bool isSuperpositioned();
    bool isPlaying();
    bool checkFlag(NoteFlagBit);
    void setFlag(NoteFlagBit, bool);
};
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
