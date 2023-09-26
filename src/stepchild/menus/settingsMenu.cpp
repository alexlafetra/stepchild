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

void settingsMenu(){
  //which 'tab' you're looking at
  uint8_t menuTab = 0;
  //the cursor
  uint8_t xCursor = 0;
  WireFrame gear = makeThickGear(10,8,8,40,true);
  gear.xPos = 100;
  gear.yPos = 10;
  gear.scale = 4;
  gear.rotate(-30,1);
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      if(y != 0){
        if(xCursor == 0){
            if(y == 1 && menuTab<1){
              menuTab++;
              lastTime = millis();
            }
            else if(y == -1 && menuTab>0){
              menuTab--;
              lastTime = millis();
            }
        }
        else{
          switch(menuTab){
            case 0:
              if(y == 1){
                if(xCursor<7){
                  xCursor++;
                  lastTime = millis();
                } 
              }
              else if(y == -1){
                if(xCursor >  6){
                  xCursor = 6;
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
      if(x != 0){
        switch(menuTab){
          //seq
          case 0:
            if(x == -1){
              if(xCursor == 0){
                xCursor++;
                lastTime = millis();
              }
              else if(xCursor<7){
                xCursor = 9;
                lastTime = millis();
              }
              else if(xCursor<10){
                xCursor++;
                lastTime = millis();
              }
            }
            else if(x == 1){
              if( xCursor<7){
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
            if(x == -1 && xCursor<2){
              xCursor++;
              lastTime = millis();
            }
            else if(x == 1 && xCursor>0){
              xCursor--;
              lastTime = millis();
            }
            break;
          //disp
          case 2:
            break;
        }
      }
      if(menu_Press){
        menu_Press = false;
        lastTime = millis();
        // constructMenu("MENU");
        return;
      }
      bool changedBrightness = false;
      while(counterA != 0){
        switch(menuTab){
          case 0:
            //changing screen brightness
            if(xCursor == 9){
              if(counterA<0){
                if(shift && screenBrightness>0){
                  screenBrightness--;
                  changedBrightness = true;
                }
                else if(!shift && screenBrightness>16){
                  screenBrightness-=16;
                  changedBrightness = true;
                }
                else if(screenBrightness != 0){
                  screenBrightness = 0;
                  changedBrightness = true;
                }
              }
              else if(counterA>0){
                if(shift && screenBrightness<255){
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
        counterA += counterA<0?1:-1;
      }
      if(changedBrightness){
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(screenBrightness);
      }
      if(sel){
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
              //external clock
              case 3:
                externalClock = !externalClock;
                lastTime = millis();
                break;
              //count-in
              case 4:
                waitForNote = !waitForNote;
                lastTime = millis();
                break;
              //rec until
              case 5:
                break;
              //after rec
              case 6:
                break;
              //remove time
              case 7:{
                lastTime = millis();
                int8_t choice = 1;
                //if there are notes that will be deleted
                uint16_t countedNotes = countNotesInRange(seqEnd-96,seqEnd);
                if(countedNotes)
                  choice = binarySelectionBox(64,32,"naur","sure","this will delete "+stringify(countedNotes)+" note(s), ok?");
                if(choice == 1){
                  removeTimeFromSeq(96,seqEnd-96);
                }
                lastTime = millis();
                }
                break;
              //add time
              case 8:
                addTimeToSeq(96,seqEnd);
                lastTime = millis();
                break;
              //brightness
              case 9:
                break;
              //load/write to flash
              case 10:
                if(shift){
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
                drawWebLink();
                enterBootsel();
                break;
              //enter bottsel
              case 2:
                enterBootsel();
                break;
            }
            break;
          case 2:
            break;
        }
      }
    }
    display.clearDisplay();
    gear.render();
    activeMenu.displaySettingsMenu(menuTab,xCursor,0,0);
    display.display();
    gear.rotate(2,2);
  }
}
void drawTemplateOptions(uint8_t x1,uint8_t whichTemplate){
  display.fillRect(x1,0,screenWidth-x1,screenHeight,0);
  display.drawFastVLine(x1,0,screenHeight,1);
  printSmall(x1+4,2,"templates",1);
  printSmall(x1+6,12,"basic",1);
  drawCheckbox(x1+30,11,whichTemplate == 0,activeMenu.highlight == 0);
  printSmall(x1+6,22,"404",1);
  drawCheckbox(x1+30,21,whichTemplate == 1,activeMenu.highlight == 1);
  printSmall(x1+6,32,"4track",1);
  drawCheckbox(x1+30,31,whichTemplate == 2,activeMenu.highlight == 2);
}

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

      printSmall(x1-5,y1,"tracks show:",1);
      drawLabel(x1+58,y1,pitchesOrNumbers?"pitches":"numbers",pitchesOrNumbers);
      
      printSmall(x1-5,y1+8,"leds:",1);
      drawLabel(x1+22,y1+8,LEDsOn?"on":"off",LEDsOn);

      printSmall(x1-5,y1+16,"clock:",1);
      drawLabel(x1+37,y1+16,externalClock?"external":"internal",!externalClock);

      printSmall(x1-5,y1+24,"count-in:",1);
      drawLabel(x1+45,y1+24,waitForNote?"wait":"",waitForNote);

      printSmall(x1-5,y1+32,"rec until:",1);
      drawLabel(x1+50,y1+32,waitForNote?"loop end":"stop",waitForNote);

      printSmall(x1-5,y1+40,"after rec:",1);
      drawLabel(x1+45,y1+40,externalClock?"stop":"play",!externalClock);
      
      //length
      printSmall(x1+3,y1+48,"length -- "+stepsToMeasures(seqEnd),1);
      printSmall(x1+2,y1+56,"("+stringify(seqEnd)+" steps)",1);
      drawArrow(x1-12,y1+54,6,1,cursor == 7);
      drawArrow(x1+56,y1+54,6,0,cursor == 8);

      //lightbulb
      display.drawBitmap(94,52,lightbulb_bmp,12,12,2);

      //save
      display.drawBitmap(111,52,save_bmp,12,12,2);

      switch(cursor){
        case 0:
          drawArrow(14+sin(millis()/200),9,2,1,true);
          break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
          drawArrow(23+sin(millis()/200),(cursor-1)*8+y1+2,2,0,false);
          break;
        //remove time
        case 7:
          drawLabel(x1+24,y1+40,"remove time",false);
          break;
        //add time
        case 8:
          drawLabel(x1+24,y1+40,"add time",false);
          break;
        //brightness
        case 9:{
          drawArrow(99,50+sin(millis()/200),2,3,true);
          String s = "light: "+stringify(screenBrightness);
          display.fillRoundRect(88,41,s.length()*4+2,7,3,1);
          printSmall(90,42,s,0);
          }
          break;
        //save
        case 10:
          drawArrow(116,50+sin(millis()/200),2,3,true);
          drawLabel(116,43,shift?"load":"save",true);
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
      printSmall(x1,y1+35,"temp: "+stringify(analogReadTemp())+"}C",1);
      printSmall(x1,y1+43,"cpu speed: "+stringify(float(rp2040.f_cpu())/float(1000000))+"MHZ",1);
      if(cursor == 0){
        drawArrow(14+sin(millis()/100),26,2,1,false);
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
    //display
    case 2:
    {
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
    display.drawFastHLine(x1+15,y1,9,1);
  }
  else{
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-8,9,1);
    display.drawFastHLine(x1,y1-8,14,1);
    printSmall(x1+2,y1-6,"sys",1);
    display.drawFastVLine(x1+14,y1-8,9,1);
    display.drawFastHLine(x1+15,y1,9,1);
  }
  if(openTab != 2){
    // uint8_t x1 = 40;
    // uint8_t y1 = 124;
    // display.drawFastVLine(x1,y1-6,7,1);
    // display.drawFastHLine(x1,y1-6,18,1);
    // printSmall(x1+2,120,"disp",1);
    // display.drawFastVLine(x1+18,y1-6,7,1);
    // display.drawFastHLine(x1+19,y1,2,1);
  }
  else{
    // uint8_t x1 = 40;
    // uint8_t y1 = 124;
    // display.drawFastVLine(x1,y1-8,9,1);
    // display.drawFastHLine(x1,y1-8,18,1);
    // printSmall(x1+2,y1-6,"disp",1);
    // display.drawFastVLine(x1+18,y1-8,9,1);
    // display.drawFastHLine(x1+19,y1,2,1);
  }
  display.drawFastHLine(40,124,24,1);
  display.setRotation(2);
}
