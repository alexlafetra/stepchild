//track class
class Track {
  public:

  uint8_t pitch = 36; //for storing which midi note the track sends and receives
  uint8_t channel = 1;//for storing midi channels
  uint8_t muteGroup;
  //255 if no note is being sent, pitch val if currently sending
  uint8_t noteLastSent;//if a note is currently being sent
  bool isSelected;
  bool isLatched;//for sending notes to 'loop' SP404 pads (or triggering latched samples in general)
  bool isPrimed;
  bool isMuted;
  bool isSolo;

  //constructor for each track
  Track() {
    isSelected = false;
    isLatched = false;
    isPrimed = true;
    isSolo = false;
    isMuted = false;
    muteGroup = 0;
    noteLastSent = 255;
  }

  Track(unsigned char p, unsigned char c){
    pitch = p;
    channel = c;
    isSelected = false;
    isLatched = false;
    isPrimed = true;
    isSolo = false;
    isMuted = false;
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
};