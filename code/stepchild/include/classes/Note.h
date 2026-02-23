#pragma once
#include "commonEnums.h"

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
    enum NoteFlagBit:uint8_t{
      SELECTED = 0,
      MUTED = 1,
      SUPERPOSITIONED = 2,
      PLAYING = 3
    };
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
    void shift(int16_t);
};