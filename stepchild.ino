//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico, not the arduino one
//will overclock at 250MH!!

//library changes:
//- added customo graphic to SSD1306 driver
//- switched out the adafruit GFX library for the pico optimized one
//- turned off Sysex in the MIDI library
#ifndef HEADLESS
  #include <vector>
  #include <Arduino.h>
  #include <Adafruit_TinyUSB.h>
  #include <MIDI.h>

  //for communicating w screen
  #include <Wire.h>

  //for drawing
  #include <Adafruit_GFX.h>//using one optimized for the pico
  #include <Adafruit_SSD1306.h>

  //for a nice font
  #include <Fonts/FreeSerifItalic9pt7b.h>
  #include <Fonts/FreeSerifItalic12pt7b.h>
  #include <Fonts/FreeSerifItalic24pt7b.h>

  //for flash storage
  #include <LittleFS.h> // LittleFS is declared
  #include "pico/stdlib.h"
  #include <SoftwareSerial.h>
  extern "C" {
  #include "pico.h"
  #include "pico/time.h"
  #include "pico/bootrom.h"
  }
  //=========================================================PINS===============================================
  //setting up screen
  #define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
  // #define i2c_Address 0x3D //initialize with the I2C addr 0x3C Typically eBay OLED's

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define OLED_RESET -1   //   QT-PY / XIAO
  // #define OLED_RESET 7   //   QT-PY / XIAO

  #undef SSD1306_NO_SPLASH
  #define SSD1306_NO_SPLASH 1

  #undef CFG_TUH_RPI_PIO_USB
  #define CFG_TUH_RPI_PIO_USB 1
#endif

//#include <bits/stdc++.h>

// #define SSD1306_SETDISPLAYCLOCKDIV B0011000  ///< See datasheet

#define UPRIGHT 2
#define UPSIDEDOWN 0

using namespace std;

#ifndef HEADLESS
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  // Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

//program booleans and global data, constants
#include "global.h"
#include "bitmaps.h"
//function prototypes
#include "prototypes.h"
//button defs and reading functions
#include "buttons.h"
//wireframe stuff
#include "WireFrame.h"

void fileMenuControls_miniMenu(WireFrame* w);
bool fileMenuControls(uint8_t menuStart, uint8_t menuEnd,WireFrame* w);

//classes
#include "Note.h"

void makeNote(Note newNoteOn, int track, bool loudly);
void makeNote(Note newNoteOn, int track);
void loadNote(Note newNote, uint8_t track);
bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart);

#include "Track.h"
#include "AutomationTrack.h"

#ifndef HEADLESS
  String stringify(int a){
    return String(a);
  }
  String stringify(const char * a){
    return String(a);
  }
  int toInt(String s){
    return s.toInt();
  }
#endif

#include "drawingFunctions.h"
#include "fonts/7_segment.cpp"
#include "fonts/cursive.cpp"
#include "fonts/small.cpp"
#include "fonts/arp.cpp"
#include "fonts/italic.cpp"
#include "fonts/chunky.cpp"
#include "scales.h"

#include "menus/strum.cpp"



#ifndef HEADLESS
  SoftwareSerial Serial3 = SoftwareSerial(SerialPIO::NOPIN,txPin_3);
  SoftwareSerial Serial4 = SoftwareSerial(SerialPIO::NOPIN,txPin_4);

  //USB MIDI object
  Adafruit_USBD_MIDI usb_midi;

  //1 and 2 are I/0, 3&4 are just O
  MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI0);
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
  MIDI_CREATE_INSTANCE(SoftwareSerial, Serial3, MIDI3);
  MIDI_CREATE_INSTANCE(SoftwareSerial, Serial4, MIDI4);
#endif

//Data variables -------------------------------------------
Note offNote; //default note, always goes in element 0 of seqData for each track
unsigned int selectionCount = 0;
vector<vector<Note>> copyBuffer;//stores copied notes
unsigned short int copyPos[2];
//loopData is loopData[loopID][start, end, iterations]
//stores cursor position in copy pos, makes a copy of all selected notes (or the target note)
vector<Note> defaultVec = {offNote};//default vector for a track, holds offNote at 0
vector<vector<uint16_t> > lookupData; //char map of notes; 0 = no note, 1-665,535 = noteID
vector<Track> trackData;//holds tracks
vector<vector<Note>> seqData;//making a 2D vec, number of rows = tracks, number of columns = usable notes, and stores Note objects
vector<vector<uint16_t>> loopData = {{0,96,0,0}};//start,end,iterations,style
//(style is either 0 = normal, 2 = random of same length, 1 = random any, 4 = inf repeat, 3 = return)
//holds all the datatracks
vector<dataTrack> dataTrackData;

volatile bool noteOnReceived = false;
volatile bool noteOffReceived = false;


#include "Arp.h"
#include "SolarSystem.h"

Arp activeArp;


unsigned short int animOffset = 0;//for animating curves

//this is just for the selectionbox
class SelectionBox{
  public:
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
  bool begun;
  SelectionBox();
  void displaySelBox();
};

SelectionBox::SelectionBox(){
  x1 = 0;
  y1 = 0;
  x2 = 0;
  y2 = 0;
  begun = false;
}

void SelectionBox::displaySelBox(){
  x2 = cursorPos;
  y2 = activeTrack;

  unsigned short int startX;
  unsigned short int startY;
  unsigned short int len;
  unsigned short int height;

  unsigned short int X1;
  unsigned short int X2;
  unsigned short int Y1;
  unsigned short int Y2;

  if(x1>x2){
    X1 = x2;
    X2 = x1;
  }
  else{
    X1 = x1;
    X2 = x2;
  }
  if(y1>y2){
    Y1 = y2;
    Y2 = y1;
  }
  else{
    Y1 = y1;
    Y2 = y2;
  }

  startX = trackDisplay+(X1-viewStart)*scale;
  len = (X2-X1)*scale;

  //if box starts before view
  if(X1<viewStart){
    startX = trackDisplay;//box is drawn from beggining, to x2
    len = (X2-viewStart)*scale;
  }
  //if box ends past view
  if(X2>viewEnd){
    len = (viewEnd-X1)*scale;
  }

  //same, but for tracks
  uint8_t startHeight = (menuIsActive||maxTracksShown==5)?debugHeight:8;
  startY = (Y1-startTrack)*trackHeight+startHeight;
  height = ((Y2+1-startTrack)*trackHeight)+startHeight - startY;
  if(Y1<startTrack){
    startY = startHeight;
    height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
  }
  display.drawRect(startX, startY, len, height, SSD1306_WHITE);
  display.drawRect(startX+1, startY+1, len-2, height-2, SSD1306_WHITE);

  // display.drawRect(startX+2, startY+2, len-4, height-4, SSD1306_WHITE);
  if(len>5 && height>=trackHeight){
    // display.fillRect(startX+3,startY+3, len-6, height-6, SSD1306_BLACK);
    // shadeArea(startX+3,startY+3, len-6, height-6,10);
    display.fillRect(startX+2,startY+2, len-4, height-4, SSD1306_BLACK);
    shadeArea(startX+2,startY+2, len-4, height-4,10);
  }
}

SelectionBox selBox;

class Curve{
  public:
  Curve();
  vector<short int> points;//stores the y-vals
  unsigned short int period;
  unsigned short int function;//stores lookup to which function to generate it with
  unsigned short int phase;
  unsigned short int amplitude;
  unsigned short int length;//stores over what range the curve is applied to
  unsigned short int xAxis;//vertical translation
  bool absolute;//whether or not the curve adds/substracts to, or replaces the values
};

Curve::Curve(){
  function = 0;//default is a sine wave
  phase = 0;//default is 0º
  amplitude = 127;
  absolute = false;
  length = 0;
  xAxis = screenHeight/2;
  period = screenWidth/10;
  for(int i = 0; i<length; i++){
    points.push_back(0);
  }
}

Curve debugCurve;

void zella(){
  WireFrame w = makeBox(10,10,10);
  w.xPos = 32;
  w.yPos = screenHeight/2;
  w.rotate(45,2);
  w.rotate(45,0);
  w.scale = 2;
  WireFrame z = makeBox(10,10,10);
  z.xPos = 96;
  z.yPos = screenHeight/2;
  z.rotate(45,2);
  z.rotate(45,0);
  z.scale = 2;
  while(true){
    w.rotate(5,1);
    z.rotate(5,1);
    display.clearDisplay();
    display.drawBitmap(0,0,zella_bmp,128,64,SSD1306_WHITE);
    w.render();
    z.render();
    display.display();
  }
}

void noBitches(){
    while(true){
    display.clearDisplay();
    display.drawBitmap(4,0,no_bitches_bmp,116,64,SSD1306_WHITE);
    display.display();
  }
}

#include "menus/random.cpp"

void addNoteToPlaylist(uint8_t note, uint8_t vel, uint8_t channel){
  vector<uint8_t> temp = {note,vel,channel};
  playlist.push_back(temp);
}
void subNoteFromPlaylist(uint8_t note){
  vector<vector<uint8_t>> tempList;
  //keep all the notes, EXCEPT for the one you're kicking out
  for(int i = 0; i<playlist.size(); i++){
    if(note != playlist[i][0]){
      tempList.push_back(playlist[i]);
    }
  }
  playlist.swap(tempList);
}
void clearPlaylist(){
  vector<vector<uint8_t>> tempList;
  playlist.swap(tempList);
}
void printPlaylist(){
  Serial.print("{");
  for(int i = 0; i<playlist.size(); i++){
    Serial.print(playlist[i][0]);
    Serial.print(",");
  }
  Serial.println("}");
  Serial.flush();
}

void genCurveDebug(){//this creates the points, then draws them
  debugCurve.points.erase(debugCurve.points.begin(), debugCurve.points.end());
  if(debugCurve.function == 0){
    for(float point = 0; point<=debugCurve.length; point++){
      float p = (debugCurve.amplitude/2)*sin(PI*(point+debugCurve.phase)/debugCurve.period);
      debugCurve.points.push_back(p);
    }
  }
  else if(debugCurve.function == 1){//square
    int sign = 1;//for stepwise ops
    for(float point = 0; point<=debugCurve.length; point++){
      if(!(int(point+debugCurve.phase)%(debugCurve.period))){
        sign = -sign;
      }
      float p = (debugCurve.amplitude/2)*sign;
      debugCurve.points.push_back(p);
    }
  }
  //saw
  else if(debugCurve.function == 2){
    int point2 = 0;
    float slope = debugCurve.amplitude/(debugCurve.period*2);
    for(float point = 0; point<=debugCurve.length; point++){
      if(!(int(point+debugCurve.phase)%(debugCurve.period*2)))
        point2 = 0;
      float p = point2*slope - debugCurve.amplitude/2;
      point2++;
      debugCurve.points.push_back(p);
    }
  }
  //was
  else if(debugCurve.function == 3){
    int point2 = 0;
    float slope = -debugCurve.amplitude/(debugCurve.period*2);
    for(float point = 0; point<=debugCurve.length; point++){
      if(!(int(point+debugCurve.phase)%(debugCurve.period*2)))
        point2 = 0;
      float p = point2*slope + debugCurve.amplitude/2;
      point2++;
      debugCurve.points.push_back(p);
    }
  }
  //triangle
  else if(debugCurve.function == 4){
    float slope = debugCurve.amplitude/debugCurve.period;
    int val = 1;
    int point2 = 0;
    for(float point = 0; point<=debugCurve.length; point++){
      if(!(int(point+debugCurve.phase)%(debugCurve.period))){
        val = -val;
      }
      float p = point2*slope + debugCurve.amplitude/2;
      debugCurve.points.push_back(p);
      point2+=val;
    }
  }
  //random
  else if(debugCurve.function == 5){
    for(float point = 0; point<=debugCurve.length; point++){
      if(point>debugCurve.period){
        debugCurve.points.push_back(debugCurve.points[int(point)%debugCurve.period]);
      }
      else{
        float p = random(-debugCurve.amplitude/2,debugCurve.amplitude/2);
        debugCurve.points.push_back(p);
      }
    }
  }
}

void drawNodeEditingIcon(uint8_t xPos, uint8_t yPos, uint8_t type, uint8_t frame, bool text){
  const uint8_t width = 19;
  const uint8_t height = 10;
  display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);

  //in this context, type means the kind of interpolation algorithm you're using
  switch(type){
    //linear
    case 0:
      {
      uint8_t p1[2] = {3,(uint8_t)(5+sin(millis()/100))};
      uint8_t p2[2] = {15,(uint8_t)(5+cos(millis()/100))};
      display.drawLine(xPos+p1[0]+2,yPos+p1[1],xPos+p2[0]-2,yPos+p2[1],SSD1306_WHITE);
      display.drawCircle(xPos+p1[0],yPos+p1[1],1,SSD1306_WHITE);
      display.drawCircle(xPos+p2[0],yPos+p2[1],1,SSD1306_WHITE);
      if(text)
        printSmall(xPos+width/2-6,yPos+height+2,"lin",SSD1306_WHITE);
      break;
      }
    //elliptical UP
    case 1:
      {
      uint8_t lastPoint;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos+sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+1;
        if(point == 0){
          display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        }
        else{
          display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        }
        lastPoint = pt;
      }
      if(text)
        printSmall(xPos+width/2-4,yPos+height+2,"up",SSD1306_WHITE);
      break;
      }
    //elliptical DOWN
    case 2:
      {
      uint8_t lastPoint;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos-sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+height-1;
        if(point == 0)
          display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        else
          display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = pt;
      }
      if(text)
        printSmall(xPos+width/2-8,yPos+height+2,"down",SSD1306_WHITE);
      break;
      }
  }
}
//draws a curve. Frame is to animated it! basically, sets x-offset. Pass a constant to it for no animation.
void drawCurveIcon(uint8_t xPos, uint8_t yPos, uint8_t type, uint8_t frame){
  //width
  const uint8_t width = 19;
  const uint8_t height = 10;
  display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  switch(type){
    //custom curve (default)
    case 0:
      drawNodeEditingIcon(xPos,yPos,0,frame,false);
      break;
    //sin curve
    case 1:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      uint8_t lastPoint;
      //drawing each sinPoint
      for(uint8_t point = 0; point<width; point++){
        if(point == 0){
          display.drawPixel(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),SSD1306_WHITE);
        }
        else
          display.drawLine(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = yPos+height/2+(height/2-3)*sin(PI*(point+frame)/(width/2));
      }
      break;
    //square
    case 2:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      for(uint8_t point = 0; point<width; point++){
        //if it's less than, it's low
        if((point+frame)%width<width/2)
          display.drawPixel(point+xPos,yPos+height-3,SSD1306_WHITE);
        //if it's greater than half a period, it's high
        else if((point+frame)%width>width/2 && (point+frame)%width<width-1)
          display.drawPixel(point+xPos,yPos+2,SSD1306_WHITE);
        //if it's equal to half a period, then it's a vert line
        else
          display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
      }
      break;
    //saw
    case 3:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        for(uint8_t point = 0; point<width; point++){
          uint8_t pt = (point+frame)%int(width/3);
          if(pt == 0)
            display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
          else
            display.drawPixel(point+xPos,yPos+pt+2,SSD1306_WHITE);
        }
      }
      break;
    //triangle
    case 4:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        int8_t coef = 1;
        uint8_t pt;
        for(uint8_t point = 0; point<width; point++){
          if((point+frame)%(width/2)>=width/4)
            pt = -(point+frame)%(width/2)+2*height/2;
          else
            pt = (point+frame)%(width/2)+3;
          display.drawPixel(xPos+point,yPos+pt+height/8,SSD1306_WHITE);
        }
      }
      break;
    case 5:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        printSmall(xPos+4,yPos+3+sin(millis()/100),"R",SSD1306_WHITE);
        printSmall(xPos+8,yPos+3+sin(millis()/100+1),"N",SSD1306_WHITE);
        printSmall(xPos+12,yPos+3+sin(millis()/100+2),"D",SSD1306_WHITE);

      }
      break;
  }
}

String getTitleText(uint8_t which){
  switch(which){
    case 0:
      return "Sequence Editor";
    case 1:
      return "Track Editor";
    case 2: 
      return "Note Editor";
  }
  return "";
}
uint8_t countStrings(uint8_t which){
  uint8_t size = 0;
  for(int i = 0; i<sizeof(helptext[which])/sizeof(*helptext[which]); i++){
    if(helptext[which][i] != "")
      size++;
  }
  return size;
}

void displayHelpText(uint8_t which){
  uint8_t startLine = 0;//controls starting line
  uint8_t scrollBar = 0;
  uint8_t numberOfLines = 6;
  uint8_t windowLength = 128;
  bool info = false;

  while(true){
    display.clearDisplay();
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        break;
      }
      if(shift){
        info = !info;
        lastTime = millis();
      }
    }
    if(itsbeen(50)){
      if(y != 0){
        if(y == -1 && startLine>0){
          startLine--;
          lastTime = millis();
        }
        else if(y == 1 && startLine+numberOfLines<sizeof(helptext[which])/sizeof(*helptext[which])){
          startLine++;
          lastTime = millis();
        }
      }
    }
    while(counterA != 0 || counterB != 0){
      if(counterA >= 1 || counterB >= 1){
        //if there's a line offscreen
        if(startLine+numberOfLines<23)
          startLine++;
      }
      else if(counterA <= -1||counterB <= -1){
        if(startLine>0)
          startLine--;
      }
      counterA += counterA<0?1:-1;
      counterB += counterB<0?1:-1;
    }
    display.setCursor(0,0);
    display.print(getTitleText(which));
    //drawing the text
    if(info){
      display.drawRect(-1,-1,getTitleText(which).length()*6+4,12,SSD1306_WHITE);
      printSmall_overflow(4,15,4,infotext[which],SSD1306_WHITE);
    }
    else{
      display.drawRect(0,11,windowLength,64,SSD1306_WHITE);
      for(int line = startLine; line<sizeof(helptext[which])/sizeof(*helptext[which]); line++){
        printSmall(4,15+(line-startLine)*7,helptext[which][line],SSD1306_WHITE);
      }
    }
    // Serial.println(sizeof(helptext[which])/sizeof(*helptext[which]));
    Serial.println(startLine);
    display.display();
  }
  return;
}

void restartSerial(unsigned int baud){
  Serial.end();
  delay(1000);
  Serial.begin(baud);
}

void hardReset(){
  rp2040.reboot();
}

void resetUSBInterface(){
}
//update mode
void enterBootsel(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}

//chord object
class Chord{
  public:
  //intervals represented as half steps from root note
  vector<uint8_t> intervals;
  //root note
  uint8_t root;
  //stores the inversion of the chord
  //basically how to finger it
  uint8_t inversion;
  uint16_t length;
  Chord();
  Chord(uint8_t, vector<uint8_t>, uint16_t);
  void play();
  void stop();
  void draw(uint8_t, uint8_t);
  void printPitchList(uint8_t, uint8_t);
  String getPitchList(uint8_t, uint8_t);
};
Chord::Chord(){
  root = 0;
  intervals = {0};
  length = 24;
}

Chord::Chord(uint8_t r, vector<uint8_t> i, uint16_t l){
  root = r;
  intervals = i;
  length = l;
}

//sends a midi on for each of the notes in the chord
void Chord::play(){
  for(uint8_t i = 0; i<intervals.size(); i++){
    sendMIDInoteOn(intervals[i]+root,127,0);
  }
}

//sends a midi off for each of the notes in the chord
void Chord::stop(){
  for(uint8_t i = 0; i<intervals.size(); i++){
    sendMIDInoteOff(intervals[i]+root,0,0);
  }
}

void Chord::printPitchList(uint8_t x1, uint8_t y1){
  String text = "(";
  for(uint8_t i = 0; i<intervals.size();i++){
    text+=pitchToString(intervals[i]+root,false,true);
    if(i != intervals.size()-1)
      text+=",";
    else
      text+=")";
  }
  printSmall(x1-(text.length()-1)*2,y1,text,SSD1306_WHITE);
}
String Chord::getPitchList(uint8_t x1, uint8_t y1){
  String text;
  for(uint8_t i = 0; i<intervals.size();i++){
    text+=pitchToString(intervals[i]+root,false,true);
    if(i != intervals.size()-1)
      text+=",";
  }
  return text;
}

const uint8_t chordHeight = 2;
//draws a chord
void Chord::draw(uint8_t x1, uint8_t y1){
  if(intervals.size() == 0)
    return;
  int8_t y2 = y1-(chordHeight+1)*intervals.size();
  for(uint8_t i = 0; i<intervals.size(); i++){
    display.drawRoundRect(x1,y2+i*(chordHeight+1),length/4,chordHeight,3,SSD1306_WHITE);
    // Serial.println(i);
    // Serial.flush();
  }
}
class Progression{
  public:
  vector<Chord> chords;
  Progression();
  Progression(vector<Chord>);
  void drawProg(uint8_t, uint8_t, int8_t);
  void add(Chord);
  void duplicate(uint8_t);
  void remove(uint8_t);
  uint8_t getLargestChordSize();
  void commit();
};

Progression::Progression(){
}
Progression::Progression(vector<Chord> c){
  chords = c;
}
void Progression::add(Chord c){
  chords.push_back(c);
}

void Progression::remove(uint8_t chord){
  vector<Chord> newChords;
  for(uint8_t i = 0; i<chords.size(); i++){
    if(i != chord)
      newChords.push_back(chords[i]);
  }
  chords.swap(newChords);
}

void Progression::duplicate(uint8_t chord){
  vector<Chord> newChords;
  for(uint8_t i = 0; i<chords.size(); i++){
    newChords.push_back(chords[i]);
    if(i == chord)
      newChords.push_back(chords[i]);
  }
  chords.swap(newChords);
}

uint8_t Progression::getLargestChordSize(){
  uint8_t largestIndex = 0;
  uint8_t largest = 0;
  for(uint8_t i = 0; i<chords.size(); i++){
    if(chords[i].intervals.size()>largest){
      largest = chords[i].intervals.size();
      largestIndex = i;
    }
  }
  return largest;
}

void Progression::drawProg(uint8_t x1, uint8_t y1,int8_t activeChord){
  if(chords.size()==0)
    return;
  uint16_t x2 = x1;
  uint8_t height = getLargestChordSize()*(chordHeight+1);
  String pitches = "(";
  //go thru each chord
  for(uint8_t i = 0; i<chords.size(); i++){
    chords[i].draw(x2,y1);
    if(activeChord != -1){
      if(i == activeChord){
        drawCurlyBracket(x2+chords[i].length/4,y1+1,chords[i].length/4,0,true,true,1);
        // display.drawFastHLine(x2,y1+3,chords[i].length/4,SSD1306_WHITE);
        chords[i].printPitchList(x2,y1+6);
      }
    }
    x2+=chords[i].length/4+1;
  }
  // display.drawRoundRect(x1-2,y1-height,screenWidth-x1,height+2,3,SSD1306_WHITE);
}

//makes tracks and notes and places them in the sequence
void Progression::commit(){
  // Serial.println("made it here 0");
  // Serial.flush();
  //get unique pitches
  vector<uint8_t> uniquePitches;
  for(uint8_t i = 0; i<chords.size(); i++){
    // Serial.println("chord "+String(i)+"---------");
    // Serial.println("root: "+String(chords[i].root));
    // Serial.println("i:"+String(i));
    for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
      // Serial.println(String(chords[i].intervals[j]));
      // Serial.println("j:"+String(j));
      //if a pitch isn't in the vector, add it
      if(!isInVector(chords[i].intervals[j]+chords[i].root,uniquePitches)){
        uniquePitches.push_back(chords[i].intervals[j]+chords[i].root);
      }
    }
  }
  for(uint8_t i = 0; i<uniquePitches.size(); i++){
    // Serial.println(uniquePitches[i]);
  }
  // Serial.println("made it here 1");
  // Serial.println("found "+String(uniquePitches.size())+" unique notes");
  // Serial.flush();
  // delay(10);
  //for knowing which tracks are 'new'
  //so we don't add a bunch of notes to old tracks
  uint8_t trackOffset = trackData.size();
  //make a new track for each unique pitch
  for(uint8_t i = 0; i<uniquePitches.size(); i++){
    addTrack_noMove(Track(uniquePitches[i],1), false);
  }
  // Serial.println("made it here 2");
  // Serial.println("created "+String(uniquePitches.size())+" new tracks");
  // Serial.flush();
  //get the corresponding track for each note and make it
  uint16_t writeHead = 0;//records the start of each chord
  for(uint8_t i = 0; i<chords.size(); i++){
    // Serial.println("i:"+String(i));
    for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
      // Serial.println("j:"+String(j));
      uint8_t track = getTrackWithPitch_above(chords[i].intervals[j]+chords[i].root,trackOffset);
      // Serial.println("making a note on track "+String(track));
      // Serial.flush();
      // delay(10);
      Note newNote = Note(writeHead,writeHead+chords[i].length,127,100,false,false);
      makeNote(newNote,track,false);
      // activeTrack = track;
      if(writeHead>=seqEnd)
        return;
    }
    writeHead+=chords[i].length;
  }
}

//Menu object
class Menu{
  public:
  Menu();
  Menu(uint16_t,uint16_t,uint16_t,uint16_t);
  Menu(unsigned short int, unsigned short int, unsigned short int, unsigned short int, String, vector<String>, vector<unsigned short int *>, unsigned char, bool);
  Menu(unsigned short int, unsigned short int, unsigned short int, unsigned short int, String, vector<String>, unsigned char, bool);  
  void displayMenu();
  void displayMenu(bool,bool);
  void displayMenu(bool, bool, bool);
  void displayMainMenu();
  void displayFilesMenu(int16_t, bool, uint8_t, uint8_t);
  void displayFilesMenu(int16_t, bool, uint8_t, uint8_t,vector<String>);
  void displaySaveMenu();
  void displaySaveMenu(uint8_t, uint8_t);
  void displayTrackMenu();
  void displayTrackMenu_trackEdit(uint8_t);
  void displayEditMenu(uint8_t*, uint8_t);
  void displayEditMenu();
  void displaySettingsMenu(uint8_t,uint8_t,uint8_t);
  void displaySwingMenu();
  void displayEchoMenu();
  void displayRecMenu(uint8_t,uint8_t,uint8_t);
  void displayFxMenu();
  void displayHumanizeMenu();
  void displayArpMenu();
  void displayClockMenu(float);
  bool moveMenuCursor(bool);
  vector<String> options;
  vector<unsigned short int *> data;
  uint8_t listStart;
  uint8_t listEnd;
  uint8_t highlight;
  unsigned short int topL[2];
  unsigned short int bottomR[2];
  bool isActive;
  String menuTitle;
  int8_t page;
};

Menu::Menu(){
  options = {"test","test","test"};
  data = {0,0,0};
  isActive = false;
  highlight = 0;
  topL [0] = 0;
  topL [1] = 0;
  bottomR [0] = screenWidth;
  bottomR [1] = screenHeight;
  menuTitle = "test";
  page = 0;
}

Menu::Menu(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
  topL[0] = x1;
  topL[1] = y1;
  bottomR[0] = x2;
  bottomR[1] = y2;
  data = {};
  options = {};
  menuTitle = "MENU";
  page = 0;
}

Menu::Menu(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2, String title, vector<String> ops, vector<unsigned short int *> dat, unsigned char s, bool f){
  topL [0] = x1;
  topL [1] = y1;
  bottomR [0] = x2;
  bottomR [1] = y2;
  options = ops;
  data = dat;
  isActive = false;
  highlight = 0;
  menuTitle = title;
  page = 0;
}
Menu::Menu(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2, String title, vector<String> ops, unsigned char s, bool f){
  topL [0] = x1;
  topL [1] = y1;
  bottomR [0] = x2;
  bottomR [1] = y2;
  data = {};
  options = ops;
  isActive = false;
  highlight = 0;
  menuTitle = title;
  page = 0;
}

Menu activeMenu;

void Menu::displayMenu(bool bounded, bool underlined, bool separateWindow){
  if(menuTitle == "CLOCK")
    activeMenu.displayClockMenu(1);
  else if(menuTitle == "SWING")
    activeMenu.displaySwingMenu();
  else if(menuTitle == "SAVE")
    activeMenu.displaySaveMenu();
  else if(menuTitle == "TRK")
    activeMenu.displayTrackMenu();
  else if(menuTitle == "EDIT"){
    activeMenu.displayEditMenu();
  }
  else if(menuTitle == "FX"){
    activeMenu.displayFxMenu();
  }
  else{
    unsigned short int menuHeight = abs(bottomR[1]-topL[1]);
    if(separateWindow)
      display.clearDisplay();
    //drawing menu box (+16 so the title is transparent)
    display.fillRect(topL[0],topL[1]+13, bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
    display.setCursor(topL[0],topL[1]+3);

    //this is so the "MENU" sign slides out from the right
    if(menuTitle == "MENU"){
      display.setCursor(topL[0]+topL[1]-2,5);
      display.fillRect(trackDisplay,0,screenWidth-trackDisplay,debugHeight,SSD1306_BLACK);
    }
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(menuTitle);
    display.setFont();

    if(bounded)
      display.drawRect(topL[0],topL[1]+12, bottomR[0]-topL[0], bottomR[1]-topL[1]-12, SSD1306_WHITE);
    else if(underlined)
      display.drawFastHLine(topL[0],topL[1]+12,bottomR[0]-topL[0],SSD1306_WHITE);
    //drawing menu options, and the highlight
    unsigned short int textWidth = 20;
    unsigned short int textHeight = menuHeight/5;

    unsigned short int menuStart;
    unsigned short int menuEnd;
    unsigned short int yLoc = 0;

    menuStart = 0;
    menuEnd = options.size()-1;
    if(highlight == 0){
      menuStart = highlight;
      menuEnd = highlight+3;
    }
    else if(highlight>0 && highlight<options.size()-2){
      menuStart = highlight-1;
      menuEnd = highlight+2;
    }
    else if(highlight >= options.size()-2){
      menuStart = options.size()-4;
      menuEnd = options.size()-1;
    }
    //making sure it can't read options that don't exist
    if(menuEnd>=options.size())
      menuEnd = options.size()-1;
    //printing out the menu
    for(int i = menuStart; i<=menuEnd; i++){
      display.setCursor(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2);
      if(highlight != i){
        //printing the special note icon
        if(menuTitle == "TRK" && options[i] == "ptch"){
          display.drawChar(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2,0x0E,SSD1306_WHITE, SSD1306_BLACK,1);
          display.setCursor(topL[0]+3+5,(yLoc+1)*textHeight+topL[1]+2);
        }
        else{
          display.print(options[i]);
        }
      }
      else if(highlight == i){
        //printing the special note icon
        if(menuTitle == "TRK" && options[i] == "ptch"){
          display.fillRect(topL[0]+2,(yLoc+1)*textHeight+topL[1]+1,7,9,SSD1306_WHITE);
          display.drawChar(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2,0x0E,SSD1306_BLACK, SSD1306_WHITE,1);
          display.setCursor(topL[0]+3+6,(yLoc+1)*textHeight+topL[1]+2);
        }
        else{
          display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
          display.print(options[i]);
        }
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
      }
      yLoc++;
    }
  }
}
void Menu::displayMenu(bool bounded, bool underlined){
  displayMenu(bounded, underlined, false);
}
void Menu::displayMenu(){
  displayMenu(true,true,false);
}

//slides a menu in from the top,right,bottom, or left
void slideMenuIn(int fromWhere, int speed){
  if(fromWhere == 1){//sliding in from where
    int xDistance = screenWidth-activeMenu.topL[0];//how far the display is gonna need to slide over
    int width = activeMenu.bottomR[0] - activeMenu.topL[0];
    int original[4] = {activeMenu.topL[0],activeMenu.topL[1],activeMenu.bottomR[0],activeMenu.bottomR[1]};
    for(int i = 0; i+speed<= xDistance; i+=speed){
      // display.clearDisplay();
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[0] = screenWidth-i;
      activeMenu.bottomR[0] = activeMenu.topL[0] + width;
      displaySeq();
      // drawPram(5,0);
    }
    activeMenu.topL[0] = original[0];
    activeMenu.topL[1] = original[1];
    activeMenu.bottomR[0] = original[2];
    activeMenu.bottomR[1] = original[3];
  }
  //from the bottom
  else if(fromWhere == 0){
    int yDistance = screenHeight-activeMenu.topL[1];
    int height = activeMenu.bottomR[1] - activeMenu.topL[1];
    int original[4] = {activeMenu.topL[0],activeMenu.topL[1],activeMenu.bottomR[0],activeMenu.bottomR[1]};
    for(int i = 0; i+speed <= yDistance; i+=speed){
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[1] = screenHeight-i;
      activeMenu.bottomR[1] = activeMenu.topL[1] + height;
      displaySeq();
      // drawPram(5,0);
    }
    activeMenu.topL[0] = original[0];
    activeMenu.topL[1] = original[1];
    activeMenu.bottomR[0] = original[2];
    activeMenu.bottomR[1] = original[3];
  }
}
//same thang, but in reverse
void slideMenuOut(int toWhere, int speed){
  if(toWhere == 1){//sliding out to the left side
    int xDistance = screenWidth-activeMenu.topL[0];//how far the display is gonna need to slide over
    int width = activeMenu.bottomR[0] - activeMenu.topL[0];
    for(int i = 0; i< xDistance; i+=speed){
      // display.clearDisplay();
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[0] += i;
      activeMenu.bottomR[0] = activeMenu.topL[0] + width;
      displaySeq();
      drawPram(5,0);
    }
  }
  //to the bottom
  else if(toWhere == 0){
    int yDistance = screenHeight-activeMenu.topL[1];//how far the display is gonna need to slide over
    int height = activeMenu.bottomR[1] - activeMenu.topL[1];
    for(int i = 0; i< yDistance; i+=speed){
      // display.clearDisplay();
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[1] += i;
      activeMenu.bottomR[1] = activeMenu.topL[1]+height;
      displaySeq();
      drawPram(5,0);
    }
  }
}

#define DEBUG 0
#define MENU 1
#define CLOCK 2
#define SETTINGS 3
#define SEQ 4
#define LOOP 5
#define MIDI 6
#define FRAGMENT 7
#define CURVE 8
#define ARP 9
#define FX 10
#define EDIT 11
#define TRK 12
#define QZ 13
#define HUMANIZE 14
#define SAVE 15
#define FILES 16
#define REC 17

void constructMenu(uint8_t id){
  resetEncoders();
  switch(id){
    case(MENU):
    {
      vector<String> debugOptions = {"debugFill","write to serial","quantize","save","files","dataTracks","vinyl","MIDI","x/y","random","knobs","arp","echo","loop","clock","fragment","humanize","Sys","Moon","fx","keys","drumPads","rec","sequence","r_seq","r_serial","change display","HARD reset"};
      vector<unsigned short int *> debugData = {};
      Menu debugMenu(25,1,93,64,"MENU",debugOptions,debugData,0,0);
      activeMenu = debugMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    case DEBUG:
    {
      vector<String> debugOptions = {"debugFill","write to serial","quantize","save","files","dataTracks","vinyl","MIDI","x/y","random","knobs","arp","echo","loop","clock","fragment","humanize","Sys","Moon","fx","keys","drumPads","rec","sequence","r_seq","r_serial","change display","HARD reset"};
      vector<unsigned short int *> debugData = {};
      Menu debugMenu(32,4,128,64,"DEBUG",debugOptions,debugData,0,0);
      activeMenu = debugMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    //This one needs a slide in animation
    case CLOCK:
    {
      vector<String> clockOptions = {"src","bpm","swing"};
      vector<unsigned short int *> clockData = {};
      Menu clckMenu(0,0,35,64,"CLOCK",clockOptions,clockData,0,0);
      activeMenu = clckMenu;
      if(menuIsActive){
        slideMenuIn(0,20);
      }
      clockMenu();
      return;
    }
    case SETTINGS:
    {
      vector<String> settingsOptions = {""};
      vector<unsigned short int *> settingsData = {};
      Menu settingMenu(0,2,128,64,"SETTINGS",settingsOptions,settingsData,0,0);
      activeMenu = settingMenu;
      settingsMenu();
      return;
    }
    case SEQ:
    {
      vector<String> seqMenuOptions = {"Length","BPM","Erase","Clock","Loop","Output"};
      vector<unsigned short int *> seqMenuData = {&seqEnd};
      Menu seqMenu(2,4,80,60,"SEQ",seqMenuOptions,seqMenuData,0,0);
      activeMenu = seqMenu;
      return;
    }
    case LOOP:
    {
      vector<String> loopOptions = {};
      vector<unsigned short int *> loopMenuData = {};
      Menu loopMenu(2,4,80,60,"LOOP",loopOptions,loopMenuData,0,0);
      activeMenu = loopMenu;
      return;
      // cassetteAnimation();
    }
    case MIDI:
    {
      vector<String> midiOptions = {};
      vector<unsigned short int *> midiMenuData = {};
      Menu midiMenu(2,4,80,60,"MIDI",midiOptions,midiMenuData,0,0);
      activeMenu = midiMenu;
      return;
    }
    case FRAGMENT:
    {
      fragmentMenu();
      return;
    }
   case CURVE:
   {
      genCurveDebug();
      vector<String> curveOptions = {"fn","A","p","x","ph","commit"};
      vector<unsigned short int *> curveData = {&debugCurve.function, &debugCurve.amplitude, &debugCurve.period, &debugCurve.xAxis, &debugCurve.phase};
      Menu curveMenu(2,4,126,62,"CURVE",curveOptions,curveData,0,1);
      activeMenu = curveMenu;
      return;
    }
    case ARP:
    {
      vector<String> arpOptions = {"isArping","subDiv","style","octaves","notes"};
      Menu arpMenu(0,0,128,64,"ARP",arpOptions,0,0);
      activeMenu = arpMenu;
      return;
    }
    case FX:
    {
      vector<String> fxOptions = {};
      Menu efexMenu(25,1,93,64,"FX",fxOptions,0,0);
      activeMenu = efexMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    //edit menu is called by note menu
    case EDIT:
    {
      vector<String> editMenuOptions = {"V","%","L"};
      vector<short unsigned int *> editMenuData = {};
      Menu noteEditMenu(trackDisplay-5,0,screenWidth,debugHeight,"EDIT",editMenuOptions,editMenuData,0,0);
      activeMenu = noteEditMenu;
      if(menuIsActive){
        slideMenuIn(1,48);
      }
      editMenu();
      return;
    }
    case TRK:
    {
      vector<String> trackMenuOptions = {"ptch","oc","ch","latch","mute","move","solo","dupe","tune","shrnk","edit"};
      vector<unsigned short int *> trackMenuData = {};
      Menu trkMenu(94,0,129,65,"TRK",trackMenuOptions,trackMenuData,0,0);
      activeMenu = trkMenu;
      if(menuIsActive){
        slideMenuIn(1,10);
      }
      trackMenu();
      return;
    }
    case QZ:
    {
      vector<String> quantizeMenuOptions = {"trk","sel","loop","seq"};
      vector<unsigned short int *> quantizeMenuData = {};
      Menu quantizeMenu(0,0,trackDisplay,screenHeight,"QZ",quantizeMenuOptions,quantizeMenuData,0,0);
      activeMenu = quantizeMenu;
      activeMenu.page = 50;
      if(menuIsActive){
        slideMenuIn(0,20);
      }
      return;
    }
    case HUMANIZE:
    {
      vector<String> humanizeMenuOptions = {"pos","vel","chance"};
      vector<unsigned short int *> humanizeMenuData = {};
      Menu humanizeMenu(0,0,trackDisplay,screenHeight,"HUMANIZE",humanizeMenuOptions,humanizeMenuData,0,0);
      activeMenu = humanizeMenu;
      if(menuIsActive){
        slideMenuIn(0,20);
      }
      return;
    }
    case SAVE:
    {
      vector<String> saveMenuOptions = {"New"};
      vector<unsigned short int *> saveMenuData = {};
      Menu savesMenu(5,3,128,64,"SAVE",saveMenuOptions,saveMenuData,0,0);
      activeMenu = savesMenu;
      loadFiles();
      // activeMenu.options.insert(activeMenu.options.begin(),"New");
      slideMenuIn(1,30);
      saveMenu();
      return;
    }
    case FILES:
    {
      vector<String> fileMenuOptions = {"*New*"};
      vector<unsigned short int *> fileMenuData = {};
      Menu fileMenu(7,3,128,64,"FILES",fileMenuOptions,fileMenuData,0,0);
      activeMenu = fileMenu;
      // vector<String> fileSizes = loadFiles();
      loadFiles();
      slideMenuIn(1,30);
      filesMenu();
      return;
    }
   case REC:
   {
      slideMenuOut(0,20);
      vector<String> recMenuOptions = {"Wait","Overwrite","Channel","Thru"};
      vector<unsigned short int *> recMenuData = {};
      Menu rMenu(32,5,80,60,"REC",recMenuOptions,recMenuData,0,0);
      activeMenu = rMenu;
      recMenu();
      return;
   }
  }
}
//these gotta go here so the menu display can talk to them
//doing it this way is a little better i think? so all the menu dat doesn't need to be global and only loads when you need it
void constructMenu(String title){
  if(title == "MENU"){
    constructMenu(MENU);
  }
  else if(title == "DEBUG"){
    constructMenu(DEBUG);
  }
  else if(title == "CLOCK"){
    constructMenu(CLOCK);
  }
  else if(title == "SETTINGS"){
    constructMenu(SETTINGS);
  }
  else if(title == "SEQ"){
    constructMenu(SEQ);
  }
  else if(title == "LOOP"){
    constructMenu(LOOP);
  }
  else if(title == "MIDI"){
    constructMenu(MIDI);
  }
  else if(title == "FRAGMENT"){
    constructMenu(FRAGMENT);
  }
  else if(title == "CURVE"){
    constructMenu(CURVE);
  }
  else if(title == "ARP"){
    constructMenu(ARP);
  }
  else if(title == "FX"){
    constructMenu(FX);
  }
  //edit menu is called by note menu
  else if(title == "EDIT"){
    constructMenu(EDIT);
  }
  else if(title == "TRK"){
    constructMenu(TRK);
  }
  else if(title == "QZ"){
    constructMenu(QZ);
  }
  else if(title == "HUMANIZE"){
    constructMenu(HUMANIZE);
  }
  else if(title == "SAVE"){
    constructMenu(SAVE);
  }
  else if(title == "FILES"){
    constructMenu(FILES);
  }
  else if(title == "REC"){
    constructMenu(REC);
  }
}


//draws and handles a vertical selection box
unsigned short int vertSelectionBox(vector <String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRoundRect(x1,y1,width,height,4,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+3,y1+4+i*10);
      if(i == select)
        display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      display.print(options[i]);
      display.setTextColor(SSD1306_WHITE);
    }
    display.display();
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(x != 0 || y != 0){
        if(y>0 && select<options.size()-1){
          select++;
          lastTime = millis();
        }
        if(y<0 && select>0){
          select--;
          lastTime = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(sel){
        selected = true;
        lastTime = millis();
      }
    }
  }
  sel = false;
  lastTime = millis();
  return select;
}

unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    // display.setFont(&FreeSerifItalic9pt7b);
    // display.print(caption);
    // display.setFont();
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(x != 0 || y != 0){
        if(x == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(x == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(sel){
        selected = true;
        time = millis();
      }
    }
  }
  sel = false;
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}

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

#include "menus/loop.cpp"

void drawConsoleTitle(){
  const uint8_t x1 = 86;
  display.fillRect(x1-4,0,33,9,0);
  for(uint8_t i = 0; i<7; i++){
    display.drawBitmap(x1+i*4,1+sin(millis()/100+i),consoleTitle[i],4,7,1);
  }
}
//displays a scrolling list of midi ins and outs
void console(){
  vector<String> midiInMessages;
  vector<String> CCInMessages;

  //stores the last pitch received
  uint8_t lastPitchReceived;
  //stores last pitch
  uint8_t lastVelReceived;
  //stores channel the message was received on 
  uint8_t lastChannelReceived;

  uint8_t lastCCReceived;
  uint8_t lastCCValReceived;
  uint8_t lastCCChannelReceived;

  const uint8_t maxLines = 8;

  while(true){
    readButtons();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        return;
      }
    }
    
    display.clearDisplay();
    //topline
    display.drawFastHLine(0,8,114,1);
    //midline
    display.drawFastVLine(44,0,64,1);
    //bounds on graph
    drawDottedLineV(95,10,64,2);
    drawDottedLineV(127,15,64,2);
    display.drawBitmap(116,1+sin(millis()/200),mainMenu_icons[7],12,12,1);

    //note icon
    display.drawBitmap(3,1,epd_bitmap_small_note,5,5,1);
    printSmall(20,1,"V",1);
    display.drawBitmap(34,2,ch_tiny,6,3,1);

    //CC icon
    display.drawBitmap(48,2,cc_tiny,5,3,1);
    printSmall(62,1,"V",1);
    display.drawBitmap(76,2,ch_tiny,6,3,1);

    drawConsoleTitle();

    //printing midi in messages
    for(uint8_t i = 0; i<midiInMessages.size(); i++){
      printSmall(0,10+i*7,midiInMessages[i],1);
    }
    //printing CC in messages
    for(uint8_t i = 0; i<CCInMessages.size(); i++){
      printSmall(48,10+i*7,CCInMessages[i],1);
      //printing line graph
      display.drawFastVLine(95+toInt(CCInMessages[i].substring(8))/4,10+i*7,7,1);
    }
    display.display();

    //note on
    if(noteOnReceived){
      noteOnReceived = false;
      // noteOffReceived = false;
      String text = pitchToString(recentNote[0],true,true);
      String text2 = stringify(recentNote[1]);
      String text3 = stringify(recentNote[2]);
      for(int8_t i = 0; i<(3-text2.length()); i++){
        text3 = "  "+text3;
      }
      for(int8_t i = 0; i<(4-text.length()); i++){
        text2 = "  "+text2;
      }
      if(text.charAt(1) != '#'){
        text2 = " "+text2;
      }
      // if(text.charAt(2) != '-' && text.charAt(1) != '-'){
      //   text2 = "  "+text2;
      // }
      text+=" "+text2+" "+text3;
      //if there are already 8 messages in the buffer
      if(midiInMessages.size()>=maxLines){
        for(uint8_t i = 1; i<maxLines; i++){
          midiInMessages[i-1] = midiInMessages[i];
        }
        midiInMessages[maxLines-1] = text;
      }
      //add the new message
      else{
        midiInMessages.push_back(text);
      }
      lastPitchReceived = recentNote[0];
      lastVelReceived = recentNote[1];
      lastChannelReceived = recentNote[2];
    } 

    //CC
    if(recentCC[0] != lastCCReceived || recentCC[1] != lastCCValReceived || recentCC[2] != lastCCChannelReceived){
        String text = stringify(recentCC[0]);
        String text2 = stringify(recentCC[1]);
        String text3 = stringify(recentCC[2]);
        for(int8_t i = 0; i<(3-text2.length()); i++){
          text3 = "  "+text3;
        }
        for(int8_t i = 0; i<(3-text.length()); i++){
          text2 = "  "+text2;
        }
        text+=" "+text2+" "+text3;
      //if there are already 8 messages in the buffer
      if(CCInMessages.size()>=maxLines){
        for(uint8_t i = 1; i<maxLines; i++){
          CCInMessages[i-1] = CCInMessages[i];
        }
        CCInMessages[maxLines - 1] = text;
      }
      //add the new message
      else{
        CCInMessages.push_back(text);
      }
      lastCCReceived = recentCC[0];
      lastCCValReceived = recentCC[1];
      lastCCChannelReceived = recentCC[2];
    } 
  }
}

//the cassette should block out most of the screen
void cassette_recMenu(){
  while(menuIsActive && activeMenu.menuTitle == "REC"){
  int xOffset = 0;
  int yOffset = 0;
  //actual cassettes are 4:2.5:0.5
  display.drawRoundRect(xOffset+14,yOffset+0,100,62,4,SSD1306_WHITE);//outer box
  display.drawRoundRect(xOffset+19,yOffset+5,90,38,4,SSD1306_WHITE);//inner label
  display.drawRoundRect(xOffset+34,yOffset+20,60,15,3,SSD1306_WHITE);//spool box
  display.drawCircle(xOffset+41,yOffset+28,4,SSD1306_WHITE);//spool1
  display.drawCircle(xOffset+86,yOffset+28,4,SSD1306_WHITE);//spool2
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(xOffset+40,yOffset+18);
  display.print("(Loop)");
  display.display();
  }
}

void drawTrackInfo(uint8_t xCursor){
  const uint8_t sideWidth = 18;
  //track scrolling
  endTrack = startTrack + trackData.size();
  trackHeight = (screenHeight - debugHeight) / trackData.size();
  if(trackData.size()>maxTracksShown){
    endTrack = startTrack + maxTracksShown;
    trackHeight = (screenHeight-debugHeight)/maxTracksShown;
  }
  while(activeTrack>endTrack-1 && trackData.size()>maxTracksShown){
    startTrack++;
    endTrack++;
  }
  while(activeTrack<startTrack && trackData.size()>maxTracksShown){
    startTrack--;
    endTrack--;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // display.setTextWrap(false);

  //sideWidth border
  drawDottedLineV2(sideWidth,debugHeight+1,64,6);

  //top and bottom bounds
  display.drawFastHLine(0,debugHeight-1,screenWidth,SSD1306_WHITE);
  // display.fillRect(0,0,screenWidth,debugHeight,SSD1306_WHITE);

  //tracks
  for(uint8_t track = startTrack; track<endTrack; track++){
    unsigned short int y1 = (track-startTrack) * trackHeight + debugHeight-1;
    unsigned short int y2 = y1 + trackHeight;
    if(trackData[track].isSelected){
      //double digit
      if((track+1)>=10){
        display.setCursor(1-sin(millis()/100), y1+4);
        display.print("{");
        display.setCursor(13+sin(millis()/100), y1+4);
        display.print("}");
      }
      else{
        display.setCursor(3-sin(millis()/100), y1+4);
        display.print("{");
        display.setCursor(13+sin(millis()/100), y1+4);
        display.print("}");
      }
    }
    //track info display
    //single digit
    if((track+1)<10){
      // display.setCursor(8, y1+3);
      // display.print(String(track+1));
      printSmall(9,y1+5,stringify(track+1),SSD1306_WHITE);
    }
    else{
      // display.setCursor(17-String(track+1).length()*6, y1+3);
      // display.print(String(track+1));
      printSmall(17-stringify(track+1).length()*6,y1+5,stringify(track+1),SSD1306_WHITE);
    }

    //track cursor
    if(track == activeTrack){
      //track
      if(xCursor == 0)
        drawArrow(3+sin(millis()/100),y1+7,2,0,true);
      //note
      else if(xCursor == 1){
        if(getTrackPitch(track).length()>1)
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 15, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //oct
      else if(xCursor == 2){
        if(trackData[track].pitch>=120){
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, trackHeight+2, 3, SSD1306_WHITE);
        }
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //channel
      else if(xCursor == 3){
        if(trackData[track].channel>=10)
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //rec
      else if(xCursor == 4){
        display.drawCircle(sideWidth+(xCursor-1)*10+10, y1+7, 5, SSD1306_WHITE);
      }
      //mute group
      else if(xCursor == 6){
        if(trackData[track].muteGroup == 0)
          display.drawRoundRect(sideWidth+56, y1+2, 19, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+56, y1+2, 5+4*stringify(trackData[track].muteGroup).length(), trackHeight+2, 3, SSD1306_WHITE);
      }
      else if(xCursor != 5)
        display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+1, 10, trackHeight+2, 3, SSD1306_WHITE);
    }

    //all the track info...
    //pitch, oct, and channel
    printSmall(sideWidth+6,  y1+5, getTrackPitch(track), SSD1306_WHITE);//pitch

    if(getOctave(trackData[track].pitch)>=10)
      printSmall(sideWidth+16, y1+5, stringify(getOctave(trackData[track].pitch)), SSD1306_WHITE);//octave
    else
      printSmall(sideWidth+20, y1+5, stringify(getOctave(trackData[track].pitch)), SSD1306_WHITE);//octave

    if(trackData[track].channel>=10)
        printSmall(sideWidth+26, y1+5, stringify(trackData[track].channel), SSD1306_WHITE);//channel
    else
      printSmall(sideWidth+30, y1+5, stringify(trackData[track].channel), SSD1306_WHITE);//channel
    
    //primed rec symbol
    if(trackData[track].isPrimed){
      if(millis()%1000>500){
        display.drawCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
      else{
        display.fillCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
    }

    //latch
    drawCheckbox(sideWidth+48,y1+4,trackData[track].isLatched,xCursor == 5 && activeTrack == track);

    //mute group
    if(trackData[track].muteGroup == 0){
      printSmall(sideWidth+58, y1+5, "none",SSD1306_WHITE);
    }
    else{
      printSmall(sideWidth+59, y1+5, stringify(trackData[track].muteGroup),SSD1306_WHITE);
    }
  }
}

void trackEditMenu(){
  //deselecting all the tracks
  for(uint8_t track = 0; track<trackData.size(); track++){
    trackData[track].isSelected = false;
  }
  //for which param to edit
  uint8_t xCursor = 0;
  //params are: track select, note, oct, channel, prime, latch, mute group
  //for which track to edit
  uint8_t yCursor = 0;
  String text1;
  String text2;
  vector<uint8_t> selectedTracks;
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(150)){
      //moving thru tracks
      if(y != 0){
        if (y == 1) {
          setActiveTrack(activeTrack + 1, true);
          lastTime = millis();
        }
        if (y == -1) {
          setActiveTrack(activeTrack - 1, true);
          lastTime = millis();
        }
      }
    }
    if(itsbeen(200)){
      //moving thru params
      if(x != 0){
        //changing params
        if(shift){
          if(x == 1){
            switch(xCursor){
              //note
              case 1:
                if(trackData[activeTrack].pitch>0){
                  trackData[activeTrack].pitch--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>0)
                      trackData[track].pitch--;
                  }
                }
                break;
              //octave
              case 2:
                if(trackData[activeTrack].pitch>11){
                  trackData[activeTrack].pitch-=12;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>11)
                      trackData[track].pitch-=12;
                  }
                }
                break;
              //channel
              case 3:
                if(trackData[activeTrack].channel>1){
                  trackData[activeTrack].channel--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].channel>0)
                      trackData[track].channel--;
                  }
                }
                break;
              //mute group
              case 6:
                if(trackData[activeTrack].muteGroup>0){
                  trackData[activeTrack].muteGroup--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup>0)
                      trackData[track].muteGroup--;
                  }
                }
                break;
            }
          }
          else if(x == -1){
            switch(xCursor){
              //note
              case 1:
                if(trackData[activeTrack].pitch<127){
                  trackData[activeTrack].pitch++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<127)
                      trackData[track].pitch++;
                  }
                }
                break;
              //octave
              case 2:
                if(trackData[activeTrack].pitch<=115){
                  trackData[activeTrack].pitch+=12;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<=115)
                      trackData[track].pitch+=12;
                  }
                }
                break;
              //channel
              case 3:
                if(trackData[activeTrack].channel<16){
                  trackData[activeTrack].channel++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].channel<16)
                      trackData[track].channel++;
                  }
                }
                break;
              //mute group
              case 6:
                if(trackData[activeTrack].muteGroup<127){
                  trackData[activeTrack].muteGroup++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup<127)
                      trackData[track].muteGroup++;
                  }
                }
                break;
            }
          }
        }
        else{
          if(x == 1 && xCursor > 0){
            xCursor--;
            lastTime = millis();
          }
          else if(x == -1 && xCursor<6){
            xCursor++;
            lastTime = millis();
          }
        }
      }
      if(n){
        addTrack(trackData[activeTrack].pitch);
        setActiveTrack(trackData.size()-1,false);
        lastTime = millis();
      }
      while(counterA != 0){
        if(counterA >= 1){
          switch(xCursor){
            //note
            case 1:
              if(trackData[activeTrack].pitch<127){
                trackData[activeTrack].pitch++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<127)
                    trackData[track].pitch++;
                }
              }
              break;
            //octave
            case 2:
              if(trackData[activeTrack].pitch<=115){
                trackData[activeTrack].pitch+=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<=115)
                    trackData[track].pitch+=12;
                }
              }
              break;
            //channel
            case 3:
              if(trackData[activeTrack].channel<16){
                trackData[activeTrack].channel++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].channel<16)
                    trackData[track].channel++;
                }
              }
              break;
            //mute group
            case 6:
              if(trackData[activeTrack].muteGroup<127){
                trackData[activeTrack].muteGroup++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup<127)
                    trackData[track].muteGroup++;
                }
              }
              break;
          }
        }
        else{
          switch(xCursor){
            //note
            case 1:
              if(trackData[activeTrack].pitch>0){
                trackData[activeTrack].pitch--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>0)
                    trackData[track].pitch--;
                }
              }
              break;
            //octave
            case 2:
              if(trackData[activeTrack].pitch>11){
                trackData[activeTrack].pitch-=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>11)
                    trackData[track].pitch-=12;
                }
              }
              break;
            //channel
            case 3:
              if(trackData[activeTrack].channel>1){
                trackData[activeTrack].channel--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].channel>0)
                    trackData[track].channel--;
                }
              }
              break;
            //mute group
            case 6:
              if(trackData[activeTrack].muteGroup>0){
                trackData[activeTrack].muteGroup--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup>0)
                    trackData[track].muteGroup--;
                }
              }
              break;
          }
        }
        counterA += counterA<0?1:-1;
      }
      if(sel){
        switch(xCursor){
          //if it's on track, select
          case 0:
            //toggles selection on all
            if(shift){
              trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isSelected = trackData[activeTrack].isSelected;
              }
            }
            //normal selection toggle
            else
              trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
            
            lastTime = millis();
            break;
          //if it's on primed
          case 4:
            //toggles prime on all
            if(shift){
              trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isPrimed = trackData[activeTrack].isPrimed;
              }
            }
            //normal selection toggle
            else{
              trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
              for(uint8_t track = 0; track<trackData.size(); track++){
                if(track != activeTrack && trackData[track].isSelected)
                  trackData[track].isPrimed = trackData[activeTrack].isPrimed;
              }
            }
            lastTime = millis();
            break;
          //if it's on latch
          case 5:
            //toggles prime on all
            if(shift){
              trackData[activeTrack].isLatched = !trackData[activeTrack].isLatched;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isLatched = trackData[activeTrack].isLatched;
              }
            }
            //normal selection toggle
            else{
              trackData[activeTrack].isLatched = !trackData[activeTrack].isLatched;
              for(uint8_t track = 0; track<trackData.size(); track++){
                if(track != activeTrack && trackData[track].isSelected)
                  trackData[track].isLatched = trackData[activeTrack].isLatched;
              }
            }
            lastTime = millis();
            break;
        }
      }
      if(menu_Press || track_Press){
        // menuIsActive = false;
        // constructMenu("MENU");
        for(uint8_t track = 0; track<trackData.size(); track++){
          trackData[track].isSelected = false;
        }
        lastTime = millis();
        menu_Press = false;
        track_Press = false;
        return;
      }
    }
    display.clearDisplay();
    drawTrackInfo(xCursor);
    activeMenu.displayTrackMenu_trackEdit(xCursor);
    display.display();
  }
}

//cassette-vibe buttons for the playBack menu
void drawCassetteButton(uint8_t x1, uint8_t y1, uint8_t which, bool state){
  // const uint8_t width = 20;
  const uint8_t width = 12;
  const uint8_t height = 12;

  drawBox(x1, y1, width, height, 2, -2, state?2:1);
  if(state)
    display.drawFastHLine(x1,y1+height+1,width,1);
  switch(which){
    //play
    case 0:
      // display.drawTriangle(x1+8,y1+height/2+2,x1+1,y1+5,x1+1,y1+11,state?0:1);
      if(state)
        display.drawTriangle(x1+8,y1+height/2+2,x1+1,y1+5,x1+1,y1+11,0);
      else
        display.fillTriangle(x1+6,y1+height/2+2,x1+2,y1+6,x1+2,y1+10,1);
      break;
    //rec
    case 1:
      if(state)
        display.drawCircle(x1+width/2-2,y1+height/2+2,3,0);
      else
        display.fillCircle(x1+width/2-2,y1+height/2+2,2,1);
      break;
    //stop
    case 2:
      if(state)
        display.drawRect(x1+1,y1+5,7,7,0);
      else
        display.fillRect(x1+2,y1+6,5,5,1);
      break;
  }
}

void drawPlaybackIcon(uint8_t x1, uint8_t y1, uint8_t which){
  uint8_t reps = (millis()/100)%16;
  if(reps>5){
    reps = 5;
  }
  const int8_t xGap = -6;
  const int8_t yGap = 0;

  const uint8_t width = 12;
  const uint8_t height = 12;
  switch(which){
    //play
    case 0:
      for(uint8_t i = 0; i<reps; i++){
        y1+=sin(PI*i);
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillTriangle(x1+xOffset,y1-yOffset,x1+width+xOffset,y1+height/2-yOffset,x1+xOffset,y1+height-yOffset,0);
        display.drawTriangle(x1+xOffset,y1-yOffset,x1+width+xOffset,y1+height/2-yOffset,x1+xOffset,y1+height-yOffset,1);
      }
      break;
    //rec
    case 1:
      // x1+=width/2;
      y1+=height;
      for(uint8_t i = 0; i<reps; i++){
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillCircle(x1+xOffset+width/2,y1-yOffset-height/2,width/2,0);
        display.drawCircle(x1+xOffset+width/2,y1-yOffset-height/2,width/2,1);
      }
      break;
    //stop
    case 2:
      for(uint8_t i = 0; i<reps; i++){
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillRect(x1+xOffset,y1-yOffset,width,height,0);
        display.drawRect(x1+xOffset,y1-yOffset,width,height,1);
      }
      break;
  }
}

void printPlaybackOptions(uint8_t which, uint8_t cursor, bool active){
  drawPlaybackIcon(40,0,which-1);
  const uint8_t x1 = 20;
  uint8_t y1 = 14;
  switch(which){
    //play options
    case 1:
      printSmall(x1,y1+1,"loop:",1);
      //loop on/off
      if(isLooping){
        printSmall(x1+24,y1+1,"on",1);
      }
      else{
        printSmall(x1+24,y1+1,"off",1);
      }
      printSmall(x1,y1+11,"clock source:",1);
      //external clock
      if(externalClock){
        printSmall(x1+56,y1+11,"external",1);
      }
      else{
        printSmall(x1+56,y1+11,"internal",1);
      }
      //highlight
      if(active){
        switch(cursor){
          case 0:
            display.fillRoundRect(x1+22,y1-1,isLooping?11:15,9,3,2);
            drawArrow(x1+37+sin(millis()/100),y1+3,3,1,false);
            break;
          case 1:
            display.fillRoundRect(x1+54,y1+9,35,9,3,2);
            drawArrow(x1+91+sin(millis()/100),y1+13,3,1,false);
            break;
        }
      }
      break;
    //rec options
    case 2:
      //prime tracks
      //make new tracks
      //count-in (or wait for a note) "begin after"
      //rec until the end of current loop, rec over the current loop only, rec continuously through the loops, or rec for an arbitrary number of steps
      break;
    //stop options
    case 3:
      //menu cursor
      if(active){
        switch(cursor){
          case 0:
            drawArrow(x1+104+sin(millis()/100),y1+10,3,1,true);
            break;
          case 1:
            drawArrow(x1+104+sin(millis()/100),y1+32,3,1,true);
            break;
        }
      }
      //continue, restart, or stop
      display.drawRoundRect(x1,y1+3,105,15,5,1);
      display.fillRect(x1,y1,59,6,0);
      printSmall(x1,y1,"when stopped...",1);
      switch(onStop){
        case 0:
          printSmall(x1+5,y1+8,"reset playhead to loop",1);
          break;
        case 1:
          printSmall(x1+5,y1+8,"reset playhead to 1st loop",1);
          break;
        case 2:
          printSmall(x1+5,y1+8,"leave playhead in place",1);
          break;
      }
      y1+=22;
      display.drawRoundRect(x1,y1+3,105,15,5,1);
      display.fillRect(x1,y1,77,6,0);
      printSmall(x1,y1,"when rec finishes...",1);
      switch(postRec){
        case 0:
          printSmall(x1+5,y1+8,"stop.",1);
          break;
        case 1:
          printSmall(x1+5,y1+8,"continue.",1);
          break;
        case 2:
          printSmall(x1+5,y1+8,"restart current loop",1);
          break;
      }
      break;
  }
}

void playBackMenu(){
  uint8_t buttonCursor = 0;
  uint8_t menuCursor = 0;
  //0 is switching between menus, 1 is play, 2 is rec, 3 is stop
  uint8_t menuState = 0;
  while(true){
    readButtons();
    joyRead();
    while(counterA != 0){
      switch(menuState){
        //play
        case 1:
          switch(menuCursor){
            case 0:
              isLooping = !isLooping;
              break;
            case 1:
              externalClock = !externalClock;
              break;
          }
          break;
        //stop
        case 3:
          switch(menuCursor){
            case 0:
              if(counterA >= 1 && onStop<2){
                onStop++;
              }
              else if(counterA <= -1 && onStop>0){
                onStop--;
              }
              break;
            case 1:
              if(counterA >= 1 && postRec<2){
                postRec++;
              }
              else if(counterA <= -1 && postRec>0){
                postRec--;
              }
              break;
          }
          break;
      }
      counterA+=counterA<0?1:-1;
    }
    if(itsbeen(200)){
      if(x != 0){
        switch(menuState){
          //play
          case 1:
            switch(menuCursor){
              case 0:
                isLooping = !isLooping;
                lastTime = millis();
                break;
              case 1:
                externalClock = !externalClock;
                lastTime = millis();
                break;
            }
            break;
          //stop
          case 3:
            switch(menuCursor){
              case 0:
                if(x == -1 && onStop<2){
                  onStop++;
                  lastTime = millis();
                }
                else if(x == 1 && onStop>0){
                  onStop--;
                  lastTime = millis();
                }
                break;
              case 1:
                if(x == -1 && postRec<2){
                  postRec++;
                  lastTime = millis();
                }
                else if(x == 1 && postRec>0){
                  postRec--;
                  lastTime = millis();
                }
                break;
            }
            break;
        }
      }
      if(y != 0){
        switch(menuState){
          //switching menus
          case 0:
            if(y == 1){
              if(buttonCursor<2){
                buttonCursor++;
              }
              lastTime = millis();
            }
            if(y == -1){
              if(buttonCursor>0){
                buttonCursor--;
              }
              lastTime = millis();
            }  
            break; 
          //play menu (same as stop menu)
          case 1:
          //rec menu (same as stop menu)
          case 2:
          //stop menu
          case 3:
            if(y == 1 && menuCursor<1){
              menuCursor++;
              lastTime = millis();
            }
            if(y == -1 && menuCursor>0){
              menuCursor--;
              lastTime = millis();
            }
            break;  
        }
      }
      if(menu_Press){
        lastTime = millis();
        //go back to menu-select
        if(menuState){
          menuState = 0;
          menuCursor = 0;
        }
        //or break out of menu
        else{
          menu_Press = false;
          return;
        }
      }
      if(sel){
        lastTime = millis();
        switch(menuState){
          //selecting a menu
          case 0:
            menuState = buttonCursor+1;
            menuCursor = 0;
            break;
        }
      }
    }
    display.clearDisplay();

    //drawing menu options
    if(!menuState){
      printPlaybackOptions(buttonCursor+1,menuCursor,false);
    }
    else{
      printPlaybackOptions(menuState,menuCursor,true);
    }

    String text;
    switch(buttonCursor){
      case 0:
        text = "ply";
        break;
      case 1:
        text = "rec";
        break;
      case 2:
        text = "stp";
        break;
    }
    printArp_wiggly(20,3,text,1);
    const uint8_t gap = 18;
    const uint8_t x1 = 3;
    const uint8_t y1 = 3;
    // drawBox(x1,y1,90,20,5,-5,1);
    display.drawFastVLine(x1+3,0,64,1);
    for(uint8_t i = 0; i<3; i++){
      drawCassetteButton(x1,y1+i*gap,i,buttonCursor==i);
      //if you're in menu-select mode
      if(buttonCursor == i && !menuState)
        drawArrow(x1+15+sin(millis()/100),y1+i*gap+6,3,1,false);
    }
    display.display();
  }
}

void recMenu(){
  //main window controls rec mode, settings
  /*
  overwrite
  count-in (how many times to play thru seq before recording): waiting, none
  rec for: infinite (until the button is pressed), number of steps
  loop: current loop only, record through loops in sequence, ignore loop points
  after rec: stop, play from end position, play from first loop
  */
  bool whichWindow = false;
  uint8_t cursor = 0;
  int16_t currentVel;
  int16_t lastVel;
  float VUval;
  uint8_t yCursor = 0;
  uint8_t activeT = 0;
  // WireFrame cassette = makeCassette();
  // cassette.scale = 5;
  // cassette.rotate(90,2);
  // cassette.xPos = 120;
  while(menuIsActive && activeMenu.menuTitle == "REC"){
    joyRead();
    readButtons();
    if(itsbeen(100)){
      if(y != 0){
        //if you're in priming mode
        if(whichWindow == 1){
          if(y == -1 && activeT>0){
            activeT--;
            if(activeT<yCursor)
              yCursor--;
            lastTime = millis();
          }
          else if(y == 1 && activeT<trackData.size()-1){
            activeT++;
            if(activeT>=yCursor+8)
              yCursor++;
            lastTime = millis();
          }
        }
        else{
          if(y == 1 && cursor<6){
            cursor++;
            lastTime = millis();
          }
          else if(y == -1 && cursor>0){
            cursor--;
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      if(x != 0){
        if(whichWindow){
          whichWindow = false;
          lastTime = millis();
        }
        else{
          switch(cursor){
            //prime
            case 0:
              whichWindow = true;
              lastTime = millis();
              break;
            //overwrite
            case 1:
              overwriteRecording = !overwriteRecording;
              lastTime = millis();
              break;
            //new tracks
            case 2:
              makeNewTracks = !makeNewTracks;
              lastTime = millis();
              break;
            //count-in
            case 3:
              if(x == 1){
                if(shift){
                  if(recCountIn == 0)
                    waitForNote = true;
                  else{
                    recCountIn--;
                  }
                  lastTime = millis();
                }
                else if(recCountIn>=24){
                  recCountIn-=24;
                  lastTime = millis();
                }
                else{
                  if(recCountIn == 0)
                    waitForNote = true;
                  else
                    recCountIn = 0;
                  lastTime = 0;
                }
              }
              else if(x == -1){
                if(waitForNote){
                  waitForNote = false;
                  recCountIn = 0;
                  lastTime = millis();
                }
                else{
                  if(shift && recCountIn<65535){
                    recCountIn++;
                    lastTime = millis();
                  }
                  else if(recCountIn<=65535-24){
                    recCountIn+=24;
                    lastTime = millis();
                  }
                  else{
                    recCountIn = 65535;
                    lastTime = millis();
                  }
                }
              }
              break;
            //rec for
            case 4:
              if(x == 1){
                if(shift && recForNSteps>0){
                  recForNSteps--;
                  lastTime = millis();
                }
                else if(recForNSteps>=24){
                  recForNSteps-=24;
                  lastTime = millis();
                }
              }
              else if(x == -1){
                if(shift && recForNSteps<65535){
                  recForNSteps++;
                  lastTime = millis();
                }
                else if(recForNSteps<=65535-24){
                  recForNSteps+=24;
                  lastTime = millis();
                }
              }
              break;
            //loop behavior
            case 5:
              if(x == 1){
                if(recLoopBehavior == 2)
                  recLoopBehavior = 0;
                else
                  recLoopBehavior++;
                lastTime = millis();
              }
              else if(x == -1){
                if(recLoopBehavior == 0)
                  recLoopBehavior = 2;
                else
                  recLoopBehavior--;
                lastTime = millis();
              }
              break;
            //post-rec behavior
            case 6:
              if(x == 1){
                if(postRec == 2)
                  postRec = 0;
                else
                  postRec++;
                lastTime = millis();
              }
              else if(x == -1){
                if(postRec == 0)
                  postRec = 2;
                else
                  postRec--;
                lastTime = millis();
              }
              break;
          }
        }
      }
      if(sel){
        if(whichWindow){
          trackData[activeT].isPrimed = !trackData[activeT].isPrimed;
          if(shift){
            for(int i = 0; i<trackData.size(); i++){
              trackData[i].isPrimed = trackData[activeT].isPrimed;
            }
          }
          lastTime = millis();
        }
        else{
          switch(cursor){
            case 0:
              whichWindow = true;
              break;
          }
        }
      }
      if(menu_Press){
        lastTime = millis();
        break;
      }
    }
    //get current VU destination value
    if(playlist.size()>0){
      currentVel = recentNote[1];
    }
    else{
      currentVel = 0;
    }
    //set VU to the lastVel variable
    VUval = float(lastVel)/float(127);
    //update lastVel, incrementing it towards the currentVel
    lastVel = lastVel + (currentVel-lastVel)/5;
    if((whichWindow || cursor == 0) && activeMenu.page<28){
      activeMenu.page+=8;
      if(activeMenu.page>28)
        activeMenu.page = 28;
    }
    else if((!whichWindow && cursor != 0) && activeMenu.page>0){
      activeMenu.page-=8;
    }
    display.clearDisplay();
    drawVU(0,50,1-VUval);
    // cassette.render();
    activeMenu.displayRecMenu(cursor,yCursor,activeT);
    display.display();
    // cassette.rotate(4,1);
  }
  constructMenu("MENU");
}

void Menu::displayRecMenu(uint8_t menuCursor,uint8_t start, uint8_t active){
  //title
  display.setRotation(1);
  display.setFont(&FreeSerifItalic12pt7b);
  display.setCursor(16,16);
  display.print("Rec");
  display.setFont();
  display.setRotation(UPRIGHT);
  if(millis()%1000>500){
    display.fillCircle(12,5,5,SSD1306_WHITE);
  }
  
  // display.setRotation(3);
  // display.setFont(&FreeSerifItalic12pt7b);
  // display.setCursor(0,14);
  // display.print("menu");
  // display.setFont();
  // display.setRotation(UPRIGHT);
  // if(millis()%1000>500){
  //   display.fillCircle(118,57,5,SSD1306_WHITE);
  // }

  uint8_t x1 = screenWidth-activeMenu.page;

  //options---------
  //cursor highlight
  int8_t bitmap1 = -1;
  int8_t bitmap2 = -1;
  switch(menuCursor){
    //prime
    case 0:
      display.fillRoundRect(20,0,62,9,3,SSD1306_WHITE);
      break;
    //overwrite
    case 1:
      display.fillRoundRect(20,9,40,9,3,SSD1306_WHITE);
      display.drawBitmap(96,0,recmenu_overwrite_paper,24,21,1);
      display.drawBitmap(96+(millis()/200)%16,16-(millis()/400)%16,recmenu_overwrite_pencil,12,12,1);
      break;
    //make new tracks
    case 2:
      display.fillRoundRect(20,19,42,9,3,SSD1306_WHITE);
      display.drawBitmap(96,10,recmenu_new_tracks_tracks,29,19,1);
      display.drawBitmap(93,3+sin(millis()/100),recmenu_new_tracks_plus,13,13,1);
      break;
    //count in
    case 3:
      display.fillRoundRect(20,28,36,9,3,SSD1306_WHITE);
      break;
    //rec-for
    case 4:
      display.fillRoundRect(20,37,30,9,3,SSD1306_WHITE);
      display.drawBitmap(96,0,recmenu_rec_for_cassette,20,27,1);
      display.drawBitmap(89+sin(millis()/100),0,recmenu_rec_for_arrow,6,5,1);
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(36+sin(millis()/100),37,recmenu_rec_for_arrow,6,5,1);
      display.setRotation(UPRIGHT);
      break;
    //loop
    case 5:
      display.fillRoundRect(20,46,20,9,3,SSD1306_WHITE);
      break;
    //after
    case 6:
      display.fillRoundRect(20,55,24,9,3,SSD1306_WHITE);
      display.drawBitmap(96,16,recmenu_stop,13,14,1);
      break;
  }
  //text
  printSmall(22,2,"prime tracks -->",2);
  printSmall(22,11,"overwrite: "+ (overwriteRecording?stringify("yes"):stringify("no")),2);
  printSmall(22,21,"new tracks: "+ (makeNewTracks?stringify("yes"):stringify("no")),2);
  printSmall(22,30,"count-in: "+ (waitForNote?stringify("listen 4 note"):stepsToMeasures(recCountIn)),2);
  printSmall(22,39,"rec for: "+ (recForNSteps == 0?stringify("ever"):stepsToMeasures(recForNSteps)),2);
  switch(recLoopBehavior){
    case 0:
      printSmall(22,48,"loop: current only",2);
      break;
    case 1:
      printSmall(22,48,"loop: sequence",2);
      break;
    case 2:
      printSmall(22,48,"loop: ignore",2);
      break;
  }
  switch(postRec){
    case 0:
      printSmall(22,57,"after: stop",2);
      break;
    case 1:
      printSmall(22,57,"after: continue",2);
      break;
    case 2:
      printSmall(22,57,"after: restart",2);
      break;
  }
  //if the prime menu is 'slid' onscreen
  if(activeMenu.page>0){
    if(start>0){
      drawArrow(x1-8,2+sin(millis()/200),2,2,true);
    }
    if(start+8<trackData.size()){
      drawArrow(x1-8,62-sin(millis()/200),2,3,true);
    }
    display.setRotation(1);
    printSmall(38,x1-10,"prime",SSD1306_WHITE);
    display.setRotation(UPRIGHT);
    //border
    display.fillRect(x1,0,screenWidth-x1,screenHeight,SSD1306_BLACK);
    display.drawFastVLine(x1,0,screenHeight,SSD1306_WHITE);
    //drawing each track
    for(uint8_t i = 0; i<8; i++){
      if(i+start<trackData.size()){
        uint8_t x2 = x1+3;
        if(i+start == active){
          x2-=2+sin(millis()/100);
          drawArrow(x1-2,i*8+3,2,0,false);
        }
        //print track pitch
        printSmall(x2+10,i*8+1,getTrackPitchOctave(i+start),SSD1306_WHITE);
        //prime icon
        if(trackData[i+start].isPrimed){
          if((millis()+i*80)%1000>500){
            display.fillCircle(x2+5,i*8+3,3,SSD1306_WHITE);
          }
          // else{
          //   display.drawCircle(x2+5,i*8+3,3,SSD1306_WHITE);
          // }
        }
        else{
          display.drawCircle(x2+5,i*8+3,3,SSD1306_WHITE);
        }
      }
    }
  }
}

void muteGroups(int callingTrack, int group){
  for(int track = 0; track<trackData.size(); track++){
    if(track != callingTrack && trackData[track].muteGroup == group && trackData[track].noteLastSent != 255){
      sendMIDInoteOff(trackData[track].noteLastSent,0,trackData[track].channel);
      subNoteFromPlaylist(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
    }
  }
}

void makeMuteGroup(){
  // vector<unsigned short int> trackIDs = selectMultipleTracks("Tracks in the group...");
}

//fragmenting works on top of the loops, by literally moving the playhead, so it doesn't mess with loop data at all
//data is stored as
//(current size, min size, max size) -- how long the seq plays before re-fragmenting
//(current iterations, min iterations, max iterations) -- how many times the fragment repeats 
//(start position, min start, max start) -- where the fragment sets the cursor to
//default is it starts within the loop
vector<vector<unsigned short int>> fragmentData = {{0,1,4},{0,0,3},{0,0,96}};

uint8_t fragmentIterations;
unsigned short int fragmentStep;
//default is a half note
unsigned short int fragmentSubDiv = 12;

void genFragment(){
  //new fragment length
  fragmentData[0][0] = random(fragmentData[0][1],fragmentData[0][2]+1);
  //new fragment iterations
  fragmentData[1][0] = random(fragmentData[1][1],fragmentData[1][2]+1);
  //fragment start position (always a clean multiple of the subDiv)
  fragmentData[2][0] = fragmentSubDiv * random((fragmentData[2][1],fragmentData[2][2]+1)/fragmentSubDiv);
  fragmentStep = 0;

  //set the active playhead to the start of the fragment
  if(playing)
    playheadPos = fragmentData[2][0];
  else if(recording)
    recheadPos = fragmentData[2][0];
  fragmentStep = 0;
}

void checkFragment(){
  if(isFragmenting){
    fragmentStep++;
    //if the seq is past the end of the fragment
    if(fragmentStep>=fragmentData[0][0]*fragmentSubDiv){
      //if that was the final iteration, generate a new fragment
      if(fragmentData[1][0] == 0){
        genFragment();
      }
      else{
        //Decrease iterations
        fragmentData[1][0]--;
        //resetting fragment step
        fragmentStep = 0;
      }
    }
  }
}
uint16_t getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    count+=seqData[track].size()-1;
  }
  return count;
}

float getNoteDensity(int timestep){
  float density = 0;
  for(int track = 0; track<trackData.size(); track++){
    if(lookupData[track][timestep] != 0){
      density++;
    }
  }
  return density/float(trackData.size());
}
float getNoteDensity(int start, int end){
  float density;
  for(int i = start; i<= end; i++){
    density+=getNoteDensity(i);
  }
  return density/float(end-start+1);
}
void fragmentAnimation(bool in){
  if(in){
    int width = 20;
    int height = 20;
    int xDepth = 10;
    int yDepth = 10;
    int maxWidth = 100;
    int maxHeight = 100;
    int h = 64;
    int k = 40;
    while(maxWidth>1){
      if(millis()%1000>10){
        height = random(maxHeight-10,maxHeight);
        width = random(maxWidth-10,maxWidth);
      }
      display.clearDisplay();
      drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
      maxWidth-=15;
      maxHeight-=15;
      k-=3;
      display.display();
    }
  }
  else if(!in){
    int width = 20;
    int height = 20;
    int xDepth = 10;
    int yDepth = 10;
    int maxWidth = 0;
    int maxHeight = 0;
    int h = 64;
    int k = 20;
    while(maxWidth<100){
      if(millis()%1000>10){
        height = random(maxHeight-10,maxHeight);
        width = random(maxWidth-10,maxWidth);
      }
      display.clearDisplay();
      drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
      maxWidth+=15;
      maxHeight+=15;
      k+=3;
      display.display();
    }
  }
}

void shadeRect(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned char height, unsigned char shade){
  display.drawRect(x1, y1, len, height, SSD1306_WHITE);
  shadeArea(x1,y1,len,height,shade);
}
void drawCenteredBracket(int x1, int y1, int length, int height){
  drawNoteBracket(x1-length/2,y1-height/2,length, height, false);
}

void fragmentMenu(){
  //controls that we need
  /*
  - on/off
  - min/max start
  - min/max repeats
  - min/max length
  */
  fragmentAnimation(true);
  int xStart = 54;//line coord
  int timeS = playheadPos;
  int h;
  int k;
  int xDepth = random(-10,10);
  int yDepth = random(-10,10);
  int minReps = fragmentData[1][1];
  int maxReps = fragmentData[1][2];
  int minWidth = fragmentData[0][1];
  int maxWidth = fragmentData[0][2];
  int height;
  int width;

  //arbitrary max length val
  int maxLengthVal = 384;
  int maxRepsVal = 8;
  //for drawing the start bounds
  bool startedDrawing = false;
  while(menuIsActive){
    h = 80;
    k = 30;
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        menuIsActive = false;
        constructMenu("MENU");
        lastTime = millis();
      }
      if(step_buttons[3]){
        step_buttons[3] = 0;
        constructMenu("MENU");
        menuIsActive = false;
        lastTime = millis();
      }
    }
    //controlling where the line starts
    if(itsbeen(100) && x != 0){
      lastTime = millis();
      if(x == 1 && timeS != -screenWidth/2){//last part is so it doesn't 'bounce'
        if(shift){
          timeS--;
        }
        else{
          if((timeS+64)%fragmentSubDiv){
            timeS-=(timeS+64)%fragmentSubDiv;
          }
          else{
            timeS-=fragmentSubDiv;
          }
        }
      }
      if(x == -1){
        if(shift){
          timeS++;
        }
        else{
          if((timeS+64)%fragmentSubDiv){
            timeS+=fragmentSubDiv-(timeS+64)%fragmentSubDiv;
          }
          else{
            timeS+=fragmentSubDiv;
          }
        }
      }
      //bounds checking
      if(timeS<seqStart-screenWidth/2){
        timeS = seqStart-screenWidth/2;
      }
      if(timeS+64>seqEnd){
        timeS = seqEnd-64;
      }
    }
    if(itsbeen(200)){
      //scrolling thru the menu, when shift isn't held
      if(y != 0 && !shift){
        if(y == 1){
          activeMenu.moveMenuCursor(true);
          lastTime = millis();
        }
        if(y == -1){
          activeMenu.moveMenuCursor(false);
          lastTime = millis();
        }
      }
      if(sel){
        if(!shift){
          fragmentData[2][1] = timeS+64;
          lastTime = millis();
        }
        else if(shift){
          fragmentData[2][2] = timeS+64;
          lastTime = millis();
        }
        //if start is bigger than end, swap em
        if(fragmentData[2][1]>fragmentData[2][2]){
          int max = fragmentData[2][1];
          int min = fragmentData[2][2];
          fragmentData[2][1] = min;
          fragmentData[2][2] = max;
        }
      }
      if(play){
        togglePlayMode();
        lastTime = millis();
      }
    }
    //encoder A changes minimums and maximums (while shifting)
    while(counterA != 0){
      //changing minimums
      if(!shift){
        if(counterA >= 1){
          //changing min length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][1]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv)
              fragmentData[0][1]++;
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][1]<maxRepsVal)
              fragmentData[1][1]++;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(fragmentData[2][1]<seqEnd-subDivInt){
              fragmentData[2][1]+=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        else if(counterA <= -1){
          //changing min length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][1]*fragmentSubDiv>=fragmentSubDiv){
              fragmentData[0][1]--;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][1]>0)
              fragmentData[1][1]--;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(!shift && fragmentData[2][1]>subDivInt){
              fragmentData[2][1]-=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        for(int i = 0; i<3; i++){
          if(fragmentData[i][1]>fragmentData[i][2]){
            fragmentData[i][2] = fragmentData[i][1];
          }
        }
      }
      //changing maximums
      else if(shift){
        if(counterA >= 1){
          //changing max length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][2]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv){
              fragmentData[0][2]++;
            }
            else if(!shift && fragmentData[0][2]<maxLengthVal-subDivInt){
              fragmentData[0][2]+=subDivInt;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][2]<maxRepsVal)
              fragmentData[1][2]++;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(shift && fragmentData[2][2]<seqEnd){
              fragmentData[2][2]++;
            }
            else if(!shift && fragmentData[2][2]<seqEnd-subDivInt){
              fragmentData[2][2]+=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        else if(counterA <= -1){
          //changing max length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][2]*fragmentSubDiv>=fragmentSubDiv){
              fragmentData[0][2]--;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][2]>0)
              fragmentData[1][2]--;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(fragmentData[2][2]>0){
              fragmentData[2][2]--;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        for(int i = 0; i<3; i++){
          if(fragmentData[i][1]>fragmentData[i][2]){
            fragmentData[i][1] = fragmentData[i][2];
          }
        }
      }
      counterA += counterA<0?1:-1;
    }
    //changing subDivInt
    while(counterB != 0 && activeMenu.options[activeMenu.highlight] == "div"){
      if(counterB <= -1 && !shift){
        changeFragmentSubDivInt(true);
      }
      //changing subdivint
      if(counterB >= 1 && !shift){
        changeFragmentSubDivInt(false);
      }
      //if shifting, toggle between 1/3 and 1/4 mode
      else while(counterB != 0 && shift){
        toggleFragmentTriplets();
      }
      counterB += counterB<0?1:-1;;
    }
    display.clearDisplay();
    int menuStart;
    if(activeMenu.highlight>1){
      menuStart = activeMenu.highlight - 1;
    }
    else if(activeMenu.highlight<=1){
      menuStart = 0;
    }
    for(int i = menuStart; i<menuStart+3; i++){
      if(i<activeMenu.options.size()){
        display.setCursor(4,2+(i-menuStart)*10);
        if(i == activeMenu.highlight){
          display.drawRoundRect(2,1+(i-menuStart)*10,activeMenu.options[i].length()*6+3,10,4,SSD1306_WHITE);
          // display.setTextColor(SSD1306_BLACK);
          display.print(activeMenu.options[i]);
          // display.setTextColor(SSD1306_WHITE);
        }
        else{
          display.setTextColor(SSD1306_WHITE);
          display.print(activeMenu.options[i]);
        }
      }
    }
    //special displays for each menu option
    if(activeMenu.options[activeMenu.highlight] == "entry"){
      h = 64;
      //display timeline when selecting the start
      for(int lineStep = 0; lineStep<screenWidth; lineStep++){
        //if it's within the seq
        if((lineStep+timeS)<=seqEnd&&(lineStep+timeS)>=seqStart){
          if(playing){
            timeS = playheadPos - 64;
            if(lineStep+timeS == playheadPos){
              display.drawRoundRect(lineStep, xStart-10, 3, 20, 3, SSD1306_WHITE);
            }
          }
          //timeline is dotted
          if((lineStep+timeS)%3){
            display.drawPixel(lineStep,xStart,SSD1306_WHITE);
          }
          //if it's on a subDiv
          if(!((lineStep+timeS)%subDivInt)){
            display.drawLine(lineStep,xStart-1,lineStep,xStart+1,SSD1306_WHITE);
          }
          //if it's on a measure
          if(!((lineStep+timeS)%96)){
            display.drawLine(lineStep,xStart-5,lineStep,xStart+5,SSD1306_WHITE);
          }
          //min fragment start value
          if(lineStep+timeS == fragmentData[2][1]){
            display.fillTriangle(lineStep-3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
          }
          //max fragment start value
          if(lineStep+timeS == fragmentData[2][2]){
            display.fillTriangle(lineStep+3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
          }
          //in between the two vals
          if(lineStep+timeS > fragmentData[2][1] && lineStep+timeS < fragmentData[2][2]){
            display.drawPixel(lineStep,xStart+3,SSD1306_WHITE);
            display.drawPixel(lineStep,xStart-3,SSD1306_WHITE);
          }
        }
      }
      printCursive(screenWidth-30,2,"min",SSD1306_WHITE);
      printCursive(screenWidth-30,20,"max",SSD1306_WHITE);
      display.setCursor(screenWidth-20-stepsToPosition(fragmentData[0][1],false).length()*3,10);
      display.print(stepsToPosition(fragmentData[2][1],false));
      display.setCursor(screenWidth-20-stepsToPosition(fragmentData[0][2],false).length()*3,28);
      display.print(stepsToPosition(fragmentData[2][2],false));
      display.setCursor(5,35);
      display.print(stepsToPosition(timeS+64,false));
    }
    else if(activeMenu.options[activeMenu.highlight] == "reps"){
      printCursive(0,k+15,"minimum",SSD1306_WHITE);
      printCursive(screenWidth-42,k+15,"maximum",SSD1306_WHITE);
      //drawing minimum
      String temp = stringify(fragmentData[1][1]);
      display.setCursor(21-temp.length()*3,k+25);
      display.print(temp);
      //drawing maximum
      temp = stringify(fragmentData[1][2]);
      display.setCursor(screenWidth-21-temp.length()*3,k+25);
      display.print(temp);

      if(fragmentData[1][2]>1 && isFragmenting){
        int reps = random(fragmentData[1][1],fragmentData[1][2]);
        if(reps == 1){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps == 2){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps == 3){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps>3){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
      }
    }
    else if(activeMenu.options[activeMenu.highlight] == "size"){
      drawBracket(h,k+10,8,fragmentData[0][2]*fragmentSubDiv*float(screenWidth)/float(maxLengthVal)-1,0,SSD1306_WHITE);
      drawBracket(h,13,4,fragmentData[0][1]*fragmentSubDiv*float(screenWidth)/float(maxLengthVal)-1,1,SSD1306_WHITE);
      printCursive(0,k+15,"minimum",SSD1306_WHITE);
      printCursive(screenWidth-42,k+15,"maximum",SSD1306_WHITE);
      //drawing minimum
      String temp = "("+stringify(fragmentData[0][1])+")x("+stepsToMeasures(fragmentSubDiv)+")";
      display.setCursor(0,k+25);
      display.print(temp);
      //drawing maximum
      temp = "("+stringify(fragmentData[0][2])+")x("+stepsToMeasures(fragmentSubDiv)+")";
      display.setCursor(screenWidth-temp.length()*6,k+25);
      display.print(temp);
    }
    else if(activeMenu.options[activeMenu.highlight] == "state"){
      display.setFont(&FreeSerifItalic9pt7b);
      display.setCursor(5,50);
      if(isFragmenting){
        display.print("on");
        // printCursive(5,50,"on",SSD1306_WHITE);
      }
      else{
        // printCursive(5,50,"off",SSD1306_WHITE);
        display.print("off");
      }
      display.setFont();
    }
    else if(activeMenu.options[activeMenu.highlight] == "div"){
      display.setTextSize(2);
      display.setCursor(2,48);
      display.print(stepsToMeasures(fragmentSubDiv));
      display.setTextSize(1);
    }
    if(isFragmenting){
      width = random(fragmentData[0][1],fragmentData[0][2]);
      height = random(1,4);
    }
    else{
      width = fragmentData[0][2];
      height = 3;
    }
    // xDepth = random(-10,10);
    // yDepth = random(-10,10);
    //animated if the sequence is fragmenting
    if(!playing&&!recording){
      drawTetra(h+cos(millis()),k+sin(millis()),height*fragmentSubDiv*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
    }
    else{
      drawTetra(h+cos(millis()),k+sin(millis()),fragmentData[1][0]*float(screenHeight)/float(fragmentData[1][2]),fragmentData[0][0]*fragmentSubDiv*float(screenWidth)/float(fragmentData[0][2]),xDepth,yDepth,0,SSD1306_WHITE);
    }
    display.display();
  }
  fragmentAnimation(false);
}

void setTrackToNearestPitch(vector<uint8_t>pitches,int track,bool allowDuplicates){
  int oldPitch = trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the track is already that pitch, return
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if no duplicates are allowed, check to see if there are any other tracks
  //with this pitch
  if(!allowDuplicates){
    for(uint8_t t = 0; t<trackData.size(); t++){
      if(t!=track && trackData[t].pitch == pitches[closestPitch]+octaveOffset){
        return;
      }
    }
  }
  setTrackPitch(track,pitches[closestPitch]+octaveOffset,false);
}

//this one won't double up on a pitch
void setTrackToNearestPitch_exclusive(vector<uint8_t>pitches,int track){
  int oldPitch = trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the distance is 0, then the track is already in tune
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if there's no track already with this pitch, set the track to the pitch
  if(getTrackWithPitch(pitches[closestPitch]+octaveOffset) == -1){
    setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
  }
  //if there is, run it again without this pitch (as long as there're still pitches left)
  else{
    if(pitches.size()>1){
      vector<uint8_t>newPitches;
      for(uint8_t i = 0; i<pitches.size(); i++){
        if(i != closestPitch)
          newPitches.push_back(pitches[i]);
      }
      pitches.swap(newPitches);
      //recursively run the function again, just without the already-occupied pitch as an option
      setTrackToNearestPitch_exclusive(pitches, track);
    }
    //if there's only one pitch,set it to it
    else{
      setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
    }
  }
}

void deleteDuplicateEmptyTracks(){
  for(uint8_t t = 0; t<trackData.size(); t++){
    for(uint8_t t2 = 0; t2<trackData.size(); t2++){
      if(t2 == t)
        continue;
      //if a track has the same pitch, channel, and is empty, delete it
      if(trackData[t].pitch == trackData[t2].pitch &&
         trackData[t].channel == trackData[t2].channel &&
         seqData[t2].size() == 1){
        deleteTrack(t2);
      }
    }
  }
}

void tuneTracksToScale(){
  selectKeysAnimation(true);
  vector<uint8_t> pitches = selectKeys();
  selectKeysAnimation(false);
  vector<uint8_t> tracks = selectMultipleTracks("select tracks to tune");
  bool allowDuplicates = binarySelectionBox(64,32,"no","ye","allow duplicate pitches?");
  for(int track = tracks.size()-1; track >= 0; track--){
    setTrackToNearestPitch(pitches, tracks[track], allowDuplicates);
  }
}

void keyboard(){
  while(keys){
    display.clearDisplay();
    drawKeys(0,5,getOctave(keyboardPitch),14,true);//always start on a C, for simplicity
    display.display();
    readButtons();
    stepButtons();//handles notes, and toggling
    if(itsbeen(200)){
      if(menu_Press){
        toggleKeys();
        menuIsActive = false;
        constructMenu("MENU");
      }
    }
    //midi messages from encoders when in keys/drumpads mode
    while(counterA != 0){
      if(counterA >= 1 && keyboardPitch<127){
        keyboardPitch++;
      }
      else if(counterA <= -1 && keyboardPitch>0){
        keyboardPitch--;
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1 && defaultChannel<16){
        defaultChannel++;
        counterB += counterB<0?1:-1;;
      }
      else if(counterB <= -1 && defaultChannel>0){
        defaultChannel--;
        counterB += counterB<0?1:-1;;
      }
    }
  }
}
void drums(){
  while(drumPads){
    display.clearDisplay();
    readButtons();
    stepButtons();//handles notes, and toggling
    //midi messages from encoders when in keys/drumpads mode
    while(counterA != 0){
      if(counterA >= 1 && keyboardPitch<127){
        keyboardPitch++;
      }
      else if(counterA <= -1 && keyboardPitch>0){
        keyboardPitch--;
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1 && defaultChannel<16){
        defaultChannel++;
        counterB += counterB<0?1:-1;;
      }
      else if(counterB <= -1 && defaultChannel>0){
        defaultChannel--;
        counterB += counterB<0?1:-1;;
      }
    }
    //play/rec
    if(itsbeen(200)){
      if(play && !recording){
        if(!shift){
          lastTime = millis();
          togglePlayMode();
        }
        else if(shift){
          lastTime = millis();
          toggleRecordingMode(waitForNote);
        }
      }
      if(play && recording){
        lastTime = millis();
        toggleRecordingMode(waitForNote);
      }
      if(menu_Press){
        toggleDrumPads();
        menuIsActive = false;
        constructMenu("MENU");
        break;
      }
    }
    drawDrumPads(screenWidth-25,5,keyboardPitch - keyboardPitch%12,16);
    display.display();
  }
}


class Knob{
  public:
    Knob();
    Knob(uint8_t,uint8_t,uint8_t);
    uint8_t cc;
    uint8_t val;
    uint8_t channel;//0 is global
    void increment(int8_t);
    void send();
};

Knob::Knob(){
  cc = 1;
  val = 0;
  channel = 1;
}
Knob::Knob(uint8_t c, uint8_t v, uint8_t ch){
  cc = c;
  val = v;
  channel = ch;
}
void Knob::increment(int8_t amt){
  if(amt>0){
    val+=amt;
    if(val>127){
      val = 127;
    }
  }
  else if(-amt<=val){
    val+=amt;
  }
  else{
    val = 0;
  }
}
void Knob::send(){
  sendMIDICC(cc,val,channel);
}

Knob controlKnobs[16];

void drawKnobs(uint8_t activeA, uint8_t activeB, uint8_t activeRow, uint8_t howMany, bool selected, uint8_t ccType,uint8_t valA, uint8_t valB, bool xyMode){
  const uint8_t bigR = 12;
  const uint8_t smallR = 5;
  const uint8_t x2 = screenWidth-bigR-5;
  uint8_t y2 = bigR;
  uint8_t x1 = 14;
  uint8_t y1 = 14;
  uint8_t count = 0;
  
  //printing little rows of encoders

  //labels for the cc/ch vals
  display.drawBitmap(0,y1-12,cc_tiny,5,3,1);
  display.drawBitmap(0,y1+13,ch_tiny,6,3,1);
  for(uint8_t row = 0; row<2; row++){
    //if one row is selected, draw the cc display on the other
    if(selected && row != activeRow){
      String param = ccType?getMKIIParameterName(controlKnobs[activeRow?(activeB+8):activeA].cc,controlKnobs[activeRow?(activeB+8):activeA].channel):getCCParameterName(controlKnobs[activeRow?(activeB+8):activeA].cc);
      printSmall_centered(48,y1,"["+param+"]",1);
    }
    //if you're not in selected mode, draw both rows like normal
    else{
      for(uint8_t column = 0; column<8; column++){
        if(count<howMany){
          //values to display
          uint8_t value = controlKnobs[row*8+column].val;
          uint8_t cc = controlKnobs[row*8+column].cc;
          uint8_t ch = controlKnobs[row*8+column].channel;
          int8_t offset = (column%2)?-2:2;//offset to stagger the dials
          //if it's the active knob
          if((row == 0 && column == activeA) || (row == 1 && column == activeB)){
            //cursor arrow for activeRow
            if(row == activeRow){
              if(selected){
                drawArrow(x2-bigR-13+sin(millis()/100),y2-bigR+6+row*32,3,0,true);
                drawArrow(x2-bigR-13+sin(millis()/100),y2-bigR+17+row*32,3,0,true);
              }
              else
                drawArrow(x1,y1+offset+smallR+6+abs(2*sin(millis()/100)),3,2,false);
            }
            //drawing each dial in white, if it's active
            display.fillCircle(x1,y1+offset,smallR,1);
            display.drawCircle(x1,y1+offset,smallR-1,0);
            drawCircleRadian(x1,y1+offset,smallR,float(value)*float(360)/float(127)+270,0);
          }
          else{
            //drawing the dials in black, when not active
            display.fillCircle(x1,y1+offset,smallR,0);
            display.drawCircle(x1,y1+offset,smallR,1);
            drawCircleRadian(x1,y1+offset,smallR,float(value)*float(360)/float(127)+270,1);
          }
          //printing the cc and ch vals
          printSmall_centered(x1+1,y1+offset-smallR-6,stringify(cc),1);
          printSmall_centered(x1+1,y1+offset+smallR+2,stringify(ch),1);
          count++;
          x1+=smallR*2-2;
        }
      }
    }
    y1+=31;
    x1 = 14;
  }
  //printing the two big encoders
  for(uint8_t i = 0; i<2; i++){
    Knob k = controlKnobs[(i==0)?activeA:(activeB+8)];
    if(!selected)
      //drawing the val with a highlight, when it's in value mode
      drawLabel(x2-bigR-7,y2+bigR-1,stringify(k.val),1);
    else{
      //when it's in 'selected' mode, don't highlight val so that
      //user knows they're changing cc/ch now
      printSmall_centered(x2-bigR-7,y2+bigR-1,stringify(k.val),1);
    }
    if(xyMode){
      //draw dials in white when in xy mode
      display.fillCircle(x2,y2,bigR,1);
      display.drawCircle(x2,y2,bigR-1,0);
      drawCircleRadian(x2,y2,bigR,float(i==0?valA:valB)/float(127)*float(360)+270,0);
    }
    else{
      //draw dials in black when in normal mode
      display.drawCircle(x2,y2,bigR,1);
      drawCircleRadian(x2,y2,bigR,float(i==0?valA:valB)/float(127)*float(360)+270,1);
    }
    //printing data to left of the dial
    display.drawBitmap(x2-bigR-10,y2-bigR,cc_tiny,5,3,1);
    display.drawBitmap(x2-bigR-10,y2-bigR+11,ch_tiny,6,3,1);
    printSmall_centered(x2-bigR-7,y2-bigR+4,stringify(k.cc),1);
    printSmall_centered(x2-bigR-7,y2-bigR+15,stringify(k.channel),1);
    y2+=32;
  }
  if(xyMode){
    printItalic(x2-3,9,"Y",0);
    printItalic(x2-3,41,"X",0);
  }
}
void knobsAnimationIn(){
  uint8_t howMany = 0;
  while(howMany<16){
    display.clearDisplay();
    drawKnobs(0,0,0,howMany,false,0,controlKnobs[0].val,controlKnobs[8].val,false);
    display.display();
    howMany++;
  }
}
void knobsAnimationOut(uint8_t activeA, uint8_t activeB, uint8_t activeRow){
  int8_t howMany = 16;
  while(howMany>=0){
    display.clearDisplay();
    drawKnobs(activeA,activeB,activeRow,howMany,false,0,controlKnobs[0].val,controlKnobs[8].val,false);
    display.display();
    howMany-=2;
  }
}

void knobs(){
  knobsAnimationIn();
  uint8_t activeKnobA = 0;
  uint8_t activeKnobB = 0;//this is also between 0 and 7
  uint8_t activeRow = 0;
  uint8_t ccType = 0;//0 for gen. midi, 1 for mk2
  bool selected = false;
  bool xyMode = false;
  //range is (5-1023) aka 0-1018 (took this from xyGrid)
  const float scaleF = float(128)/float(1018);

  uint8_t valA = controlKnobs[activeKnobA].val;
  uint8_t valB = controlKnobs[activeKnobB].val;

  while(true){
    valA += (controlKnobs[activeKnobA].val-valA)/4;
    valB += (controlKnobs[activeKnobB+8].val-valB)/4;
    if(xyMode){
      controlKnobs[activeKnobB+8].val = abs(128 - (analogRead(x_Pin) - 5) * scaleF);
      controlKnobs[activeKnobA].val = abs((analogRead(y_Pin) - 5) * scaleF);
      if(controlKnobs[activeKnobA].val>127)
        controlKnobs[activeKnobA].val = 127;
      if(controlKnobs[activeKnobB+8].val>127)
        controlKnobs[activeKnobB+8].val = 127;
    }
    display.clearDisplay();
    drawKnobs(activeKnobA,activeKnobB,activeRow,16,selected,ccType,valA,valB,xyMode);
    display.display();
    readButtons();
    joyRead();
    //changing cc/channel
    if(selected){
      while(counterA != 0){
        if(counterA >= 1 && controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel<16){
          controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel++;
        }
        else if(counterA <= -1 && controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel>0){
          controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel--;
        }
        controlKnobs[activeRow?(activeKnobB+8):activeKnobA].send();

        counterA += counterA<0?1:-1;
      }
      while(counterB != 0){
        if(counterB >= 1 && controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc<127){
          controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc++;
        }
        else if(counterB <= -1 && controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc>0){
          controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc--;
        }
        controlKnobs[activeRow?(activeKnobB+8):activeKnobA].send();
        counterB += counterB<0?1:-1;;
      }
    }
    else{
      // Serial.println("B:"+stringify(counterB));
      // Serial.println("A:"+stringify(counterA));
      // Serial.flush();
      while(counterB != 0){
        if(counterB >= 1){
          controlKnobs[activeKnobA].increment(shift?1:8);
        }
        else if(counterB <= -1){
          controlKnobs[activeKnobA].increment(shift?-1:int8_t(-8));
        }
        controlKnobs[activeKnobA].send();
        // counterB += counterB<0?1:-1;;
        counterB+=counterB<0?1:-1;
      }
      while(counterA != 0){
        if(counterA >= 1){
          controlKnobs[activeKnobB+8].increment(shift?1:8);
        }
        else if(counterA <= -1){
          controlKnobs[activeKnobB+8].increment(shift?-1:int8_t(-8));
        }
        controlKnobs[activeKnobB+8].send();
        counterA += counterA<0?1:-1;
      }
    }
    if(itsbeen(100)){
      if(x != 0){
        if(!xyMode){
          if(x == -1){
            if(activeRow == 0 && activeKnobA<7){
              activeKnobA++;
              lastTime = millis();
            }
            else if(activeRow == 1 && activeKnobB<7){
              activeKnobB++;
              lastTime = millis();
            }
          }
          else if(x == 1){
            if(activeRow == 0 && activeKnobA>0){
              activeKnobA--;
              lastTime = millis();
            }
            else if(activeRow == 1 && activeKnobB>0){
              activeKnobB--;
              lastTime = millis();
            }
          }
        }
      }
      if(y != 0){
        if(!xyMode){
          if(y == 1 && activeRow == 0){
            activeRow = 1;
            lastTime = millis();
          }
          if(y == -1 && activeRow == 1){
            activeRow = 0;
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      if(sel){
        selected = !selected;
        lastTime = millis();
      }
      if(shift){
        activeRow = activeRow?1:0;
        lastTime = millis();
      }
      if(play){
        xyMode = !xyMode;
        lastTime = millis();
      }
      if(menu_Press){
        lastTime = millis();
        if(selected){
          selected = false;
        }
        else{
          break;
        }
      }
    }
  }
  knobsAnimationOut(activeKnobA, activeKnobB, activeRow);
}

class Raindrop{
  public:
    uint8_t length;
    uint8_t x1;
    float y1;
    float vel;//pixels/frame
    bool madeSound;
    Raindrop();
    Raindrop(uint8_t,uint8_t,uint8_t);
    void render();
    bool update();
};

Raindrop::Raindrop(){
  vel = 0.7;
  length = vel*2;
  x1 = random(0,screenWidth);
  y1 = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
Raindrop::Raindrop(uint8_t xPos, uint8_t maxVel, uint8_t minVel){
  vel = pow(float(random(minVel,maxVel))/float(5),2);
  length = vel+3;
  x1 = xPos;
  y1 = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
void Raindrop::render(){
  display.drawFastVLine(x1,y1,length,1);
}
bool Raindrop::update(){
  bool  hasNotCrossedYet = false;
  if(y1<64){
    hasNotCrossedYet = true;
  }
  y1+=vel;
  if(y1>64 &&  hasNotCrossedYet){
    madeSound = true;
  }
  else{
    madeSound = false;
  }
  if(y1>=screenHeight+length){
    return false;
  }
  else{
    return true;
  }
}

//gradient on top representing rain flow, moveable 
//drops velocities correspond to note vel
//X axis is pitch bend? or pitch? and y-axis is intensity (can also be controlled with encoders)
void rain(){
  vector<Raindrop> drops;
  //dropsPerFrame and stormIntensity both increase as the storm gets worse
  uint8_t stormIntensity = 5;
  uint8_t maxDrops = 10;
  const uint8_t maxIntensity = 20;
  uint8_t xCoord = 64;
  const uint8_t intensityVariance = 5;
  const uint8_t xVariance = 20;
  uint8_t frames = 0;
  while(true){
    readButtons();
    joyRead();
    while(counterB != 0){
      if(counterB >= 1 && stormIntensity<maxIntensity){
        stormIntensity++;
        maxDrops++;
      }
      else if(stormIntensity>1){
        stormIntensity--;
        maxDrops--;
      }
      counterB += counterB<0?1:-1;;
    }
    while(counterA != 0){
      if(counterA >= 1 && xCoord<(screenWidth-xVariance)){
        xCoord++;
      }
      else if(xCoord>xVariance){
        xCoord--;
      }
      counterA += counterA<0?1:-1;
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        for(uint8_t i = 0; i<20; i++){
          if(drops.size()<maxDrops)
            drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?0.1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
        }
      }
      if(menu_Press){
        lastTime  =  millis();
        return;
      }
    }
    //rendering drops
    display.clearDisplay();
    printSmall(0,0,stringify(stormIntensity),1);
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render();
    }
    display.display();
    for(uint8_t i = 0; i<drops.size(); i++){
      if(!drops[i].update()){
        drops.erase(drops.begin()+i,drops.begin()+i+1);
        continue;
      }
      if(drops[i].madeSound){
        sendMIDInoteOn(60,drops[i].vel,1);
        sendMIDInoteOff(60,0,1);
      }
    }
    //making new drops
    for(uint8_t i = 0; i<random(0,maxDrops); i++){
      if(drops.size()<maxDrops)
        drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
    }
  }
}

bool Menu::moveMenuCursor(bool forward){
  if(options.size() == 0){
    highlight = 0;
    return false;
  }
  else{
    if(forward && highlight<options.size()-1){
      highlight++;
      return true;
    }
    else if(!forward && highlight>0){
      highlight--;
      return true;
    }
  }
  return false;
  // Serial.print("menu highlighting option ");
  // Serial.print(highlight);
  // Serial.print("; ");
  // Serial.println(activeMenu.options[highlight]);
}
WireFrame getMenuWireFrame(){
  WireFrame w;
  switch(activeMenu.highlight){
    //dataTracks
    case 0:
      w = makeGraphBox(5);
      break;
    //loops
    case 1:
      w = makeLoopArrows(0);
      break;
    //keys
    case 2:
      w = makeKeys();
      break;
    //wrench
    case 3:
      w = makeWrench();
      break;

    //quicksave
    case 4:
      w = makeCD();
      break;
    //fx
    case 5:
      w = makeHammer();
      break;
    //rec
    case 6:
      w = makeCassette();
      break;
    //heart
    case 7:
      w = makePram();
      // w.yPos = 20;
      break;

    //midi
    case 8:
      w = makeMIDI();
      break;
    //files
    case 9:
      w = makeFolder(30);
      break;
    //clock
    case 10:
      w = makeMetronome(0);
      break;
    //arp
    case 11:
      w = makeHand_flat(0,0,0,0,0);
      w.yPos = 32;
      break;
  }
  return w;
}
void animateIcon(WireFrame* w){
  switch(activeMenu.highlight){
    //datatracks
    case 0:
      graphAnimation(w);
      w -> rotate(3,1);
      break;
    //loop
    case 1:
      loopArrowAnimation(w);
      break;
    //keys
    case 2:
      w -> rotate(3,1);
      break;
    //settings
    case 3:
      w -> rotate(3,1);
      break;
    case 4:
      w -> rotate(3,1);
      break;
    //fx
    case 5:
      w -> rotate(3,1);
      break;
    //rec
    case 6:
      w -> rotate(3,1);
      break;
    //heart
    case 7:
      w -> rotate(3,1);
      break;
    
    //midi
    case 8:
      w -> rotate(3,1);
      break;
    //files
    case 9:
      folderAnimation(w);
      w -> rotate(3,1);
      break;
    //clock
    case 10:
      metAnimation(w);
      w -> yPos = 16 + 2*sin(millis()/400);
      break;
    //arp
    case 11:
      handAnimation(w);
      w -> yPos=20+2*sin(millis()/200);
      w -> rotate(4*sin(millis()/400),1);
      break;
  }
}

//for no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2){
  return binarySelectionBox(x1,y1,op1,op2,"");
}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title){
  //bool for breaking from the loop
  bool notChosenYet = true;
  //storing the state
  bool state = false;

  const uint8_t height = 11;

  //getting the longest string so we know how long to make the box
  uint8_t maxLength;
  if(op1.length()>op2.length())
    maxLength = op1.length();
  else
    maxLength = op2.length();

  uint8_t length = (maxLength*4+3)*2;
  lastTime = millis();
  while(true){
    display.fillRect(x1-length/2,y1-height/2,length,height,SSD1306_BLACK);
    drawSlider(x1-length/2,y1-height/2,length,height,state);
    printSmall(x1-length/4-op1.length()*2+1,y1-2,op1,2);
    printSmall(x1+length/4-op2.length()*2,y1-2,op2,2);
    if(title.length()>0){
      const uint8_t height2 = 9;
      uint8_t length2 = title.length()*4-countSpaces(title)*2;
      display.fillRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_BLACK);
      display.drawRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_WHITE);
      printSmall(x1-length2/2,y1-height2-4,title,SSD1306_WHITE);
    }
    display.display();
    joyRead();
    readButtons();
    if(itsbeen(200)){
      //x to select option
      if(x != 0){
        if(x == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(x == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/delete to cancel
      if(menu_Press || del){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(n || sel){
        lastTime = millis();
        return state;
      }
    }
  }
  return false;
}

void drawVU(int8_t x1, int8_t y1, float val){
  display.drawBitmap(x1,y1,VUmeter_bmp,19,14,SSD1306_WHITE);
  float angle = radians(5)+(PI-radians(5))*val;
  int8_t pY = 12*sin(angle);
  int8_t pX = 12*cos(angle);
  display.drawLine(x1+9,y1+12,x1+10+pX,y1+12-pY,SSD1306_BLACK);
  display.drawRect(x1,y1,19,14,SSD1306_WHITE);
}
void drawPendulum(int16_t x2, int16_t y2, int8_t length, float val){
  //pendulum
  int a = length;
  int h = x2;
  int k = y2;
  float x1;
  float y1;
  x1 = h + a * cos(radians(val))/float(2.4);
  // if (val > 180) {
    // y1 = k - a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  // }
  // else {
    y1 = k + a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  // }
  display.drawLine(x1,y1,h,k,SSD1306_WHITE);
  display.fillCircle(x1,y1,2,SSD1306_BLACK);
  display.drawCircle(x1,y1,2,SSD1306_WHITE);
}

void drawLabel(uint8_t x1, uint8_t y1, String text, bool wOrB){
  display.fillRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-1,text.length()*4-countSpaces(text)*2+5,7,3,wOrB == true ? 1:0 );
  if(!wOrB)
    display.drawRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-1,text.length()*4-countSpaces(text)*2+5,7,3,1);
  printSmall(x1-text.length()*2+countSpaces(text),y1,text,2);
}

void drawSlider(uint8_t x1, uint8_t y1, uint8_t w, uint8_t h, bool state){
  display.fillRect(x1,y1,w,h,0);
  display.drawRect(x1,y1,w,h,SSD1306_WHITE);
  if(state){
    display.fillRect(x1+w/2,y1+2,w/2-2,h-4,SSD1306_WHITE);
  }
  else{
    display.fillRect(x1+2,y1+2,w/2-2,h-4,SSD1306_WHITE);
  }
}
void drawSlider(uint8_t x1, uint8_t y1, String a, String b, bool state){
  uint8_t length = a.length()*4+b.length()*4+9;
  //if length is odd, add 1
  length+=(length%2)?1:0;
  drawSlider(x1,y1,length,11,state);
  printSmall_centered(x1+length/4+2,y1+3,a,2);
  printSmall_centered(x1+3*length/4,y1+3,b,2);
  // printSmall_centered(x1+length/2,y1+3,a,2);
  // printSmall_centered(x1+length-b.length()*4-2,y1+3,b,2);
}

//returns the number of digits in a byte-sized (8bit) number
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
void drawCenteredBanner(int8_t x1, int8_t y1, String text){
  uint8_t len = text.length()*4-countSpaces(text)*2+countChar(text,'#')*2;
  x1-=len/2;
  drawBanner(x1,y1,text);
}

void drawBanner(int8_t x1, int8_t y1, String text){
  display.drawBitmap(x1-13,y1-4,bannerL_bmp,12,9,SSD1306_WHITE);
  // display.drawBitmap(x1+text.length()*4-countSpaces(text)*2,y1,bannerR_bmp,11,9,SSD1306_WHITE);
  display.setRotation(UPSIDEDOWN);
  display.drawBitmap(screenWidth-(x1+text.length()*4-countSpaces(text)*2+countChar(text,'#')*2)-12,screenHeight-y1-9,bannerL_bmp,12,9,SSD1306_WHITE);
  display.setRotation(UPRIGHT);
  display.fillRect(x1-1,y1-1,text.length()*4-countSpaces(text)*2+countChar(text,'#')*2+1,7,SSD1306_WHITE);
  printSmall(x1,y1,text,SSD1306_BLACK);
}
void drawFxLabel(){
  String text = "NAH";
  switch(activeMenu.highlight){
    case 0:
      text = "CHORD";
      break;
    case 1:
      text = "ECHO";
      break;
    case 2:
      text = "HUMANIZE";
      break;
    case 3:
      text = "WARP";
      break;
    case 4:
      text = "QUANTIZE";
      break;
    case 5:
      text = "REVERSE";
      break;
    case 6:
      text = "RANDOM";
      break;
    case 7:
      text = "SCRAMBLE";
      break;
    case 8:
      text = "SPLIT";
      break;
    case 9:
      text = "STRUM";
      break;
    case 10:
      text = "CHOP";
      break;
    case 11:
      text = "MAYHEM";
      break;
  }
  printChunky(activeMenu.topL[0]+activeMenu.topL[1]+38,4,text,SSD1306_WHITE);
}

void selectInstrumentMenu(){
  uint8_t cursor = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(100)){
      if(x != 0){
        if(x == -1 && (cursor%4!=3)){
          cursor++;
          lastTime = millis();
        }
        else if(x == 1 && (cursor%4)){
          cursor--;
          lastTime = millis();
        }
      }
      if(y != 0){
        if(y == 1 && (cursor<12)){
          cursor+=4;
          lastTime = millis();
        }
        else if(y == -1 && (cursor>3)){
          cursor-=4;
          lastTime = millis();
        }
      }
    }
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(sel){
        lastTime = millis();
        sel = false;
        switch(cursor){
          case 0:
            xyGrid();
            break;
          case 1:
            testSolarSystem();
            break;
          case 2:
            rain();
            break;
          case 3:
            knobs();
            break;
          case 4:
            toggleKeys();
            break;
          case 5:
            toggleDrumPads();
            break;
        }
      }
    }
    display.clearDisplay();
    const uint8_t x1 = 36;
    const uint8_t y1 = 1;
    const uint8_t width = 14;
    const uint8_t height = 14;
    uint8_t count = 0;
    for(uint8_t j = 0; j<4; j++){
      for(uint8_t i = 0; i<4; i++){
        display.drawRoundRect(x1+16*i,y1+16*j+sin(millis()/200+j*4+i),width,height,3,1);
        if(count<6)
          display.drawBitmap(x1+16*i+1,y1+16*j+sin(millis()/200+j*4+i)+1,instrument_icons[count],12,12,1);
        count++;
      }
    }

    //cursor
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_fill_bmp,9,15,0);
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_outline_bmp,9,15,1);
    
    //title
    // printCursive(-1,0,"instruments",1);
    display.drawPixel(64,5,0);
    String text;
    switch(cursor){
      case 0:
        text = "joystick";
        break;
      case 1:
        text = "Planets";
        break;
      case 2:
        text = "Rain";
        break;
      case 3:
        text = "Knobs";
        break;
      case 4:
        text = "keys";
        break;
      case 5:
        text = "pads";
        break;
    }
    // printArp(0,10,text,1);
    display.setRotation(1);
    // printSmall(0,10,text,1);
    printItalic(screenHeight-text.length()*8,27,text,1);
    display.setRotation(UPRIGHT);
    display.display();
  }
}

void fxMenu(){
  int angleX = 0;
  int angleY = 0;
  int angleZ = 0;

  int angle2X = 0;
  int angle2Y = 0;
  int angle2Z = 0;

  WireFrame icon = makeGyro(angleX,angleY,angleZ,
                  angle2X,angle2Y,angle2Z);
  while(menuIsActive && activeMenu.menuTitle == "FX"){
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    drawPram(5,0);
    display.fillCircle(111,13,23,0);
    display.drawCircle(111,13,23,1);
    activeMenu.displayFxMenu();
    icon.render();
    printItalic(103,13,"FX",1);
    display.display();
    joyRead();
    readButtons();
    if(!fxMenuControls()){
      break;
    }
    angleX+=2;
    angleY-=2;
    // angleZ-=2;
    angleX%=360;
    angleY%=360;
    angleZ%=360;
    angle2X+=2;
    // angle2Y+=2;
    angle2Z+=2;
    angle2X%=360;
    angle2Y%=360;
    angle2Z%=360;
    icon = makeGyro(angleX,angleY,angleZ,
                    angle2X,angle2Y,angle2Z);
  }
  constructMenu("MENU");
  activeMenu.highlight = 5;//so the 3d icon doesn't flash
  // mainMenu();
}
void Menu::displayFxMenu(){
  display.fillRect(topL[0],topL[1]+13, bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
  display.drawRect(topL[0],topL[1]+12, bottomR[0]-topL[0], bottomR[1]-topL[1]-12, SSD1306_WHITE);

  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(topL[0]+topL[1]-2,11);
  display.print("Fx");
  display.setFont();
  
  drawFxLabel();
  //printing page number/arrows
  printSmall(topL[0]+topL[1]+21,2,stringify(page+1)+"/2",SSD1306_WHITE);
  if(page == 0){
    drawArrow(topL[0]+topL[1]+26,11+sin(millis()/200),2,3,true);
  }
  else{
    drawArrow(topL[0]+topL[1]+26,9+sin(millis()/200),2,2,true);
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),fxMenu_icons[page*12+count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight-12*page){
        display.drawRoundRect(topL[0]+2+width*i,topL[1]+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}


void deleteCustomLength(uint8_t which){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<activeArp.lengths.size(); i++){
    if(i != which){
      newNotes.push_back(activeArp.lengths[i]);
    }
  }
  activeArp.lengths.swap(newNotes);
}

void swapCustomLengths(uint8_t which1, uint8_t which2){
  uint8_t temp = activeArp.lengths[which1];
  activeArp.lengths[which1] = activeArp.lengths[which2];
  activeArp.lengths[which2] = temp;
}

void addCustomLength(uint8_t size, uint8_t where){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<activeArp.lengths.size(); i++){
    if(i == where){
      newNotes.push_back(size);
    }
    newNotes.push_back(activeArp.lengths[i]);
  }
  activeArp.lengths.swap(newNotes);
}

void customLengthsMenu(){
  uint8_t cursorX = 0;
  bool selected = false;
  uint8_t startNote = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(sel){
        lastTime = millis();
        activeArp.uniformLength = !activeArp.uniformLength;
      }
    }
    if(activeArp.uniformLength){
      if(itsbeen(200)){
        if(menu_Press){
          menu_Press = false;
          lastTime = millis();
          return;
        }
      }
    }
    //these controls are only active if the arp is using custom lengths
    else{
      if(itsbeen(200)){
        if(n){
          addCustomLength(activeArp.lengths[cursorX],cursorX);
          lastTime = millis();
        }
        if(sel){
          activeArp.uniformLength = !activeArp.uniformLength;
          lastTime = millis();
        }
        if(del && activeArp.lengths.size()>1){
          deleteCustomLength(cursorX);
          lastTime = millis();
          while(startNote+7>=activeArp.lengths.size()&& activeArp.lengths.size()>8){
            startNote--;
          }
          while(cursorX>=activeArp.lengths.size())
            cursorX--;
        }
        if(menu_Press){
          lastTime = millis();
          menu_Press = false;
          break;
        }
        if(sel){
          selected = !selected;
          lastTime = millis();
        }
      }
      if(itsbeen(120)){
        if(y != 0){
          if(shift){
            activeArp.lengths[cursorX] = toggleTriplets(activeArp.lengths[cursorX]);
            lastTime = millis();
          }
          else{
            if(y == -1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(true,activeArp.lengths[cursorX],false);
            }
            else if(y == 1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(false,activeArp.lengths[cursorX],false);
            }
          }
        }
      }
      if(itsbeen(100)){
        if(x != 0){
          if(x == -1 && cursorX<activeArp.lengths.size()-1){
            cursorX++;
            lastTime = millis();
          }
          if(x == 1 && cursorX>0){
            cursorX--;
            lastTime = millis();
          }
          while(cursorX<startNote){
            startNote--;
          }
          while(cursorX>startNote+7){
            startNote++;
          }
        }
      }
    }
    display.clearDisplay();
    drawArpStepLengths(0,0,startNote,cursorX,selected);
    display.display();
  }
}

void drawModBoxes(uint8_t cursor){
  //three boxes to represent the three parameters
  uint8_t start = 32-activeArp.maxVelMod/4;
  uint8_t end = 32+activeArp.minVelMod/4;
  uint8_t length = end-start;
  drawBox(4,3*sin(float(millis())/float(400))+start,16,length,4,-4,0);
  printSmall(8,3*sin(float(millis())/float(400))+32,"v",1);

  drawBox(20,3*sin(float(millis())/float(400)+200)+32-activeArp.chanceMod/4,16,activeArp.chanceMod/2,4,-4,0);
  printSmall(24,3*sin(float(millis())/float(400)+200)+32,"%",1);

  // drawBox(40,3*sin(float(millis())/float(400)+400)+32-activeArp.lengthMod/4,16,activeArp.lengthMod/2,4,-4,0);
  // printSmall(44,3*sin(float(millis())/float(400)+400)+32,"l",1);

  drawBox(36,3*sin(float(millis())/float(400)+600)+32-activeArp.repMod/4,16,activeArp.repMod/2,4,-4,0);
  printSmall(40,3*sin(float(millis())/float(400)+600)+32,"x",1);

  start = 32 - activeArp.maxPitchMod/4;
  end = 32 + activeArp.minPitchMod/4;
  length = end - start;
  drawBox(52,3*sin(float(millis())/float(400)+800)+start,16,length,4,-4,0);
  printSmall(56,3*sin(float(millis())/float(400)+800)+32,"$",1);

  drawBox(77,23,16,16,4,-4,(cursor == 4)?2:1);
  drawArpModeIcon(76,30,activeArp.playStyle,2);

  switch(cursor){
    case 0:
      // drawDottedLineV(14,36-activeArp.velMod/4,32+activeArp.velMod/4,2);
      drawArrow(9,37+activeArp.minVelMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,30,3,0);//box
      display.drawRoundRect(95,22,32,30,3,1);//box
      display.drawFastHLine(97,22,13,0);//mask for letters
      printSmall(98,20,"vel",1);
      printSmall(98,30,"mx:",1);
      print007SegSmall(109,29,stringify(64+activeArp.maxVelMod/2),1);
      printSmall(98,40,"mn:",1);
      print007SegSmall(109,39,stringify(64-activeArp.minVelMod/2),1);
      break;
    case 1:
      // drawDottedLineV(30,36-activeArp.chanceMod/4,32+activeArp.chanceMod/4,2);
      drawArrow(25,37+activeArp.chanceMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,18,3,0);//box
      display.drawRoundRect(95,22,32,18,3,1);//box
      display.drawFastHLine(97,22,26,0);//mask for letters
      printSmall(98,20,"chance",1);
      print007SegSmall(100,29,stringify(activeArp.chanceMod)+"%",1);
      break;
    case 2:
      // drawDottedLineV(62,36-activeArp.repMod/4,32+activeArp.repMod/4,2);
      drawArrow(41,37+activeArp.repMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,18,3,0);//box
      display.drawRoundRect(95,22,32,18,3,1);//box
      display.drawFastHLine(95,22,17,0);//mask for letters
      printSmall(98,20,"reps",1);
      print007SegSmall(103,29,stringify(activeArp.repMod)+"%",1);
      break;
    case 3:
      drawArrow(57,37+activeArp.minPitchMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,30,3,0);//box
      display.drawRoundRect(95,22,32,30,3,1);//box
      display.drawFastHLine(97,22,21,0);//mask for letters
      printSmall(98,20,"pitch",1);
      printSmall(101,30,"mx:",1);
      printSmall(113,30,stringify(activeArp.maxPitchMod/16),1);
      // print007SegSmall(105,29,stringify(activeArp.maxPitchMod/16),1);
      printSmall(101,40,"mn:",1);
      printSmall(113,40,stringify(-activeArp.minPitchMod/16),1);
      // print007SegSmall(105,39,stringify(activeArp.minPitchMod/16),1);
      break;
    case 4:
      drawArrow(81,43+sin(millis()/200),3,2,true);

      String text;
      switch(activeArp.playStyle){
        case 0:
          text = "play order";
          break;
        case 5:
          text = "down/up";
          break;
        case 4:
          text = "up/down";
          break;
        case 3:
          text = "down";
          break;
        case 2:
          text = "up";
          break;
        case 1:
          text = "random";
          break;
      }
      // display.fillRoundRect(17,22,41,20,3,0);//box
      // display.drawRoundRect(17,22,41,20,3,1);//box
      // display.drawFastHLine(19,22,45,0);//mask for letters
      printSmall(88,47,text,1);
      break;
  }
}

void arpModMenu(){
  uint8_t cursor = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(x != 0){
        if(x == -1 && cursor<4){
          cursor++;
          lastTime = millis();
        }
        else if(x == 1 && cursor>0){
          cursor--;
          lastTime = millis();
        }
      }
      if(del){
        switch(cursor){
          case 0:
            activeArp.maxVelMod = 0;
            activeArp.minVelMod = 0;
            lastTime = millis();
            break;
          case 1:
            activeArp.chanceMod = 100;
            lastTime = millis();
            break;
          case 2:
            activeArp.repMod = 0;
            lastTime = millis();
            break;
          case 3:
            activeArp.maxPitchMod = 0;
            activeArp.minPitchMod = 0;
            lastTime = millis();
            break;
          case 4:
            activeArp.playStyle = 0;
            lastTime = millis();
            break;
        }
      }
      while(counterA != 0){
        switch(cursor){
          case 0:
            if(!shift){
              if(counterA <= -1){
                if(activeArp.minVelMod>117)
                  activeArp.minVelMod = 127;
                else
                  activeArp.minVelMod += 10;
              }
              if(counterA >= 1){
                if(activeArp.minVelMod>10)
                  activeArp.minVelMod-=10;
                else
                  activeArp.minVelMod = 0;
              }
            }
            else{
              if(counterA <= -1 && activeArp.minVelMod<127){
                activeArp.minVelMod++;
              }
              else if(counterA >= 1 && activeArp.minVelMod>0){
                activeArp.minVelMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 1:
            if(!shift){
              if(counterA >= 1){
                if(activeArp.chanceMod>90)
                  activeArp.chanceMod = 100;
                else
                  activeArp.chanceMod += 10;
              }
              if(counterA <= -1){
                if(activeArp.chanceMod>10)
                  activeArp.chanceMod-=10;
                else
                  activeArp.chanceMod = 0;
              }
            }
            else{
              if(counterA >= 1 && activeArp.chanceMod<100){
                activeArp.chanceMod++;
              }
              else if(counterA <= -1 && activeArp.chanceMod>0){
                activeArp.chanceMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 2:
            if(!shift){
              if(counterA >= 1){
                if(activeArp.repMod>90)
                  activeArp.repMod = 100;
                else
                  activeArp.repMod += 10;
              }
              if(counterA <= -1){
                if(activeArp.repMod>10)
                  activeArp.repMod-=10;
                else
                  activeArp.repMod = 0;
              }
            }
            else{
              if(counterA >= 1 && activeArp.repMod<100){
                activeArp.repMod++;
              }
              else if(counterA <= -1 && activeArp.repMod>0){
                activeArp.repMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 3:
            if(!shift){
              if(counterA <= -1){
                if(activeArp.minPitchMod>117)
                  activeArp.minPitchMod = 127;
                else
                  activeArp.minPitchMod += 10;
              }
              if(counterA >= 1){
                if(activeArp.minPitchMod>10)
                  activeArp.minPitchMod-=10;
                else
                  activeArp.minPitchMod = 0;
              }
            }
            else{
              if(counterA <= -1 && activeArp.minPitchMod<127){
                activeArp.minPitchMod++;
              }
              else if(counterA >= 1 && activeArp.minPitchMod>0){
                activeArp.minPitchMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 4:
            if(counterA <= -1){
              if(activeArp.playStyle == 0)
                activeArp.playStyle = 5;
              else
                activeArp.playStyle--;
            }
            else{
              if(activeArp.playStyle == 5)
                activeArp.playStyle = 0;
              else
                activeArp.playStyle++;
            }
            counterA += counterA<0?1:-1;;
            break;
        }
      }
      while(counterB != 0){
        switch(cursor){
          case 0:
            if(!shift){
              if(counterB >= 1){
                if(activeArp.maxVelMod>117)
                  activeArp.maxVelMod = 127;
                else
                  activeArp.maxVelMod += 10;
              }
              if(counterB <= -1){
                if(activeArp.maxVelMod>10)
                  activeArp.maxVelMod-=10;
                else
                  activeArp.maxVelMod = 0;
              }
            }
            else{
              if(counterB >= 1 && activeArp.maxVelMod<127){
                activeArp.maxVelMod++;
              }
              else if(counterB <= -1 && activeArp.maxVelMod>0){
                activeArp.maxVelMod--;
              }
            }
            counterB += counterB<0?1:-1;;
            break;
          case 1:
            break;
          case 2:
            break;
          case 3:
            if(!shift){
              if(counterB >= 1){
                if(activeArp.maxPitchMod>117)
                  activeArp.maxPitchMod = 127;
                else
                  activeArp.maxPitchMod += 10;
              }
              if(counterB <= -1){
                if(activeArp.maxPitchMod>10)
                  activeArp.maxPitchMod-=10;
                else
                  activeArp.maxPitchMod = 0;
              }
            }
            else{
              if(counterB >= 1 && activeArp.maxPitchMod<127){
                activeArp.maxPitchMod++;
              }
              else if(counterB <= -1 && activeArp.maxPitchMod>0){
                activeArp.maxPitchMod--;
              }
            }
            counterB += counterB<0?1:-1;;
            break;
        }
      }
    }
    display.clearDisplay();
    printArp_wiggly(66,3,"modulation",1);
    drawModBoxes(cursor);
    display.display();
  }
}
void drawArpModeIcon(uint8_t x1, uint8_t y1, uint8_t which,uint16_t c){
  // which%=6;
  display.drawBitmap(x1,y1,arpMode_icons[which],11,11,c);
}

//"notes" option adds notes from the scale onto whatever notes are playing
void arpMenu(){
  // keyboardAnimation(38,14,0,14,true);
  int xStart = 10;
  int yStart = 32;
  int spacing = 4;
  int width = screenWidth/8-spacing-2;
  //step boxes
  int noteStart = 0;
  int noteEnd = 7;
  uint8_t cursor = 0;
  while(true){
    display.clearDisplay();
    //last note played
    String lastNote = pitchToString(activeArp.lastPitchSent,true,true);
    printTrackPitch(115,0,lastNote,false,false, 1);
    if(activeArp.holding && millis()%800>400){
      printSmall(106,9,"[HOLD]",1);
    }
    const uint8_t y1 = 15;
    //on/off
    drawSlider(0,y1,"on","off",!isArping);

    //step lengths
    String stepLength = activeArp.uniformLength?stepsToMeasures(activeArp.arpSubDiv):"custom";
    drawSlider(0,y1+20,"custom","uniform",activeArp.uniformLength);
    printSmall(0,y1+14,"length:" + stepLength,1);

    //modulation
    printArp(0,52,"MOD",1);

    fillSquareVertically(20,50,11,float(activeArp.maxVelMod*100)/float(127));
    printSmall(24,53,"v",2);

    fillSquareVertically(33,50,11,float(activeArp.chanceMod));
    printSmall(37,53,"%",2);

    fillSquareVertically(46,50,11,float(activeArp.repMod*100)/float(127));
    printSmall(50,53,"x",2);

    fillSquareVertically(59,50,11,float(activeArp.maxPitchMod*100)/float(127));
    printSmall(63,53,"$",2);

    //arp mode icon
    drawArpModeIcon(72,50,activeArp.playStyle,1);

    switch(cursor){
      case 0:
        drawArrow(29+sin(millis()/200),y1+5,3,1,false);
        break;
      case 1:
        drawArrow(61+sin(millis()/200),y1+25,3,1,false);
        break;
      case 2:
        drawArrow(82+sin(millis()/200),y1+40,3,1,false);
        break;
      // case 3:
      //   drawArrow(108+sin(millis()/200),y1+17,3,0,false);
      //   break;
    }
    //channel icon
    drawSmallChannelIcon(93,1,activeArp.channel);
    // display.drawBitmap(93,1,ch_tiny,6,3,SSD1306_WHITE);
    // printSmall(100,1,String(activeArp.channel),1);

    //input icon
    // printSmall(50,1,"input:"+(activeArp.getNotesFromExternalInput?String("ex"):String("seq")),1);
    printSmall(50,1,"input:",1);
    display.drawBitmap(76,0,activeArp.getNotesFromExternalInput?tiny_midi_bmp:tiny_stepchild_bmp,7,7,1);

    drawKeys(38,14,getOctave(keyboardPitch),14,false);//always start on a C, for simplicity
    //title
    display.drawBitmap(0,3+3*sin(float(millis())/float(200)),arpTitle[0],5,7,1);
    display.drawBitmap(6,3+3*sin(float(millis())/float(200)+200),arpTitle[1],5,7,1);
    display.drawBitmap(12,3+3*sin(float(millis())/float(200)+400),arpTitle[2],5,7,1);
    display.drawBitmap(18,3+3*sin(float(millis())/float(200)+600),arpTitle[3],5,7,1);
    display.drawBitmap(24,3+3*sin(float(millis())/float(200)+800),arpTitle[4],5,7,1);
    display.drawBitmap(30,3+3*sin(float(millis())/float(200)+1000),arpTitle[5],5,7,1);
    display.drawBitmap(36,3+3*sin(float(millis())/float(200)+1200),arpTitle[6],5,7,1);
    // display.drawBitmap(42,3+3*sin(float(millis())/float(200)+1400),arpTitle[7],5,7,1);
    display.display();

    //controls
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(y != 0){
        if(y == -1 && cursor>0){
          //jump from arp mode button up to length
          // if(cursor == 3){
          //   cursor = 1;
          // }
          // else{
            cursor--;
          // }
          lastTime = millis();
        }
        else if(y == 1 && cursor<2){
          cursor++;
          lastTime = millis();
        }
      }
      if(x != 0){
        switch(cursor){
          //on/off
          case 0:
            if(x == -1 && isArping){
              isArping = false;
              lastTime = millis();
            }
            else if(x == 1 && !isArping){
              isArping = true;
              lastTime = millis();
            }
            break;
          //step lengths
          case 1:
            if(x == 1 && activeArp.uniformLength){
              activeArp.uniformLength = false;
              lastTime = millis();
            }
            else if(x == -1 && !activeArp.uniformLength){
              activeArp.uniformLength = true;
              lastTime = millis();
            }
            break;
          case 2:
            if(x == -1){
              // cursor = 3;
              lastTime = millis();
            }
            break;
          case 3:
            if(x == 1){
              cursor = 2;
              lastTime = millis();
            }
            break;
        }
      }
      if(sel){
        switch(cursor){
          //on/off
          case 0:
            isArping = !isArping;
            lastTime = millis();
            break;
          //step lengths
          case 1:
            lastTime = millis();
            customLengthsMenu();
            break;
          case 2:
            lastTime = millis();
            arpModMenu();
            break;
          case 3:
            // activeArp.playStyle++;
            // activeArp.playStyle%=6;
            // lastTime = millis();
            break;
        }
      }
      //'hold' locks notes that are currently in the arp
      if(copy_Press){
        activeArp.holding = !activeArp.holding;
        lastTime = millis();
      }
      //swap between external/internal input
      if(play){
        activeArp.getNotesFromExternalInput = !activeArp.getNotesFromExternalInput;
        lastTime = millis();
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      while(counterA != 0){
        switch(cursor){
          case 0:
            isArping = !isArping;
            counterA += counterA<0?1:-1;;
            break;
          case 1:
            if(shift){
              activeArp.arpSubDiv = toggleTriplets(activeArp.arpSubDiv);
              counterA += counterA<0?1:-1;;
            }
            else{
              if(counterA >= 1){
                activeArp.arpSubDiv = changeSubDiv(true,activeArp.arpSubDiv,false);
                counterA += counterA<0?1:-1;;
              }
              else{
                activeArp.arpSubDiv = changeSubDiv(false,activeArp.arpSubDiv,false);
                counterA += counterA<0?1:-1;;
              }
            }
            break;
          case 2:
            if(counterA >= 1){
              activeArp.playStyle++;
              if(activeArp.playStyle == 6)
                activeArp.playStyle = 0;
            }
            else if(counterA <= -1){
              if(activeArp.playStyle == 0)
                activeArp.playStyle = 5;
              else
                activeArp.playStyle--;
            }
            counterA += counterA<0?1:-1;;
            break;
        }
      }
      while(counterB != 0){
        if(counterB <= -1 && activeArp.channel>1){
          activeArp.channel--;
        }
        else if(counterB >= 1 && activeArp.channel<16){
          activeArp.channel++;
        }
        lastTime = millis();
        counterB += counterB<0?1:-1;;
      }
    }
  }
}
void drawFullKeyBed(vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
  drawFullKeyBed(0,pressList,mask,activeKey,octave);
}
void drawFullKeyBed(uint8_t y1, vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
  //white keys
  const uint8_t keyWidth = 5;
  const uint8_t wKeyHeight = 13;
  const uint8_t wKeyPattern[21] = {0, 2, 4, 5, 7,9 ,11,
                                  12,14,16,17,19,21,23,
                                  24,26,28,29,31,33,35};
  //black keys
  const uint8_t bKeyHeight = 8;
  const uint8_t bKeyPattern[15] = {1, 3, 6, 8,10,
                                  13,15,18,20,22,
                                  25,27,30,32,34};
  String text = pitchToString(activeKey,false,true);
  //these 'patterns' help reference the actual note value from the iterator i'm using to draw them
  //the reason i'm doing this in realtime, not with a bitmap, is so i can animate
  //keypresses and (maybe) even change which keys are displayed at all    
  //first draw white keys
  for(uint8_t i = 0; i<21; i++){
    //if there's no mask, or if the key is in the mask
    if(mask.size() == 0 || isInVector(wKeyPattern[i],mask)){
      //if it's pressed, draw it blinking
      if(isInVector(wKeyPattern[i]+12*octave,pressList)){
        if(millis()%800>400)
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        else
          display.fillRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey){
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
          drawArrow(i*(keyWidth+1)+2,y1+17+sin(millis()/100),3,2,true);
          printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+sin(millis()/100),text,SSD1306_WHITE);
        }
        display.drawPixel(i*(keyWidth+1)+2,y1+15,SSD1306_WHITE);
      }
      else{
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey){
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
          drawArrow(i*(keyWidth+1)+2,y1+17+sin(millis()/100),3,2,true);
          printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+sin(millis()/100),text,SSD1306_WHITE);
        }
        else
          display.fillRect(i*(keyWidth+1),y1,keyWidth,wKeyHeight,SSD1306_WHITE);
      }
    }
  }
  //then draw black keys
  uint8_t xPos = 3;
  for(uint8_t i = 0; i<15; i++){
    if(mask.size() == 0 || isInVector(bKeyPattern[i]+12*octave,mask)){
      if(isInVector(bKeyPattern[i]+12*octave,pressList) || bKeyPattern[i]+12*octave == activeKey){
        display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
        //if it's pressed
        if(isInVector(bKeyPattern[i]+12*octave,pressList)){
          if(millis()%800>400){
            display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
          }
          display.drawPixel(xPos+2,y1+15,SSD1306_WHITE);
        }
        else
          display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
        display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
        if(bKeyPattern[i]+12*octave == activeKey){
          drawArrow(xPos+2,y1+17+sin(millis()/100),3,2,false);
          printSmall(xPos+2-text.length()*2,y1+22+sin(millis()/100),text,SSD1306_WHITE);
        }
      }
      else{
        display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
        display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
      }
    }
    //if it's a D# or a Bb, you're about to jump
    if(abs(bKeyPattern[i]%12) == 3 || abs(bKeyPattern[i]%12) == 10)
      xPos+= 2*(keyWidth+1); 
    //if it's not, just increment like normal
    else
      xPos+= 1+keyWidth;
  }
}

//this generator makes a chord from a scale (or all twelve keys), and can be specified to include notes
//"mask" is the scale and "definiteKeys" are the keys that will be in the chord
vector<uint8_t> genRandomChord(vector<uint8_t> mask,vector<uint8_t> definiteKeys, uint8_t numberOfNotes,uint8_t octave){
  //if mask is empty, just make it all the notes
  if(mask.size() == 0){
    for(uint8_t i = 0; i<36; i++){
      mask.push_back(i+12*octave);
    }
  }
  //notes starts off already including definiteKeys
  vector<uint8_t> notes = definiteKeys;
  for(uint8_t i = notes.size(); i<numberOfNotes; i++){
    uint8_t note = mask[random(0,mask.size())];
    //if this note is already present, keep genning new notes
    while(isInVector(note,notes)){
      note = mask[random(0,mask.size())];
      //if all the notes are already in the mask
      if(notes.size() == mask.size()){
        break;
      }
    }
    notes.push_back(note);
  }
  return notes;
}

//chord builder
void chordBuilder(){
  Progression chordSequence;
  uint8_t activeChord = 0;
  uint8_t keyCursor = 0;
  vector<uint8_t> pressedKeys;
  vector<uint8_t> mask;
  uint16_t length = 24;
  uint8_t octave = 0;
  uint8_t maskIndex = 0;
  //0 = selecting notes for a new chord
  //1 = selecting a chord to edit
  //2 = editing a chord
  uint8_t editorState = 0;
  while(true){
    joyRead();
    readButtons();
    while(counterA != 0){
      uint16_t *lengthPointer;
      //switch so you can edit the new length val, or the length of an existing chord
      if(editorState == 1){
        lengthPointer = &(chordSequence.chords[activeChord].length);
      }
      else{
        lengthPointer = &length;
      }
      //changing length
      if(!shift){
        if(counterA >= 1 && (*lengthPointer)<96){
          (*lengthPointer)+=subDivInt;
          if((*lengthPointer)>96){
            (*lengthPointer) = 96;
          }
          counterA += counterA<0?1:-1;;
        }
        if(counterA <= -1 && (*lengthPointer)>subDivInt){
          (*lengthPointer)-=subDivInt;
          if((*lengthPointer)<subDivInt){
            (*lengthPointer) = subDivInt;
          }
          counterA += counterA<0?1:-1;;  
        }
      }
    }
    while(counterB != 0){
      if(!shift){   
        if(counterB >= 1){
          changeSubDivInt(true);
          counterB += counterB<0?1:-1;;
        }
        //changing subdivint
        if(counterB <= -1){
          changeSubDivInt(false);
          counterB += counterB<0?1:-1;;
        }
      }
    }
    if(itsbeen(150)){
      if(y != 0){
        lastTime = millis();
        if(y == 1 && editorState == 0 && chordSequence.chords.size()>0){
          activeChord = 0;
          editorState = 1;
        }
        else if(y == -1 && editorState == 1){
          editorState = 0;
        }
      }
      if(x != 0){
        //moving between chords in the chord sequence
        if(editorState == 1){
          if(chordSequence.chords.size()>0){
            if(x == -1 && activeChord<(chordSequence.chords.size()-1)){
              activeChord++;
              lastTime = millis();
            }
            else if(x == 1 && activeChord>0){
              activeChord--;
              lastTime = millis();
            }
          }
        }
        else if(editorState == 0 || editorState == 2){
          //if shift, move by an octave
          if(shift){
            if(x == -1 &&keyCursor<=24){
              keyCursor+=12;
              lastTime = millis();
            }
            else if(x == 1 && keyCursor>=12){
              keyCursor-=12;
              lastTime = millis();
            }
          }
          //if it's masked, it moves to the next masked note
          if(mask.size()>0){
            if(x == -1 && maskIndex<mask.size()-1){
              keyCursor = mask[maskIndex++];
              lastTime = millis();
            }
            else if(x == 1 && maskIndex>0){
              keyCursor = mask[maskIndex--];
              lastTime = millis();
            }
            if(keyCursor<0)
              keyCursor = mask[maskIndex++];
            if(keyCursor>=36)
              keyCursor = mask[maskIndex--];
          }
          else{
            if(x == -1 && keyCursor<35){
              keyCursor++;
              lastTime = millis();
            }
            else if(x == 1 && keyCursor>0){
              keyCursor--;
              lastTime = millis();
            }
          }
        }
      }
    }
    if(itsbeen(200)){
      if(loop_Press){
        lastTime = millis();
        //gen a random sequence
        if(shift){
          //always adds 4 random notes to the selection
          pressedKeys = genRandomChord(mask,pressedKeys,pressedKeys.size()+4,octave);
        }
        else{
          //if there's no mask, make one
          //(mask uses highlit note as root)
          if(mask.size() == 0){
            //if it's within the first octave
            // if(keyCursor/12 == 0)
              mask = genScale(MAJOR,keyCursor%12,3,octave);
          }
          //if there is, unmake one
          else{
            vector<uint8_t> temp;
            mask.swap(temp);
          }
        }
      }
      //selecting/deselecting notes
      if(sel){
        //making new chord
        if(editorState == 0 || editorState == 2){
          lastTime = millis();
          //deselect all notes
          if(shift){
            vector<uint8_t> temp;
            pressedKeys.swap(temp);
          }
          else{
            //if the key is already selected, remove it
            if(isInVector(keyCursor,pressedKeys)){
              vector<uint8_t> temp;
              //removing keys from pressedKeys;
              for(uint8_t i = 0; i<pressedKeys.size(); i++){
                if(pressedKeys[i] != keyCursor){
                  temp.push_back(pressedKeys[i]);
                }
              }
              pressedKeys.swap(temp);
            }
            else{
              pressedKeys.push_back(keyCursor);
            }
          }
          //setting the active chord intervals to the newly pressed keys if you're in edit mode
          if(editorState == 2){
            uint8_t root = getLowestVal(pressedKeys)+12*octave;
            vector<uint8_t> intervals;
            for(uint8_t i = 0; i<pressedKeys.size(); i++){
              intervals.push_back(pressedKeys[i]-root);
            }
            chordSequence.chords[activeChord].root = root;
            chordSequence.chords[activeChord].intervals = intervals;
          }
        }
        //transitioning to edit chord mode
        else if(editorState == 1){
          lastTime = millis();
          editorState = 2;
          //swap pressed keys for the chord intervals
          vector<uint8_t> tempNotes;
          for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
            tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
          }
          pressedKeys.swap(tempNotes);
        }
      }
      //making a chord
      if(n){
        //new chord mode
        if(editorState == 0){
          lastTime = millis();
          //commit chord
          if(shift){
            chordSequence.commit();
            menuIsActive = false;
            constructMenu("MENU");
            return;
          }
          else{
            if(pressedKeys.size()>0){
              uint8_t root = getLowestVal(pressedKeys)+12*octave;
              vector<uint8_t> intervals;
              for(uint8_t i = 0; i<pressedKeys.size(); i++){
                intervals.push_back(pressedKeys[i]-root);
              }
              Chord newChord = Chord(root,intervals,length);
              chordSequence.add(newChord);
              vector<uint8_t> temp;
              pressedKeys.swap(temp);
            }
          }
        }
        //edit chord mode (commits keys to chord)
        if(editorState == 2){
          lastTime = millis();
          if(pressedKeys.size()>0){
            uint8_t root = getLowestVal(pressedKeys)+12*octave;
            vector<uint8_t> intervals;
            for(uint8_t i = 0; i<pressedKeys.size(); i++){
              intervals.push_back(pressedKeys[i]-root);
            }
            //setting chord intervals to the new, edited intervals
            chordSequence.chords[activeChord].intervals = intervals;
            vector<uint8_t> temp;
            pressedKeys.swap(temp);
            editorState = 0;
          }
          else{
            chordSequence.remove(activeChord);
            if(chordSequence.chords.size() == 0)
              activeChord = 0;
            else if(activeChord>=chordSequence.chords.size())
              activeChord = chordSequence.chords.size()-1;
            editorState = 0;
          }
        }
      }
      if(del){
        if(editorState == 1){
          chordSequence.remove(activeChord);
          lastTime = millis();
          //if there're no chords left, set activeC to 0
          //else, lower it by one
          chordSequence.chords.size() > 0 ? activeChord-- : activeChord = 0;
        }
      }
      if(menu_Press){
        lastTime = millis();
        if(editorState == 0 || editorState == 1){
          return;
        }
        else if(editorState == 2){
          editorState = 0;
        }
      }
    }
    display.clearDisplay();
    //normal, make-new-chord mode
    if(editorState == 0){
      drawFullKeyBed(pressedKeys,mask,keyCursor,octave);
      chordSequence.drawProg(32, 50, -1);

      //if you're not editing a chord, display length var and subDiv
      //making nice boxes
      //top
      display.drawRoundRect(101,33,13,12,3,SSD1306_WHITE);
      //bottom
      display.drawRoundRect(101,47,13,11,3,SSD1306_WHITE);
      //side
      display.fillRoundRect(111,30,21,30,3,SSD1306_BLACK);
      display.drawRoundRect(111,30,21,30,3,SSD1306_WHITE);

      //display subDivInt
      String text = stepsToMeasures(subDivInt);
      printSmall(103,37,"~",SSD1306_WHITE);
      printFractionCentered(121,36,stepsToMeasures(subDivInt));
      
      //length
      text = stepsToMeasures(length);
      printSmall(105,50,"L",SSD1306_WHITE);
      printFractionCentered(121,49,stepsToMeasures(length));
      drawCenteredBanner(64,55,getPitchList(pressedKeys));
    }
    else if(editorState == 1){
      //draw the keys that are in the chord as "selected"
      //send a temp vector that's adjusted so the notes display correctly
      vector<uint8_t> tempNotes;
      for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
        tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
      }
      drawFullKeyBed(tempNotes,mask,keyCursor,0);
      chordSequence.drawProg(32, 50, activeChord);
      //if you are editing a chord, display it's length
      printFraction(115,32,stepsToMeasures(chordSequence.chords[activeChord].length));
    }
    else if(editorState == 2){
      vector<uint8_t> tempNotes;
      // for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
      //   tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
      // }
      drawFullKeyBed(pressedKeys,mask,keyCursor,0);
      //draw a second keybed lower, JUST showing the notes that are in-key
      drawFullKeyBed(32,tempNotes,pressedKeys,255,0);
      drawCenteredBanner(64,55,chordSequence.chords[activeChord].getPitchList(0,59));
    }
    //if there's a mask
    if(mask.size()>0){
      drawCenteredBanner(64,55,pitchToString(mask[0],false,true)+" major");
    }
    display.display();
  }
}
String getPitchList(vector<uint8_t> intervals){
  String text;
  for(uint8_t i = 0; i<intervals.size(); i++){
    text += pitchToString(intervals[i],false,true);
    if(i != intervals.size()-1)
      text += ",";
  }
  return text;
}
void drawEcho(unsigned short int xStart, unsigned short int yStart, short unsigned int time, short unsigned int decay, short unsigned int repeats){
  unsigned short int previewLength = 12;
  // drawNote(xStart,yStart,previewLength,trackHeight,1,false,false);//pilot note
  float vel = 127;
  for(int rep = 1; rep<repeats+1; rep++){
    vel = vel * decay/100;
    short unsigned int x1 = xStart+time*rep;
    short unsigned int y1 = yStart;
    // drawNote_vel(x1,y1,previewLength,trackHeight,vel,false,false);
  }
}

void echoMenu(){
  animOffset = 0;
  // echoAnimation();
  while(true){
    readButtons();
    joyRead();
    if(!echoMenuControls()){
      return;
    }
    display.clearDisplay();
    activeMenu.displayEchoMenu();
    display.display();
  }
}

bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
  while(true){
    joyRead();
    readButtons();
    defaultEncoderControls();
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      selBox.begun = false;
      selectBox();
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        return true;
      }
      if(menu_Press){
        clearSelection();
        lastTime = millis();
        return false;
      }
      if(sel){
        if(shift){
          clearSelection();
          toggleSelectNote(activeTrack,getIDAtCursor(),false);
        }
        else{
          toggleSelectNote(activeTrack,getIDAtCursor(),true);
        }
        lastTime = millis();
      }
    }
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
        }
      }
      if (y == 1) {
        if(recording)
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, true);
        lastTime = millis();
      }
      if (y == -1) {
        if(recording)
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
        lastTime = millis();
      }
    }
    if (itsbeen(50)) {
      if (x == 1 && shift) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (x == -1 && shift) {
        moveCursor(1);
        lastTime = millis();
      }
    }
    display.clearDisplay();
    drawSeq(true, false, true, false, false, viewStart, viewEnd);
    if(!selectionCount){
      printSmall(trackDisplay,0,"select notes to "+text+"!",1);
    }
    else{
      printSmall(trackDisplay,0,stringify(selectionCount)+" selected - [n] to "+text,1);
    }
    iconFunction(7,1,14,true);
    display.display();
  }
}

void echoSelectedNotes(){
  //if no notes are selected, just return
  if(!selectionCount){
    return;
  }
  for(uint8_t track = 0; track<trackData.size(); track++){
    for(uint8_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        echoNote(track,note);
      }
    }
  }
  clearSelection();
}

bool echoMenuControls_menuless(uint8_t* cursor){
  if(itsbeen(200)){
    if(x == 1 && (*cursor) > 0){
      (*cursor)--;
      lastTime = millis();
    }
    if(x == -1 && (*cursor) < 2){
      (*cursor)++;
      lastTime = millis();
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      menu_Press = false;
      return false;
    }
    if(n){
      n = false;
      lastTime = millis();
      int totalNotes = 0;
      for(int track = 0; track<trackData.size(); track++){
        totalNotes += seqData[track].size()-1;
      }
      if(totalNotes != 0){
        lastTime = millis();
        vector<String> ops = {"Specific Notes","Specific Tracks","Entire Sequence"};
        int choice = vertSelectionBox(ops,10,16,100,38);
        if(choice == 0){
          vector<vector<uint8_t>> notes = selectMultipleNotes("[SEL] notes to echo","[N] To commit,[SH+N] to echo all");
          int8_t choice = binarySelectionBox(64,32,"COMMIT?","DON'T");
          for(int track = 0; track<notes.size(); track++){
            for(int note = 0; note<notes[track].size(); note++){
              echoNote(track, notes[track][note]);
            }
          }
          return false;
        }
        else if(choice == 1){
          menuIsActive = false;
          vector<uint8_t> tracks = selectMultipleTracks("echo which?");
          for(int i = 0; i<tracks.size(); i++){
            if(seqData[i].size()-1>0)
              echoTrack(tracks[i]);
          }
          return false;
        }
        else if(choice == 2){
          for(int i = 0; i<trackData.size(); i++){
            if(seqData[i].size()-1>0){
              echoTrack(i);
            }
          }
          return false;
        }
      }
    }
  }
  while(counterA != 0){//if there's data for this option
    if(counterA >= 1){
      if((*cursor) == 0 && echoData[0]<96){
        if(shift)
          echoData[0]++;
        else
          echoData[0]*=2;
        if(echoData[0]>96)
          echoData[0] = 96;
      }
      else if((*cursor) == 1)
        echoData[2]++;
      else if((*cursor) == 2){
        if(shift && echoData[1]<100)
          echoData[1]++;
        else if(echoData[1]<=90)
          echoData[1]+=10;
      }
    }
    else if(counterA <= -1){
      if((*cursor) == 0){
        if(shift && echoData[0]>0)
          echoData[0]--;
        else if(echoData[0]>=2)
          echoData[0]/=2;
      }
      else if((*cursor) == 1 &&  echoData[2] > 1)
        echoData[2]--;
      else if((*cursor) == 2){
        if(shift && echoData[1]>2)
          echoData[1]--;
        else if(echoData[1]>=11)
          echoData[1]-=10;
      }
    }
    counterA += counterA<0?1:-1;;
  }
  return true;
}
void echoMenu_menuless(){
  animOffset = 0;
  uint8_t cursor = 0;
  while(true){
    readButtons();
    joyRead();
    if(!echoMenuControls_menuless(&cursor)){
      return;
    }
    display.clearDisplay();
    drawEchoMenu(cursor);
    display.display();
  }
}
void drawEchoMenu(uint8_t cursor){
  int xCoord = 64;
  int yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  // spacing  = float(echoData[0]*10)/float(24);
  int spacing = echoData[0];
  int maxReps = echoData[2];
  display.clearDisplay();

  int8_t triOffset = 2*sin(millis()/200);
  switch(cursor){
    case 0:{
      display.drawBitmap(52,17-triOffset,time_bmp,25,6,SSD1306_WHITE);
      display.drawBitmap(52,41+triOffset,time_inverse_bmp,25,6,SSD1306_WHITE);
      display.drawFastHLine(52,42,24,SSD1306_BLACK);
      display.drawFastHLine(52,44,24,SSD1306_BLACK);
      display.drawFastHLine(52,46,24,SSD1306_BLACK);

      //time
      String text = stepsToMeasures(echoData[0]);
      printFraction_small_centered(64,29,text);

      //arrows
      drawArrow(78-sin(millis()/200),31,3,0,false);
      }
      break;
    case 1:{
      display.drawBitmap(29,17-triOffset,repetitions_bmp,66,11,SSD1306_WHITE);
      display.drawBitmap(29,41+triOffset,repetitions_inverse_bmp,66,11,SSD1306_WHITE);
      display.drawFastHLine(29,42,66,SSD1306_BLACK);
      display.drawFastHLine(29,44,66,SSD1306_BLACK);
      display.drawFastHLine(29,46,66,SSD1306_BLACK);
      printSmall(64-stringify(echoData[2]).length()*2,29,stringify(echoData[2]),SSD1306_WHITE);
      
      drawArrow(78-sin(millis()/200),31,3,0,false);
      drawArrow(50+sin(millis()/200),31,3,1,false);
      }
      break;
    case 2:{
      display.drawBitmap(49,17-triOffset,decay_bmp,30,12,SSD1306_WHITE);
      display.drawBitmap(49,41+triOffset,decay_inverse_bmp,30,12,SSD1306_WHITE);
      display.drawFastHLine(49,42,30,SSD1306_BLACK);
      display.drawFastHLine(49,44,30,SSD1306_BLACK);
      display.drawFastHLine(49,46,30,SSD1306_BLACK);
      printSmall(64-(stringify(echoData[1]).length()+1)*2,29,stringify(echoData[1])+"%",SSD1306_WHITE);
      
      drawArrow(50+sin(millis()/200),31,3,1,false);
      }
      break;
  }

  if(animOffset<=32){
    if(animOffset>8){//drops and reflection
      display.drawCircle(xCoord, animOffset-8, 1+sin(animOffset), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-(animOffset-8), 1+sin(animOffset), SSD1306_WHITE);
    }
    display.drawCircle(xCoord, animOffset, 3+sin(animOffset), SSD1306_WHITE);
    display.drawCircle(xCoord, screenHeight-animOffset, 3+sin(animOffset), SSD1306_WHITE);
  }
  else if(animOffset>yCoord){
    int reps = (animOffset-yCoord)/spacing+1;
    if(reps>maxReps){
      reps = maxReps;
    }
    for(int i = 0; i<reps; i++){
      if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(screenWidth+16))
        drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
    }
  }
  if(animOffset<yCoord){
    animOffset+=5;
  }
  else
    animOffset+=6;
  if(animOffset>=8*spacing*maxReps/3+8*32+20){
    animOffset = 0;
  }
}
void Menu::displayEchoMenu(){
  int xCoord = 64;
  int yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  // spacing  = float(echoData[0]*10)/float(24);
  int spacing = echoData[0];
  int maxReps = echoData[2];
  display.clearDisplay();

  int8_t triOffset = 2*sin(millis()/200);

  switch(highlight){
    case 0:{
      display.drawBitmap(52,17-triOffset,time_bmp,25,6,SSD1306_WHITE);
      display.drawBitmap(52,41+triOffset,time_inverse_bmp,25,6,SSD1306_WHITE);
      display.drawFastHLine(52,42,24,SSD1306_BLACK);
      display.drawFastHLine(52,44,24,SSD1306_BLACK);
      display.drawFastHLine(52,46,24,SSD1306_BLACK);

      //time
      String text = stepsToMeasures(echoData[0]);
      printFraction_small_centered(64,29,text);

      //arrows
      drawArrow(78-sin(millis()/200),31,3,0,false);
      }
      break;
    case 1:{
      display.drawBitmap(29,17-triOffset,repetitions_bmp,66,11,SSD1306_WHITE);
      display.drawBitmap(29,36+triOffset,repetitions_inverse_bmp,66,11,SSD1306_WHITE);
      display.drawFastHLine(29,42,66,SSD1306_BLACK);
      display.drawFastHLine(29,44,66,SSD1306_BLACK);
      display.drawFastHLine(29,46,66,SSD1306_BLACK);
      printSmall(64-stringify(echoData[2]).length()*2,29,stringify(echoData[2]),SSD1306_WHITE);
      
      drawArrow(78-sin(millis()/200),31,3,0,false);
      drawArrow(50+sin(millis()/200),31,3,1,false);
      }
      break;
    case 2:{
      display.drawBitmap(49,17-triOffset,decay_bmp,30,12,SSD1306_WHITE);
      display.drawBitmap(49,35+triOffset,decay_inverse_bmp,30,12,SSD1306_WHITE);
      display.drawFastHLine(49,42,30,SSD1306_BLACK);
      display.drawFastHLine(49,44,30,SSD1306_BLACK);
      display.drawFastHLine(49,46,30,SSD1306_BLACK);
      printSmall(64-(stringify(echoData[1]).length()+1)*2,29,stringify(echoData[1])+"%",SSD1306_WHITE);
      
      drawArrow(50+sin(millis()/200),31,3,1,false);
      }
      break;
  }

  if(animOffset<=32){
    if(animOffset>8){//drops and reflection
      display.drawCircle(xCoord, animOffset-8, 1+sin(animOffset), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-(animOffset-8), 1+sin(animOffset), SSD1306_WHITE);
    }
    display.drawCircle(xCoord, animOffset, 3+sin(animOffset), SSD1306_WHITE);
    display.drawCircle(xCoord, screenHeight-animOffset, 3+sin(animOffset), SSD1306_WHITE);
  }
  else if(animOffset>yCoord){
    int reps = (animOffset-yCoord)/spacing+1;
    if(reps>maxReps){
      reps = maxReps;
    }
    for(int i = 0; i<reps; i++){
      if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(screenWidth+16))
        drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
    }
  }
  if(animOffset<yCoord){
    animOffset+=5;
  }
  else
    animOffset+=6;
  if(animOffset>=8*spacing*maxReps/3+8*32+20){
    animOffset = 0;
  }
}

void printCursive_reflected(int8_t x1, int8_t y1,int8_t gap,String text){
  printCursive(x1,y1,text,SSD1306_WHITE);
  display.setRotation(UPSIDEDOWN);
  printCursive(screenWidth-x1-text.length()*6,screenHeight-y1-2*gap-18,text,SSD1306_WHITE);
  display.drawFastHLine(screenWidth-x1-text.length()*6,screenHeight-y1-2*gap-17,text.length()*6,SSD1306_BLACK);
  display.drawFastHLine(screenWidth-x1-text.length()*6,screenHeight-y1-2*gap-15,text.length()*6,SSD1306_BLACK);
  display.setRotation(UPRIGHT);
}

uint16_t getLookupID(uint8_t track, uint16_t pos){
  return lookupData[track][pos];
}

void filesMenu(){
  uint8_t menuStart = 0;
  uint8_t menuEnd = 4;
  clearButtons();
  WireFrame folder = makeFolder(30);
  folder.scale = 3;
  folder.xPos = 96;
  folder.yPos = screenHeight/2;
  while(menuIsActive){
    display.clearDisplay();
    //draw menu
    activeMenu.displayFilesMenu(0,false,menuStart,menuEnd);
    //render wireframe
    folder.render();
    //draw mini menu
    if(activeMenu.page!=0)
      displayMiniMenu();
    display.display();
    folder.rotate(1,1);
    readButtons();
    joyRead();
    if(activeMenu.page == 0){
      if(!fileMenuControls(menuStart,menuEnd,&folder)){
        constructMenu("MENU");
        return;
      }
        //menu data shit
      if(activeMenu.highlight>menuEnd){
        menuEnd = activeMenu.highlight;
        menuStart = menuEnd-4;
      }
      else if(activeMenu.highlight<menuStart){
        menuStart = activeMenu.highlight;
        menuEnd = menuStart+4;
      }
    }
    else{
      fileMenuControls_miniMenu(&folder);
    }
  }
}

void displayMiniMenu(){
  const uint8_t x1 = 66;
  const uint8_t y1 = 9;
  const vector<String> options = {"Load","Overwrite","Rename","Export","Delete","Back"};

  //mask for back arrow
  display.fillRoundRect(x1,y1+39,19,16,3,SSD1306_BLACK);
  display.drawRoundRect(x1,y1+39,19,16,3,SSD1306_WHITE);

  //menubox
  display.fillRoundRect(x1,y1,44,43,3,SSD1306_BLACK);
  display.drawRoundRect(x1,y1,44,43,3,SSD1306_WHITE);

  //mask (again)
  display.drawFastHLine(x1+1,y1+42,17,SSD1306_BLACK);
  display.drawPixel(x1+1,y1+41,SSD1306_BLACK);
  if(activeMenu.highlight == 5)
    display.drawBitmap(x1+2,y1+41,back_arrow_light_bmp,14,13,SSD1306_WHITE);
  else
    display.drawBitmap(x1+4,y1+43,back_arrow_bmp,10,9,SSD1306_WHITE);

  //if it's on an item other than the 'back' arrow
  for(uint8_t i = 0; i<5; i++){
    if(activeMenu.highlight == i){
      display.fillRoundRect(x1+2,y1+8*i+2,options[i].length()*4+4,7,2,SSD1306_WHITE);
    }
    printSmall(x1+4,y1+8*i+3,options[i],2);
  }
  printChunky(87-options[activeMenu.highlight].length()*3,y1-7,options[activeMenu.highlight],SSD1306_WHITE);
}

void Menu::displayFilesMenu(int16_t textOffset, bool open, uint8_t menuStart, uint8_t menuEnd){
  unsigned short int menuHeight = abs(bottomR[1]-topL[1]);
  display.setCursor(topL[0]+9,topL[1]+3);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Files");
  display.setFont();
  printSmall(topL[0]+9,topL[1]+10,"--------",SSD1306_WHITE);

  const uint8_t textWidth = 20;
  const uint8_t textHeight = 9;

  uint8_t yLoc = 0;

  //making sure it can't read options that don't exist
  if(options.size() == 0){
    printSmall(24,24,"No files, kid",SSD1306_WHITE);
    return;
  }
  if(menuEnd>=options.size()){
    menuEnd = options.size()-1;
  }
  //drawing indicator arrows
  if(menuStart>0){
    drawArrow(topL[0],topL[1]+13-sin(millis()/150),2,2,false);
  }
  if(menuEnd<options.size()-1){
    drawArrow(topL[0],topL[1]+60+sin(millis()/150),2,3,false);
  }
  //printing out the menu
  for(int i = menuStart; i<=menuEnd; i++){
    if(page == 0){
      if(highlight != i){
        printSmall(topL[0]+10,(yLoc+1)*textHeight+topL[1]+6,options[i],SSD1306_WHITE);
      }
      else if(highlight == i){
        drawBanner(topL[0]+16,(yLoc+1)*textHeight+topL[1]+6,options[i]);
      }
    }
    else{
      if(page != i){
        printSmall(topL[0]+10,(yLoc+1)*textHeight+topL[1]+6,options[i],SSD1306_WHITE);
      }
      else if(page == i){
        drawBanner(topL[0]+16,(yLoc+1)*textHeight+topL[1]+6,options[i]);
      }
    }
    yLoc++;
  }
}

#include "menus/track.cpp"
#include "menus/save.cpp"
#include "menus/settings.cpp"
#include "menus/quantize.cpp"
#include "menus/humanize.cpp"

void fillSquareVertically(uint8_t x0, uint8_t y0, uint8_t width, uint8_t fillAmount){
  display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  uint8_t maxLine = float(fillAmount)/float(100)*(width-4);
  for(uint8_t line = 0; line<maxLine; line++){
    display.drawFastHLine(x0+2,y0+width-3-line,width-4,1);
  }
}
//fill amount is a percent
void fillSquareDiagonally(uint8_t x0, uint8_t y0, uint8_t width,uint8_t fillAmount){
  display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  //fillAmount = lines/width
  uint8_t maxLine = float(fillAmount)/float(100)*width*sqrt(2);
  // if(fillAmount == 1)
  //   maxLine++;
  for(uint8_t line = 0; line<maxLine; line++){
    //bottom right
    int8_t x1 = x0+2+line;
    int8_t y1 = y0+width-3;
    if(x1>x0+width-2)
      x1=x0+width-2;

    //top left
    int8_t x2 = x0+2;
    int8_t y2 = y0+width-3-line;

    if(x1>=x0+width-2){
      x1 = x0+width-3;
      y1 = y0+width-3-(line-width+5);
    }
    if(y2<y0+2){
      y2 = y0+2;
      x2 = x0+2+(line-width+5);
    }
    if(y1<y0+2)
      y1 = y0+2;
    if(x2>x0+width-2)
      x2 = x0+width-2;
    // Serial.println("x1:"+stringify(x1));
    // Serial.println("y1:"+stringify(y1));
    // Serial.println("x2:"+stringify(x2));
    // Serial.println("y2:"+stringify(y2));
    display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
    // Serial.println("line:"+stringify(line));
    // Serial.println("w:"+stringify(width));
  }
}

void drawWormhole(){
  srand(10);
  float rotationOffset = humanizeParameters[0]*float(90)/float(100);
  int numSquares = 20;
  int xPos = 70;
  int yPos = 40;
  // int r = random(-humanizeParameters[1]/4,humanizeParameters[1]/4);
  for(int i = 0; i<numSquares; i++){
    uint8_t rand = random(0,100)*4;
    if(rand>humanizeParameters[2]){
      // drawRotatedRect(xPos + 2*i + (i>12 ? (i-12)*2:0), yPos - i*i/10 + sin(millis()/100+i) + (i>10 ? pow(i-10,3)/20:0)+humanizeParameters[1]*sin(4*i/(PI)),40-2*i,40-2*i,rotationOffset*i,SSD1306_WHITE);
      drawRotatedRect(xPos + 3*i, yPos -  2*i + sin(millis()/100+i) + humanizeParameters[1]*sin(4*i/(PI))/2,40-2*i,40-2*i,rotationOffset*i,SSD1306_WHITE);
    }
  }
}
vector<uint8_t> getTracksWithSelectedNotes(){
  vector<uint8_t> list;
  uint16_t count;
  if(selectionCount>0){
    for(uint8_t track = 0; track<trackData.size(); track++){
      for(uint16_t note = 1; note<seqData[track].size(); note++){
        //once you find a selected note, jump to the next track
        if(seqData[track][note].isSelected){
          list.push_back(track);
          track++;
          note = 1;
        }
      }
    }
  }
  return list;
}

vector<uint16_t> getSelectionBounds2(){
  //stored as xStart,yStart,xEnd,yEnd
  //(initially store it as dramatic as possible)
  vector<uint16_t> bounds = {seqEnd,(uint16_t)trackData.size(),0,0};
  uint16_t checkedNotes = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        //if it's the new highest track
        if(track<bounds[1])
          bounds[1] = track;
        //if it's the new lowest track
        if(track>bounds[3])
          bounds[3] = track;
        //if it's the new earliest note
        if(seqData[track][note].startPos<bounds[0])
          bounds[0] = seqData[track][note].startPos;
        //if it's the new latest note
        if(seqData[track][note].endPos>bounds[2])
          bounds[2] = seqData[track][note].endPos;

        //if you've checked all the selected notes, return
        checkedNotes++;
        if(checkedNotes == selectionCount)
          return bounds;
      }
    }
  }
  return bounds;
}
//returns two coordinate pairs that bound all selected notes
vector<vector<uint16_t>> getSelectionBounds(){
  uint16_t checkedNotes = 0;
  vector<vector<uint16_t>> bounds = {{seqEnd,uint16_t(trackData.size())},{0,0}};
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      if(seqData[track].size()-1>0){
        for(int note = 1; note<=seqData[track].size()-1; track++){
          if(seqData[track][note].isSelected){
            if(track<bounds[0][1])
              bounds[0][1] = track;
            if(track>bounds[1][1]);
              bounds[1][1] = track;
            //if this note is outside the bounds, embiggen em
            if(seqData[track][note].startPos<bounds[0][0]){
              bounds[0][0] = seqData[track][note].startPos;
            }
            if(seqData[track][note].endPos>bounds[1][0]){
              bounds[1][0] = seqData[track][note].endPos;
            }
          }
        }
      }
    }
  }
  return bounds;
}

#include "menus/edit.cpp"
#include "menus/clock.cpp"
#include "menus/main.cpp"

//prints pitch with a small # and either a large or small Octave number
void printTrackPitch(uint8_t xCoord, uint8_t yCoord, String pitch, bool bigOct, bool channel, uint16_t c){
  display.setCursor(xCoord,yCoord);
  display.print(pitch.charAt(0));
  // printChunky(xCoord,yCoord,stringify(pitch.charAt(0)),c);
  //if it's a sharp
  if(pitch.charAt(1) == '#'){
    printSmall(xCoord+6,yCoord,pitch.charAt(1),c);
    if(bigOct){
      display.setCursor(xCoord+12,yCoord);
      display.print(pitch.charAt(2));
      // printChunky(xCoord+12,yCoord,stringify(pitch.charAt(2)),c);
      if(pitch.charAt(2) == '-'){
        display.print(pitch.charAt(3));
      }
    }
    else{
      printSmall(xCoord+12,yCoord,pitch.charAt(2),c);
      if(pitch.charAt(2) == '-'){
        printSmall(xCoord+16,yCoord,pitch.charAt(3),c);
      }
    }
  }
  else{
    if(bigOct){
      display.setCursor(xCoord+6,yCoord);
      display.print(pitch.charAt(1));
      // printChunky(xCoord+6,yCoord,stringify(pitch.charAt(1)),c);
      if(pitch.charAt(1) == '-'){
        // printChunky(xCoord+10,yCoord,stringify(pitch.charAt(2)),c);
        display.print(pitch.charAt(2));
      }
    }
    else{
      printSmall(xCoord+6,yCoord,pitch.charAt(1),c);
      if(pitch.charAt(1) == '-'){
        printSmall(xCoord+10,yCoord,pitch.charAt(2),c);
      }
    }
  }
}

void drawBox(uint8_t cornerX, uint8_t cornerY, uint8_t width, uint8_t height, uint8_t depth, int8_t xSlant, uint8_t fill){
  // if(cornerX+width>screenWidth || cornerY+height>screenHeight){
  //   return;
  // }
  int16_t point[4][2] = {{cornerX,cornerY},
                         {int16_t(cornerX+width),cornerY},
                         {int16_t(cornerX+width+xSlant),int16_t(cornerY+depth)},
                         {int16_t(cornerX+xSlant),int16_t(cornerY+depth)}};
  switch(fill){
    //transparent box (wireframe)
    case 0:
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);
      //draw bottom face
      display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      break;
    //for a solid self-occluding box
    case 1:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);

      if(xSlant>=0){
        //draw bottom face
        display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
        //draw vertical edges
        display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      else if(xSlant<0){
        //draw bottom face
        display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        //draw vertical edges
        display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      break;
    //for a solid,filled box
    //draw top face again and again
    case 2:
      for(int i = 0; i<height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      break;
    //for a solid box, with black edges
    case 3:
      for(int i = 0; i<height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);

      //draw bottom face
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;

    //for a box with a white border, but no internal lines
    case 4:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face (just the back two lines)
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      //draw bottom face (just the front two lines)
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges (just the two edges)
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      break;
    //this one is obscure, but basically a solid white box with a black edge (helps it stand out over white things)
    case 5:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face (just the back two lines)
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      //draw bottom face (just the front two lines)
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges (just the two edges)
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 6:
      //draw top face
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);
      //draw bottom face
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;
    //transparent box (wireframe) with inverted color
    case 7:
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],2);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],2);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,2);
      display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,2);
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,2);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,2);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,2);
      display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,2);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,2);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 8:
      //draw top face
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
  }
}

void keyboardAnimation(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeys, bool into){
  display.clearDisplay();
  uint8_t keyLength = 40;
  uint8_t keyHeight = 5;
  uint8_t keyWidth = 3;
  uint8_t xSlant = 8;
  uint8_t offset = 3;
  uint8_t whiteKeys = 0;
  uint8_t blackKeys = 0;

  if(into){
    //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
    for(int key = startKey; key<startKey+numberOfKeys; key++){
      //if it's a black key
      if((startKey+key)%12 == 1 || (startKey+key)%12 == 3 || (startKey+key)%12 == 6 || (startKey+key)%12 == 8 || (startKey+key)%12 == 10){
        uint8_t blackKeyOffset = 0;
        if(blackKeys%12>1 && blackKeys%12<4){
          blackKeyOffset = key/12+1;
        }
        if(blackKeys%12>=4 && blackKeys%12<=6){
          blackKeyOffset = key/12+1;
        }
        drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
        blackKeys++;
        display.display();
        // delay(2);
      }
      else{
        drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
        whiteKeys++;
        display.display();
        // delay(2);
      }
    }
  }
  //this one needs to go in reverse. Both drum and this animation do this by just drawing the first X keys, then decrementing it
  else if(!into){
    while(numberOfKeys>0){
      display.clearDisplay();
      whiteKeys = 0;
      blackKeys = 0;
      for(int key = startKey; key<startKey+numberOfKeys; key++){
        //if it's a black key
        if((startKey+key)%12 == 1 || (startKey+key)%12 == 3 || (startKey+key)%12 == 6 || (startKey+key)%12 == 8 || (startKey+key)%12 == 10){
          uint8_t blackKeyOffset = 0;
          if(blackKeys%12>1 && blackKeys%12<4){
            blackKeyOffset = key/12+1;
          }
          if(blackKeys%12>=4 && blackKeys%12<=6){
            blackKeyOffset = key/12+1;
          }
          drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
          blackKeys++;
        }
        else{
          drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
          whiteKeys++;
        }
      }
      numberOfKeys--;
      if(!(numberOfKeys%4)){
        display.display();
        // delay(5);
      }
    }
  }
}
//each byte represents the channels an output will send on
//each BIT of each byte is a channel
uint16_t midiChannels[5] = {65535,65535,65535,65535,65535};
#ifdef HEADLESS
  bool isThru(uint8_t i){
    return true;
  }
  void sendThruOn(uint8_t c, uint8_t n, uint8_t v){
    return;
  }
  void sendThruOff(uint8_t c, uint8_t n){
    return;
  }
  void setThru(uint8_t o, bool s){
    return;
  }
#endif
#ifndef HEADLESS
bool isThru(uint8_t output){
  switch(output){
    case 0:
      return MIDI0.getThruState();
    case 1:
      return MIDI1.getThruState();
    case 2:
      return MIDI2.getThruState();
    case 3:
      return MIDI3.getThruState();
    case 4:
      return MIDI4.getThruState();
  }
  //if it's an invalid midi port
  return 0;
}

void sendThruOn(uint8_t channel, uint8_t note, uint8_t vel){
  //if it's a valid thru & channel
  if(isThru(0) && isActiveChannel(channel, 0)){
    MIDI0.sendNoteOn(note,vel,channel);
  }
  if(isThru(1) && isActiveChannel(channel, 1)){
    MIDI1.sendNoteOn(note,vel,channel);
  }
  if(isThru(2) && isActiveChannel(channel, 2)){
    MIDI2.sendNoteOn(note,vel,channel);
  }
  if(isThru(3) && isActiveChannel(channel, 3)){
    MIDI3.sendNoteOn(note,vel,channel);
  }
  if(isThru(4) && isActiveChannel(channel, 4)){
    MIDI4.sendNoteOn(note,vel,channel);
  }
}

void sendThruOff(uint8_t channel, uint8_t note){
  //if it's a valid thru & channel
  if(isThru(0) && isActiveChannel(channel, 0)){
    MIDI0.sendNoteOff(note,0,channel);
  }
  if(isThru(1) && isActiveChannel(channel, 1)){
    MIDI1.sendNoteOff(note,0,channel);
  }
  if(isThru(2) && isActiveChannel(channel, 2)){
    MIDI2.sendNoteOff(note,0,channel);
  }
  if(isThru(3) && isActiveChannel(channel, 3)){
    MIDI3.sendNoteOff(note,0,channel);
  }
  if(isThru(4) && isActiveChannel(channel, 4)){
    MIDI4.sendNoteOff(note,0,channel);
  }
}

void setThru(uint8_t output, bool state){
  if(state){
    switch(output){
      case 0:
        MIDI0.turnThruOn();
        break;
      case 1:
        MIDI1.turnThruOn();
        break;
      case 2:
        MIDI2.turnThruOn();
        break;
      case 3:
        MIDI3.turnThruOn();
        break;
      case 4:
        MIDI4.turnThruOn();
        break;
    }
  }
  else{
    switch(output){
      case 0:
        MIDI0.turnThruOff();
        break;
      case 1:
        MIDI1.turnThruOff();
        break;
      case 2:
        MIDI2.turnThruOff();
        break;
      case 3:
        MIDI3.turnThruOff();
        break;
      case 4:
        MIDI4.turnThruOff();
        break;
    }
  }
}
#endif

bool toggleThru(uint8_t output){
  bool isActive = isThru(output);
  setThru(output, !isActive);
  return !isActive;
}

bool isTotallyMuted(uint8_t which){
  if(!midiChannels[which])
    return true;
  else
    return false;
}
bool isActiveChannel(uint8_t channel, uint8_t output){
  //if it's an omni channel, it'll be max values
  if(midiChannels[output] == 65535){
    return true;
  }
  else{
    bool value = (midiChannels[output] & (1 << channel-1)) != 0 ;
    return value;
  }
}

void setMidiChannel(uint8_t channel, uint8_t output, bool status){
  //for activating, you use OR
  if(status){
    uint16_t byte = 1 << channel-1;
    midiChannels[output] = midiChannels[output] | byte;
  }
  //for deactivating, you use AND (and NOT to create the mask)
  else{
    uint16_t byte = ~(1 << channel-1);
    midiChannels[output] = midiChannels[output] & byte;
  }
  // Serial.print(midiChannels[output],BIN);
}

//toggles the channel on an output, and returns its new value
bool toggleMidiChannel(uint8_t channel, uint8_t output){
  bool isActive = isActiveChannel(channel, output);
  setMidiChannel(channel, output, !isActive);
  return !isActive;//return new state of channel
}

void muteMidiPort(uint8_t which){
  midiChannels[which] = 0;
}

void unmuteMidiPort(uint8_t which){
  midiChannels[which] = 65535;
}

void toggleMidiPort(uint8_t which){
  bool isActive = toggleMidiChannel(0,which);
  for(int i = 0; i<16; i++){
    setMidiChannel(i+1,which,isActive);
  }
}
void drawPortIcon(uint8_t which){
  uint8_t x1,y1;
  switch(which){
    case 0:
      x1 = 64;
      y1 = 46;
      if(midiChannels[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),usb_logo_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),9,SSD1306_WHITE);
      drawDottedLineDiagonal(x1,y1+8,x1,35,3);
      break;
    case 1:
      x1 = 15;
      y1 = 7;
      if(midiChannels[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1+9,y1+8,x1+22,y1+8,3);
      break;
    case 2:
      x1 = 25;
      y1 = 37;
      if(midiChannels[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1+1,y1+10,x1+22,y1-4,3);
      break;
    case 3:
      x1 = screenWidth-25;
      y1 = 37;
      if(midiChannels[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1-2,y1+9,x1-22,y1-4,3);
      break;
    case 4:
      x1 = screenWidth-15;
      y1 = 7;
      if(midiChannels[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1-8,y1+8,x1-22,y1+8,3);
      break;
  }
}
//a closeup menu on one port
void portMenu(uint8_t which){
  uint8_t cursors[3];
  uint8_t menuState = 0;
  //vu meter
  int16_t currentVel;
  int16_t lastVel;
  float VUval;
  uint8_t channelMenuStart = 0;
  while(true){
    display.clearDisplay();
    drawPortIcon(which);
    switch(which){
      case 0:
        drawVU(0,0,1-VUval);
        break;
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
    }
    display.display();
    //if something is being sent/received on this port
    bool something;
    for(uint8_t i = 0; i<playlist.size(); i++){
      if(isActiveChannel(playlist[i][2],which)){
        currentVel = playlist[i][1];
        something = true;
      }
    }
    if(!something)
      currentVel = 0;
    //set VU to the lastVel variable
    VUval = float(lastVel)/float(127);
    //update lastVel, incrementing it towards the currentVel
    lastVel = lastVel + (currentVel-lastVel)/5;

    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(del){
        lastTime = millis();
        muteMidiPort(which);
      }
      if(n){
        lastTime = millis();
        unmuteMidiPort(which);
      }
      if(sel){
        toggleMidiPort(which);
        lastTime = millis();
      }
    }
  }
}

void thruMenu(){ //controls which midi port you're editing
  uint8_t xCursor = 1;
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1 && xCursor<4){
          xCursor++;
          lastTime = millis();
        }
        else if(x == 1 && xCursor>0){
          xCursor--;
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        bool isActive = toggleThru(xCursor);
        if(shift){
          for(int i = 0; i<5; i++){
            setThru(i, isActive);
          }
        }
        lastTime = millis();
      }
    }
    display.clearDisplay();
    // printSmall(40,48,"Thru routing",SSD1306_WHITE);
    printCursive(22,48, "set thru ports",SSD1306_WHITE);
    uint8_t xOffset = 6;
    for(uint8_t midiPort = 0; midiPort<5; midiPort++){
      if(midiPort == 0){
        printSmall(xOffset+3+midiPort*25,0,"USB",SSD1306_WHITE);
        if(xCursor == 0)
          display.drawBitmap(xOffset+midiPort*25,8+2*sin(midiPort+millis()/100),usb_logo_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xOffset+midiPort*25,8,usb_logo_bmp,17,17,SSD1306_WHITE);
      }
      else{
        printSmall(xOffset+7+midiPort*25,0,stringify(midiPort),SSD1306_WHITE);
        if(xCursor == midiPort)
          display.drawBitmap(xOffset+midiPort*25,8+2*sin(midiPort+millis()/100),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xOffset+midiPort*25,8,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      if(xCursor == midiPort)
        display.drawCircle(xOffset+midiPort*25+8,8+2*sin(midiPort+millis()/100)+8,8,SSD1306_WHITE);
      if(isThru(midiPort)){
        display.fillRect(xOffset+midiPort*25,34,17,7,SSD1306_WHITE);
        printSmall(xOffset+1+midiPort*25,35,"Thru",SSD1306_BLACK);
      }
      else{
        printSmall(xOffset+2+midiPort*25,35,"off",SSD1306_WHITE);
      }
    }
    display.display();
  }
}

void midiPortAnimation(bool in){
  if(in){

  }
  else{

  }
}

void midiPortAnimation_old(bool in){
  if(in){
    int8_t frameCount = 8;
    while(frameCount>-3){
      display.clearDisplay();
      for(uint8_t port = 0; port<5; port++){
        uint8_t xCoord = 6+port*25;
        int8_t yCoord = 8+port*10+frameCount*8;
        if(yCoord<8)
          yCoord = 8;
        if(port == 0)
          display.drawBitmap(xCoord,yCoord,usb_logo_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xCoord,yCoord,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      display.display();
      // delay(100);
      frameCount--;
    }
  }
  else{
    int8_t frameCount = -3;
    while(frameCount<8){
      display.clearDisplay();
      for(uint8_t port = 0; port<5; port++){
        uint8_t xCoord = 6+port*25;
        int8_t yCoord = 8+frameCount*8+port*10;
        if(yCoord<8)
          yCoord = 8;
        if(port == 0)
          display.drawBitmap(xCoord,yCoord,usb_logo_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xCoord,yCoord,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      display.display();
      frameCount++;
    }
  }
}
//this is a complex menu! it has two modes, and a very custom selection screen
void midiMenu(){
  uint8_t xCursor = 0;
  uint8_t yCursor = 0;
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(sel){
        lastTime = millis();
        //center menu mode
        if(yCursor == 0){
          switch(xCursor){
            //thru
            case 0:
              routeMenu();
              break;
            //channels
            case 1:
              inputMenu();
              break;
            case 2:
              thruMenu();
              break;
          }
        }
        else{
          // portMenu(xCursor);
          toggleMidiPort(xCursor);
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        // midiPortAnimation(false);
        break;
      }
      if(play){
        lastTime = millis();
        togglePlayMode();
      }
      if(del && yCursor == 0){
        muteMidiPort(xCursor);
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(y != 0){
        //center menu mode
        if(yCursor == 0){
          if(y == 1){
            if(xCursor == 2){
              xCursor = 0;
              yCursor = 1;
              lastTime = millis();
            }
            else{
              xCursor++;
              lastTime = millis();
            }
          }
          if(y == -1 && xCursor>0){
            xCursor--;
            lastTime = millis();
          }
        }
        //midi mute mode
        else{
          //moving up
          if(y == -1){
            //if it's on the center, jump back up to menu
            if(xCursor == 0){
              yCursor = 0;
              xCursor = 2;
              lastTime = millis();
            }
            //if not, then increment the cursor
            else if(xCursor == 3){
              xCursor = 4;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 1;
              lastTime = millis();
            }
          }
          //moving down
          else if(y == 1){
            if(xCursor == 1){
              xCursor = 2;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 4){
              xCursor = 3;
              lastTime = millis();
            }
          }
        }
      }
      //x jumps you off of and back into center menu
      if(x != 0){
        //if you were in center menu, jump off to the left or right
        if(yCursor == 0){
          yCursor = 1;
          if(x == -1){
            xCursor = 4;
          }
          else if(x == 1){
            xCursor = 1;
          }
          lastTime = millis();
        }
        //if you're in the mute menu
        else{
          //right
          if(x == -1){
            //go back to normal mode
            if(xCursor == 1){
              yCursor = 0;
              xCursor = 0;
              lastTime = millis();
            }
            //valid moves
            else if(xCursor == 0){
              xCursor = 3;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 3){
              xCursor = 4;
              lastTime = millis();
            }
          }
          //left
          else if(x == 1){
            //go back to normal mode
            if(xCursor == 4){
              yCursor = 0;
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 0){
              xCursor = 2;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 1;
              lastTime = millis();
            }
            else if(xCursor == 3){
              xCursor = 0;
              lastTime = millis();
            }
          }
        }
      }
    }
    //getting active channels/pitches to draw note icons
    vector<vector<uint8_t>> activeChannels;
    for(uint8_t track = 0; track<trackData.size(); track++){
      if(trackData[track].noteLastSent != 255)
        activeChannels.push_back({trackData[track].channel,trackData[track].noteLastSent});
    }

    display.clearDisplay();

    uint8_t x1;
    uint8_t y1;
    //drawing each midi port icon and info
    for(uint8_t port = 0; port<5; port++){
      bool isCurrentlySending = false;
      uint8_t currentPitch;
      //drawing note icon for actively sending/rxing ports
      for(uint8_t i = 0; i<activeChannels.size(); i++){
        if(isActiveChannel(activeChannels[i][0],port)){
          isCurrentlySending = true;
          currentPitch = activeChannels[i][1];
        }
      }
      //getting the right coords for each port
      switch(port){
        //usb
        case 0:
        //usb is in the middle
          x1 = screenWidth/2;
          y1 = 46;
          drawDottedLineDiagonal(x1,y1+8,x1,35,3);
          break;
        //1 and 2 are on left
        case 1:
          x1 = 15;
          y1 = 7;
          drawDottedLineDiagonal(x1+9,y1+8,x1+22,y1+8,3);
          break;
        case 2:
          x1 = 25;
          y1 = 37;
          drawDottedLineDiagonal(x1+1,y1+10,x1+22,y1-4,3);
          break;
        //3 and 4 are on right
        case 3:
          x1 = screenWidth-25;
          y1 = 37;
          drawDottedLineDiagonal(x1-2,y1+9,x1-22,y1-4,3);
          break;
        case 4:
          x1 = screenWidth-15;
          y1 = 7;
          drawDottedLineDiagonal(x1-8,y1+8,x1-22,y1+8,3);
          break;
      }
      if(playing){
        uint8_t x2 = 95;
        uint8_t y2 = 4;
        display.fillTriangle(x2+sin(millis()/100)+1,y2+6,x2+sin(millis()/100)+1,y2,x2+6+sin(millis()/100)+1,y2+3,SSD1306_WHITE);
      }
      //draw feed line
      //if it's currently sending a note, then draw the note pitch instead of the port #
      //AND the port bounces faster
      if(isCurrentlySending){
        //drawing bitmaps
        if(port == 0){
          //note icon
          display.drawChar(x1-5,y1-5+2*sin(millis()/100+port),0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
          //pitch
          printSmall(x1+1,y1-5+2*sin(millis()/100+port),pitchToString(currentPitch,true,true),SSD1306_WHITE);
          //bitmap
          if(!isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+2*sin(millis()/100+port),usb_logo_bmp,17,17,SSD1306_WHITE);
          else
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,SSD1306_WHITE);
          //'usb' doesn't display (it's replaced by note)
          // printSmall(x1-5,y1-4+2*sin(millis()/100+port),"usb",SSD1306_WHITE);
        }
        else{
          //note icon
          display.drawChar(x1-5,y1+18+2*sin(millis()/100+port),0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
          //pitch
          printSmall(x1+1,y1+18+2*sin(millis()/100+port),pitchToString(currentPitch,true,true),SSD1306_WHITE);
          //bitmap
          if(!isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+2*sin(millis()/100+port),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
          else
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,SSD1306_WHITE);
          //number
          printSmall(x1-1,y1-4+2*sin(millis()/100+port),stringify(port),SSD1306_WHITE);
        }
      }
      //if it's not sending, the ports have numbers and bounce slower
      else{
        //number
        if(port == 0){
          if(!isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+sin(millis()/100+port),usb_logo_bmp,17,17,SSD1306_WHITE);
          else{
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),9,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,1);
          }
          printSmall(x1-5,y1-5+sin(millis()/100+port),"usb",SSD1306_WHITE);
        }
        else{
          if(!isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+sin(millis()/100+port),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
          else{
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),8,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,1);
          }
          printSmall(x1-1,y1-4+sin(millis()/100+port),stringify(port),SSD1306_WHITE);
        }
      }

      //if it's in mute mode, draw a circle around the active port
      if(yCursor == 1 && xCursor == port){
        if(isCurrentlySending){
          drawArrow(x1+9+2*sin(millis()/100+port),y1+8+sin(millis()/100),2,1,true);
          //for the usb port, it's a little bigger
          if(port == 0){
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,SSD1306_WHITE);
          }
          else{
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,SSD1306_WHITE);
          }
        }
        else{
          //usb port is a little bigger
          if(port == 0){
            display.drawCircle(x1,y1+8+sin(millis()/100+port),9,SSD1306_WHITE);
            drawArrow(x1+9+sin(millis()/100),y1+8+sin(millis()/100),2,1,true);
          }
          else{
            display.drawCircle(x1,y1+8+sin(millis()/100+port),8,SSD1306_WHITE);
            drawArrow(x1,y1+18+sin(millis()/100),2,2,true);
          }
        }
      }
    }

    x1 = 51;
    y1 = 10;
    //menu title
    display.setRotation(1);
    printChunky(42-y1,x1-12,"MIDI",2);
    display.setRotation(UPRIGHT);

    display.setRotation(3);
    printChunky(y1-1,x1-13,"MENU",2);
    display.setRotation(UPRIGHT);
    //bounding box
    //menu options
    printSmall(x1,y1,"Routing",SSD1306_WHITE);
    printSmall(x1,y1+8,"Input",SSD1306_WHITE);
    printSmall(x1,y1+16,"Thru",SSD1306_WHITE);
      
    //center menu mode
    if(yCursor == 0){
      //bounding box
      display.drawRoundRect(x1-3,y1-3,33,27,3,SSD1306_WHITE);
      //selection arrows
      switch(xCursor){
        //port routing
        case 0:
          drawArrow(x1+29+sin(millis()/100),y1+2,3,1,false);
          // display.drawRoundRect(54+sin(millis()/100),40,22-int(2*cos(millis()/100)),9,3,SSD1306_WHITE);
          break;
        //input
        case 1:
          drawArrow(x1+29+sin(millis()/100),y1+10,3,1,false);
          // display.drawRoundRect(32+sin(millis()/100),47,66-int(2*cos(millis()/100)),9,3,SSD1306_WHITE);
          break;
        //clock source
        case 2:
          drawArrow(x1+29+sin(millis()/100),y1+18,3,1,false);
          break;
      }
    }
    display.display();
  }
}

uint16_t midiChannelFilter = 65535;
uint8_t midiMessageFilter = 255;

void toggleFilter_channel(uint8_t channel){
  bool state = midiChannelFilter & (1<<channel);
  uint16_t mask = 1<<channel;//a 1 in the target place

  //invert mask if you're turning it off
  if(state){
    mask = ~mask;
    midiChannelFilter &= mask;
  }
  else{
    midiChannelFilter |= mask;
  }
}
void toggleFilter_message(uint8_t message){
  bool state = midiMessageFilter & (1<<message);
  uint8_t mask = 1<<message;//a 1 in the target place

  //invert mask if you're turning it off
  if(state){
    mask = ~mask;
    midiMessageFilter &= mask;
  }
  else{
    midiMessageFilter |= mask;
  }
}
//input menu can set a filter on the input
//two filters: one for channels, one for kinds of messages
//kinds of messages:
/*
- note
- cc
-clock (start, stop, timeframe)
*/
void inputMenu(){
  //for channel filter
  uint8_t x1 = 61;
  uint8_t y1 = 10;
  //for control filter
  uint8_t x2 = 90;
  uint8_t y2 = 35;

  uint8_t xCursor = 0;
  uint8_t yCursor = 0;
  uint8_t menuStart[2] = {0,0};

  const uint8_t maxPorts = 7;

  while(true){
    display.clearDisplay();
    //printing the channel filter
    for(int i = 0; i<maxPorts; i++){
      //print channel numbers
      printSmall(x1+12, y1+i*6, stringify(i+menuStart[0]+1),SSD1306_WHITE);
      //if the channel is 
      if(midiChannelFilter & (1<<(i+menuStart[0]))){
        //if this box is cursore'd, AND if it's the active midi port
        if(i == yCursor && xCursor == 0)
          drawCheckbox(x1+1, y1+i*6, true, true);
        else
          drawCheckbox(x1+1, y1+i*6, true, false);
      }
      else{
        if(i == yCursor && xCursor == 0)
          drawCheckbox(x1+1, y1+i*6, false, true);
        else
          drawCheckbox(x1+1, y1+i*6, false, false);
      }
    }
    // const vector<String> labels = {"note:","cc:","clock:"};
    const unsigned char* icons[3] = {epd_bitmap_small_note, small_clock, cc_small};
    //printing the control filter
    for(uint8_t i = 0; i<3; i++){
      display.drawBitmap(x2+11,y2+i*10+1,icons[i],5,5,SSD1306_WHITE);
      if(midiMessageFilter & (1<<i)){
        if(yCursor == i && xCursor == 1)
          drawCheckbox(x2+1, y2+i*10, true, true);
        else
          drawCheckbox(x2+1, y2+i*10, true, false);
      }
      else{
        if(yCursor == i && xCursor == 1)
          drawCheckbox(x2+1, y2+i*10, false, true);
        else
          drawCheckbox(x2+1, y2+i*10, false, false);
      }
    }
    int8_t bigOffset;
    if(isReceiving())
      bigOffset = 2*sin(millis()/100);
    else
      bigOffset = 2*sin(millis()/300);
    display.drawBitmap(-12,-4+bigOffset,big_midi_inverse,45,45,SSD1306_WHITE);
    printCursive(69,0,"midi",SSD1306_WHITE);
    printCursive(83,8,"filters",SSD1306_WHITE);
    if(xCursor == 0){
      display.setRotation(1);
      printChunky(screenHeight-y1-45,x1-8,"channels",SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      drawDottedLineDiagonal(21,16+bigOffset,41,36+bigOffset,3);
      drawDottedLineDiagonal(41,36+bigOffset,50,36+bigOffset,3);
    }
    else if(xCursor == 1){
      display.setRotation(1);
      printChunky(screenHeight-y2-28,x2-8,"types",SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      drawDottedLineDiagonal(10,32+bigOffset,10,58+bigOffset,3);
      drawDottedLineDiagonal(10,56+bigOffset,x2-10,56+bigOffset,3);
    }
    display.display();

    readButtons();
    joyRead();
    //controls
    if(itsbeen(200)){
      //moving xCursor
      if(x != 0){
        xCursor = !xCursor;
        if(xCursor == 1 && yCursor>=3){
          yCursor = 2;
        }
        lastTime = millis();
      }
      //exit
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        if(shift){

        }
        else{
          //toggle channel filter
          if(xCursor == 0){
            toggleFilter_channel(yCursor+menuStart[0]);
            lastTime = millis();
          }
          //toggle message filter
          else if(xCursor == 1){
            toggleFilter_message(yCursor);
            lastTime = millis();
          }
        }
      }
    }
    //moving vertical cursor
    if(itsbeen(80)){
      if(y != 0){
        if(y == 1){
          //for channel filter
          if(xCursor == 0){
            //if the cursor is already at the end of the menu, move that ish
            if(yCursor == maxPorts-1 && menuStart[0]<(16-maxPorts)){
              menuStart[0]++;
              lastTime = millis();
            }
            //if it's not at the end of the menu, move the cursor
            if(yCursor<maxPorts-1){
              yCursor++;
              lastTime = millis();
            }
          }
          //for message filter
          else if(xCursor == 1){
            if(yCursor<2){
              yCursor++;
              lastTime = millis();
            }
          }
        }
        else if(y == -1){
          //for channel filter
          if(xCursor == 0){
            //if the cursor is already at the end of the menu, move that ish
            if(yCursor == 0 && menuStart[0]>0){
              menuStart[0]--;
              lastTime = millis();
            }
            //if it's not at the end of the menu, move the cursor
            if(yCursor>0){
              yCursor--;
              lastTime = millis();
            }
          }
          //message filter
          else if(xCursor == 1){
            if(yCursor>0){
              yCursor--;
              lastTime = millis();
            }
          }
        }
      }
    }
  }
}

void routeMenu(){
  //controls which midi port you're editing
  uint8_t xCursor = 1;
  //controls which channel (relative to menuStart) you're looking at
  uint8_t yCursor = 0;
  //controls where each of the five menus starts
  uint8_t menuStart[5] = {0,0,0,0,0};
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(80)){
      if(y != 0){
        if(y == 1){
          //if the cursor is already at the end of the menu, move that ish
          if(yCursor == 4 && menuStart[xCursor]<11){
            menuStart[xCursor]++;
            lastTime = millis();
          }
          //if it's not at the end of the menu, move the cursor
          if(yCursor<4){
            yCursor++;
            lastTime = millis();
          }
        }
        else if(y == -1){
          //if the cursor is already at the end of the menu, move that ish
          if(yCursor == 0 && menuStart[xCursor]>0){
            menuStart[xCursor]--;
            lastTime = millis();
          }
          //if it's not at the end of the menu, move the cursor
          if(yCursor>0){
            yCursor--;
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1 && xCursor<4){
          xCursor++;
          lastTime = millis();
        }
        else if(x == 1 && xCursor>0){
          xCursor--;
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        bool isActive = toggleMidiChannel(yCursor+menuStart[xCursor]+1,xCursor);
        // Serial.println("toggling "+stringify(yCursor+menuStart[xCursor])+" on out "+stringify(xCursor));
        if(shift){
          for(int i = 0; i<16; i++){
            setMidiChannel(i+1,xCursor,isActive);
          }
        }
        lastTime = millis();
      }
    }
    display.clearDisplay();
    uint8_t xOffset = 2;
    for(uint8_t midiPort = 0; midiPort<5; midiPort++){
      uint8_t yCoord = 8;
      if(midiPort == 0){
        printSmall(xOffset+3+midiPort*25,0,"USB",SSD1306_WHITE);
        if(xCursor == 0)
          yCoord = 8+2*sin(midiPort+millis()/100);
        if(!isTotallyMuted(0))
          display.drawBitmap(xOffset+midiPort*25,yCoord,usb_logo_bmp,17,17,SSD1306_WHITE);
      }
      else{
        printSmall(xOffset+7+midiPort*25,0,stringify(midiPort),SSD1306_WHITE);
        if(xCursor == midiPort)
          yCoord = 8+2*sin(midiPort+millis()/100);
        if(!isTotallyMuted(midiPort))
          display.drawBitmap(xOffset+midiPort*25,yCoord,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      if(xCursor == midiPort)
        display.drawCircle(xOffset+midiPort*25+8,8+2*sin(midiPort+millis()/100)+8,8,SSD1306_WHITE);

      for(int i = 0; i<5; i++){
        //print channel numbers
        printSmall(xOffset+12+midiPort*25, 30+i*6, stringify(i+menuStart[midiPort]+1),SSD1306_WHITE);
        //print channel values
        //if this box is cursore'd, AND if it's the active midi port
        if(i == yCursor && xCursor == midiPort)
          drawCheckbox(xOffset+1+midiPort*25, 29+i*6, isActiveChannel(i+menuStart[midiPort]+1,midiPort), true);
        else
          drawCheckbox(xOffset+1+midiPort*25, 29+i*6, isActiveChannel(i+menuStart[midiPort]+1,midiPort), false);
      }
      //"more channels" indicators
      //more channels above
      if(menuStart[midiPort]>0){
        drawArrow(xOffset+17+midiPort*25,26+sin(millis()/200),2,2,true);
      }
      //more channels below
      if(menuStart[midiPort]<11){
        drawArrow(xOffset+17+midiPort*25,63-sin(millis()/200),2,3,true);
      }
    }
    display.display();
  }
}


void drawArpStepLengths(uint8_t xStart, uint8_t yStart, uint8_t startNote, uint8_t xCursor, bool selected){
  if(activeArp.uniformLength){
    drawCenteredBanner(64,20,"using uniform steps of "+stepsToMeasures(activeArp.arpSubDiv));
    drawLabel(64,32,"[sel] to toggle custom steps",true);
  }
  uint8_t spacing = 3;
  uint8_t thickness = (screenWidth-8)/activeArp.lengths.size()-spacing;
  if(activeArp.lengths.size()>=8){
    thickness = (screenWidth-12)/8-spacing;
  }
  uint8_t height;
  for(uint8_t i = 0; i<8; i++){
    //only draw blocks for lengths that exist (in case there are less than 16)
    //also, only draw blocks that will still be on screen
    if(i<activeArp.lengths.size()){
      height = float(activeArp.lengths[i+startNote])*float(64-23)/float(96);
      //drawing filled rect for steps that correspond to currently
      //held notes
      if(i + startNote<activeArp.notes.size())
        display.fillRect(9+(spacing+thickness)*i,screenHeight-height-7, thickness, height,SSD1306_WHITE);
      //and empty rects for steps that don't
      else
        display.drawRect(9+(spacing+thickness)*i,screenHeight-height-7, thickness, height,SSD1306_WHITE);
      //highlighting the step that's currently playing
      if(activeArp.playing && (i+startNote == activeArp.activeNote)){
        // display.fillRect(9+(spacing+thickness)*i,screenHeight-6,thickness,6,1);
        display.drawRect(7+(spacing+thickness)*i,screenHeight-height-9, thickness+4, height+4,SSD1306_WHITE);
      }
      printSmall(10+(spacing+thickness)*i+thickness/2-stringify(i+startNote+1).length()*2,screenHeight-5,stringify(i+startNote),2);
      //step the cursor is on
      if(i == xCursor-startNote){
        drawArrow(9+(spacing+thickness)*i+thickness/2,screenHeight-height-10+2*sin(millis()/100),3,3,true);
        printSmall(8+(spacing+thickness)*i+thickness/2-stepsToMeasures(activeArp.lengths[i+startNote]).length()*2+2,screenHeight-height-21+2*sin(millis()/100),stepsToMeasures(activeArp.lengths[i+startNote]),SSD1306_WHITE);
      }
      //if there are steps offscreen
      if(startNote>0){
        drawArrow(2+2*sin(millis()/100),61,2,1,true);
      }
      if(activeArp.lengths.size()>startNote+8){
        drawArrow(screenWidth-2-2*sin(millis()/100),61,2,0,true);
      }
    }
  }
  // printArp(28,0,"step lengths",SSD1306_WHITE);
  // printSmall_centered(64,0,"steps",1);
  display.setRotation(1);
  printItalic(16,0,"steps",1);
  display.setRotation(UPRIGHT);
}

//drawing keys
void drawKeys(uint8_t xStart,uint8_t yStart,uint8_t octave,uint8_t numberOfKeys, bool fromPlaylist){
  uint8_t keyLength = 40;
  uint8_t keyHeight = 5;
  uint8_t keyWidth = 3;
  uint8_t xSlant = 8;
  uint8_t offset = 3;
  //keys are on an isometric grid
  uint8_t whiteKeys = 0;
  uint8_t blackKeys = 0;
  bool pressed;
  //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
  for(int key = 0; key<numberOfKeys; key++){
    pressed = false;
    // printPlaylist();
    if(fromPlaylist){
      for(int note = 0; note<playlist.size(); note++){
        if(playlist[note][0] == key+octave*12){
          pressed = true;
        }
      }
    }
    else{
      if(activeArp.playing && activeArp.lastPitchSent%12 == key){
        pressed = true;
      }
    }
    //if it's a black key
    if((key)%12 == 1 || (key)%12 == 3 || (key)%12 == 6 || (key)%12 == 8 || (key)%12 == 10){
      if(blackKeys<6){//only draw the keys that you need
        uint8_t blackKeyOffset = 0;
        if(blackKeys%12>1 && blackKeys%12<4){
          blackKeyOffset = key/12+1;
        }
        if(blackKeys%12>=4 && blackKeys%12<=6){
          blackKeyOffset = key/12+1;
        }
        if(shift){
          display.setCursor(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25+keyLength, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3);
          display.print(pitchToString(key+12*octave,false,true));
        }
        if(shift && key+12*octave == keyboardPitch){//while shifting the pitch
            drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,0);
        }
        else{
          if(pressed){
            drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-1,keyLength-10, keyHeight, keyWidth, xSlant,4);
          }
          else
            drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
        }
        blackKeys++;
      }
    }
    else if(whiteKeys<9){
      //drawing pitches
      if(shift){
        display.setCursor(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset)+2+8);
        display.print(pitchToString(key+12*octave,false,true));
      }
      if(shift && key+12*octave == keyboardPitch){//while shifting the pitch
        drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,0);
        whiteKeys++;
      }
      else{
        if(pressed){
          drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset)+2,keyLength, keyHeight, keyWidth, xSlant,2);
        }
        else
          drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
        whiteKeys++;
      }
    }
  }
  if(shift){
    display.setCursor(0,screenHeight-16);
    display.setTextSize(2);
    display.print(getOctave(keyboardPitch));
    display.setTextSize(1);
  }
}
void drawKeys_inverse(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeys){
  uint8_t keyLength = 40;
  uint8_t keyHeight = 5;
  uint8_t keyWidth = 3;
  uint8_t xSlant = 8;
  uint8_t offset = 3;

  //keys are on an isometric grid
  uint8_t whiteKeys = 0;
  uint8_t blackKeys = 0;
  bool pressed;
  //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
  for(int key = startKey; key<startKey+numberOfKeys; key++){
    pressed = false;
    for(int note = 0; note<playlist.size(); note++){
      if(playlist[note][0]%12 == key%12){
        pressed = true;
      }
    }
    //if it's a black key
    if((startKey+key)%12 == 1 || (startKey+key)%12 == 3 || (startKey+key)%12 == 6 || (startKey+key)%12 == 8 || (startKey+key)%12 == 10){
      uint8_t blackKeyOffset = 0;
      if(blackKeys%12>1 && blackKeys%12<4){
        blackKeyOffset = key/12+1;
      }
      if(blackKeys%12>=4 && blackKeys%12<=6){
        blackKeyOffset = key/12+1;
      }
      if(pressed){
        drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-1,keyLength-10, keyHeight, keyWidth, xSlant,5);
      }
      else
        drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,1);
      blackKeys++;
    }
    else{
      if(pressed){
        drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset)+2,keyLength, keyHeight, keyWidth, xSlant,4);
      }
      else
        drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,3);
      whiteKeys++;
    }
  }
}
void drumPadAnimation(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads, bool into){
  uint8_t width = 15;
  int xSlant = -7;
  uint8_t xSpacing = 5;
  uint8_t ySpacing = 2;
  uint8_t thickness = 4;
  uint8_t rows = 4;
  uint8_t columns = 4;
  bool pressed;
  uint8_t pad = startPad+numberOfPads-1;
  if(into){
    display.clearDisplay();
    //draws pads one at a time, with a delay in between
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(pad>=startPad){
          drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
          pad--;
          display.display();
          delay(10);
        }
      }
    }
  }
  else if(!into){
  //this one needs to loop backwards! or at least look like it
    while(numberOfPads > 0){
      display.clearDisplay();
      pad = numberOfPads;
      for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
          if(pad>0){
            drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
            pad--;
          }
        }
      }
      numberOfPads--;
      if(!(numberOfPads%4)){
        display.display();
        delay(5);
      }
    }
  }
}
void drawDrumPads(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads){
  uint8_t width = 15;
  int xSlant = -7;
  uint8_t xSpacing = 5;
  uint8_t ySpacing = 2;
  uint8_t thickness = 4;
  uint8_t rows = 4;
  uint8_t columns = 4;

  bool pressed;
  uint8_t pad = startPad+numberOfPads-1;

  // drawBox(xStart, yStart, width, thickness, width-7, xSlant, 1);
  for(int i = 0; i<rows; i++){
    for(int j = 0; j<columns; j++){
      if(pad>=startPad){
        pressed = false;
        for(int note = 0; note<playlist.size(); note++){
          if(playlist[note][0] == pad){
            pressed = true;
          }
        }
        if(pressed){
          if(pad == keyboardPitch && shift){//so that you can see the keyboard pitch when shifting through
            drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,0);
          }
          else
            drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,2);
          if(shift){
            display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            display.setTextColor(SSD1306_BLACK);
            // display.print(pitchToString(pad,false,true));
            display.print(pad);
            display.setTextColor(SSD1306_WHITE);
          }
        }
        else{
          if(pad == keyboardPitch && shift){//so that you can see the keyboard pitch when shifting through
            drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,0);
          }
          else
            drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
          if(shift){
            display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            display.print(pitchToString(pad,false,true));
          }
        }
        pad--;
      }
    }
  }
  if(shift){
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print(getOctave(keyboardPitch));
    display.setTextSize(1);
  }
}

vector<uint8_t> selectKeys() {
  uint8_t selected = 0;
  bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float animOffset = 0;
  bool done = false;
  uint8_t root = 0;
  //FUCK
  const uint8_t scaleOptions[12] =  { MAJOR , HARMONIC_MINOR , MELODIC_MINOR , MAJOR_PENTATONIC , MINOR_PENTATONIC , BLUE , DORIAN , PHRYGIAN , LYDIAN , MIXOLYDIAN , AEOLIAN , LOCRIAN };
  uint8_t activeScale;
  bool showingScale = false;
  while (!done) {
    animOffset++;
    readButtons_MPX();
    joyRead();
    if (itsbeen(100)) {
      if (x == -1 && selected < 11) {
        selected++;
        lastTime = millis();
      }
      if (x == 1 && selected > 0) {
        selected--;
        lastTime = millis();
      }
    }
    if (itsbeen(200)) {
      if (sel) {
        sel = false;
        lastTime = millis();
        keys[selected] = !keys[selected];
      }
      if(n){
        n = false;
        lastTime = millis();
        done = true;
      }
      if(menu_Press){
        vector<uint8_t> empty;
        return empty;
      }
    }
    //changing root
    while(counterA != 0){
      if(counterA >= 1)
        root++;
      else if(counterA <= -1){
        if(root == 0)
          root = 11;
        else
          root--;
      }
      root%=12;
      counterA += counterA<0?1:-1;;
    }
    //changing scale
    while(counterB != 0){
      if(!showingScale){
        showingScale = true;
        activeScale = 0;
      }
      else{
        if(counterB >= 1){
          activeScale++;
        }
        else if(counterB <= -1){
          if(activeScale == 0)
            activeScale = 11;
          else activeScale--;
        }
        activeScale %= 12;
      }
      vector<uint8_t> newScale = genScale(scaleOptions[activeScale],0);
      //clear out old scale
      for(int i = 0; i<12; i++){
        keys[i] = false;
      }
      //putting in newScale
      for(int i = 0; i<newScale.size(); i++){
        keys[newScale[i]] = true;
      }
      counterB += counterB<0?1:-1;;
    }
    //also changing root and scale, but with joystick
    if(itsbeen(200)){
      if(y != 0){
        if(shift){
          if(y == 1){
            root++;
            root%=12;
            lastTime = millis();
          }
          else if(y == -1){
            if(root == 0)
              root = 11;
            else
              root--;
            root%=12;
            lastTime = millis();
          }
        }
        else{
          if(!showingScale){
            showingScale = true;
            activeScale = 0;
            lastTime = millis();
          }
          else{
            if(y == 1){
              activeScale++;
              lastTime = millis();
            }
            else if(y == -1){
              if(activeScale == 0)
                activeScale = 11;
              else activeScale--;
              lastTime = millis();
            }
            activeScale %= 12;
          }
          vector<uint8_t> newScale = genScale(scaleOptions[activeScale],0);
          //clear out old scale
          for(int i = 0; i<12; i++){
            keys[i] = false;
          }
          //putting in newScale
          for(int i = 0; i<newScale.size(); i++){
            keys[newScale[i]] = true;
          }
        }
      }
    }
    //-----------------------------------
    display.clearDisplay();
    uint8_t xStart = 4;
    uint8_t yStart = 11;
    uint8_t keyHeight = 28;
    uint8_t keyWidth = 15;
    uint8_t offset = 3;
    uint8_t blackKeys = 0;
    uint8_t whiteKeys = 0;
    //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
        if (i == selected) {
          display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys)-2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          display.print(getInterval(0, i));
        }
        else if (keys[i]) {
          shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
          display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) - 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          display.print(getInterval(0, i));
        }
        else {
          display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
        }
        whiteKeys++;
      }
    }
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
        //if it's highlighted
        if (i == selected) {
          display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          display.print(getInterval(0, i));
        }
        //if it's selected
        else if (keys[i]) {
          shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
          display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          display.print(getInterval(0, i));
        }
        //if it's normal
        else {
          display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
          display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
        }
        blackKeys++;
      }
    }
    if(showingScale){
      printCursive(28, 1, getScaleName(scaleOptions[activeScale]),SSD1306_WHITE);
    }
    else{
      printCursive(49, 1, "scale",SSD1306_WHITE);
    }
    printCursive(40, screenHeight-7,"root",SSD1306_WHITE);
    display.setCursor(69,screenHeight-7);
    display.print(" "+pitchToString(root,false,true));
    display.display();
  }
  vector<uint8_t> returnedKeys;
  for(int i = 0; i<12; i++){
    if(keys[i]){
      returnedKeys.push_back(i+root);
    }
  }
  return returnedKeys;
}
void selectKeysAnimation(bool in) {
  if(in){
    uint8_t selected = 0;
    bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float animOffset = 0;
    int otherAnimOffset = 24;
    while (otherAnimOffset>0) {
      animOffset++;
      otherAnimOffset-=2;
      //-----------------------------------
      display.clearDisplay();
      uint8_t xStart = 4;
      uint8_t yStart = 11;
      uint8_t keyHeight = 28;
      uint8_t keyWidth = 15;
      uint8_t offset = 3;
      uint8_t blackKeys = 0;
      uint8_t whiteKeys = 0;
      int test = 6;
      //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
      for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));            
            }
            else{
              shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      display.display();
    }
  }
  else{
    uint8_t selected = 0;
    bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float animOffset = 0;
    int otherAnimOffset = 0;
    while (otherAnimOffset<24) {
      animOffset++;
      otherAnimOffset+=2;
      //-----------------------------------
      display.clearDisplay();
      uint8_t xStart = 4;
      uint8_t yStart = 11;
      uint8_t keyHeight = 28;
      uint8_t keyWidth = 15;
      uint8_t offset = 3;
      uint8_t blackKeys = 0;
      uint8_t whiteKeys = 0;
      int test = 6;
      //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
      for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));            
            }
            else{
              shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      display.display();
    }
  }
}
String getInterval(int root, int pitch) {
  int interval = (pitch - root) % 12;
  switch (interval) {
    case 0://root (unison)
      return "I";
      break;
    case 1://minor 2nd
      return "ii";
      break;
    case 2://major 2nd
      return "II";
      break;
    case 3://minor 3rd
      return "iii";
      break;
    case 4://major 3rd
      return "III";
      break;
    case 5://perfect 4th
      return "IV";
      break;
    case 6://diminished fifth
      return "dV";
      break;
    case 7://p 5th
      return "V";
      break;
    case 8://minor 6th
      return "vi  ";
      break;
    case 9://major 6th
      return "VI";
      break;
    case 10://minor 7th
      return "vii";
      break;
    case 11://major 7th
      return "VII";
      break;
  }
  return "";
}

/*
                                                                __.
         __           _________ .__    .__.__       .___   _    \  \
   _____/ /____  ____ \_   ___ \|  |__ |__|  |    __| _/    \____)__\
  / ___/ __/ _ \/ __ \/    \  \/|  |  \|  |  |   / __ |     |========|
 (__  ) /_/  __/ /_/ />     \___|   Y  \  |  |__/ /_/ |      \______/
/____/\__/\___/ .___/  \______  /___|  /__|____/\____ |       _/  \_
             /_/      ________\/_____\/______________\/_     (_)  (_)       
                      \________________________________/
*/
void testAsciiPram(){
  display.setTextColor(SSD1306_WHITE);
  while(true){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(
stringify("                                                                __.\n")+
stringify("         __           _________ .__    .__.__       .___   _    \\  \\\n")+
stringify("   _____/ /____  ____ \\_   ___ \\|  |__ |__|  |    __| _/    \\____)__\\\n")+
stringify("  / ___/ __/ _ \\/ __ \\/    \\  \\/|  |  \\|  |  |   / __ |     |========|\n")+
stringify(" (__  ) /_/  __/ /_/ />     \\___|   Y  \\  |  |__/ /_/ |      \\______/\n")+
stringify("/____/\\__/\\___/ .___/  \\______  /___|  /__|____/\\____ |       _/  \\_\n")+
stringify("             /_/      ________\\/_____\\/______________\\/_     (_)  (_)\n")+       
stringify("                      \\________________________________/\n"));
    display.display();
  }
}

bool isCoordWithinVector(uint8_t xVal, uint8_t yVal, vector<vector<uint8_t>> vec){
  for(uint16_t i = 0; i<vec.size(); i++){
    if(xVal == vec[i][0] && yVal == vec[i][1])
      return true;
  }
  return false;
}



void fadeScreen(bool black, uint8_t speed){
  //how many pixels need to be set on each pass
  float increment = (screenWidth*screenHeight)/speed;
  //randomly set "increment" many pixels to black, and do this "speed" number of times
  vector<vector<uint8_t>> points;
  uint16_t changedPoints = 0;
  // Serial.println("increment = "+String(increment));
  for(uint8_t i = 0; i<speed; i++){
    // Serial.println(i);
    changedPoints = 0;
    while(changedPoints<increment-1){
      uint8_t xCoord = random(0,128);
      uint8_t yCoord = random(0,64);
      //if the two coords haven't been written yet, write them and push them to the vectors
      if(!isCoordWithinVector(xCoord,yCoord,points)){
        vector<uint8_t> temp = {xCoord,yCoord};
        points.push_back(temp);
        changedPoints++;
        // Serial.println("setting ("+String(xCoord)+","+String(yCoord)+")");
        // Serial.println(changedPoints);
        if(black)
          display.drawPixel(xCoord,yCoord,SSD1306_BLACK);
        else
          display.drawPixel(xCoord,yCoord,SSD1306_WHITE);
      }
    }
    display.display();
  }
}

void drawEllipse(uint8_t h, uint8_t k, int a, int b, uint16_t c) {
  //centerX = h
  //centerY = k
  //horizontal radius = a
  //vertical radius = b
  int y1;
  //if the ellipse is secretely a circle
  if (a == b) {
    display.drawCircle(h, k, a, c);
  }
  //for every x that falls along the length of the ellipse, get a y and draw a point
  else if (a > 0 && b > 0) {
    for (int x1 = h - a; x1 <= h + a; x1++) {
      if (x1 < screenWidth && x1 >= 0) {
        int root = b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        y1 = k + root;
        if (y1 < screenHeight) {
          display.drawPixel(x1, y1, c);
        }
        y1 = k - root;
        if (y1 >= 0) {
          display.drawPixel(x1, y1, c);
        }
      }
    }
  }
}


void drawBracket(uint8_t h, uint8_t k, uint8_t height, uint8_t width, uint8_t style, uint16_t c){
  int16_t point[4][2] = 
  {{int16_t(h-width/2),int16_t(k-height)},
   {int16_t(h-width/2),k},
   {int16_t(h+width/2),k},
   {int16_t(h+width/2),int16_t(k-height)}};
  //horizontal bracket facing up
  if(style == 0){
   //left side
   display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //bottom
   display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],c);
   //right side
    display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
  //horizontal bracket facing down
  if(style == 1){
   //left side
   display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //top
   display.drawLine(point[0][0],point[0][1],point[3][0],point[3][1],c);
   //right side
    display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}
void drawTetra(uint8_t h, uint8_t k, uint8_t height, uint8_t width, int xDepth, int yDepth, uint8_t style, uint16_t c){
  //upside down, and transparent
  if(style == 0){
    int point[4][2] =
   {{h,k+height/2},
    {h-width/2,k-height/2},
    {h+width/2,k-height/2},
    {h-xDepth,k-yDepth}};
    for(int i = 0; i<4; i++){
      while(point[i][0]<0){
        point[i][0]++;
      }
      while(point[i][1]<0){
        point[i][1]++;
      }
    }
    //draw front triangle
    display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[2][0],point[2][1],c);
    //draw left triangle
    display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[3][0],point[3][1],c);
    //draw right triangle
    display.drawTriangle(point[0][0],point[0][1],point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}

void moon(){
  int moonphase = 0;
  bool waxing = true;
  while(true){
    readButtons();
    while(counterA != 0){
      if(counterA >= 1){
        moonphase+=10;
        if(moonphase>100){
          moonphase -= 10;
          waxing = !waxing;
        }
      }
      else if(counterA <= -1){
        moonphase-=10;;
        if(moonphase<0){
          moonphase = -moonphase;
          waxing = !waxing;
        }
      }
      counterA += counterA<0?1:-1;;
    }
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        return;
      }
    }
    if(shift){
      moonphase+=2;
      if(moonphase>100){
        moonphase = 0;
        waxing = !waxing;
      }
    }
    display.clearDisplay();
    drawMoon(moonphase,waxing);
    display.display();
  }
}

void cutoutCircle(float x, float y, float r, uint16_t c){
  for(float i = 0; i<(x-r+10); i++){
    display.drawCircle(x,y,r+i,c);
  }
}

void drawMoon(int phase, bool forward){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  // phase*=scale;
  int r = screenHeight/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase<0)
    phase = 0;
  //controls whether or not the moon is waxing or waning
  if(forward){
    if(phase>0)
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    fillEllipse(screenWidth-2*r+phase/2,yPos,r,r+phase,SSD1306_BLACK);//filling earth's shadow
    if(phase>58 && phase < 88)
      fillEllipse(xPos,yPos,phase-58,r,SSD1306_WHITE);//filling earth's shadow
    if(phase>=88){
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);
    }
    else{
      cutoutCircle(63,32,r+1,SSD1306_BLACK);
    }
    display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
  else if(!forward){
    if(phase == 0)
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    else{
      if(phase<88)
        fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
      if(phase>58 && phase<88)
        fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
      if(phase>=88){
        display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
      }
      else{
        cutoutCircle(63,31,r,SSD1306_BLACK);
        cutoutCircle(63,32,r+1,SSD1306_BLACK);
      }
      display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
    }
  }
}
void drawMoon_reverse(int phase){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  //  phase*=scale;
  int r = screenHeight/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase == 0)
    display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
  else{
    if(phase<88)
      fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
    if(phase>58)
      fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
    if(phase>=88){
      display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
    }
    else{
      cutoutCircle(63,31,r,SSD1306_BLACK);
    }
    display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
}

void echoAnimation(){
  int maxReps = 1;//for how many rings
  int spacing = 10;//for the spacing
  int animFrame = 0;
  int xCoord = 64;
  int yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  while(animFrame<8*spacing*maxReps/3+8*32+30){
    spacing = echoData[0];
    maxReps = 1;
    display.clearDisplay();
    // drawSeq();
    if(animFrame<=32){
      if(animFrame>8){//drops and reflection
        display.drawCircle(xCoord, animFrame-8, 1+sin(animFrame), SSD1306_WHITE);
        display.drawCircle(xCoord, screenHeight-(animFrame-8), 1+sin(animFrame), SSD1306_WHITE);
      }
      display.drawCircle(xCoord, animFrame, 3+sin(animFrame), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-animFrame, 3+sin(animFrame), SSD1306_WHITE);
    }
    else if(animFrame>yCoord){
      int reps = (animFrame-yCoord)/spacing+1;
      if(reps>maxReps){
        reps = maxReps;
      }
      for(int i = 0; i<reps; i++){
        // fillEllipse_cheap(xCoord, yCoord, animFrame/3-spacing*i+sin(animFrame)*(i%2), animFrame/8-spacing*i/3, SSD1306_BLACK);
        drawEllipse(xCoord, yCoord, animFrame/3-spacing*i+sin(animFrame)*(i%2), animFrame/8-spacing*i/3, SSD1306_WHITE);
      }
    }
    if(animFrame<yCoord){
      animFrame+=5;
    }
    else
      animFrame+=6;
    display.display();
  }
}

//animation for the file menu
void fileAnimation(bool in){
  if(in){
    activeMenu.highlight = -1;
    int xDistance = screenWidth-activeMenu.topL[0];//how far the display is gonna need to slide over
    int width = activeMenu.bottomR[0] - activeMenu.topL[0];
    for(int i = 0; i< xDistance; i+=10){
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[0] = screenWidth-i;
      activeMenu.bottomR[0] = activeMenu.topL[0] + width;
      display.clearDisplay();
      activeMenu.displayFilesMenu(20-i/10,0,0,3);
      // display.drawBitmap(screenWidth-i,0,folder_closed_bmp,14,26,SSD1306_WHITE);
      display.display();
    }
    activeMenu.highlight = 0;
    display.drawBitmap(0,0,folder_open_bmp,24,26,SSD1306_WHITE);
  }
  else if(!in){
    activeMenu.highlight = -1;
    int xDistance = screenWidth-activeMenu.topL[0];//how far the display is gonna need to slide over
    int width = activeMenu.bottomR[0] - activeMenu.topL[0];
    for(int i = xDistance; i> 0; i-=10){
      //this literally just shifts where the menu is over and over again
      activeMenu.topL[0] = screenWidth-i;
      activeMenu.bottomR[0] = activeMenu.topL[0] + width;
      display.clearDisplay();
      activeMenu.displayFilesMenu(30-i/10,0,0,3);
      // display.drawBitmap(xDistance-i,0,folder_closed_bmp,14,26,SSD1306_WHITE);
      display.display();
    }
    activeMenu.highlight = 0;
  }
}

void cassetteAnimation(){
  int reps = 30;
  int xOffset = 0;
  int yOffset = 0;
  //actual cassettes are 4:2.5:0.5
  while(reps>0){
    display.clearDisplay();
    for(int i = 0; i<reps; i++){
      xOffset = i;
      yOffset = -15-i+(pow(i-10,2))/5;
//      yOffset = -i;
      display.drawRoundRect(xOffset+14,yOffset+0,100,62,4,SSD1306_WHITE);//outer box
      display.drawRoundRect(xOffset+19,yOffset+5,90,38,4,SSD1306_WHITE);//inner label
      display.drawRoundRect(xOffset+34,yOffset+20,60,15,3,SSD1306_WHITE);//spool box
      display.drawCircle(xOffset+41,yOffset+28,4,SSD1306_WHITE);//spool1
      display.drawCircle(xOffset+86,yOffset+28,4,SSD1306_WHITE);//spool2
      display.setTextColor(SSD1306_WHITE);
      display.setFont(&FreeSerifItalic9pt7b);
      display.setCursor(xOffset+40,yOffset+18);
      display.print("(Loop)");
      // display.drawChar(41,14,0x3C,SSD1306_WHITE,SSD1306_BLACK,2);
      // display.drawChar(50,14,0x3E,SSD1306_WHITE,SSD1306_BLACK,2);
    }
    reps-=4;
    display.display();
  }
  // delay(15);
  int div = 1;
  while(div<120){
    display.drawRoundRect(xOffset+14,yOffset+0,100,62,4,SSD1306_WHITE);//outer box
    display.drawRoundRect(xOffset+19,yOffset+5,90,38,4,SSD1306_WHITE);//inner label
    display.drawRoundRect(xOffset+34,yOffset+20,60,15,3,SSD1306_WHITE);//spool box
    display.fillCircle(xOffset+41,yOffset+28,4+div,SSD1306_BLACK);//spool1
    display.fillCircle(xOffset+86,yOffset+28,4+div,SSD1306_BLACK);//spool2
    display.drawCircle(xOffset+41,yOffset+28,4+div,SSD1306_WHITE);//spool1
    display.drawCircle(xOffset+86,yOffset+28,4+div,SSD1306_WHITE);//spool2
    display.setTextColor(SSD1306_WHITE);
    display.setFont(&FreeSerifItalic9pt7b);
    display.setCursor(xOffset+40,yOffset+18);
    display.print("(Loop)");
    display.display();
    div+=12;
  }
}
void drawCassette(){
  int reps = 25;
  int xOffset = 0;
  int yOffset = 0;
  //actual cassettes are 4:2.5:0.5
  display.drawRoundRect(xOffset+14,yOffset+0,100,62,4,SSD1306_WHITE);//outer box
  display.drawRoundRect(xOffset+19,yOffset+5,90,38,4,SSD1306_WHITE);//inner label
  display.drawRoundRect(xOffset+34,yOffset+20,60,15,3,SSD1306_WHITE);//spool box
  display.drawCircle(xOffset+41,yOffset+28,4,SSD1306_WHITE);//spool1
  display.drawCircle(xOffset+86,yOffset+28,4,SSD1306_WHITE);//spool2
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(xOffset+40,yOffset+18);
  display.print("(Loop)");
  display.display();
}

#ifdef HEADLESS
  void startMIDI(){
    return;
  }
  void sendMIDICC(uint8_t cc, uint8_t v, uint8_t c){
    return;
  }
  void sendMIDIallOff(){
    return;
  }
  void sendMIDInoteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
    return;
  }
  void sendMIDInoteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
    return;
  }
  void readMIDI(){
    return;
  }
  void sendClock(){
    return;
  }
  void sendMIDIStart(){
    return;
  }
  void sendMIDIStop(){
    return;
  }
#endif
#ifndef HEADLESS
void startMIDI(){
  MIDI0.begin(MIDI_CHANNEL_OMNI);
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI4.begin(MIDI_CHANNEL_OMNI);

  MIDI0.turnThruOff();
  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  MIDI4.turnThruOff();
}


void sendMIDICC(uint8_t controller, uint8_t val, uint8_t channel){
  if(isActiveChannel(channel, 0))
    MIDI0.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 1))
    MIDI1.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendControlChange(controller, val, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendControlChange(controller, val, channel);
  // Serial.println(val);
  // Serial.flush();
}

void sendMIDIallOff(){
  MIDI0.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI1.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI2.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI3.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  MIDI4.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
}


void sendMIDInoteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
  if(isActiveChannel(channel, 0))
    MIDI0.sendNoteOn(pitch,vel,channel);
  if(isActiveChannel(channel, 1))
    MIDI1.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendNoteOn(pitch, vel, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendNoteOn(pitch, vel, channel);
}

void sendMIDInoteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
  if(isActiveChannel(channel, 0))
    MIDI0.sendNoteOff(pitch,vel,channel);
  if(isActiveChannel(channel, 1))
    MIDI1.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 2))
    MIDI2.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 3))
    MIDI3.sendNoteOff(pitch, vel, channel);
  if(isActiveChannel(channel, 4))
    MIDI4.sendNoteOff(pitch, vel, channel);
}

void readMIDI(){
  MIDI0.read();
  MIDI1.read();
}

void sendClock(){
  MIDI0.sendRealTime(midi::Clock);
  MIDI1.sendRealTime(midi::Clock);
  MIDI2.sendRealTime(midi::Clock);
  MIDI3.sendRealTime(midi::Clock);
  MIDI4.sendRealTime(midi::Clock);
}

void sendMIDIStart(){
  MIDI0.sendRealTime(midi::Start);
  MIDI1.sendRealTime(midi::Start);
  MIDI2.sendRealTime(midi::Start);
  MIDI3.sendRealTime(midi::Start);
  MIDI4.sendRealTime(midi::Start);
}

void sendMIDIStop(){
  MIDI0.sendRealTime(midi::Stop);
  MIDI1.sendRealTime(midi::Stop);
  MIDI2.sendRealTime(midi::Stop);
  MIDI3.sendRealTime(midi::Stop);
  MIDI4.sendRealTime(midi::Stop);
}
#endif

//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
int8_t velModifier[2] = {0,0};
int8_t chanceModifier[2] = {0,0};
int8_t pitchModifier[2] = {0,0};

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      velModifier[0] = channel;
      velModifier[1] = val-63;
      break;
    //chance
    case 1:
      chanceModifier[0] = channel;
      chanceModifier[1] = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      pitchModifier[0] = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      pitchModifier[1] = float(val)/float(127) * 48 - 24;
      break;
    //bpm
    case 3:
      setBpm(float(val)*2);
      break;
    //swing amount
    case 4:
      swingVal += val-63;
    //track channel
    case 5:
      break;
  }
}

String getCCParameterName(uint8_t param){
  if(param<16)
    return CCparameters[param];
  else if(param >= 16 && param <= 19)
    return CCparameters[16];
  else if(param >= 20 && param <= 31)
    return CCparameters[17];
  else if(param >= 32 && param <= 63)
    return CCparameters[18]+stringify(param-32);
  else if(param >= 64 && param <= 79)
    return CCparameters[param-45];
  else if(param >= 80 && param <= 83)
    return CCparameters[35];
  else if(param == 84)
    return CCparameters[36];
  else if(param >= 85 && param <= 87)
    return CCparameters[37];
  else if(param == 88)
    return CCparameters[38];
  else if(param == 89 || param == 90)
    return CCparameters[39];
  else if(param >= 91 && param <= 101)
    return CCparameters[param - 51];
  else if(param >= 102 && param <= 119)
    return CCparameters[51];
  else  
    return "";
}

String getBUSFromChannel(uint8_t channel){
  if(channel >= 1 && channel <= 4)
    return "BUS "+stringify(channel);
  else if(channel == 5)
    return "INPUT";
  else
    return "[No BUS]";
}
String getMKIIParameterName(uint8_t param,  uint8_t channel){
  String bus = getBUSFromChannel(channel);
  if(param >= 16 && param <= 18)
    return bus+" "+MKIICCparameters[0]+stringify(param-15);
  else if (param == 19)
    return bus+" "+MKIICCparameters[1];
  else if(param >= 80 && param <= 82)
    return bus+" "+MKIICCparameters[0]+stringify(param-76);
  else if(param == 83)
    return bus+" "+MKIICCparameters[2];
  else
    return "";
}

String getStepChildCCParameterName(uint8_t param, uint8_t channel){
  String name;
  if(param<5){
    if(param>2){
      name+= "Global "+stepChildCCParameters[param];
    }
    else{
      if(channel == 0){
        name += "Global ";
      }
      else
        name+= "Ch"+stringify(channel)+" ";
      name+=stepChildCCParameters[param];
    }
  }
  return name;
}

//turns a numbered parameter like "third in the MKII list" to a MIDI CC number like "83"
uint8_t MKIIParamToCC(uint8_t param){
  if(param >= 0 && param <= 3)
    return param+16;
  else if(param >= 4 && param <= 7)
    return param+76;
  else
    return 100;
}

uint8_t stepchildParamToCC(uint8_t p){
  if(p>4)
    p = 4;
  return p;
}

uint8_t moveToNextCCParam(uint8_t param, bool up, uint8_t whichList){
  switch(whichList){
    //default
    case 0:
      if(param>0 && !up)
        param--;
      else if(param<127 && up)
        param++;
      break;
    //mkII
    case 1:
      if(!up){
        //if it's within param bounds
        if((param > 16 && param <= 19) || (param > 80 && param <= 83))
          param--;
        //if it aint
        else if(param > 83)
          param = 83;
        else if(param > 19 && param <= 80)
          param = 19;
      }
      else{
        if((param >= 16 && param < 19) || (param >= 80 && param < 83))
          param++;
        else if(param < 16)
          param = 16;
        else if(param >= 19 && param < 80)
          param = 80;
      }
      break;
    //stepChild
    case 2:
      if(up){
        if(param<4)
          param++;
        else
          param = 4;
      }
      else{
        if(param>0)
          param--;
        else
          param = 0;
      }
      break;
  }
  return param;
}

void testParameters(){
  for(uint8_t param = 0; param <= 119; param++){
    Serial.println("--------------");
    Serial.println("CC #:"+stringify(param)+" -- "+getCCParameterName(param));
    Serial.flush();
  }
}
void printParam_centered(uint8_t which, uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC){
  String p;
  switch(type){
    case 0:
      p = getCCParameterName(param);
      break;
    case 1:
      p = getMKIIParameterName(param,dataTrackData[which].channel);
      break;
    case 2:
      p = getStepChildCCParameterName(param,dataTrackData[which].channel);
      break;
  }
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*(p.length()+stringify(param).length())+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*(p.length()+stringify(param).length())+3,9,SSD1306_WHITE);
  }
  if(withCC)
    printSmall(xPos-p.length()*2-6,yPos,"CC"+stringify(param)+":"+p,SSD1306_WHITE);
  else
    printSmall(xPos-p.length()*2,yPos,p,SSD1306_WHITE);
}

void printParam(uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC){
  String p;
  if(withCC)
    p = "CC"+stringify(param)+":";
  switch(type){
    case 0:
      p += getCCParameterName(param);
      break;
    case 1:
      p += getMKIIParameterName(param,dataTrackData[activeDataTrack].channel);
      break;
    case 2:
      p += getStepChildCCParameterName(param,dataTrackData[activeDataTrack].channel);
      break;
  }
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*p.length()+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*p.length()+3,9,SSD1306_WHITE);
  }
  printSmall(xPos,yPos,p,SSD1306_WHITE);
}

String getCCParam(uint8_t param, uint8_t channel, uint8_t type){
  switch(type){
    case 0:
      return getCCParameterName(param);
    case 1:
      return getMKIIParameterName(param,channel);
    case 2:
      return getStepChildCCParameterName(param,channel);
  }
  return "";
}

void printChannel(uint8_t xPos, uint8_t yPos, uint8_t channel, bool withBox){
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_WHITE);
  }
  printSmall(xPos,yPos,"Channel:"+stringify(channel),SSD1306_WHITE);
}

void printParamList(uint8_t which, uint8_t target, uint8_t start, uint8_t end, uint8_t channel){
  switch(which){
    //default
    case 0:
      for(uint8_t p = start; p<end; p++){
        // display.setCursor(0,(p-start)*8);
        // display.print(getCCParameterName(p));
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        if(p<10)
          printSmall(3,27+(p-start)*6,stringify(p),2);
        else
          printSmall(2,27+(p-start)*6,stringify(p),2);
        printSmall(20+xPos,27+(p-start)*6,getCCParameterName(p),2);
      }
      break;
    //mkII
    case 1:
      for(uint8_t p = start; p<end; p++){
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        if(MKIIParamToCC(p)<10)
          printSmall(3,27+(p-start)*6,stringify(MKIIParamToCC(p)),2);
        else
          printSmall(2,27+(p-start)*6,stringify(MKIIParamToCC(p)),2);
        //convert number to CC, then to the parameter name
        printSmall(20+xPos,27+(p-start)*6,getMKIIParameterName(MKIIParamToCC(p),channel),2);
      }
      break;
    //Internal
    case 2:
      for(uint8_t p = start; p<end; p++){
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        printSmall(2,27+(p-start)*6,stringify(p),2);
        //convert number to CC, then to the parameter name
        printSmall(20+xPos,27+(p-start)*6,getStepChildCCParameterName(p,channel),2);
      }
      break;
  }
}

vector<uint8_t> switchBetweenCCLists(uint8_t start, uint8_t end, uint8_t targetParam, uint8_t which, bool left){
  vector<uint8_t> listControls = {start,end,targetParam,which};
  if(left){
    if(which>0){
      which--;
    }
    else
      return listControls;
  }
  else{
    if(which<2){
      which++;
    }
    else
      return listControls;
  }
  switch(which){
    //default
    case 0:
      end = start+6;
      if(end>127){
        end = 127;
        start = end - 6;
      }
      break;
    //mkII
    case 1:
      end = start+6;
      if(end>8){
        end = 8;
        start = 2;
      }
      break;
    //stepchild
    case 2:
      start = 0;
      end = 5;
      break;
  }
  targetParam = start;
  listControls[0] = start;
  listControls[1] = end;
  listControls[2] = targetParam;
  listControls[3] = which;
  return listControls;
}

//menu for selecting a CC parameter
uint8_t selectCCParam_dataTrack(uint8_t which){
  //which "list" you choose from
  //can be 0 -- "all" or 1 -- "SP404mkII"
  int8_t targetParam = 0;
  int8_t start = 0;
  uint8_t end = 6;
  if(which == 2)
    end = 5;
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1){
          vector<uint8_t> listControls = switchBetweenCCLists(start,end,targetParam,which,false);
          start = listControls[0];
          end = listControls[1];
          targetParam = listControls[2];
          which = listControls[3];
          lastTime = millis();
        }
        else if(x == 1){
          vector<uint8_t> listControls = switchBetweenCCLists(start,end,targetParam,which,true);
          start = listControls[0];
          end = listControls[1];
          targetParam = listControls[2];
          which = listControls[3];
          lastTime = millis();
        }
      }
      if(sel){
        sel = false;
        lastTime = millis();
        //if it's no longer an internal track, 
        dataTrackData[activeDataTrack].parameterType = which;
        if(dataTrackData[activeDataTrack].parameterType != 2 && dataTrackData[activeDataTrack].channel == 0)
          dataTrackData[activeDataTrack].channel = 1; 
        if(which == 1)
          return MKIIParamToCC(targetParam);
        else if(which == 2)
          return stepchildParamToCC(targetParam);
        else
          return targetParam;
      }
      if(menu_Press){
        menu_Press = false;
        lastTime = millis();
        return dataTrackData[activeDataTrack].control;
      }
      if(note_Press && which != 2){
        start-= 6;
        if(start<0)
          start = 0;
        end = start + 6;
        targetParam = start;
        lastTime = millis();
      }
      if(track_Press && which != 2){
        end+= 6;
        if(which == 1){
          if(end > 7){
            end = 7;
            start = 1;
          }
        }
        else if(end>127) 
          end = 127;
        start = end - 6;
        targetParam = start;
        lastTime = millis();
      }
    }
    if(itsbeen(100)){
      if(y != 0){
        if(y == -1 && targetParam>0){
          targetParam--;
          lastTime = millis();
        }
        else if(y == 1 && targetParam<127){
          targetParam++;
          if(which == 1){
            if(targetParam > 7)
              targetParam = 7;
          }
          else if(which == 2){
            if(targetParam>4)
              targetParam = 4;
          }
          lastTime = millis();
        }
        
        if(targetParam<start){
          start = targetParam;
          end = start+6;
        }
        else if(targetParam>(end-1)){
          end = targetParam+1;
          start = end-6;
        }
      }
    }

    display.clearDisplay();

    //carriage
    display.drawBitmap(0,1+sin(millis()/100),carriage_bmp,14,15,SSD1306_WHITE);

    //title
    printCursive(16,0,"midi   list",SSD1306_WHITE);
    printChunky(44,0,"CC",SSD1306_WHITE);

    //list brackets
    switch(which){
      case 0:
        display.fillRect(8,8,14,8,SSD1306_BLACK);
        display.drawLine(0,16,8,16,SSD1306_WHITE);
        display.drawLine(8,16,8,8,SSD1306_WHITE);
        display.drawLine(8,8,22,8,SSD1306_WHITE);
        display.drawLine(22,8,22,16,SSD1306_WHITE);
        display.drawLine(22,16,screenWidth,16,SSD1306_WHITE);
        break;
      case 1:
        display.drawLine(0,16,28,16,SSD1306_WHITE);
        display.drawLine(28,16,28,8,SSD1306_WHITE);
        display.drawLine(28,8,66,8,SSD1306_WHITE);
        display.drawLine(66,8,66,16,SSD1306_WHITE);
        display.drawLine(66,16,screenWidth,16,SSD1306_WHITE);
        break;
      case 2:
        display.drawLine(0,16,72,16,SSD1306_WHITE);
        display.drawLine(72,16,72,8,SSD1306_WHITE);
        display.drawLine(72,8,110,8,SSD1306_WHITE);
        display.drawLine(110,8,110,16,SSD1306_WHITE);
        display.drawLine(110,16,screenWidth,16,SSD1306_WHITE);
        break;
    }

    //list options
    printSmall(10,10,"All",SSD1306_WHITE);
    printSmall(30,10,"SP404mkii",SSD1306_WHITE);
    printSmall(74,10,"stepChild",SSD1306_WHITE);

    //list titles
    printSmall(2,20,"#",SSD1306_WHITE);
    printSmall(20,20,"name",SSD1306_WHITE);

    //parameter values
    printParamList(which,targetParam,start,end,dataTrackData[activeDataTrack].channel);

    display.display();
  }

  return targetParam;
}

//-----------------------------------------------------------
bool isBetween(float val, float margin, float target){
  if(val<target+margin && val> target-margin)
    return true;
  else  
    return false;
}

void copy(){
  if(selectionCount == 0) 
    return;
  //making sure buffer has enough 'columns' to store the notes from each track
  //clearing copybuffer
  vector<vector<Note>>temp1;
  vector<Note> temp2 = {};
  for(int i = 0; i<trackData.size(); i++){
    temp1.push_back(temp2);
  }
  copyBuffer.swap(temp1);

  copyPos[0] = activeTrack;
  copyPos[1] = cursorPos;

  uint16_t numberOfNotes = 0;

  //add all selected notes to the copy buffer
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      for(int note = 1; note<=seqData[track].size()-1; note++){// <= bc notes aren't 0 indexed
        if(seqData[track][note].isSelected){
          copyBuffer[track].push_back(seqData[track][note]);
          numberOfNotes++;
        }
      }
    }
    clearSelection();
  }
  //or if there's a target note, but it's not selected
  else if(lookupData[activeTrack][cursorPos] != 0){
    copyBuffer[activeTrack].push_back(seqData[activeTrack][lookupData[activeTrack][cursorPos]]);
    numberOfNotes = 1;
  }
  menuText = "copied "+stringify(numberOfNotes)+stringify(numberOfNotes)=="1"?"note":"notes";
}

void copyLoop(){
  //clear copyBuffer
  while(copyBuffer.size()>0){
    copyBuffer.pop_back();
  }
  //making sure buffer has enough 'columns' to store the notes from each track
  copyBuffer.resize(trackData.size());
  //treat copying the loop like you're copying it from the start of the loop
  copyPos[0] = 0;
  copyPos[1] = loopData[activeLoop][0];
  //add all selected notes to the copy buffer
  if(loopData[activeLoop][1]-loopData[activeLoop][0]>0){
    for(int track = 0; track<trackData.size(); track++){
      for(int step = loopData[activeLoop][0]; step<=loopData[activeLoop][1]; step++){// <= bc notes aren't 0 indexed
        if(lookupData[track][step] != 0){
          copyBuffer[track].push_back(seqData[track][lookupData[track][step]]);
          //move to the end of the note, so it's not double-counted
          step = seqData[track][lookupData[track][step]].endPos;
        }
      }
    }
  }
}

void debugPrintCopyBuffer(){
  for(int track = 0; track<trackData.size(); track++){
    for(int i = 0; i<copyBuffer[track].size(); i++){
      Serial.print("note ");
      Serial.println(i);
      Serial.print("s:");
      Serial.println(copyBuffer[track][i].startPos);
      Serial.print("e:");
      Serial.println(copyBuffer[track][i].endPos);
      Serial.print("on track:");
      Serial.println(track);
    }
  }
}
//pastes copybuffer
void paste(){
  if(copyBuffer.size()>0){
    uint16_t pastedNotes;
    //offset of all the notes (relative to where they were copied from)
    int yOffset = activeTrack - copyPos[0];
    int xOffset = cursorPos - copyPos[1];
    //moves through each track and note in copyBuffer, places a note at those positions in the seq
    for(int tracks = 0; tracks<copyBuffer.size(); tracks++){//for each track in the copybuffer
      if(copyBuffer[tracks].size()>0 && tracks+yOffset>=0 && tracks+yOffset<trackData.size()){//if there's a note stored for this track, and it'd be copied according to the new activeTrack
        for(int notes = 0; notes<copyBuffer[tracks].size(); notes++){
          if(copyBuffer[tracks][notes].startPos + xOffset<= seqEnd){
            //if the note will be copied to someWhere within the seqence, make note
            int start = xOffset+copyBuffer[tracks][notes].startPos;
            int end = xOffset+copyBuffer[tracks][notes].endPos;
            int track = tracks+yOffset;
            unsigned char vel = copyBuffer[tracks][notes].velocity;
            unsigned char chance = copyBuffer[tracks][notes].chance;
            bool mute = copyBuffer[tracks][notes].muted;
            //if note ends past seq, truncate it
            if(end>seqEnd){
              end = seqEnd;
            }
            //if note begins before seq, but also extends into seq, truncate it
            if(start<seqStart && end>seqStart){
              start = seqStart;
            }
            else if(start<0 && end<=seqStart){
              continue;
            }
            if(track<0||track>=trackData.size()){
              continue;
            }
            Note newNoteOn(start,end, vel, chance, mute, true);
            makeNote(newNoteOn,track,false);
            pastedNotes++;
          }
        }
      }
    }
    menuText = "pasted "+stringify(pastedNotes)+stringify(pastedNotes)=="1"?"note":"notes";
  }
}

void halveNote(int track, int id){
  Note targetNote = seqData[track][id];
  Note newNote = targetNote;
  int length = targetNote.endPos-targetNote.startPos+1;
  if(length>=4){
    newNote.startPos += length/2;
    makeNote(newNote,track);
  }
}

void halveSelectedNotes(){
  for(int track = 0; track<trackData.size(); track++){
    int origNoteCount = seqData[track].size()-1;//so that the new notes don't keep dividing
    for(int note = 1; note<=origNoteCount; note++){
      if(seqData[track][note].isSelected){
        halveNote(track, note);
      }
    }
  }
}

//merges a note and the next note, if they're touching
void fuseNotes(int track, int id){
  Note targetNote = seqData[track][id];
  int len = targetNote.endPos - targetNote.startPos + 1;
  //if there's a note next to it, and that note is the same length
  if(lookupData[track][targetNote.endPos+1] != 0 && seqData[track][lookupData[track][targetNote.endPos+1]].endPos-seqData[track][lookupData[track][targetNote.endPos+1]].startPos+1 == len){
    //if there's room for it
    if(targetNote.endPos+len<=seqEnd){
      //delete notes that are in the way
      for(int i = 1; i<=len; i++){
        //if there's a diff note there
        if(lookupData[track][targetNote.endPos+i] != 0 && lookupData[track][targetNote.endPos+i] != id){
          deleteNote_byID(track, lookupData[track][targetNote.endPos+i]);
        }
      }
      //doubling the note length
      changeNoteLength(len, track, id);
    }
  }
}

void fuseSelectedNotes(){
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note<seqData[track].size()-1; note++){
      if(seqData[track][note].isSelected){
        fuseNotes(track, note);
      }
    }
  }
}


//we should also have a reverse echo, because why not
void echoNote(int track, int id){
  int delay = echoData[0];
  int decay = echoData[1];
  int repeats = echoData[2];
  Note targetNote = seqData[track][id];
  for(int i = 0; i<repeats; i++){
    Note echoNote = targetNote;
    echoNote.isSelected = false;
    uint16_t offset = (i+1)*delay;
    echoNote.startPos += offset;
    echoNote.endPos += offset;
    echoNote.velocity *= pow(float(decay)/float(100),i+1);
    if(echoNote.velocity>0)
      makeNote(echoNote,track);
    else{
      return;
    }
  }
}

void echoTrack(int track){
  //do it from back to front, so echo's don't collide
  for(int note = seqData[track].size()-1; note>=1; note--){
    echoNote(track, note);
  }
}

void echoSeq(){
  for(int track = 0; track<trackData.size(); track++){
    echoTrack(track);
  }
}

void createArp(){
  Arp newArp;
  activeArp = newArp;
}

// void printArp(Arp arp){
//   arp.debugPrintArp();
// }



//Notes ------------------------------------------------------------------------
unsigned short int getRecentNoteID(int track, int time) {
  unsigned short int recentID;
  for (int i = time; i>=seqStart; i--) { //step backwards through the lookupdata until you find the start of the note
    recentID = lookupData[track][i];
    if (recentID != 0)
      break;
  }
  return recentID;
}

unsigned short int getRecentNoteID() {
  unsigned short int recentID;
  int time = cursorPos;
  int track = activeTrack;
  for (int i = time; i>=seqStart; i--) { //step backwards through the lookupdata until you find the start of the note
    recentID = lookupData[track][i];
    if (recentID != 0)
      break;
  }
  return recentID;
}

//truncates any note in this position
void truncateNote(int track, int time) {
  int id = lookupData[track][time];
  if(id == 0)
    return;
  for(int i = seqData[track][id].endPos-1; i>time; i--){//check this [CHECK]
    lookupData[track][i] = 0;
  }
  seqData[track][id].endPos = time;
}

//deletes a note from seqData
void deleteNoteFromData(int track, int id){
  if(id<=seqData[track].size()-1){
    //lowering selectionCount
    if(seqData[track][id].isSelected){
      selectionCount--;
    }
    //erasing note from seqData
    seqData[track].erase(seqData[track].begin() + id);
    //since we deleted it from seqData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the deleted note's ID should be decremented.
    for (int notes = 0; notes < seqEnd; notes++) {
      int changedNotes = 0;
      if ((lookupData[track][notes] > (id)) && (lookupData[track][notes] != 0)) { //if there's a higher note and if there are still notes to be changed
        lookupData[track][notes] -= 1;
        changedNotes++;
      }
      if(changedNotes >= seqData[track].size()-1)
        break;
    }
  }
}
void deleteNote_byID(int track, int targetNoteID){
    // Serial.println("deleting note "+stringify(targetNoteID));
  //if there's a note/something here, and it's in data
  if (targetNoteID != 0 && targetNoteID <= seqData[track].size()) {
    //clearing note from lookupData
    for (int i = seqData[track][targetNoteID].startPos; i < seqData[track][targetNoteID].endPos; i++) {
      if (lookupData[track][i] != 0) {
        lookupData[track][i] = 0;
      }
    }
    //lowering selectionCount
    if(seqData[track][targetNoteID].isSelected){
      selectionCount--;
    }
    //erasing note from seqData
    //make a copy of the seqData[track] vector which excludes the note
    //hopefully, this does a better job of freeing memory
    //swapping it like this! this is so the memory is free'd up again
    vector<Note> temp = defaultVec;
    for(int i = 1; i<=seqData[track].size()-1; i++){
      if(i != targetNoteID){//if it's not the target note, or an empty spot, copy it to the temp vector
        temp.push_back(seqData[track][i]);
      }
    }
    temp.swap(seqData[track]);
    //since we deleted it from seqData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the deleted note's ID should be decremented.
    if(seqData[track].size()-1>0){
      for (int step = seqStart; step <= seqEnd; step++) {
        if (lookupData[track][step] > targetNoteID) { //if there's a higher note and if there are still notes to be changed
          lookupData[track][step] -= 1;
        }
      }
    }
  }
}

void deleteNote(int track, int time) {
  deleteNote_byID(track, lookupData[track][time]);
}

void deleteNote() {
  deleteNote(activeTrack, cursorPos);
}

void drawProgBar(String text, float progress){
  display.setCursor(screenWidth-text.length()*10,screenHeight/2-8);
  display.setTextSize(2);
  display.print(text);
  display.setTextSize(1);
  // display.fillRect(32,screenHeight/2+10,64,8,SSD1306_BLACK);//clearing out the progress bar
  display.drawRect(32,screenHeight/2+10,64,8,SSD1306_WHITE);//drawing outline
  display.fillRect(32,screenHeight/2+10,64*progress,8,SSD1306_WHITE);//filling it
  display.display();
}

void deleteSelected(){
  if(selectionCount>0){
    if(binarySelectionBox(64,32,"nah","yea","delete "+stringify(selectionCount)+" note(s)?")==1){
      for(uint8_t track = 0; track<trackData.size(); track++){
        for(uint16_t note = 0; note<seqData[track].size(); note++){
          if(seqData[track][note].isSelected){
            deleteNote_byID(track, note);
            (note == 0) ? note = 0: note--;
          }
          if(selectionCount == 0)
            return;
        }
      }
    }
  }
  updateLEDs();
}

//this one deletes notes if they're there
void makeNote_hard(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
  if(lookupData[track][time] != 0){
    deleteNote(track,time);
  }
  if (lookupData[track][time] == blank_ID) { //if there's no note there
    if (lookupData[track][time] != 0) {
      truncateNote(track, time);
    }
    unsigned short int id = seqData[track].size()-1;
    lookupData[track][time] = seqData[track].size()-1;//set noteID in lookupData to the index of the new note
    for (int i = time; i < length + time-1; i++) { //sets id
      if (lookupData[track][i + 1] == 0)
        lookupData[track][i + 1] = id;
      else { //if there's something there, then set the end to the step before it
        lookupData[track][i] = id;
        newNoteOn.endPos = i;
        break;
      }
    }
  }
  if (loudly) {
    sendMIDInoteOn(trackData[track].pitch, newNoteOn.velocity, trackData[track].channel);
    sendMIDInoteOff(trackData[track].pitch, 0, trackData[track].channel);
  }
  seqData[track].push_back(newNoteOn);
}

void makeNote(Note newNoteOn, int track, bool loudly){
  //if you're placing it on the end of the seq, just return
  if(newNoteOn.startPos == seqEnd)
    return;
  //if there's a 0 where the note is going to go, or if there's not a zero BUT it's also not the start of that other note
  if (lookupData[track][newNoteOn.startPos] == 0 || newNoteOn.startPos != seqData[activeTrack][lookupData[activeTrack][newNoteOn.startPos]].startPos) { //if there's no note there
    if (lookupData[track][newNoteOn.startPos] != 0) {
      truncateNote(track, newNoteOn.startPos);
    }
    if(newNoteOn.isSelected){
      selectionCount++;
    }
    unsigned short int id = seqData[track].size();
    lookupData[track][newNoteOn.startPos] = id;//set noteID in lookupData to the index of the new note
    for (int i = newNoteOn.startPos+1; i < newNoteOn.endPos; i++) { //sets id
      if (lookupData[track][i] == 0 && i<seqEnd)
        lookupData[track][i] = id;
      else { //if there's something there, then set the end to the step before it
        newNoteOn.endPos = i;
        break;
      }
    }
    seqData[track].push_back(newNoteOn);
    if (loudly) {
      sendMIDInoteOn(trackData[track].pitch, newNoteOn.velocity, trackData[track].channel);
      sendMIDInoteOff(trackData[track].pitch, 0, trackData[track].channel);
    }
  }
}
void makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
  Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
  makeNote(newNoteOn,track,loudly);
}

void makeNote(Note newNoteOn, int track){
  makeNote(newNoteOn,track,false);
}

//this makes notes without updating the noteCount, and doesn't check bounds n stuff
void loadNote(int id, int track, int start, int velocity, bool isMuted, int chance, int end, bool selected){
  Note newNoteOn(start, end, velocity, chance, isMuted, false);
  newNoteOn.isSelected = selected;
  if(selected){
    selectionCount++;
  }
  loadNote(newNoteOn, track);
}

void loadNote(Note newNote, uint8_t track){
  //adding to seqData
  seqData[track].push_back(newNote);
  //adding to lookupData
  for (uint16_t i =  newNote.startPos; i < newNote.endPos; i++) { //sets id
    lookupData[track][i] = seqData[track].size()-1;
  }
}

void makeNote(int track, int time, int length, int velocity, int chance, bool loudly){
  makeNote(track,time,length,velocity,chance,false,false,loudly);
}
//this one is for quickly placing a ntoe at the cursor, on the active track
void makeNote(int track, int time, int length, bool loudly) {
  Note newNote(time,time+length,defaultVel,100,false,false);
  makeNote(newNote,track,loudly);
}

void makeNote(int length, bool loudly) {
  makeNote(activeTrack, cursorPos, length, defaultVel, 100, false, false, loudly);
}

//draws notes every "count" subDivs, from viewStart to viewEnd
void stencilNotes(uint8_t count){
  for(uint16_t i = viewStart; i<viewEnd; i+=(subDivInt*count)){
    if(lookupData[activeTrack][i] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos)
      makeNote(activeTrack, i, subDivInt*count+1, defaultVel, 100, false);
  }
}

#include "menus/autotrack.cpp"

void toggleKeys(){
  keys = !keys;
  clearButtons();
  clearPlaylist();
  if(keys){
    if(drumPads){
      toggleDrumPads();
    }
    keyboardAnimation(0,5,0,14,keys);
    keyboard();
  }
  else{
    keyboardAnimation(0,5,0,14,keys);
  }
}

void toggleDrumPads(){
  drumPads = !drumPads;
  clearButtons();
  if(drumPads){
    if(keys){
      toggleKeys();
    }
    drumPadAnimation(screenWidth-25,5,36,16, drumPads);
    drums();
  }
  else{
    drumPadAnimation(screenWidth-25,5,36,16, drumPads);
  }
}

void toggleNote(int track, int time, int length){
  if(lookupData[track][time] == 0 || (lookupData[track][time] != 0 && seqData[track][lookupData[track][time]].startPos != time)){
    if(!playing && !recording)
      makeNote(track, time, length, true);
    else
      makeNote(track, time, length, false);
  }
  else if(time == seqData[track][lookupData[track][time]].startPos){
    deleteNote(track, time);
  }
  updateLEDs();
}

//gets a note from data, returns a note object
Note getNote(int track, int timestep) {
  unsigned short int noteID;
  if (lookupData[track][timestep] != 0) {
    noteID = lookupData[track][timestep];
    Note targetNote = seqData[track][noteID];
    return targetNote;
  }
  else {
    Serial.print("No note, your majesty\n");
    return offNote;
  }
}

void changeVel(int amount){
  if(selectionCount>0){
    changeVelSelected(amount);
    //see "changeChance() for explanation"
    if(!seqData[activeTrack][lookupData[activeTrack][cursorPos]].isSelected){
      changeVel(lookupData[activeTrack][cursorPos], activeTrack, amount);
    }
  }
  else
    changeVel(lookupData[activeTrack][cursorPos], activeTrack, amount);
}

void changeVelSelected(int amount){
  for(int track = 0; track<trackData.size(); track++){
    for(int note = seqData[track].size()-1; note>0; note--){
      if(seqData[track][note].isSelected){
        changeVel(note,track, amount);
      }
    }
  }
}

void changeVel(int id, int track, int amount) {
  int vel = seqData[track][id].velocity;
  vel += amount;
  if(vel>=127)
    seqData[track][id].velocity = 127;
  else if(vel<0)
    seqData[track][id].velocity = 0;
  else
    seqData[track][id].velocity = vel;
}

void changeChance(int amount){
  if(selectionCount>0){
    changeChanceSelected(amount);
    //if the cursor note isn't selected
    //it still needs to be changed (but don't change it twice!)
    if(!seqData[activeTrack][lookupData[activeTrack][cursorPos]].isSelected)
      changeChance(lookupData[activeTrack][cursorPos],activeTrack,amount);
  }
  else{
    changeChance(lookupData[activeTrack][cursorPos],activeTrack,amount);
  }
}

void changeChanceSelected(int amount){
  for(uint16_t track = 0; track<trackData.size(); track++){
    for(uint16_t note = seqData[track].size()-1; note>0; note--){
      if(seqData[track][note].isSelected){
        changeChance(note,track, amount);
      }
    }
  }
}

void changeChance(int noteID, int track, int amount){
  int8_t chance = seqData[track][noteID].chance;
  chance += amount;
  if(chance>=100)
    seqData[track][noteID].chance = 100;
  else if(chance<0)
    seqData[track][noteID].chance = 0;
  else
    seqData[track][noteID].chance = chance;
}

//Tracks ------------------------------------------------------------------------

//above index is an INCLUSIVE lower bound!
int getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex){
  for(int i=aboveIndex ; i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return 0;
}
//returns id of track with a specific pitch, returns -1 if track doesn't exist
int getTrackWithPitch(int pitch, int channel){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch && trackData[i].channel == channel)
      return i;
  }
  return -1;
}
int getTrackWithPitch(int pitch){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}

int makeTrackWithPitch(int pitch, int channel){
  int track  = getTrackWithPitch(pitch,channel);
  if(track == -1){
    track = addTrack_return(pitch, channel, false);
  }
  return track;
}

void addTrack(Track newTrack, bool loudly){
  if(trackData.size()<255){
    trackData.push_back(newTrack);

    //widening lookupData
    lookupData.resize(trackData.size());
    //filling with 0
    lookupData[trackData.size()-1].resize(seqEnd,0);

    //widening seqData
    seqData.resize(trackData.size());
    seqData[trackData.size()-1] = defaultVec;
    activeTrack = trackData.size()-1;

    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}
void addTrack_noMove(Track newTrack, bool loudly){
  if(trackData.size()<255){
    trackData.push_back(newTrack);

    //widening lookupData
    lookupData.resize(trackData.size());
    //filling with 0
    lookupData[trackData.size()-1].resize(seqEnd,0);

    //widening seqData
    seqData.resize(trackData.size());
    seqData[trackData.size()-1] = defaultVec;

    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}

//this function should add a new row to the seqData, and lookupData initialized with 64 zeroes
void addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly){
  if(trackData.size()<255){
    Track newTrack(pitch, channel);
    newTrack.isLatched = latch;
    newTrack.muteGroup = muteGroup;
    newTrack.isPrimed = primed;
    trackData.push_back(newTrack);//adding track to vector data
    seqData.resize(trackData.size()); //adding row to seqData
    lookupData.resize(trackData.size()); //adding a row to the lookupData vector, to store lookupID's for notes in this track
    lookupData[trackData.size()-1].resize(seqEnd, blank_ID);//adding all the columns the track holds to the lookupData
    seqData[trackData.size()-1] = defaultVec;//setting the note data for the new track to the default blank data
    activeTrack = trackData.size()-1;
    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}
void addTrack(unsigned char pitch, unsigned char channel, bool loudly) {
  addTrack(pitch, channel, false, 0, true, loudly);
}
void addTrack(unsigned char pitch) {
  addTrack(pitch, defaultChannel, false);
}
void addTrack(unsigned char pitch, bool loudly) {
  addTrack(pitch, defaultChannel, loudly);
}
int16_t addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly) {
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    trackData.push_back(newTrack);
    seqData.resize(trackData.size());
    lookupData.resize(trackData.size());
    lookupData[trackData.size()-1].resize(seqEnd, blank_ID);
    seqData[trackData.size()-1] = defaultVec;
    activeTrack = trackData.size()-1;
    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
    return (activeTrack);
  }
  else{
    return -1;
  }
}

void dupeTrack(unsigned short int track){
  if(trackData.size()<256){
    Track newTrack = trackData[track];
    trackData.push_back(newTrack);
    lookupData.push_back(lookupData[track]);
    seqData.push_back(seqData[track]);
  }
}

//erases notes, but doesn't delete track
void eraseTrack() {
  seqData[activeTrack].resize(1);//truncating note data to just the first, blank note
  for (int i = 0; i < seqEnd; i++) {
    clearSelection(activeTrack, i);
    lookupData[activeTrack][i] = blank_ID;
  }
}
void eraseTrack(int track) {
  seqData[track].resize(1);
  for (int i = 0; i < seqEnd; i++) {
    clearSelection(track, i);
    lookupData[track][i] = blank_ID;
  }
}
//erases every track
void eraseSeq() {
  Serial.print("erasing seq\n");
  for (int i = 0; i < trackData.size(); i++) {
    eraseTrack(i);
  }
  clearSelection();
}

void deleteEmptyTracks(){
  for(int i = 0; i<trackData.size(); i++){
    if(seqData[i].size()-1 == 0 && trackData.size()>1){
      deleteTrack(i);
      i--;
    }
  }
  if(activeTrack>=trackData.size())
    activeTrack = trackData.size()-1;
}

//CHECK do you need to update trackID's??? idk if this will cause problems
//deletes the track AND notes stored within it (from seqData and lookupData)
//as long as you delete tracks from the back, i think this is okay
void deleteTrack(unsigned short int track){
  deleteTrack(track,false);
}

void deleteTrack(unsigned short int track, bool hard, bool askFirst){
  int choice = 1;
  if(askFirst && seqData[track].size()-1>0){
    vector<String> ops = {"nay","yeah"};
    choice = binarySelectionBox(64,32,"nay","yeah","delete track w/"+stringify(seqData[track].size()-1)+" note(s)?");
  }
  if(choice == 1){
    if(trackData.size() == 1 && !hard){
      eraseTrack(track);
      return;
    }
    //if the end track is within view
    if(endTrack == trackData.size()){
      endTrack--;
      if(startTrack>0)
        startTrack--;
    }
    if(activeTrack == track && activeTrack>0){
      activeTrack--;
    }

    eraseTrack(track);

    //making new data without the deleted track
    vector<Track> tempTrackData;
    vector<vector<Note>> tempSeqData;
    vector<vector<uint16_t>> tempLookupData;
    for(uint8_t t = 0; t<trackData.size(); t++){
      if(t != track){
        tempTrackData.push_back(trackData[t]);
        tempSeqData.push_back(seqData[t]);
        tempLookupData.push_back(lookupData[t]);
      }
    }
    trackData.swap(tempTrackData);
    seqData.swap(tempSeqData);
    lookupData.swap(tempLookupData);
    // trackData.erase(trackData.begin() + track);
    // seqData.erase(seqData.begin() + track);
    // lookupData.erase(lookupData.begin() + track);
  }
}

void deleteTrack(unsigned short int track, bool hard){
  deleteTrack(track, hard, true);
}
//deletes all tracks
void deleteAllTracks(){
  while(trackData.size()>0){
    deleteTrack(0,true,false);
  }
}

void shrinkTracks(){
  isShrunk = !isShrunk;
  //if it's already shrunk, toggle it
  if(!isShrunk){
    if(trackData.size()>6){
        maxTracksShown = 6;
        startTrack = 0;
        endTrack = 5;
      }
    else{
      maxTracksShown = trackData.size();
      startTrack = 0;
      endTrack = trackData.size()-1;
    }
    // debugHeight = 16;
  }
  //if it's not shrunk, shrink em
  else{
    maxTracksShown = 16;
    startTrack = 0;
    endTrack = 16;
    // debugHeight = 8;
  }
  setActiveTrack(0,false);
}

void transposeAllChannels(int increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackChannel(i,trackData[i].channel+increment,true);//only the active track makes a noise
    else
      setTrackChannel(i,trackData[i].channel+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackChannel(int track, int channel, bool loud){
  if(channel>=1 && channel<=16){
    sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].channel = channel;
    if(loud){
      sendMIDInoteOn(trackData[track].pitch,63,trackData[track].channel);
      sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

void transposeAllPitches(int increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackPitch(i,trackData[i].pitch+increment,true);//only the active track makes a noise
    else
      setTrackPitch(i,trackData[i].pitch+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackPitch(int track, int note, bool loud) {
  if(note>=0 && note<=120){
    sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].pitch = note;
    if(loud){
      sendMIDInoteOn(trackData[track].pitch,63,trackData[track].channel);
      sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(trackData.size());
  menuIsActive = false;
  bool movingBetweenNotes = false;
  while(true){
    readButtons();
    joyRead();
    //selectionBox
    //when sel is pressed and stick is moved, and there's no selection box
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      if(selBox.x1>selBox.x2){
        unsigned short int x1_old = selBox.x1;
        selBox.x1 = selBox.x2;
        selBox.x2 = x1_old;
      }
      if(selBox.y1>selBox.y2){
        unsigned short int y1_old = selBox.y1;
        selBox.y1 = selBox.y2;
        selBox.y2 = y1_old;
      }
      for(int track = selBox.y1; track<=selBox.y2; track++){
        for(int time = selBox.x1; time<=selBox.x2; time++){
          if(lookupData[track][time] != 0){
            //if the note isn't in the vector yet, add it
            if(!isInVector(lookupData[track][time],selectedNotes[track]))
              selectedNotes[track].push_back(lookupData[track][time]);
            time = seqData[track][lookupData[track][time]].endPos;
          }
        }
      }
      selBox.begun = false;
    }
    if(itsbeen(100)){
      if(!movingBetweenNotes){
        if(y == 1){
          setActiveTrack(activeTrack+1,true);
          lastTime = millis();
        }
        if(y == -1){
          setActiveTrack(activeTrack-1,true);
          lastTime = millis();
        }
      }
    }
    if(itsbeen(100)){
      if(x != 0){
        if(!movingBetweenNotes){
          if (x == 1 && !shift) {
            if(cursorPos%subDivInt){
              moveCursor(-cursorPos%subDivInt);
              lastTime = millis();
            }
            else{
              moveCursor(-subDivInt);
              lastTime = millis();
            }
          }
          if (x == -1 && !shift) {
            if(cursorPos%subDivInt){
              moveCursor(subDivInt-cursorPos%subDivInt);
              lastTime = millis();
            }
            else{
              moveCursor(subDivInt);
              lastTime = millis();
            }
          }
        }
        else{
          if(x == 1){
            moveToNextNote(false,false);
            lastTime = millis();
          }
          else if(x == -1){
            moveToNextNote(true,false);
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      //select
      if(sel && lookupData[activeTrack][cursorPos] != 0 && !selBox.begun){
        unsigned short int id;
        id = lookupData[activeTrack][cursorPos];
        if(shift){
          //delete old vec
          vector<vector<uint8_t>> temp;
          temp.resize(trackData.size());
          selectedNotes = temp;
          selectedNotes[activeTrack].push_back(lookupData[activeTrack][cursorPos]);
        }
        else{
          //if the note isn't in the vector yet, add it
          if(!isInVector(lookupData[activeTrack][cursorPos],selectedNotes[activeTrack]))
            selectedNotes[activeTrack].push_back(lookupData[activeTrack][cursorPos]);
          //if it is, remove it
          else{
            vector<uint8_t> temp;
            for(int i = 0; i<selectedNotes[activeTrack].size(); i++){
              //push back all the notes that aren't the one the cursor is on
              if(selectedNotes[activeTrack][i] != lookupData[activeTrack][cursorPos]){
                temp.push_back(selectedNotes[activeTrack][i]);
              }
            }
            selectedNotes[activeTrack] = temp;
          }
        }
        lastTime = millis();
      }
      if(del){
        lastTime = millis();
        selectedNotes.clear();
        break;
      }
      if(n){
        n = false;
        lastTime = millis();
        break;
      }
      if(loop_Press){
        lastTime = millis();
        movingBetweenNotes = !movingBetweenNotes;
      }
    }
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    printSmall(0,0,text1,SSD1306_WHITE);
    printSmall(0,8,text2,SSD1306_WHITE);
    if(movingBetweenNotes){
      if(millis()%1000 >= 500){
        display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
      }
      else{
        display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
      }
    }
    //draw a note bracket on any note that's been added to the selection
    for(int track = 0; track<selectedNotes.size(); track++){
      for(int note = 0; note<selectedNotes[track].size(); note++){
        // Serial.println("drawing bracket on track "+stringify(track)+", note "+stringify(note));
        drawNoteBracket(seqData[track][selectedNotes[track][note]],track);
      }
    }
    display.display();
  }
  menuIsActive = true;
  return selectedNotes;
}

vector<uint8_t> selectSeqArea(String text){
  vector<uint8_t> selection;
  while(true){
    display.clearDisplay();
    drawSeq(true, true, true, true, true);
    display.display();
    readButtons();
    joyRead();
    //selectionBox
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      if(selBox.x1>selBox.x2){
        unsigned short int x1_old = selBox.x1;
        selBox.x1 = selBox.x2;
        selBox.x2 = x1_old;
      }
      if(selBox.y1>selBox.y2){
        unsigned short int y1_old = selBox.y1;
        selBox.y1 = selBox.y2;
        selBox.y2 = y1_old;
      }
      selection.push_back(selBox.x1);
      selection.push_back(selBox.y1);
      selection.push_back(selBox.x2);
      selection.push_back(selBox.y2);
      selBox.begun = false;
      break;
    }
    while(counterA != 0){
      //changing zoom
      if(counterA >= 1 && !shift && !track_Press){
          zoom(true);
        }
      if(counterA <= -1 && !shift && !track_Press){
        zoom(false);
      }
      counterA += counterA<0?1:-1;;
    }
    while(counterB != 0){      
      if(counterB >= 1 && !shift){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(counterB <= -1 && !shift){
        changeSubDivInt(false);
      }
      //if shifting, toggle between 1/3 and 1/4 mode
      else while(counterB != 0 && shift){
        toggleTriplets();
      }
      counterB += counterB<0?1:-1;;
    }
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
        }
      }
    }
    if (itsbeen(50)) {
      //moving
      if (x == 1 && shift) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (x == -1 && shift) {
        moveCursor(1);
        lastTime = millis();
      }
      //changing vel
      if (y == 1 && shift) {
        changeVel(-10);
        lastTime = millis();
      }
      if (y == -1 && shift) {
        changeVel(10);
        lastTime = millis();
      }
    }
    if(itsbeen(60)){
      if(y == 1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
      if(y == -1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(del){
        break;
      }
    }
  }
  return selection;
}

//gen midi numbers taken from https://usermanuals.finalemusic.com/SongWriter2012Win/Content/PercussionMaps.htm
void drawDrumIcon(uint8_t x1, uint8_t y1, uint8_t note){
  int8_t which = -1;
  switch(note){
    //"bass drum" => 808
    case 35:
      which = 0;
      break;
    //"kick"
    case 36:
      which = 1;
      break;
    //cowbell
    case 34:
    case 56:
      which = 2;
      break;
    //clap
    case 39:
      which = 3;
      break;
    //crash cymbal
    case 49:
      which = 4;
      break;
    //"laser" ==> gun (sfx)
    case 27:
    case 28:
    case 29:
    case 30:
      which = 5;
      break;
    //closing hat pedal
    case 44:
      which = 6;
      break;
    //open hat
    case 46:
      which = 7;
      break;
    //closed hat
    case 42:
      which = 8;
      break;
    //rim
    case 31:
    case 32:
    case 37:
      which = 9;
      break;
    //shaker
    case 82:
      which = 10;
      break;
    //snare
    case 38:
    case 40:
      which = 11;
      break;
    //tomL
    case 45:
      which = 12;
      break;
    //tomM
    case 47:
    case 48:
      which = 13;
      break;
    //tomS
    case 50:
      which = 14;
      break;
    //triangle
    case 80:
    case 81:
      which = 15;
      break;
  }
  if(which != -1){
    display.drawBitmap(x1,y1,drum_icons[which],16,8,1,0);
  }
}

vector<uint8_t> selectMultipleTracks(String text){
  //clearing out the selected tracks
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].isSelected){
      trackData[i].isSelected = false;
    }
  }
  //to hold the id's and return
  vector<uint8_t> selection;
  while(true){
    display.clearDisplay();
    drawSeq(true, false, false, false, true, viewStart, viewEnd);
    printSmall(screenWidth-text.length()*4,0,text,1);

    display.setCursor(0,7);
    display.setFont(&FreeSerifItalic9pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.print("Trk");
    display.print(stringify(activeTrack+1));
    display.setFont();

    display.display();
    readButtons();
    joyRead();
    if(itsbeen(100)){
      if(y == 1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
      if(y == -1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(sel && !shift){
        sel = false;
        lastTime = millis();
        trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
      }
      //toggle the selection on all of them
      if(shift && sel){
        n = false;
        lastTime = millis();
        trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
        for(int i = 0; i<trackData.size(); i++){
          trackData[i].isSelected = trackData[activeTrack].isSelected;
        }
      }
      if(n && !shift){
        n = false;
        lastTime = millis();
        //adding all the selected tracks to the list
        for(int i = 0; i<trackData.size(); i++){
          if(trackData[i].isSelected){
            selection.push_back(i);
            trackData[i].isSelected = false;
          }
        }
        break;
      }
      if(del || menu_Press){
        lastTime = millis();
        for(uint8_t track = 0; track<trackData.size(); track++){
          trackData[track].isSelected = false;
        }
        selection.clear();
        break;
      }
    }
  }
  return selection;
}

vector<uint8_t> selectMultipleTracks(){
  String empty;
  return selectMultipleTracks(empty);
}

void muteTrack(unsigned short int id){
  trackData[id].isMuted = true;
}
void unmuteTrack(uint16_t id){
  trackData[id].isMuted = false;
}

void toggleMute(uint16_t id){
  trackData[id].isMuted = !trackData[id].isMuted;
}

void muteMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    muteTrack(ids[track]);
  }
}

void soloTrack(unsigned short int id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id){
      muteTrack(track);
      trackData[track].isSolo = false;
    }
    else 
      trackData[id].isSolo = true;
  }
}

void unsoloTrack(uint16_t id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id)
      unmuteTrack(track);
    else
      trackData[id].isSolo = false;
  }
}

void toggleSolo(uint16_t id){
  if(trackData[id].isSolo)
    unsoloTrack(id);
  else
    soloTrack(id);
}

void eraseMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    eraseTrack(ids[track]);
  }
}

void swapTracks(unsigned short int track1, unsigned short int track2){
  //making sure the tracks are real
  if(track1 < trackData.size() && track2 < trackData.size() && track1>=0 && track2 >= 0){
    //swapping track data
    Track old_activeTrack = trackData[track1];
    trackData[track1] = trackData[track2];
    trackData[track2] = old_activeTrack;
    //swapping lookupData
    vector<unsigned short int> old_lookupData = lookupData[track1];
    lookupData[track1] = lookupData[track2];
    lookupData[track2] = old_lookupData;
    //swapping sequence data
    vector<Note> old_seqData = seqData[track1];
    seqData[track1] = seqData[track2];
    seqData[track2] = old_seqData;
  }
}

void swapTracks(){
  slideMenuOut(1,7);
  unsigned short int track1 = activeTrack;
  sel = false;
  while(true){
    if(itsbeen(60)){
      if(y == 1){
        swapTracks(track1,activeTrack+1);
        setActiveTrack(activeTrack+1,true);
        track1 = activeTrack;
        lastTime = millis();
      }
      if(y == -1){
        swapTracks(track1,activeTrack-1);
        setActiveTrack(activeTrack-1,true);
        track1 = activeTrack;
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(sel || menu_Press){
        sel = false;
        menu_Press = false;
        lastTime = millis();
        break;
      }
    }
    joyRead();
    readButtons();
    display.clearDisplay();
    drawSeq();
    if(millis()%1000 >= 500){
      display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
    }
    display.display();
  }
  slideMenuIn(1,7);
}

//Debug -------------------------------------------------------------------------
//counts notes
void debugNoteCount() {
  int totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes += seqData[i].size()-1;
  }
  Serial.print(totalNotes + " notes stored in memory\n");
}

//prints the selection buffer, doe
void debugPrint() {
  Serial.println("Printing out seqData...");
  for(int i = 0; i<trackData.size(); i++){
    for(int j = 1; j<seqData[i].size()-1; j++){
      Serial.println("+------------+");
      Serial.print("id: ");
      Serial.println(j);
      Serial.print("start: ");
      Serial.println(seqData[i][j].startPos);
      Serial.print("end: ");
      Serial.println(seqData[i][j].endPos);
    }
  }
  Serial.print("total notes on this track: ");
  Serial.print(int(seqData[activeTrack].size()-1));
  Serial.print("total notes in the sequence: ");
  int totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes = totalNotes + seqData[i].size()-1;
  }
  Serial.print(totalNotes);
  Serial.print("\nNotes in data: ");
  totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes = seqData[i].size() - 1 + totalNotes;
  }
  Serial.print(totalNotes);
}

//fills sequence with notes
void debugFillSeq() {
  Serial.print("filling with notes\n");
  for (int i = 0; i < trackData.size(); i++) {
    for (int j = 0; j < 254; j++) {
      makeNote(i, j, subDivInt, 0);
    }
  }
  debugNoteCount();
}

//prints out memory allocation
void debugPrintMem(bool verby){
  if(verby){
    Serial.print("tracks:");
    Serial.println(seqData.size());
    Serial.print("notes:");
    for(int i = 0; i<trackData.size(); i++){
      Serial.print(i);
      Serial.print(" - ");
      Serial.println(seqData[i].size());
    }
  }
  Serial.print("free:");
  Serial.println(rp2040.getFreeHeap());
  Serial.print("used:");
  Serial.println(rp2040.getUsedHeap());
  // Serial.print("total:");
  // Serial.println(rp2040.getTotalHeap());
  Serial.print("% used:");
  Serial.println(float(rp2040.getUsedHeap())/float(rp2040.getTotalHeap()));
}
//fills sequence with notes
void debugFillTrack(int division) {
  Serial.print("placing note every ");
  Serial.print(division);
  Serial.print(" slices...\n");
  for (int j = 0; j < 254; j += division) {
    makeNote(activeTrack, j, subDivInt, 0);
  }
}
void addTimeToSeq(uint16_t amount, uint16_t insertPoint){
  //if it'll cause a wrap around
  if(seqEnd+amount>65535)
    return;
  //extend seq
  seqEnd+=amount;

  //move through notes that appear AFTER the insert point and move them back
  //sweeping from old seq end (there should be no notes after that)-->insertPoint
  for(uint8_t t = 0; t<trackData.size(); t++){
    //extend lookupdata
    lookupData[t].resize(seqEnd,0);
    for(uint16_t i = seqEnd-amount; i>insertPoint; i--){
      uint16_t id = lookupData[t][i];
      //if there's a note there
      if(id != 0){
        //if it starts after the insert point, move the whole note
        if(seqData[t][id].startPos>insertPoint){
          //set i to old beginning of note (so you definitely don't hit it twice)
          i = seqData[t][id].startPos;
          moveNote(id,t,t,seqData[t][id].startPos+amount);
        }
        //if the insert point intersects it somehow, truncate it
        else{
          truncateNote(t,insertPoint);
          //break bc that was the last note
          break;
        }
      }
    }
  }
  
}

//counts notes
uint16_t countNotesInRange(uint16_t start, uint16_t end){
  uint16_t count = 0;
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there are no notes, ignore it
    if(seqData[t].size() == 1)
      continue;
    else{
      //move over each note
      for(uint16_t i = 1; i<seqData.size(); i++){
        if(seqData[t][i].startPos>=start && seqData[t][i].startPos<end)
          count++;
      }
    }
  }
  return count;
}
//insert or cut time from the seq
//cursor+new inserts time
//cursor+del deletes time
//selbox+new duplicates time
//selbox+del deletes time
//selbox stays until new selbox is made
void cutInsertTime(){
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        return;
      }
    }
  }
}

void removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
  //if you're trying to delete more than (or as much as) exists! just return
  if(amount>=seqEnd)
    return;
  //if you're trying to delete more than exists between insertpoint --> seqEnd,
  //then set amount to everything there
  if(insertPoint+amount>seqEnd){
    amount = seqEnd-insertPoint;
  }
  //move through 'deleted' area and clear out notes
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there are no notes, skip this track
    if(seqData[t].size() == 1)
      continue;
    for(uint16_t i = insertPoint+1; i<insertPoint+amount; i++){
      uint16_t id = lookupData[t][i];
      //if there's a note there
      if(id != 0){
        //if it starts before/at insert point, truncate it
        if(seqData[t][id].startPos<=insertPoint){
          truncateNote(t,insertPoint);
          //break bc you know that was the last note
          break;
        }
        //if it's in the delete area, delete it
        else if(seqData[t][id].startPos>insertPoint && seqData[t][id].startPos<=insertPoint+amount){
          deleteNote_byID(t,id);
        }
      }
    }
  }
  //move notes that fall beyond the delete area
  for(uint8_t t = 0; t<trackData.size(); t++){
    //if there're no notes, skip this track
    if(seqData[t].size() == 1)
      continue;
    for(uint16_t i = insertPoint+amount; i<seqEnd; i++){
      uint16_t id = lookupData[t][i];
      //if there's a note here, move it back by amount
      if(id != 0){
        moveNote(id,t,t,seqData[t][id].startPos-amount);
        i = seqData[t][id].endPos-1;
      }
    }
    //resize lookupData
    lookupData[t].resize(seqEnd-amount);
  }
  seqEnd -= amount;
  //fixing loop point positions
  for(uint8_t loop = 0; loop<loopData.size(); loop++){
    //if start or end are past seqend, set to seqend
    if(loopData[loop][0]>seqEnd)
      loopData[loop][0] = seqEnd;
    if(loopData[loop][1]>seqEnd)
      loopData[loop][1] = seqEnd;
  }
  //make sure view stays within seq
  // if(viewEnd > seqEnd){
  //   viewEnd = seqEnd;
  // }
  checkView();
}

//this function needs to either trash all the data beyond the new sequence end, or 
void setSeqEnd(int newEnd){
  if(newEnd>6144){
    newEnd = 6144;
  }
  if(newEnd == seqEnd){
    return;
  }
  //expansion
  if(newEnd>seqEnd){
    for(int track = 0; track<trackData.size(); track++){
      lookupData[track].resize(newEnd,0);
    }
    seqEnd = newEnd;
    return;
  }
  //contraction
  else if(newEnd<seqEnd){
    vector<vector<unsigned short int>> newLookupData;
    newLookupData.resize(trackData.size());
    //deleting everything after the new end, copying into smaller vectors what isn't
    for(int step = seqStart; step<=seqEnd; step++){
      for(int track = 0; track<trackData.size(); track++){
        //deleting
        if(step>newEnd){
          if(lookupData[track][step] != 0){
            deleteNote(track, step);
          }
        }
        //copying
        else{
          //save into new vector
          newLookupData[track].push_back(lookupData[track][step]);
        }
      }
    }
    lookupData.swap(newLookupData);
    seqEnd = newEnd;
    //handling bounds on shit
    if(cursorPos>seqEnd){
      cursorPos = seqEnd;
    }
    if(playheadPos > seqEnd){
      playheadPos = seqEnd;
    }
    if(recheadPos > seqEnd){
      recheadPos = seqEnd;
    }
    if(viewEnd > seqEnd){
      viewEnd = seqEnd;
      // viewLength = viewEnd - viewStart;
    }
    //handling loop bounds
    for(int loop = 0; loop<loopData.size(); loop++){
      if(loopData[loop][1]>seqEnd){
        loopData[loop][1] = seqEnd;
      }
      if(loopData[activeLoop][1]>seqEnd){
        loopData[activeLoop][1] = seqEnd;
      }
    }
  }
  checkView();
}

//Sequence parameters -----------------------------------------------------------
void initSeq(int tracks, int length) {

  offNote.startPos = 0;
  offNote.endPos = 0;
  offNote.velocity = 0;

  defaultChannel = 1;
  defaultPitch = 36;

  bpm = 120;
  seqStart = 0;
  //default is 4 measures = 24*4*4=384
  //1 measure  = 96
  seqEnd = length;
  //6144 = 64 measures
  // seqEnd = 6144;
  // seqEnd = 28800;//<--can it run this?? theoretical max is 65,000 before overflow


  loopData[activeLoop][0] = 0;
  loopData[activeLoop][1] = 192;
  isLooping = 1;
  loopData[activeLoop][2] = 0;
  activeLoop = 0;

  loopData[0][0] = loopData[activeLoop][0];
  loopData[0][1] = loopData[activeLoop][1];
  loopData[0][2] = loopData[activeLoop][2];

  viewStart = 0;
  viewEnd = 192;
  // viewLength = viewEnd-viewStart;
  subDivInt = subDivInt; //default note length

  counterA = 0;
  counterB = 0;

  cursorPos = viewStart; //cursor position (before playing)
  activeTrack = 0; //sets which track you're editing, you'll only be able to edit one at a time
  subDivInt = 24;//sets where the divider bars are in the console output
  //cursor jump is locked to this division
  defaultVel = 127;

  MicroSperTimeStep = round(2500000/bpm);

  seqData.resize(tracks);
  lookupData.resize(tracks);

  //this is so we can count down, instead of up
  defaultPitch += tracks-1;

  for(int i = 0; i < tracks; i++){
    lookupData[i].resize(seqEnd+1, blank_ID);
    seqData[i] = defaultVec; //each new track is defaultVec, with 0 notes
    Track newTrack;
    trackData.push_back(newTrack);
    defaultPitch--;
  }
}
void initSeq_SP404(int tracks) {
  offNote.startPos = 0;
  offNote.endPos = 0;
  offNote.velocity = 0;

  defaultChannel = 1;
  defaultPitch = 0;

  bpm = 120;
  seqStart = 0;
  //default is 4 measures = 24*4*4=384
  seqEnd = 768;

  loopData[activeLoop][0] = 0;
  loopData[activeLoop][1] = 192;
  isLooping = 1;
  loopData[activeLoop][2] = 1;
  activeLoop = 0;

  viewStart = 0;
  viewEnd = 192;
  // viewLength = viewEnd-viewStart;
  subDivInt = subDivInt; //default note length

  counterA = 0;
  counterB = 0;

  cursorPos = viewStart; //cursor position (before playing)
  activeTrack = 0; //sets which track you're editing, you'll only be able to edit one at a time
  subDivInt = 24;//sets where the divider bars are in the console output
  //cursor jump is locked to this division
  defaultVel = 127;

  MicroSperTimeStep = round(2500000/(bpm));

  vector<unsigned short int> loop1{loopData[activeLoop][0],loopData[activeLoop][1]};
  loopData.push_back(loop1);
  seqData.resize(tracks);
  lookupData.resize(tracks);

  //this is so we can count down, instead of up

  for(int i = 0; i < tracks; i++){
    lookupData[i].resize(seqEnd+1, blank_ID);
    seqData[i] = defaultVec; //each new track is defaultVec, with 0 notes
    Track newTrack((16-i%16),(i/16));
    trackData.push_back(newTrack);
    defaultPitch--;
  }
}

void initSeq(){
  initSeq(4,768);
}

void debugFillSequence(){
  for(uint16_t step = 0; step<seqEnd; step++){
    Note debugNote(step,step,127,100,false,false);
    for(uint8_t track = 0; track<trackData.size(); track++){
      makeNote(debugNote,track,false);
    }
  }
}
void newSeq(){
  selectionCount = 0;
  vector<vector<uint16_t>> newLookupData;
  newLookupData.swap(lookupData);
  vector<vector<Note>> newSeqData;
  newSeqData.swap(seqData);
  vector<Track> newTrackData;
  newTrackData.swap(trackData);
}
#ifdef HEADLESS
  void flashTest(){
    return;
  }
  void writeSeqFile(String fname){
    return;
  }
  void writeBytes(uint8_t* byteArray, uint16_t number){
    return;
  }
  void writeCurrentSeqToSerial(bool w){
    return;
  }
  void sendByteCount(String filename){
    return;
  }
  void sendFileName(String filename){
    return;
  }
  void exportSeqFileToSerial_standAlone(String filename){
    return;
  }
  void exportSeqFileToSerial(String filename){
    return;
  }
  void dumpFilesToSerial(){
    return;
  }
  uint32_t getByteCount_standAlone(String filename){
    return 0;
  }
  uint32_t getByteCount(String filename){
    return 0;
  }
  void writeSeqSerial_plain(){
    return;
  }
  void loadSeqFile(String filename){
    return;
  }
  vector<String> loadFilesAndSizes(){
    vector<String> fileSizes = {"0"};
    return fileSizes;
  }
  void loadFiles(){
    return;
  }
  void renameSeqFile(String filename){
    return;
  }
  bool deleteSeqFile(String filename){
    return false;
  }
  void duplicateSeqFile(String filename){
    return;
  }
#endif
#ifndef HEADLESS
//flash mem
void flashTest(){
  char totalNotes;
  for(int i = 0; i<trackData.size(); i++){
    totalNotes += seqData[i].size()-1;
  }

  LittleFS.begin();

  //reading
  unsigned char buff[32];
  File saveFile = LittleFS.open("test.txt","r");
  if(saveFile){
    Serial.println("Reading:");
    saveFile.read(buff, 31);
    for(int i = 0; i<32; i++){
      Serial.println(buff[i]);
    }
  }
  else
    Serial.println("couldn't open save :/");
  saveFile.close();

  //writing
  if(LittleFS.exists("test.txt")){
    Serial.println("attempting...");
    File file2 = LittleFS.open("test.txt","w");
    if(file2){
      Serial.println("Writing...");
      for(int i = 0; i<32; i++){
        buff[i] = millis();
      }
      file2.write(buff,32);
      file2.close();
    }
  }
  Serial.println("ending...");
  LittleFS.end();
}

void writeSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"w");

  //writing seq info
  //start, end
  uint8_t start[2] = {uint8_t(seqStart>>8),uint8_t(seqStart)};
  uint8_t end[2] = {uint8_t(seqEnd>>8),uint8_t(seqEnd)};
  seqFile.write(start,2);
  seqFile.write(end,2);
  //writing track info
  //(number of tracks)
  uint8_t t[1] = {uint8_t(trackData.size())};
  seqFile.write(t,1);
  for(int track = 0; track<trackData.size(); track++){
    //(noteCount1)(noteCount2),(pitch),(channel),(latched),(muteGroup),(primed)
    uint8_t trackDat[7] = {uint8_t((seqData[track].size()-1)>>8),uint8_t(seqData[track].size()-1),uint8_t(trackData[track].pitch),uint8_t(trackData[track].channel),uint8_t(trackData[track].isLatched),uint8_t(trackData[track].muteGroup),uint8_t(trackData[track].isPrimed)};
    seqFile.write(trackDat,7);
  }
  //writing note info
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note < seqData[track].size(); note++){
      //notes are stored start, end,
      //and then vel, chance, selected, muted
      //                              first 8 bits                          last 8 bites
      uint8_t notePosData[4] = {uint8_t(seqData[track][note].startPos>>8),uint8_t(seqData[track][note].startPos),uint8_t(seqData[track][note].endPos>>8),uint8_t(seqData[track][note].endPos)};
      uint8_t noteData[4] = {uint8_t(seqData[track][note].velocity), uint8_t(seqData[track][note].chance), uint8_t(seqData[track][note].isSelected), uint8_t(seqData[track][note].muted)};
      seqFile.write(notePosData,4);
      seqFile.write(noteData,4);
    }
  }
  
  //writing dataTrack info, if there are any dataTracks
  //number of bytes is split in two! because it's a uint16_t
  //stored as (number of dataTracks),(control),(channel),(isActive),(number of bytes1),(number of bytes2),(data)...
  //number of datatracks is always written, even if it's zero!
  uint8_t numOfDataTracks[1] = {uint8_t(dataTrackData.size())};
  seqFile.write(numOfDataTracks,1);
  if(dataTrackData.size()>0){      
    //writing dt data
    for(uint8_t dt = 0; dt<numOfDataTracks[0]; dt++){
      uint8_t dataTrackInfoData[4] = {dataTrackData[dt].control,dataTrackData[dt].channel,dataTrackData[dt].isActive,dataTrackData[dt].parameterType};
      uint8_t numberOfDataPoints[2] = {uint8_t(dataTrackData[dt].data.size()>>8),uint8_t(dataTrackData[dt].data.size())};
      uint8_t dataTrackRawData[dataTrackData[dt].data.size()];
      for(uint16_t dataPoint = 0; dataPoint<dataTrackData[dt].data.size(); dataPoint++){
        dataTrackRawData[dataPoint] = dataTrackData[dt].data[dataPoint];
      }
      seqFile.write(dataTrackInfoData,4);
      seqFile.write(numberOfDataPoints,2);
      seqFile.write(dataTrackRawData,dataTrackData[dt].data.size());
    }
  }

  //writing loop data
  uint8_t numberOfLoops[1] = {uint8_t(loopData.size())}; 
  seqFile.write(numberOfLoops,1);
  if(loopData.size()>0){
    //loops are stored as
    //(number of loops),(start1),(start2),(end1),(end2),(iterations),(style)
    for(uint8_t loop = 0; loop<loopData.size(); loop++){
      uint8_t start[2] = {uint8_t(loopData[loop][0]>>8),uint8_t(loopData[loop][0])};
      uint8_t end[2] = {uint8_t(loopData[loop][1]>>8),uint8_t(loopData[loop][1])};
      uint8_t loopDat[2] = {uint8_t(loopData[loop][2]),uint8_t(loopData[loop][3])};
      seqFile.write(start,2);
      seqFile.write(end,2);
      seqFile.write(loopDat,2);
    }
  }
  //writing clock data
  //bpm
  uint8_t bpmBytes[2] = {uint8_t(bpm>>8),uint8_t(bpm)};
  seqFile.write(bpmBytes,2);
  //swing amount
  uint8_t swingAmountBytes[2] = {uint8_t(swingVal>>8),uint8_t(swingVal)};
  seqFile.write(swingAmountBytes,2);
  //swing subDiv
  uint8_t swingSubDivBytes[2] = {uint8_t(swingSubDiv>>8),uint8_t(swingSubDiv)};
  seqFile.write(swingSubDivBytes,2);
  //swing on/off
  uint8_t swingByte[1] = {uint8_t(swung)};
  seqFile.write(swingByte,1);

  //writing routing data
  uint8_t midiChannelBytes[10] = {uint8_t(midiChannels[0]>>8),uint8_t(midiChannels[0]),
                          uint8_t(midiChannels[1]>>8),uint8_t(midiChannels[1]),
                          uint8_t(midiChannels[2]>>8),uint8_t(midiChannels[2]),
                          uint8_t(midiChannels[3]>>8),uint8_t(midiChannels[3]),
                          uint8_t(midiChannels[4]>>8),uint8_t(midiChannels[4])};
  seqFile.write(midiChannelBytes,10);
  
  //writing thru data
  uint8_t midiThruBytes[5];
  for(uint8_t port = 0; port<5; port++){
    midiThruBytes[port] = isThru(port);
  }
  seqFile.write(midiThruBytes,5);

  Serial.println("done");
  Serial.flush();
  seqFile.close();
  LittleFS.end();
}

//helper function for writing bytes to the serial buffer
void writeBytes(uint8_t* byteArray, uint16_t number){
  // for(uint16_t byte = 0; byte<number; byte++){
  //   Serial.write(byteArray[byte]);
  //   Serial.flush();//idk if you need this
  // }
  Serial.write(byteArray,number);
}

void writeCurrentSeqToSerial(bool waitForResponse){
  //clearing serial buffer
  Serial.flush();

  //writing track info
  //(number of tracks)
  uint8_t t[1] = {uint8_t(trackData.size())};
  writeBytes(t,1);
  for(int track = 0; track<trackData.size(); track++){
    //(noteCount1)(noteCount2),(pitch),(channel),(latched),(muteGroup),(primed)
    uint8_t trackDat[7] = {uint8_t((seqData[track].size()-1)>>8),uint8_t(seqData[track].size()-1),uint8_t(trackData[track].pitch),uint8_t(trackData[track].channel),uint8_t(trackData[track].isLatched),uint8_t(trackData[track].muteGroup),uint8_t(trackData[track].isPrimed)};
    writeBytes(trackDat,7);
  }
  //writing note info
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note < seqData[track].size(); note++){
      //notes are stored start, end,
      //and then vel, chance, selected, muted
      //                              first 8 bits                          last 8 bites
      uint8_t notePosData[4] = {uint8_t(seqData[track][note].startPos>>8),uint8_t(seqData[track][note].startPos),uint8_t(seqData[track][note].endPos>>8),uint8_t(seqData[track][note].endPos)};
      uint8_t noteData[4] = {uint8_t(seqData[track][note].velocity), uint8_t(seqData[track][note].chance), uint8_t(seqData[track][note].isSelected), uint8_t(seqData[track][note].muted)};
      writeBytes(notePosData,4);
      writeBytes(noteData,4);
    }
  }
  
  //writing dataTrack info, if there are any dataTracks
  //number of bytes is split in two! because it's a uint16_t
  //stored as (number of dataTracks),(control),(channel),(isActive),(number of bytes1),(number of bytes2),(data)...
  //number of datatracks is always written, even if it's zero!
  uint8_t numOfDataTracks[1] = {uint8_t(dataTrackData.size())};
  writeBytes(numOfDataTracks,1);
  if(dataTrackData.size()>0){      
    //writing dt data
    for(uint8_t dt = 0; dt<numOfDataTracks[0]; dt++){
      uint8_t dataTrackInfoData[3] = {dataTrackData[dt].control,dataTrackData[dt].channel,dataTrackData[dt].isActive};
      uint8_t numberOfDataPoints[2] = {uint8_t(dataTrackData[dt].data.size()>>8),uint8_t(dataTrackData[dt].data.size())};
      uint8_t dataTrackRawData[dataTrackData[dt].data.size()];
      for(uint16_t dataPoint = 0; dataPoint<dataTrackData[dt].data.size(); dataPoint++){
        dataTrackRawData[dataPoint] = dataTrackData[dt].data[dataPoint];
      }
      writeBytes(dataTrackInfoData,3);
      writeBytes(numberOfDataPoints,2);
      writeBytes(dataTrackRawData,dataTrackData[dt].data.size());
    }
  }

  //writing loop data
  uint8_t numberOfLoops[1] = {uint8_t(loopData.size())}; 
  writeBytes(numberOfLoops,1);
  if(loopData.size()>0){
    //loops are stored as
    //(number of loops),(start1),(start2),(end1),(end2),(iterations),(style)
    for(uint8_t loop = 0; loop<loopData.size(); loop++){
      uint8_t start[2] = {uint8_t(loopData[loop][0]>>8),uint8_t(loopData[loop][0])};
      uint8_t end[2] = {uint8_t(loopData[loop][1]>>8),uint8_t(loopData[loop][1])};
      uint8_t loopDat[2] = {uint8_t(loopData[loop][2]),uint8_t(loopData[loop][3])};
      writeBytes(start,2);
      writeBytes(end,2);
      writeBytes(loopDat,2);
    }
  }

  //writing clock data
  //bpm
  uint8_t bpmBytes[2] = {uint8_t(bpm>>8),uint8_t(bpm)};
  writeBytes(bpmBytes,2);
  //swing amount
  uint8_t swingAmountBytes[2] = {uint8_t(swingVal>>8),uint8_t(swingVal)};
  writeBytes(swingAmountBytes,2);
  //swing subDiv
  uint8_t swingSubDivBytes[2] = {uint8_t(swingSubDiv>>8),uint8_t(swingSubDiv)};
  writeBytes(swingSubDivBytes,2);
  //swing on/off
  uint8_t swingByte[1] = {uint8_t(swung)};
  writeBytes(swingByte,1);

  //writing routing data
  uint8_t midiChannelBytes[10] = {uint8_t(midiChannels[0]>>8),uint8_t(midiChannels[0]),
                          uint8_t(midiChannels[1]>>8),uint8_t(midiChannels[1]),
                          uint8_t(midiChannels[2]>>8),uint8_t(midiChannels[2]),
                          uint8_t(midiChannels[3]>>8),uint8_t(midiChannels[3]),
                          uint8_t(midiChannels[4]>>8),uint8_t(midiChannels[4])};
  writeBytes(midiChannelBytes,10);
  
  //writing thru data
  uint8_t midiThruBytes[5];
  for(uint8_t port = 0; port<5; port++){
    midiThruBytes[port] = isThru(port);
  }
  writeBytes(midiThruBytes,5);

}

//sends the number of bytes in the file
void sendByteCount(String filename){
  uint32_t byteCount = getByteCount(filename);
  uint8_t bytes[4] = {uint8_t(byteCount>>24),uint8_t(byteCount>>16),uint8_t(byteCount>>8),uint8_t(byteCount)};
  Serial.write(bytes,4);
}

void sendFileName(String filename){
  Serial.print('\n'+filename+'\n');
}

//handles the LittleFS begin and close actions on its own
void exportSeqFileToSerial_standAlone(String filename){
  LittleFS.begin();
  exportSeqFileToSerial(filename);
  LittleFS.end(); 
}

void exportSeqFileToSerial(String filename){
  // //first, send a newline
  // Serial.print('\n');

  //first, send the filename with a newline on either end
  sendFileName(filename);

  //second, send the number of bytes in the file
  sendByteCount(filename);

  //now you're ready to send ya data!
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    //start, end
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    writeBytes(start,2);
    writeBytes(end,2);

    //loading tracks
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    writeBytes(tracks,1);

    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      writeBytes(trackDat,7);
      noteCount.push_back((uint16_t(trackDat[0])<<8)+trackDat[1]);
    }

    //loading notes
    for(int track = 0; track<tracks[0]; track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        writeBytes(notePosData,4);
        writeBytes(noteData,4);
      }
    }
    
    //loading DT's
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    writeBytes(numberOfDts,1);
    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        writeBytes(dtInfo,4);
        writeBytes(numberOfPoints,2);

        //getting points
        uint16_t numberOfDtPoints = (numberOfPoints[0]<<8)+numberOfPoints[1];
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        writeBytes(points,numberOfDtPoints);
      }
    }
    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    writeBytes(numberOfLoops,1);
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      writeBytes(start,2);
      writeBytes(end,2);
      writeBytes(loopInfo,2);
    }
    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    writeBytes(bpmBytes,2);
    writeBytes(swingAmountBytes,2);
    writeBytes(swingSubDivBytes,2);
    writeBytes(swingByte,1);

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    writeBytes(midiChannelBytes,10);
    writeBytes(midiThruBytes,5);

    seqFile.close();
  }
}

void dumpFilesToSerial(){
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    //write each file to Serial
    while(saves.next()){
      exportSeqFileToSerial(saves.fileName());
    }
  }
  LittleFS.end();
}

uint32_t getByteCount_standAlone(String filename){
  LittleFS.begin();
  uint32_t byteCount = getByteCount(filename);
  LittleFS.end();
  return byteCount;
}
//returns Bytes of a file (must be called within LittleFS)
uint32_t getByteCount(String filename){
  uint32_t byteCount = 0;
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    //2 bytes for start and end each
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    byteCount+=4;
    //one byte for number of tracks;
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    byteCount++;
    
    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      //7 bytes per track
      byteCount+=7;
      noteCount.push_back((uint16_t(trackDat[0])<<8)+uint16_t(trackDat[1]));
    }
    //loading notes
    for(int track = 0; track<tracks[0]; track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        //8 bytes per note
        byteCount+=8;
      }
    }
    
    //one byte for number of DTs
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    byteCount++;
    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        //six bytes per DT
        byteCount+=6;

        //getting points
        uint16_t numberOfDtPoints = (uint16_t(numberOfPoints[0])<<8)+uint16_t(numberOfPoints[1]);
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        //one byte for each DT point
        byteCount+=numberOfDtPoints;
      }
    }
    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    //one byte for the number of loops;
    byteCount++;
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      //6 bytes per loop
      byteCount+=6;
    }
    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    //7 bytes for clock data
    byteCount+=7;

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    //15 bytes for routing data
    byteCount+=15;

    seqFile.close();
  }
  return byteCount;
}

void writeSeqSerial_plain(){
  Serial.print("trackData.size():");
  Serial.println(trackData.size());
  for(int track = 0; track<trackData.size(); track++){
    Serial.print("-----------------[Track ");
    Serial.print(track);
    Serial.println("]-----------------");
    Serial.print("notes: ");
    Serial.println(seqData[track].size()-1);
    Serial.print("pitch: ");
    Serial.println(trackData[track].pitch);
    Serial.print("channel: ");
    Serial.println(trackData[track].channel);
  }
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note <= seqData[track].size()-1; note++){
      Serial.print("---[Note ");
      Serial.print(note);
      Serial.println("]---");
      Serial.print("startPos: ");
      Serial.println(seqData[track][note].startPos);
      Serial.print("vel: ");
      Serial.println(seqData[track][note].velocity);
      Serial.print("mute: ");
      Serial.println(seqData[track][note].muted);
      Serial.print("chance: ");
      Serial.println(seqData[track][note].chance);
      Serial.print("endPos: ");
      Serial.println(seqData[track][note].endPos);
    }
  }

}

//load a sequence, from a file
void loadSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    //replacing sequence with empty data
    newSeq();

    //loading start and end
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    seqStart = (uint16_t(start[0])<<8)+uint16_t(start[1]);
    seqEnd = (uint16_t(end[0])<<8)+uint16_t(end[1]);

    //loading tracks
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      Track newTrack;

      newTrack.pitch = trackDat[2];
      newTrack.channel = trackDat[3];
      newTrack.isLatched = trackDat[4];
      newTrack.muteGroup = trackDat[5];
      newTrack.isPrimed = trackDat[6];

      addTrack(newTrack,false);

      noteCount.push_back((uint16_t(trackDat[0])<<8)+trackDat[1]);
    }

    // Serial.println("loading notes");
    // Serial.flush();
    //loading notes
    for(int track = 0; track<trackData.size(); track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        uint16_t notePos[2] = {uint16_t((notePosData[0]<<8)+notePosData[1]),uint16_t((notePosData[2]<<8)+notePosData[3])};
        loadNote(note+1, track, notePos[0], noteData[0], noteData[3], noteData[1], notePos[1], noteData[2]);
      }
    }
    
    // Serial.println("loading dt's");
    // Serial.flush();
    //loading DT's
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    //clear out dataTrackData
    vector<dataTrack> newData;
    newData.swap(dataTrackData);

    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        uint16_t numberOfDtPoints = uint16_t((numberOfPoints[0]<<8)+numberOfPoints[1]);

        //getting points
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        vector<uint8_t> temp;
        for(uint16_t point = 0; point<numberOfDtPoints; point++){
          temp.push_back(points[point]);
        }
        createDataTrack(dtInfo[0],dtInfo[1],dtInfo[2],temp,dtInfo[3]);
      }
    }

    // Serial.println("loading loops");
    // Serial.flush();
    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    vector<vector<uint16_t>> newLoopData;
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      vector<uint16_t> loopDat = {uint16_t((start[0]<<8)+start[1]),uint16_t((end[0]<<8)+end[1]),loopInfo[0],loopInfo[1]};
      newLoopData.push_back(loopDat);
    }
    loopData.swap(newLoopData);

    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    bpm = uint16_t(bpmBytes[0]<<8) + uint16_t(bpmBytes[1]);
    setBpm(bpm);
    swingVal = uint16_t(swingAmountBytes[0]<<8) + uint16_t(swingAmountBytes[1]);
    swingSubDiv = uint16_t(swingSubDivBytes[0]<<8) + uint16_t(swingSubDivBytes[1]);
    swung = swingByte[0];

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    for(uint8_t port = 0; port<5; port++){
      midiChannels[port] = uint16_t(midiChannelBytes[port*2]<<8)+uint16_t(midiChannelBytes[port*2+1]);
      setThru(port,bool(midiThruBytes[port]));
    }

    seqFile.close();
    LittleFS.end();
    Serial.println("done.");
    updateLEDs();
    setActiveTrack(0,false);
  }
  else
    Serial.println("failed! weird.");
}

//Settings ------
/*
template (0 = 16 tracks, 1 = 4 tracks, 2 = 127 tracks)
display pitches/numbers for tracks
external/internal clock
isLooping
default channel
load file on bootup
*/
//create the default settings file
void createNewSettingsFile(){
  File newSettingsFile = LittleFS.open("/settings.txt","w");
  // const uint8_t defaultSettings[20] = [];
}

//loads settings from settings.txt
void loadSettings(){
  LittleFS.begin();
  if(LittleFS.exists("/settings.txt")){

  }
  else{
    createNewSettingsFile();
  }
}

//loads files into menu, for browsing
//also returns each file's filesize
vector<String> loadFilesAndSizes(){
  vector<String> fileSizes;
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
      // Serial.println(saves.fileName());
      activeMenu.options.push_back(saves.fileName());
      fileSizes.push_back(bytesToString(getByteCount(saves.fileName())));
    }
  }
  else{
    // Serial.println("no files!");
    // Serial.flush();
  }
  LittleFS.end();
  return fileSizes;
}
void loadFiles(){
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
      activeMenu.options.push_back(saves.fileName());
    }
  }
  else{
  }
  LittleFS.end();
}

void renameSeqFile(String filename){
  String newName = enterText("new name?");
  filename = "/SAVES/"+filename;
  newName = "/SAVES/"+newName;
  LittleFS.begin();
  LittleFS.rename(filename,newName);
  LittleFS.end();
}

bool deleteSeqFile(String filename){
  int  choice = binarySelectionBox(64,32,"NO","YEA");
  if(choice == 1){
    filename = "/SAVES/"+filename;
    LittleFS.begin();
    LittleFS.remove(filename);
    LittleFS.end();
    return true;
  }
  return false;
}

void duplicateSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File targetFile = LittleFS.open(path,"r");
  //copy file data into an array
  uint16_t size = targetFile.size();
  uint8_t data[size];
  targetFile.read(data,size);
  targetFile.close();
  String newPath = "/SAVES/"+filename+"_copy";
  File newFile = LittleFS.open(newPath,"w");
  //write array into new file
  newFile.write(data,size);
  newFile.close();
  LittleFS.end();
}
#endif
//system functions------------------------------------
void drawWebLink(){
  display.clearDisplay();
  display.drawBitmap(39,7,web_bmp,50,50,SSD1306_WHITE);
  display.display();
  clearButtons();
  while(true){
    if(itsbeen(200)&&anyActiveInputs()){
      clearButtons();
      lastTime = millis();
      return;
    }
  }
}

//Sleep---------------------------
//pause core1 (you need to call this fn from core0)
bool sleeping = false;
#ifdef HEADLESS
  void enterSleepMode(){
    return;
  }
  void leaveSleepMode(){
    return;
  }
#endif
#ifndef HEADLESS
void enterSleepMode(){
  //turn off power consuming things
  display.clearDisplay();
  display.display();
  clearButtons();
  turnOffLEDs();
  
  sleeping = true;
  //wait for core1 to sleep
  while(core1ready){
  }
   //idle core1
  rp2040.idleOtherCore();
  //sleep until a change is detected on 
  while(sleeping){
    long time  = millis();
    sleep_ms(1000);
    if(anyActiveInputs()){
      sleeping = false;
    }
    // Serial.println("sleeping...");
    // Serial.print(millis()-time);
    // Serial.flush();
  }
  //wake up
  Serial.println("gooood morning");
  Serial.flush();
  leaveSleepMode();
}

void leaveSleepMode(){
  //vv these break it? for some reason
  // Serial.end();
  // startSerial();
  restartDisplay();
  rp2040.resumeOtherCore();
  // rp2040.restartCore1(); <-- this also breaks it
  while(!core1ready){//wait for core1
    Serial.println("waiting");
    Serial.flush();
  }
  Serial.println("ready");
  Serial.flush();
  return;
}
#endif

void setLoop(unsigned int id){
  if(id<loopData.size() && id >=0){
    activeLoop = id;
    loopCount = 0;
  }
}
void serialDispLoopData(){
  Serial.print("activeLoop: ");
  Serial.println(activeLoop);
  Serial.print("loopData[activeLoop][0]:");
  Serial.println(loopData[activeLoop][0]);
  Serial.print("loopData[activeLoop][1]:");
  Serial.println(loopData[activeLoop][1]);
  Serial.print("iterations:");
  Serial.println(loopData[activeLoop][2]);
  Serial.print("count:");
  Serial.println(loopCount);
}
void addLoop(){
  vector<unsigned short int> newLoop{loopData[activeLoop][0],loopData[activeLoop][1],loopData[activeLoop][2],loopData[activeLoop][3]};//makes a copy of the current loop
  loopData.push_back(newLoop);
  setLoop(loopData.size()-1);
}

void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, unsigned short int type){
  vector<unsigned short int> newLoop{start,end,iter,type};
  loopData.push_back(newLoop);
}

void deleteLoop(uint8_t id){
  if(loopData.size() > 1 && loopData.size()>id){//if there's more than one loop, and id is in loopData
    vector<vector<unsigned short int>> tempVec;
    for(int i = 0; i<loopData.size(); i++){
      if(i!=id){
        tempVec.push_back(loopData[i]);
      }
    }
    loopData.swap(tempVec);
    //if activeLoop was the loop that got deleted, or above it
    //decrement it's id so it reads correct (and existing) data
    if(activeLoop>=loopData.size()){  
      activeLoop = loopData.size()-1;
    }
  }
  setLoop(activeLoop);
}

void toggleLoop(){
  isLooping = !isLooping;
}

void setLoopToInfinite(uint8_t targetL){
  //if it's already a 3, set it to 0
  if(loopData[targetL][3] == 4){
    loopData[targetL][3] = 0;
  }
  //if not, set this loop to 3
  else{
    loopData[targetL][3] = 4;
  }
  //set all other inf loops to 0
  for(uint8_t l = 0; l<loopData.size(); l++){
    if(l != targetL){
      if(loopData[l][3] == 4)
        loopData[l][3] = 0;
    }
  }
}

void nextLoop(){//moves to the next loop in loopSeq
  if(loopData.size()>1){
    //infinite
    if(loopData[activeLoop][3] == 4){
      if(playing)
        playheadPos = loopData[activeLoop][0];
      if(recording)
        recheadPos = loopData[activeLoop][0];
    }
    //return loops
    else if(loopData[activeLoop][3] == 3){
      activeLoop = 0;
      loopData[activeLoop][0] = loopData[activeLoop][0];
      loopData[activeLoop][1] = loopData[activeLoop][1];
      loopData[activeLoop][2] = loopData[activeLoop][2];
      if(playing)
        playheadPos = loopData[activeLoop][0];
      if(recording)
        recheadPos = loopData[activeLoop][0];
    }
    //random of same size and normal mode
    else if(loopData[activeLoop][3] == 0 || loopData[activeLoop][3] == 2){
      //move to next loop
      if(activeLoop < loopData.size()-1)
        activeLoop++;
      else
        activeLoop = 0;
      
      //if rnd of same size mode, choose a random loop
      if(loopData[activeLoop][3] == 2){
        int currentLength = loopData[activeLoop][1] - loopData[activeLoop][0];
        vector<uint8_t> similarLoops;
        for(int i = 0; i<loopData.size(); i++){
          int len = loopData[i][1]-loopData[i][0];
          if(len == currentLength){
            similarLoops.push_back(i);
          }
        }
        activeLoop = similarLoops[random(0,similarLoops.size())];
        loopData[activeLoop][0] = loopData[activeLoop][0];
        loopData[activeLoop][1] = loopData[activeLoop][1];
        loopData[activeLoop][2] = loopData[activeLoop][2];
        if(playing)
          playheadPos = loopData[activeLoop][0];
        if(recording)
          recheadPos = loopData[activeLoop][0];
      }
      //normal sequenze mode
      else{
        loopData[activeLoop][0] = loopData[activeLoop][0];
        loopData[activeLoop][1] = loopData[activeLoop][1];
        loopData[activeLoop][2] = loopData[activeLoop][2];
        if(playing)
          playheadPos = loopData[activeLoop][0];
        if(recording)
          recheadPos = loopData[activeLoop][0];
      }
    }
    //for full random mode
    else if(loopData[activeLoop][3] == 1){
      activeLoop = random(0,loopData.size());
      loopData[activeLoop][0] = loopData[activeLoop][0];
      loopData[activeLoop][1] = loopData[activeLoop][1];
      loopData[activeLoop][2] = loopData[activeLoop][2];
      if(playing)
        playheadPos = loopData[activeLoop][0];
      if(recording)
        recheadPos = loopData[activeLoop][0];
    }
  }
  loopCount = 0;
}

void tapBpm(){
  int activeLED = 0;
  bool leds[8] = {0,0,0,0,0,0,0,0};
  long t1;
  long timeE;
  long timeL;
  bool time1 = false;
  display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
  display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
  display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
  display.setCursor(46,12);
  display.print("bpm");
  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(64-stringify(int(bpm)).length()*4,35);
  print7SegNumber(64,35,bpm,true);
  // display.print(int(bpm));
  display.setFont();
  display.display();
  t1 = millis();
  while(true){
    //checking if it's been a 1/4 note
    timeE = micros()-timeL;
    if(timeE  >= MicroSperTimeStep*24){
      timeL = micros();
      leds[activeLED] = 0;
      activeLED += 1;
      activeLED %= 8;
      leds[activeLED] = 1;
      writeLEDs(leds);
    }
    readButtons_MPX();
    joyRead();
    menuScrolling();
    if(menu_Press  || activeMenu.options[activeMenu.highlight] != "tap"){
      lastTime = millis();
      return;
    }
    if(itsbeen(75)){
      if(step_buttons[0]||step_buttons[1]||step_buttons[2]||step_buttons[3]||step_buttons[4]||step_buttons[5]||step_buttons[6]||step_buttons[7]){
        setBpm(float(60000)/float(millis()-t1));
        t1 = millis();
        display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
        display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
        display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
        display.setCursor(46,12);
        display.print("bpm");
        display.setFont(&FreeSerifItalic9pt7b);
        display.setCursor(64-stringify(int(bpm)).length()*4,35);
        display.print(bpm);
        display.setFont();
        display.display();
        clearButtons();
        lastTime = millis();
      }
    }
    while(counterA != 0){
      if(counterA >= 1){
        if(shift){
          setBpm(bpm+1);
        }
        else{
          setBpm(bpm+10);
        }
      }
      if(counterA <= -1){
        if(shift){
          setBpm(bpm-1);
        }
        else{
          setBpm(bpm-10);
        }
      }
    }
  }
}

void setBpm(int newBpm) {
  if(newBpm<=0){
    newBpm = 1;
  }
  else if(newBpm>999){
    newBpm = 999;
  }
  bpm = newBpm;
  MicroSperTimeStep = round(2500000/(bpm));
}

//changes which track is active, changing only to valid tracks
void setActiveTrack(uint8_t newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < trackData.size()) {
    if(activeTrack == 4 && newActiveTrack == 5 && maxTracksShown == 5){
      maxTracksShown = 6;
    }
    else if(activeTrack == 1 && newActiveTrack == 0 && maxTracksShown == 6){
      maxTracksShown = 5;
    }
    activeTrack = newActiveTrack;
    if (loudly) {
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
      if(trackData[activeTrack].isLatched){
        sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
        sendMIDInoteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
      }
    }
  }
  updateLEDs();
  menuText = pitchToString(trackData[activeTrack].pitch,true,true);
}

void changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    trackData[id].channel = newChannel;
  }
}

void changeAllTrackChannels(int newChannel){
  for(int track = 0; track<trackData.size(); track++){
    changeTrackChannel(track, newChannel);
  }
}

//checks if the cursor is out of bounds
bool isOutOfBounds(){
  if(cursorPos>seqEnd || cursorPos < seqStart)
    return true;
  else
    return false;
}

void moveToNextNote_inTrack(bool up){
  uint8_t track = activeTrack;
  uint16_t currentID = lookupData[activeTrack][cursorPos];
  bool foundTrack = false;
  //moving the track up/down until it hits a track with notes
  //and checking bounds
  if(up){
    while(track<trackData.size()-1){
      track++;
      if(seqData[track].size()>1){
        Serial.println("found a note on track "+stringify(track));
        foundTrack = true;
        break;
      }
    }
  }
  else{
    while(track>0){
      track--;
      if(seqData[track].size()>1){
        Serial.println("found a note on track "+stringify(track));
        foundTrack = true;
        break;
      }
    }
  }
  //if you couldn't find a track with a note on it, just return
  if(!foundTrack){
    Serial.println("couldn't find a track");
    return;
  }
  Serial.println("moving to track "+stringify(track));
  for(uint16_t dist = 0; dist<seqEnd; dist++){
    bool stillValid = false;
    //if the new position is in bounds
    if(cursorPos+dist<=seqEnd){
      stillValid = true;
      //and if there's something there!
      if(lookupData[track][cursorPos+dist] != 0){
        //move to it
        moveCursor(dist);
        setActiveTrack(track,false);
        Serial.println("HEY moved "+stringify(dist)+" on track "+stringify(track));
        return;
      }
    }
    if(cursorPos>=dist){
      stillValid = true;
      if(lookupData[track][cursorPos-dist] != 0){
        moveCursor(-dist);
        setActiveTrack(track,false);
        Serial.println("HEY moved "+stringify(dist)+" on track "+stringify(track));
        return;
      }
    }
    //if it's reached the bounds
    if(!stillValid){
      Serial.println("HEY couldn't find a note on either side");
      Serial.println("checked "+stringify(dist)+" steps");
      return;
    }
  }
  Serial.println("IDK man");
}
//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void moveToNextNote(bool forward,bool endSnap){
  //if there's a note on this track at all
  if(seqData[activeTrack].size()>1){
    unsigned short int id = lookupData[activeTrack][cursorPos];
    if(forward){
      for(int i = cursorPos; i<seqEnd; i++){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          moveCursor(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(seqEnd-cursorPos);
      }
      return;
    }
    else{
      for(int i = cursorPos; i>seqStart; i--){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          moveCursor(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(-cursorPos);
      }
      return;
    }
  }
}

void moveToNextNote(bool forward){
  moveToNextNote(forward, false);
}

//stops all midi sends, clears playlist
void stop(){
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].noteLastSent != 255){
      sendMIDInoteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      trackData[track].noteLastSent = 255;
    }
    else{
      sendMIDInoteOff(trackData[track].pitch, 0, trackData[track].channel);
    }
  }
  vector<vector<uint8_t>> temp;
  playlist.swap(temp);
}

//View ------------------------------------------------------------------
//view start is inclusive, starts at 0
//view end is inclusive, 127
//handles making sure the view is correct
void moveView(int val) {
  int oldViewLength = viewEnd-viewStart;
  if((viewStart+val)<0){
    viewStart = seqStart;
    viewEnd = viewStart+oldViewLength;
  }
  if(viewEnd+val>seqEnd){
    viewEnd = seqEnd;
    viewStart = viewEnd - oldViewLength;
  }
  if(viewEnd+val<=seqEnd && viewStart+val>=0){
    viewStart += val;
    viewEnd += val;
  }
}

//moving cursor around while in dataTrack mode
void moveDataTrackCursor(int moveAmount){
  uint8_t originalDataPoint = dataTrackData[activeDataTrack].data[cursorPos];
//if you're trying to move back at the start
  if(cursorPos==0 && moveAmount < 0){
    return;
  }
  if(moveAmount<0 && abs(moveAmount)>=cursorPos)
    cursorPos = 0;
  else
    cursorPos += moveAmount;
  if(cursorPos >= seqEnd) {
    cursorPos = seqEnd-1;
  }
  if(cursorPos<seqStart){
    cursorPos = seqStart;
  }
  while (cursorPos < viewStart+subDivInt && viewStart>seqStart) {
    moveView(-1);
  }
  while (cursorPos > viewEnd-subDivInt && viewEnd<seqEnd) {
    moveView(1);
  }
}

//moving the cursor around
int16_t moveCursor(int moveAmount){
  int16_t amt;
  //if you're trying to move back at the start
  if(cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && cursorPos+moveAmount<0){
    amt = cursorPos;
    cursorPos = 0;
  }
  else{
    cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(cursorPos > seqEnd) {
    amt += (seqEnd-cursorPos);
    cursorPos = seqEnd;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(seqData[activeTrack][seqData[activeTrack].size()-1].endPos<cursorPos)
      changeNoteLength(cursorPos-seqData[activeTrack][seqData[activeTrack].size()-1].endPos,activeTrack,seqData[activeTrack].size()-1);
  }
  while (cursorPos < viewStart+subDivInt && viewStart>seqStart) {
    moveView(-1);
  }
  while (cursorPos > viewEnd-subDivInt && viewEnd<seqEnd) {
    moveView(1);
  }
  updateLEDs();
  menuText = (moveAmount>0)?(stepsToPosition(cursorPos,true)+">>"):("<<"+stepsToPosition(cursorPos,true));
  // Serial.println("moved "+stringify(amt));
  // Serial.flush();
  return amt;
}
void setCursorToBeginningOfNote(uint8_t track, uint16_t id){
  setCursor(seqData[track][id].startPos);
}

void setCursor(uint16_t loc){
  moveCursor(loc-cursorPos);
}

void updateCursor(){
  // viewLength = viewEnd-viewStart;
  if (cursorPos < seqStart) {
    cursorPos = seqStart;
  }
  if (cursorPos > seqEnd-1) {
    cursorPos = seqEnd-1;
  }
  if (cursorPos < viewStart) {
    moveView(cursorPos-viewStart);
  }
  if (cursorPos >= viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(cursorPos-viewEnd);
  }
}

unsigned short int getNoteLength(unsigned short int track, unsigned short int id){
  unsigned short int length = seqData[track][id].endPos-seqData[track][id].startPos+1;
  return length;
}

int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    int newEnd;
    //if it's 1 step long and being increased by subDivInt, make it subDivInt steps long instead of subDivInt+1 steps
    //(just to make editing more intuitive)
    if(seqData[track][id].endPos - seqData[track][id].startPos == 1 && val == subDivInt){
      newEnd = seqData[track][id].endPos + val - 1;
    }
    else{
      newEnd = seqData[track][id].endPos + val;
    }
    //check and see if there's a different note there
    //if there is, set the new end to be 
    for(uint16_t step = 1; step<=val; step++){
      if(lookupData[track][step+seqData[track][id].endPos] != 0 && lookupData[track][step+seqData[track][id].endPos] != id){
        //if it's the first step, just fail to save time
        if(step == 1)
          return 0;
        //if it's not the first step, set the new end to right before the other note
        else{
          newEnd = seqData[track][id].endPos+step;
          break;
        }
      }
    }
    //if the new end is before/at the start, don't do anything
    if(newEnd<=seqData[track][id].startPos)
      return 0;
      // newEnd = seqData[track][id].startPos+1;
    //if the new end is past/at the end of the seq
    if(newEnd>seqEnd){
      newEnd = seqEnd;
    }
    Note note = seqData[track][id];
    int16_t amount = newEnd-note.endPos;
    note.endPos = newEnd;
    deleteNote_byID(track, id);
    makeNote(note, track, false);
    return amount;
  }
  return 0;
}


void changeNoteLengthSelected(int amount){
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note <= seqData[track].size()-1; note++){
      if(seqData[track][note].isSelected){
        changeNoteLength(amount, track, note);
      }
    }
  }
}

int16_t changeNoteLength(int amount){
  if(selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectionBounds2();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      setCursor(bounds[0]);
    return 0;
  }
  else{
    return changeNoteLength(amount, activeTrack, lookupData[activeTrack][cursorPos]);
  }
}

//this one jumps the cursor to the end or start of the note
void changeNoteLength_jumpToEnds(int16_t amount){
  if(selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectionBounds2();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      setCursor(bounds[0]);
  }
  else{
    //if the note was changed
    if(changeNoteLength(amount, activeTrack, lookupData[activeTrack][cursorPos]) != 0){
      //if you're shrinking the note
      if(amount<0){
        setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].startPos);
        //if it's out of view
        // else
          // setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos+amount);
      }
      //if you're growing it
      else
        setCursor(seqData[activeTrack][seqData[activeTrack].size()-1].endPos-subDivInt);
        // setCursor(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos - amount);
    }
  }
}

//sets cursor to the visually nearest note
//steps to pixels = steps*scale
//for a note to be "visually" closer, it needs to have a smaller pixel
//distance from the cursor than another note
//compare trackDistance * trackHeight to stepDistance * scale
float getDistanceFromNoteToCursor(Note note,uint8_t track){
                                                          //if the start of the note is closer than the end
  return sqrt(pow(activeTrack - track,2)+pow(((abs(note.startPos-cursorPos)<abs(note.endPos-cursorPos))?(note.startPos-cursorPos):(note.endPos-cursorPos)),2));
}

void setCursorToNearestNote(){
  const float maxPossibleDist = seqEnd*scale+trackData.size()*trackHeight;
  float minDist = maxPossibleDist;
  int minTrack;
  int minNote;
  for(int track = 0; track<seqData.size(); track++){
    for(int note = 1; note<seqData[track].size(); note++){
      // Serial.println("checking n:"+stringify(note)+" t:"+stringify(track));
      // Serial.flush();
      float distance = getDistanceFromNoteToCursor(seqData[track][note],track);
      if(distance<minDist){
        minTrack = track;
        minNote = note;
        minDist = distance;
        if(minDist == 0)
          break;
      }
    }
    if(minDist == 0)
      break;
  }
  // Serial.println("setting cursor...");
  // Serial.flush();
  if(minDist != maxPossibleDist){
    setCursor((seqData[minTrack][minNote].startPos<cursorPos)?seqData[minTrack][minNote].startPos:seqData[minTrack][minNote].endPos-1);
    setActiveTrack(minTrack,false);
  }
}

void debugPrintSelection(){
  Serial.print("#:");
  Serial.println(selectionCount);
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note<=seqData[track].size()-1; note++){
      if(seqData[track][note].isSelected){
        Serial.print("note ");
        Serial.print(note);
        Serial.print(" on track ");
        Serial.println(track);
        Serial.print("[");
        Serial.print(seqData[track][note].startPos);
        Serial.print(",");
        Serial.print(seqData[track][note].endPos);
        Serial.println("]");
      }
    }
  }
}
void reverse(uint16_t start, uint16_t end){
  for(uint8_t track = 0; track<trackData.size(); track++){
    if(trackData[track].isSelected){
      reverseTrack(track,start,end);
    }
  }
}
void reverseTrack(uint8_t track, uint16_t start, uint16_t end){
  vector<Note> targetNotes;
  //add all notes to seq data
  for(int i = 1; i<seqData[track].size(); i++){
    //if both the start and the end of the note are within bounds
    //(you might want to change this to crop the note instead)
    if(seqData[track][i].startPos>=start && seqData[track][i].endPos<=end){
      //add the note to the vector
      targetNotes.push_back(seqData[track][i]);
      //delete it from lookup data
      deleteNote_byID(track, i);
      i--;
      Serial.println("test");
    }
  }
  //make new notes
  for(int i = 0; i<targetNotes.size(); i++){
    int d = end-targetNotes[i].endPos+start;
    int newEnd = -targetNotes[i].startPos+d;
    int newStart = -targetNotes[i].endPos+d;
    targetNotes[i].startPos = newStart;
    targetNotes[i].endPos = newEnd;
    makeNote(targetNotes[i],track,false);
  }
}
void selectAllTracks(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    trackData[i].isSelected = true;
  }
}
void deselectAllTracks(){
  for(uint8_t i = 0; i<trackData.size(); i++){
    trackData[i].isSelected = false;
  }
}
void reverseNotes(){
  bool selBoxFinished = false;
  selectAllTracks();
  while(true){
    joyRead();
    readButtons();
    Serial.println(selBox.x1);
    Serial.println(selBox.x2);
    Serial.flush();
    //creating selbox when cursor is moved and sel is held down
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
      selBoxFinished = false;
    }
    //if sel is released, and there's a selection box
    //then set those coords to the selbox! It's that simple
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      if(selBox.x1>selBox.x2){
        unsigned short int x1_old = selBox.x1;
        selBox.x1 = selBox.x2;
        selBox.x2 = x1_old;
      }
      if(selBox.y1>selBox.y2){
        unsigned short int y1_old = selBox.y1;
        selBox.y1 = selBox.y2;
        selBox.y2 = y1_old;
      }
      selBoxFinished = true;
      selBox.begun = false;
    }
    while(counterA != 0){
      if(counterA >= 1 && !track_Press){
        zoom(true);
      }
      if(counterA <= -1 && !track_Press){
        zoom(false);
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1 && !shift){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(counterB <= -1 && !shift){
        changeSubDivInt(false);
      }
      //if shifting, toggle between 1/3 and 1/4 mode
      else while(counterB != 0 && shift){
        toggleTriplets();
      }
      counterB += counterB<0?1:-1;;
    }
    if(itsbeen(200)){
      //exit
      if(menu_Press || loop_Press){
        lastTime = millis();
        menu_Press = false;
        loop_Press = false;
        deselectAllTracks();
        return;
      }
      //select/deselect track
      if(track_Press){
        lastTime = millis();
        trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
        //if shift, select/deselect all tracks
        if(shift){
          for(uint8_t i = 0; i<trackData.size(); i++){
            trackData[i].isSelected = trackData[activeTrack].isSelected;
          }
        }
      }
      //committing reverse
      if(n){
        lastTime = millis();
        n = false;
        //reverse a specific area
        if(selBoxFinished){
          reverse(selBox.x1,selBox.x2);
        }
        //reverse the whole seq
        else{
          uint8_t choice = binarySelectionBox(64,32,"NAH","YEH","Reverse entire seq?");
          if(choice == 1){
            reverse(seqStart,seqEnd);
          }
        }
      }
    }
    //moving cursor
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
        }
      }
    }
    //moving active track
    if(itsbeen(100)){
      if (y == 1 && !shift && !loop_Press) {
        if(recording)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, true);
        drawingNote = false;
        lastTime = millis();
      }
      if (y == -1 && !shift && !loop_Press) {
        if(recording)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
        drawingNote = false;
        lastTime = millis();
      }
    }
    display.clearDisplay();
    drawSeq(true,true,true,false,true);//no menus! but track selection
    if(selBoxFinished)
      drawSelectionBracket();
    display.display();
  }
}

//warping
//gets selection bounds and prints em
void warpSeq(int amount){
  vector<uint16_t> bounds = getSelectionBounds2();
  vector<uint8_t> tracks = getTracksWithSelectedNotes();
  for(uint8_t t = 0; t<tracks.size(); t++){
    Serial.println("start:"+stringify(bounds[0]));
    Serial.println("end:"+stringify(bounds[2]));
    Serial.println("track:"+stringify(tracks[t]));
    Serial.flush();
    warpChunk(bounds[0],bounds[2],tracks[t],tracks[t],amount);
  }
}
void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation){
  switch(rotation){
    //down
    case 0:
      drawCurlyBracket(x1,y1,length,height,start,end);
      break;
    //up
    case 1:
      display.setRotation(UPSIDEDOWN);
      drawCurlyBracket(screenWidth-x1,screenHeight-y1,length,height,start,end);
      display.setRotation(UPRIGHT);
      break;
    //right
    case 2:
      display.setRotation(1);
      drawCurlyBracket(y1,x1,height,length,start,end);
      display.setRotation(UPRIGHT);
      break;
  }
}
void drawCurlyBracket(int16_t x1, uint8_t y1, uint8_t length,uint8_t height,bool start, bool end){
  //left leg
  if(start){
    display.drawFastVLine(x1,y1-height,height,SSD1306_WHITE);
    display.drawPixel(x1+1,y1-height-1,SSD1306_WHITE);
    //left top
    display.drawFastHLine(x1+2,y1-height-2,length/2-3,SSD1306_WHITE);
  }
  //if start is out of view
  else{
     display.drawFastHLine(trackDisplay,y1-height-2,length/2-3-(viewStart-x1),SSD1306_WHITE);
  }
  //middle
  display.drawPixel(x1+length/2-1,y1-height-1,SSD1306_WHITE);
  display.drawFastVLine(x1+length/2,y1-height-4,3,SSD1306_WHITE);
  display.drawPixel(x1+length/2+1,y1-height-1,SSD1306_WHITE);
  //right top
  display.drawFastHLine(x1+length/2+2,y1-height-2,length/2-3,SSD1306_WHITE);
  //right leg
  display.drawPixel(x1+length-1,y1-height-1,SSD1306_WHITE);
  display.drawFastVLine(x1+length,y1-height,height,SSD1306_WHITE);
}

void drawWarpPoint(uint16_t s, uint16_t e, bool warping){
  int32_t x1 = trackDisplay;
  uint16_t x2 = viewEnd +1;
  bool sInView = false,eInView = false;
  //if it's in view
  if(s >= viewStart && s <= viewEnd){
    sInView = true;
    x1 = (s - viewStart)*scale+trackDisplay;
    uint8_t y1 = 15;
    if(movingLoop == -1 || movingLoop == 2){
      y1 += sin(millis()/50);
      if(millis()%400<200){
        display.drawFastVLine(x1-1,y1,50,SSD1306_WHITE);
      }
    }
    else{
      if(cursorPos == s)
        y1 += sin(millis()/200);
      display.drawFastVLine(x1-1,y1,50,SSD1306_WHITE);
    }
    // printSmall(x1-2,y1-7,"S",SSD1306_WHITE);
    printItalic(x1-4,y1-8,"S",1);
  }
  if(e >= viewStart && e <= viewEnd){
    eInView = true;
    x2 = (e - viewStart)*scale+trackDisplay;
    uint8_t y1 = 15;
    if(movingLoop == 1 || movingLoop == 2){
      y1 += sin(millis()/50);
      if(millis()%400<200){
        display.drawFastVLine(x2-1,y1,50,SSD1306_WHITE);
      }
    }
    else{
      if(cursorPos == e)
        y1 += sin(millis()/200);
      display.drawFastVLine(x2-1,y1,50,SSD1306_WHITE);
    }
    // printSmall(x2-2,y1-7,"E",SSD1306_WHITE);
    printItalic(x2-3,y1-8,"E",1);
  }
  uint8_t graphicS,graphicE;
  //if both are in view
  if(sInView){
    graphicS = (s - viewStart)*scale+trackDisplay;
  }
  else if(s<viewStart){
    graphicS = trackDisplay;
  }
  else if(s>viewEnd){
    return;
  }
  if(eInView){
    graphicE = (e - viewStart)*scale+trackDisplay;
  }
  else if(e>viewEnd){
    graphicE = screenWidth;
  }
  else if(e<viewStart){
    return;
  }
  for(uint8_t i = graphicS+3; i <= graphicE-4; i+= 3){
    // display.fillRect(i,3+2*sin(i/4-millis()/300),2,6,1);
    if(!warping)
      display.fillRect(i,8+2*sin(i/4-millis()/300),2,5,1);
    else
      display.fillRect(i,8+2*sin(i/4-millis()/(50)),2,5,1);
      // display.fillRect(i,8+2*sin(i/4-millis()/(e-s)),2,5,1);
  }
}
//clone of move loop points
void moveWarpPoints(uint16_t* start, uint16_t* end, int16_t amount){
  uint16_t length = (*end) - (*start);
  //backwards
  if(amount<0){
    if((*start)+amount<=0){
      (*start) = 0;
    }
    else{
     (*start) += amount;
    }
    (*end) = (*start)+length;
  }
  else{
    if((*end)+amount>=seqEnd){
      (*end) = seqEnd;
    }
    else{
      (*end) += amount;
    }
    (*start) = (*end)-length;
  }
}
//draw a warp wiggle
void drawWarpWiggle(uint8_t y1){
  int8_t offset = millis()/50;
  for(uint8_t x1 = 0; x1<screenWidth; x1++){
    display.drawPixel(x1,y1+2*sin(x1+offset),SSD1306_WHITE);
  }
}
//sets a warp point to the cursor
void setWarpPoint(uint16_t* start, uint16_t* end, bool which){
  //start
  if(which){
    if(cursorPos>=*end)
      *start = *end;
    else
      *start = cursorPos;
  }
  else{
    if(cursorPos<=*start)
      *end = *start;
    else
      *end = cursorPos;
  }
}
//runs while warping, lets you drag warp points around to warp sequence
//"new" drops warp points
void warp(){
  //warp points start off as loop points
  uint16_t warpStart = loopData[activeLoop][0];
  uint16_t warpEnd = loopData[activeLoop][1];
  uint8_t trackS = 0;
  uint8_t trackE = 0;
  uint16_t originalLength;
  bool warping = false;
  float warpAmount = 100;
  while(true){
    //draw seq without top info, side info, or menus
    display.clearDisplay();
    //just show track info
    drawSeq(true,false,false,false,false);
    drawWarpPoint(warpStart,warpEnd,warping);
    display.drawCircle(16,8,8*sin(float(millis())/float(300)),1);
    // drawWarpWiggle(8);
    if(warping){
      // print7SegSmall(trackDisplay,0,stringify(int(warpAmount))+"%",SSD1306_WHITE);
      printSmall(trackDisplay-5,0,"warp:"+stringify(int(warpAmount))+"%",SSD1306_WHITE);
    }
    else{
      printSmall(trackDisplay-5,0,"[n]",1);
      printSmall(trackDisplay+15,0,"[sh+n] x2",1);
      printSmall(trackDisplay+48,7,"[sh+del] /2",1);
    }
    display.display();
    joyRead();
    readButtons();
    //sel box
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      selectBox();
      selBox.begun = false;
    }
    if(itsbeen(200)){
      //go back to main edit menu
      if(menu_Press || note_Press){
        lastTime = millis();
        movingLoop = 0;
        return;
      }
      //n drops warp points
      if(loop_Press){
        //moving warp points the same way you move loop points
        if(movingLoop == 0){
          if(cursorPos == warpStart){
            movingLoop = -1;
          }
          else if(cursorPos == warpEnd){
            movingLoop = 1;
          }
          else{
            movingLoop = 2;
          }
          lastTime = millis();
        }
        //stop moving if you were moving
        else{
          movingLoop = 0;
          lastTime = millis();
        }
      }
      //start the warp
      if(n){
        warping = !warping;
        if(warping){
          originalLength = warpEnd - warpStart+1;
        }
        lastTime = millis();
      }
    }
    if(x != 0){
      if(warping){
        if(shift && itsbeen(50)){
          //stretching
          if(x == -1){
            warpChunk(warpStart,warpEnd,activeTrack,activeTrack,1);
            warpEnd += 1;
            if(warpEnd>seqEnd)
              warpEnd = seqEnd;
            warpAmount = float(warpEnd-warpStart+1)/float(originalLength)*100;
            lastTime = millis();
          }
          else if(x == 1){
            warpChunk(warpStart,warpEnd,activeTrack,activeTrack,-1);
            if(warpEnd-1<warpStart)
              warpEnd = warpStart;
            else
              warpEnd -= 1;
            warpAmount = float(warpEnd-warpStart+1)/float(originalLength)*100;
            lastTime = millis();
          }

        }
        else if(!shift && itsbeen(100)){
          //stretching
          if(x == -1){
            warpChunk(warpStart,warpEnd,activeTrack,activeTrack,subDivInt);
            warpEnd += subDivInt;
            if(warpEnd>seqEnd)
              warpEnd = seqEnd;
            warpAmount = float(warpEnd-warpStart+1)/float(originalLength)*100;
            lastTime = millis();
          }
          else if(x == 1){
            warpChunk(warpStart,warpEnd,activeTrack,activeTrack,-subDivInt);
            if(warpEnd-subDivInt<warpStart)
              warpEnd = warpStart;
            else
              warpEnd -= subDivInt;
            warpAmount = float(warpEnd-warpStart+1)/float(originalLength)*100;
            lastTime = millis();
          }
        }
      }
      else{
        //if not shifting
        if(!shift && itsbeen(100)){
          if (x == 1) {
            if(cursorPos%subDivInt){
              moveCursor(-cursorPos%subDivInt);
              lastTime = millis();
              if(movingLoop == 2){
                moveWarpPoints(&warpStart,&warpEnd,-cursorPos%subDivInt);
              }
            }
            else{
              moveCursor(-subDivInt);
              lastTime = millis();
              if(movingLoop == 2){
                moveWarpPoints(&warpStart,&warpEnd,-subDivInt);
              }
            }
          }
          if (x == -1) {
            if(cursorPos%subDivInt){
              moveCursor(subDivInt-cursorPos%subDivInt);
              lastTime = millis();
              if(movingLoop == 2){
                moveWarpPoints(&warpStart,&warpEnd,subDivInt-cursorPos%subDivInt);
              }
            }
            else{
              moveCursor(subDivInt);
              lastTime = millis();
              if(movingLoop == 2){
                moveWarpPoints(&warpStart,&warpEnd,subDivInt);
              }
            }
          }
        }
        //if shifting
        else if(shift && itsbeen(50)){
          if(x == 1){
            moveCursor(-1);
            lastTime = millis();
            if(movingLoop == 2){
              moveWarpPoints(&warpStart,&warpEnd,-1);
            }
          }
          else if(x == -1){
            moveCursor(1);
            lastTime = millis();
            if(movingLoop == 2){
              moveWarpPoints(&warpStart,&warpEnd,1);
            }
          }
        }
        //handling loop moves
        if(movingLoop == -1)
          setWarpPoint(&warpStart,&warpEnd,true);
        else if(movingLoop == 1)
          setWarpPoint(&warpStart,&warpEnd,false);
      }
    }
    if(!warping)
      yControls();
    defaultEncoderControls();
  }
}

//warps a chunk of time and pitch in a direction
bool warpChunk(uint16_t start, uint16_t end, uint16_t trackStart, uint16_t trackEnd, int16_t amount){
  if(end == start)
    return false;
  //if you're trying to move more than the length, return
  if(amount+end-start < 0)
    return false;
  //scale that each note length/position will be multiplied by
  //if amount is positive, scale will be >1 and if negative, it'll be <1
  //scale = newlength/oldlength
  float sc = float(end-start+amount)/float(end-start);
  //if the scale is 0 (bad) or 1 (not bad, just useless) return
  if(sc <= 0 || sc == 1){
    return false;
  }
  //grab all the notes in the area and delete them from the sequence
  //holds notes
  vector<vector<Note>> notes;
  //for each of the selected tracks (including the last one)
  for(uint8_t t = trackStart; t<=trackEnd; t++){
    vector<Note> test;
    for(uint16_t step = start; step<end; step++){
      if(lookupData[t][step] != 0){
        test.push_back(seqData[t][lookupData[t][step]]);
        deleteNote_byID(t,lookupData[t][step]);
      }
    }
    notes.push_back(test);
  }
  //change each startPos and endpos by scale
  for(uint8_t t = 0;t<notes.size(); t++){
    for(uint8_t note = 0; note<notes[t].size(); note++){
      //scaling length
      float length = notes[t][note].endPos-notes[t][note].startPos;
      length = float(length)*sc;
      //scaling start, end
      notes[t][note].startPos = (notes[t][note].startPos-start)*sc + start;
      makeNote(notes[t][note],t+trackStart,false);
      // Serial.println("t:"+stringify(t)+" note:"+stringify(note));
      // Serial.flush();
    }
  }
  return true;
}

//gets the closest pitch of a scale
uint8_t getNearestPitch(int pitch, int scale){
  uint8_t closestPitch;
  return closestPitch;
}

//------------------------------------------------------------------------------------------------------------------------------
//returns a byteCount as a more recognizeable value, like 7kB or 5mB
String bytesToString(uint32_t bytes){
  //if it's less than 500, print bytes as-is
  if(bytes<500){
    return stringify(bytes)+"B";
  }
  //probably never going to go over this
  else if(bytes<500000){
    //divide by 100 so the decimal value is now in the 1's place
    bytes/= 100;
    String byteCount = stringify(bytes);
    byteCount += byteCount[byteCount.length()-1];
    byteCount[byteCount.length()-2] = '.';
    return byteCount+"kB";
  }
  else{
    return "bruh";
  }
}
//returns the pitch in english AND in either sharp, or flat form
String pitchToString(uint8_t input, bool oct, bool sharps){
  String pitch;
  int octave = (input/12)-2;//idk why this offset is needed
  input = input%12;
  if(!sharps){
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "Db";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "Eb";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "Gb";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "Ab";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "Bb";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  else{
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "C#";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "D#";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "F#";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "G#";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "A#";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  if(oct){
    pitch+=octave;
  }
  return pitch;
}
String getTrackPitchOctave(int id){
  return pitchToString(trackData[id].pitch,true, true);
}
String getTrackPitch(int id){
  return pitchToString(trackData[id].pitch, false, true);
}

String stepsToPosition(int steps,bool verby){
  String text;
  text += stringify(steps/96 + 1);
  text += stringify(":");
  text += stringify((steps%96)/24 + 1);
  return text;
}

String stepsToMeasures(int32_t stepVal){
  // Serial.println("steps:"+stringify(stepVal));
  String text;
  uint16_t smallest = 0;
  uint16_t steps = stepVal;

  //% steps by each denominator to see if there's no remainder
  //if there's no remainder, then you know that the smallest
  uint16_t denominators[12] = {96,48,32,24,16,12,8,6,4,3,2,1};
  for(uint8_t i = 0; i<12; i++){
    if(smallest == 0){
      steps=stepVal%denominators[i];
      if(steps == 0){
        smallest = i;
        break;
      }
    }
  }
  //steps are each a 1/96
  //this is basically asking "if steps is cleanly divisible by X, with no remainder, then X is the smallest denominator needed"
  //reset steps to the length - wholenotes
  steps = stepVal;
  if(steps>=96){
    text = stringify(steps/96);
    steps %= 96;
  }
  //if there's no numerator
  if(steps/denominators[smallest] == 0){
    if(text == "")
      text = "0";
    return text;
  }
  if(text != "")
    text += " ";
  text += stringify(steps/denominators[smallest])+"/"+stringify(denominators[11-smallest]);
  return text;
}

int getOctave(int val){
  int oct = 0;
  while(val>11){
    oct++;
    val-=12;
  }
  return oct;
}
//draws dashed note (MAKE THIS FASTER!)
void drawNote(uint16_t id, uint8_t track, unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned short int height, unsigned short int shade, bool isSelected, bool mute){
  //makes drawing zoomed out notes faster
  if(len>=3){
    if(!mute){
      if(shade != 1){//so it does this faster
        // display.fillRect(x1,y1,len,height,SSD1306_BLACK);//clearing out the note area
        display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_BLACK);//clearing out the note area
        for(int j = 1; j<height-2; j++){//shading the note...
          for(int i = x1+1;i+j%shade<x1+len-1; i+=shade){
            display.drawPixel(i+j%shade,y1+j,SSD1306_WHITE);
          }
        }
        display.drawRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      }
      else{//filling note
        display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      }
      //line at the end, if there's something at the end
      if(lookupData[track][seqData[track][id].endPos] != 0)
        display.drawFastVLine(x1+len,y1+1,height-2,SSD1306_BLACK);
    }
    //if it's muted
    else{
      //body
      display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_BLACK);
      display.drawRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      display.drawLine(x1+1,y1+1, x1+len-1, y1+height-2,SSD1306_WHITE);
      display.drawLine(x1+1,y1+height-2,x1+len-1,y1+1,SSD1306_WHITE);
      display.drawFastVLine(x1+len,y1+1,height-2,SSD1306_BLACK);
    }
    if(isSelected){
      display.drawRect(x1,y1,len+1,trackHeight,SSD1306_WHITE);
      display.drawRect(x1,y1+1,len,trackHeight-2,SSD1306_BLACK);
      display.drawRect(x1+2,y1+2,len-3,trackHeight-4,SSD1306_WHITE);
    }
  }
  else{
    display.fillRect(x1, y1+1, len+2, height-2, SSD1306_WHITE);
  }
}
//draws dashed note (MAKE THIS FASTER!)
void drawNote_orig(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned short int height, unsigned short int shade, bool isSelected, bool mute){
  //makes drawing zoomed out notes faster
  if(len>=3){
    if(!mute){
      if(shade != 1){//so it does this faster
        display.fillRect(x1+1,y1,len,height,SSD1306_BLACK);//clearing out the note area
        for(int j = 0; j<height; j++){//shading the note...
          for(int i = x1+1;i+j%shade<x1+len; i+=shade){
            display.drawPixel(i+j%shade,y1+j,SSD1306_WHITE);
          }
        }
      }
      else{//filling note
        display.fillRect(x1+1,y1,len,height,SSD1306_WHITE);
      }
      display.drawRect(x1+2, y1+1, len-1, height-2, SSD1306_WHITE);
      display.drawRect(x1+1,y1,2*len+1,height, SSD1306_BLACK);
    }
    //if it's muted
    else{
      display.drawRect(x1+2, y1+1, len-1, height-2, SSD1306_WHITE);
      display.drawRect(x1+1,y1,2*len+1,height, SSD1306_BLACK);
      display.drawLine(x1+2,y1-2+height,x1+len,y1+1,SSD1306_WHITE);
      display.drawLine(x1+2,y1+1,x1+len,y1+height-2,SSD1306_WHITE);
    }
    if(isSelected){
      display.drawRect(x1,y1,len+1,trackHeight,SSD1306_WHITE);
      display.drawRect(x1,y1+1,len,trackHeight-2,SSD1306_BLACK);
    }
  }
  else{
    display.fillRect(x1, y1+1, len+2, height-2, SSD1306_WHITE);
  }
}
void drawNote_vel(uint16_t id, uint8_t track, unsigned short int xStart, unsigned short int yStart, unsigned short int length, unsigned short int height, unsigned short int vel, bool isSelected, bool isMuted){
  if(vel>125)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,isMuted);
  else if(vel>110)
    drawNote(id, track, xStart,yStart,length,height,2,isSelected,isMuted);
  else if(vel>100)
    drawNote(id, track, xStart,yStart,length,height,3,isSelected,isMuted);
  else if(vel>90)
    drawNote(id, track, xStart,yStart,length,height,4,isSelected,isMuted);
  else if(vel>80)
    drawNote(id, track, xStart,yStart,length,height,5,isSelected,isMuted);
  else if(vel>70)
    drawNote(id, track, xStart,yStart,length,height,6,isSelected,isMuted);
  else if(vel>60)
    drawNote(id, track, xStart,yStart,length,height,7,isSelected,isMuted);
  else if(vel>50)
    drawNote(id, track, xStart,yStart,length,height,8,isSelected,isMuted);
  else if(vel>40)
    drawNote(id, track, xStart,yStart,length,height,9,isSelected,isMuted);
  else if(vel>30)
    drawNote(id, track, xStart,yStart,length,height,10,isSelected,isMuted);
  else if(vel>20)
    drawNote(id, track, xStart,yStart,length,height,11,isSelected,isMuted);
  else if(vel>10)
    drawNote(id, track, xStart,yStart,length,height,12,isSelected,isMuted);
  else if(vel>0)
    drawNote(id, track, xStart,yStart,length,height,13,isSelected,isMuted);
  else if(vel == 0)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,true);
}

void drawNote_chance(uint16_t id, uint8_t track, unsigned short int xStart, unsigned short int yStart, unsigned short int length, unsigned short int height, unsigned short int chance, bool isSelected, bool isMuted){
  if(chance>=100)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,isMuted);
  else if(chance>90)
    drawNote(id, track, xStart,yStart,length,height,2,isSelected,isMuted);
  else if(chance>80)
    drawNote(id, track, xStart,yStart,length,height,3,isSelected,isMuted);
  else if(chance>70)
    drawNote(id, track, xStart,yStart,length,height,4,isSelected,isMuted);
  else if(chance>60)
    drawNote(id, track, xStart,yStart,length,height,5,isSelected,isMuted);
  else if(chance>50)
    drawNote(id, track, xStart,yStart,length,height,6,isSelected,isMuted);
  else if(chance>40)
    drawNote(id, track, xStart,yStart,length,height,7,isSelected,isMuted);
  else if(chance>30)
    drawNote(id, track, xStart,yStart,length,height,8,isSelected,isMuted);
  else if(chance>20)
    drawNote(id, track, xStart,yStart,length,height,9,isSelected,isMuted);
  else if(chance>10)
    drawNote(id, track, xStart,yStart,length,height,10,isSelected,isMuted);
  else if(chance>0)
    drawNote(id, track, xStart,yStart,length,height,11,isSelected,isMuted);
  else if(chance == 0)
    drawNote(id, track, xStart,yStart,length,height,12,isSelected,true);
}

bool isInView(int target){
  if(target>=viewStart && target<=viewEnd)
    return true;
  else
    return false;
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

void changeSubDivInt(bool down){
  changeSubDivInt(down,false);
}

void changeSubDivInt(bool down, bool limitToView){
  if(down){
    if(subDivInt>3 && (!limitToView || (subDivInt*scale)>2))
      subDivInt /= 2;
    else if(subDivInt == 3)
      subDivInt = 1;
  }
  else{
    if(subDivInt == 1)//if it's one, set it to 3
      subDivInt = 3;
    else if(subDivInt !=  96 && subDivInt != 32){
      //if triplet mode
      if(!(subDivInt%2))
        subDivInt *= 2;
      else if(!(subDivInt%3))
        subDivInt *=2;
    }
  }
  menuText = "~"+stepsToMeasures(subDivInt);
}

void changeFragmentSubDivInt(bool down){
  if(down){
    if(fragmentSubDiv>3)
      fragmentSubDiv /= 2;
    else if(fragmentSubDiv == 3)
      fragmentSubDiv = 1;
  }
  else{
    if(fragmentSubDiv == 1)//if it's one, set it to 3
      fragmentSubDiv = 3;
    else if(fragmentSubDiv !=  96 && fragmentSubDiv != 32){
      //if triplet mode
      if(!(fragmentSubDiv%2))
        fragmentSubDiv *= 2;
      else if(!(fragmentSubDiv%3))
        fragmentSubDiv *=2;
    }
  }
}
uint8_t toggleTriplets(uint8_t subDiv){
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
  menuText = stepsToMeasures(subDivInt);
  return subDiv;
}
void toggleTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDivInt == 192){
    subDivInt = 32;
  }
  else if(!(subDivInt%3)){//if it's in 1/4 mode...
    subDivInt = 2*subDivInt/3;//set it to triplet mode
  }
  else if(!(subDivInt%2)){//if it was in triplet mode...
    subDivInt = 3*subDivInt/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(subDivInt);
}

void toggleFragmentTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(fragmentSubDiv == 192){
    fragmentSubDiv = 32;
  }
  else if(!(fragmentSubDiv%3)){//if it's in 1/4 mode...
    fragmentSubDiv = 2*fragmentSubDiv/3;//set it to triplet mode
  }
  else if(!(fragmentSubDiv%2)){//if it was in triplet mode...
    fragmentSubDiv = 3*fragmentSubDiv/2;//set it to 1/4 mode
  }
}
//makes sure scale/viewend line up with the display
void checkView(){
  if(viewEnd>seqEnd){
    scale = float(96)/float(seqEnd);
    viewEnd = seqEnd+1;
  }
}
//zooms in/out
void zoom(bool in){
  uint16_t viewLength = viewEnd-viewStart;
  if(!in && viewLength<seqEnd){
    scale /= 2;
  }
  else if(in && viewLength/2>1){
    scale *= 2;
  }  
  viewStart = 0;
  viewEnd = 96/scale;
  checkView();

  changeSubDivInt(in);

  updateCursor();
  updateLEDs();

  menuText = stepsToMeasures(viewStart)+"<-->"+stepsToMeasures(viewEnd);
}
bool areThereAnyNotes(){
  for(uint8_t t = 0; t<seqData.size(); t++){
    if(seqData[t].size()>1){
      return true;
    }
  }
  return false;
}
//checks for notes above or below a track
bool areThereMoreNotes(bool above){
  if(trackData.size()>maxTracksShown){
    if(!above){
      for(int track = endTrack+1; track<trackData.size();track++){
        if(seqData[track].size()-1>0)
        return true;
      }
    }
    else if(above){
      for(int track = startTrack-1; track>=0; track--){
        if(seqData[track].size()-1>0)
        return true;
      }
    }
  }
  return false;
}

//DEPRECATED! i don't think i'll use this in any of the loop stuff
void drawLoopTimeLine(int xStart, int yStart){
  for(int step = 0; step <= seqEnd; step++){
    //if the step can be seen
    if(step*scale+xStart<screenWidth && step*scale+xStart>0){
      //draw a pixel every other
      if(!(step%2))
        display.drawPixel(step*scale+xStart,yStart,SSD1306_WHITE);
      //for the playhead
      if(step == playheadPos && playing){
        display.drawRoundRect(step*scale+xStart,yStart-5,step*scale+xStart+3,yStart+5,3,SSD1306_WHITE);
      }
      //if it's on a subDivInt
      if(!(step%subDivInt)){
        display.drawFastVLine(step*scale+xStart,yStart-3,6,SSD1306_WHITE);
      }
      if(!(step%96)){
        display.drawFastVLine(step*scale+xStart,yStart-5,10,SSD1306_WHITE);
      }
      if(step == seqEnd+1){
        display.setCursor(step*scale+xStart,yStart-3);
        display.print(")");
      }
      if(step == seqStart-1){
        display.setCursor(step*scale+xStart-4,yStart-3);
        display.print("(");
      }
      //drawing the loop start/ends
        //check if a loop starts or ends there, and if it does, draw it
      //loop through all loopData
      int flagHeight = 3;
      for(int loop = 0; loop<loopData.size(); loop++){
        //if the loop starts here
        if(step == loopData[loop][0]){
          display.drawFastVLine(step*scale+xStart,yStart,(loop+1)*8,SSD1306_WHITE);
          display.fillTriangle(xStart+scale*step, yStart-(loop+1)*5,step*scale+xStart, yStart-(loop+1)*5-flagHeight,step*scale+xStart+flagHeight, xStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          if(loopData.size()-1 == loop){//if it's the top loop
            display.setCursor(step*scale+xStart, yStart-(loop+1)*8);
          }
          else{
            display.setCursor(step*scale+xStart, yStart-(loop+1)*8);
          }
          if(loop == activeLoop){
            display.print("{");
            display.print(loop);
            display.print("}");
          }
          else{
            display.print(loop);
          }
        }
        //if the loop ends here
        if(step == loopData[loop][1]){
          display.drawLine(step*scale+xStart, yStart-(loop+1)*5, step*scale+xStart,yStart, SSD1306_WHITE);
          display.drawTriangle(step*scale+xStart, yStart-(loop+1)*5,step*scale+xStart, yStart-(loop+1)*5-flagHeight,step*scale+xStart-flagHeight, yStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          display.setCursor(step*scale+xStart+9, yStart-(loop+1)*8);
          display.print("{");
          display.print(loop);
          display.print("}");
        }
      }
    }
  }
}

void gridAnimation(bool in){
  //draws box, growing out from top left and bottom right corners
  //draws cursor, sliding in from top and left
  const uint8_t framerate = 12;
  if(in){
    for(int i = 0; i<=64; i+=framerate){
      display.clearDisplay();
      //top
      display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      drawDottedLineH(64-i/2,64+i/2,32,3);

      display.display();
    }
  }
  else{
    for(int i = 64; i>=0; i-=framerate){
      display.clearDisplay();
      //top
      display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      drawDottedLineH(64-i/2,64+i/2,32,3);

      display.display();
    }
  }
}
void xyGrid(){
  //range is (5-1023) aka 0-1018
  const float scaleF = float(128)/float(1018);
  // int8_t xCoord = abs(128 - (analogRead(x_Pin) - 5) * scaleF);
  // int8_t yCoord = abs(128 - (analogRead(y_Pin) - 5) * scaleF);
  int8_t xCoord = abs(128 - (analogRead(x_Pin) - 5) * scaleF);
  int8_t yCoord = abs(128 - (analogRead(y_Pin) - 5) * scaleF);
  uint8_t controlX = 1;
  uint8_t controlY = 2;

  uint8_t channelX = 1;
  uint8_t channelY = 1;

  bool pauseX = false;
  bool pauseY = false;
  gridAnimation(true);
  while(true){
    //a little averaging for smoother motion
    if(!pauseY)
      yCoord = (yCoord+abs(128 - (analogRead(y_Pin) - 5) * scaleF))/2;
      // yCoord = (yCoord+abs((analogRead(y_Pin) - 5) * scaleF))/2;
    if(!pauseX)
      // xCoord = (xCoord+abs(128 - (analogRead(y_Pin) - 5) * scaleF))/2;
      xCoord = (xCoord+abs((analogRead(x_Pin) - 5) * scaleF))/2;

    display.clearDisplay();
    //rec/play icon
    if(recording){
      if(externalClock && !gotClock){
        if(waiting){
          if(millis()%1000>500){
            display.drawCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          display.drawCircle(3,3,3,SSD1306_WHITE);
      }
      else if((externalClock && gotClock) || !externalClock){
        if(waiting){
          if(millis()%1000>500){
            display.fillCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          display.fillCircle(3,3,3,SSD1306_WHITE);
      }
    }
    else if(playing){
      if(externalClock && !gotClock){
        display.drawTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
      else{
        display.fillTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
    }
    display.drawRect(32,0,64,64,SSD1306_WHITE);
    //axes
    if(!pauseY)
      drawDottedLineV(64,0,64,3);
    if(!pauseX)
      drawDottedLineH(32,96,32,3);
    display.fillCircle(xCoord/2+34,64 - yCoord/2,3,SSD1306_WHITE);
    display.drawCircle(xCoord/2+34,64 - yCoord/2,5,SSD1306_WHITE);


    //printing values
    printSmall(40,57,"<"+stringify(xCoord)+">",SSD1306_WHITE);
    display.setRotation(1);
    printSmall(7,34,"<"+stringify(yCoord)+">",SSD1306_WHITE);
    display.drawBitmap(8,26,ch_tiny,6,3,1);
    // printSmall(5,26,"Value",SSD1306_WHITE);
    display.setRotation(UPRIGHT);

    //printing controller numbers
    printSmall(81-stringify(controlX).length()*4,2,"<"+stringify(controlX)+">",SSD1306_WHITE);
    display.setRotation(3);
    display.drawBitmap(8,26,cc_tiny,5,3,1);
    printSmall(7,34,"<"+stringify(controlY)+">",SSD1306_WHITE);
    // printSmall(5,26,"Controller",SSD1306_WHITE);
    display.setRotation(UPRIGHT);

    //x and y
    display.setFont(&FreeSerifItalic24pt7b);
    display.setCursor(35,22);
    if(!pauseX)
      display.print("x");
    display.setRotation(1);
    display.setCursor(1,94);
    if(!pauseY)
      display.print("y");
    display.setRotation(UPRIGHT);
    display.setFont();

    display.display();

    //if you're not keybinding, keep sending vals
    if(!shift){
      sendMIDICC(controlX,xCoord,channelX);
      sendMIDICC(controlY,yCoord,channelY);
      pauseX = false;
      pauseY = false;
    }

    readButtons();
    joyRead();

    //sending CC vals for keybinding
    if(shift){
      if(x != 0){
        sendMIDICC(controlX,xCoord,channelX);
        pauseY = true;
        pauseX = false;
      }
      else if(y != 0){
        sendMIDICC(controlY,yCoord,channelY);
        pauseY = false;
        pauseX = true;
      }
    }

    while(counterA != 0){
      if(counterA >= 1 && controlY<127){
        controlY++;
      }
      else if(counterA <= -1 && controlY>0){
        controlY--;
      }
      counterA += counterA<0?1:-1;;
    }
    while(counterB != 0){
      if(counterB >= 1 && controlX<127){
        controlX++;
      }
      else if(counterB <= -1 && controlX>0){
        controlX--;
      }
      counterB += counterB<0?1:-1;;
    }
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        gridAnimation(false);
        return;
      }
      if(play){
        togglePlayMode();
        lastTime = millis();
      }
      if(track_Press){
        pauseY = !pauseY;
        if(pauseY)
          pauseX = false;
        lastTime = millis();
      }
      if(note_Press){
        pauseX = !pauseX;
        if(pauseX)
          pauseY = false;
        lastTime = millis();
      }
    }
  }
}

// #include "mainSequencer.h"
/*
    This is the code for the main sequencer piano roll view
*/

void drawPram(uint8_t x1, uint8_t y1){
  if(onBeat(2,30))
    display.drawBitmap(x1,y1+1,carriage_bmp,14,15,SSD1306_WHITE);
  else
    display.drawBitmap(x1,y1,carriage_bmp,14,15,SSD1306_WHITE);
}

void drawPlayIcon(int8_t x1, int8_t y1){
  if(externalClock && !gotClock){
    display.drawTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
  else{
    display.fillTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
}

void drawPower(uint8_t x, uint8_t y){
  bool usb = digitalRead(usbPin);
  if(usb){
    display.drawBitmap(x,y+1,tiny_usb,10,4,SSD1306_WHITE);
  }
  else{
    display.drawBitmap(x,y,batt_bmp,10,7,SSD1306_WHITE);
    int batt = getBattLevel();
    //pico input SHOULD range from 1.8 (lowest it can handle) to 4.8 (max w/3AA's)
    //increments of 6 => 3/6 = 0.5
    if(batt<=2.0){
    }
    else if(batt<2.5){
      display.fillRect(x+2,y+2,1,3,SSD1306_WHITE);
    }
    else if(batt<3){
      display.fillRect(x+2,y+2,2,3,SSD1306_WHITE);
    }
    else if(batt<3.5){
      display.fillRect(x+2,y+2,3,3,SSD1306_WHITE);
    }
    else if(batt<4){
      display.fillRect(x+2,y+2,4,3,SSD1306_WHITE);
    }
    else if(batt<4.5){
      display.fillRect(x+2,y+2,5,3,SSD1306_WHITE);
    }
    else{
      display.fillRect(x+2,y+2,6,3,SSD1306_WHITE);
    }
  }
}

#define BATTSCALE 0.009677419355
//float(3.3*3)/float(1023);

float getBattLevel(){
  float val = float(analogRead(Vpin))*BATTSCALE;
  return val;
}

//draws pram, other icons (not loop points tho)
void drawTopIcons(){
  if(!menuIsActive || (activeMenu.menuTitle == "MENU") || (activeMenu.menuTitle == "FX") || (activeMenu.menuTitle == "EDIT" && activeMenu.topL[0]>32)){
    if(maxTracksShown == 5){
      if(!playing && !recording){
        drawPram(5,0);
      }
      //pram bounces faster
      else if(playing || recording){
        //if the playhead/rechead is on a subdiv, bounce the pram (this is set in loop1, it's too hard to do accurately here)
        display.drawBitmap(5,pramOffset,carriage_bmp,14,15,SSD1306_WHITE);
      }
    }
    else{
      if(!playing && !recording){
        if(onBeat(2,30))
          display.drawBitmap(8,1,tinyPram,7,7,SSD1306_WHITE);
        else
          display.drawBitmap(8,0,tinyPram,7,7,SSD1306_WHITE);
      }
      //pram bounces faster
      else if(playing || recording){
        //if the playhead/rechead is on a subdiv, bounce the pram (this is set in loop1, it's too hard to do accurately here)
        display.drawBitmap(8,pramOffset,tinyPram,7,7,SSD1306_WHITE);
      }
    }
  }

  // if it's recording and waiting for a note
  if(recording && waiting){
    if(millis()%1000>500){
      writeLEDs(0,true);
    }
    else{
      writeLEDs(0,false);
    }
  }

  //music symbol while receiving notes
  if(isReceiving()){
    if(!((animOffset/10)%2)){
      display.drawChar(21,0,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
    else{
      display.drawChar(21,1,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
  }

  //note presence indicator(if notes are offscreen)
  if(areThereMoreNotes(true)){
    uint8_t y1 = (maxTracksShown>5&&!menuIsActive)?8:debugHeight;
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,y1+3,trackDisplay-3,y1+3,trackDisplay-5,y1+1, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,y1+2,trackDisplay-3,y1+2,trackDisplay-5,y1, SSD1306_WHITE);
    }
  }
  if(areThereMoreNotes(false)){
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,screenHeight-5,trackDisplay-3,screenHeight-5,trackDisplay-5,screenHeight-3, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,screenHeight-4,trackDisplay-3,screenHeight-4,trackDisplay-5,screenHeight-2, SSD1306_WHITE);
    }
  }

  uint8_t x1 = 32;
  //most recent note played or received
  if(playlist.size()>0){
    // String p = pitchTostringify(playlist[playlist.size()-1][0],true,true);
    // printSmall(x1,1+sin(millis()/100),p,1);
    // x1 += p.length()*4;
  }
  //rec/play icon
  if(recording){
    if(externalClock && !gotClock){
      if(waiting){
        if(millis()%1000>500){
          display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    else if((externalClock && gotClock) || !externalClock){
      if(waiting){
        if(millis()%1000>500){
          display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    x1+=8;
    switch(recMode){
      //if one-shot rec
      case 0:
        display.drawBitmap(x1+sin(millis()/100),0,oneShot_rec,7,7,SSD1306_WHITE);
        x1+=8;
        break;
    }
  }
  else if(playing){
    drawPlayIcon(trackDisplay+sin(millis()/100)+1,0);
    x1 += 10;
    switch(isLooping){
      //if not looping
      case 0:
        if(millis()%900>600){
          display.drawBitmap(x1,0,oneshot3,8,7,SSD1306_WHITE);
        }
        else if(millis()%900>300){
          display.drawBitmap(x1,0,oneshot2,8,7,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(x1,0,oneshot1,8,7,SSD1306_WHITE);
        }
        x1+=10;
        break;
      //if looping
      case 1:
        if(millis()%1000>500){
          display.drawBitmap(x1,0,toploop_arrow1,7,7,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(x1,0,toploop_arrow2,7,7,SSD1306_WHITE);
        }
        x1+=10;
        break;
    }

  }
  //Data track icon
  if(dataTrackData.size()>0){
    if(millis()%1600>800)
      display.drawBitmap(x1,1,sine_small_bmp,6,4,SSD1306_WHITE);
    else{
      display.drawBitmap(x1,1,sine_small_reverse_bmp,6,4,SSD1306_WHITE);
    }
    x1+=8;
  }

  //swing icon
  if(swung){
    uint16_t a = millis()%800;
    if(a>600)
      display.drawBitmap(x1,0,swing_1,7,8,SSD1306_WHITE);
    else if(a>400)
      display.drawBitmap(x1,0,swing_2,7,8,SSD1306_WHITE);
    else if(a>200)
      display.drawBitmap(x1,0,swing_1,7,8,SSD1306_WHITE);
    else if(a>=0)
      display.drawBitmap(x1,0,swing_3,7,8,SSD1306_WHITE);
    x1+=10;
  }
  //fragment gem
  if(isFragmenting){
    drawTetra(x1,5,10+sin(float(millis())/float(500)),10+sin(float(millis())/float(500)),6+sin(float(millis())/float(500)),1+sin(float(millis())/float(500)),0,SSD1306_WHITE);
    x1+=8;
  }

  //velocity/chance indicator while shifting
  if(shift){
    if(displayingVel){
      if(lookupData[activeTrack][cursorPos] == 0){
        printSmall(x1,1,"v:"+stringify(defaultVel),SSD1306_WHITE);
        x1+=20;
      }
      else{
        printSmall(x1,1,"v:"+stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].velocity),SSD1306_WHITE);
        x1+=20;
      }
    }
    else{
      if(lookupData[activeTrack][cursorPos] == 0){
        printSmall(x1,1,"c:"+stringify(100),SSD1306_WHITE);
        x1+=20;
      }
      else{
        printSmall(x1,1,"c:"+stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].chance),SSD1306_WHITE);
        x1+=20;
      }
    }
  }
  if(isArping){
    display.drawPixel(x1,3+2*sin(float(millis())/float(200)),1);
    display.drawPixel(x1+2,3+2*sin(float(millis())/float(200)+100),1);
    display.drawPixel(x1+4,3+2*sin(float(millis())/float(200)+200),1);
    x1+=8;
  }

  //draw menu text
  printSmall(x1,1,menuText,SSD1306_WHITE);
  x1+=menuText.length()*4+2;

  //power/battery indicator
  drawPower(screenWidth-10,0);
}

//same function, but doesn't clear or display the screen
void drawSeq(){
  drawSeq(true,true,true,true,false);
}
//sends data to screen
//move through rows/columns, printing out data
void displaySeq(){
  display.clearDisplay();
  drawSeq();
  display.display();
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection){
  drawSeq(trackLabels,topLabels,loopPoints,menus,trackSelection,viewStart,viewEnd);
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, uint16_t start, uint16_t end){
  if(!screenSaving){
    //handling the note view
    if(end>seqEnd){
      end = seqEnd;
    }
    if(start>end){
      start = seqStart;
    }
    uint16_t viewLength = end - start;

    uint8_t startHeight = 0;
    bool loopFlags = loopPoints;

    if(maxTracksShown == 6 && !menuIsActive){
      startHeight = 8;
      trackHeight = (screenHeight-startHeight)/maxTracksShown;
      loopFlags = false;
    }
    else{
      trackHeight = (screenHeight-debugHeight)/maxTracksShown;
      startHeight = debugHeight;
    }

    if(trackData.size()>maxTracksShown){
      endTrack = startTrack + maxTracksShown;
    }
    else{
      endTrack = startTrack + trackData.size();
    }
    while(activeTrack>=endTrack && trackData.size()>maxTracksShown){
      startTrack++;
      endTrack++;
    }
    while(activeTrack<startTrack && trackData.size()>maxTracksShown){
      startTrack--;
      endTrack--;
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    // display.setTextWrap(false);

    //drawing selection box, since it needs to overlay stepSeq data
    if(selBox.begun){
      selBox.displaySelBox();
    }
    uint8_t height;
    if(endTrack == trackData.size())
      height = startHeight+trackHeight*maxTracksShown;
    else if(trackData.size()>maxTracksShown)
      height = startHeight+trackHeight*(maxTracksShown+1);
    else
      height = startHeight+trackHeight*trackData.size();

    //drawing the measure bars
    for (int step = start; step < end; step++) {
      unsigned short int x1 = trackDisplay+int((step-start)*scale);
      unsigned short int x2 = x1 + (step-start)*scale;
      //if the last track is showing
      if(endTrack == trackData.size()){
        //measure bars
        if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>1) {
          drawDottedLineV(x1,startHeight,height,2);
        }
        if(!(step%96)){
          drawDottedLineV2(x1,startHeight,height,6);
        }
      }
      else{
        //measure bars
        if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>1) {
          drawDottedLineV(x1,startHeight,height,2);
        }
        if(!(step%96)){
          drawDottedLineV2(x1,startHeight,height,6);
        }
      }

      //drawing loop points
      if(loopPoints){//check
        if(step == loopData[activeLoop][0]){
          if(loopFlags){
            if(movingLoop == -1 || movingLoop == 2){
              display.fillTriangle(trackDisplay+(step-start)*scale, debugHeight-3-sin(millis()/50), trackDisplay+(step-start)*scale, debugHeight-7-sin(millis()/50), trackDisplay+(step-start)*scale+4, debugHeight-7-sin(millis()/50),SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-start)*scale,debugHeight-3,3,SSD1306_WHITE);
            }
            else{
              if(cursorPos == step){
                display.fillTriangle(trackDisplay+(step-start)*scale, debugHeight-3, trackDisplay+(step-start)*scale, debugHeight-7, trackDisplay+(step-start)*scale+4, debugHeight-7,SSD1306_WHITE);
                display.drawFastVLine(trackDisplay+(step-start)*scale,debugHeight-3,3,SSD1306_WHITE);
              }
              else{
                display.fillTriangle(trackDisplay+(step-start)*scale, debugHeight-1, trackDisplay+(step-start)*scale, debugHeight-5, trackDisplay+(step-start)*scale+4, debugHeight-5,SSD1306_WHITE);
              }
            }
          }
          else{
            display.drawPixel(trackDisplay+(loopData[activeLoop][0]-start)*scale, startHeight-1,1);
          }
          if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
            display.drawFastVLine(trackDisplay+(step-start)*scale,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-start)*scale-1,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
          }
        }
        if(step == loopData[activeLoop][1]-1){
          if(loopFlags){
            if(movingLoop == 1 || movingLoop == 2){
              display.drawTriangle(trackDisplay+(loopData[activeLoop][1]-start)*scale, debugHeight-3-sin(millis()/50), trackDisplay+(loopData[activeLoop][1]-start)*scale-4, debugHeight-7-sin(millis()/50), trackDisplay+(loopData[activeLoop][1]-start)*scale, debugHeight-7-sin(millis()/50),SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(loopData[activeLoop][1]-start)*scale,debugHeight-3,3,SSD1306_WHITE);
            }
            else{
              if(cursorPos == step+1){
                display.drawTriangle(trackDisplay+(loopData[activeLoop][1]-start)*scale, debugHeight-3, trackDisplay+(loopData[activeLoop][1]-start)*scale-4, debugHeight-7, trackDisplay+(loopData[activeLoop][1]-start)*scale, debugHeight-7,SSD1306_WHITE);
                display.drawFastVLine(trackDisplay+(loopData[activeLoop][1]-start)*scale,debugHeight-3,3,SSD1306_WHITE);
              }
              else{
                display.drawTriangle(trackDisplay+(loopData[activeLoop][1]-start)*scale-1, debugHeight-1, trackDisplay+(loopData[activeLoop][1]-start)*scale-5, debugHeight-5, trackDisplay+(loopData[activeLoop][1]-start)*scale-1, debugHeight-5,SSD1306_WHITE);
              }
            }
          }
          else{
            display.drawPixel(trackDisplay+(loopData[activeLoop][1]-start)*scale, startHeight-1,1);
          }
          if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
            display.drawFastVLine(trackDisplay+(loopData[activeLoop][1]-start)*scale+1,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(loopData[activeLoop][1]-start)*scale+2,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
          }
        }
        if(movingLoop == 2){
          if(step>loopData[activeLoop][0] && step<loopData[activeLoop][1] && step%2){
            display.drawPixel(trackDisplay+(step-start)*scale, startHeight-7-sin(millis()/50),SSD1306_WHITE);
          }
        }
        if(loopFlags && (step == loopData[activeLoop][0]+(loopData[activeLoop][1]-loopData[activeLoop][0])/2))
          printSmall(trackDisplay+(step-start)*scale-1,10,stringify(activeLoop),SSD1306_WHITE);
      }
    }

    //top and bottom bounds
    if(startTrack == 0){
      display.drawFastHLine(trackDisplay,startHeight,screenWidth,SSD1306_WHITE);
    }
    //if the bottom is in view
    if(endTrack == trackData.size()){
      // if(trackData.size()>maxTracksShown)
        display.drawFastHLine(trackDisplay,startHeight+trackHeight*maxTracksShown,screenWidth,SSD1306_WHITE);
      // else
      //   display.drawFastHLine(trackDisplay,startHeight+trackHeight*trackData.size(),screenWidth,SSD1306_WHITE);

    }
    else if(endTrack<=trackData.size()-1)
        endTrack++;
    //drawin all da steps
    //---------------------------------------------------
    for (int track = startTrack; track < endTrack; track++) {
      unsigned short int y1 = (track-startTrack) * trackHeight + startHeight;
      unsigned short int y2 = y1 + trackHeight;
      uint8_t xCoord;
      //track info display
      if(activeTrack == track){
        xCoord = 9;
        // display.setCursor(9, y1+trackHeight/2-2);
        if(trackLabels)
          drawArrow(6+sin(millis()/200),y1+trackHeight/2+1,2,0,true);
      }
      else{
        xCoord = 5;
        // display.setCursor(5, y1+trackHeight/2-2);
      }
      if(trackLabels){
        if(!isShrunk){
          //printing note names
          if(pitchesOrNumbers){
            int8_t octave = (trackData[track].pitch/12)-2;//idk why this offset is needed
            printTrackPitch(xCoord, y1+trackHeight/2-2,getTrackPitchOctave(track),false,false,SSD1306_WHITE);
          }
          //just printing pitches
          else{
            display.setCursor(xCoord,y1);
            display.print(trackData[track].pitch);
          }
          if(shift || (menuIsActive && activeMenu.menuTitle == "TRK")){
            if(trackData[track].isLatched){
              if(activeTrack == track)
                printSmall(3,y1+3,"<",SSD1306_WHITE);
              else
                printSmall(0,y1+3,"<",SSD1306_WHITE);
            }
            printSmall(trackDisplay-stringify(trackData[track].channel).length()*4,y1+2,stringify(trackData[track].channel),SSD1306_WHITE);
          }
          else{
            if(track == activeTrack && isModulated(trackData[track].channel)){
              if(playing)
                display.drawBitmap(trackDisplay-7,y1+3+sin(millis()/100),sine_small_bmp,6,4,SSD1306_WHITE);
              else
                display.drawBitmap(trackDisplay-7,y1+2,sine_small_bmp,6,4,SSD1306_WHITE);
            }
          }
          if(trackData[track].noteLastSent != 255){
            // display.drawRect(0,y1,trackDisplay,trackHeight+2,SSD1306_WHITE);
            printSmall(trackDisplay-5,y1+1+sin(millis()/50),"$",1);
          }
          //track prime icon
          if(recording && trackData[track].isPrimed){
            if((millis()+track*10)%1000>500){
              display.fillCircle(trackDisplay-5,y1+trackHeight/2+1,2,1);
            }
            else{
              display.drawCircle(trackDisplay-5,y1+trackHeight/2+1,2,1);
            }
          }
        }
        //if it's shrunk, draw it small
        else{
          int octave = (trackData[track].pitch/12)-2;//idk why this offset is needed
          String pitch = getTrackPitchOctave(track);
          if(track%2){
            printSmall(18, y1, pitchToString(trackData[track].pitch,true,true), SSD1306_WHITE);
          }
          else
            printSmall(2, y1, pitchToString(trackData[track].pitch,true,true), SSD1306_WHITE);
          if(trackData[track].noteLastSent != 255){
            display.drawRect(0,y1,trackDisplay,trackHeight+2,SSD1306_WHITE);
          }
        }
      }
      //if you're only drawing selected tracks, and this track ISN'T selected, shade it
      if(trackSelection && !trackData[track].isSelected){
        display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,3);
      }
      //if the track is muted, just hatch it out (don't draw any notes)
      //if it's solo'd and muted, draw it normal (solo overrules mute)
      else if(trackData[track].isMuted && !trackData[track].isSolo){
        // display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,9);
        continue;
      }
      else{
        //highlight for solo'd tracks
        if(trackData[track].isSolo)
          drawNoteBracket(trackDisplay+3,y1-1,screenWidth-trackDisplay-5,trackHeight+2,true);
        // display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
        for (int step = start; step < end; step++) {
          int id = lookupData[track][step];
          unsigned short int x1 = trackDisplay+int((step-start)*scale);
          unsigned short int x2 = x1 + (step-start)*scale;
          //drawing note
          if (id != blank_ID){
            if(step == seqData[track][id].startPos){
              uint16_t length = (seqData[track][id].endPos - seqData[track][id].startPos)*scale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,seqData[track][id].velocity,seqData[track][id].isSelected,seqData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,seqData[track][id].chance,seqData[track][id].isSelected,seqData[track][id].muted);
              if(seqData[track][id].isSelected){
                display.drawRect(x1,y1,length+1,trackHeight,SSD1306_WHITE);
                display.drawRect(x1+1,y1+1,length-1,trackHeight-2,SSD1306_BLACK);
              }
            }
            else if(!isInView(seqData[track][id].startPos) && step == start){
              unsigned short int length = (seqData[track][id].endPos - start)*scale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,seqData[track][id].velocity,seqData[track][id].isSelected,seqData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,seqData[track][id].chance,seqData[track][id].isSelected,seqData[track][id].muted);
            }
          }
        }
      }
    }
    //---------------------------------------------------
    //drawing cursor
    if(!editingNote && !(menuIsActive && activeMenu.menuTitle == "EDIT" && activeMenu.page == 1)){
      uint8_t cPos = trackDisplay+int((cursorPos-start)*scale);
      if(cPos>127)
        cPos = 126;
      if(endTrack == trackData.size()){
        display.drawFastVLine(cPos, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((cursorPos-start)*scale), startHeight, 2, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
      }
      else{
        display.drawFastVLine(cPos, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((cursorPos-start)*scale), startHeight, 2, screenHeight-startHeight, SSD1306_WHITE);
      }
    }
    if(topLabels){
      //carriage bitmap/title
      drawTopIcons();
    }
    //playhead/rechead
    if(playing && isInView(playheadPos))
      display.drawRoundRect(trackDisplay+(playheadPos-start)*scale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    if(recording && isInView(recheadPos))
      display.drawRoundRect(trackDisplay+(recheadPos-start)*scale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    
    int cursorX = trackDisplay+int((cursorPos-start)*scale)-8;
    if(!playing && !recording){
      cursorX = 32;
    }
    else{
      //making sure it doesn't print over the subdiv info
      cursorX = 42;
    }
    //SubDivInt
    // printSmall(cursorX,2,stepsToMeasures(subDivInt),SSD1306_WHITE);


    //drawing active track highlight
    unsigned short int x1 = trackDisplay;
    unsigned short int y1 = (activeTrack-startTrack) * trackHeight + startHeight;
    unsigned short int x2 = x1+screenWidth-trackDisplay;
    unsigned short int y2 = y1 + trackHeight;
    // display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);
    display.drawFastHLine(x1,y1,screenWidth-x1,1);
    display.drawFastHLine(x1,y1+trackHeight-1,screenWidth-x1,1);

    //CursorPos
    // printSmall(screenWidth-stepsToPosition(cursorPos,true).length()*4-12,2,stepsToPosition(cursorPos,true),SSD1306_WHITE);
  }
  if(menus){
    //drawing menu
    if(menuIsActive && activeMenu.menuTitle == "CURVE"){
      activeMenu.displayMenu(false,false,true);
    }
    else if(menuIsActive && activeMenu.menuTitle == "TRK")
      activeMenu.displayMenu(true,false,false);
    else if(menuIsActive && activeMenu.menuTitle == "EDIT")
      activeMenu.displayMenu(false, true, false);
    else if(menuIsActive && activeMenu.menuTitle == "FILES"){
      activeMenu.displayMenu(false,false,true);
    }
    else if(menuIsActive){
      activeMenu.displayMenu(true,true);
    }
  }

  //anim offset (for the pram)
  if(!menuIsActive){
    animOffset++;
    if(animOffset>100)
      animOffset = 0;
  }
}


//checks to see if a channel is currently being modulated
bool isModulated(uint8_t ch){
  for(uint8_t dt = 0; dt<dataTrackData.size(); dt++){
    if(dataTrackData[dt].parameterType == 2){
      if(dataTrackData[dt].channel == ch && dataTrackData[dt].isActive)
        return true;
    }
  }
  return false;
}

//sort function for sorting tracks by channel
bool compareChannels(Track t1, Track t2){
  return t1.channel>t2.channel;
}

void indicatorLED(){
  if(playing || recording){
    if(pramOffset){
      digitalWrite(ledPin,HIGH);
    }
    else{
      digitalWrite(ledPin,LOW);
    }
  }
  else{
    digitalWrite(ledPin,LOW);
  }
}
//displays notes in all tracks from viewStart -> viewEnd
void displaySeqSerial() {
  // return;
  unsigned short int id = lookupData[activeTrack][cursorPos];
  Serial.print("\n");
  for (int track = trackData.size() - 1; track > -1; track--) {
    Serial.print(getTrackPitch(track));
    int len = getTrackPitch(track).length();
    for(int i = 0; i< 5-len; i++){
      Serial.print(" ");
    }
    Serial.print("(");
    Serial.print(trackData[track].pitch);//prints the pitch before each track
    Serial.print(")");
    for (int note = viewStart; note < viewEnd*scale; note++) {
      if (!(note % subDivInt)) { //if note is a multiple of subDivInt, print a divider
        Serial.print("|");
      }
      //if no note
      if (lookupData[track][note] == blank_ID) {
        if (note == cursorPos) { //if the cursor is there
          Serial.print("#");
        }
        else if (track == activeTrack) { //if the track is active
          Serial.print("=");
        }
        else {
          Serial.print(".");//default track icon
        }
      }//if there's a tail_ID
      //if there is a note
      if (lookupData[track][note] != blank_ID) {
        if (note == cursorPos && track == activeTrack) { //if the cursor is on it and the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("{S}");
          }
          else {
            Serial.print("[N]");
          }
        }
        else if (track == activeTrack) { //if the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("s");
          }
          else {
            Serial.print("n");
          }
        }
        else { //default display of a note
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("s");
          }
          else {
            Serial.print("n");
          }
        }
      }
    }
    Serial.print("|\n");
  }
}

uint16_t getIDAtCursor(){
  return lookupData[activeTrack][cursorPos];
}

//Selection------------------------------------------------------------------------------------------

//this searches thru the buffer and deletes a target note, resets its tag
void clearSelection(int track, int time) {
  if(lookupData[track][time] != 0 && seqData[track][lookupData[track][time]].isSelected){
    selectionCount--;
    seqData[track][lookupData[track][time]].isSelected = false;
  }
}

void clearSelection(){
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      for(int note = seqData[track].size()-1; note>0; note--){
        if(seqData[track][note].isSelected){
          seqData[track][note].isSelected = false;
          selectionCount--;
        }
      }
    }
  }
}

void deselectNote(uint8_t track, uint16_t id){
  if(seqData[track][id].isSelected){
    selectionCount--;
    seqData[track][id].isSelected = false;
  }
}

void selectNotesInTrack(uint8_t track){
  for(uint16_t note = 1; note<seqData.size(); note++){
    if(!seqData[track][note].isSelected){
      seqData[track][note].isSelected = true;
      selectionCount++;
    }
  }
}

//select a note
void selectNote(uint8_t track, uint16_t id){
  //if it's already selected
  if(seqData[track][id].isSelected)
    return;
  seqData[track][id].isSelected = true;
  selectionCount++;
}

//togglet a note's selection state  by it's track and ID
void toggleSelectNote(uint8_t track, uint16_t id, bool additive){
    //if id == 0, just return
    if(!id){
      return;
    }
    if(!additive&&!seqData[track][id].isSelected){
      clearSelection();
      selectNote(track,id);
    }
    else{
      if(seqData[track][id].isSelected && selectionCount>0)
        deselectNote(track,id);
      else if(!seqData[track][id].isSelected){
        selectNote(track,id);
      }
    }
}


void selectAllNotesInTrack(){
  for(uint16_t i = 1; i<seqData[activeTrack].size();i++){
    selectNote(activeTrack, i);
  }
}

//selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
void selectAll() {
  for(uint8_t track = 0; track<trackData.size(); track++){
    for(uint16_t id = 1; id<seqData[track].size(); id++){
      selectNote(track,id);
    }
  }
}

uint8_t getLowestVal(vector<uint8_t> vec){
  uint8_t lowest = 255;
  for(uint8_t i = 0; i<vec.size(); i++){
    if(vec[i]<lowest)
      lowest = vec[i];
  }
  return lowest;
}

bool isInVector(int val, vector<uint8_t> vec){
  if(vec.size() == 0) 
    return false;
  for(int i = 0; i<vec.size(); i++){
    if(val == vec[i]){
      return true;
    }
  }
  return false;
}

void selectBox(){
  if(selBox.x1>selBox.x2){
    unsigned short int x1_old = selBox.x1;
    selBox.x1 = selBox.x2;
    selBox.x2 = x1_old;
  }
  if(selBox.y1>selBox.y2){
    unsigned short int y1_old = selBox.y1;
    selBox.y1 = selBox.y2;
    selBox.y2 = y1_old;
  }
  for(int track = selBox.y1; track<=selBox.y2; track++){
    for(int time = selBox.x1; time<selBox.x2; time++){//< and not <= so it doesn't grab trailing notes
      if(lookupData[track][time] != 0){
        //this is a little inconsistent with how select usually works, but it allows whatever's in the box to DEFINITELY be selected.
        //it makes sense (a little) because it seems rare that you would ever need to deselect notes using the box
        selectNote(track, lookupData[track][time]);
        time = seqData[track][lookupData[track][time]].endPos-1;
      }
    }
  }
}

void playTrack(uint8_t track, uint16_t timestep){
  //if there's no note, skip to the next track
  if (lookupData[track][timestep] == 0){
    if(trackData[track].noteLastSent != 255){//if the track was sending, send a note off
      sendMIDInoteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
      subNoteFromPlaylist(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
    }
    return;
  }
  //if there's a note there
  else{
    //if it's the start of the note, or if the track wasn't sending already
    if(timestep == seqData[track][lookupData[track][timestep]].startPos || trackData[track].noteLastSent == 255){ //if it's the start
      //if it's not muted
      if(!seqData[track][lookupData[track][timestep]].muted){
        //if the track was already sending a note, send note off
        if(trackData[track].noteLastSent != 255){
          sendMIDInoteOff(trackData[track].noteLastSent, 0, trackData[track].channel);
          trackData[track].noteLastSent = 255;
        }
        //modifying chance value and pitch value and vel
        int16_t chance = seqData[track][lookupData[track][timestep]].chance;
        int16_t pitch = trackData[track].pitch;
        int16_t vel = seqData[track][lookupData[track][timestep]].velocity;
        //if the channel matches, or if the modifier is global
        if(trackData[track].channel == chanceModifier[0] || chanceModifier[0] == 0){
          chance += chanceModifier[1];
          if(chance<0)
            chance = 0;
          else if(chance>100)
            chance = 100;
        }
        if(trackData[track].channel == pitchModifier[0] || pitchModifier[0] == 0){ 
          pitch += pitchModifier[1];
          if(pitch<0)
            pitch = 0;
          else if(pitch>127)
            pitch = 127;
        }
        if(trackData[track].channel == velModifier[0] || velModifier[0] == 0){
          vel += velModifier[1];
          // vel = vel*(float(velModifier[1])/float(127));
          // Serial.println("modifier: "+String(chanceModifier[1]));
          if(vel<0)
            vel = 0;
          else if(vel>127)
            vel = 127;
          // Serial.println("chance: "+String(chance));
          // Serial.println("modifier: "+String(velModifier[1]));
          // Serial.println("vel: "+String(vel));
        }
        // Serial.println(pitch);
        // Serial.println(vel);
        // Serial.println(chance);
        // Serial.flush();
        //if chance is 100%
        if(chance >= 100){
          //if it's part of a muteGroup
          if(trackData[track].muteGroup!=0){
            muteGroups(track, trackData[track].muteGroup);
          }
          sendMIDInoteOn(pitch, vel, trackData[track].channel);
          trackData[track].noteLastSent = pitch;
          if(trackData[track].isLatched)
            sendMIDInoteOff(pitch, 0, trackData[track].channel);
          addNoteToPlaylist(pitch,vel,trackData[track].channel);
          return;
        }
        else if(chance > 0){
          uint8_t odds = random(100);
          if(seqData[track][lookupData[track][timestep]].chance > odds){
            if(trackData[track].muteGroup!=0){
              muteGroups(track, trackData[track].muteGroup);
            }
            sendMIDInoteOn(pitch, vel, trackData[track].channel);
            trackData[track].noteLastSent = pitch;
            if(trackData[track].isLatched)
              sendMIDInoteOff(pitch, 0, trackData[track].channel);
            addNoteToPlaylist(pitch,vel,trackData[track].channel);
            return;
          }
        }
      }
    }
  }
}

void playDT(uint8_t dt, uint16_t timestep){
  if(dataTrackData[dt].isActive){
    if(timestep<dataTrackData[dt].data.size()){
      if(dataTrackData[dt].data[timestep] != 255)
        dataTrackData[dt].play(timestep);
    }
  }
}

void playStep(uint16_t timestep) {
    //playing dataTracks too
  for(uint8_t dT = 0; dT < dataTrackData.size(); dT++){
    playDT(dT,timestep);
  }
  //playing each track
  for (int track = 0; track < trackData.size(); track++) {
    //if it's unmuted or solo'd, play it
    if(!trackData[track].isMuted || trackData[track].isSolo)
      playTrack(track,timestep);
  }
}

void nudgeNote(short int Xamount, short int Yamount){
  unsigned short int id = lookupData[activeTrack][cursorPos];
  unsigned short int start = seqData[activeTrack][id].startPos;
  unsigned short int end = seqData[activeTrack][id].endPos;
  if(Xamount>=0){
    if(end+Xamount<seqEnd){
      moveNote(id, activeTrack, activeTrack+Yamount, start+Xamount);
      moveCursor(Xamount);
      activeTrack+=Yamount;
    }
  }
  else if(Xamount<= 0) {
    if(start-Xamount>=seqStart){
      moveNote(id, activeTrack, activeTrack+Yamount, start-Xamount);
      moveCursor(-Xamount);
      activeTrack+=Yamount;
    }
  }
}

//moves all selected notes (or doesn't)
bool moveSelectedNotes(int xOffset,int yOffset){
  // debugPrintLookup();
  //to temporarily store all the notes
  vector<vector<Note>> selectedNotes;
  selectedNotes.resize(seqData.size());

  //grab all the selected notes
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      //if the note is selected, push it into the buffer and then delete it
      if(seqData[track][note].isSelected){
        selectedNotes[track].push_back(seqData[track][note]);
        deleteNote(track, seqData[track][note].startPos);
        note--;
      }
      if(selectionCount==0){
        break;
      }
    }
    if(selectionCount==0){
      break;
    }
  }
  //check each note in the buffer to see if it's a valid move
  for(uint8_t track = 0; track<selectedNotes.size(); track++){
    for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
      //if it hits a single bad note, then remake all the notes and exit
      if(!checkNoteMove(selectedNotes[track][note],track,track+yOffset,selectedNotes[track][note].startPos+xOffset)){
        for(uint8_t track2 = 0; track2<selectedNotes.size(); track2++){
          for(uint16_t note2  = 0; note2<selectedNotes[track2].size(); note2++){
            //remake old note
            makeNote(selectedNotes[track2][note2],track2,false);
          }
        }
        return false;
      }
    }
  }
  //if all notes pass the check... move em!
  for(uint8_t track = 0; track<selectedNotes.size(); track++){
    for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
      unsigned short int length = selectedNotes[track][note].endPos-selectedNotes[track][note].startPos;
      makeNote(track+yOffset, selectedNotes[track][note].startPos+xOffset, length+1, selectedNotes[track][note].velocity, selectedNotes[track][note].chance, selectedNotes[track][note].muted, selectedNotes[track][note].isSelected, false);
    }
  }
  // debugPrintLookup();
  return true;
}

//this one doesn't check for self collision! use it only if the note you're trying to move is already deleted
bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart){
  unsigned short int length = targetNote.endPos-targetNote.startPos;
  //checking bounds
  if(newStart<0 || newStart>seqEnd || newTrack>=trackData.size() || newTrack<0)
    return false;
  //checking lookupData
  for(uint16_t start = newStart; start < newStart+length; start++){
    //for moving horizontally within one track
    if(track == newTrack && lookupData[newTrack][start] != 0)
      return false;
    //for vertical kinds of movement, where you won't collide with yourself
    else if(lookupData[newTrack][start] != 0)
      return false;
  }
  return true;
}

bool checkNoteMove(int id, int track, int newTrack, int newStart){
  Note targetNote = seqData[track][id];
  unsigned short int length = targetNote.endPos-targetNote.startPos;
  //checking bounds
  if(newStart<0 || newStart>seqEnd || newTrack>=trackData.size() || newTrack<0){
    return false;
  }
  //checking lookupData
  for(uint16_t start = newStart; start < newStart+length; start++){
    //for moving horizontally within one track
    if(track == newTrack){
      if(lookupData[newTrack][start] != 0 && lookupData[newTrack][start] != id){
        return false;
      }
    }
    //for vertical kinds of movement, where you won't collide with yourself
    else{
      if(lookupData[newTrack][start] != 0){
        return false;
      } 
    }
  }
  return true;
}

void drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch){
  display.drawBitmap(x1,y1,ch_tiny,6,3,SSD1306_WHITE);
  printSmall(x1+7,y1,stringify(ch),1);
}

bool moveNote(int id,int track,int newTrack,int newStart){
  //if there's room
  if(checkNoteMove(id, track, newTrack, newStart)){
    Note targetNote = seqData[track][id];
    uint16_t length = targetNote.endPos-targetNote.startPos;
    //clear out old note
    deleteNote(track, targetNote.startPos);
    //make room
    makeNote(newTrack, newStart, length+1, targetNote.velocity, targetNote.chance, targetNote.muted, targetNote.isSelected, false);
    return true;
  }
  else{
    return false;
  }
}

void muteNote(unsigned short int track, unsigned short int id, bool toggle){
  if(id != 0){
    if(toggle){
      seqData[track][id].muted = !seqData[track][id].muted;
    }
    else{
      seqData[track][id].muted = true;
    }
  }
  else
    return;
}

void unmuteNote(unsigned short int track, unsigned short int id, bool toggle){
  if(id != 0){
    if(toggle){
      seqData[track][id].muted = !seqData[track][id].muted;
    }
    else{
      seqData[track][id].muted = false;
    }
  }
  else
    return;
}

void updateLookupData_track(unsigned short int track){
    lookupData[track].assign(seqEnd,0);//blank the track
    for(int id = 1; id<seqData[track].size()-1+1; id++){//for each note in data
      unsigned short int start = seqData[track][id].startPos;
      unsigned short int end = seqData[track][id].endPos;
      for(int i = start; i<end; i++){
        lookupData[track][i] = id;
      }
    }
}

//idrk why this is broken, but this is super broken (but maybe not???)
void updateLookupData(){
  for(int track = 0; track<trackData.size(); track++){//for each track
    updateLookupData_track(track);
  }
}

void writeCC(uint16_t step, uint8_t channel, uint8_t controller, uint8_t value){
  for(uint8_t dt = 0; dt < dataTrackData.size(); dt++){
    //if the track is primed
    if(dataTrackData[dt].isPrimed){
      //if the channel and control number match
      if(channel == dataTrackData[dt].channel && controller == dataTrackData[dt].control){
        dataTrackData[dt].data[step] = value;
      }
    }
  }
}

void writeNoteOn(unsigned short int step, uint8_t pitch, uint8_t vel, uint8_t channel){
  int track = makeTrackWithPitch(pitch,channel);
  if(trackData[track].isPrimed){
    Note newNote(step, step, vel);//this constuctor sets the endPos of the note at the same position
    if(lookupData[track][step] != 0){
      deleteNote(track,step);
    }
    seqData[track].push_back(newNote);
    lookupData[track][step] = seqData[track].size()-1;
    trackData[track].noteLastSent = pitch;
  }
}

void writeNoteOff(unsigned short int step, uint8_t pitch, uint8_t channel){
  short int track = getTrackWithPitch(pitch,channel);
  if(trackData[track].isPrimed && trackData[track].noteLastSent != 255){
    unsigned short int note = seqData[track].size()-1;
    //if the track actually was sending, and exists
    if(trackData[track].noteLastSent != 255 && track != -1){
      seqData[track][note].endPos = step;
      trackData[track].noteLastSent = 255;

      // if(lookupData[track][step] == note){
      //   lookupData[track][step] = 0;
      // }
      // lookupData[track][step] = note;
    }
  }
}

//continues notes that were started, and deletes notes in their way
void continueStep(unsigned short int step){
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].isPrimed){
      if(trackData[track].noteLastSent != 255){
        int id = seqData[track].size()-1;
        if(id>0){
          //if there's a different note at this step, delete it
          if(lookupData[track][step] != 0 && lookupData[track][step] != id){
            //if it's in overwrite mode, delete the conflicting note
            if(overwriteRecording){
              deleteNote(track, step);
              id = seqData[track].size()-1;
            }
            //if it's not in overwrite mode, then end the note 
            else{
              seqData[track][id].endPos = step;
              trackData[track].noteLastSent = 255;
              continue;
            }
          }
          lookupData[track][step] = id;
          seqData[track][id].endPos = step;
        }
      }
      //if it's not being written to, clear this step out
      else if(overwriteRecording){
        //if there's something there, and the track isn't sending (make sure it's not a tail note that just got written)
        //AND make sure that the track is primed! if not, then don't overwrite it
        if(lookupData[track][step] != 0){
          //if it's not the end of the note (since that note might have been written)
          //i don't think this will cause problems? 
          if(seqData[track][lookupData[track][step]].endPos != step){
            deleteNote_byID(track, lookupData[track][step]);
          }
        }
      }
    }
    //if the track isn't primed, play it normally
    else if(!trackData[track].isPrimed){
      // playTrack(track,step);
    }
  }
}

void handleStop_playing(){
  hasStarted = false;
  stop();
}

void handleClock_playing(){
  gotClock = true;
}

void handleStart_playing(){
  hasStarted = true;
  startTime = micros();
}

void handleClock_recording(){
  gotClock = true;
}

void handleStart_recording(){
  hasStarted = true;
  startTime = micros();
  if(waitForNote && waiting){
    waiting = false;
  }
}

void handleStop_recording(){
  hasStarted = false;
}

void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOn(recheadPos, note, velocity, channel);
  waiting = false;
  recentNote[0] = note;
  recentNote[1] = velocity;
  recentNote[2] = channel;
  noteOnReceived = true;
}

void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOff(recheadPos, note, channel);
  waiting = false;
  noteOffReceived = true;
}

void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value){
  writeCC(recheadPos,channel,cc,value);
  recentCC[0] = cc;
  recentCC[1] = value;
  recentCC[2] = channel;
  waiting = false;
}

void handleNoteOn_Listening(uint8_t channel, uint8_t note, uint8_t velocity){
  int track = makeTrackWithPitch(note,channel);
  trackData[track].noteLastSent = note;
}

void handleNoteOff_Listening(uint8_t channel, uint8_t note, uint8_t velocity){
  trackData[getTrackWithPitch(note)].noteLastSent = 255;
}

void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value){
  recentCC[0] = cc;
  recentCC[1] = value;
  recentCC[2] = channel;
}

void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  addNoteToPlaylist(note,velocity,channel);  
  int track = getTrackWithPitch(note);
  if(track != -1){
    trackData[track].noteLastSent = note;
  }
  sendThruOn(channel, note, velocity);
  recentNote[0] = note;
  recentNote[1] = velocity;
  recentNote[2] = channel;
  // printPlaylist();
  noteOnReceived = true;
}

void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity){
  subNoteFromPlaylist(note);
  // printPlaylist();
  int track = getTrackWithPitch(note);
  if(track != -1){
    trackData[track].noteLastSent = 255;
  }
  sendThruOff(channel, note);
  noteOffReceived = true;
}

void handleStart_Normal(){
  if(externalClock){
    if(!playing && !recording){
      togglePlayMode();
    }
  }
}

void handleStop_Normal(){
  if(externalClock){
    if(playing){
      togglePlayMode();
    }
  }
}

void cleanupRecording(uint16_t stopTime){
  for(int8_t i = 0; i<trackData.size(); i++){
    //if there's a note on this track
    if(seqData[i].size()-1>0){
      //if the track was sending
      if(trackData[i].noteLastSent != 255){
        //set the end of the note to the stop time
        seqData[i][seqData[i].size()-1].endPos = stopTime;
        trackData[i].noteLastSent = 255;
      }
      for(uint16_t note = 1; note<seqData[i].size(); note++){
        //if the note is fucked up
        if(seqData[i][note].startPos>=seqData[i][note].endPos){
          seqData[i][note].endPos = seqData[i][note].startPos+1;
        }
      }
    }
  }
  updateLookupData();
  displaySeqSerial();
}

bool isReceiving(){
  for(int track = 0; track<trackData.size(); track++){
    if(trackData[track].noteLastSent != 255)
      return true;
  }
  if(playlist.size()>0){
    return true;
  }
  return false;
}

//prints out lookupdata
void debugPrintLookup(){
  for(int i = 0; i<trackData.size();i++){
    Serial.print(stringify(i)+":"+getTrackPitch(i));
    Serial.print('|');
    for(int j = seqStart; j<= seqEnd; j++){
      // if(!j%subDivInt)
      //   Serial.print('|');
      if(lookupData[i][j]==0)
        Serial.print('-');
      else
        Serial.print(lookupData[i][j]);
    }
    Serial.println('|');
  }
}
void togglePlayMode(){
  playing = !playing;
  //if it's looping, set the playhead to the activeLoop start
  if(isLooping)
    playheadPos = loopData[activeLoop][0];
  else
    playheadPos = 0;
  if(playing){
    if(recording){
      toggleRecordingMode(waitForNote);
    }
    #ifndef HEADLESS
    MIDI1.setHandleClock(handleClock_playing);
    MIDI1.setHandleStart(handleStart_playing);
    MIDI1.setHandleStop(handleStop_playing);
    MIDI0.setHandleClock(handleClock_playing);
    MIDI0.setHandleStart(handleStart_playing);
    MIDI0.setHandleStop(handleStop_playing);
    #endif

    startTime = micros();
    if(isArping){
      activeArp.start();
    }
    sendMIDIStart();
  }
  else if(!playing){
    stop();
    setNormalMode();
    sendMIDIStop();
    velModifier[1] = 0;
    chanceModifier[1] = 0;
    pitchModifier[1] = 0;
  }
}

void setNormalMode(){
  stop();
  if(isArping){
    activeArp.stop();
  }
  #ifndef HEADLESS
  MIDI1.disconnectCallbackFromType(midi::Clock);
  MIDI1.disconnectCallbackFromType(midi::Start);
  MIDI1.disconnectCallbackFromType(midi::Stop);
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.disconnectCallbackFromType(midi::Clock);
  MIDI0.disconnectCallbackFromType(midi::Start);
  MIDI0.disconnectCallbackFromType(midi::Stop);
  MIDI0.disconnectCallbackFromType(midi::NoteOn);
  MIDI0.disconnectCallbackFromType(midi::NoteOff);
  MIDI0.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.setHandleNoteOn(handleNoteOn_Normal);
  MIDI0.setHandleNoteOff(handleNoteOff_Normal);
  MIDI0.setHandleStart(handleStart_Normal);
  MIDI0.setHandleStop(handleStop_Normal);

  MIDI1.setHandleNoteOn(handleNoteOn_Normal);
  MIDI1.setHandleNoteOff(handleNoteOff_Normal);
  MIDI1.setHandleStart(handleStart_Normal);
  MIDI1.setHandleStop(handleStop_Normal);

  MIDI0.setHandleControlChange(handleCC_Normal);
  MIDI1.setHandleControlChange(handleCC_Normal);
  #endif
}

void toggleListeningMode(){
  listening = !listening;
  #ifndef HEADLESS
  if(listening){
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);
    MIDI1.setHandleNoteOn(handleNoteOn_Listening);
    MIDI1.setHandleNoteOff(handleNoteOff_Listening);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
    MIDI0.setHandleNoteOn(handleNoteOn_Listening);
    MIDI0.setHandleNoteOff(handleNoteOff_Listening);
  }
  if(!listening){
    stop();
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
  }
  #endif
}

void toggleRecordingMode(bool butWait){
  recording = !recording;
  //if it stopped recording
  if(!recording)
    cleanupRecording(recheadPos);
  //if it's recording to the loop
  if(recMode == 0 || recMode == 1)
    recheadPos = loopData[activeLoop][0];
  if(butWait){
    waiting = true;
  }
  if(recording){
    if(playing){
      togglePlayMode();
    }
    stop();
    #ifndef HEADLESS
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);
    MIDI1.disconnectCallbackFromType(midi::Clock);
    MIDI1.disconnectCallbackFromType(midi::Start);
    MIDI1.disconnectCallbackFromType(midi::Stop);
    MIDI1.setHandleNoteOn(handleNoteOn_Recording);
    MIDI1.setHandleNoteOff(handleNoteOff_Recording);
    MIDI1.setHandleClock(handleClock_recording);
    MIDI1.setHandleStart(handleStart_recording);
    MIDI1.setHandleStop(handleStop_recording);
    MIDI1.setHandleControlChange(handleCC_Recording);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
    MIDI0.disconnectCallbackFromType(midi::Clock);
    MIDI0.disconnectCallbackFromType(midi::Start);
    MIDI0.disconnectCallbackFromType(midi::Stop);
    MIDI0.setHandleNoteOn(handleNoteOn_Recording);
    MIDI0.setHandleNoteOff(handleNoteOff_Recording);
    MIDI0.setHandleClock(handleClock_recording);
    MIDI0.setHandleStart(handleStart_recording);
    MIDI0.setHandleStop(handleStop_recording);
    MIDI0.setHandleControlChange(handleCC_Recording);
    #endif
    startTime = micros();
  }
  if(!recording){//go back to normal listening mode
    setNormalMode();
  }
}

unsigned short int getLowestPitch(){
  unsigned short int pitch = trackData[activeTrack].pitch;
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].pitch<pitch)
      pitch = trackData[i].pitch;
  }
  return pitch;
}

bool itsbeen(int time){
  if(millis()-lastTime >= time)
    return true;
  else
    return false;
}
void printSelectionBounds(){
  vector<vector<unsigned short int>> coords = getSelectionBounds();
  Serial.print("X1:");
  Serial.println(coords[0][0]);
  Serial.print("Y1:");
  Serial.println(coords[0][1]);
  Serial.print("X2:");
  Serial.println(coords[1][0]);
  Serial.print("Y2:");
  Serial.println(coords[1][1]);

}
//this is the big switch statement that listens for key inputs and runs the according functions
void keyListen() {
  unsigned char key = 0;
  String pitch;
  int track;
  key = Serial.read();
  switch (key) {
    case 'k':
      activeArp.start();
      break;
    case 'x':
      printSelectionBounds();
      break;
    case 'z':
      debugPrintSelection();
      break;
    case 'F':
      menuIsActive = true;
      genFragment();
      fragmentMenu();
      break;
    case '/':
      writeSeqSerial_plain();
      break;
    case 'C':
      copy();
      break;
    case 'N':
      constructMenu("EDIT");
      menuIsActive = true;
      break;
    case 'I':
      Serial.println(decimalToNumeral(11));
      break;
    case 'c':
      break;
    case '`':
      updateLookupData();
      displaySeqSerial();
      break;
    case 'u':
      track = getTrackWithPitch(24);
      Serial.println(track);
      break;
    case '.'://>
      moveCursor(subDivInt);
      displaySeqSerial();
      break;
    case 'f':
      debugPrintLookup();
      break;
    case ','://<
      moveCursor(-subDivInt);
      displaySeqSerial();
      break;
    case '>'://precise movement
      moveCursor(1);
       displaySeqSerial();
      break;
    case '<':
      moveCursor(-1);
        displaySeqSerial();
      break;
    case 'n':
      Serial.print("writing a note...\n");
      makeNote(activeTrack, cursorPos, subDivInt, 1); //default makes an 8th note
      displaySeqSerial();
      break;
    case ']':
      setActiveTrack(activeTrack - 1, 1);
        displaySeqSerial();
      break;
    case '[':
      setActiveTrack(activeTrack + 1, 1);
        displaySeqSerial();
      break;
    case 's'://select a note
        toggleSelectNote(activeTrack,getIDAtCursor(),true);
        displaySeqSerial();
      break;
    case 'p'://debug print selected notes
      // debugPrint();
        // displaySeqSerial();
      break;
    case 't': //add track (tricky! big moment if this works first time) LMAO
      addTrack(defaultPitch);
      defaultPitch++;
        displaySeqSerial();
      break;
    case 'd'://deleting the selected note(s)
      Serial.print("attempting to delete ");
      if (selectionCount > 0) {
        Serial.print("all selected notes...\n");
        deleteSelected();
      }
      else {
        Serial.print("1 note...\n");
        deleteNote();
      }
      displaySeqSerial();
      break;
    case 'e'://erase a track
      eraseTrack();
        displaySeqSerial();
      break;
    case 'E'://erases the sequence
      eraseSeq();
        displaySeqSerial();
      break;
    case 'm':
      menuIsActive = true;
      break;
    case '1':
      constructMenu("MENU");
      mainMenu();
      break;
    case '2'://counts notes
      constructMenu("ARP");
      break;
    case '3'://counts notes
      echoMenu();
      break;
    case '4'://counts notes
      constructMenu("CURVE");
      break;
    case 'a'://selects all in track
      break;
    case ';':
      loopData[activeLoop][1] -= 16;
        displaySeqSerial();
      break;
    case '\'':
      loopData[activeLoop][1] += 16;
    case 'r':
      toggleRecordingMode(waitForNote);
      break;
    case 'R':
        displaySeqSerial();
      break;
    case 'P':
      togglePlayMode();
      break;
    case '_':
        displaySeqSerial();
      break;
    case '+':
      zoom(true);
      break;
    case '-':
      zoom(false);
      break;
  }
}

bool justOneButton(){
  int score = n+sel+shift+del+loop_Press+play+copy_Press+menu_Press+track_Press+note_Press;
  if(score == 1)
    return true;
  else
    return false;
}

bool just(bool button){
  //if more than one button is pressed, return false
  if(!justOneButton)
    return false;
  //check every button
  else{
    if(n && &n == &button)
      return true;
    else if(sel && &sel == &button)
      return true;
    else if(shift && &shift == &button)
      return true;
    else if(del && &del == &button)
      return true;
    else if(loop_Press && &loop_Press == &button)
      return true;
    else if(menu_Press && &menu_Press == &button)
      return true;
    else if(play && &play == &button)
      return true;
    else if(copy_Press && &copy_Press == &button)
      return true;
    else if(track_Press && &track_Press == &button)
      return true;
    else if(note_Press && &note_Press == &button)
      return true;
    else
      return false;
  }
}

void rotaryActionA_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterA += readEncoder(0);
}

void rotaryActionB_Handler(){
  //this is bad programming! prob shouldn't have this in an interrupt
  counterB += readEncoder(1);
}

int readEncoder(bool encoder){
    int8_t r;
    int8_t l;
    
    static uint8_t lrmem = 3;
    static int lrsum = 0;
    const int8_t TRANS[] = {0, -1, 1, 14, 1, 0, 14, -1, -1, 14, 0, 1, 14, 1, -1, 0};

    int clk_Pin;
    int data_Pin;

    // Read BOTH pin states to deterimine validity of rotation (ie not just switch bounce)
    if(!encoder){
      l = digitalRead(track_clk_Pin);
      r = digitalRead(track_data_Pin);
    }
    else{
      l = digitalRead(note_clk_Pin);
      r = digitalRead(note_data_Pin);
    }
  
    // Move previous value 2 bits to the left and add in our new values
    lrmem = ((lrmem & 0x03) << 2) + 2 * l + r;

    // Convert the bit pattern to a movement indicator (14 = impossible, ie switch bounce)
    lrsum += TRANS[lrmem];

    /* encoder not in the neutral (detent) state */
    if (lrsum % 4 != 0)
    {
        return 0;
    }

    /* encoder in the neutral state - clockwise rotation*/
    if (lrsum == 4)
    {
        lrsum = 0;
        return -1;
    }

    /* encoder in the neutral state - anti-clockwise rotation*/
    if (lrsum == -4)
    {
        lrsum = 0;
        return 1;
    }

    // An impossible rotation has been detected - ignore the movement
    lrsum = 0;
    return 0;
}

//moves the whole loop
void moveLoop(int16_t amount){
  uint16_t length = loopData[activeLoop][1]-loopData[activeLoop][0];
  //if it's being moved back
  if(amount<0){
    //if amount is larger than start, meaning start would be moved before 0
    if(loopData[activeLoop][0]<=amount)
      setLoopPoint(0,true);
    else
      setLoopPoint(loopData[activeLoop][0]+amount,true);
    setLoopPoint(loopData[activeLoop][0]+length,false);
  }
  //if it's being moved forward
  else{
    //if amount is larger than the gap between seqend and loopData[activeLoop][1]
    if((seqEnd-loopData[activeLoop][1])<=amount)
      setLoopPoint(seqEnd,false);
    else
      setLoopPoint(loopData[activeLoop][1]+amount,false);
    setLoopPoint(loopData[activeLoop][1] - length,true);
  }
}
void toggleLoopMove(){
  switch(movingLoop){
    case 0:
      movingLoop = 1;
      moveCursor(loopData[activeLoop][0]-cursorPos);
      break;
    case -1:
      movingLoop = 1;
      moveCursor(loopData[activeLoop][1]-cursorPos);
      break;
    case 1:
      movingLoop = 2;
      break;
    case 2:
      movingLoop = 0;
      break;
  }
}

void yControls(){
  if(itsbeen(100)){
    if (y == 1) {
      setActiveTrack(activeTrack + 1, false);
      drawingNote = false;
      lastTime = millis();
    }
    if (y == -1) {
      setActiveTrack(activeTrack - 1, false);
      drawingNote = false;
      lastTime = millis();
    }
  }
}

void joyCommands(){
  if(!menuIsActive){
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
          //moving entire loop
          if(movingLoop == 2)
            moveLoop(-cursorPos%subDivInt);
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
          //moving entire loop
          if(movingLoop == 2)
            moveLoop(-subDivInt);
        }
        //moving loop start/end
        if(movingLoop == -1){
          setLoopPoint(cursorPos,true);
        }
        else if(movingLoop == 1){
          setLoopPoint(cursorPos,false);
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
          if(movingLoop == 2)
            moveLoop(subDivInt-cursorPos%subDivInt);
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
          if(movingLoop == 2)
            moveLoop(subDivInt);
        }
        //moving loop start/end
        if(movingLoop == -1){
          setLoopPoint(cursorPos,true);
        }
        else if(movingLoop == 1){
          setLoopPoint(cursorPos,false);
        }
      }
    }
    if(itsbeen(100)){
      if (y == 1 && !shift && !loop_Press) {
        if(recording)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, true);
        drawingNote = false;
        lastTime = millis();
      }
      if (y == -1 && !shift && !loop_Press) {
        if(recording)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
        drawingNote = false;
        lastTime = millis();
      }
    }
    if (itsbeen(50)) {
      //moving
      if (x == 1 && shift) {
        moveCursor(-1);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(-1);
        else if(movingLoop == -1)
          setLoopPoint(cursorPos,true);
        else if(movingLoop == 1)
          setLoopPoint(cursorPos,false);
      }
      if (x == -1 && shift) {
        moveCursor(1);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(1);
        else if(movingLoop == -1)
          loopData[activeLoop][0] = cursorPos;
        else if(movingLoop == 1)
          loopData[activeLoop][1] = cursorPos;
      }
      //changing vel
      if (y == 1 && shift) {
        changeVel(-10);
        lastTime = millis();
      }
      if (y == -1 && shift) {
        changeVel(10);
        lastTime = millis();
      }

      if(lookupData[activeTrack][cursorPos]==0){
        if(y == 1 && shift){
          defaultVel-=10;
          if(defaultVel<1)
            defaultVel = 1;
          lastTime = millis();
        }
        if(y == -1 && shift){
          defaultVel+=10;
          if(defaultVel>127)
            defaultVel = 127;
          lastTime = millis();
        }
      }
    }
  }
}

void printByte(uint8_t b){
  Serial.print("\n");
  uint8_t i = 0;
  while(i<8){
    Serial.print(b&1);
    b = b>>1;
    i++;
  }
}
//array to hold the LED states
//displays notes on LEDs
void updateLEDs(){
  uint16_t viewLength = viewEnd-viewStart;
  //move through the view, check every subDivInt
  uint8_t dat = 0;//00000000
  const uint16_t jump = viewLength/8;
  if(LEDsOn){
    if(seqData[activeTrack].size()>1){
      for(uint8_t i = 0; i<8; i++){
        uint16_t step = viewStart+i*jump;
        if(lookupData[activeTrack][step] != 0){
          if(seqData[activeTrack][lookupData[activeTrack][step]].startPos == step){
            //if playing or recording, and the head is on that step, then it should be off (so it blinks)
            // if((playing && (playheadPos != step)) || (recording && (recheadPos != step)) || (!playing && !recording))
              dat = dat|(1<<i);
          }
        }
      }
    }
  }

  //reversing dat (since I reversed my shift register for cleanliness)
  // dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;


  //https://dronebotworkshop.com/shift-registers/

  // sending data to shift reg
  digitalWrite(latchPin_LEDS, LOW);
  shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  digitalWrite(latchPin_LEDS, HIGH);

  printByte(dat);

}

void writeLEDs(uint8_t led, bool state){
  if(LEDsOn){
    bool leds[8];
    leds[led] = state;
    writeLEDs(leds);
  }
}
//this one turns on a range of LEDS
void writeLEDs(uint8_t first, uint8_t last){
  uint8_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<8; i++){
      dat = dat<<1;
      if(i >= first && i <= last){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  // sending data to shift reg
  digitalWrite(latchPin_LEDS, LOW);
  shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  digitalWrite(latchPin_LEDS, HIGH);
}
void writeLEDs(bool leds[8]){
  uint8_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<8; i++){
      dat = dat<<1;
      if(leds[i]){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  // sending data to shift reg
  digitalWrite(latchPin_LEDS, LOW);
  shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  digitalWrite(latchPin_LEDS, HIGH);
}

void turnOffLEDs(){
  uint8_t dat = 0;
  // sending data to shift reg
  digitalWrite(latchPin_LEDS, LOW);
  shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  digitalWrite(latchPin_LEDS, HIGH);
}

void mainSequencerButtons(){
  //selectionBox
  //when sel is pressed and stick is moved, and there's no selection box
  if(sel && !selBox.begun && (x != 0 || y != 0)){
    selBox.begun = true;
    selBox.x1 = cursorPos;
    selBox.y1 = activeTrack;
  }
  //if sel is released, and there's a selection box
  if(!sel && selBox.begun){
    selBox.x2 = cursorPos;
    selBox.y2 = activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!n)
    drawingNote = false;
  
  //delete happens a liitle faster (so you can draw/erase fast)
  if(itsbeen(75)){
    //delete
    if(del && !shift){
      if (selectionCount > 0){
        deleteSelected();
        lastTime = millis();
      }
      else if(lookupData[activeTrack][cursorPos] != 0){
        deleteNote(activeTrack,cursorPos);
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    //new
    if(n && !track_Press && !drawingNote && !sel){
      if((!shift)&&(lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos)){
        makeNote(activeTrack,cursorPos,subDivInt,true);
        drawingNote = true;
        lastTime = millis();
        moveCursor(subDivInt);
        // debugPrintLookup();
      }
      if(shift){
        addTrack(defaultPitch, defaultChannel);
        lastTime = millis();
      }
    }
    //select
    if(sel && !selBox.begun){
      uint16_t id = getIDAtCursor();
      if(id == 0){
        clearSelection();
      }
      else{
        //select all
        if(n){
          selectAll();
        }
        //select only one
        else if(shift){
          clearSelection();
          toggleSelectNote(activeTrack,id, false);
        }
        //normal select
        else{
          toggleSelectNote(activeTrack, id, true);          
        }
      }
      lastTime = millis();
    }
    if(del && shift){
      muteNote(activeTrack, getIDAtCursor(), true);
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(play && !shift && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+shift, or if play and it's already recording
    if(play && shift || play && recording){
      toggleRecordingMode(waitForNote);
      lastTime = millis();
    }
 
    //loop
    if(loop_Press){
      if(shift){
        lastTime = millis();
        loopMenu();
      }
      else{
        //if you're not moving a loop, start
        if(movingLoop == 0){
          //if you're on the start, move the start
          if(cursorPos == loopData[activeLoop][0]){
            movingLoop = -1;
          }
          //if you're on the end
          else if(cursorPos == loopData[activeLoop][1]){
            movingLoop = 1;
          }
          //if you're not on either, move the whole loop
          else{
            movingLoop = 2;
          }
          lastTime = millis();
        }
        //if you were moving, stop
        else{
          movingLoop = 0;
          lastTime = millis();
        }
      }
    }
    //menu press
    if(menu_Press){
      if(shift){
        debugPrintLookup();
        lastTime = millis();
        displayHelpText(0);
        return;
      }
      else{
        lastTime = millis();
        menuIsActive = true;
        constructMenu("MENU");
        mainMenu();
        return;
      }
    }
    if(track_Press && !del && !n && !sel && !shift){
      menuIsActive = true;
      lastTime = millis();
      track_Press = false;
      constructMenu("TRK");
    }
    if(note_Press){
      lastTime = millis();
      note_Press = false;
      menuIsActive = true;
      constructMenu("EDIT");
    }
    //copy/pasate
    if(copy_Press){
      if(shift)
        paste();
      else{
        copy();
      }
      lastTime = millis();
    }
  }
}

void serialButtonPrint(){
  // if(n)
  //   Serial.println("n");
  // if(sel)
  //   Serial.println("sel");
  // if(shift)
  //   Serial.println("shift");
  // if(del)
  //   Serial.println("del");
  // if(loop_Press)
  //   Serial.println("loop");
  // if(play)
  //   Serial.println("play");
  // if(copy_Press)
  //   Serial.println("copy");
  // if(menu_Press)
  //   Serial.println("menu");

  // Serial.println("+-+");
  // Serial.println("|"+(n?stringify("X"):stringify("O"))+"| new");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(shift?"X":"O")+"| shift");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(sel?"X":"O")+"| select");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(del?"X":"O")+"| delete");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(loop_Press?"X":"O")+"| loop");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(play?"X":"O")+"| play");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(copy_Press?"X":"O")+"| copy");
  // Serial.println("+-+");

  // Serial.println("+-+");
  // Serial.println("|"+(menu_Press?"X":"O")+"| menu");
  // Serial.println("+-+");

  for(int i = 0; i<8; i++){
    if(step_buttons[i]){
      Serial.println("step "+stringify(i));
    }
  }

  if(x != 0){
    Serial.println("x:"+stringify(x));
  }
  if(y != 0){
    Serial.println("y:"+stringify(y));
  }
}

//sets all the buttons to 0 (useful for making the animations transition nicely)
void clearButtons(){
  for(int i = 0; i < 8; i++){
    step_buttons[i] = false;
  }
  counterA = 0;
  counterB = 0;
  x = false;
  y = false;
  joy_Press = false;
  n = false;
  sel = false;
  shift = false;
  del = false;
  play = false;
  track_Press = false;
  note_Press = false;
  loop_Press = false;
  copy_Press = false;
  menu_Press = false;
}

//checks all inputs
bool anyActiveInputs(){
  //normal buttons
  digitalWrite(buttons_load,LOW);
  digitalWrite(buttons_load,HIGH);
  digitalWrite(buttons_clockIn, HIGH);
  digitalWrite(buttons_clockEnable,LOW);
  unsigned char bits_buttons = shiftIn(buttons_dataIn, buttons_clockIn, LSBFIRST);
  digitalWrite(buttons_clockEnable, HIGH);

  for(int digit = 0; digit<8; digit++){
    if(!((bits_buttons>>digit)&1))//if any of these are high, return true
      return true;
  }

  //stepButtons
  if(stepButtonsAreActive){
    digitalWrite(buttons_load,LOW);
    digitalWrite(buttons_load,HIGH);
    digitalWrite(buttons_clockIn, HIGH);
    digitalWrite(buttons_clockEnable,LOW);
    unsigned char bits_stepButtons = shiftIn(stepButtons_dataIn, buttons_clockIn, LSBFIRST);
    digitalWrite(buttons_clockEnable, HIGH);
  
    for(int digit = 0; digit<8; digit++){
      if(!((bits_stepButtons>>digit)&1))
        return true;
    }
  }

  joyRead();
  if(x || y)
    return true;
  //encoder presses
  if(digitalRead(note_press_Pin) || digitalRead(track_press_Pin)){
    return true;
  }
  //encoder clicks
  if(counterA || counterB){
    counterA = 0;
    counterB = 0;
    return true;
  }

  return false;
}

bool isBeingPlayed(uint8_t pitch){
  if(playlist.size()>0){
    for(int i = 0; i<playlist.size(); i++){
      if(playlist[i][0] == pitch)
        return true;
    }
  }
  return 0;
}

void stepButtons(){
  if(itsbeen(200)){
    //special functions
    if(shift){
      if(step_buttons[0]){
        step_buttons[0] = 0;
        toggleKeys();
        lastTime = millis();
      }
      if(step_buttons[1]){
        step_buttons[1] = 0;
        toggleDrumPads();
        lastTime = millis();
      }
      if(step_buttons[2]){
        step_buttons[2] = 0;
        lastTime = millis();
      }
      if(step_buttons[3]){
        step_buttons[3] = 0;
        constructMenu("FRAGMENT");
        menuIsActive = true;
        lastTime = millis();
      }
    }
    else{
      if(!keys && !drumPads){
        bool atLeastOne = false;
        //if it's in 1/4 mode
        if(!(subDivInt%3)){
          for(int i = 0; i<8; i++){
            if(step_buttons[i]){
              uint16_t viewLength = viewEnd-viewStart;
              toggleNote(activeTrack, viewStart+i*viewLength/8, viewLength/8);
              atLeastOne = true;
            }
          }
        }
        //if it's in 1/3 mode, last two buttons do nothing
        else if(!(subDivInt%2)){
          for(int i = 0; i<6; i++){
            if(step_buttons[i]){
              uint16_t viewLength = viewEnd-viewStart;
              toggleNote(activeTrack,viewStart+i*viewLength/6,viewLength/6);
              atLeastOne = true;
            }
          }
        }
        if(atLeastOne){
          lastTime = millis();
        }
      }
    }
  }
}

void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}

void defaultEncoderControls(){
  while(counterA != 0){
    //changing zoom
    if(counterA >= 1){
      zoom(true);
    }
    if(counterA <= -1){
      zoom(false);
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(shift){
      toggleTriplets();
    }
    else if(counterB >= 1){
      changeSubDivInt(true);
    }
    //changing subdivint
    else if(counterB <= -1){
      changeSubDivInt(false);
    }
    counterB += counterB<0?1:-1;;
  }
}

void mainSequencerEncoders(){
  while(counterA != 0){
    //changing pitch
    if(shift){
      if(counterA >= 1){
        setTrackPitch(activeTrack,trackData[activeTrack].pitch+1,true);
      }
      if(counterA <= -1){
        setTrackPitch(activeTrack,trackData[activeTrack].pitch-1,true);
      }
    }
    //changing zoom
    else{
      if(counterA >= 1 && !track_Press){
        zoom(true);
      }
      if(counterA <= -1 && !track_Press){
        zoom(false);
      }
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    if(counterB >= 1 && !shift){
      changeSubDivInt(true,true);
    }
    //changing subdivint
    if(counterB <= -1 && !shift){
      changeSubDivInt(false,true);
    }
    //if shifting, toggle between 1/3 and 1/4 mode
    else while(counterB != 0 && shift){
      toggleTriplets();
    }
    counterB += counterB<0?1:-1;;
  }
}

bool menuScrolling(){
  if(menuIsActive && itsbeen(100)){
    bool didIt = false;
    if(y == 1){
      didIt = activeMenu.moveMenuCursor(true);
      lastTime = millis();
    }
    if(y == -1){
      didIt = activeMenu.moveMenuCursor(false);
      lastTime = millis();
    }
    return didIt;
  }
  return false;
}

String enterText_serial(String title){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  const vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","enter"};
  const vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","enter"};
  alphabet = alpha1;
  while(!done){
    readButtons();
    joyRead();
    text+=Serial.readString();
    if(itsbeen(200)){
      if(shift){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }
      if(del && text.length()>0){
        del = false;
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(sel){
        //adding character to text
        if(alphabet[highlight] != "enter" && text.length()<10){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "enter")
          done = true;
        lastTime = millis();
      }
    }
    if(itsbeen(100)){
      if(x == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(x == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(y == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(y == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    display.clearDisplay();

    //title
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();
    
    //text tooltip
    printSmall(96,6,"[SH] for caps",SSD1306_WHITE);
    printSmall(96,14,"[MENU] to exit",SSD1306_WHITE);
    
    //input text
    display.setCursor(10,12);
    display.print(text);
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(alphabet[count]);
            display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }
    display.display();
  }
  if(text == ""){
    text = "seq";
  }
  return text;
}

String enterText(String title){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  alphabet = alpha1;
  while(!done){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(shift){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }

      if(del && text.length()>0){
        del = false;
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(sel){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<10){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        lastTime = millis();
      }
      if(menu_Press){
        text = "";
        done = true;
        lastTime = millis();
      }
    }
    if(itsbeen(100)){
      if(x == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(x == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(y == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(y == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    //title
    display.clearDisplay();
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();

    //text tooltip
    printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      display.print(text+alphabet[highlight]);
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,15,10,SSD1306_WHITE);
      }
    }
    else
      display.print(text);
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // display.print(alphabet[count]);
              // display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              display.print(alphabet[count]);
              display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    display.display();
  }
  if(text == ""){
    text = "default";
  }
  return text;
}

bool fxMenuControls(){
  if(itsbeen(100)){
    if(x != 0){
      if(x == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((activeMenu.highlight+1)%4)
          activeMenu.highlight++;
        lastTime = millis();
      }
      else if(x == 1){
        if((activeMenu.highlight)%4)
          activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if(y != 0){
      if(y == 1){
        //first row
        if(activeMenu.highlight < 20)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(y == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
      activeMenu.page = activeMenu.highlight/12;
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      return false;
    }
    if(sel){
      sel = false;
      lastTime = millis();
      switch(activeMenu.highlight){
        //chord
        case 0:
          chordBuilder();
          break;
        //echo
        case 1:
          echoMenu();
          break;
        //humanize
        case 2:
          humanizeMenu();
          break;
        //mayhem
        case 3:
          // slideMenuOut(0,20);
          warp();
          // slideMenuIn(0,30);
          break;
        //quant
        case 4:
          quantizeMenu();
          break;
        //reverse
        case 5:
          break;
        //random
        case 6:
          randMenu();
          break;
        //scramble
        case 7:
          break;
        //split
        case 8:
          break;
        //strum
        case 9:
          break;
      }
    }
  }
  return true;
}

bool mainMenuControls(){
  if(itsbeen(100)){
    if(x != 0){
      if(x == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((activeMenu.highlight+1)%4)
          activeMenu.highlight++;
        lastTime = millis();
      }
      else if(x == 1){
        if((activeMenu.highlight)%4)
          activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if(y != 0){
      if(y == 1){
        //first row
        if(activeMenu.highlight < 8)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(y == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      if(shift){
        slideMenuOut(0,20);
        lastTime = millis();
        constructMenu("DEBUG");
      }
      else{
        lastTime = millis();
        slideMenuOut(0,20);
        menuIsActive = false;
        return false;
      }
    }
    if(loop_Press){
      lastTime = millis();
      loopMenu();
    }
    if(n){
      lastTime = millis();
      slideMenuOut(0,20);
      constructMenu("FX");
      fxMenu();
    }
    if(sel){
      sel = false;
      lastTime = millis();
      switch(activeMenu.highlight){
        //datatracks
        case 0:
          dataTrackViewer();
          break;
        //loop
        case 1:
          loopMenu();
          break;
        //keys
        case 2:
          // toggleKeys();
          selectInstrumentMenu();
          break;
        //settings
        case 3:
          // slideMenuOut(0,20);
          // constructMenu("SYS");  
          constructMenu("SETTINGS");  
          break;
        //quicksave
        case 4:
          //if you're shifting, load most recent backup
          if(shift){
            loadBackup();
          }
          else{
            quickSave();
          }
          break;
        //fx
        case 5:
          slideMenuOut(0,20);
          constructMenu("FX");
          fxMenu();
          break;
        //rec
        case 6:
          // slideMenuOut(0,20);
          playBackMenu();
          // slideMenuIn(0,30);
          break;
        //console
        case 7:
          console();
          break;
        //midi
        case 8:
          // menuIsActive = false;
          // slideMenuOut(0,20);
          midiMenu();
          break;
        //files
        case 9:
          // slideMenuOut(0,20);
          constructMenu("FILES");
          break;
        //clock
        case 10:
          slideMenuOut(0,20);
          constructMenu("CLOCK");
          break;
        //arp
        case 11:
          // slideMenuOut(0,20);
          arpMenu();
          keyboardAnimation(38,14,0,14,false);
          break;
      }
    }
  }
  return true;
}

void debugMenuControls(){
  menuScrolling();
  if(loop_Press){
    lastTime = millis();
    constructMenu("LOOP");
    activeMenu.displayMenu(true,true);
  }
  if(sel && itsbeen(100)){
    sel = false;
    lastTime = millis();
    if(activeMenu.options[activeMenu.highlight] == "random"){
      menuIsActive = false;
      randMenu();
    }
    else if(activeMenu.options[activeMenu.highlight] == "x/y"){
      menuIsActive = false;
      // gridAnimation(true);
      xyGrid();
    }
    else if(activeMenu.options[activeMenu.highlight] == "write to serial"){
      writeCurrentSeqToSerial(false);
    }
    else if(activeMenu.options[activeMenu.highlight] == "debugFill"){
      debugFillSequence();
    }
    else if(activeMenu.options[activeMenu.highlight] == "dataTracks"){
      menuIsActive = false;
      dataTrackViewer();
    }
    else if(activeMenu.options[activeMenu.highlight] == "MIDI"){
      menuIsActive = false;
      // fadeScreen(true,5);
      // midiPortAnimation(true);
      midiMenu();
    }
    else if(activeMenu.options[activeMenu.highlight] == "loop"){
      constructMenu("LOOP");
      activeMenu.displayMenu(true,true);
    }
    else if(activeMenu.options[activeMenu.highlight] == "tap"){
      tapBpm();
    }
    else if(activeMenu.options[activeMenu.highlight] == "fragment"){
      constructMenu("FRAGMENT");
    }
    else if(activeMenu.options[activeMenu.highlight] == "humanize"){
      constructMenu("HUMANIZE");
    }
    else if(activeMenu.options[activeMenu.highlight] == "quantize"){
      quantizeMenu();
    }
    else if(activeMenu.options[activeMenu.highlight] == "clock"){
      constructMenu("CLOCK");
    }
    else if(activeMenu.options[activeMenu.highlight] == "Sys"){
      slideMenuOut(0,20);
      constructMenu("SYS");    
    }
    else if(activeMenu.options[activeMenu.highlight] == "rec"){
      constructMenu("REC");
    }
    else if(activeMenu.options[activeMenu.highlight] == "echo"){
      echoMenu_menuless();
    }
    else if(activeMenu.options[activeMenu.highlight] == "drumPads"){
      toggleDrumPads();
      menuIsActive = false;
    }
    else if(activeMenu.options[activeMenu.highlight] == "keys"){
      toggleKeys();
      menuIsActive = false;
    }
    else if(activeMenu.options[activeMenu.highlight] == "arp"){
      constructMenu("ARP");
    }
    else if(activeMenu.options[activeMenu.highlight] == "fx"){
      slideMenuOut(0,20);
      constructMenu("FX");
      fxMenu();
    }
    else if(activeMenu.options[activeMenu.highlight] == "r_serial"){
      restartSerial(9600);
      menuIsActive = !menuIsActive;
    }
    else if(activeMenu.options[activeMenu.highlight] == "r_seq"){
      initSeq();
      menuIsActive = !menuIsActive;
    }
    else if(activeMenu.options[activeMenu.highlight] == "files"){
      constructMenu("FILES");
    }
    else if(activeMenu.options[activeMenu.highlight] == "save"){
      constructMenu("SAVE");
    }
    else if(activeMenu.options[activeMenu.highlight] == "sequence"){
      constructMenu("SEQ");
    }
    else if(activeMenu.options[activeMenu.highlight] == "HARD reset"){
      hardReset();
    }
    else if(activeMenu.options[activeMenu.highlight] == "change display"){
      displayingVel = !displayingVel;
      menuIsActive = !menuIsActive;
    }
    else if(activeMenu.options[activeMenu.highlight] == "Moon"){
      moon();
    }
  }
  if(menu_Press && itsbeen(200)){
    lastTime = millis();
    slideMenuOut(0,20);
    menuIsActive = false;
  }
}

void seqMenuControls(){
  menuScrolling();
  while(counterA != 0){
    if(counterA >= 1){
      if(activeMenu.options[activeMenu.highlight] == "bpm")
        setBpm(bpm+1);
      else if(activeMenu.options[activeMenu.highlight] == "Clock")
        externalClock = false;
      else if(activeMenu.options[activeMenu.highlight] == "Length"){
        setSeqEnd(seqEnd+96);
      }
    }
    else if(counterA <= -1){
      if(activeMenu.options[activeMenu.highlight] == "bpm")
        setBpm(bpm-1);
      else if(activeMenu.options[activeMenu.highlight] == "Clock")
        externalClock = true;
      else if(activeMenu.options[activeMenu.highlight] == "Length" && seqEnd>96){
        setSeqEnd(seqEnd-96);
      }
    }
    counterA += counterA<0?1:-1;;
  }
  if(itsbeen(200)){
    if(menu_Press){
      menu_Press = false;
      lastTime = millis();
      constructMenu("MENU");
    }
    if(sel){
      sel = false;
      if(activeMenu.options[activeMenu.highlight] == "Loop"){
        lastTime = millis();
        constructMenu("LOOP");
      }
    }
  }
}
void drawOSScreen(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  clearButtons();
  lastTime = millis();
  while(true){
    if(itsbeen(200)&&anyActiveInputs()){
      clearButtons();
      lastTime = millis();
      return;
    }
  }
}
void systemMenuControls(){
  menuScrolling();
  if(itsbeen(200)){
    if(sel){
      sel = false;
      if(activeMenu.options[activeMenu.highlight] == "help"){
        lastTime = millis();
        drawWebLink();
      }
      else if(activeMenu.options[activeMenu.highlight] == "sleep"){
        menuIsActive = false;
        constructMenu("MENU");
        lastTime = millis();
        enterSleepMode();
      }
      else if(activeMenu.options[activeMenu.highlight] == "reset"){
        hardReset();
      }
      else if(activeMenu.options[activeMenu.highlight] == "OS"){
        drawOSScreen();
      }
      else if(activeMenu.options[activeMenu.highlight] == "update"){
        enterBootsel();
      }
    }
    if(menu_Press){
      constructMenu("MENU");
      lastTime = millis();
    }
  }
}

bool echoMenuControls(){
  if(itsbeen(200)){
    if(x == 1 && activeMenu.highlight > 0){
      activeMenu.highlight--;
      lastTime = millis();
    }
    if(x == -1 && activeMenu.highlight < 2){
      activeMenu.highlight++;
      lastTime = millis();
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(n){
      lastTime = millis();
      while(true){
        //echo selected notes
        if(selectNotes("echo",drawEchoIcon)){
          echoSelectedNotes();
          clearSelection();
        }
        //cancel
        else{
          break;
        }
      }
    }
  }
  while(counterA != 0){//if there's data for this option
    if(counterA >= 1){
      if(activeMenu.highlight == 0 && echoData[0]<96){
        if(shift)
          echoData[0]++;
        else
          echoData[0]*=2;
        if(echoData[0]>96)
          echoData[0] = 96;
      }
      else if(activeMenu.highlight == 1)
        echoData[2]++;
      else if(activeMenu.highlight == 2){
        if(shift && echoData[1]<100)
          echoData[1]++;
        else if(echoData[1]<=90)
          echoData[1]+=10;
      }
    }
    else if(counterA <= -1){
      if(activeMenu.highlight == 0){
        if(shift && echoData[0]>0)
          echoData[0]--;
        else if(echoData[0]>=2)
          echoData[0]/=2;
      }
      else if(activeMenu.highlight == 1 &&  echoData[2] > 1)
        echoData[2]--;
      else if(activeMenu.highlight == 2){
        if(shift && echoData[1]>2)
          echoData[1]--;
        else if(echoData[1]>=11)
          echoData[1]-=10;
      }
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    if(counterB <= -1 && activeMenu.highlight>0){
      activeMenu.highlight--;
    }
    else if(counterB >= 1 && activeMenu.highlight<2){
      activeMenu.highlight++;
    }
    counterB += counterB<0?1:-1;;
  }
  return true;
}

void arpMenuControls(){
  menuScrolling();
  while(counterA != 0){
    if(activeMenu.highlight == 0){
      isArping = !isArping;
      counterA += counterA<0?1:-1;;
    }
    else if(activeMenu.highlight == 1){
      if(counterA >= 1){
        if(activeMenu.options[activeMenu.highlight] == "subDiv"){
          activeArp.arpSubDiv++;
        }
      }
      else if(counterA <= -1){
        if(activeMenu.options[activeMenu.highlight] == "subDiv"){
          activeArp.arpSubDiv--;
        }
      }
      for(int i = 0; i<activeArp.lengths.size(); i++){
        activeArp.lengths[i] = activeArp.arpSubDiv;
      }
      counterA += counterA<0?1:-1;;
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      menuIsActive = false;
      constructMenu("MENU");
    }
    if(sel){
      if(activeMenu.options[activeMenu.highlight] == "notes"){
        activeArp.selectExtended();
      }
      else if(activeMenu.options[activeMenu.highlight] == "subDiv")
        customLengthsMenu();
      lastTime = millis();
    }
    while(counterA != 0){
      if(counterA >= 1){
        if(activeMenu.options[activeMenu.highlight] == "isArping"){
          isArping = !isArping;
        }
        else if(activeMenu.options[activeMenu.highlight] == "subDiv"){
          activeArp.arpSubDiv++;
        }
      }
      else if(counterA <= -1){
        if(activeMenu.options[activeMenu.highlight] == "isArping"){
          isArping = !isArping;
        }
        else if(activeMenu.options[activeMenu.highlight] == "subDiv"){
          activeArp.arpSubDiv--;
        }
      }
      counterA += counterA<0?1:-1;;
    }
  }
}
void curveMenuControls(){
  menuScrolling();
  if(activeMenu.highlight < activeMenu.data.size()){//if there's data for this option
    if(counterA >= 1){
      if(!shift && (activeMenu.options[activeMenu.highlight] != "fn" || (activeMenu.options[activeMenu.highlight] == "fn" && (*activeMenu.data[activeMenu.highlight]) < 5))){
        (*activeMenu.data[activeMenu.highlight])++;
      }
      else if(shift && activeMenu.options[activeMenu.highlight] != "fn"){
        (*activeMenu.data[activeMenu.highlight])+=10;
      }
      counterA += counterA<0?1:-1;;
      genCurveDebug();
    }
    if(counterA <= -1 && (*activeMenu.data[activeMenu.highlight])>0){
      if(shift && activeMenu.options[activeMenu.highlight] != "fn" && (*activeMenu.data[activeMenu.highlight])>=10){
        (*activeMenu.data[activeMenu.highlight])-=10;

      }
      else
        (*activeMenu.data[activeMenu.highlight])--;
      counterA += counterA<0?1:-1;;
      genCurveDebug();
    }
  }
  if(sel){
    sel = false;
    if(activeMenu.options[activeMenu.highlight] == "commit"){
      vector <String> ops = {"loop","seq","range"};
      unsigned short int choice1 = vertSelectionBox(ops,45,5,45,55);
      ops = {"vel","pos","chance"};
      unsigned short int choice = vertSelectionBox(ops, 45, 5, 45, 55);
      menuIsActive = false;
      constructMenu("MENU");
    }
  }
  if(activeMenu.options[activeMenu.highlight] == "ph")
    animOffset = 0; 
  else{
    animOffset++;
  }
}
//this should move the note the cursor is on (if any)
bool moveNotes(int xAmount, int yAmount){
  if(selectionCount == 0){
    if(lookupData[activeTrack][cursorPos] != 0){
      bool worked = moveNote(lookupData[activeTrack][cursorPos],activeTrack,activeTrack+yAmount,seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos+xAmount);
      return worked;
    }
    else
      return false;
  }
  else{
    bool worked = moveSelectedNotes(xAmount,yAmount);
    return worked;
  }
}

void noteMenuControls(){
  readButtons();
  joyRead();
  menuScrolling();
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      menuIsActive = false;
      constructMenu("MENU");
    }
    if(note_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      menuIsActive = false;
      constructMenu("MENU");
    }
    if(track_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      constructMenu("TRK");
    }
    if(sel){
      if(activeMenu.options[activeMenu.highlight] == "edit"){
        lastTime = millis();
        sel = false;
        slideMenuOut(0,20);
        constructMenu("EDIT");
      }
      if(activeMenu.options[activeMenu.highlight] == "div"){
        lastTime = millis();
        sel = false;
      }
    }
  }
}

void quickSave(){
  //if it hasn't been saved yet
  if(currentFile == ""){
    String fileName = enterText("filename?");
    if(fileName != "default"){
      writeSeqFile(fileName);
      currentFile = fileName;
      menuText = "saved \'"+currentFile+"\'";
    }
  }
  //if there is a filename for it
  else{
    writeSeqFile(currentFile);
    menuText = "saved \'"+currentFile+"\'";
  }
}

void loadBackup(){
  //if there's an active filename
  if(currentFile != ""){
    vector<String> ops = {"NAUR","YEA"};
    int8_t choice = binarySelectionBox(59,32,"NO","YEA","LOAD BACKUP?");
    if(choice){
      loadSeqFile(currentFile);
      slideMenuOut(1,30);
      menuIsActive = false;
      constructMenu("MENU");
    }
  }
  //if there isn't, just enter files menu
  else{
    slideMenuOut(0,20);
    constructMenu("FILES");
  }
}


void fileMenuControls_miniMenu(WireFrame* w){
  //scrolling
  if(itsbeen(100)){
    if(y == 1 && activeMenu.highlight<5){
      activeMenu.highlight++;
      lastTime = millis();
    }
    else if(y == -1 && activeMenu.highlight>0){
      activeMenu.highlight--;
      lastTime = millis();
    }
  }
  if(itsbeen(200)){
    //back to normal mode
    if(menu_Press){
      lastTime = millis();
      activeMenu.highlight = activeMenu.page;
      activeMenu.page = 0;
    }
    //selecting an option
    if(sel){
      lastTime = millis();
      sel = false;
      switch(activeMenu.highlight){
        //overwrite
        case 1:
        {
          String fileName = activeMenu.options[activeMenu.page];
          int8_t choice = binarySelectionBox(64,32,"NO","YEA");
          if(choice == 1){
            writeSeqFile(fileName);
          }
          break;
        }
        //load
        case 0:
        {
          String fileName = activeMenu.options[activeMenu.page];
          loadSeqFile(fileName);
          currentFile = fileName;
          menuIsActive = false;
          constructMenu("MENU");
          break;
        }
        //rename
        case 2:
        {
          String filename = activeMenu.options[activeMenu.page];
          renameSeqFile(filename);
          loadFiles();
          break;
        }
        //export
        case 3:
          exportSeqFileToSerial_standAlone(activeMenu.options[activeMenu.page]);
          break;
        //delete
        case 4:
        {
          String filename = activeMenu.options[activeMenu.page];
          if(deleteSeqFile(filename));
          // constructMenu("FILES");
          activeMenu.options = {"*New*"};
          loadFiles();
          activeMenu.highlight = activeMenu.page-1;
          activeMenu.page = 0;
          openFolderAnimation(w,30);
          break;
        }
        //back
        case 5:
          lastTime = millis();
          activeMenu.highlight = activeMenu.page;
          activeMenu.page = 0;
          openFolderAnimation(w,30);
          break;
      }
    }
  }
}

bool fileMenuControls(uint8_t menuStart, uint8_t menuEnd,WireFrame* w){
  menuScrolling();
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(1,30);
      return false;
    }
    if(n){
      if(shift){
        dumpFilesToSerial();
      }
      else{
      }
    }
    if(activeMenu.options.size()>0){
      if(sel){
        lastTime = millis();
        sel = false;
        if(activeMenu.options[activeMenu.highlight] == "*New*"){
          String fileName = enterText("filename?");
          if(fileName != "default"){
            writeSeqFile(fileName);
            slideMenuOut(1,30);
            menuIsActive = false;
            constructMenu("MENU");
          }
        }
        //entering the minimenu
        else{
          //set the 'page' variable to the current file index
          activeMenu.page = activeMenu.highlight;
          //reset highlight to 0
          activeMenu.highlight = 0;
          //open wireFrame
          openFolderAnimation(w,120);
          return true;
        }
      }
      if(del){
        lastTime = millis();
        del = false;
        String filename = activeMenu.options[activeMenu.highlight];
        deleteSeqFile(filename);
        // constructMenu("FILES");
        activeMenu.options = {};
        loadFiles();
        if(activeMenu.options.size() == 0)
          activeMenu.highlight = 0;
      }
      if(copy_Press){
        lastTime = millis();
        copy_Press = false;
        String filename = activeMenu.options[activeMenu.highlight];
        duplicateSeqFile(filename);
        activeMenu.options = {};
        loadFiles();
      }
    }
  }
  return true;
}

void saveMenuControls(){
  menuScrolling();
  if(itsbeen(200)){
    if(sel){
      sel = false;
      lastTime = millis();
      if(activeMenu.options[activeMenu.highlight] == "New"){
        String fileName = enterText("filename?");
        if(fileName != "New"){
          writeSeqFile(fileName);
          slideMenuOut(1,30);
          menuIsActive = false;
          constructMenu("MENU");
        }
        else{
          alert("Choose diff name, pls",800);
        }
      }
      else{
        String fileName = activeMenu.options[activeMenu.highlight];
        vector<String> ops = {"NO","YEAH"};
        int choice = horzSelectionBox("Overwrite?",ops,0,0,screenWidth,screenHeight);
        if(choice){
          writeSeqFile(fileName);
          slideMenuOut(1,30);
          menuIsActive = false;
          constructMenu("MENU");
        }
      }
    }
    if(n){
      n = false;
      lastTime = millis();
      String fileName = enterText("filename?");
      if(fileName != "New"){
        writeSeqFile(fileName);
        slideMenuOut(1,30);
        menuIsActive = false;
        constructMenu("MENU");
      }
      else{
        alert("Choose diff name, pls",800);
      }
    }
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(1,30);
      menuIsActive = false;
      constructMenu("MENU");
    }
  }
}

void inputRead() {
  joyRead();
  readButtons();
  if(menuIsActive){
    if(activeMenu.menuTitle == "DEBUG")
      debugMenuControls();
    else if(activeMenu.menuTitle == "MENU")
      mainMenuControls();
    else if(activeMenu.menuTitle == "CURVE")
      curveMenuControls();
    else if(activeMenu.menuTitle == "TRK")
      trackMenuControls();
    else if(activeMenu.menuTitle == "NOTE")
      noteMenuControls();
    else if(activeMenu.menuTitle == "SAVE")
      saveMenuControls();
    else if(activeMenu.menuTitle == "SEQ")
      seqMenuControls();
    else if(activeMenu.menuTitle == "SYS")
      systemMenuControls();
  }
  else{
    mainSequencerButtons();
    stepButtons();
    mainSequencerEncoders();
    joyCommands();
  }
}

void drawX(uint8_t x1, uint8_t y1, uint8_t width){
  uint8_t points[3][2] = {{uint8_t(x1-width/2+width/4),uint8_t(y1-width/2)},{x1,uint8_t(y1-width/4)},{uint8_t(x1+width/2-width/4),uint8_t(y1-width/2)}};
}

void drawStar(uint8_t centerX, uint8_t centerY, uint8_t r1, uint8_t r2, uint8_t points){
  uint8_t numberOfPoints = points*2;//the actual number of points (both convex and concave vertices)
  uint8_t coords[numberOfPoints][2];
  for(uint8_t pt = 0; pt<numberOfPoints; pt++){
    vector<uint8_t> pair;
    //if it's odd, it's a convex point
    if(!(pt%2))
      pair = getRadian(centerX, centerY, r1, r1, pt*360/numberOfPoints);
    else
      pair = getRadian(centerX, centerY, r2, r2, pt*360/numberOfPoints);
    coords[pt][0] = pair[0];
    coords[pt][1] = pair[1];
  }
  for(uint8_t pt = 0; pt<numberOfPoints; pt++){
    if(pt == numberOfPoints-1){
      display.drawLine(coords[pt][0],coords[pt][1],coords[0][0],coords[0][1],SSD1306_WHITE);
    }
    else{
      display.drawLine(coords[pt][0],coords[pt][1],coords[pt+1][0],coords[pt+1][1],SSD1306_WHITE);
    }
  }
}
void testItalicFont(){
  while(true){
    display.clearDisplay();
    for(uint8_t i = 0; i<26; i++){
      display.drawBitmap(i*8,0,italic_font[i],7,7,1,0);
    }
    display.display();
  }
}

void bootscreen(){
  display.clearDisplay();
  uint16_t frameCount = 0;
  display.setTextColor(SSD1306_WHITE);
  char child[5] = {'c','h','i','l','d'};
  int16_t xCoord;
  int16_t yCoord;

  uint8_t xOffset = 30;
  uint8_t yOffset = 15;
  WireFrame pram = makePram();
  pram.yPos = 40;
  pram.xPos = 64;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<60){
    display.clearDisplay();
    display.setTextSize(2);
    pram.render();
    //CHILD
    for(uint8_t letter = 0; letter<5; letter++){
      // xCoord = xOffset+letter*12;
      // xCoord = xOffset+letter*5;
      xCoord = 20+letter*8;
      yCoord = screenHeight-frameCount*10+letter*10;
      if(yCoord<0)
        yCoord = 0;
      yCoord+=yOffset;
      display.setCursor(xCoord,yCoord);
      // display.print(stringify(child[letter]));
      // printCursive(xCoord,yCoord,stringify(child[letter]),SSD1306_WHITE);
      printItalic(xCoord,yCoord,child[letter],1);
    }
    //OS
    if(frameCount>20){
      display.setTextSize(1);
      display.setFont(&FreeSerifItalic12pt7b);
      display.setCursor(xCoord+10,yCoord+5);
      display.print("OS");
      display.setFont();

      // drawStar(xOffset+68,yOffset-8+sin(millis()/100),3,7,5);
      drawStar(xOffset+68,yOffset-8,3,7,5);
      // drawStar(xOffset+76,yOffset-15,1,3,5);

      // printSmall(xOffset+35,yOffset+10,OSversionNumber,SSD1306_WHITE);
      // display.drawFastHLine(xOffset-1,yOffset+7,27,SSD1306_WHITE);
    }
    // if(frameCount>50)
      // printSmall(xOffset+35,yOffset+10,OSversionNumber,SSD1306_WHITE);
    display.display();
    pram.rotate(5,1);
    frameCount++;
  }
}

void helloChild_1(){
  display.clearDisplay();
  display.setCursor(50, 20);
  // display.setTextSize(3.5);
  display.setTextColor(SSD1306_WHITE);
  // display.setFont(&FreeSerifBoldItalic18pt7b);
  display.print("hello");
  display.display();
  delay(1000);
  display.setCursor(10, 42);
  display.setFont();
  display.print("child.");
  display.invertDisplay(true);
  display.display();
  delay(1000);
  display.invertDisplay(false);

  display.setTextColor(SSD1306_WHITE);
  display.setCursor(50, 20);
  // display.setFont(&FreeSerifBoldItalic18pt7b);
  display.print("hello");
  display.setCursor(10, 42);
  display.setFont();
  display.print("child.");

  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1500);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
  // for(int i = 0; i<20; i++){
  //   display.drawBitmap(16+i*10,i*10,carriage_bmp,14,15,SSD1306_BLACK);
  //   display.display();
  //   delay(30);
  // }
}
void helloChild_2(){
  display.clearDisplay();
  display.invertDisplay(true);
  display.display();
  delay(1500);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  display.invertDisplay(false);
  delay(1500);
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.fillRect(0,0,128,64,SSD1306_BLACK);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
}
void helloChild_3(){
  display.clearDisplay();
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
}
void helloChild_4(){
  display.clearDisplay();
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(500);
  for(float i = 0; i<20; i++){
    display.clearDisplay();
    display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(1000);
}
void helloChild_5(){
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1000);
  for(float i = 0; i<20; i++){
    display.clearDisplay();
    display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(500);
}



void setupPins(){
  #ifndef HEADLESS
  //joystick
  pinMode(x_Pin, INPUT);
  pinMode(y_Pin, INPUT);
  // pinMode(joy_press_Pin, INPUT_PULLUP);

  // pinMode(29, INPUT);
  pinMode(Vpin, INPUT);
  pinMode(onboard_ledPin, OUTPUT);
  digitalWrite(onboard_ledPin,HIGH);

  //encoders
  pinMode(track_press_Pin, INPUT_PULLUP);
  pinMode(note_press_Pin, INPUT_PULLUP);
  pinMode(track_clk_Pin, INPUT_PULLUP);
  pinMode(note_clk_Pin, INPUT_PULLUP);
  pinMode(track_data_Pin, INPUT_PULLUP);
  pinMode(note_data_Pin, INPUT_PULLUP);

  //button bit shift reg
  pinMode(buttons_load, OUTPUT);
  pinMode(buttons_clockEnable, OUTPUT);
  pinMode(buttons_clockIn, OUTPUT);
  pinMode(buttons_dataIn, INPUT);

  //LED bit shift reg
  pinMode(latchPin_LEDS, OUTPUT);
  pinMode(clockPin_LEDS, OUTPUT);
  pinMode(dataPin_LEDS, OUTPUT);

  // CPU0 handles the encoder interrupts now
  //setting up interrupts (hopefully this helps the encoders catch more steps)
  attachInterrupt(track_clk_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(track_data_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(note_clk_Pin,rotaryActionB_Handler, CHANGE);
  attachInterrupt(note_data_Pin,rotaryActionB_Handler, CHANGE);

  pinMode(ledPin, OUTPUT);
  #endif
}
//startsMIDI and Serial
void startSerial(){
  startMIDI();
}
void restartDisplay(){
  #ifndef HEADLESS
  Wire.end();
  // Wire.begin();
  display.begin(i2c_Address,true);
  display.display();
  #endif
}

//this should restart USB serial/midi if it's become disconnected
void checkSerial(){
  #ifndef HEADLESS
  if(digitalRead(usbPin) && !TinyUSBDevice.mounted()){
  }
  #endif
}
#ifdef HEADLESS
  void setup() {

    // display.begin(i2c_Address,true);
    display.display();
    display.setRotation(UPRIGHT);

    //seeding random number generator
    srandom(1);

    initSeq(16,768);
    updateLEDs();

    if(helloChild){
      bootscreen();
      // helloChild_5();
    }
    
    setNormalMode();
    turnOffLEDs();
    core0ready = true;
    lastTime = millis();
    displaySeq();
  }
#else
void setup() {
  Serial1.setRX(rxPin);
  Serial1.setTX(txPin_1);
  Serial2.setTX(txPin_2);
  Wire.setSDA(8);

  startMIDI();
  
  //starting serial monitor output
  Serial.begin(9600);
  // if(!display.begin(SSD1306_EXTERNALVCC, i2c_Address)) {
  if(!display.begin(SSD1306_SWITCHCAPVCC, i2c_Address)) {
    for(;;); // Don't proceed, loop forever
  }

  // display.display();
  display.setRotation(UPRIGHT);
  display.setTextWrap(false);

  // these two string must be exactly 32 chars long
  //                                   01234567890123456789012345678912
  USBDevice.setManufacturerDescriptor("Silent Instruments Inc.         ");
  USBDevice.setProductDescriptor     ("Stepchild V0.1                  ");

  setupPins();

  //seeding random number generator
  srand(1);

  initSeq(16,768);
  turnOffLEDs();

  // if(helloChild){
    bootscreen();
    // testItalicFont();
    // helloChild_5();
  // }
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }

  setNormalMode();

  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }

  counterA = 0;
  counterB = 0;

  core0ready = true;
  lastTime = millis();
}
#endif

void sequenceLEDs(){
  int length = loopData[activeLoop][1]-loopData[activeLoop][0];
  int val = playheadPos/(length/8);
  bool leds[8] = {0,0,0,0,0,0,0,0};
  leds[val] = true;
  writeLEDs(leds);
}

void testLEDs(){
  bool leds[8] = {0,0,0,0,0,0,0,0};
  for(int i = 0; i<8; i++){
    if(step_buttons[i]){
      leds[i] = 1;
    }
  }
  writeLEDs(leds);
}
void testJoyStick(){
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(200) && menu_Press){
      lastTime = millis();
      return;
    }
    float X = analogRead(x_Pin);
    float Y = analogRead(y_Pin);
    display.clearDisplay();
    printSmall(0,0,"X: "+stringify(X)+","+stringify(x),1);
    printSmall(0,10,"Y: "+stringify(Y)+","+stringify(y),1);
    display.drawCircle(X/8.0,Y/16.0,4,1);
    display.display();
  }
}

#include "screenSavers.h"

void loop() {
  inputRead();
  displaySeq();
  screenSaverCheck();
}

//the closer the step is to the subDiv (both forward and backward), the shorter the time val
unsigned long getTimeValue(unsigned short int step){
  step%=swingSubDiv;
  //if it's closer to the next subDiv, move it forward
  if(step>=swingSubDiv/2){
  }
  //if it's closer to the last subDiv, move it backward
  else if(step<swingSubDiv/2){
  }
  if(swingVal != 0){
    return MicroSperTimeStep;
  }
  else
    return MicroSperTimeStep;
}

//returns the offset in microseconds at a given step
float swingOffset(unsigned short int step){
  float offset = swingVal*sin(2*PI/swingSubDiv * (step-swingSubDiv/4));
  return offset;
}

void debugTestSwing(){
  long int i;
  swingSubDiv = 24;//whole note swing
  long int  time;
  while(true){
    float t = swingOffset(i);
    i++;
    Serial.print("step: ");
    Serial.println(i);
    Serial.print("swing offset: ");
    Serial.println(t);
    Serial.print("w/o swing: ");
    Serial.println(MicroSperTimeStep);
    Serial.print("w/swing: ");
    Serial.println(t+MicroSperTimeStep);
    i%=96;
  }
}
//this is a sloppy lil function that returns true if the time is within (x) of the subDiv
bool onBeat(int subDiv, int fudge){
  long msPerB = (240000/(bpm*subDiv));
  long offVal = millis()%msPerB;
  //if it's within fudge of this beat or the next
  if(offVal<=fudge || offVal>= msPerB - fudge ){
    return true;
  }
  else 
    return false;
}

//old,working one without floats (uses %)
bool hasItBeenEnoughTime_swing(){
  timeElapsed = micros()-timeLastStepPlayed;
  if(timeElapsed >= MicroSperTimeStep+swingOffset(playheadPos)){
    if(!(playheadPos%swingSubDiv)){//if it's a multiple of the swing subDiv, it should be perfectly on time, so grab the offset from here
     offBy = (micros()-startTime)%(MicroSperTimeStep);
    }
    // Serial.println(timeElapsed);
    if(offBy == 0)
      startTime = micros();
    timeLastStepPlayed = micros();
    return true;
  }
  else
    return false;
}

//old,working one without floats (uses %)
bool hasItBeenEnoughTime(){
  if(swung){
    return hasItBeenEnoughTime_swing();
  }
  else{
    timeElapsed = micros()-timeLastStepPlayed;
    if(timeElapsed + offBy >= MicroSperTimeStep){
      offBy = (micros()-startTime)%MicroSperTimeStep;
      if(offBy == 0)
        startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    }
    else
      return false;
  }
}
bool hasItBeenEnoughTime_cycleCount(){
  timeElapsed = rp2040.getCycleCount()-timeLastStepPlayed;
  if(timeElapsed + offBy >= MicroSperTimeStep){
    offBy = (micros()-startTime)%MicroSperTimeStep;
    timeLastStepPlayed = rp2040.getCycleCount();
    return true;
  }
  else
    return false;
}
//this one is for the clockMenu
bool hasItBeenEnoughTime_clock(int timeStep){
  timeElapsed = micros()-timeLastStepPlayed;
  if(swung){
    if(timeElapsed >= MicroSperTimeStep+swingOffset(timeStep)){
      timeLastStepPlayed = micros();
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(timeElapsed >= MicroSperTimeStep){
      timeLastStepPlayed = micros();
      return true;
    }
    else{
      return false;
    }
  }
}

//this sets up non-midi stuff
void setup1() {
  core1ready = true;
  while(!core0ready){
  }
}

//cuts notes off when loop repeats, then starts new note at beginning
void cutLoop(){
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].noteLastSent != 255){
      seqData[i][seqData[i].size()-1].endPos = loopData[activeLoop][1];
      //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
      if(recMode == 1)
        writeNoteOn(loopData[activeLoop][0],trackData[i].pitch,seqData[i][seqData[i].size()-1].velocity,trackData[i].channel);
    }
  }
}

//this checks loops bounds, moves to next loop, and cuts loop
void checkLoop(){
  if(playing){
    if (playheadPos > loopData[activeLoop][1]-1) { //if the timestep is past the end of the loop, loop it to the start
      loopCount++;
      if(loopCount > loopData[activeLoop][2]){
        nextLoop();
      }
      playheadPos = loopData[activeLoop][0];
      if(!isLooping)
        togglePlayMode();
    }
  }
  else if(recording){
    //one-shot record to current loop, without looping
    if(recMode == 0){
      if(recheadPos>=loopData[activeLoop][1]){
        toggleRecordingMode(waitForNote);
      }
    }
    //record to loops as they play in sequence
    else if(recMode == 1){
      if(recheadPos>=loopData[activeLoop][1]){
        cutLoop();
        loopCount++;
        if(loopData[activeLoop][2]>=loopCount){
          nextLoop();
        }
        recheadPos = loopData[activeLoop][0];
      }
    }
  }
}

//this cpu handles time-sensitive things
void loop1(){
  //play mode
  if(playing){
    //internal timing
    if(!externalClock){
      if(hasItBeenEnoughTime()){
        // digitalWrite(onboard_ledPin,((passiveTimer/96)%2)?HIGH:LOW);
        sendClock();
        playStep(playheadPos);
        playheadPos++;
        passiveTimer++;
        checkLoop();
        checkFragment();
        if(!(playheadPos%12)){
          pramOffset = !pramOffset;
        }
      }
    }
    //external timing
    else if(externalClock){
      readMIDI();
      if(gotClock && hasStarted){
        gotClock = false;
        playStep(playheadPos);
        playheadPos += 1;
        passiveTimer+=1;
        checkLoop();
        checkFragment();
      }
    }
  }
  //record mode
  else if(recording){
    readMIDI();
    if(!externalClock){
      if(hasItBeenEnoughTime()){
        timeLastStepPlayed = micros();
        //if it's not in wait mode, or if it is but a note has been received
        if(!waitForNote || !waiting){
          continueStep(recheadPos);
          sendClock();
          recheadPos++;
          passiveTimer++;
          checkLoop();
          checkFragment();
        }
      }
    }
    else if(externalClock){
      if(gotClock && hasStarted){
        gotClock = false;
        continueStep(recheadPos);
        recheadPos+=1;
        passiveTimer+=1;
        checkLoop();
        checkFragment();
      }
    }
  }
  //default state
  else{
    playheadPos = loopData[activeLoop][0];
    recheadPos = loopData[activeLoop][0];
    fragmentStep = 0;
    readMIDI();
  }
  if(isArping){
    //if it was active, but hadn't started playing yet
    if(!activeArp.playing){
      if(playlist.size()>0){
        activeArp.start();
      }
    }
    if(activeArp.playing){
     //if there are no notes,and it's not latched, just stop
      if(playlist.size() == 0 && !activeArp.holding){
        // activeArp.debugPrintArp();
        activeArp.stop();
        // sendMIDIallOff();
      }
      //if there are no notes, and it IS latched, then trigger the "waiting for more notes" var and continue
      else if(activeArp.hasItBeenEnoughTime()){
        activeArp.playstep();
      }
    }
  }
  //this is so that it chills out before going into sleep mode
  //maybe not helpful/not fixing the problem
  // while(sleeping){
  //   core1ready = false;
  // }
  // core1ready = true;
  //encoders
}

#ifdef HEADLESS
const int windowW = 128*windowScale;
const int windowH = 64*windowScale;

int main(int argc, char **argv)
{
    setup();
    headless();
    return 0;
}
#endif
