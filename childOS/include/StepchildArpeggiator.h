#pragma once
#include <vector>
#include <cstdint>

enum ArpSource:uint8_t{
  NOTES_FROM_MIDI_INPUT,
  NOTES_FROM_SEQUENCE,
  NOTES_FROM_SEQUENCE_AND_MIDI_INPUT
};
ArpSource& operator%=(ArpSource& lhs, int rhs);
ArpSource operator--(ArpSource &c,int);
ArpSource operator++(ArpSource &c,int);

//arpeggiator objects! these store which notes they play (scale), in what order (order), how fast (subDiv)
//what does an arpeggiator need? which notes to play, how fast to play them, and in what order
//also, randomness
//arps don't create notes in data, unless you commit. They play 'virtual notes', so no data is actually created unless you want it to be
class Stepchild;

class StepchildArpeggiatior {
public:

  StepchildArpeggiatior(Stepchild*);

  Stepchild* stepchild;
  
  std::vector<unsigned char> notes;
  std::vector<unsigned char> order;
  std::vector<unsigned char> lengths;
  std::vector<unsigned char> extendedNotes;

  uint16_t arpSubDiv = 24;

  //activeNote is the index of the order list, we move through order to call notes
  uint8_t activeNote;
  uint8_t range;
  uint8_t channel;
  uint8_t maxVelMod;
  uint8_t minVelMod;
  uint8_t chanceMod;
  uint8_t repMod;
  uint8_t maxPitchMod;
  uint8_t minPitchMod;
  uint8_t playheadPos;
  uint8_t playStyle;
  uint8_t lastPitchSent;
  
  bool isActive;
  bool playing;
  bool uniformLength;
  bool holding;
  ArpSource source = NOTES_FROM_SEQUENCE_AND_MIDI_INPUT; //can be 0 (external only) 1 (internal only) or 2 (both)

  void grabNotesFromPlaylist();
  void playstep();
  void selectExtended();
  void start();
  void stop();
  void setOrder();
  bool hasItBeenEnoughTime();
  bool addStepLength(uint16_t, uint8_t);
  bool addStepLength(uint16_t);
  bool addStepLength();
  uint8_t getOrder();
  //vars for the timing function
  unsigned long startTime;
  unsigned int offBy;
  unsigned long timeLastStepPlayed;
  uint16_t stepCount;
};