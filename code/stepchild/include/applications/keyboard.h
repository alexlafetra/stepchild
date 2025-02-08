/*
    Using the Stepchild's stepbuttons as a keyboard or drumpad (depending on the mode)
*/

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
    if(fromPlaylist){
      if(receivedNotes.containsPitch(key+octave*12)){
        pressed = true;
      }
    }
    else{
      if(arp.playing && arp.lastPitchSent%12 == key){
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
    if(receivedNotes.containsPitch(key%12)){
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
    display.clearDisplay();
    //draws pads one at a time, with a delay in between
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(pad>=startPad){
          graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
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
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
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

  // graphics.drawBox(xStart, yStart, width, thickness, width-7, xSlant, 1);
  for(int i = 0; i<rows; i++){
    for(int j = 0; j<columns; j++){
      if(pad>=startPad){
        pressed = false;
        if(receivedNotes.containsPitch(pad)){
          pressed = true;
        }
        if(pressed){
          if(pad == 36 && controls.SHIFT()){//so that you can see the keyboard pitch when shifting through
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,0);
          }
          else
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i+2, width, thickness, width-4, xSlant,2);
          if(controls.SHIFT()){
            display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            display.setTextColor(SSD1306_BLACK);
            // display.print(pitchToString(pad,false,true));
            display.print(pad);
            display.setTextColor(SSD1306_WHITE);
          }
        }
        else{
          if(pad == 36 && controls.SHIFT()){//so that you can see the keyboard pitch when shifting through
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,0);
          }
          else
            graphics.drawBox(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i, yStart+(width-4+ySpacing)*i, width, thickness, width-4, xSlant,1);
          if(controls.SHIFT()){
            display.setCursor(xStart-(xSpacing+width)*j-(-xSlant+ySpacing-1)*i-1,yStart+(width-4+ySpacing)*i+2);
            display.print(pitchToString(pad,false,true));
          }
        }
        pad--;
      }
    }
  }
  if(controls.SHIFT()){
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print(getOctave(36));
    display.setTextSize(1);
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

  // display.clearDisplay();

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
        display.display();
        delay(KEYBOARD_ANIMATION_IN_DELAY);
      }
      else{
        graphics.drawBox(xStart+whiteKeys*(xSlant+offset), yStart+whiteKeys*(keyWidth+offset),keyLength, keyHeight, keyWidth, xSlant,1);
        whiteKeys++;
        display.display();
        delay(KEYBOARD_ANIMATION_IN_DELAY);
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

void drawKeyboard(uint8_t xStart,uint8_t yStart,uint8_t startKey,uint8_t numberOfKeysToDraw){
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
    controls.readButtons();
    mainSequencerStepButtons();//handles notes, and toggling
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        break;
      }
    }
    //changing pitch range
    while(controls.counterA != 0){
      if(controls.counterA >= 1 && 36<127){
        // 36++;
      }
      else if(controls.counterA <= -1 && 36>0){
        // 36--;
      }
      controls.counterA += controls.counterA<0?1:-1;
    }
    while(controls.counterB != 0){
      if(controls.counterB >= 1 && sequence.defaultChannel<16){
        sequence.defaultChannel++;
        controls.counterB += controls.counterB<0?1:-1;;
      }
      else if(controls.counterB <= -1 && sequence.defaultChannel>0){
        sequence.defaultChannel--;
        controls.counterB += controls.counterB<0?1:-1;;
      }
    }
    display.clearDisplay();
    drawKeys(0,5,getOctave(36),14,true);//always start on a C, for simplicity
    display.display();
  }
  keyboardAnimation(0,5,0,14,false);
}

