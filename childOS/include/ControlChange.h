#pragma once

#include "Arduino.h"
/*
CC numbers:
1 -- Modulation
7 -- Volume
10 -- Pan
11 -- Expression
64 -- Sustain
121 -- Reset All Controllers
123 -- All Notes Off
*/

inline const std::vector<String> CCparameters = {"Bank Select","Mod Wheel","Breath Controller","Undefined","Foot Pedal","Portamento Time",
  "Data Entry","Volume","Balance","Undefined","Pan","Expression","Effect 1","Effect 2","Undefined",
  "Undefined","Gen. Purpose","Undefined","LSB Cont. for ","Damper Pedal","Portamento","Sostenuto Pedal",
  "Soft Pedal","Legato Switch","Hold 2","Sound Cont. 1","Sound Cont. 2","Sound Cont. 3",
  "Sound Cont. 4","Sound Cont. 5","Sound Cont. 6","Sound Cont. 7","Sound Cont. 8",
  "Sound Cont. 9","Sound Cont. 10","Gen. Purpose","Portamento","Undefined","High Res. Vel. Prefix",
  "Undefined","Effect 1 Depth","Effect 2 Depth","Effect 3 Depth", "Effect 4 Depth", "Effect 5 Depth",
  "Increment +1","Increment -1","Non-Reg. Param. LSB","Non-Reg. Param. MSB",
  "Reg. Param. LSB","Reg. Param. MSB","Undefined"};
inline const std::vector<String> MKIICCparameters = {"Ctrl ","EFX On/Off","EFX Selector"};
inline const std::vector<String> stepChildCCParameters = {"Velocity","Probability","Pitch","BPM","Swing"};


String getCCParameterName(uint8_t param);
String getBUSFromChannel(uint8_t channel);
String getMKIIParameterName(uint8_t param,  uint8_t channel);
String getStepchildCCParameterName(uint8_t param, uint8_t channel);
//turns a numbered parameter like "third in the MKII list" to a MIDI CC number like "83"
uint8_t MKIIParamToCC(uint8_t param);
uint8_t stepchildParamToCC(uint8_t p);
uint8_t moveToNextCCParam(uint8_t param, bool up, uint8_t whichList);
String getCCParam(uint8_t param, uint8_t channel, uint8_t type);
void printParam_centered(uint8_t which, uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC);
void printParam(uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC);
void printParamList(uint8_t which, uint8_t target, uint8_t start, uint8_t end, uint8_t channel);
std::vector<uint8_t> switchBetweenCCLists(uint8_t start, uint8_t end, uint8_t targetParam, uint8_t which, bool left);
//menu for selecting a CC parameter
uint8_t selectCCParam_autotrack(uint8_t which);
