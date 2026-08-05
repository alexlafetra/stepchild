/*
    Using the Stepchild's stepbuttons as a keyboard or drumpad (depending on the mode)
*/

#include "Stepchild.h"

#include "mainSequence.h"
;

using namespace std;

//drawing keys
void StepchildGraphics::drawKeys(uint8_t xStart,uint8_t yStart,uint8_t octave,uint8_t numberOfKeys, bool fromPlaylist){
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
    if(fromPlaylist){
      if(stepchild.receivedNotes.containsPitch(key+octave*12)){
        pressed = true;
      }
    }
    else{
      if(stepchild.arpeggiator.playing && stepchild.arpeggiator.lastPitchSent%12 == key){
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
        if(pressed){
          graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-1,keyLength-10, keyHeight, keyWidth, xSlant,4);
        }
        else
          graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
        blackKeys++;
      }
    }
    else if(whiteKeys<9){
      //drawing pitches
      if(pressed){
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset)+2,keyLength, keyHeight, keyWidth, xSlant,2);
      }
      else
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
      whiteKeys++;
    }
  }
}
void StepchildGraphics::drawKeys_inverse(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeys){
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
    if(stepchild.receivedNotes.containsPitch(key%12)){
      pressed = true;
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
        graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-1,keyLength-10, keyHeight, keyWidth, xSlant,5);
      }
      else
        graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,1);
      blackKeys++;
    }
    else{
      if(pressed){
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset)+2,keyLength, keyHeight, keyWidth, xSlant,4);
      }
      else
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,3);
      whiteKeys++;
    }
  }
}

#define KEYBOARD_ANIMATION_IN_DELAY 10
#define KEYBOARD_ANIMATION_OUT_DELAY 10

void keyboardAnimation(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeys, bool into){
  uint8_t keyLength = 40;
  uint8_t keyHeight = 5;
  uint8_t keyWidth = 3;
  uint8_t xSlant = 8;
  uint8_t offset = 3;
  uint8_t whiteKeys = 0;
  uint8_t blackKeys = 0;

  // stepchild.display.clearDisplay();

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
        graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
        blackKeys++;
        stepchild.display.display();
        delay(KEYBOARD_ANIMATION_IN_DELAY);
      }
      else{
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
        whiteKeys++;
        stepchild.display.display();
        delay(KEYBOARD_ANIMATION_IN_DELAY);
      }
    }
  }
  //this one needs to go in reverse. Both drum and this animation do this by just drawing the first X keys, then decrementing it
  else if(!into){
    while(numberOfKeys>0){
      stepchild.display.clearDisplay();
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
          graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
          blackKeys++;
        }
        else{
          graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
          whiteKeys++;
        }
      }
      numberOfKeys--;
      delay(KEYBOARD_ANIMATION_OUT_DELAY);
    }
  }
}

void StepchildGraphics::drawKeyboard(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeysToDraw){
  const uint8_t keyLength = 40;
  const uint8_t keyHeight = 5;
  const uint8_t keyWidth = 3;
  const uint8_t xSlant = 8;
  const uint8_t offset = 3;
  uint8_t whiteKeys = 0;
  uint8_t blackKeys = 0;

  //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
  for(int key = startKey; key<startKey+numberOfKeysToDraw; key++){
    //if it's a black key
    if((startKey+key)%12 == 1 || (startKey+key)%12 == 3 || (startKey+key)%12 == 6 || (startKey+key)%12 == 8 || (startKey+key)%12 == 10){
      uint8_t blackKeyOffset = 0;
      if(blackKeys%12>1 && blackKeys%12<4){
        blackKeyOffset = key/12+1;
      }
      if(blackKeys%12>=4 && blackKeys%12<=6){
        blackKeyOffset = key/12+1;
      }
      graphics.drawBox(xStart+(blackKeys+blackKeyOffset)*(xSlant+offset)+25, yStart+(blackKeys+blackKeyOffset)*(keyWidth+offset)-3,keyLength-10, keyHeight, keyWidth, xSlant,3);
      blackKeys++;
    }
    else{
      graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
      whiteKeys++;
    }
  }
}

void keyboard(){
  keyboardAnimation(0,5,0,14,true);
  while(true){
    stepchild.buttons.readButtons();
    mainSequencerStepButtons();//handles notes, and toggling
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        break;
      }
    }
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
    stepchild.display.clearDisplay();
    graphics.drawKeys(0,5,stepchild.getOctave(36),14,true);//always start on a C, for simplicity
    stepchild.display.display();
  }
  keyboardAnimation(0,5,0,14,false);
}

