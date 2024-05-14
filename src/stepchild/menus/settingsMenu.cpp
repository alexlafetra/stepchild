/*
“settings” needs options for:
- system (memory, overclock, temp)
- display (shrunk, notes/numbers,vel/chance)
- seq (length,presets (sp, piano, sr16)
*/

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

void drawSettingsTabs(uint8_t);

void printMemoryInfo(uint8_t x1, uint8_t y1){
  printCursive(x1,y1-7,"memory",1);
  display.fillRect(x1,y1,59,33,0);
  display.drawRect(x1,y1,59,33,1);
  printSmall(x1+2,y1+2,"used-->"+stringify(rp2040.getUsedHeap())+"B",1);
  printSmall(x1+22,y1+10,"("+stringify(float(rp2040.getUsedHeap())/float(rp2040.getTotalHeap())*100)+"%)",1);
  printSmall(x1+2,y1+18,"free-->"+stringify(rp2040.getFreeHeap())+"B",1);
  printSmall(x1+22,y1+26,"("+stringify(float(rp2040.getFreeHeap())/float(rp2040.getTotalHeap())*100)+"%)",1);
}

void printPowerInfo(uint8_t x1, uint8_t y1){
  String s = stringify(getBattLevel());
  graphics.drawLabel(x1,y1,"vsys: "+s+"v",true);
}

void displaySettingsMenu_selectionBox(){
    activeMenu.displaySettingsMenu(0,7,0,0);
}

WireFrame getSettingsMenuWireFrame(uint8_t which){
  WireFrame w;
  switch(which){
    //gear
    case 0:
      w = makeThickGear(10,8,8,40,true);
      w.xPos = 100;
      w.yPos = 10;
      w.scale = 4;
      w.rotate(-30,1);
      break;
    //pram
    case 1:
      w = makePram();
      w.xPos = 100;
      w.yPos = 20;
      w.scale = 4;
      break;
    //computer
    case 2:
      w = makeMonitor();
      w.xPos = 100;
      w.yPos = 30;
      w.scale = 4;
      w.rotate(-30,1);
      w.rotate(-10,0);
      break;
  }
  return w;
}

void animateSettingsMenuWireFrame(WireFrame &w,uint8_t which){
  switch(which){
    case 0:
      w.rotate(2,2);
      break;
    case 1:
      w.rotate(2,1);
      break;
    case 2:
      w.yPos = 20+8.0*sin(millis()/600.0);
      w.rotate(PI/6.0*sin(millis()/400),1);
      break;
  }
}
void settingsMenu(){
  //which 'tab' you're looking at
  uint8_t menuTab = 0;
  //the cursor
  uint8_t xCursor = 0;
  WireFrame gear = getSettingsMenuWireFrame(menuTab);
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.joystickY != 0){
        if(xCursor == 0){
            if(controls.joystickY == 1 && menuTab<2){
              menuTab++;
              lastTime = millis();
              gear = getSettingsMenuWireFrame(menuTab);
            }
            else if(controls.joystickY == -1 && menuTab>0){
              menuTab--;
              lastTime = millis();
              gear = getSettingsMenuWireFrame(menuTab);
            }
        }
        else{
          switch(menuTab){
            case 0:
              if(controls.joystickY == 1){
                if(xCursor<6){
                  xCursor++;
                  lastTime = millis();
                } 
              }
              else if(controls.joystickY == -1){
                if(xCursor >  5){
                  xCursor = 5;
                  lastTime = millis();
                }
                else if(xCursor > 0){
                  xCursor--;
                  lastTime = millis();
                }
              }
              break;
            case 1:
              break;
            case 2:
              break;
          }
        }
      }
      if(controls.joystickX != 0){
        switch(menuTab){
          //seq
          case 0:
            if(controls.joystickX == -1){
              if(xCursor == 0){
                xCursor++;
                lastTime = millis();
              }
              else if(xCursor<6){
                xCursor = 8;
                lastTime = millis();
              }
              else if(xCursor<9){
                xCursor++;
                lastTime = millis();
              }
            }
            else if(controls.joystickX == 1){
              if( xCursor<6){
                xCursor = 0;
                lastTime = millis();
              }
              else{
                xCursor--;
                lastTime = millis();
              }
            }
            break;
          //sys
          case 1:
            if(controls.joystickX == -1 && xCursor<2){
              xCursor++;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && xCursor>0){
              xCursor--;
              lastTime = millis();
            }
            break;
          //interface
          case 2:
              if(controls.joystickX == -1 && xCursor<1){
              xCursor++;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && xCursor>0){
              xCursor--;
              lastTime = millis();
            }
            break;
        }
      }
      if(controls.MENU()){
        controls.setMENU(false) ;
        lastTime = millis();
        // constructMenu("MENU");
        return;
      }
      bool changedBrightness = false;
      while(controls.counterA != 0){
        switch(menuTab){
          case 0:
            //changing screen brightness
            if(xCursor == 8){
              if(controls.counterA<0){
                if(controls.SHIFT() && screenBrightness>0){
                  screenBrightness--;
                  changedBrightness = true;
                }
                else if(!controls.SHIFT() && screenBrightness>16){
                  screenBrightness-=16;
                  changedBrightness = true;
                }
                else if(screenBrightness != 0){
                  screenBrightness = 0;
                  changedBrightness = true;
                }
              }
              else if(controls.counterA>0){
                if(controls.SHIFT() && screenBrightness<255){
                  screenBrightness++;
                  changedBrightness = true;
                }
                else if(screenBrightness<239){
                  screenBrightness+=16;
                  changedBrightness = true;
                }
                else if(screenBrightness != 255){
                  screenBrightness = 255;
                  changedBrightness = true;
                }
              }
            }
            break;
        }
        controls.counterA += controls.counterA<0?1:-1;
      }
      if(changedBrightness){
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(screenBrightness);
      }
      if(controls.SELECT() ){
        switch(menuTab){
          //seq
          case 0:
            switch(xCursor){
              //showing pitches
              case 1:
                pitchesOrNumbers = !pitchesOrNumbers;
                lastTime = millis();
                break;
              //leds on/off
              case 2:
                LEDsOn = !LEDsOn;
                updateLEDs();
                lastTime = millis();
                break;
              case 3:
                waitForNoteBeforeRec = !waitForNoteBeforeRec;
                lastTime = millis();
                break;
              //overwrite
              case 4:
                overwriteRecording = !overwriteRecording;
                lastTime = millis();
                break;
              //rec mode
              case 5:
                recMode++;
                recMode%=4;
                lastTime = millis();
                break;
              //remove time
              case 6:{
                lastTime = millis();
                int8_t choice = 1;
                //if there are notes that will be deld
                uint16_t countedNotes = countNotesInRange(sequence.sequenceLength-96,sequence.sequenceLength);
                if(countedNotes)
                  choice = binarySelectionBox(64,32,"naur","sure","this will del "+stringify(countedNotes)+" note(s), ok?",displaySettingsMenu_selectionBox);
                if(choice == 1){
                    sequence.removeTimeFromSeq(96,sequence.sequenceLength-96);
                }
                lastTime = millis();
                }
                break;
              //add time
              case 7:
                    sequence.addTimeToSeq(96,sequence.sequenceLength);
                lastTime = millis();
                break;
              //brightness
              case 8:
                break;
              //load/write to flash
              case 9:
                if(controls.SHIFT()){
                  loadSettings();
                  lastTime = millis();
                }
                else{
                  writeCurrentSettingsToFile();
                  alert("saved!",500);
                  lastTime = millis();
                }
                break;
            }
            break;
          //system
          case 1:
            switch(xCursor){
              //enter update mode 
              case 1:
                display.drawBitmap(39,7,web_bmp,50,50,SSD1306_WHITE);
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
            }
            break;
        }
      }
    }
    display.clearDisplay();
    gear.render();
    activeMenu.displaySettingsMenu(menuTab,xCursor,0,0);
    display.display();
    animateSettingsMenuWireFrame(gear,menuTab);
  }
}
void drawTemplateOptions(uint8_t x1,uint8_t whichTemplate){
  display.fillRect(x1,0,screenWidth-x1,screenHeight,0);
  display.drawFastVLine(x1,0,screenHeight,1);
  printSmall(x1+4,2,"templates",1);
  printSmall(x1+6,12,"basic",1);
  graphics.drawCheckbox(x1+30,11,whichTemplate == 0,activeMenu.highlight == 0);
  printSmall(x1+6,22,"404",1);
  graphics.drawCheckbox(x1+30,21,whichTemplate == 1,activeMenu.highlight == 1);
  printSmall(x1+6,32,"4track",1);
  graphics.drawCheckbox(x1+30,31,whichTemplate == 2,activeMenu.highlight == 2);
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

void Menu::displaySettingsMenu(uint8_t whichMenu,uint8_t cursor,uint8_t x2,uint8_t whichTemplate){
  drawSettingsTabs(whichMenu);
  switch(whichMenu){
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

      printSmall(x1-5,y1,"track labels:",1);
      graphics.drawLabel(x1+62,y1,pitchesOrNumbers?"pitches":"numbers",true);
      
      printSmall(x1-5,y1+8,"leds:",1);
      graphics.drawLabel(x1+22,y1+8,LEDsOn?"on":"off",LEDsOn);

      display.drawFastHLine(x1-7,y1+16,53,1);
      printSmall(x1+47,y1+14,"rec",1);

      printSmall(x1-5,y1+20,"wait:",1);
      graphics.drawLabel(x1+22,y1+20,waitForNoteBeforeRec?"on":"off",waitForNoteBeforeRec);

      printSmall(x1-5,y1+28,"overwrite:",1);
      graphics.drawLabel(x1+42,y1+28,overwriteRecording?"on":"off",overwriteRecording);

      printSmall(x1-5,y1+36,"rec mode:",1);
      graphics.drawLabel(x1+50,y1+36,recMode?"continuous":"1-shot",true);
      
      //length
      printSmall(x1+3,y1+48,"length -- "+stepsToMeasures(sequence.sequenceLength),1);
      printSmall(x1+2,y1+56,"("+stringify(sequence.sequenceLength)+" steps)",1);
      graphics.drawArrow(x1-12,y1+54,6,1,cursor == 6);
      graphics.drawArrow(x1+56,y1+54,6,0,cursor == 7);

      //lightbulb
      display.drawBitmap(94,52,lightbulb_bmp,12,12,2);

      //save
      display.drawBitmap(111,52,save_bmp,12,12,2);

      switch(cursor){
        case 0:
          graphics.drawArrow(14+((millis()/400)%2),9,2,1,true);
          break;
        case 1:
        case 2:
          graphics.drawArrow(23+((millis()/400)%2),(cursor-1)*8+y1+2,2,0,false);
          break;
        case 3:
        case 4:
        case 5:
          graphics.drawArrow(23+((millis()/400)%2),(cursor-1)*8+y1+6,2,0,false);
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
          graphics.drawArrow(99,50+((millis()/400)%2),2,3,true);
          String s = "light: "+stringify(screenBrightness);
          display.fillRoundRect(88,41,s.length()*4+2,7,3,1);
          printSmall(90,42,s,0);
          }
          break;
        //save
        case 9:
          graphics.drawArrow(116,50+((millis()/400)%2),2,3,true);
          graphics.drawLabel(116,43,controls.SHIFT()?"load":"save",true);
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
      printSmall(x1,y1+35,"temp: "+stringify(analogReadTemp())+"}C",1);
      printSmall(x1,y1+43,"cpu speed: "+stringify(float(rp2040.f_cpu())/float(1000000))+"MHZ",1);
      if(cursor == 0){
        graphics.drawArrow(14+((millis()/200)%2),26,2,1,false);
        display.drawRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        printSmall(x1+6,y1+52,"U^",1);
        display.drawRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        printSmall(x1+21,y1+52,"B&",1);
      }
      else if(cursor == 1){
        display.fillRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        printSmall(x1+6,y1+52,"U^",0);
        display.drawRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        printSmall(x1+21,y1+52,"B&",1);
        printSmall(x1+32,y1+52,"enter update mode",1);
      }
      else if(cursor == 2){
        display.drawRoundRect(x1,y1+50,16,10,3,SSD1306_WHITE);
        printSmall(x1+6,y1+52,"U^",1);
        display.fillRoundRect(x1+18,y1+50,12,10,3,SSD1306_WHITE);
        printSmall(x1+21,y1+52,"B&",0);
        printSmall(x1+32,y1+52,"enter bootsel mode",1);
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
      display.drawBitmap(20,6,screen_capture_bmp,20,20,1);
      display.drawBitmap(20,30,connect_to_interface_bmp,20,20,1);
      switch(cursor){
        case 0:
          graphics.drawArrow(14+((millis()/200)%2),46,2,1,false);
          break;
        case 1:
          break;
        case 2:
          break;
      }
    }
      break;
  }
}

void drawSettingsTabs(uint8_t openTab){
  display.setRotation(3);
  //small tab if inactive
  if(openTab != 0){
    uint8_t x1 = 0;
    uint8_t y1 = 124;
    display.drawFastHLine(x1,y1,2,1);
    display.drawFastVLine(x1+2,y1-6,7,1);
    display.drawFastHLine(x1+2,y1-6,14,1);
    printSmall(x1+4,120,"seq",1);
    display.drawFastVLine(x1+16,y1-6,7,1);
    display.drawFastHLine(x1+17,y1,2,1);
  }
  //big if active
  else{
    uint8_t x1 = 0;
    uint8_t y1 = 124;
    display.drawFastHLine(x1,y1,2,1);
    display.drawFastVLine(x1+2,y1-8,9,1);
    display.drawFastHLine(x1+2,y1-8,14,1);
    printSmall(x1+4,y1-6,"seq",1);
    display.drawFastVLine(x1+16,y1-8,9,1);
    display.drawFastHLine(x1+17,y1,2,1);
  }
  if(openTab != 1){
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-6,7,1);
    display.drawFastHLine(x1,y1-6,14,1);
    printSmall(x1+2,120,"sys",1);
    display.drawFastVLine(x1+14,y1-6,7,1);
    display.drawFastHLine(x1+15,y1,2,1);
  }
  else{
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-8,9,1);
    display.drawFastHLine(x1,y1-8,14,1);
    printSmall(x1+2,y1-6,"sys",1);
    display.drawFastVLine(x1+14,y1-8,9,1);
    display.drawFastHLine(x1+15,y1,2,1);
  }
  if(openTab != 2){
    uint8_t x1 = 36;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-6,7,1);
    display.drawFastHLine(x1,y1-6,18,1);
    printSmall(x1+2,y1-4,"intr",1);
    display.drawFastVLine(x1+18,y1-6,7,1);
    display.drawFastHLine(x1+19,y1,2,1);
  }
  else{
    uint8_t x1 = 36;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-8,9,1);
    display.drawFastHLine(x1,y1-8,18,1);
    printSmall(x1+2,y1-6,"intr",1);
    display.drawFastVLine(x1+18,y1-8,9,1);
    display.drawFastHLine(x1+19,y1,2,1);
  }
  display.drawFastHLine(57,124,24,1);
  display.setRotation(2);
}
