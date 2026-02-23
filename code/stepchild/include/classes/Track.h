#pragma once
#include <Arduino.h>

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
    SOLO = 4,
    PRIME_BACKUP_WHILE_LIVELOOPING = 5
  };
  uint8_t flags = 0b00101000;//tracks start primed
  
  //constructor for each track
  Track();
  Track(unsigned char p, unsigned char c);

  //Returns a formatted string containing the track's pitch and octave
  String getPitchAndOctave();
  //Returns a formatted string containing the track's pitch
  String getPitch();
  void setFlag(TrackFlagBit b, bool s);
  bool isSelected();
  bool isMuted();
  bool isLatched();
  bool isPrimed();
  bool isSolo();
  void reprime();
  void storePrimeState();
  void setPrimed(bool);
  void setLatched(bool);
  void setSelected(bool);
  void setSolo(bool);
  void setMuted(bool);
  bool checkFlag(TrackFlagBit);
};