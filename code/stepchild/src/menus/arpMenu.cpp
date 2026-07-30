#include "Stepchild.h"

;

using namespace std;


// 'gear', 11x11px
const unsigned char gear_bmp [] = {
	0x04, 0x00, 0x15, 0x00, 0x3f, 0x80, 0x71, 0xc0, 0x2e, 0x80, 0xea, 0xe0, 0x2e, 0x80, 0x71, 0xc0, 
	0x3f, 0x80, 0x15, 0x00, 0x04, 0x00
};

// 'down_mode', 11x11px
const unsigned char arp_down_mode []  = {
	0xff, 0xe0, 0x80, 0x20, 0xa0, 0xa0, 0xb1, 0xa0, 0xbb, 0xa0, 0xbf, 0xa0, 0x9f, 0x20, 0x8e, 0x20, 
	0x84, 0x20, 0x80, 0x20, 0xff, 0xe0
};
// 'down_up', 11x11px
const unsigned char arp_down_up []  = {
	0xff, 0xe0, 0x80, 0x20, 0x8a, 0x20, 0x8b, 0x20, 0x8a, 0xa0, 0x8a, 0x20, 0xaa, 0x20, 0x9a, 0x20, 
	0x8a, 0x20, 0x80, 0x20, 0xff, 0xe0
};
// 'play_mode', 11x11px
const unsigned char arp_play_mode []  = {
	0xff, 0xe0, 0x80, 0x20, 0xb0, 0x20, 0xac, 0x20, 0xa3, 0x20, 0xa0, 0xa0, 0xa3, 0x20, 0xac, 0x20, 
	0xb0, 0x20, 0x80, 0x20, 0xff, 0xe0
};
// 'random_mode', 11x11px
const unsigned char arp_random_mode []  = {
	0xff, 0xe0, 0x80, 0x20, 0x9e, 0x20, 0xb5, 0x20, 0xaf, 0xa0, 0xb9, 0xa0, 0xaa, 0xa0, 0x9c, 0xa0, 
	0x8f, 0xa0, 0x80, 0x20, 0xff, 0xe0
};
// 'up_down', 11x11px
const unsigned char arp_up_down []  = {
	0xff, 0xe0, 0x80, 0x20, 0x8a, 0x20, 0x9a, 0x20, 0xaa, 0x20, 0x8a, 0x20, 0x8a, 0xa0, 0x8b, 0x20, 
	0x8a, 0x20, 0x80, 0x20, 0xff, 0xe0
};
// 'up_mode', 11x11px
const unsigned char arp_up_mode []  = {
	0xff, 0xe0, 0x80, 0x20, 0x84, 0x20, 0x8e, 0x20, 0x9f, 0x20, 0xbf, 0xa0, 0xbb, 0xa0, 0xb1, 0xa0, 
	0xa0, 0xa0, 0x80, 0x20, 0xff, 0xe0
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 288)
const unsigned char* arpMode_icons[6] = {
	arp_play_mode,
	arp_down_mode,
	arp_up_mode,
	arp_up_down,
	arp_down_up,
	arp_random_mode
};

void drawArpMenu(uint8_t cursor, int8_t xStart);

void drawArpStepLengths(uint8_t xStart, uint8_t yStart, uint8_t startNote, uint8_t xCursor, bool selected){
  if(stepchild.arpeggiator.uniformLength){
    graphics.drawCenteredBanner(64,20,"using uniform steps of "+stepchild.stepsToMeasures(stepchild.arpeggiator.arpSubDiv));
    graphics.drawLabel(64,32,"[SEL] to toggle custom steps",true);
  }
  uint8_t spacing = 3;
  uint8_t thickness = (stepchild.SCREEN_WIDTH-8)/stepchild.arpeggiator.lengths.size()-spacing;
  if(stepchild.arpeggiator.lengths.size()>=8){
    thickness = (stepchild.SCREEN_WIDTH-12)/8-spacing;
  }
  uint8_t height;
  for(uint8_t i = 0; i<8; i++){
    //only draw blocks for lengths that exist (in case there are less than 16)
    //also, only draw blocks that will still be on screen
    if(i<stepchild.arpeggiator.lengths.size()){
      height = float(stepchild.arpeggiator.lengths[i+startNote])*float(64-23)/float(96);
      //drawing filled rect for steps that correspond to currently
      //held notes
      if(i + startNote<stepchild.arpeggiator.notes.size())
        stepchild.display.fillRect(9+(spacing+thickness)*i,stepchild.SCREEN_HEIGHT-height-7, thickness, height,SSD1306_WHITE);
      //and empty rects for steps that don't
      else
        stepchild.display.drawRect(9+(spacing+thickness)*i,stepchild.SCREEN_HEIGHT-height-7, thickness, height,SSD1306_WHITE);
      //highlighting the step that's currently playing
      if(stepchild.arpeggiator.playing && (i+startNote == stepchild.arpeggiator.activeNote)){
        // stepchild.display.fillRect(9+(spacing+thickness)*i,stepchild.SCREEN_HEIGHT-6,thickness,6,1);
        stepchild.display.drawRect(7+(spacing+thickness)*i,stepchild.SCREEN_HEIGHT-height-9, thickness+4, height+4,SSD1306_WHITE);
      }
      graphics.printSmall(10+(spacing+thickness)*i+thickness/2-stringify(i+startNote+1).length()*2,stepchild.SCREEN_HEIGHT-5,stringify(i+startNote),2);
      //step the cursor is on
      if(i == xCursor-startNote){
        graphics.drawArrow(9+(spacing+thickness)*i+thickness/2,stepchild.SCREEN_HEIGHT-height-10+2*((millis()/200)%2),3,ARROW_DOWN,true);
        graphics.printSmall(8+(spacing+thickness)*i+thickness/2-stepchild.stepsToMeasures(stepchild.arpeggiator.lengths[i+startNote]).length()*2+2,stepchild.SCREEN_HEIGHT-height-21+2*((millis()/200)%2),stepchild.stepsToMeasures(stepchild.arpeggiator.lengths[i+startNote]),SSD1306_WHITE);
      }
      //if there are steps offscreen
      if(startNote>0){
        graphics.drawArrow(2+2*((millis()/200)%2),61,2,ARROW_LEFT,true);
      }
      if(stepchild.arpeggiator.lengths.size()>startNote+8){
        graphics.drawArrow(stepchild.SCREEN_WIDTH-2-2*((millis()/200)%2),61,2,ARROW_RIGHT,true);
      }
    }
  }
  // printArp(28,0,"step lengths",SSD1306_WHITE);
  // graphics.printSmall_centered(64,0,"steps",1);
  stepchild.display.setRotation(1);
  graphics.printItalic(16,0,"steps",1);
  stepchild.display.setRotation(DISPLAY_UPRIGHT);
}

void deleteCustomLength(uint8_t which){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<stepchild.arpeggiator.lengths.size(); i++){
    if(i != which){
      newNotes.push_back(stepchild.arpeggiator.lengths[i]);
    }
  }
  stepchild.arpeggiator.lengths.swap(newNotes);
}

void swapCustomLengths(uint8_t which1, uint8_t which2){
  uint8_t temp = stepchild.arpeggiator.lengths[which1];
  stepchild.arpeggiator.lengths[which1] = stepchild.arpeggiator.lengths[which2];
  stepchild.arpeggiator.lengths[which2] = temp;
}

void addCustomLength(uint8_t size, uint8_t where){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<stepchild.arpeggiator.lengths.size(); i++){
    if(i == where){
      newNotes.push_back(size);
    }
    newNotes.push_back(stepchild.arpeggiator.lengths[i]);
  }
  stepchild.arpeggiator.lengths.swap(newNotes);
}

void customLengthsMenu(){
  uint8_t cursorX = 0;
  bool selected = false;
  uint8_t startNote = 0;
  while(true){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.SELECT() ){
        stepchild.lastTime = millis();
        stepchild.arpeggiator.uniformLength = !stepchild.arpeggiator.uniformLength;
      }
    }
    if(stepchild.arpeggiator.uniformLength){
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.MENU()){
          stepchild.buttons.setMENU(false) ;
          stepchild.lastTime = millis();
          return;
        }
      }
    }
    //these controls are only active if the arp is using custom lengths
    else{
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.NEW()){
          addCustomLength(stepchild.arpeggiator.lengths[cursorX],cursorX);
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.SELECT() ){
          stepchild.arpeggiator.uniformLength = !stepchild.arpeggiator.uniformLength;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.DELETE() && stepchild.arpeggiator.lengths.size()>1){
          deleteCustomLength(cursorX);
          stepchild.lastTime = millis();
          while(startNote+7>=stepchild.arpeggiator.lengths.size()&& stepchild.arpeggiator.lengths.size()>8){
            startNote--;
          }
          while(cursorX>=stepchild.arpeggiator.lengths.size())
            cursorX--;
        }
        if(stepchild.buttons.MENU()){
          stepchild.lastTime = millis();
          stepchild.buttons.setMENU(false) ;
          break;
        }
        if(stepchild.buttons.SELECT() ){
          selected = !selected;
          stepchild.lastTime = millis();
        }
      }
      if(stepchild.itsbeen(120)){
        if(stepchild.buttons.joystickY != 0){
          if(stepchild.buttons.SHIFT()){
            stepchild.arpeggiator.lengths[cursorX] = toggleTriplets(stepchild.arpeggiator.lengths[cursorX]);
            stepchild.lastTime = millis();
          }
          else{
            if(stepchild.buttons.joystickY == -1){
              stepchild.lastTime = millis();
              stepchild.arpeggiator.lengths[cursorX] = changeSubDiv(true,stepchild.arpeggiator.lengths[cursorX],false);
            }
            else if(stepchild.buttons.joystickY == 1){
              stepchild.lastTime = millis();
              stepchild.arpeggiator.lengths[cursorX] = changeSubDiv(false,stepchild.arpeggiator.lengths[cursorX],false);
            }
          }
        }
      }
      if(stepchild.itsbeen(100)){
        if(stepchild.buttons.joystickX != 0){
          if(stepchild.buttons.joystickX == -1 && cursorX<stepchild.arpeggiator.lengths.size()-1){
            cursorX++;
            stepchild.lastTime = millis();
          }
          if(stepchild.buttons.joystickX == 1 && cursorX>0){
            cursorX--;
            stepchild.lastTime = millis();
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
    stepchild.display.clearDisplay();
    drawArpStepLengths(0,0,startNote,cursorX,selected);
    stepchild.display.display();
  }
}

void drawArpModeIcon(uint8_t x1, int8_t y1, uint8_t which,uint16_t c){
  which%=6;
  stepchild.display.fillRect(x1,y1,11,11,0);
  stepchild.display.drawBitmap(x1,y1,arpMode_icons[which],11,11,c);
}

void drawModBoxes_old(uint8_t cursor){
  stepchild.display.fillRect(0,2,70,57,0);
  stepchild.display.drawRoundRect(0,2,70,57,3,1);
  graphics.printItalic(2,4,"settings",1);
  String text;
  const uint8_t mid = 32;
  //vel is special, needs a top and a bottom
  int8_t start = mid-stepchild.arpeggiator.maxVelMod/10;
  int8_t end = mid+stepchild.arpeggiator.minVelMod/10;
  int8_t length = end-start;
  stepchild.display.fillRect(3,start,7,length,1);
  graphics.printSmall_centered(7,end+3,stringify(64-stepchild.arpeggiator.minVelMod/2),1);
  graphics.printSmall_centered(7,start-7,stringify(stepchild.arpeggiator.maxVelMod/2),1);
  if(cursor == 0){
    graphics.drawArrow(6,end+8+(millis()/200)%2,3,ARROW_UP,true);
    text = "velocity";
  }
  
  //chance
  stepchild.display.fillRect(13,mid+13-stepchild.arpeggiator.chanceMod/4,7,stepchild.arpeggiator.chanceMod/4,1);
  graphics.printSmall_centered(17,mid+7-stepchild.arpeggiator.chanceMod/4,stringify(stepchild.arpeggiator.chanceMod),1);
  if(cursor == 1){
    graphics.drawArrow(16,mid+15+(millis()/200)%2,3,ARROW_UP,true);
    text = "chance";
  }

  //reps
  stepchild.display.fillRect(23,mid+13-stepchild.arpeggiator.repMod/4,7,stepchild.arpeggiator.repMod/4,1);
  graphics.printSmall_centered(27,mid+7-stepchild.arpeggiator.repMod/4,stringify(stepchild.arpeggiator.repMod),1);
  if(cursor == 2){
    graphics.drawArrow(26,mid+15+(millis()/200)%2,3,ARROW_UP,true);
    text = "repeats";
  }

  //pitch is also special, needs a top and a bottom
  start = mid - stepchild.arpeggiator.maxPitchMod/10;
  end = mid + stepchild.arpeggiator.minPitchMod/10;
  length = end - start;
  stepchild.display.fillRect(33,start,7,length,1);
  graphics.printSmall_centered(37,end+3,stringify(-stepchild.arpeggiator.minPitchMod/16),1);
  graphics.printSmall_centered(37,start-7,stringify(stepchild.arpeggiator.maxPitchMod/16),1);
  if(cursor == 3){
    graphics.drawArrow(36,end+8+(millis()/200)%2,3,ARROW_UP,true);
    text = "octave";
  }

  //play order
  if(cursor == 4){
    start = mid-(stepchild.arpeggiator.playStyle)*12-5;
    for(uint8_t i = 0; i<6; i++){
      drawArpModeIcon(44,start+i*12,i,1);
    }
    graphics.drawArrow(56+(millis()/200)%2,mid,3,ARROW_LEFT,true);
    switch(stepchild.arpeggiator.playStyle){
      case 5:
        text = "random";
        break;
      case 4:
        text = "down/up";
        break;
      case 3:
        text = "up/down";
        break;
      case 2:
        text = "up";
        break;
      case 1:
        text = "down";
        break;
      case 0:
        text = "play order";
        break;
    }
  }
  else{
    drawArpModeIcon(44,mid-5,stepchild.arpeggiator.playStyle,1);
  }
  stepchild.display.setRotation(3);
  graphics.printSmall(16,60,text,1);
  stepchild.display.setRotation(2);
  stepchild.display.fillRoundRect(65,0,15,15,3,0);
  stepchild.display.drawRoundRect(65,0,15,15,3,1);
  stepchild.display.drawBitmap(67,2,gear_bmp,11,11,1);
}
void drawModBoxes(uint8_t cursor){
  graphics.printArp_wiggly(0,3,"settings",1);
  String text;
  const uint8_t mid = 40;
  //vel is special, needs a top and a bottom
  int8_t start = mid-stepchild.arpeggiator.maxVelMod/10;
  int8_t end = mid+stepchild.arpeggiator.minVelMod/10;
  int8_t length = end-start;
  stepchild.display.fillRect(3,start,7,length,1);
  graphics.printSmall_centered(7,end+3,stringify(64-stepchild.arpeggiator.minVelMod/2),1);
  graphics.printSmall_centered(7,start-7,stringify(stepchild.arpeggiator.maxVelMod/2),1);
  if(cursor == 0){
    graphics.drawArrow(6,end+8+(millis()/200)%2,3,ARROW_UP,true);
    text = "velocity";
  }
  
  //chance
  stepchild.display.fillRect(13,mid+13-stepchild.arpeggiator.chanceMod/4,7,stepchild.arpeggiator.chanceMod/4,1);
  graphics.printSmall_centered(17,mid+7-stepchild.arpeggiator.chanceMod/4,stringify(stepchild.arpeggiator.chanceMod),1);
  if(cursor == 1){
    graphics.drawArrow(16,mid+15+(millis()/200)%2,3,ARROW_UP,true);
    text = "chance";
  }

  //reps
  stepchild.display.fillRect(23,mid+13-stepchild.arpeggiator.repMod/4,7,stepchild.arpeggiator.repMod/4,1);
  graphics.printSmall_centered(27,mid+7-stepchild.arpeggiator.repMod/4,stringify(stepchild.arpeggiator.repMod),1);
  if(cursor == 2){
    graphics.drawArrow(26,mid+15+(millis()/200)%2,3,ARROW_UP,true);
    text = "repeats";
  }

  //pitch is also special, needs a top and a bottom
  start = mid - stepchild.arpeggiator.maxPitchMod/10;
  end = mid + stepchild.arpeggiator.minPitchMod/10;
  length = end - start;
  stepchild.display.fillRect(33,start,7,length,1);
  graphics.printSmall_centered(37,end+3,stringify(-stepchild.arpeggiator.minPitchMod/16),1);
  graphics.printSmall_centered(37,start-7,stringify(stepchild.arpeggiator.maxPitchMod/16),1);
  if(cursor == 3){
    graphics.drawArrow(36,end+8+(millis()/200)%2,3,ARROW_UP,true);
    text = "octave";
  }

  //play order
  if(cursor == 4){
    start = mid-(stepchild.arpeggiator.playStyle)*12-5;
    for(uint8_t i = 0; i<6; i++){
      drawArpModeIcon(44,start+i*12,i,1);
    }
    graphics.drawArrow(56+(millis()/200)%2,mid,3,ARROW_LEFT,true);
    switch(stepchild.arpeggiator.playStyle){
      case 5:
        text = "random";
        break;
      case 4:
        text = "down/up";
        break;
      case 3:
        text = "up/down";
        break;
      case 2:
        text = "up";
        break;
      case 1:
        text = "down";
        break;
      case 0:
        text = "play order";
        break;
    }
  }
  else{
    drawArpModeIcon(44,mid-5,stepchild.arpeggiator.playStyle,1);
  }
  graphics.printSmall(0,59,text,1);
}

void arpModMenu(){
  uint8_t cursor = 0;
  while(true){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(50)){
       if(stepchild.buttons.joystickY != 0){
        if(stepchild.buttons.joystickY == 1){
          switch(cursor){
            case 0:
              //editing min vel
              if(stepchild.buttons.SHIFT()){
                if(stepchild.arpeggiator.minVelMod<117)
                  stepchild.arpeggiator.minVelMod+=10;
                else
                  stepchild.arpeggiator.minVelMod = 127;
              }
              else{
                if(stepchild.arpeggiator.maxVelMod>10)
                  stepchild.arpeggiator.maxVelMod-=10;
                else
                  stepchild.arpeggiator.maxVelMod = 0;
              }
              stepchild.lastTime = millis();
              break;
            case 1:
              //editing chance
              if(stepchild.arpeggiator.chanceMod>10)
                stepchild.arpeggiator.chanceMod-=10;
              else
                stepchild.arpeggiator.chanceMod=0;
              stepchild.lastTime = millis();
              break;
            case 2:
              //editing repeats
              if(stepchild.arpeggiator.repMod>10)
                stepchild.arpeggiator.repMod-=10;
              else
                stepchild.arpeggiator.repMod = 0;
              stepchild.lastTime = millis();
              break;
            //pitch
            case 3:
              if(stepchild.buttons.SHIFT()){
                if(stepchild.arpeggiator.minPitchMod<117)
                  stepchild.arpeggiator.minPitchMod+=10;
                else
                  stepchild.arpeggiator.minPitchMod = 127;
              }
              else{
                if(stepchild.arpeggiator.maxPitchMod>10)
                  stepchild.arpeggiator.maxPitchMod-=10;
                else
                  stepchild.arpeggiator.maxPitchMod = 0;
              }
              stepchild.lastTime = millis();
              break;
            //play style
            case 4:
              if(stepchild.arpeggiator.playStyle<5 && stepchild.itsbeen(200)){
                stepchild.arpeggiator.playStyle++;
                stepchild.lastTime = millis();
              }
              break;
          }
        }
        else if(stepchild.buttons.joystickY == -1){
          switch(cursor){
            case 0:
              //editing min vel
              if(stepchild.buttons.SHIFT()){
                if(stepchild.arpeggiator.minVelMod>10)
                  stepchild.arpeggiator.minVelMod-=10;
                else
                  stepchild.arpeggiator.minVelMod = 0;
              }
              else{
                if(stepchild.arpeggiator.maxVelMod<117)
                  stepchild.arpeggiator.maxVelMod+=10;
                else
                  stepchild.arpeggiator.maxVelMod = 127;
              }
              stepchild.lastTime = millis();
              break;
            //editing chance
            case 1:
              if(stepchild.arpeggiator.chanceMod<90)
                stepchild.arpeggiator.chanceMod+=10;
              else
                stepchild.arpeggiator.chanceMod = 100;
              stepchild.lastTime = millis();
              break;
            //editing repeats
            case 2:
              if(stepchild.arpeggiator.repMod<90)
                stepchild.arpeggiator.repMod+=10;
              else
                stepchild.arpeggiator.repMod = 100;
              stepchild.lastTime = millis();
              break;
            //pitch
            case 3:
              if(stepchild.buttons.SHIFT()){
                if(stepchild.arpeggiator.minPitchMod>10)
                  stepchild.arpeggiator.minPitchMod-=10;
                else
                  stepchild.arpeggiator.minPitchMod = 0;
              }
              else{
                if(stepchild.arpeggiator.maxPitchMod<117)
                  stepchild.arpeggiator.maxPitchMod+=10;
                else
                  stepchild.arpeggiator.maxPitchMod = 127;
              }
              stepchild.lastTime = millis();
              break;
            case 4:
              if(stepchild.arpeggiator.playStyle>0 && stepchild.itsbeen(200)){
                stepchild.arpeggiator.playStyle--;
                stepchild.lastTime = millis();
              }
              break;
          }
        }
      }
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        stepchild.buttons.setMENU(false) ;
        return;
      }
      if(stepchild.buttons.joystickX != 0){
        if(stepchild.buttons.joystickX == -1 && cursor<4){
          cursor++;
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == 1 && cursor>0){
          cursor--;
          stepchild.lastTime = millis();
        }
      }
      if(stepchild.buttons.DELETE()){
        switch(cursor){
          case 0:
            stepchild.arpeggiator.maxVelMod = 0;
            stepchild.arpeggiator.minVelMod = 0;
            stepchild.lastTime = millis();
            break;
          case 1:
            stepchild.arpeggiator.chanceMod = 100;
            stepchild.lastTime = millis();
            break;
          case 2:
            stepchild.arpeggiator.repMod = 0;
            stepchild.lastTime = millis();
            break;
          case 3:
            stepchild.arpeggiator.maxPitchMod = 0;
            stepchild.arpeggiator.minPitchMod = 0;
            stepchild.lastTime = millis();
            break;
          case 4:
            stepchild.arpeggiator.playStyle = 0;
            stepchild.lastTime = millis();
            break;
        }
      }
      while(stepchild.buttons.counterA != 0){
        switch(cursor){
          //min vel
          case 0:
            if(stepchild.buttons.counterA < 0 && stepchild.arpeggiator.minVelMod<127){
              stepchild.arpeggiator.minVelMod++;
            }
            else if(stepchild.buttons.counterA > 0 && stepchild.arpeggiator.minVelMod>0){
              stepchild.arpeggiator.minVelMod--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
            break;
          //chance
          case 1:
            if(stepchild.buttons.counterA > 0 && stepchild.arpeggiator.chanceMod<100){
              stepchild.arpeggiator.chanceMod++;
            }
            else if(stepchild.buttons.counterA < 0 && stepchild.arpeggiator.chanceMod>0){
              stepchild.arpeggiator.chanceMod--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
            break;
          //reps
          case 2:
            if(stepchild.buttons.counterA > 0 && stepchild.arpeggiator.repMod<100){
              stepchild.arpeggiator.repMod++;
            }
            else if(stepchild.buttons.counterA < 0 && stepchild.arpeggiator.repMod>0){
              stepchild.arpeggiator.repMod--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
            break;
          case 3:
            if(stepchild.buttons.counterA < 0 && stepchild.arpeggiator.minPitchMod<127){
              stepchild.arpeggiator.minPitchMod++;
            }
            else if(stepchild.buttons.counterA > 0 && stepchild.arpeggiator.minPitchMod>0){
              stepchild.arpeggiator.minPitchMod--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
            break;
          case 4:
            if(stepchild.buttons.counterA < 0){
              if(stepchild.arpeggiator.playStyle > 0)
                stepchild.arpeggiator.playStyle--;
            }
            else{
              if(stepchild.arpeggiator.playStyle < 5)
                stepchild.arpeggiator.playStyle++;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
            break;
        }
      }
      while(stepchild.buttons.counterB != 0){
        switch(cursor){
          case 0:
            if(stepchild.buttons.counterB > 0 && stepchild.arpeggiator.maxVelMod<127){
              stepchild.arpeggiator.maxVelMod++;
            }
            else if(stepchild.buttons.counterB < 0 && stepchild.arpeggiator.maxVelMod>0){
              stepchild.arpeggiator.maxVelMod--;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
            break;
          case 1:
            stepchild.buttons.counterB = 0;
            break;
          case 2:
            stepchild.buttons.counterB = 0;
            break;
          case 3:
            if(stepchild.buttons.counterB > 0 && stepchild.arpeggiator.maxPitchMod<127){
              stepchild.arpeggiator.maxPitchMod++;
            }
            else if(stepchild.buttons.counterB < 0 && stepchild.arpeggiator.maxPitchMod>0){
              stepchild.arpeggiator.maxPitchMod--;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
            break;
        }
      }
    }
    stepchild.display.clearDisplay();
    drawArpMenu(2,0);
    drawModBoxes(cursor);
    stepchild.display.display();
  }
}

void drawArpMenu(uint8_t cursor, int8_t xStart){

  //last note played
  String lastNote = stepchild.pitchToString(stepchild.arpeggiator.lastPitchSent,true,true);
  graphics.printPitch(115-xStart,0,lastNote,false,false, 1);
  if(stepchild.arpeggiator.holding && millis()%800>400){
    graphics.printSmall(106-xStart,9,"[HOLD]",1);
  }
  else if(!stepchild.arpeggiator.holding){
    graphics.printSmall(110-xStart,9,"cpy 2",1);
    graphics.printSmall(112-xStart,15,"hold",1);
  }

  const uint8_t y1 = 15;
  //on/off
  graphics.drawSlider(xStart,y1,"on","off",!stepchild.arpeggiator.isActive);

  //step lengths
  String stepLength = stepchild.arpeggiator.uniformLength?stepchild.stepsToMeasures(stepchild.arpeggiator.arpSubDiv):"custom";
  graphics.drawSlider(xStart,y1+20,"custom","uniform",stepchild.arpeggiator.uniformLength);
  graphics.printSmall(xStart,y1+14,"length:" + stepLength,1);

  //modulation
  graphics.printArp(xStart,52,"MOD",1);

  stepchild.display.fillRect(xStart+20,50,11,11,0);
  graphics.fillSquareVertically(xStart+20,50,11,float(stepchild.arpeggiator.maxVelMod*100)/float(127));
  graphics.printSmall(xStart+24,53,"v",2);

  stepchild.display.fillRect(xStart+33,50,11,11,0);
  graphics.fillSquareVertically(xStart+33,50,11,float(stepchild.arpeggiator.chanceMod));
  graphics.printSmall(xStart+37,53,"%",2);

  stepchild.display.fillRect(xStart+46,50,11,11,0);
  graphics.fillSquareVertically(xStart+46,50,11,float(stepchild.arpeggiator.repMod*100)/float(127));
  graphics.printSmall(xStart+50,53,"x",2);

  stepchild.display.fillRect(xStart+59,50,11,11,0);
  graphics.fillSquareVertically(xStart+59,50,11,float(stepchild.arpeggiator.maxPitchMod*100)/float(127));
  graphics.printSmall(xStart+63,53,"$",2);

  //arp mode icon
  drawArpModeIcon(xStart+72,50,stepchild.arpeggiator.playStyle,1);

  switch(cursor){
    case 0:
      graphics.drawArrow(xStart+29+((millis()/400)%2),y1+5,3,ARROW_LEFT,false);
      break;
    case 1:
      graphics.drawArrow(xStart+61+((millis()/400)%2),y1+25,3,ARROW_LEFT,false);
      break;
    case 2:
      graphics.drawArrow(xStart+82+((millis()/400)%2),y1+40,3,ARROW_LEFT,false);
      break;
  }
  //channel icon
  graphics.drawSmallChannelIcon(93-xStart,1,stepchild.arpeggiator.channel);

  //input icon
  graphics.printSmall(xStart+50,1,"src:",1);
  switch(stepchild.arpeggiator.source){
    //external
    case NOTES_FROM_MIDI_INPUT:
      stepchild.display.drawBitmap(xStart+64,0,tiny_midi_bmp,7,7,1);
      break;
    //internal
    case NOTES_FROM_SEQUENCE:
      stepchild.display.drawBitmap(xStart+64,0,tiny_stepchild_bmp,7,7,1);
      break;
    //both
    case NOTES_FROM_SEQUENCE_AND_MIDI_INPUT:
      stepchild.display.drawBitmap(xStart+64,0,tiny_midi_bmp,7,7,1);
      graphics.printSmall(xStart+72,1,"+",1);
      stepchild.display.drawBitmap(xStart+76,0,tiny_stepchild_bmp,7,7,1);
      break;
  }  
  //title
  graphics.printArp_wiggly(xStart,3,"arpeggi",1);
}

//"notes" option adds notes from the 1scale onto whatever notes are playing
void arpMenu(){
  uint8_t cursor = 0;

  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.trackLabels = false;
  settings.drawPram = false;
  settings.drawTrackChannel = false;
  settings.drawLoopFlags = false;

  //Animating the slide in
  const int8_t startOffset = 88;
  const int8_t animSpeed = 8;
  int8_t xStart = -startOffset;
  int8_t numberOfKeysToDraw = 0;
  
  while(xStart < 0){
    stepchild.display.clearDisplay();
    graphics.drawSeq(settings);
    graphics.ditherBackground(32,16,128,64);
    drawArpMenu(cursor,xStart);
    graphics.drawKeyboard(38,14,3,numberOfKeysToDraw);
    stepchild.display.display();
    xStart+=animSpeed;
    numberOfKeysToDraw = (startOffset+xStart)/(startOffset/14);
  }
  // keyboardAnimation(38,14,0,14,true);

  while(true){
    stepchild.display.clearDisplay();
    graphics.drawSeq(settings);
    graphics.drawKeys(38,14,3,14,false);//always start on a C, for simplicity
    drawArpMenu(cursor,xStart);
    stepchild.display.display();

    //controls
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.joystickY != 0){
        if(stepchild.buttons.joystickY == -1 && cursor>0){
          //jump from arp mode button up to length
          // if(cursor == 3){
          //   cursor = 1;
          // }
          // else{
            cursor--;
          // }
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickY == 1 && cursor<2){
          cursor++;
          stepchild.lastTime = millis();
        }
      }
      if(stepchild.buttons.joystickX != 0){
        switch(cursor){
          //on/off
          case 0:
            if(stepchild.buttons.joystickX == -1 && stepchild.arpeggiator.isActive){
              stepchild.arpeggiator.isActive = false;
              stepchild.lastTime = millis();
            }
            else if(stepchild.buttons.joystickX == 1 && !stepchild.arpeggiator.isActive){
              stepchild.arpeggiator.isActive = true;
              stepchild.lastTime = millis();
            }
            break;
          //step lengths
          case 1:
            if(stepchild.buttons.joystickX == 1 && stepchild.arpeggiator.uniformLength){
              stepchild.arpeggiator.uniformLength = false;
              stepchild.lastTime = millis();
            }
            else if(stepchild.buttons.joystickX == -1 && !stepchild.arpeggiator.uniformLength){
              stepchild.arpeggiator.uniformLength = true;
              stepchild.lastTime = millis();
            }
            break;
          case 2:
            if(stepchild.buttons.joystickX == -1){
              // cursor = 3;
              stepchild.lastTime = millis();
            }
            break;
          case 3:
            if(stepchild.buttons.joystickX == 1){
              cursor = 2;
              stepchild.lastTime = millis();
            }
            break;
        }
      }
      if(stepchild.buttons.SELECT() ){
        switch(cursor){
          //on/off
          case 0:
            stepchild.arpeggiator.isActive = !stepchild.arpeggiator.isActive;
            stepchild.lastTime = millis();
            break;
          //step lengths
          case 1:
            stepchild.lastTime = millis();
            customLengthsMenu();
            break;
          case 2:
            stepchild.lastTime = millis();
            arpModMenu();
            break;
          case 3:
            // stepchild.arpeggiator.playStyle++;
            // stepchild.arpeggiator.playStyle%=6;
            // stepchild.lastTime = millis();
            break;
        }
      }
      //'hold' locks notes that are currently in the arp
      if(stepchild.buttons.COPY()){
        stepchild.arpeggiator.holding = !stepchild.arpeggiator.holding;
        stepchild.lastTime = millis();
      }
      //swap between external/internal input
      if(stepchild.buttons.PLAY() || stepchild.buttons.NEW()){
        stepchild.arpeggiator.source++;
        stepchild.arpeggiator.source%=3;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        stepchild.buttons.setMENU(false) ;
        break;
      }
      while(stepchild.buttons.counterA != 0){
        switch(cursor){
          case 0:
            stepchild.arpeggiator.isActive = !stepchild.arpeggiator.isActive;
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
            break;
          case 1:
            if(stepchild.buttons.SHIFT()){
              stepchild.arpeggiator.arpSubDiv = toggleTriplets(stepchild.arpeggiator.arpSubDiv);
              stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
            }
            else{
              if(stepchild.buttons.counterA > 0){
                stepchild.arpeggiator.arpSubDiv = changeSubDiv(true,stepchild.arpeggiator.arpSubDiv,false);
                stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
              }
              else{
                stepchild.arpeggiator.arpSubDiv = changeSubDiv(false,stepchild.arpeggiator.arpSubDiv,false);
                stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
              }
            }
            break;
          case 2:
            if(stepchild.buttons.counterA > 0){
              stepchild.arpeggiator.playStyle++;
              if(stepchild.arpeggiator.playStyle == 6)
                stepchild.arpeggiator.playStyle = 0;
            }
            else if(stepchild.buttons.counterA < 0){
              if(stepchild.arpeggiator.playStyle == 0)
                stepchild.arpeggiator.playStyle = 5;
              else
                stepchild.arpeggiator.playStyle--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
            break;
        }
      }
      while(stepchild.buttons.counterB != 0){
        if(stepchild.buttons.counterB < 0 && stepchild.arpeggiator.channel>1){
          stepchild.arpeggiator.channel--;
        }
        else if(stepchild.buttons.counterB > 0 && stepchild.arpeggiator.channel<16){
          stepchild.arpeggiator.channel++;
        }
        stepchild.lastTime = millis();
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
      }
    }
  }
  //sliding back out
  while(xStart > -startOffset){
    stepchild.display.clearDisplay();
    graphics.drawSeq(settings);
    drawArpMenu(cursor,xStart);
    graphics.drawKeyboard(38,14,0,numberOfKeysToDraw);
    stepchild.display.display();
    xStart-=animSpeed;
    numberOfKeysToDraw = (startOffset+xStart)/(startOffset/14);
  }
}
