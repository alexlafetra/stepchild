#pragma once

#include "classes/Note.h"

struct NoteID{
  public:
      NoteID(uint8_t, uint16_t);
      Note getNote();
      uint8_t getPitch();
      uint8_t track;
      uint16_t id;
};

//structs
struct NoteCoords{
  int16_t x1;
  int16_t length;
  int16_t y1;
  int16_t y2;
  void offsetY(int16_t y){
    y1+=y;
    y2+=y;
  }
};

struct CCData{
  CCData(uint8_t, uint8_t, uint8_t);
  CCData();
  uint8_t cc;
  uint8_t val;
  uint8_t channel;
};

//stores note data (pitch, vel, channel) and source (INTERNAL_CLOCK, EXTERNAL_CLOCK)
struct NoteData{
  uint8_t pitch;
  uint8_t vel;
  uint8_t channel;

  NoteData();
  NoteData(uint8_t, uint8_t, uint8_t);
};


//holds all the data for the echo fx
struct EchoData{
  uint8_t delay = 24;
  uint8_t decay = 75;
  uint8_t repeats = 2;
};

struct RandomData{
  int8_t odds = 60;
  int8_t minChance = 100;
  int8_t maxChance = 100;
  uint8_t minLength = 24;
  uint8_t maxLength = 24;
  uint8_t minVel = 100;
  uint8_t maxVel = 127;
  uint8_t everyNSteps = 24;
};


//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
struct ccChannelValue{
  uint8_t channel = 0;
  uint8_t value = 0;
};

struct GlobalModifiers{
  ccChannelValue velocity;
  ccChannelValue chance;
  ccChannelValue pitch;
};


//Stores loop data as start,end,reps,and type
struct Loop{
  //The start of the Loop (in steps)
  uint16_t start;
  //The end of the Loop (in steps)
  uint16_t end;
  //the number of times-1 the loop will play before linking to the next loop. 0 sets the Loop to play once.
  uint8_t reps;
  //how the Loop links to the next Loop
  LoopType type;
  /*
  Type:
  0 = go to next Loop
  1 = go to a random Loop
  2 = go to a random Loop of the same length
  3 = return to the first Loop
  4 = repeat this loop again (infinite repeat)
  */
  Loop(){}
  Loop(uint16_t s, uint16_t e, uint8_t r, uint8_t t){
      this->start = s;
      this->end = e;
      this->reps = r;
      this->type = static_cast<LoopType>(t);
  }
  uint16_t length(){
    return this->end-this->start;
  }
};


struct NoteTrackPair{
  public:
      Note note;
      uint8_t trackID;
      NoteTrackPair(Note n, uint8_t t);
      uint8_t getPitch();
};


//Stores one x,y pair
struct Coordinate{
    int16_t x;
    int16_t y;
    Coordinate(){
        this->x = 0;
        this->y = 0;
    }
    Coordinate(int16_t x1, int16_t y1){
        this->x = x1;
        this->y = y1;
    }
};

struct CoordinatePair{
    Coordinate start;
    Coordinate end;
    CoordinatePair(){}
    CoordinatePair(uint16_t xStart, uint16_t xEnd){
        this->start = Coordinate(xStart,0);
        this->end = Coordinate(xEnd,0);
    }
    CoordinatePair(uint16_t xStart, uint8_t yStart, uint16_t xEnd, uint8_t yEnd){
        this->start = Coordinate(xStart,yStart);
        this->end = Coordinate(xEnd,yEnd);
    }

    bool isVertical(){
        return (this->start.x) == (this->end.x);
    }
};
