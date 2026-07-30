#include "Arduino.h"
#include <vector>
#include "commonEnums.h"
#include "Stepchild.h"

#include "mainSequence.h"

using namespace std;

/*
    Functions that involve custom GUI layouts for user input/output
*/
void alert(String text, int time){
  unsigned short int len = text.length()*6;
  stepchild.display.clearDisplay();
  stepchild.display.setCursor(stepchild.SCREEN_WIDTH/2-len/2, 29);
  stepchild.display.print(text);
  stepchild.display.display();
  delay(time);
}


CoordinatePair selectNotesAndArea(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
    CoordinatePair bounds;
    //storing a copy of the current loop, so we can edit the current loop to set our bounds
    //and then reset it back to the original loop
    while(true){
        stepchild.buttons.readJoystick();
        stepchild.buttons.readButtons();
        defaultSelectBoxControls();
        defaultJoystickControls(false);
        defaultEncoderControls();
        if(stepchild.itsbeen(200)){
            defaultLoopControls();
            defaultSelectControls();
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                stepchild.clearSelection();
                break;
            }
            if(stepchild.buttons.NEW()){
                stepchild.lastTime = millis();
                break;
            }
        }
        stepchild.display.clearDisplay();
        SequenceRenderSettings settings;
        settings.topLabels = false;
        settings.shadeOutsideLoop = true;
        settings.drawPram = false;
        settings.shrinkTopDisplay = false;
        graphics.drawSeq(settings);
        graphics.printSmall(stepchild.TRACK_LABEL_WIDTH,0,text,1);
        iconFunction(7,1,14,true);
        stepchild.display.display();
    }
    bounds.start.x = stepchild.loopData[stepchild.activeLoop].start;
    bounds.end.x = stepchild.loopData[stepchild.activeLoop].end;
    return bounds;
}



void drawCoordinateBox(CoordinatePair coords, SequenceRenderSettings& settings){
  if(!stepchild.selectionBox.begun && (coords.start.x != coords.end.x)){
    //correcting bounds for view
    unsigned short int X1;
    unsigned short int X2;
    unsigned short int Y1;
    unsigned short int Y2;

    if(coords.start.x>coords.end.x){
      X1 = coords.end.x;
      X2 = coords.start.x;
    }
    else{
      X1 = coords.start.x;
      X2 = coords.end.x;
    }
    if(coords.start.y>coords.end.y){
      Y1 = coords.end.y;
      Y2 = coords.start.y;
    }
    else{
      Y1 = coords.start.y;
      Y2 = coords.end.y;
    }

    //if it's offscreen, return
    if(X2<=stepchild.viewStart || X1>=stepchild.viewEnd || Y1 > stepchild.startTrack+stepchild.endTrack || Y2<stepchild.startTrack){
      return;
    }

    if(X1<stepchild.viewStart){
      X1 = stepchild.viewStart;
    }
    if(X2>stepchild.viewEnd){
      X2 = stepchild.viewEnd;
    }
    if(Y1<stepchild.startTrack){
      Y1 = stepchild.startTrack;
    }
    if(Y2>(stepchild.startTrack+stepchild.maxTracksShown)){
      Y2 = stepchild.startTrack+stepchild.maxTracksShown;
    }
    uint8_t startX = stepchild.TRACK_LABEL_WIDTH+(X1-stepchild.viewStart)*stepchild.viewScale;
    uint8_t length = (X2-X1)*stepchild.viewScale;
    uint8_t startHeight = stepchild.shrinkTopDisplay?8:stepchild.HEADER_HEIGHT;
    uint8_t startY = (Y1-stepchild.startTrack)*stepchild.trackHeight+startHeight;
    uint8_t height = ((Y2 - stepchild.startTrack + 1)*stepchild.trackHeight - startY)%(stepchild.SCREEN_HEIGHT-startHeight) + startHeight;
   
   if((millis())%400>200){
      graphics.shadeRect(startX,startY,length,height,3);
    }
    else{
      stepchild.display.drawRect(startX,startY,length,height,1);
    }
  }
}

String enterText(String title, uint8_t count){
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
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.SHIFT()){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.DELETE() && text.length()>0){
        stepchild.buttons.setDELETE(false);
        String newString = text.substring(0,text.length()-1);
        text = newString;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.SELECT() ){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<count){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.MENU()){
        text = "";
        done = true;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        highlight = alphabet.size()-1;
      }
    }
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickX == -1 && highlight<alphabet.size()-1){
        highlight++;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickX == 1 && highlight>0){
        highlight--;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == -1 && highlight>=columns){
        highlight-=columns;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        stepchild.lastTime = millis();
      }
    }
    //title
    stepchild.display.clearDisplay();
    stepchild.display.setCursor(5,6);
    stepchild.display.setFont(&FreeSerifItalic9pt7b);
    stepchild.display.print(title);
    stepchild.display.setFont();

    //text tooltip
    graphics.printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    graphics.printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    stepchild.display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      stepchild.display.print(text+(text.length()<count?alphabet[highlight]:""));
      //cursor
      if(millis()%750>250){
        stepchild.display.drawFastVLine(9+text.length()*6,14,9,SSD1306_WHITE);
      }
    }
    else
      stepchild.display.print(text);

    //blank spaces
    for(uint8_t i = text.length(); i<count; i++){
      stepchild.display.drawFastHLine(10+i*6,22,5,1);
    }
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          stepchild.display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              stepchild.display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              stepchild.display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // stepchild.display.print(alphabet[count]);
              // stepchild.display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              stepchild.display.print(alphabet[count]);
              stepchild.display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // stepchild.display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            stepchild.display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    stepchild.display.display();
  }
  stepchild.buttons.clearButtons();
  stepchild.lastTime = millis();
  return text;
}

String enterText(String title){
  return enterText(title, 10);
}


void binarySelectionBoxPlaceholderDisplayFunction(){}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
//Binary Selection box w a title, returns -1 for 
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //storing the state
  bool state = false;
  stepchild.lastTime = millis();
  
  while(true){
    stepchild.display.clearDisplay();
    drawingFunction();
    graphics.drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    stepchild.display.display();
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    if(stepchild.itsbeen(200)){
      //x to select option
      if(stepchild.buttons.joystickX != 0){
        if(stepchild.buttons.joystickX == 1 && state){
          state = !state;
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == -1 && !state){
          state = !state;
          stepchild.lastTime = millis();
        }
      }
      //menu/del to cancel
      if(stepchild.buttons.MENU() || stepchild.buttons.DELETE()){
        stepchild.lastTime = millis();
        return -1;
      }
      //choose option
      else if(stepchild.buttons.NEW() || stepchild.buttons.SELECT() ){
        stepchild.lastTime = millis();
          stepchild.buttons.setNEW(false);
        stepchild.buttons.setSELECT(false);
        return state;
      }
    }
  }
  return false;
}
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    stepchild.display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    stepchild.display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    stepchild.display.setCursor(x1+20, y1+10);
    graphics.printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      stepchild.display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        stepchild.display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      graphics.printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    stepchild.display.display();
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0){
        if(stepchild.buttons.joystickX == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(stepchild.buttons.joystickX == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(stepchild.buttons.SELECT() ){
        selected = true;
        time = millis();
      }
    }
  }
  stepchild.buttons.setSELECT(false);
  stepchild.lastTime = millis();
  stepchild.display.invertDisplay(false);
  return select;
}

//this one has a title, but displays a blank screen
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title){
  return binarySelectionBox(x1,y1,op1,op2,title,binarySelectionBoxPlaceholderDisplayFunction);
}

//Binary Selection box w no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}


uint8_t dropDownMenu(vector<String> options, void (*drawingFunction)()){
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t menuStart = 0;
  uint8_t cursor = 0;
  uint8_t longestOptionLength = 0;
  for(String s:options){
    if(graphics.getSmallTextLength(s)>longestOptionLength)
      longestOptionLength = graphics.getSmallTextLength(s);
  }
  //each is 9px tall ==> 64/9 = 7
  uint8_t maxNumberOfOptionsShown = options.size()>7?7:options.size();
  while(true){
    stepchild.display.clearDisplay();
    drawingFunction();
    stepchild.display.fillRect(x,y,longestOptionLength,maxNumberOfOptionsShown*9,0);
    stepchild.display.drawRect(x,y,longestOptionLength,maxNumberOfOptionsShown*9,1);
    for(uint8_t i = 0; i<maxNumberOfOptionsShown; i++){
      graphics.printSmall(x,y+i*9,options[i+menuStart],1);
      if(i == cursor-menuStart)
        graphics.drawArrow(x,y+i*9+3,3,ARROW_RIGHT,false);
    }
    stepchild.display.display();

    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.DOWN() && cursor < options.size()){
        cursor++;
        if(cursor-menuStart>7)
          menuStart++;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.UP() && cursor > 0){
        cursor--;
        while(cursor<menuStart)
          menuStart--;
        stepchild.lastTime = millis();
      }
    }
  }
  return cursor;
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


vector<uint8_t> selectKeys(uint8_t startRoot) {
  uint8_t selected = 0;
  bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float animOffset = 0;
  bool done = false;
  uint8_t root = startRoot%12;
  //FUCK
  ScaleName activeScale = MAJOR;
  bool showingScale = false;
  while (!done) {
    animOffset++;
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if (stepchild.itsbeen(100)) {
      if (stepchild.buttons.joystickX == -1 && selected < 11) {
        selected++;
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickX == 1 && selected > 0) {
        selected--;
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.itsbeen(200)) {
      if (stepchild.buttons.SELECT() ) {
        stepchild.buttons.setSELECT(false);
        stepchild.lastTime = millis();
        keys[selected] = !keys[selected];
      }
      if(stepchild.buttons.NEW()){
          stepchild.buttons.setNEW(false);
        stepchild.lastTime = millis();
        done = true;
      }
      if(stepchild.buttons.MENU()){
        stepchild.buttons.setMENU(false) ;
        stepchild.lastTime = millis();
        done = true;
      }
    }
    //changing root
    while(stepchild.buttons.counterA != 0){
      if(stepchild.buttons.counterA >= 1)
        root++;
      if(stepchild.buttons.counterA <= -1){
        if(root == 0)
          root = 11;
        else
          root--;
      }
      root%=12;
      stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
    }
    //changing scale
    while(stepchild.buttons.counterB != 0){
      if(!showingScale){
        showingScale = true;
        activeScale = MAJOR;
      }
      else{
        if(stepchild.buttons.counterB > 0){
          activeScale++;
        }
        if(stepchild.buttons.counterB < 0){
          activeScale--;
        }
      }
      vector<uint8_t> newScale = stepchild.makePitchListFromScale(activeScale,0);
      //clear out old scale
      for(uint8_t i = 0; i<12; i++){
        keys[i] = false;
      }
      //putting in newScale
      for(uint8_t i = 0; i<newScale.size(); i++){
        keys[newScale[i]] = true;
      }
      stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
    }
    //also changing root and scale, but with joystick
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.joystickY != 0){
        if(stepchild.buttons.SHIFT()){
          if(stepchild.buttons.joystickY == 1){
            root++;
            root%=12;
            stepchild.lastTime = millis();
          }
          if(stepchild.buttons.joystickY == -1){
            if(root == 0)
              root = 11;
            else
              root--;
            stepchild.lastTime = millis();
          }
        }
        else{
          if(!showingScale){
            showingScale = true;
            activeScale = MAJOR;
            stepchild.lastTime = millis();
          }
          else{
            if(stepchild.buttons.joystickY == 1){
              activeScale++;
              stepchild.lastTime = millis();
            }
            if(stepchild.buttons.joystickY == -1){
              activeScale--;
              stepchild.lastTime = millis();
            }
          }
          vector<uint8_t> newScale = stepchild.makePitchListFromScale(activeScale,0);
          //clear out old scale
          for(uint8_t i = 0; i<12; i++){
            keys[i] = false;
          }
          //putting in newScale
          for(uint8_t i = 0; i<(newScale.size()>12?12:newScale.size()); i++){
            keys[newScale[i]] = true;
          }
        }
      }
    }
    //-----------------------------------
    uint8_t xStart = 4;
    uint8_t yStart = 11;
    uint8_t keyHeight = 28;
    uint8_t keyWidth = 15;
    uint8_t offset = 3;
    uint8_t blackKeys = 0;
    uint8_t whiteKeys = 0;
    stepchild.display.clearDisplay();
    //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
        if (i == selected) {
          stepchild.display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys)-2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          stepchild.display.print(getInterval(0, i));
        }
        else if (keys[i]) {
          graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
          stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) - 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          stepchild.display.print(getInterval(0, i));
        }
        else {
          stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
        }
        whiteKeys++;
      }
    }
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
        //if it's highlighted
        if (i == selected) {
          stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          stepchild.display.print(getInterval(0, i));
        }
        //if it's selected
        else if (keys[i]) {
            graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
          stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          stepchild.display.print(getInterval(0, i));
        }
        //if it's normal
        else {
          stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
          stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
        }
        blackKeys++;
      }
    }
    if(showingScale){
      graphics.printCursive_centered(64, 1, stepchild.getScaleName(activeScale),SSD1306_WHITE);
    }
    else{
      graphics.printCursive_centered(64, 1, "scale",SSD1306_WHITE);
    }
    graphics.printCursive(40, stepchild.SCREEN_HEIGHT-7,"root",SSD1306_WHITE);
    stepchild.display.setCursor(69,stepchild.SCREEN_HEIGHT-7);
    stepchild.display.print(" "+stepchild.pitchToString(root,false,true));
    stepchild.display.display();
  }
    vector<uint8_t> returnedKeys = {};
  for(uint8_t i = 0; i<12; i++){
    if(keys[i]){
      returnedKeys.push_back((i+root)%12);
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
      stepchild.display.clearDisplay();
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
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              stepchild.display.print(getInterval(0, i));            
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      stepchild.display.display();
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
      stepchild.display.clearDisplay();
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
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              stepchild.display.print(getInterval(0, i));
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              stepchild.display.print(getInterval(0, i));            
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              stepchild.display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              stepchild.display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              stepchild.display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              stepchild.display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      stepchild.display.display();
    }
  }
}


//Enter note selection screen, and display a custom icon in the corner
//Used for the FX a lot
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
  while(true){
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    defaultEncoderControls();
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
      stepchild.selectionBox.begun = true;
      stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
    }
    //if stepchild.buttons.SELECT()  is released, and there's a selection box
    if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
      stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
      stepchild.selectionBox.begun = false;
      stepchild.selectionBox.select();
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        return true;
      }
      if(stepchild.buttons.MENU()){
        stepchild.clearSelection();
        stepchild.lastTime = millis();
        return false;
      }
      if(stepchild.buttons.SELECT() ){
        if(stepchild.buttons.SHIFT()){
          stepchild.clearSelection();
          stepchild.toggleSelectNote(stepchild.activeTrack,stepchild.IDAtCursor(),false);
        }
        else{
          stepchild.toggleSelectNote(stepchild.activeTrack,stepchild.IDAtCursor(),true);
        }
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.itsbeen(100)) {
      if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(stepchild.cursorPos%stepchild.subDivision){
          stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveCursor(-stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
      if (stepchild.buttons.joystickX == -1 && !stepchild.buttons.SHIFT()) {
        if(stepchild.cursorPos%stepchild.subDivision){
          stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveCursor(stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
      if (stepchild.buttons.joystickY == 1) {
        if(stepchild.recording())
          stepchild.setActiveTrack(stepchild.activeTrack + 1, false);
        else
          stepchild.setActiveTrack(stepchild.activeTrack + 1, false);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickY == -1) {
        if(stepchild.recording())
          stepchild.setActiveTrack(stepchild.activeTrack - 1, false);
        else
          stepchild.setActiveTrack(stepchild.activeTrack - 1, false);
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.itsbeen(50)) {
      if (stepchild.buttons.joystickX == 1 && stepchild.buttons.SHIFT()) {
        stepchild.moveCursor(-1);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickX == -1 && stepchild.buttons.SHIFT()) {
        stepchild.moveCursor(1);
        stepchild.lastTime = millis();
      }
    }
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    settings.shrinkTopDisplay = false;
    graphics.drawSeq(settings);
    if(!stepchild.selectionCount){
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH,0,"select notes to "+text,1);
    }
    else{
      graphics.drawButton(stepchild.TRACK_LABEL_WIDTH,0,"n",1);
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH+9,1,"to "+text+" "+stringify(stepchild.selectionCount)+(stepchild.selectionCount == 1?" note":" notes"),1);
    }
    iconFunction(7,1,14,true);
    stepchild.display.display();
  }
}

void defaultIconFunction(uint8_t x, uint8_t y, uint8_t w, bool s){return;}
bool selectNotes(String t){
  return selectNotes(t,defaultIconFunction);
}

vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(stepchild.trackData.size());
  bool movingBetweenNotes = false;
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawLoopPoints = false;
  settings.trackSelection = false;
  while(true){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    //selectionBox
    //when stepchild.buttons.SELECT()  is pressed and stick is moved, and there's no selection box
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
      stepchild.selectionBox.begun = true;
      stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
    }
    //if stepchild.buttons.SELECT()  is released, and there's a selection box
    if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
      stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
      if(stepchild.selectionBox.coords.start.x>stepchild.selectionBox.coords.end.x){
        unsigned short int x1_old = stepchild.selectionBox.coords.start.x;
        stepchild.selectionBox.coords.start.x = stepchild.selectionBox.coords.end.x;
        stepchild.selectionBox.coords.end.x = x1_old;
      }
      if(stepchild.selectionBox.coords.start.y>stepchild.selectionBox.coords.end.y){
        unsigned short int y1_old = stepchild.selectionBox.coords.start.y;
        stepchild.selectionBox.coords.start.y = stepchild.selectionBox.coords.end.y;
        stepchild.selectionBox.coords.end.y = y1_old;
      }
      for(int track = stepchild.selectionBox.coords.start.y; track<=stepchild.selectionBox.coords.end.y; track++){
        for(int time = stepchild.selectionBox.coords.start.x; time<=stepchild.selectionBox.coords.end.x; time++){
          if(stepchild.lookupTable[track][time] != 0){
            //if the note isn't in the vector yet, add it
            if(!isInVector(stepchild.lookupTable[track][time],selectedNotes[track]))
              selectedNotes[track].push_back(stepchild.lookupTable[track][time]);
            time = stepchild.noteData[track][stepchild.lookupTable[track][time]].endPos;
          }
        }
      }
      stepchild.selectionBox.begun = false;
    }
    if(stepchild.itsbeen(100)){
      if(!movingBetweenNotes){
        if(stepchild.buttons.joystickY == 1){
          stepchild.setActiveTrack(stepchild.activeTrack+1,false);
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.joystickY == -1){
          stepchild.setActiveTrack(stepchild.activeTrack-1,false);
          stepchild.lastTime = millis();
        }
      }
    }
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickX != 0){
        if(!movingBetweenNotes){
          if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
            if(stepchild.cursorPos%stepchild.subDivision){
              stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
              stepchild.lastTime = millis();
            }
            else{
              stepchild.moveCursor(-stepchild.subDivision);
              stepchild.lastTime = millis();
            }
          }
          if (stepchild.buttons.joystickX == -1 && !stepchild.buttons.SHIFT()) {
            if(stepchild.cursorPos%stepchild.subDivision){
              stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
              stepchild.lastTime = millis();
            }
            else{
              stepchild.moveCursor(stepchild.subDivision);
              stepchild.lastTime = millis();
            }
          }
        }
        else{
          if(stepchild.buttons.joystickX == 1){
            stepchild.moveToNextNote(false,false);
            stepchild.lastTime = millis();
          }
          else if(stepchild.buttons.joystickX == -1){
            stepchild.moveToNextNote(true,false);
            stepchild.lastTime = millis();
          }
        }
      }
    }
    if(stepchild.itsbeen(200)){
      //select
      if(stepchild.buttons.SELECT()  && stepchild.IDAtCursor() != 0 && !stepchild.selectionBox.begun){
        unsigned short int id;
        id = stepchild.IDAtCursor();
        if(stepchild.buttons.SHIFT()){
          //del old vec
          vector<vector<uint8_t>> temp;
          temp.resize(stepchild.trackData.size());
          selectedNotes = temp;
          selectedNotes[stepchild.activeTrack].push_back(stepchild.IDAtCursor());
        }
        else{
          //if the note isn't in the vector yet, add it
          if(!isInVector(stepchild.IDAtCursor(),selectedNotes[stepchild.activeTrack]))
            selectedNotes[stepchild.activeTrack].push_back(stepchild.IDAtCursor());
          //if it is, remove it
          else{
            vector<uint8_t> temp;
            for(int i = 0; i<selectedNotes[stepchild.activeTrack].size(); i++){
              //push back all the notes that aren't the one the cursor is on
              if(selectedNotes[stepchild.activeTrack][i] != stepchild.IDAtCursor()){
                temp.push_back(selectedNotes[stepchild.activeTrack][i]);
              }
            }
            selectedNotes[stepchild.activeTrack] = temp;
          }
        }
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.DELETE()){
        stepchild.lastTime = millis();
        selectedNotes.clear();
        break;
      }
      if(stepchild.buttons.NEW()){
        stepchild.buttons.setNEW(false);
        stepchild.lastTime = millis();
        break;
      }
      if(stepchild.buttons.LOOP()){
        stepchild.lastTime = millis();
        movingBetweenNotes = !movingBetweenNotes;
      }
    }
    stepchild.display.clearDisplay();
    graphics.drawSeq(settings);
    graphics.printSmall(0,0,text1,SSD1306_WHITE);
    graphics.printSmall(0,8,text2,SSD1306_WHITE);
    if(movingBetweenNotes){
      if(millis()%1000 >= 500){
        stepchild.display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
      }
      else{
        stepchild.display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
      }
    }
    //draw a note bracket on any note that's been added to the selection
    for(int track = 0; track<selectedNotes.size(); track++){
      for(int note = 0; note<selectedNotes[track].size(); note++){
        graphics.drawNoteBracket(stepchild.noteData[track][selectedNotes[track][note]],track,settings);
      }
    }
    stepchild.display.display();
  }
  return selectedNotes;
}
