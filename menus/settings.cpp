/*
“settings” needs options for:
- system (memory, overclock, temp)
- display (shrunk, notes/numbers,vel/chance)
- seq (length,presets (sp, piano, sr16)
*/

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
  uint8_t menuTab = 2;
  uint8_t xCursor = 0;
  uint8_t x1 = 0;
  bool displayingTemplates = false;
  WireFrame gear = makeThickGear(10,8,8,40,true);
  gear.xPos = 100;
  gear.yPos = 10;
  gear.scale = 4;
  gear.rotate(-30,1);
  // gear.rotate(-20,0);
  while(true){
    joyRead();
    readButtons();
    if(itsbeen(200)){
      if(y != 0){
        if(xCursor == 0){
            if(y == 1 && menuTab<2){
              menuTab++;
              lastTime = millis();
            }
            else if(y == -1 && menuTab>0){
              menuTab--;
              lastTime = millis();
            }
        }
        else if(menuTab == 0){
          if(!displayingTemplates){
            if(xCursor != 4)
              xCursor = 4;
            else
              xCursor = 1;
            lastTime = millis();
          }
          //moving between template options
          else{
            if(y == -1 && activeMenu.highlight>0){
              activeMenu.highlight--;
              lastTime = millis();
            }
            else if(y == 1 && activeMenu.highlight<2){
              activeMenu.highlight++;
              lastTime = millis();
            }
          }
        }
      }
      if(x != 0){
        switch(menuTab){
          //seq
          case 0:
            if(displayingTemplates){
              displayingTemplates = false;
              lastTime = millis();
            }
            else{
              if(x == -1 && xCursor<3){
                xCursor++;
                lastTime = millis();
              }
              else if(x == 1 && xCursor>0){
                xCursor--;
                lastTime = millis();
              }
            }
            break;
          //disp
          case 1:
            break;
          //sys
          case 2:
            if(x == -1 && xCursor<2){
              xCursor++;
              lastTime = millis();
            }
            else if(x == 1 && xCursor>0){
              xCursor--;
              lastTime = millis();
            }
            break;
        }
      }
      if(menu_Press){
        menu_Press = false;
        lastTime = millis();
        constructMenu("MENU");
        return;
      }
      if(sel){
        switch(menuTab){
          //seq
          case 0:
            //selecting templates
            if(displayingTemplates){
              whichTemplate = activeMenu.highlight;
              lastTime = millis();
            }
            else{
              switch(xCursor){
                //remove time
                case 1:{
                  lastTime = millis();
                  int8_t choice = 1;
                  uint16_t countedNotes = countNotesInRange(seqEnd-96,seqEnd);
                  if(countedNotes)
                    choice = binarySelectionBox(64,32,"naur","sure","this will delete "+stringify(countedNotes)+" note(s), ok?");
                  if(choice == 1){
                    removeTimeFromSeq(96,seqEnd-96);
                  }
                  lastTime = millis();
                }
                  break;
                //cut/insert
                case 2:
                  lastTime = millis();
                  cutInsertTime();
                  break;
                //add time
                case 3:
                  addTimeToSeq(96,seqEnd);
                  lastTime = millis();
                  break;
                //trigger template menu
                case 4:
                  displayingTemplates = !displayingTemplates;
                  lastTime = millis();
                  break;
              }
            }
            break;
        }
      }
    }
    if(displayingTemplates && x1<40){
      x1+=10;
    }
    else if(!displayingTemplates && x1>9){
      x1-=10;
    }

    display.clearDisplay();
    gear.render();
    activeMenu.displaySettingsMenu(menuTab,xCursor,x1);
    display.display();
    gear.rotate(2,2);
  }
}
void drawTemplateOptions(uint8_t x1){
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

void Menu::displaySettingsMenu(uint8_t whichMenu,uint8_t cursor,uint8_t x2){
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
      const uint8_t y1 = 10;
      //length
      printSmall(x1+15,y1,"length -- "+stepsToMeasures(seqEnd),1);
      printSmall(x1+14,y1+8,"("+stringify(seqEnd)+" steps)",1);

      if(cursor == 0){
        drawArrow(14+sin(millis()/200),9,2,1,true);
      }
      //shorten
      if(cursor == 1){
        drawArrow_highlight(x1,y1+24,6,1);
        printSmall(x1+15,y1+37,"remove time",1);
      }
      else
        drawArrow(x1,y1+24,6,1,false);
      
      //center button
      if(cursor == 2){
        display.fillRoundRect(x1+15,y1+16,40,17,7,1);
        printSmall(x1+17,y1+37,"cut/insert",1);
      }
      else{
        display.fillRoundRect(x1+15,y1+16,40,17,7,0);
        display.drawRoundRect(x1+15,y1+16,40,17,7,1);
      }
      //lengthen
      if(cursor == 3){
        drawArrow_highlight(x1+68,y1+24,6,0);
        printSmall(x1+21,y1+37,"add time",1);
      }
      else{
        drawArrow(x1+68,y1+24,6,0,false);
      }
      //templates
      if(cursor == 4){
        display.fillRoundRect(64-18,55,39,9,3,1);
      }
      else{
        display.drawRoundRect(64-18,55,39,9,3,1);
      }
      printSmall(64-16,57,"templates",2);
      //template menu
      if(x2>0){
        drawTemplateOptions(screenWidth-x2);
      }
    }
      break;
    //display
    case 1:
    {
    }
      break;
    //system
    case 2:
    {
      const uint8_t x1 = 20;
      const uint8_t y1 = 6;
      printMemoryInfo(x1,y1);
      printSmall(x1,y1+35,"temp: "+stringify(analogReadTemp()),1);
      printSmall(x1,y1+43,"cpu speed: "+stringify(float(rp2040.f_cpu())/float(1000000))+"MHZ",1);
      if(cursor == 0){
        drawArrow(14+sin(millis()/100),47,2,1,sin(millis()/100)>0);
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
    display.drawFastHLine(x1,y1-6,18,1);
    printSmall(x1+2,120,"disp",1);
    display.drawFastVLine(x1+18,y1-6,7,1);
    display.drawFastHLine(x1+19,y1,2,1);
  }
  else{
    uint8_t x1 = 19;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-8,9,1);
    display.drawFastHLine(x1,y1-8,18,1);
    printSmall(x1+2,y1-6,"disp",1);
    display.drawFastVLine(x1+18,y1-8,9,1);
    display.drawFastHLine(x1+19,y1,2,1);
  }
  if(openTab != 2){
    uint8_t x1 = 40;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-6,7,1);
    display.drawFastHLine(x1,y1-6,14,1);
    printSmall(x1+2,120,"sys",1);
    display.drawFastVLine(x1+14,y1-6,7,1);
    display.drawFastHLine(x1+15,y1,9,1);
  }
  else{
    uint8_t x1 = 40;
    uint8_t y1 = 124;
    display.drawFastVLine(x1,y1-8,9,1);
    display.drawFastHLine(x1,y1-8,14,1);
    printSmall(x1+2,y1-6,"sys",1);
    display.drawFastVLine(x1+14,y1-8,9,1);
    display.drawFastHLine(x1+15,y1,9,1);
  }
  display.setRotation(2);
}
