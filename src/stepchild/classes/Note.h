//storage efficient Note class:
class Note {
  public:
    Note();
    Note(unsigned short int, unsigned short int, unsigned char, unsigned char, bool, bool);
    Note(unsigned short int, unsigned short int, unsigned char);
    Note(unsigned short int, unsigned char, unsigned char);
    uint16_t getLength();
    bool isSelected;
    bool muted;
    uint8_t velocity;
    uint8_t chance;
    uint16_t startPos;
    uint16_t endPos;
    // vector<uint8_t> altPitches;//stores a list of alternative pitches this note can trigger
};

//default constructor
Note::Note() {
  startPos = cursorPos;
  endPos = cursorPos + subDivInt;
  velocity = defaultVel;
  isSelected = false;
  chance = 100;
  muted = false;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel, unsigned char odds, bool mute, bool select) {
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  isSelected = select;
  chance = odds;
  muted = mute;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel){
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  isSelected = false;
  chance = 100;
  muted = false;
}
Note::Note(unsigned short int xPos, unsigned char vel, unsigned char odds){
  startPos = xPos;
  endPos = xPos + subDivInt;
  velocity = vel;
  isSelected = false;
  chance = odds;
  muted = false;
}
uint16_t Note::getLength(){
  return endPos-startPos+1;
}