#pragma once
#include <Arduino.h>
#include <vector>
#include "commonStructs.h"
#include "graphics/SequenceRenderSettings.h"

/*
    Functions that involve custom GUI layouts for user input/output
*/
void alert(String text, int time);
String enterText(String title);
String enterText(String title, uint8_t count);
//this one has a title, but displays a blank screen
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title);
//Binary Selection box w no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)());
//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
//Binary Selection box w a title, returns -1 for 
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)());
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, std::vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height);
uint8_t dropDownMenu(std::vector<String> options, void (*drawingFunction)());
void selectKeysAnimation(bool in);
std::vector<uint8_t> selectKeys(uint8_t startRoot);
void drawCoordinateBox(CoordinatePair coords, SequenceRenderSettings& settings);
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool));
bool selectNotes(String t);
CoordinatePair selectNotesAndArea(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool));
void defaultIconFunction(uint8_t x, uint8_t y, uint8_t w, bool s);
std::vector<std::vector<uint8_t>> selectMultipleNotes(String text1, String text2);