void deleteCustomLength(uint8_t which){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<activeArp.lengths.size(); i++){
    if(i != which){
      newNotes.push_back(activeArp.lengths[i]);
    }
  }
  activeArp.lengths.swap(newNotes);
}

void swapCustomLengths(uint8_t which1, uint8_t which2){
  uint8_t temp = activeArp.lengths[which1];
  activeArp.lengths[which1] = activeArp.lengths[which2];
  activeArp.lengths[which2] = temp;
}

void addCustomLength(uint8_t size, uint8_t where){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<activeArp.lengths.size(); i++){
    if(i == where){
      newNotes.push_back(size);
    }
    newNotes.push_back(activeArp.lengths[i]);
  }
  activeArp.lengths.swap(newNotes);
}

void customLengthsMenu(){
  uint8_t cursorX = 0;
  bool selected = false;
  uint8_t startNote = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(sel){
        lastTime = millis();
        activeArp.uniformLength = !activeArp.uniformLength;
      }
    }
    if(activeArp.uniformLength){
      if(itsbeen(200)){
        if(menu_Press){
          menu_Press = false;
          lastTime = millis();
          return;
        }
      }
    }
    //these controls are only active if the arp is using custom lengths
    else{
      if(itsbeen(200)){
        if(n){
          addCustomLength(activeArp.lengths[cursorX],cursorX);
          lastTime = millis();
        }
        if(sel){
          activeArp.uniformLength = !activeArp.uniformLength;
          lastTime = millis();
        }
        if(del && activeArp.lengths.size()>1){
          deleteCustomLength(cursorX);
          lastTime = millis();
          while(startNote+7>=activeArp.lengths.size()&& activeArp.lengths.size()>8){
            startNote--;
          }
          while(cursorX>=activeArp.lengths.size())
            cursorX--;
        }
        if(menu_Press){
          lastTime = millis();
          menu_Press = false;
          break;
        }
        if(sel){
          selected = !selected;
          lastTime = millis();
        }
      }
      if(itsbeen(120)){
        if(y != 0){
          if(shift){
            activeArp.lengths[cursorX] = toggleTriplets(activeArp.lengths[cursorX]);
            lastTime = millis();
          }
          else{
            if(y == -1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(true,activeArp.lengths[cursorX],false);
            }
            else if(y == 1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(false,activeArp.lengths[cursorX],false);
            }
          }
        }
      }
      if(itsbeen(100)){
        if(x != 0){
          if(x == -1 && cursorX<activeArp.lengths.size()-1){
            cursorX++;
            lastTime = millis();
          }
          if(x == 1 && cursorX>0){
            cursorX--;
            lastTime = millis();
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
    display.clearDisplay();
    drawArpStepLengths(0,0,startNote,cursorX,selected);
    display.display();
  }
}

void drawModBoxes(uint8_t cursor){
  //three boxes to represent the three parameters
  uint8_t start = 32-activeArp.maxVelMod/4;
  uint8_t end = 32+activeArp.minVelMod/4;
  uint8_t length = end-start;
  drawBox(4,3*sin(float(millis())/float(400))+start,16,length,4,-4,0);
  printSmall(8,3*sin(float(millis())/float(400))+32,"v",1);

  drawBox(20,3*sin(float(millis())/float(400)+200)+32-activeArp.chanceMod/4,16,activeArp.chanceMod/2,4,-4,0);
  printSmall(24,3*sin(float(millis())/float(400)+200)+32,"%",1);

  // drawBox(40,3*sin(float(millis())/float(400)+400)+32-activeArp.lengthMod/4,16,activeArp.lengthMod/2,4,-4,0);
  // printSmall(44,3*sin(float(millis())/float(400)+400)+32,"l",1);

  drawBox(36,3*sin(float(millis())/float(400)+600)+32-activeArp.repMod/4,16,activeArp.repMod/2,4,-4,0);
  printSmall(40,3*sin(float(millis())/float(400)+600)+32,"x",1);

  start = 32 - activeArp.maxPitchMod/4;
  end = 32 + activeArp.minPitchMod/4;
  length = end - start;
  drawBox(52,3*sin(float(millis())/float(400)+800)+start,16,length,4,-4,0);
  printSmall(56,3*sin(float(millis())/float(400)+800)+32,"$",1);

  drawBox(77,23,16,16,4,-4,(cursor == 4)?2:1);
  drawArpModeIcon(76,30,activeArp.playStyle,2);

  switch(cursor){
    case 0:
      // drawDottedLineV(14,36-activeArp.velMod/4,32+activeArp.velMod/4,2);
      drawArrow(9,37+activeArp.minVelMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,30,3,0);//box
      display.drawRoundRect(95,22,32,30,3,1);//box
      display.drawFastHLine(97,22,13,0);//mask for letters
      printSmall(98,20,"vel",1);
      printSmall(98,30,"mx:",1);
      print007SegSmall(109,29,stringify(64+activeArp.maxVelMod/2),1);
      printSmall(98,40,"mn:",1);
      print007SegSmall(109,39,stringify(64-activeArp.minVelMod/2),1);
      break;
    case 1:
      // drawDottedLineV(30,36-activeArp.chanceMod/4,32+activeArp.chanceMod/4,2);
      drawArrow(25,37+activeArp.chanceMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,18,3,0);//box
      display.drawRoundRect(95,22,32,18,3,1);//box
      display.drawFastHLine(97,22,26,0);//mask for letters
      printSmall(98,20,"chance",1);
      print007SegSmall(100,29,stringify(activeArp.chanceMod)+"%",1);
      break;
    case 2:
      // drawDottedLineV(62,36-activeArp.repMod/4,32+activeArp.repMod/4,2);
      drawArrow(41,37+activeArp.repMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,18,3,0);//box
      display.drawRoundRect(95,22,32,18,3,1);//box
      display.drawFastHLine(95,22,17,0);//mask for letters
      printSmall(98,20,"reps",1);
      print007SegSmall(103,29,stringify(activeArp.repMod)+"%",1);
      break;
    case 3:
      drawArrow(57,37+activeArp.minPitchMod/4+sin(millis()/200),3,2,true);

      display.fillRoundRect(95,22,32,30,3,0);//box
      display.drawRoundRect(95,22,32,30,3,1);//box
      display.drawFastHLine(97,22,21,0);//mask for letters
      printSmall(98,20,"pitch",1);
      printSmall(101,30,"mx:",1);
      printSmall(113,30,stringify(activeArp.maxPitchMod/16),1);
      // print007SegSmall(105,29,stringify(activeArp.maxPitchMod/16),1);
      printSmall(101,40,"mn:",1);
      printSmall(113,40,stringify(-activeArp.minPitchMod/16),1);
      // print007SegSmall(105,39,stringify(activeArp.minPitchMod/16),1);
      break;
    case 4:
      drawArrow(81,43+sin(millis()/200),3,2,true);

      String text;
      switch(activeArp.playStyle){
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
      // display.fillRoundRect(17,22,41,20,3,0);//box
      // display.drawRoundRect(17,22,41,20,3,1);//box
      // display.drawFastHLine(19,22,45,0);//mask for letters
      printSmall(88,47,text,1);
      break;
  }
}

void arpModMenu(){
  uint8_t cursor = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(x != 0){
        if(x == -1 && cursor<4){
          cursor++;
          lastTime = millis();
        }
        else if(x == 1 && cursor>0){
          cursor--;
          lastTime = millis();
        }
      }
      if(del){
        switch(cursor){
          case 0:
            activeArp.maxVelMod = 0;
            activeArp.minVelMod = 0;
            lastTime = millis();
            break;
          case 1:
            activeArp.chanceMod = 100;
            lastTime = millis();
            break;
          case 2:
            activeArp.repMod = 0;
            lastTime = millis();
            break;
          case 3:
            activeArp.maxPitchMod = 0;
            activeArp.minPitchMod = 0;
            lastTime = millis();
            break;
          case 4:
            activeArp.playStyle = 0;
            lastTime = millis();
            break;
        }
      }
      while(counterA != 0){
        switch(cursor){
          case 0:
            if(!shift){
              if(counterA < 0){
                if(activeArp.minVelMod>117)
                  activeArp.minVelMod = 127;
                else
                  activeArp.minVelMod += 10;
              }
              if(counterA > 0 ){
                if(activeArp.minVelMod>10)
                  activeArp.minVelMod-=10;
                else
                  activeArp.minVelMod = 0;
              }
            }
            else{
              if(counterA < 0 && activeArp.minVelMod<127){
                activeArp.minVelMod++;
              }
              else if(counterA > 0 && activeArp.minVelMod>0){
                activeArp.minVelMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 1:
            if(!shift){
              if(counterA > 0){
                if(activeArp.chanceMod>90)
                  activeArp.chanceMod = 100;
                else
                  activeArp.chanceMod += 10;
              }
              if(counterA < 0){
                if(activeArp.chanceMod>10)
                  activeArp.chanceMod-=10;
                else
                  activeArp.chanceMod = 0;
              }
            }
            else{
              if(counterA > 0 && activeArp.chanceMod<100){
                activeArp.chanceMod++;
              }
              else if(counterA < 0 && activeArp.chanceMod>0){
                activeArp.chanceMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 2:
            if(!shift){
              if(counterA > 0){
                if(activeArp.repMod>90)
                  activeArp.repMod = 100;
                else
                  activeArp.repMod += 10;
              }
              if(counterA < 0){
                if(activeArp.repMod>10)
                  activeArp.repMod-=10;
                else
                  activeArp.repMod = 0;
              }
            }
            else{
              if(counterA > 0 && activeArp.repMod<100){
                activeArp.repMod++;
              }
              else if(counterA < 0 && activeArp.repMod>0){
                activeArp.repMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 3:
            if(!shift){
              if(counterA < 0){
                if(activeArp.minPitchMod>117)
                  activeArp.minPitchMod = 127;
                else
                  activeArp.minPitchMod += 10;
              }
              if(counterA > 0){
                if(activeArp.minPitchMod>10)
                  activeArp.minPitchMod-=10;
                else
                  activeArp.minPitchMod = 0;
              }
            }
            else{
              if(counterA < 0 && activeArp.minPitchMod<127){
                activeArp.minPitchMod++;
              }
              else if(counterA > 0 && activeArp.minPitchMod>0){
                activeArp.minPitchMod--;
              }
            }
            counterA += counterA<0?1:-1;;
            break;
          case 4:
            if(counterA < 0){
              if(activeArp.playStyle == 0)
                activeArp.playStyle = 5;
              else
                activeArp.playStyle--;
            }
            else{
              if(activeArp.playStyle == 5)
                activeArp.playStyle = 0;
              else
                activeArp.playStyle++;
            }
            counterA += counterA<0?1:-1;;
            break;
        }
      }
      while(counterB != 0){
        switch(cursor){
          case 0:
            if(!shift){
              if(counterB > 0){
                if(activeArp.maxVelMod>117)
                  activeArp.maxVelMod = 127;
                else
                  activeArp.maxVelMod += 10;
              }
              if(counterB < 0){
                if(activeArp.maxVelMod>10)
                  activeArp.maxVelMod-=10;
                else
                  activeArp.maxVelMod = 0;
              }
            }
            else{
              if(counterB > 0 && activeArp.maxVelMod<127){
                activeArp.maxVelMod++;
              }
              else if(counterB < 0 && activeArp.maxVelMod>0){
                activeArp.maxVelMod--;
              }
            }
            counterB += counterB<0?1:-1;;
            break;
          case 1:
            counterB = 0;
            break;
          case 2:
            counterB = 0;
            break;
          case 3:
            if(!shift){
              if(counterB > 0){
                if(activeArp.maxPitchMod>117)
                  activeArp.maxPitchMod = 127;
                else
                  activeArp.maxPitchMod += 10;
              }
              if(counterB < 0){
                if(activeArp.maxPitchMod>10)
                  activeArp.maxPitchMod-=10;
                else
                  activeArp.maxPitchMod = 0;
              }
            }
            else{
              if(counterB > 0 && activeArp.maxPitchMod<127){
                activeArp.maxPitchMod++;
              }
              else if(counterB < 0 && activeArp.maxPitchMod>0){
                activeArp.maxPitchMod--;
              }
            }
            counterB += counterB<0?1:-1;;
            break;
        }
      }
    }
    display.clearDisplay();
    printArp_wiggly(66,3,"modulation",1);
    drawModBoxes(cursor);
    display.display();
  }
}
void drawArpModeIcon(uint8_t x1, uint8_t y1, uint8_t which,uint16_t c){
  // which%=6;
  display.drawBitmap(x1,y1,arpMode_icons[which],11,11,c);
}

//"notes" option adds notes from the scale onto whatever notes are playing
void arpMenu(){
  // keyboardAnimation(38,14,0,14,true);
  int xStart = 10;
  int yStart = 32;
  int spacing = 4;
  int width = screenWidth/8-spacing-2;
  //step boxes
  int noteStart = 0;
  int noteEnd = 7;
  uint8_t cursor = 0;
  while(true){
    display.clearDisplay();
    //last note played
    String lastNote = pitchToString(activeArp.lastPitchSent,true,true);
    printPitch(115,0,lastNote,false,false, 1);
    if(activeArp.holding && millis()%800>400){
      printSmall(106,9,"[HOLD]",1);
    }
    else if(!activeArp.holding){
      printSmall(110,9,"cpy 2",1);
        printSmall(112,15,"hold",1);
    }
    const uint8_t y1 = 15;
    //on/off
    drawSlider(0,y1,"on","off",!isArping);

    //step lengths
    String stepLength = activeArp.uniformLength?stepsToMeasures(activeArp.arpSubDiv):"custom";
    drawSlider(0,y1+20,"custom","uniform",activeArp.uniformLength);
    printSmall(0,y1+14,"length:" + stepLength,1);

    //modulation
    printArp(0,52,"MOD",1);

    fillSquareVertically(20,50,11,float(activeArp.maxVelMod*100)/float(127));
    printSmall(24,53,"v",2);

    fillSquareVertically(33,50,11,float(activeArp.chanceMod));
    printSmall(37,53,"%",2);

    fillSquareVertically(46,50,11,float(activeArp.repMod*100)/float(127));
    printSmall(50,53,"x",2);

    fillSquareVertically(59,50,11,float(activeArp.maxPitchMod*100)/float(127));
    printSmall(63,53,"$",2);

    //arp mode icon
    drawArpModeIcon(72,50,activeArp.playStyle,1);

    switch(cursor){
      case 0:
        drawArrow(29+sin(millis()/200),y1+5,3,1,false);
        break;
      case 1:
        drawArrow(61+sin(millis()/200),y1+25,3,1,false);
        break;
      case 2:
        drawArrow(82+sin(millis()/200),y1+40,3,1,false);
        break;
      // case 3:
      //   drawArrow(108+sin(millis()/200),y1+17,3,0,false);
      //   break;
    }
    //channel icon
    drawSmallChannelIcon(93,1,activeArp.channel);
    // display.drawBitmap(93,1,ch_tiny,6,3,SSD1306_WHITE);
    // printSmall(100,1,String(activeArp.channel),1);

    //input icon
    // printSmall(50,1,"input:"+(activeArp.getNotesFromExternalInput?String("ex"):String("seq")),1);
    printSmall(50,1,"input:",1);
    display.drawBitmap(76,0,activeArp.getNotesFromExternalInput?tiny_midi_bmp:tiny_stepchild_bmp,7,7,1);

    drawKeys(38,14,getOctave(keyboardPitch),14,false);//always start on a C, for simplicity
    //title
    display.drawBitmap(0,3+3*sin(float(millis())/float(200)),arpTitle[0],5,7,1);
    display.drawBitmap(6,3+3*sin(float(millis())/float(200)+200),arpTitle[1],5,7,1);
    display.drawBitmap(12,3+3*sin(float(millis())/float(200)+400),arpTitle[2],5,7,1);
    display.drawBitmap(18,3+3*sin(float(millis())/float(200)+600),arpTitle[3],5,7,1);
    display.drawBitmap(24,3+3*sin(float(millis())/float(200)+800),arpTitle[4],5,7,1);
    display.drawBitmap(30,3+3*sin(float(millis())/float(200)+1000),arpTitle[5],5,7,1);
    display.drawBitmap(36,3+3*sin(float(millis())/float(200)+1200),arpTitle[6],5,7,1);
    // display.drawBitmap(42,3+3*sin(float(millis())/float(200)+1400),arpTitle[7],5,7,1);
    display.display();

    //controls
    readButtons();
    joyRead();
    if(itsbeen(200)){
      if(y != 0){
        if(y == -1 && cursor>0){
          //jump from arp mode button up to length
          // if(cursor == 3){
          //   cursor = 1;
          // }
          // else{
            cursor--;
          // }
          lastTime = millis();
        }
        else if(y == 1 && cursor<2){
          cursor++;
          lastTime = millis();
        }
      }
      if(x != 0){
        switch(cursor){
          //on/off
          case 0:
            if(x == -1 && isArping){
              isArping = false;
              lastTime = millis();
            }
            else if(x == 1 && !isArping){
              isArping = true;
              lastTime = millis();
            }
            break;
          //step lengths
          case 1:
            if(x == 1 && activeArp.uniformLength){
              activeArp.uniformLength = false;
              lastTime = millis();
            }
            else if(x == -1 && !activeArp.uniformLength){
              activeArp.uniformLength = true;
              lastTime = millis();
            }
            break;
          case 2:
            if(x == -1){
              // cursor = 3;
              lastTime = millis();
            }
            break;
          case 3:
            if(x == 1){
              cursor = 2;
              lastTime = millis();
            }
            break;
        }
      }
      if(sel){
        switch(cursor){
          //on/off
          case 0:
            isArping = !isArping;
            lastTime = millis();
            break;
          //step lengths
          case 1:
            lastTime = millis();
            customLengthsMenu();
            break;
          case 2:
            lastTime = millis();
            arpModMenu();
            break;
          case 3:
            // activeArp.playStyle++;
            // activeArp.playStyle%=6;
            // lastTime = millis();
            break;
        }
      }
      //'hold' locks notes that are currently in the arp
      if(copy_Press){
        activeArp.holding = !activeArp.holding;
        lastTime = millis();
      }
      //swap between external/internal input
      if(play){
        activeArp.getNotesFromExternalInput = !activeArp.getNotesFromExternalInput;
        lastTime = millis();
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      while(counterA != 0){
        switch(cursor){
          case 0:
            isArping = !isArping;
            counterA += counterA<0?1:-1;;
            break;
          case 1:
            if(shift){
              activeArp.arpSubDiv = toggleTriplets(activeArp.arpSubDiv);
              counterA += counterA<0?1:-1;;
            }
            else{
              if(counterA > 0){
                activeArp.arpSubDiv = changeSubDiv(true,activeArp.arpSubDiv,false);
                counterA += counterA<0?1:-1;;
              }
              else{
                activeArp.arpSubDiv = changeSubDiv(false,activeArp.arpSubDiv,false);
                counterA += counterA<0?1:-1;;
              }
            }
            break;
          case 2:
            if(counterA > 0){
              activeArp.playStyle++;
              if(activeArp.playStyle == 6)
                activeArp.playStyle = 0;
            }
            else if(counterA < 0){
              if(activeArp.playStyle == 0)
                activeArp.playStyle = 5;
              else
                activeArp.playStyle--;
            }
            counterA += counterA<0?1:-1;;
            break;
        }
      }
      while(counterB != 0){
        if(counterB < 0 && activeArp.channel>1){
          activeArp.channel--;
        }
        else if(counterB > 0 && activeArp.channel<16){
          activeArp.channel++;
        }
        lastTime = millis();
        counterB += counterB<0?1:-1;;
      }
    }
  }
}