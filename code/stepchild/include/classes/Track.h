//track class
class Track {
  public:

  uint8_t pitch = 36; //for storing which midi note the track sends and receives
  uint8_t channel = 1;//for storing midi channels
  uint8_t muteGroup;
  //255 if no note is being sent, pitch val if currently sending
  uint8_t noteLastSent;//if a note is currently being sent
  enum TrackFlagBit:uint8_t{
    SELECTED = 0,
    MUTED = 1,
    LATCHED = 2,
    PRIMED = 3,
    SOLO = 4
  };
  uint8_t flags = 0b00001000;//tracks start primed
  
  //constructor for each track
  Track() {
    muteGroup = 0;
    noteLastSent = 255;
  }

  Track(unsigned char p, unsigned char c){
    pitch = p;
    channel = c;
    muteGroup = 0;
    noteLastSent = 255;
  }

  //Returns a formatted string containing the track's pitch and octave
  String getPitchAndOctave(){
    return pitchToString(this->pitch,true, true);
  }
  //Returns a formatted string containing the track's pitch
  String getPitch(){
    return pitchToString(this->pitch, false, true);
  }

  void setFlag(TrackFlagBit b, bool s);
  bool isSelected();
  bool isMuted();
  bool isLatched();
  bool isPrimed();
  bool isSolo();
  void setPrimed(bool);
  void setLatched(bool);
  void setSelected(bool);
  void setSolo(bool);
  void setMuted(bool);
  bool checkFlag(TrackFlagBit);

};
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