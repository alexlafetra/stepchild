#pragma once
#include <Arduino.h>

typedef uint16_t Timestep;
typedef uint8_t TrackID;

enum LoopType : uint8_t{
  NORMAL,
  RANDOM,
  RANDOM_SAME_LENGTH,
  RETURN,
  INFINITE
};

LoopType operator++(LoopType &c,int);
LoopType operator--(LoopType &c,int);

enum TextAlign:uint8_t{
  ALIGN_LEFT,
  ALIGN_CENTER,
  ALIGN_RIGHT
};

enum ArrowDirection:uint8_t{
  ARROW_RIGHT,
  ARROW_LEFT,
  ARROW_UP,
  ARROW_DOWN
};


enum CurveType:uint8_t{LINEAR_CURVE,SINEWAVE_CURVE,SQUAREWAVE_CURVE,SAWTOOTH_CURVE,TRIANGLE_CURVE,RANDOM_CURVE,NOISE_CURVE};
CurveType operator++(CurveType &c,int);
CurveType operator--(CurveType &c,int);
enum MovingLoopState{
    MOVING_NO_LOOP_POINTS,
    MOVING_LOOP_END,
    MOVING_LOOP_START,
    MOVING_BOTH_LOOP_POINTS
  };
  
#ifndef HEADLESS
enum PlayState{
  STOPPED,
  PLAYING,
  RECORDING,
};
#endif

enum NoteProperty{
  VELOCITY,
  CHANCE,
  PITCH
};

  //0 is one-shot recording to current loop, 1 is recording to loops as they play in sequence
//2 is recording from seqStart to seqend
enum RecordingMode:uint8_t{
    ONESHOT,
    CURRENT_LOOP,
    LOOP_SEQUENCE,
    FULL_SEQUENCE
};


enum ScaleName:uint8_t{
  MAJOR,
  HARMONIC_MINOR,
  MELODIC_MINOR,
  MAJOR_PENTATONIC,
  MINOR_PENTATONIC,
  BLUE,
  DORIAN,
  PHRYGIAN,
  LYDIAN,
  MIXOLYDIAN,
  AEOLIAN,
  LOCRIAN
};

ScaleName& operator++(ScaleName& e);
ScaleName operator++(ScaleName& e, int);
// Define a free-standing function to overload --
ScaleName& operator--(ScaleName& e);
// Define a free-standing function to overload postfix --
ScaleName operator--(ScaleName& e, int);

enum TriggerSource:uint8_t{
  GLOBAL_TRIGGER,
  TRACK_TRIGGER,
  CHANNEL_TRIGGER
};

TriggerSource operator++(TriggerSource &c,int);
TriggerSource operator--(TriggerSource &c,int);

enum RecordingFrom:uint8_t{
  EXTERNAL_MIDI,
  ENCODER_A,
  ENCODER_B,
  JOY_X,
  JOY_Y
};

enum SlideDirection:uint8_t{
    IN_FROM_RIGHT,
    IN_FROM_LEFT,
    IN_FROM_BOTTOM,
    OUT_FROM_RIGHT,
    OUT_FROM_LEFT,
    OUT_FROM_BOTTOM
};

enum MenuReturnValue:uint8_t{
  NO_ACTION,
  BACK_TO_MAIN_SEQUENCE,
  BACK_TO_MAIN_MENU
};

enum MenuSlideSpeed:uint8_t{
  MENU_SLIDE_FAST = 20,
  MENU_SLIDE_MEDIUM_FAST = 10,
  MENU_SLIDE_MEDIUM = 10,
  MENU_SLIDE_MEDIUM_SLOW = 10,
  MENU_SLIDE_SLOW = 5,
};