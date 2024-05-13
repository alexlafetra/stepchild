void drawArpMenu(uint8_t cursor);

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
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(sel){
        lastTime = millis();
        activeArp.uniformLength = !activeArp.uniformLength;
      }
    }
    if(activeArp.uniformLength){
      if(utils.itsbeen(200)){
        if(menu_Press){
          menu_Press = false;
          lastTime = millis();
          return;
        }
      }
    }
    //these controls are only active if the arp is using custom lengths
    else{
      if(utils.itsbeen(200)){
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
      if(utils.itsbeen(120)){
        if(controls.joystickY != 0){
          if(shift){
            activeArp.lengths[cursorX] = toggleTriplets(activeArp.lengths[cursorX]);
            lastTime = millis();
          }
          else{
            if(controls.joystickY == -1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(true,activeArp.lengths[cursorX],false);
            }
            else if(controls.joystickY == 1){
              lastTime = millis();
              activeArp.lengths[cursorX] = changeSubDiv(false,activeArp.lengths[cursorX],false);
            }
          }
        }
      }
      if(utils.itsbeen(100)){
        if(controls.joystickX != 0){
          if(controls.joystickX == -1 && cursorX<activeArp.lengths.size()-1){
            cursorX++;
            lastTime = millis();
          }
          if(controls.joystickX == 1 && cursorX>0){
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
  display.fillRect(0,2,70,57,0);
  display.drawRoundRect(0,2,70,57,3,1);
  printItalic(2,4,"settings",1);
  String text;
  const uint8_t mid = 32;
  //vel is special, needs a top and a bottom
  int8_t start = mid-activeArp.maxVelMod/10;
  int8_t end = mid+activeArp.minVelMod/10;
  int8_t length = end-start;
  display.fillRect(3,start,7,length,1);
  printSmall_centered(7,end+3,stringify(64-activeArp.minVelMod/2),1);
  printSmall_centered(7,start-7,stringify(activeArp.maxVelMod/2),1);
  if(cursor == 0){
    graphics.drawArrow(6,end+8+(millis()/200)%2,3,2,true);
    text = "velocity";
  }
  
  //chance
  display.fillRect(13,mid+13-activeArp.chanceMod/4,7,activeArp.chanceMod/4,1);
  printSmall_centered(17,mid+7-activeArp.chanceMod/4,stringify(activeArp.chanceMod),1);
  if(cursor == 1){
    graphics.drawArrow(16,mid+15+(millis()/200)%2,3,2,true);
    text = "chance";
  }

  //reps
  display.fillRect(23,mid+13-activeArp.repMod/4,7,activeArp.repMod/4,1);
  printSmall_centered(27,mid+7-activeArp.repMod/4,stringify(activeArp.repMod),1);
  if(cursor == 2){
    graphics.drawArrow(26,mid+15+(millis()/200)%2,3,2,true);
    text = "repeats";
  }

  //pitch is also special, needs a top and a bottom
  start = mid - activeArp.maxPitchMod/10;
  end = mid + activeArp.minPitchMod/10;
  length = end - start;
  display.fillRect(33,start,7,length,1);
  printSmall_centered(37,end+3,stringify(-activeArp.minPitchMod/16),1);
  printSmall_centered(37,start-7,stringify(activeArp.maxPitchMod/16),1);
  if(cursor == 3){
    graphics.drawArrow(36,end+8+(millis()/200)%2,3,2,true);
    text = "octave";
  }

  //play order
  if(cursor == 4){
    start = mid-(activeArp.playStyle)*12-5;
    for(uint8_t i = 0; i<6; i++){
      drawArpModeIcon(44,start+i*12,i,1);
    }
    graphics.drawArrow(56+(millis()/200)%2,mid,3,1,true);
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
  }
  else{
    drawArpModeIcon(44,mid-5,activeArp.playStyle,1);
  }
  display.setRotation(3);
  printSmall(16,60,text,1);
  display.setRotation(2);
  display.fillRoundRect(65,0,15,15,3,0);
  display.drawRoundRect(65,0,15,15,3,1);
  display.drawBitmap(67,2,gear_bmp,11,11,1);
}

void arpModMenu(){
  uint8_t cursor = 0;
  while(true){
    readButtons();
    controls.readJoystick();
    if(utils.itsbeen(50)){
       if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          switch(cursor){
            case 0:
              //editing min vel
              if(shift){
                if(activeArp.minVelMod<117)
                  activeArp.minVelMod+=10;
                else
                  activeArp.minVelMod = 127;
              }
              else{
                if(activeArp.maxVelMod>10)
                  activeArp.maxVelMod-=10;
                else
                  activeArp.maxVelMod = 0;
              }
              lastTime = millis();
              break;
            case 1:
              //editing chance
              if(activeArp.chanceMod>10)
                activeArp.chanceMod-=10;
              else
                activeArp.chanceMod=0;
              lastTime = millis();
              break;
            case 2:
              //editing repeats
              if(activeArp.repMod>10)
                activeArp.repMod-=10;
              else
                activeArp.repMod = 0;
              lastTime = millis();
              break;
            //pitch
            case 3:
              if(shift){
                if(activeArp.minPitchMod<117)
                  activeArp.minPitchMod+=10;
                else
                  activeArp.minPitchMod = 127;
              }
              else{
                if(activeArp.maxPitchMod>10)
                  activeArp.maxPitchMod-=10;
                else
                  activeArp.maxPitchMod = 0;
              }
              lastTime = millis();
              break;
            //play style
            case 4:
              if(activeArp.playStyle<5 && utils.itsbeen(200)){
                activeArp.playStyle++;
                lastTime = millis();
              }
              break;
          }
        }
        else if(controls.joystickY == -1){
          switch(cursor){
            case 0:
              //editing min vel
              if(shift){
                if(activeArp.minVelMod>10)
                  activeArp.minVelMod-=10;
                else
                  activeArp.minVelMod = 0;
              }
              else{
                if(activeArp.maxVelMod<117)
                  activeArp.maxVelMod+=10;
                else
                  activeArp.maxVelMod = 127;
              }
              lastTime = millis();
              break;
            //editing chance
            case 1:
              if(activeArp.chanceMod<90)
                activeArp.chanceMod+=10;
              else
                activeArp.chanceMod = 100;
              lastTime = millis();
              break;
            //editing repeats
            case 2:
              if(activeArp.repMod<90)
                activeArp.repMod+=10;
              else
                activeArp.repMod = 100;
              lastTime = millis();
              break;
            //pitch
            case 3:
              if(shift){
                if(activeArp.minPitchMod>10)
                  activeArp.minPitchMod-=10;
                else
                  activeArp.minPitchMod = 0;
              }
              else{
                if(activeArp.maxPitchMod<117)
                  activeArp.maxPitchMod+=10;
                else
                  activeArp.maxPitchMod = 127;
              }
              lastTime = millis();
              break;
            case 4:
              if(activeArp.playStyle>0 && utils.itsbeen(200)){
                activeArp.playStyle--;
                lastTime = millis();
              }
              break;
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(controls.joystickX != 0){
        if(controls.joystickX == -1 && cursor<4){
          cursor++;
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && cursor>0){
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
          //min vel
          case 0:
            if(counterA < 0 && activeArp.minVelMod<127){
              activeArp.minVelMod++;
            }
            else if(counterA > 0 && activeArp.minVelMod>0){
              activeArp.minVelMod--;
            }
            counterA += counterA<0?1:-1;
            break;
          //chance
          case 1:
            if(counterA > 0 && activeArp.chanceMod<100){
              activeArp.chanceMod++;
            }
            else if(counterA < 0 && activeArp.chanceMod>0){
              activeArp.chanceMod--;
            }
            counterA += counterA<0?1:-1;
            break;
          //reps
          case 2:
            if(counterA > 0 && activeArp.repMod<100){
              activeArp.repMod++;
            }
            else if(counterA < 0 && activeArp.repMod>0){
              activeArp.repMod--;
            }
            counterA += counterA<0?1:-1;
            break;
          case 3:
            if(counterA < 0 && activeArp.minPitchMod<127){
              activeArp.minPitchMod++;
            }
            else if(counterA > 0 && activeArp.minPitchMod>0){
              activeArp.minPitchMod--;
            }
            counterA += counterA<0?1:-1;
            break;
          case 4:
            if(counterA < 0){
              if(activeArp.playStyle > 0)
                activeArp.playStyle--;
            }
            else{
              if(activeArp.playStyle < 5)
                activeArp.playStyle++;
            }
            counterA += counterA<0?1:-1;
            break;
        }
      }
      while(counterB != 0){
        switch(cursor){
          case 0:
            if(counterB > 0 && activeArp.maxVelMod<127){
              activeArp.maxVelMod++;
            }
            else if(counterB < 0 && activeArp.maxVelMod>0){
              activeArp.maxVelMod--;
            }
            counterB += counterB<0?1:-1;
            break;
          case 1:
            counterB = 0;
            break;
          case 2:
            counterB = 0;
            break;
          case 3:
            if(counterB > 0 && activeArp.maxPitchMod<127){
              activeArp.maxPitchMod++;
            }
            else if(counterB < 0 && activeArp.maxPitchMod>0){
              activeArp.maxPitchMod--;
            }
            counterB += counterB<0?1:-1;
            break;
        }
      }
    }
    display.clearDisplay();
    drawArpMenu(2);
    drawModBoxes(cursor);
    display.display();
  }
}
void drawArpModeIcon(uint8_t x1, int8_t y1, uint8_t which,uint16_t c){
  which%=6;
  display.fillRect(x1,y1,11,11,0);
  display.drawBitmap(x1,y1,arpMode_icons[which],11,11,c);
}

void drawArpMenu(uint8_t cursor){
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
      graphics.drawArrow(29+((millis()/400)%2),y1+5,3,1,false);
      break;
    case 1:
      graphics.drawArrow(61+((millis()/400)%2),y1+25,3,1,false);
      break;
    case 2:
      graphics.drawArrow(82+((millis()/400)%2),y1+40,3,1,false);
      break;
  }
  //channel icon
  drawSmallChannelIcon(93,1,activeArp.channel);

  //input icon
  printSmall(50,1,"src:",1);
  switch(activeArp.source){
    //external
    case 0:
      display.drawBitmap(64,0,tiny_midi_bmp,7,7,1);
      break;
    //internal
    case 1:
      display.drawBitmap(64,0,tiny_stepchild_bmp,7,7,1);
      break;
    //both
    case 2:
      display.drawBitmap(64,0,tiny_midi_bmp,7,7,1);
      printSmall(72,1,"+",1);
      display.drawBitmap(76,0,tiny_stepchild_bmp,7,7,1);
      break;
  }

  drawKeys(38,14,getOctave(keyboardPitch),14,false);//always start on a C, for simplicity
  
  //title
  printArp_wiggly(0,3,"arpeggi",1);
}

//"notes" option adds notes from the 1scale onto whatever notes are playing
void arpMenu(){
  keyboardAnimation(38,14,0,14,true);
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
    drawArpMenu(cursor);
    display.display();

    //controls
    readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.joystickY != 0){
        if(controls.joystickY == -1 && cursor>0){
          //jump from arp mode button up to length
          // if(cursor == 3){
          //   cursor = 1;
          // }
          // else{
            cursor--;
          // }
          lastTime = millis();
        }
        else if(controls.joystickY == 1 && cursor<2){
          cursor++;
          lastTime = millis();
        }
      }
      if(controls.joystickX != 0){
        switch(cursor){
          //on/off
          case 0:
            if(controls.joystickX == -1 && isArping){
              isArping = false;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && !isArping){
              isArping = true;
              lastTime = millis();
            }
            break;
          //step lengths
          case 1:
            if(controls.joystickX == 1 && activeArp.uniformLength){
              activeArp.uniformLength = false;
              lastTime = millis();
            }
            else if(controls.joystickX == -1 && !activeArp.uniformLength){
              activeArp.uniformLength = true;
              lastTime = millis();
            }
            break;
          case 2:
            if(controls.joystickX == -1){
              // cursor = 3;
              lastTime = millis();
            }
            break;
          case 3:
            if(controls.joystickX == 1){
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
      if(play || n){
        activeArp.source++;
        activeArp.source%=3;
        lastTime = millis();
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
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
  // keyboardAnimation();
}
