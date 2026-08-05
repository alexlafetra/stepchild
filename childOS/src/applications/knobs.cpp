#include "Stepchild.h"

#include "stringPatch.h"



using namespace std;

void drawKnobs(uint8_t activeA, uint8_t activeB, uint8_t activeRow, uint8_t howMany, bool selected, uint8_t ccType,uint8_t valA, uint8_t valB, bool xyMode){
  const uint8_t bigR = 12;
  const uint8_t smallR = 5;
  const uint8_t x2 = stepchild.SCREEN_WIDTH-bigR-5;
  uint8_t y2 = bigR;
  uint8_t x1 = 14;
  uint8_t y1 = 14;
  uint8_t count = 0;
  
  //printing little rows of encoders

  //labels for the cc/ch vals
  stepchild.display.drawBitmap(0,y1-12,cc_tiny,5,3,1);
  stepchild.display.drawBitmap(0,y1+13,ch_tiny,6,3,1);
  for(uint8_t row = 0; row<2; row++){
    //if one row is selected, draw the cc display on the other
    if(selected && row != activeRow){
      String param = ccType?getMKIIParameterName(stepchild.controlKnobs[activeRow?(activeB+8):activeA].cc,stepchild.controlKnobs[activeRow?(activeB+8):activeA].channel):getCCParameterName(stepchild.controlKnobs[activeRow?(activeB+8):activeA].cc);
      graphics.printSmall_centered(48,y1,"["+param+"]",1);
    }
    //if you're not in selected mode, draw both rows like normal
    else{
      for(uint8_t column = 0; column<8; column++){
        if(count<howMany){
          //values to display
          uint8_t value = stepchild.controlKnobs[row*8+column].val;
          uint8_t cc = stepchild.controlKnobs[row*8+column].cc;
          uint8_t ch = stepchild.controlKnobs[row*8+column].channel;
          int8_t offset = (column%2)?-2:2;//offset to stagger the dials
          //if it's the active knob
          if((row == 0 && column == activeA) || (row == 1 && column == activeB)){
            //cursor arrow for activeRow
            if(row == activeRow){
              if(selected){
                graphics.drawArrow(x2-bigR-13+((millis()/200)%2),y2-bigR+6+row*32,3,ARROW_RIGHT,true);
                graphics.drawArrow(x2-bigR-13+((millis()/200)%2),y2-bigR+17+row*32,3,ARROW_RIGHT,true);
              }
              else
                graphics.drawArrow(x1,y1+offset+smallR+6+2*((millis()/200)%2),3,ARROW_UP,false);
            }
            //drawing each dial in white, if it's active
            stepchild.display.fillCircle(x1,y1+offset,smallR,1);
            stepchild.display.drawCircle(x1,y1+offset,smallR-1,0);
            graphics.drawCircleRadian(x1,y1+offset,smallR,float(value)*float(360)/float(127)+270,0);
          }
          else{
            //drawing the dials in black, when not active
            stepchild.display.fillCircle(x1,y1+offset,smallR,0);
            stepchild.display.drawCircle(x1,y1+offset,smallR,1);
            graphics.drawCircleRadian(x1,y1+offset,smallR,float(value)*float(360)/float(127)+270,1);
          }
          //printing the cc and ch vals
          graphics.printSmall_centered(x1+1,y1+offset-smallR-6,stringify(cc),1);
          graphics.printSmall_centered(x1+1,y1+offset+smallR+2,stringify(ch),1);
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
    Knob k = stepchild.controlKnobs[(i==0)?activeA:(activeB+8)];
    if(!selected)
      //drawing the val with a highlight, when it's in value mode
      graphics.drawLabel(x2-bigR-7,y2+bigR-1,stringify(k.val),1);
    else{
      //when it's in 'selected' mode, don't highlight val so that
      //user knows they're changing cc/ch now
      graphics.printSmall_centered(x2-bigR-7,y2+bigR-1,stringify(k.val),1);
    }
    if(xyMode){
      //draw dials in white when in xy mode
      stepchild.display.fillCircle(x2,y2,bigR,1);
      stepchild.display.drawCircle(x2,y2,bigR-1,0);
      graphics.drawCircleRadian(x2,y2,bigR,float(i==0?valA:valB)/float(127)*float(360)+270,0);
    }
    else{
      //draw dials in black when in normal mode
      stepchild.display.drawCircle(x2,y2,bigR,1);
      graphics.drawCircleRadian(x2,y2,bigR,float(i==0?valA:valB)/float(127)*float(360)+270,1);
    }
    //printing data to left of the dial
    stepchild.display.drawBitmap(x2-bigR-10,y2-bigR,cc_tiny,5,3,1);
    stepchild.display.drawBitmap(x2-bigR-10,y2-bigR+11,ch_tiny,6,3,1);
    graphics.printSmall_centered(x2-bigR-7,y2-bigR+4,stringify(k.cc),1);
    graphics.printSmall_centered(x2-bigR-7,y2-bigR+15,stringify(k.channel),1);
    y2+=32;
  }
  if(xyMode){
    graphics.printItalic(x2-3,9,"Y",0);
    graphics.printItalic(x2-3,41,"X",0);
  }
}
void knobsAnimationIn(){
  uint8_t howMany = 0;
  while(howMany<16){
    stepchild.display.clearDisplay();
    drawKnobs(0,0,0,howMany,false,0,stepchild.controlKnobs[0].val,stepchild.controlKnobs[8].val,false);
    stepchild.display.display();
    howMany++;
  }
}
void knobsAnimationOut(uint8_t activeA, uint8_t activeB, uint8_t activeRow){
  int8_t howMany = 16;
  while(howMany>=0){
    stepchild.display.clearDisplay();
    drawKnobs(activeA,activeB,activeRow,howMany,false,0,stepchild.controlKnobs[0].val,stepchild.controlKnobs[8].val,false);
    stepchild.display.display();
    howMany-=2;
  }
}

bool knobs(){
  knobsAnimationIn();
  uint8_t activeKnobA = 0;
  uint8_t activeKnobB = 0;//this is also between 0 and 7
  uint8_t activeRow = 0;
  uint8_t ccType = 0;//0 for gen. midi, 1 for mk2
  bool selected = false;
  bool xyMode = false;
  //range is (5-1023) aka 0-1018 (took this from xyGrid)
  const float scaleF = float(128)/float(1018);

  uint8_t valA = stepchild.controlKnobs[activeKnobA].val;
  uint8_t valB = stepchild.controlKnobs[activeKnobB].val;

  while(true){
    valA += (stepchild.controlKnobs[activeKnobA].val-valA)/4;
    valB += (stepchild.controlKnobs[activeKnobB+8].val-valB)/4;
    if(xyMode){
      stepchild.controlKnobs[activeKnobB+8].val = abs(128 - (analogRead(JOYSTICK_X) - 5) * scaleF);
      stepchild.controlKnobs[activeKnobA].val = abs((analogRead(JOYSTICK_Y) - 5) * scaleF);
      if(stepchild.controlKnobs[activeKnobA].val>127)
        stepchild.controlKnobs[activeKnobA].val = 127;
      if(stepchild.controlKnobs[activeKnobB+8].val>127)
        stepchild.controlKnobs[activeKnobB+8].val = 127;
    }
    stepchild.display.clearDisplay();
    drawKnobs(activeKnobA,activeKnobB,activeRow,16,selected,ccType,valA,valB,xyMode);
    stepchild.display.display();
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    //changing cc/channel
    if(selected){
      while(stepchild.buttons.counterA != 0){
        if(stepchild.buttons.counterA >= 1 && stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel<16){
          stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel++;
        }
        else if(stepchild.buttons.counterA <= -1 && stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel>0){
          stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].channel--;
        }
        stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].send();

        stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
      }
      while(stepchild.buttons.counterB != 0){
        if(stepchild.buttons.counterB >= 1 && stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc<127){
          stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc++;
        }
        else if(stepchild.buttons.counterB <= -1 && stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc>0){
          stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].cc--;
        }
        stepchild.controlKnobs[activeRow?(activeKnobB+8):activeKnobA].send();
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
      }
    }
    else{
      while(stepchild.buttons.counterB != 0){
        if(stepchild.buttons.counterB >= 1){
          stepchild.controlKnobs[activeKnobA].increment(stepchild.buttons.SHIFT()?1:8);
        }
        else if(stepchild.buttons.counterB <= -1){
          stepchild.controlKnobs[activeKnobA].increment(stepchild.buttons.SHIFT()?-1:int8_t(-8));
        }
        stepchild.controlKnobs[activeKnobA].send();
        // stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
        stepchild.buttons.counterB+=stepchild.buttons.counterB<0?1:-1;
      }
      while(stepchild.buttons.counterA != 0){
        if(stepchild.buttons.counterA >= 1){
          stepchild.controlKnobs[activeKnobB+8].increment(stepchild.buttons.SHIFT()?1:8);
        }
        else if(stepchild.buttons.counterA <= -1){
          stepchild.controlKnobs[activeKnobB+8].increment(stepchild.buttons.SHIFT()?-1:int8_t(-8));
        }
        stepchild.controlKnobs[activeKnobB+8].send();
        stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
      }
    }
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickX != 0){
        if(!xyMode){
          if(stepchild.buttons.joystickX == -1){
            if(activeRow == 0 && activeKnobA<7){
              activeKnobA++;
              stepchild.lastTime = millis();
            }
            else if(activeRow == 1 && activeKnobB<7){
              activeKnobB++;
              stepchild.lastTime = millis();
            }
          }
          else if(stepchild.buttons.joystickX == 1){
            if(activeRow == 0 && activeKnobA>0){
              activeKnobA--;
              stepchild.lastTime = millis();
            }
            else if(activeRow == 1 && activeKnobB>0){
              activeKnobB--;
              stepchild.lastTime = millis();
            }
          }
        }
      }
      if(stepchild.buttons.joystickY != 0){
        if(!xyMode){
          if(stepchild.buttons.joystickY == 1 && activeRow == 0){
            activeRow = 1;
            stepchild.lastTime = millis();
          }
          if(stepchild.buttons.joystickY == -1 && activeRow == 1){
            activeRow = 0;
            stepchild.lastTime = millis();
          }
        }
      }
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.SELECT() ){
        selected = !selected;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.SHIFT()){
        activeRow = activeRow?1:0;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.PLAY()){
        xyMode = !xyMode;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
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
  return true;
}
