/*

this file contains a lot of miscellaneous utility functions for formatting strings
and converting numbers.

*/

#include "Stepchild.h"
#include "commonStructs.h"

Coordinate getRadian(uint8_t h, uint8_t k, int a, int b, float angle);
uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero);

uint16_t toggleTriplets(uint16_t subDiv);
//Formats a number 1-13 into its Roman Numeral equivalent
//This is really only used for printing intervals in Roman Numeral notation
//eg. printing the 3rd degree of a scale as "III"
String decimalToNumeral(int dec);

float limit(float target,float min,float max);

//returns the number of digits (eg. 100 = 3, 1 = 1) in a byte-sized (8bit) number
uint8_t countDigits_byte(uint8_t number);

uint8_t countSpaces(String text);
uint8_t countChar(String text,unsigned char c);
String getCurveTypeString(CurveType t);

uint8_t getLowestVal(std::vector<uint8_t> vec);
bool isInVector(int val, std::vector<uint8_t> vec);
void hardReset();
//update mode
void enterBootsel();
bool isConnectedToUSBPower();
float getBattLevel();
void maxCurrentDrawTest();
//pulses the onboard LED
void ledPulse(uint8_t speed);
void testButton(uint8_t bit);
/*
  Test routine that prompts the user to use each input, one by one, on the Stepchild to see if everything is working correctly
*/
void testAllInputs();
void debugPrintButtons();
//decrements each note ID if it's larger than the last, so that if these notes are 
//deleted in this order their IDs stay referenced
std::vector<NoteID> crunchNoteIDsForDeletion(std::vector<NoteID> targetNoteIDs);
std::vector<NoteID> getSelectedNoteIDs();
std::vector<NoteTrackPair> sortNotes(std::vector<NoteTrackPair> notes, uint8_t sortBy, uint8_t type);
//sorts a list of [track,note] pairs
std::vector<NoteID> sortNotes(std::vector<NoteID> ids, uint8_t sortBy, uint8_t type);
std::vector<NoteID> grabSelectedNotesAsNoteIDs();
long mapVal(long x, long in_min, long in_max, long out_min, long out_max) ;
float mapVal(float x, float in_min, float in_max, float out_min, float out_max) ;