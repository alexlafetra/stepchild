//track class
class Track {
  public:
    Track();
    Track(unsigned char, unsigned char); //constructs with a midi channel input
    uint8_t pitch; //for storing which midi note the track sends and receives
    uint8_t channel;//for storing midi channels
    uint8_t muteGroup;
    //255 if no note is being sent, pitch val if currently sending
    uint8_t noteLastSent;//if a note is currently being sent
    bool isSelected;
    bool isLatched;//for sending notes to 'loop' SP404 pads (or triggering latched samples in general)
    bool isPrimed;
    bool isMuted;
    bool isSolo;
};

//constructor for each track
Track::Track() {
  pitch = defaultPitch;//c1 to start
  channel = defaultChannel;
  isSelected = false;
  isLatched = false;
  isPrimed = true;
  isSolo = false;
  isMuted = false;
  muteGroup = 0;
  noteLastSent = 255;
}

Track::Track(unsigned char p, unsigned char c){
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

void addTrack_noMove(Track newTrack, bool loudly);