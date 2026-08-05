#include "Stepchild.h"

#include "mainSequence.h"

using namespace std;

;


void drumPadAnimation(uint8_t xStart,uint8_t yStart, uint8_t startPad, uint8_t numberOfPads, bool into){
  uint8_t width = 15;
  int xSlant = -7;
  uint8_t xSpacing = 5;
  uint8_t ySpacing = 2;
  uint8_t thickness = 4;
  uint8_t rows = 4;
  uint8_t columns = 4;
  uint8_t pad = startPad+numberOfPads-1;
  if(into){
    stepchild.display.clearDisplay();
    //draws pads one at a time, with a delay in between
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(pad>=startPad){
          graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
          pad--;
          stepchild.display.display();
          delay(10);
        }
      }
    }
  }
  else if(!into){
  //this one needs to loop backwards! or at least look like it
    while(numberOfPads > 0){
      stepchild.display.clearDisplay();
      pad = numberOfPads;
      for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
          if(pad>0){
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
            pad--;
          }
        }
      }
      numberOfPads--;
      if(!(numberOfPads%4)){
        stepchild.display.display();
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

  // graphics.drawBox(xStart, yStart, width, thickness, width-7, xSlant, 1);
  for(int i = 0; i<rows; i++){
    for(int j = 0; j<columns; j++){
      if(pad>=startPad){
        pressed = false;
        if(stepchild.receivedNotes.containsPitch(pad)){
          pressed = true;
        }
        if(pressed){
          if(pad == 36 && stepchild.buttons.SHIFT()){//so that you can see the keyboard pitch when shifting through
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,0);
          }
          else
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,2);
          if(stepchild.buttons.SHIFT()){
            stepchild.display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            stepchild.display.setTextColor(SSD1306_BLACK);
            // stepchild.display.print(stepchild.pitchToString(pad,false,true));
            stepchild.display.print(pad);
            stepchild.display.setTextColor(SSD1306_WHITE);
          }
        }
        else{
          if(pad == 36 && stepchild.buttons.SHIFT()){//so that you can see the keyboard pitch when shifting through
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,0);
          }
          else
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
          if(stepchild.buttons.SHIFT()){
            stepchild.display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            stepchild.display.print(stepchild.pitchToString(pad,false,true));
          }
        }
        pad--;
      }
    }
  }
  if(stepchild.buttons.SHIFT()){
    stepchild.display.setCursor(0,0);
    stepchild.display.setTextSize(2);
    stepchild.display.print(stepchild.getOctave(36));
    stepchild.display.setTextSize(1);
  }
}

void drumPad(){
  drumPadAnimation(stepchild.SCREEN_WIDTH-25,5,36,16, true);
  while(true){
    stepchild.buttons.readButtons();
    mainSequencerStepButtons();
    //changing pitch range
    while(stepchild.buttons.counterA != 0){
      if(stepchild.buttons.counterA >= 1 && 36<127){
        // 36++;
      }
      else if(stepchild.buttons.counterA <= -1 && 36>0){
        // 36--;
      }
      stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
    }
    while(stepchild.buttons.counterB != 0){
      if(stepchild.buttons.counterB >= 1 && stepchild.defaultChannel<16){
        stepchild.defaultChannel++;
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
      }
      else if(stepchild.buttons.counterB <= -1 && stepchild.defaultChannel>0){
        stepchild.defaultChannel--;
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
      }
    }
    //play/rec
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.PLAY() && !stepchild.recording()){
        if(!stepchild.buttons.SHIFT()){
          stepchild.lastTime = millis();
          stepchild.togglePlay();
        }
        else if(stepchild.buttons.SHIFT()){
          stepchild.lastTime = millis();
          stepchild.toggleRecording(stepchild.waitForNoteBeforeRec);
        }
      }
      if(stepchild.buttons.PLAY() && stepchild.recording()){
        stepchild.lastTime = millis();
        stepchild.toggleRecording(stepchild.waitForNoteBeforeRec);
      }
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        break;
      }
    }
    stepchild.display.clearDisplay();
    drawDrumPads(stepchild.SCREEN_WIDTH-25,5,36 - 36%12,16);
    stepchild.display.display();
  }
  drumPadAnimation(stepchild.SCREEN_WIDTH-25,5,36,16, false);
}