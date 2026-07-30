/*
“settings” needs options for:
- system (memory, overclock, temp)
- display (shrunk, notes/numbers,vel/chance)
- seq (length,presets (sp, piano, sr16)

rethinking this:

  - hardware
    - leds, oled brightness, memory
    - CV on/off, CV voltage limit?
    - update/enter bootsel
    - broadcast screen via USB
  - Sequence
    - presets
    - canvas size
    - display notes/numbers
    - display vel/chance

  "would you like to save these as default settings?" on exit

*/

#include "guiUtilities.h"
#include "Stepchild.h"
#include "menus.h"

#include "graphics/WireFrame.h"


;
extern void webInterface();
using namespace std;

// 'website', 50x50px
const unsigned char web_bmp []  = {
  0x00, 0x03, 0xc3, 0x30, 0x30, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x30, 0x30, 0x00, 0x00, 0x3f, 0xf3, 
  0x30, 0x00, 0xf3, 0xff, 0x00, 0x3f, 0xf3, 0x30, 0x00, 0xf3, 0xff, 0x00, 0x30, 0x33, 0xfc, 0xf3, 
  0x33, 0x03, 0x00, 0x30, 0x33, 0xfc, 0xf3, 0x33, 0x03, 0x00, 0x30, 0x33, 0x33, 0x30, 0xf3, 0x03, 
  0x00, 0x30, 0x33, 0x33, 0x30, 0xf3, 0x03, 0x00, 0x30, 0x33, 0xc0, 0xff, 0x33, 0x03, 0x00, 0x30, 
  0x33, 0xc0, 0xff, 0x33, 0x03, 0x00, 0x3f, 0xf3, 0x3c, 0x00, 0xf3, 0xff, 0x00, 0x3f, 0xf3, 0x3c, 
  0x00, 0xf3, 0xff, 0x00, 0x00, 0x03, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x03, 0x33, 0x33, 0x30, 
  0x00, 0x00, 0xff, 0xff, 0xfc, 0x33, 0x3f, 0xff, 0xc0, 0xff, 0xff, 0xfc, 0x33, 0x3f, 0xff, 0xc0, 
  0x00, 0x30, 0x33, 0x0c, 0x0c, 0xcc, 0xc0, 0x00, 0x30, 0x33, 0x0c, 0x0c, 0xcc, 0xc0, 0xfc, 0xff, 
  0xc0, 0x3c, 0xcc, 0x3f, 0x00, 0xfc, 0xff, 0xc0, 0x3c, 0xcc, 0x3f, 0x00, 0x00, 0x00, 0x0f, 0x00, 
  0xc3, 0x33, 0xc0, 0x00, 0x00, 0x0f, 0x00, 0xc3, 0x33, 0xc0, 0x03, 0x3c, 0x0c, 0xff, 0x3f, 0xff, 
  0xc0, 0x03, 0x3c, 0x0c, 0xff, 0x3f, 0xff, 0xc0, 0x3c, 0x03, 0xcf, 0xfc, 0xf0, 0x0c, 0x00, 0x3c, 
  0x03, 0xcf, 0xfc, 0xf0, 0x0c, 0x00, 0x0c, 0xcf, 0x0f, 0xf0, 0x00, 0x3f, 0xc0, 0x0c, 0xcf, 0x0f, 
  0xf0, 0x00, 0x3f, 0xc0, 0x33, 0x33, 0xf3, 0xcf, 0xc3, 0x30, 0xc0, 0x33, 0x33, 0xf3, 0xcf, 0xc3, 
  0x30, 0xc0, 0x33, 0xcf, 0xcc, 0xcf, 0x3f, 0xfc, 0x00, 0x33, 0xcf, 0xcc, 0xcf, 0x3f, 0xfc, 0x00, 
  0x30, 0xf0, 0xc3, 0x00, 0x00, 0x0f, 0x00, 0x30, 0xf0, 0xc3, 0x00, 0x00, 0x0f, 0x00, 0xff, 0xff, 
  0x0f, 0x3c, 0x3f, 0x3f, 0x00, 0xff, 0xff, 0x0f, 0x3c, 0x3f, 0x3f, 0x00, 0x00, 0x03, 0x0c, 0x00, 
  0x33, 0x30, 0x00, 0x00, 0x03, 0x0c, 0x00, 0x33, 0x30, 0x00, 0x3f, 0xf3, 0xcc, 0xfc, 0x3f, 0x0f, 
  0xc0, 0x3f, 0xf3, 0xcc, 0xfc, 0x3f, 0x0f, 0xc0, 0x30, 0x33, 0x03, 0x03, 0x00, 0x0f, 0xc0, 0x30, 
  0x33, 0x03, 0x03, 0x00, 0x0f, 0xc0, 0x30, 0x33, 0x03, 0xf3, 0x0f, 0x3c, 0x00, 0x30, 0x33, 0x03, 
  0xf3, 0x0f, 0x3c, 0x00, 0x30, 0x33, 0x03, 0xc3, 0xf3, 0x33, 0x00, 0x30, 0x33, 0x03, 0xc3, 0xf3, 
  0x33, 0x00, 0x3f, 0xf3, 0x0c, 0xcf, 0xc0, 0x0c, 0xc0, 0x3f, 0xf3, 0x0c, 0xcf, 0xc0, 0x0c, 0xc0, 
  0x00, 0x03, 0x03, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x03, 0x03, 0xff, 0xff, 0xf0, 0x00
};

// 'save', 12x12px
const unsigned char save_bmp [] = {
	0x7f, 0x80, 0xa2, 0x40, 0xa2, 0x20, 0xbe, 0x10, 0x80, 0x50, 0x80, 0x10, 0xbf, 0xd0, 0xa0, 0x50, 
	0xbf, 0xd0, 0xa0, 0x50, 0xa0, 0x50, 0xff, 0xf0
};
// 'lightbulb', 12x12px
const unsigned char lightbulb_bmp [] = {
	0x4f, 0x20, 0x10, 0x80, 0x20, 0x40, 0x25, 0x40, 0xaa, 0x50, 0x20, 0x40, 0x10, 0x80, 0x0f, 0x00, 
	0x29, 0x40, 0x09, 0x00, 0x09, 0x00, 0x06, 0x00
};

class SettingsMenu:public StepchildMenu{
  public:
    WireFrame wireframe;
    uint8_t menuTab = 0;
    uint8_t xCursor = 0;
    SettingsMenu(){
      wireframe = getSettingsMenuWireFrame();
    }

    void drawSettingsTabs();
    void printMemoryInfo(uint8_t x1, uint8_t y1);
    void printPowerInfo(uint8_t x1, uint8_t y1);
    WireFrame getSettingsMenuWireFrame();
    void animateSettingsMenuWireFrame();
    void drawTemplateOptions(uint8_t, uint8_t);
    void displaySettingsMenu(uint8_t, uint8_t);
    void displaySettingsMenu_selectionBox();
    void displayMenu();
    bool settingsMenuControls();
};


void SettingsMenu::printMemoryInfo(uint8_t x1, uint8_t y1){
  graphics.printCursive(x1,y1-7,"memory",1);
  stepchild.display.fillRect(x1,y1,59,33,0);
  stepchild.display.drawRect(x1,y1,59,33,1);
  graphics.printSmall(x1+2,y1+2,"used-->"+stringify(rp2040.getUsedHeap())+"B",1);
  graphics.printSmall(x1+22,y1+10,"("+stringify(float(rp2040.getUsedHeap())/float(rp2040.getTotalHeap())*100)+"%)",1);
  graphics.printSmall(x1+2,y1+18,"free-->"+stringify(rp2040.getFreeHeap())+"B",1);
  graphics.printSmall(x1+22,y1+26,"("+stringify(float(rp2040.getFreeHeap())/float(rp2040.getTotalHeap())*100)+"%)",1);
}

void SettingsMenu::printPowerInfo(uint8_t x1, uint8_t y1){
  String s = stringify(getBattLevel());
  graphics.drawLabel(x1,y1,"vsys: "+s+"v",true);
}

void SettingsMenu::displaySettingsMenu_selectionBox(){
  displaySettingsMenu(0,0);
}

WireFrame SettingsMenu::getSettingsMenuWireFrame(){
  WireFrame w;
  switch(menuTab){
    //gear
    case 0:
      w = makeThickGear(10,8,8,40,true);
      w.offset.x = 100;
      w.offset.y = 10;
      w.scale = 4;
      w.rotate(-30,1);
      break;
    //pram
    case 1:
      w = makePram();
      w.offset.x = 105;
      w.offset.y = 30;
      w.scale = 1.8;
      break;
    //computer
    case 2:
      w = makeMonitor();
      w.offset.x = 100;
      w.offset.y = 40;
      w.scale = 4;
      w.rotate(-30,1);
      w.rotate(-10,0);
      break;
  }
  return w;
}

void SettingsMenu::animateSettingsMenuWireFrame(){
  switch(menuTab){
    case 0:
      wireframe.rotate(2,2);
      break;
    case 1:
      wireframe.rotate(2,1);
      break;
    case 2:
      animateMonitor(wireframe,5.0,23);
      break;
  }
}

bool SettingsMenu::settingsMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.joystickY != 0){
      if(xCursor == 0){
          if(stepchild.buttons.joystickY == 1 && menuTab<2){
            menuTab++;
            stepchild.lastTime = millis();
            wireframe = getSettingsMenuWireFrame();
            xCursor = 0;
          }
          else if(stepchild.buttons.joystickY == -1 && menuTab>0){
            menuTab--;
            stepchild.lastTime = millis();
            wireframe = getSettingsMenuWireFrame();
            xCursor = 0;
          }
      }
      else{
        switch(menuTab){
          case 0:
            if(stepchild.buttons.joystickY == 1){
              if(xCursor<6){
                xCursor++;
                stepchild.lastTime = millis();
              } 
            }
            else if(stepchild.buttons.joystickY == -1){
              if(xCursor >  5){
                xCursor = 5;
                stepchild.lastTime = millis();
              }
              else if(xCursor > 0){
                xCursor--;
                stepchild.lastTime = millis();
              }
            }
            break;
          case 1:
            break;
          //PC/interface
          case 2:
            if(stepchild.buttons.joystickY){
              xCursor = xCursor == 1?2:1;
              stepchild.lastTime = millis();
            }
            break;
        }
      }
    }
    if(stepchild.buttons.joystickX != 0){
      switch(menuTab){
        //seq
        case 0:
          if(stepchild.buttons.joystickX == -1){
            if(xCursor == 0){
              xCursor++;
              stepchild.lastTime = millis();
            }
            else if(xCursor<6){
              xCursor = 8;
              stepchild.lastTime = millis();
            }
            else if(xCursor<9){
              xCursor++;
              stepchild.lastTime = millis();
            }
          }
          else if(stepchild.buttons.joystickX == 1){
            if( xCursor<6){
              xCursor = 0;
              stepchild.lastTime = millis();
            }
            else{
              xCursor--;
              stepchild.lastTime = millis();
            }
          }
          break;
        //sys
        case 1:
          if(stepchild.buttons.joystickX == -1 && xCursor<2){
            xCursor++;
            stepchild.lastTime = millis();
          }
          else if(stepchild.buttons.joystickX == 1 && xCursor>0){
            xCursor--;
            stepchild.lastTime = millis();
          }
          break;
        //interface
        case 2:
          if(stepchild.buttons.joystickX == -1 && xCursor == 0){
            xCursor = 1;
            stepchild.lastTime = millis();
          }
          else if(stepchild.buttons.joystickX == 1 && xCursor > 0){
            xCursor = 0;
            stepchild.lastTime = millis();
          }
          break;
      }
    }
    if(stepchild.buttons.MENU()){
      stepchild.buttons.setMENU(false) ;
      stepchild.lastTime = millis();
      return false;
    }
    bool changedBrightness = false;
    while(stepchild.buttons.counterA != 0){
      switch(menuTab){
        case 0:
          //changing screen brightness
          if(xCursor == 8){
            if(stepchild.buttons.counterA<0){
              if(stepchild.buttons.SHIFT() && stepchild.screenBrightness>0){
                stepchild.screenBrightness--;
                changedBrightness = true;
              }
              else if(!stepchild.buttons.SHIFT() && stepchild.screenBrightness>16){
                stepchild.screenBrightness-=16;
                changedBrightness = true;
              }
              else if(stepchild.screenBrightness != 0){
                stepchild.screenBrightness = 0;
                changedBrightness = true;
              }
            }
            else if(stepchild.buttons.counterA>0){
              if(stepchild.buttons.SHIFT() && stepchild.screenBrightness<255){
                stepchild.screenBrightness++;
                changedBrightness = true;
              }
              else if(stepchild.screenBrightness<239){
                stepchild.screenBrightness+=16;
                changedBrightness = true;
              }
              else if(stepchild.screenBrightness != 255){
                stepchild.screenBrightness = 255;
                changedBrightness = true;
              }
            }
          }
          break;
      }
      stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
    }
    if(changedBrightness){
      stepchild.display.ssd1306_command(SSD1306_SETCONTRAST);
      stepchild.display.ssd1306_command(stepchild.screenBrightness);
    }
    if(stepchild.buttons.SELECT() ){
      switch(menuTab){
        //seq
        case 0:
          switch(xCursor){
            //showing pitches
            case 1:
              stepchild.pitchesOrNumbers = !stepchild.pitchesOrNumbers;
              stepchild.lastTime = millis();
              break;
            //leds on/off
            case 2:
              stepchild.buttons.LEDsActive = !stepchild.buttons.LEDsActive;
              stepchild.lastTime = millis();
              break;
            case 3:
              stepchild.waitForNoteBeforeRec = !stepchild.waitForNoteBeforeRec;
              stepchild.lastTime = millis();
              break;
            //overwrite
            case 4:
              stepchild.overwriteRecording = !stepchild.overwriteRecording;
              stepchild.lastTime = millis();
              break;
            //rec mode
            case 5:
              stepchild.recMode = static_cast<RecordingMode>(uint8_t(stepchild.recMode)+1);
              stepchild.recMode = static_cast<RecordingMode>(uint8_t(stepchild.recMode)%4);
              stepchild.lastTime = millis();
              break;
            //remove time
            case 6:{
              stepchild.lastTime = millis();
              int8_t choice = 1;
              //if there are notes that will be deld
              uint16_t countedNotes = stepchild.countNotesInRange(stepchild.sequenceLength-96,stepchild.sequenceLength);
              if(countedNotes)
                choice = binarySelectionBox(64,32,"naur","sure","this will del "+stringify(countedNotes)+" note(s), ok?");
              if(choice == 1){
                  stepchild.removeTimeFromSeq(96,stepchild.sequenceLength-96);
              }
              stepchild.lastTime = millis();
              }
              break;
            //add time
            case 7:
              stepchild.addTimeToSeq(96,stepchild.sequenceLength);
              stepchild.lastTime = millis();
              break;
            //brightness
            case 8:
              break;
            //load/write to flash
            case 9:
              if(stepchild.buttons.SHIFT()){
                stepchild.filesystem.loadSettings();
                stepchild.lastTime = millis();
              }
              else{
                stepchild.filesystem.writeCurrentSettingsToFile();
                alert("saved!",500);
                stepchild.lastTime = millis();
              }
              break;
          }
          break;
        //system
        case 1:
          switch(xCursor){
            //enter update mode 
            case 1:
              stepchild.display.drawBitmap(39,7,web_bmp,50,50,SSD1306_WHITE);
              enterBootsel();
              break;
            //enter bottsel
            case 2:
              enterBootsel();
              break;
          }
          break;
        //interface
        case 2:
          switch(xCursor){
            case 1:
              webInterface();
              break;
            //toggle screen sharing
            case 2:
              // stepchild.display.sendScreenViaUSB = !stepchild.display.sendScreenViaUSB;
              stepchild.lastTime = millis();
              break;
          }
          break;
      }
    }
  }
  return true;
}

void SettingsMenu::displayMenu(){
  animateSettingsMenuWireFrame();
  stepchild.display.clearDisplay();
  wireframe.render();
  displaySettingsMenu(0,0);
  stepchild.display.display();
}

void settingsMenu(){
  SettingsMenu settingsMenu;
  while(settingsMenu.settingsMenuControls()){
    settingsMenu.displayMenu();
  }
}

void SettingsMenu::drawTemplateOptions(uint8_t x1,uint8_t whichTemplate){
  stepchild.display.fillRect(x1,0,stepchild.SCREEN_WIDTH-x1,stepchild.SCREEN_HEIGHT,0);
  stepchild.display.drawFastVLine(x1,0,stepchild.SCREEN_HEIGHT,1);
  graphics.printSmall(x1+4,2,"templates",1);
  graphics.printSmall(x1+6,12,"basic",1);
  graphics.drawCheckbox(x1+30,11,whichTemplate == 0,cursor == 0);
  graphics.printSmall(x1+6,22,"404",1);
  graphics.drawCheckbox(x1+30,21,whichTemplate == 1,cursor == 1);
  graphics.printSmall(x1+6,32,"4track",1);
  graphics.drawCheckbox(x1+30,31,whichTemplate == 2,cursor == 2);
}

// 'connect_to_interface', 20x20px
const unsigned char connect_to_interface_bmp [] = {
	0x07, 0xf8, 0x00, 0x18, 0x04, 0x00, 0x60, 0x1c, 0x00, 0x80, 0x64, 0xc0, 0xe1, 0x95, 0x20, 0x96, 
	0x75, 0x20, 0x89, 0xf6, 0x40, 0x8b, 0xf4, 0x80, 0x8b, 0xe4, 0xb0, 0x8b, 0x98, 0x70, 0x8a, 0x60, 
	0x30, 0x69, 0x80, 0x00, 0x1e, 0x00, 0x80, 0x08, 0x03, 0xc0, 0x00, 0x0f, 0xe0, 0x00, 0x3f, 0xc0, 
	0x00, 0xff, 0x00, 0x01, 0xfc, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x40, 0x00
};
// 'screen_capture_2', 24x24px
const unsigned char screen_capture_bmp [] = {
	0xff, 0xf0, 0x00, 0x80, 0x10, 0x00, 0xb9, 0x90, 0x00, 0xb3, 0x10, 0x00, 0xa6, 0x50, 0x00, 0x80, 
	0x10, 0x00, 0xff, 0xf0, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 0x81, 0xe0, 0x00, 0x82, 0x10, 
	0x00, 0x82, 0x08, 0x00, 0x44, 0x08, 0x00, 0x38, 0xff, 0xf0, 0x00, 0x80, 0x10, 0x00, 0xb9, 0x90, 
	0x00, 0xb3, 0x10, 0x00, 0xa6, 0x50, 0x00, 0x80, 0x10, 0x00, 0xff, 0xf0
};

void SettingsMenu::displaySettingsMenu(uint8_t x2,uint8_t whichTemplate){
  drawSettingsTabs();
  switch(menuTab){
    //sequence
    /*
    - length
    - button to shorten, button to cut a portion, button to extend
    - number of notes
    - number of tracks
    - name
    */
    case 0:
    {
      const uint8_t x1 = 30;
      const uint8_t y1 = 2;

      graphics.printSmall(x1-5,y1,"track labels:",1);
      graphics.drawLabel(x1+62,y1,stepchild.pitchesOrNumbers?"pitches":"numbers",true);
      
      graphics.printSmall(x1-5,y1+8,"leds:",1);
      graphics.drawLabel(x1+22,y1+8,stepchild.buttons.LEDsActive?"on":"off",stepchild.buttons.LEDsActive);

      stepchild.display.drawFastHLine(x1-7,y1+16,53,1);
      graphics.printSmall(x1+47,y1+14,"rec",1);

      graphics.printSmall(x1-5,y1+20,"wait:",1);
      graphics.drawLabel(x1+22,y1+20,stepchild.waitForNoteBeforeRec?"on":"off",stepchild.waitForNoteBeforeRec);

      graphics.printSmall(x1-5,y1+28,"overwrite:",1);
      graphics.drawLabel(x1+42,y1+28,stepchild.overwriteRecording?"on":"off",stepchild.overwriteRecording);

      graphics.printSmall(x1-5,y1+36,"rec mode:",1);
      graphics.drawLabel(x1+50,y1+36,stepchild.recMode?"continuous":"1-shot",true);
      
      //length
      graphics.printSmall(x1+3,y1+48,"length -- "+stepchild.stepsToMeasures(stepchild.sequenceLength),1);
      graphics.printSmall(x1+2,y1+56,"("+stringify(stepchild.sequenceLength)+" steps)",1);
      graphics.drawArrow(x1-12,y1+54,6,ARROW_LEFT,xCursor == 6);
      graphics.drawArrow(x1+56,y1+54,6,ARROW_RIGHT,xCursor == 7);

      //lightbulb
      stepchild.display.drawBitmap(94,52,lightbulb_bmp,12,12,2);

      //save
      stepchild.display.drawBitmap(111,52,save_bmp,12,12,2);

      switch(xCursor){
        case 0:
          graphics.drawArrow(14+((millis()/400)%2),9,2,ARROW_LEFT,true);
          break;
        case 1:
        case 2:
          graphics.drawArrow(23+((millis()/400)%2),(xCursor-1)*8+y1+2,2,ARROW_RIGHT,false);
          break;
        case 3:
        case 4:
        case 5:
          graphics.drawArrow(23+((millis()/400)%2),(xCursor-1)*8+y1+6,2,ARROW_RIGHT,false);
          break;
        //remove time
        case 6:
          graphics.drawLabel(x1+24,y1+40,"remove time",false);
          break;
        //add time
        case 7:
          graphics.drawLabel(x1+24,y1+40,"add time",false);
          break;
        //brightness
        case 8:{
          graphics.drawArrow(99,50+((millis()/400)%2),2,ARROW_DOWN,true);
          String s = "light: "+stringify(stepchild.screenBrightness);
          stepchild.display.fillRoundRect(88,41,s.length()*4+2,7,3,1);
          graphics.printSmall(90,42,s,0);
          }
          break;
        //save
        case 9:
          graphics.drawArrow(116,50+((millis()/400)%2),2,ARROW_DOWN,true);
          graphics.drawLabel(116,43,stepchild.buttons.SHIFT()?"load":"save",true);
          break;
      }
    }
      break;
    //system
    case 1:
    {
      const uint8_t x1 = 20;
      const uint8_t y1 = 6;
      printMemoryInfo(x1,y1);
      printPowerInfo(105,1);
      graphics.printSmall(x1,y1+35,"temp: "+stringify(analogReadTemp())+"}C",1);
      graphics.printSmall(x1,y1+43,"cpu speed: "+stringify(float(rp2040.f_cpu())/float(1000000))+"MHZ",1);
      if(xCursor == 0){
        graphics.drawArrow(14+((millis()/200)%2),26,2,ARROW_LEFT,false);
        stepchild.display.drawRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+6,y1+52,"U^",1);
        stepchild.display.drawRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+21,y1+52,"B&",1);
        graphics.printItalic(64,55,"CPU/MEM",1);
      }
      else if(xCursor == 1){
        stepchild.display.fillRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+6,y1+52,"U^",0);
        stepchild.display.drawRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+21,y1+52,"B&",1);
        graphics.printSmall(x1+32,y1+52,"enter update mode",1);
      }
      else if(xCursor == 2){
        stepchild.display.drawRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+6,y1+52,"U^",1);
        stepchild.display.fillRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        graphics.printSmall(x1+21,y1+52,"B&",0);
        graphics.printSmall(x1+32,y1+52,"enter bootsel mode",1);
      }
    }
      break;
    //interface
    case 2:
    /*
    - button to start screen capture
    - button to connect to interface app (go into interface mode)
    */
    {
      graphics.printItalic(44,0,"COMPUTER",1);
      switch(xCursor){
        case 0:
          graphics.drawArrow(14+((millis()/200)%2),41,2,ARROW_LEFT,false);
          break;
        //interface
        case 1:
          stepchild.display.fillRoundRect(18,30,24,24,3,1);
          graphics.drawArrow(44+((millis()/200)%2),43,4,ARROW_LEFT,true);
          graphics.printSmall(0,59,"Connect to Web Interface",1);
          break;
        //screen sharing
        case 2:
          // stepchild.display.fillRoundRect(18,2+(stepchild.display.sendScreenViaUSB?(millis()/100%2):0),24,24,3,1);
          graphics.drawArrow(44+((millis()/200)%2),14,4,ARROW_LEFT,true);
          graphics.printSmall(0,59,"share screen via USB",1);
          break;
      }
      // stepchild.display.drawBitmap(20,4+(stepchild.display.sendScreenViaUSB?(millis()/100%2):0),screen_capture_bmp,20,20,2);
      stepchild.display.drawBitmap(20,32,connect_to_interface_bmp,20,20,2);
    }
      break;
  }
}

void SettingsMenu::drawSettingsTabs(){
  stepchild.display.setRotation(3);
  //small tab if inactive
  if(menuTab != 0){
    uint8_t x1 = 0;
    uint8_t y1 = 124;
    stepchild.display.drawFastHLine(x1,y1,2,1);
    stepchild.display.drawFastVLine(x1+2,y1-6,7,1);
    stepchild.display.drawFastHLine(x1+2,y1-6,14,1);
    graphics.printSmall(x1+4,120,"seq",1);
    stepchild.display.drawFastVLine(x1+16,y1-6,7,1);
    stepchild.display.drawFastHLine(x1+17,y1,2,1);
  }
  //big if active
  else{
    uint8_t x1 = 0;
    uint8_t y1 = 124;
    stepchild.display.drawFastHLine(x1,y1,2,1);
    stepchild.display.drawFastVLine(x1+2,y1-8,9,1);
    stepchild.display.drawFastHLine(x1+2,y1-8,14,1);
    graphics.printSmall(x1+4,y1-6,"seq",1);
    stepchild.display.drawFastVLine(x1+16,y1-8,9,1);
    stepchild.display.drawFastHLine(x1+17,y1,2,1);
  }
  if(menuTab != 1){
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    stepchild.display.drawFastVLine(x1,y1-6,7,1);
    stepchild.display.drawFastHLine(x1,y1-6,14,1);
    graphics.printSmall(x1+2,120,"sys",1);
    stepchild.display.drawFastVLine(x1+14,y1-6,7,1);
    stepchild.display.drawFastHLine(x1+15,y1,2,1);
  }
  else{
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    stepchild.display.drawFastVLine(x1,y1-8,9,1);
    stepchild.display.drawFastHLine(x1,y1-8,14,1);
    graphics.printSmall(x1+2,y1-6,"sys",1);
    stepchild.display.drawFastVLine(x1+14,y1-8,9,1);
    stepchild.display.drawFastHLine(x1+15,y1,2,1);
  }
  if(menuTab != 2){
    uint8_t x1 = 36;
    uint8_t y1 = 124;
    stepchild.display.drawFastVLine(x1,y1-6,7,1);
    stepchild.display.drawFastHLine(x1,y1-6,10,1);
    graphics.printSmall(x1+2,y1-4,"PC",1);
    stepchild.display.drawFastVLine(x1+10,y1-6,7,1);
    stepchild.display.drawFastHLine(x1+11,y1,8,1);
  }
  else{
    uint8_t x1 = 36;
    uint8_t y1 = 124;
    stepchild.display.drawFastVLine(x1,y1-8,9,1);
    stepchild.display.drawFastHLine(x1,y1-8,11,1);
    graphics.printSmall(x1+2,y1-6,"PC",1);
    stepchild.display.drawFastVLine(x1+10,y1-8,9,1);
    stepchild.display.drawFastHLine(x1+11,y1,8,1);
  }
  stepchild.display.setRotation(2);
}
