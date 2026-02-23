#include "Stepchild.h"
#include "StepchildGraphics.h"
#include "guiUtilities.h"
#include "mainSequence.h"
#include "graphics/WireFrame.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;

using namespace std;

/*

  Code for the random menu, and the genRandom() & selectArea_random() functions

*/

CoordinatePair selectArea_random(){
  CoordinatePair coords;
  coords.start.x = 0;
  coords.end.x = 0;
  coords.start.y = 0;
  coords.end.y = 0;
  WireFrame dieModel = genRandMenuObjects(12,0,10,0.5);
  while(true){
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    defaultEncoderControls();
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
      stepchild.selectionBox.begun = true;
      stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
      coords.start.x = stepchild.cursorPos;
      coords.start.y = stepchild.activeTrack;
    }
    //if stepchild.buttons.SELECT()  is released, and there's a selection box
    if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
      stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
      stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
      stepchild.selectionBox.begun = false;
      coords.end.x = stepchild.cursorPos;
      coords.end.y = stepchild.activeTrack;
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        return coords;
      }
      if(stepchild.buttons.MENU()){
        coords.start.x = 0;
        coords.end.x = 0;
        coords.start.y = 0;
        coords.end.y = 0;
        stepchild.lastTime = millis();
        return coords;
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
          stepchild.setActiveTrack(stepchild.activeTrack + 1, true);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickY == -1) {
        if(stepchild.recording())
          stepchild.setActiveTrack(stepchild.activeTrack - 1, false);
        else
          stepchild.setActiveTrack(stepchild.activeTrack - 1, true);
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
    dieModel.rotate(1,0);
    dieModel.rotate(1,1);
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawPram = false;
    settings.topLabels = false;
    graphics.drawSeq(settings);
    drawCoordinateBox(coords,settings);
    if(stepchild.shrinkTopDisplay){
      dieModel.scale = 0.25;
      dieModel.offset.y = 4;
      dieModel.drawDots = false;
      dieModel.renderDie();
    }
    else{
      dieModel.scale = 0.5;
      dieModel.offset.y = 8;
      dieModel.drawDots = true;
      dieModel.renderDie();
    }
    if(coords.start.x == 0 && coords.end.x == 0 && coords.start.y == 0 && coords.end.y == 0){
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH,1,"select an area!",1);
    }
    else{
      graphics.drawButton(stepchild.TRACK_LABEL_WIDTH,0,"n",1);
      graphics.printSmall(stepchild.TRACK_LABEL_WIDTH+9,1,"to randomize",1);
    }
    stepchild.display.display();
  }
}

void genRandom(RandomData randData){
  // while(true){
    CoordinatePair coords = selectArea_random();
    if(coords.start.x == coords.end.x && coords.start.y == coords.end.y){
      return;
    }
    if(coords.start.x>coords.end.x){
      uint16_t temp = coords.start.x;
      coords.start.x = coords.end.x;
      coords.end.x = temp;
    }
    if(coords.start.y>coords.end.y){
      uint8_t temp = coords.start.y;
      coords.start.y = coords.end.y;
      coords.end.y = temp;
    }
    //iterate over the tracks (inclusively)
    for(uint8_t t = coords.start.y; t<=coords.end.y; t++){
      for(uint16_t step = coords.start.x; step<coords.end.x; step+=randData.everyNSteps){
        //if the step isn't a start pos
        if(stepchild.lookupTable[t][step] == 0 || stepchild.noteAt(t,step).startPos != t){
          if(random(0,100)<randData.odds){
            uint8_t chance = random(randData.minChance,randData.maxChance+1);
            uint16_t length = random(randData.minLength,randData.maxLength+1);
            uint8_t vel = random(randData.minVel,randData.maxVel+1);
            //making sure notes won't run off the end
            if(step+length>coords.end.x){
              length = coords.end.x-step;
            }
            Note newNote = Note(step,step+length,vel,chance,false,false);
            stepchild.makeNote(newNote, t);
          }
        }
      }
    }
  // }
}
/*
  Note options: vel, chance, length
  placement options: odds of a note being placed, where the note is placed
*/
class RandomMenu:public StepchildMenu{
  public:
    WireFrame icon;
    SequenceRenderSettings settings;
    RandomMenu(){
      settings.topLabels = false;
      settings.shrinkTopDisplay = false;
      coords = CoordinatePair(44,0,128,64);
      icon = genRandMenuObjects(112,13,10,1);
      icon.scale = 0.8;
    }
    void displayMenu(){
      icon.rotate(1,0);
      icon.rotate(1,1);
      stepchild.display.clearDisplay();
      graphics.drawSeq(settings);
      graphics.ditherBackground(32,16,128,64);

      graphics.drawButton(32,0,"n",1);
      graphics.printSmall(41,1,"to add notes",1);
      stepchild.display.fillRoundRect(coords.start.x-2,12,coords.end.x-coords.start.x+2,coords.end.y-12,5,0);
      stepchild.display.drawRoundRect(coords.start.x-2,12,coords.end.x-coords.start.x+5,coords.end.y-12,5,1);
      stepchild.display.drawFastHLine(coords.start.x+51,12,77-coords.start.x,0);
      stepchild.display.fillRect(coords.start.x+52,0,127,12,0);
      stepchild.display.drawFastVLine(coords.start.x+51,0,13,1);
      icon.renderDie();

      //min/max chart
      const uint8_t x = 54;
      const uint8_t y = coords.start.y+28;
      graphics.printSmall(coords.start.x+x,y,"min",1);
      stepchild.display.fillCircle(coords.start.x+x+5,y+10,4,1);
      graphics.printSmall(coords.start.x+x+4,y+8,"B",0);

      //dividing line on chart
      stepchild.display.drawFastVLine(coords.start.x+x+13,y,64-y,1);
      //max
      graphics.printSmall(coords.start.x+x+16,y,"max",1);
      stepchild.display.fillCircle(coords.start.x+x+21,y+10,4,1);
      graphics.printSmall(coords.start.x+x+20,y+8,"A",0);

      //placement
      graphics.printChunky(coords.start.x,coords.start.y+16,"placement",1);
      graphics.printSmall(coords.start.x+7,coords.start.y+23,"odds:",1);
      graphics.printSmall(coords.start.x+30,coords.start.y+23,stringify(stepchild.randomFXData.odds)+"%",1);
      if(cursor == 0){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+25,2,ARROW_RIGHT,true);
      }
      graphics.printSmall(coords.start.x+7,coords.start.y+29,"every:",1);
      graphics.printSmall(coords.start.x+30,coords.start.y+29,stepchild.stepsToMeasures(stepchild.randomFXData.everyNSteps),1);
      if(cursor == 1){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+31,2,ARROW_RIGHT,true);
      }

      //note options
      graphics.printChunky(coords.start.x,coords.start.y+37,"notes",1);

      //vel
      graphics.printSmall(coords.start.x+7,coords.start.y+44,"velocity:",1);
      String s  = stringify(stepchild.randomFXData.minVel);
      graphics.printSmall(coords.start.x+x+12-graphics.getSmallTextLength(s),coords.start.y+44,s,1);
      s = stringify(stepchild.randomFXData.maxVel);
      graphics.printSmall(coords.start.x+x+15,coords.start.y+44,s,1);
      if(cursor == 2){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+46,2,ARROW_RIGHT,true);
      }

      //length
      graphics.printSmall(coords.start.x+7,coords.start.y+50,"length:",1);
      s = stepchild.stepsToMeasures(stepchild.randomFXData.minLength);
      graphics.printSmall(coords.start.x+x+12-graphics.getSmallTextLength(s),coords.start.y+50,s,1);
      s = stepchild.stepsToMeasures(stepchild.randomFXData.maxLength);
      graphics.printSmall(coords.start.x+x+15,coords.start.y+50,s,1);
      if(cursor == 3){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+52,2,ARROW_RIGHT,true);
      }

      //chance
      graphics.printSmall(coords.start.x+7,coords.start.y+57,"chance:",1);
      s  = stringify(stepchild.randomFXData.minChance);
      graphics.printSmall(coords.start.x+x+12-graphics.getSmallTextLength(s),coords.start.y+57,s,1);
      s = stringify(stepchild.randomFXData.maxChance);
      graphics.printSmall(coords.start.x+x+15,coords.start.y+57,s,1);
      if(cursor == 4){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+59,2,ARROW_RIGHT,true);
      }
      stepchild.display.display();
    }
    bool randomMenuControls(){
      stepchild.buttons.readButtons();
      stepchild.buttons.readJoystick();
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.MENU()){
          stepchild.lastTime = millis();
          return false;
        }
        if(stepchild.buttons.DOWN() && cursor > 0){
          cursor--;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.UP() && cursor < 4){
          cursor++;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.NEW()){
          slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
          stepchild.lastTime = millis();
          genRandom(stepchild.randomFXData);
          slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
        }
        while(stepchild.buttons.counterB){
          switch(cursor){
            //odds of a note appearing
            case 0:{
              int8_t temp = stepchild.randomFXData.odds;
              int8_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterB<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                stepchild.buttons.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterB = 0;
              }
              stepchild.randomFXData.odds = temp;
            }
              break;
            //place each note is attempted to be placed ('every n steps')
            case 1:
              if(stepchild.buttons.SHIFT()){
                stepchild.randomFXData.everyNSteps = toggleTriplets(stepchild.randomFXData.everyNSteps);
              }
              stepchild.randomFXData.everyNSteps = changeSubDiv(stepchild.buttons.counterB>0,stepchild.randomFXData.everyNSteps,false);
              break;
            //velocity
            case 2:{
              int16_t temp = stepchild.randomFXData.maxVel;
              int16_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterB<0?-increment:increment; 
              if(temp > 127){
                temp = 127;
                stepchild.buttons.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterB = 0;
              }
              stepchild.randomFXData.maxVel = temp;
            }
              break;
            //length
            case 3:
              if(stepchild.buttons.SHIFT()){
                stepchild.randomFXData.maxLength = toggleTriplets(stepchild.randomFXData.maxLength);
              }
              stepchild.randomFXData.maxLength = changeSubDiv(stepchild.buttons.counterB>0,stepchild.randomFXData.maxLength,false);
              break;
            //chance
            case 4:{
              int8_t temp = stepchild.randomFXData.maxChance;
              int8_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterB<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                stepchild.buttons.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterB = 0;
              }
              stepchild.randomFXData.maxChance = temp;
            }
              break;
          }
          stepchild.buttons.countDownB();
        }
        while(stepchild.buttons.counterA){
          switch(cursor){
            //odds of a note appearing
            case 0:{
              int8_t temp = stepchild.randomFXData.odds;
              int8_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterA<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                stepchild.buttons.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterA = 0;
              }
                stepchild.randomFXData.odds = temp;
            }
              break;
            case 1:
              if(stepchild.buttons.SHIFT()){
                stepchild.randomFXData.everyNSteps = toggleTriplets(stepchild.randomFXData.everyNSteps);
              }
              stepchild.randomFXData.everyNSteps = changeSubDiv(stepchild.buttons.counterA>0,stepchild.randomFXData.everyNSteps,false);
              break;
            case 2:{
              int16_t temp = stepchild.randomFXData.minVel;
              int16_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterA<0?-increment:increment; 
              if(temp > 127){
                temp = 127;
                stepchild.buttons.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterA = 0;
              }
              stepchild.randomFXData.minVel = temp;
            }
              break;
            case 3:
              if(stepchild.buttons.SHIFT()){
                stepchild.randomFXData.minLength = toggleTriplets(stepchild.randomFXData.minLength);
              }
              stepchild.randomFXData.minLength = changeSubDiv(stepchild.buttons.counterA>0,stepchild.randomFXData.minLength,false);
              break;
            case 4:{
              int8_t temp = stepchild.randomFXData.minChance;
              int8_t increment = 10;
              if(stepchild.buttons.SHIFT()){
                increment = 1;
              }
              temp += stepchild.buttons.counterA<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                stepchild.buttons.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterA = 0;
              }
              stepchild.randomFXData.minChance = temp;
            }
              break;
          }
          stepchild.buttons.countDownA();
        }
      }
      return true;
    }
    
};

bool randomMenu(){
  RandomMenu menu;
  menu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
  while(menu.randomMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
  return false;
}
