#include "Stepchild.h"
#include "utils.h"
#include "commonStructs.h"
#include <algorithm>
#include "guiUtilities.h"
#include "StepchildGraphics.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;

using namespace std;

float limit(float target,float min,float max){
  if(target<min)
    return min;
  else if(target>max)
    return max;
  return target;
}

Coordinate getRadian(uint8_t h, uint8_t k, int a, int b, float angle) {
    float x1 = h + a * cos(radians(angle));
    float y1;
    if (angle > 180) {
        y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
    }
    else {
        y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
    }
    return Coordinate(int16_t(x1),int8_t(y1));
}

uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero){
  //down
  if(!direction){
    if(subDiv == 1 && allowZero)
      subDiv = 0;
    else if(subDiv>3)
      subDiv /= 2;
    else if(subDiv == 3)
      subDiv = 1;
  }
  else{
    if(subDiv == 0)
      subDiv = 1;
    else if(subDiv == 1)//if it's one, set it to 3
      subDiv = 3;
    else if(subDiv !=  96 && subDiv != 32){
      //if triplet mode
      if(!(subDiv%2))
        subDiv *= 2;
      else if(!(subDiv%3))
        subDiv *=2;
    }
  }
  return subDiv;
}

uint16_t toggleTriplets(uint16_t subDiv){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDiv == 192){
    subDiv = 32;
  }
  else if(!(subDiv%3)){//if it's in 1/4 mode...
    subDiv = 2*subDiv/3;//set it to triplet mode
  }
  else if(!(subDiv%2)){//if it was in triplet mode...
    subDiv = 3*subDiv/2;//set it to 1/4 mode
  }
  return subDiv;
}

//Formats a number 1-13 into its Roman Numeral equivalent
//This is really only used for printing intervals in Roman Numeral notation
//eg. printing the 3rd degree of a scale as "III"
String decimalToNumeral(int dec){
  String numeral = "";
  if(dec < 4){
    for(int i = 0; i<dec; i++){
      numeral+="I";
    }
  }
  if(dec == 4)
    numeral = "IV";
  if(dec>4 && dec<9){
    numeral = "V";
    for(int i = 5; i<dec; i++){
      numeral += "I";
    }
  }
  if(dec == 9)
    numeral = "IX";
  if(dec>9 && dec<13){
    numeral = "X";
    for(int i = 10; i<dec; i++){
      numeral += "I";
    }
  }
  return numeral;
}

//returns the number of digits (eg. 100 = 3, 1 = 1) in a byte-sized (8bit) number
uint8_t countDigits_byte(uint8_t number){
  uint8_t count = 1;//it's ~always~ at least 1 digit
  while(number>=10){
    number/=10;
    count++;
  }
  return count;
}

uint8_t countSpaces(String text){
  uint8_t count = 0;
  for(uint8_t i = 0; i<text.length(); i++){
    if(text.charAt(i) == ' '){
      count++;
    }
  }
  return count;
}

uint8_t countChar(String text,unsigned char c){
  uint8_t count = 0;
  for(uint8_t i = 0; i<text.length(); i++){
    if(text.charAt(i) == c){
      count++;
    }
  }
  return count;
}

String getCurveTypeString(CurveType t){
  switch(t){
    case LINEAR_CURVE:
      return "linear";
    case SINEWAVE_CURVE:
      return "sinewave";
    case SQUAREWAVE_CURVE:
      return "squarewave";
    case SAWTOOTH_CURVE:
      return "sawtooth";
    case TRIANGLE_CURVE:
      return "triangle";
    case RANDOM_CURVE:
      return "random";
    case NOISE_CURVE:
      return "noise";
    default:
      return "idk bruh";
  }
}

uint8_t getLowestVal(std::vector<uint8_t> vec){
  uint8_t lowest = 255;
  for(uint8_t i = 0; i<vec.size(); i++){
    if(vec[i]<lowest)
      lowest = vec[i];
  }
  return lowest;
}

bool isInVector(int val, std::vector<uint8_t> vec){
  if(vec.size() == 0) 
    return false;
  for(int i = 0; i<vec.size(); i++){
    if(val == vec[i]){
      return true;
    }
  }
  return false;
}


void hardReset(){
  rp2040.reboot();
}

//update mode
void enterBootsel(){
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(0,0,childOS_bmp,128,64,SSD1306_WHITE);
  stepchild.display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}

bool isConnectedToUSBPower(){
  return digitalRead(USB_PIN);
}

#define BATTSCALE 0.00966796875
//3.0*3.3/1024.0;
//idk why ^^this isn't 4095.0, but it ain't

float getBattLevel(){
  //So when USB is in, VSYS is ~5.0
  //When all 3AA's are in, if they're 1.5v batts VSYS is ~4.5
  //But if they're 1.2v batts VSYS is ~3.6;
  float val = float(analogRead(VOLTAGE_PIN))*BATTSCALE;
  return val;
}

void maxCurrentDrawTest(){
  stepchild.buttons.writeLEDs(0b1111111111111111);
  stepchild.display.fillRect(0,0,128,64,1);
  stepchild.display.display();
  while(true){
  }
}

//pulses the onboard LED
void ledPulse(uint8_t speed){
  //use abs() so that it counts DOWN when millis() overflows into the negatives
  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
  analogWrite(ONBOARD_LED,4*abs(int8_t(millis()/speed)));
}

void testButton(uint8_t bit){
#ifndef HEADLESS
  do{
    stepchild.buttons.readMainButtons();
  }
  while(!(stepchild.buttons.mainButtonState&(1<<bit)));
#endif
}

/*
  Test routine that prompts the user to use each input, one by one, on the Stepchild to see if everything is working correctly
*/
void testAllInputs(){
  Serial.println("--- Input Test! ---");
  Serial.print("Press 'New': ");
  Serial.flush();
  testButton(NEW_BUTTON);
  Serial.println("NEW!");
  Serial.print("Press 'Shift': ");
  Serial.flush();
  testButton(SHIFT_BUTTON);
  Serial.println("SHIFT!");
  Serial.print("Press 'Select': ");
  Serial.flush();
  testButton(SELECT_BUTTON);
  Serial.println("SELECT!");
  Serial.print("Press 'Delete': ");
  Serial.flush();
  testButton(DELETE_BUTTON);
  Serial.println("DELETE!");
  Serial.print("Press 'Loop': ");
  Serial.flush();
  testButton(LOOP_BUTTON);
  Serial.println("LOOP!");
  Serial.print("Press 'Play': ");
  Serial.flush();
  testButton(PLAY_BUTTON);
  Serial.println("PLAY!");
  Serial.print("Press 'Copy': ");
  Serial.flush();
  testButton(COPY_BUTTON);
  Serial.println("COPY!");
  Serial.print("Press 'Menu': ");
  Serial.flush();
  testButton(MENU_BUTTON);
  Serial.println("Menu!");
  Serial.print("Press 'A': ");
  Serial.flush();
  testButton(A_BUTTON);
  Serial.println("A!");
  Serial.print("Press 'B': ");
  Serial.flush();
  testButton(B_BUTTON);
  Serial.println("B!");

  //Encoder Dials
  stepchild.buttons.clearButtons();
  Serial.print("Roll A: ");
  while(!stepchild.buttons.counterA){};
  Serial.println(stepchild.buttons.counterA>0?"UP!":"DOWN!");
  stepchild.buttons.clearButtons();
  Serial.print("Roll B: ");
  while(!stepchild.buttons.counterB){};
  Serial.println(stepchild.buttons.counterB>0?"UP!":"DOWN!");

  //Joystick
}

void debugPrintButtons(){
#ifndef HEADLESS
  Serial.println("-- main buttons --");
  for(uint8_t i = 0; i<8; i++){
    Serial.print((stepchild.buttons.mainButtonState>>i)&1);
  }
  Serial.println("X:"+stringify(stepchild.buttons.joystickX)+" ("+stringify(analogRead(JOYSTICK_X))+")");
  Serial.println("Y:"+stringify(stepchild.buttons.joystickY)+" ("+stringify(analogRead(JOYSTICK_Y))+")");
  Serial.flush();
#endif
}


//decrements each note ID if it's larger than the last, so that if these notes are 
//deleted in this order their IDs stay referenced
std::vector<NoteID> crunchNoteIDsForDeletion(std::vector<NoteID> targetNoteIDs){
  for(uint8_t i = 0; i<targetNoteIDs.size(); i++){
    //start w/ the next note
    for(uint8_t j = i+1; j<targetNoteIDs.size(); j++){
      if(targetNoteIDs[i].id<targetNoteIDs[j].id)
        targetNoteIDs[j].id--;
    }
  }
  return targetNoteIDs;
}

std::vector<NoteID> getSelectedNoteIDs(){
  std::vector<NoteID> ids = {};
  for(uint16_t i = 0; i<stepchild.noteData.size(); i++){
    for(uint16_t j = 1; j<stepchild.noteData[i].size(); j++){
      if(stepchild.noteData[i][j].isSelected()){
        ids.push_back(NoteID(i,j));
      }
    }
  }
  return ids;
}


bool compareTracks_NoteID(NoteID n1, NoteID n2){
    return n1.track>n2.track;
}
bool comparePitches_NoteID(NoteID n1, NoteID n2){
    return n1.getPitch()>n2.getPitch();
}
bool compareTracks_NoteTrackPair(NoteTrackPair n1, NoteTrackPair n2){
  return n1.trackID>n2.trackID;
}
bool comparePitches_NoteTrackPair(NoteTrackPair n1, NoteTrackPair n2){
  return n1.getPitch()>n2.getPitch();
}

vector<NoteTrackPair> sortNotes(vector<NoteTrackPair> notes, uint8_t sortBy, uint8_t type){
  vector<NoteTrackPair> sortedVec = notes;
  switch(sortBy){
    //sort by pitch
    case 0:
      sort(sortedVec.begin(), sortedVec.end(),comparePitches_NoteTrackPair);
      break;
    //sort by track
    case 1:
      sort(sortedVec.begin(), sortedVec.end(),compareTracks_NoteTrackPair);
      break;
  }
  //if it's ascending
  if(type == 0){
    return sortedVec;
  }
  //if it's descending, reverse it
  else{
    reverse(sortedVec.begin(),sortedVec.end());
    return sortedVec;
  }
}

//sorts a list of [track,note] pairs
vector<NoteID> sortNotes(vector<NoteID> ids, uint8_t sortBy, uint8_t type){
  vector<NoteID> sortedVec = ids;
  switch(sortBy){
    //sort by pitch
    case 0:
      sort(sortedVec.begin(), sortedVec.end(),comparePitches_NoteID);
      break;
    //sort by track
    case 1:
      sort(sortedVec.begin(), sortedVec.end(),compareTracks_NoteID);
      break;
  }
  //if it's ascending
  if(type == 0){
    return sortedVec;
  }
  //if it's descending, reverse it
  else{
    reverse(sortedVec.begin(),sortedVec.end());
    return sortedVec;
  }
}

vector<NoteID> grabSelectedNotesAsNoteIDs(){
    vector<NoteID> notes;
    for(uint8_t i = 0; i<stepchild.noteData.size(); i++){
        for(uint8_t j = 1; j<stepchild.noteData[i].size(); j++){
            if(stepchild.noteData[i][j].isSelected()){
                NoteID newNote = NoteID(i,j);
                notes.push_back(newNote);
            }
        }
    }
    return notes;
}

//reimplementing the map() fn with a new name because it conflicts
long mapVal(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float mapVal(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
