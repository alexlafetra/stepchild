void drawArpMenu(uint8_t cursor, bool justKeys);
void drawArpMenu(uint8_t cursor);

void deleteCustomLength(uint8_t which){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<arp.lengths.size(); i++){
    if(i != which){
      newNotes.push_back(arp.lengths[i]);
    }
  }
  arp.lengths.swap(newNotes);
}

void swapCustomLengths(uint8_t which1, uint8_t which2){
  uint8_t temp = arp.lengths[which1];
  arp.lengths[which1] = arp.lengths[which2];
  arp.lengths[which2] = temp;
}

void addCustomLength(uint8_t size, uint8_t where){
  vector <uint8_t> newNotes;
  for(uint8_t i = 0; i<arp.lengths.size(); i++){
    if(i == where){
      newNotes.push_back(size);
    }
    newNotes.push_back(arp.lengths[i]);
  }
  arp.lengths.swap(newNotes);
}

void customLengthsMenu(){
  uint8_t cursorX = 0;
  bool selected = false;
  uint8_t startNote = 0;
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.SELECT() ){
        lastTime = millis();
        arp.uniformLength = !arp.uniformLength;
      }
    }
    if(arp.uniformLength){
      if(utils.itsbeen(200)){
        if(controls.MENU()){
          controls.setMENU(false) ;
          lastTime = millis();
          return;
        }
      }
    }
    //these controls are only active if the arp is using custom lengths
    else{
      if(utils.itsbeen(200)){
        if(controls.NEW()){
          addCustomLength(arp.lengths[cursorX],cursorX);
          lastTime = millis();
        }
        if(controls.SELECT() ){
          arp.uniformLength = !arp.uniformLength;
          lastTime = millis();
        }
        if(controls.DELETE() && arp.lengths.size()>1){
          deleteCustomLength(cursorX);
          lastTime = millis();
          while(startNote+7>=arp.lengths.size()&& arp.lengths.size()>8){
            startNote--;
          }
          while(cursorX>=arp.lengths.size())
            cursorX--;
        }
        if(controls.MENU()){
          lastTime = millis();
          controls.setMENU(false) ;
          break;
        }
        if(controls.SELECT() ){
          selected = !selected;
          lastTime = millis();
        }
      }
      if(utils.itsbeen(120)){
        if(controls.joystickY != 0){
          if(controls.SHIFT()){
            arp.lengths[cursorX] = sequence.toggleTriplets(arp.lengths[cursorX]);
            lastTime = millis();
          }
          else{
            if(controls.joystickY == -1){
              lastTime = millis();
              arp.lengths[cursorX] = sequence.changeSubDiv(true,arp.lengths[cursorX],false);
            }
            else if(controls.joystickY == 1){
              lastTime = millis();
              arp.lengths[cursorX] = sequence.changeSubDiv(false,arp.lengths[cursorX],false);
            }
          }
        }
      }
      if(utils.itsbeen(100)){
        if(controls.joystickX != 0){
          if(controls.joystickX == -1 && cursorX<arp.lengths.size()-1){
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

void drawModBoxes_old(uint8_t cursor){
  display.fillRect(0,2,70,57,0);
  display.drawRoundRect(0,2,70,57,3,1);
  printItalic(2,4,"settings",1);
  String text;
  const uint8_t mid = 32;
  //vel is special, needs a top and a bottom
  int8_t start = mid-arp.maxVelMod/10;
  int8_t end = mid+arp.minVelMod/10;
  int8_t length = end-start;
  display.fillRect(3,start,7,length,1);
  printSmall_centered(7,end+3,stringify(64-arp.minVelMod/2),1);
  printSmall_centered(7,start-7,stringify(arp.maxVelMod/2),1);
  if(cursor == 0){
    graphics.drawArrow(6,end+8+(millis()/200)%2,3,2,true);
    text = "velocity";
  }
  
  //chance
  display.fillRect(13,mid+13-arp.chanceMod/4,7,arp.chanceMod/4,1);
  printSmall_centered(17,mid+7-arp.chanceMod/4,stringify(arp.chanceMod),1);
  if(cursor == 1){
    graphics.drawArrow(16,mid+15+(millis()/200)%2,3,2,true);
    text = "chance";
  }

  //reps
  display.fillRect(23,mid+13-arp.repMod/4,7,arp.repMod/4,1);
  printSmall_centered(27,mid+7-arp.repMod/4,stringify(arp.repMod),1);
  if(cursor == 2){
    graphics.drawArrow(26,mid+15+(millis()/200)%2,3,2,true);
    text = "repeats";
  }

  //pitch is also special, needs a top and a bottom
  start = mid - arp.maxPitchMod/10;
  end = mid + arp.minPitchMod/10;
  length = end - start;
  display.fillRect(33,start,7,length,1);
  printSmall_centered(37,end+3,stringify(-arp.minPitchMod/16),1);
  printSmall_centered(37,start-7,stringify(arp.maxPitchMod/16),1);
  if(cursor == 3){
    graphics.drawArrow(36,end+8+(millis()/200)%2,3,2,true);
    text = "octave";
  }

  //play order
  if(cursor == 4){
    start = mid-(arp.playStyle)*12-5;
    for(uint8_t i = 0; i<6; i++){
      drawArpModeIcon(44,start+i*12,i,1);
    }
    graphics.drawArrow(56+(millis()/200)%2,mid,3,1,true);
    switch(arp.playStyle){
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
    drawArpModeIcon(44,mid-5,arp.playStyle,1);
  }
  display.setRotation(3);
  printSmall(16,60,text,1);
  display.setRotation(2);
  display.fillRoundRect(65,0,15,15,3,0);
  display.drawRoundRect(65,0,15,15,3,1);
  display.drawBitmap(67,2,gear_bmp,11,11,1);
}
void drawModBoxes(uint8_t cursor){
  printArp_wiggly(0,3,"settings",1);
  String text;
  const uint8_t mid = 40;
  //vel is special, needs a top and a bottom
  int8_t start = mid-arp.maxVelMod/10;
  int8_t end = mid+arp.minVelMod/10;
  int8_t length = end-start;
  display.fillRect(3,start,7,length,1);
  printSmall_centered(7,end+3,stringify(64-arp.minVelMod/2),1);
  printSmall_centered(7,start-7,stringify(arp.maxVelMod/2),1);
  if(cursor == 0){
    graphics.drawArrow(6,end+8+(millis()/200)%2,3,2,true);
    text = "velocity";
  }
  
  //chance
  display.fillRect(13,mid+13-arp.chanceMod/4,7,arp.chanceMod/4,1);
  printSmall_centered(17,mid+7-arp.chanceMod/4,stringify(arp.chanceMod),1);
  if(cursor == 1){
    graphics.drawArrow(16,mid+15+(millis()/200)%2,3,2,true);
    text = "chance";
  }

  //reps
  display.fillRect(23,mid+13-arp.repMod/4,7,arp.repMod/4,1);
  printSmall_centered(27,mid+7-arp.repMod/4,stringify(arp.repMod),1);
  if(cursor == 2){
    graphics.drawArrow(26,mid+15+(millis()/200)%2,3,2,true);
    text = "repeats";
  }

  //pitch is also special, needs a top and a bottom
  start = mid - arp.maxPitchMod/10;
  end = mid + arp.minPitchMod/10;
  length = end - start;
  display.fillRect(33,start,7,length,1);
  printSmall_centered(37,end+3,stringify(-arp.minPitchMod/16),1);
  printSmall_centered(37,start-7,stringify(arp.maxPitchMod/16),1);
  if(cursor == 3){
    graphics.drawArrow(36,end+8+(millis()/200)%2,3,2,true);
    text = "octave";
  }

  //play order
  if(cursor == 4){
    start = mid-(arp.playStyle)*12-5;
    for(uint8_t i = 0; i<6; i++){
      drawArpModeIcon(44,start+i*12,i,1);
    }
    graphics.drawArrow(56+(millis()/200)%2,mid,3,1,true);
    switch(arp.playStyle){
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
    drawArpModeIcon(44,mid-5,arp.playStyle,1);
  }
  printSmall(0,59,text,1);
}

void arpModMenu(){
  uint8_t cursor = 0;
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(50)){
       if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          switch(cursor){
            case 0:
              //editing min vel
              if(controls.SHIFT()){
                if(arp.minVelMod<117)
                  arp.minVelMod+=10;
                else
                  arp.minVelMod = 127;
              }
              else{
                if(arp.maxVelMod>10)
                  arp.maxVelMod-=10;
                else
                  arp.maxVelMod = 0;
              }
              lastTime = millis();
              break;
            case 1:
              //editing chance
              if(arp.chanceMod>10)
                arp.chanceMod-=10;
              else
                arp.chanceMod=0;
              lastTime = millis();
              break;
            case 2:
              //editing repeats
              if(arp.repMod>10)
                arp.repMod-=10;
              else
                arp.repMod = 0;
              lastTime = millis();
              break;
            //pitch
            case 3:
              if(controls.SHIFT()){
                if(arp.minPitchMod<117)
                  arp.minPitchMod+=10;
                else
                  arp.minPitchMod = 127;
              }
              else{
                if(arp.maxPitchMod>10)
                  arp.maxPitchMod-=10;
                else
                  arp.maxPitchMod = 0;
              }
              lastTime = millis();
              break;
            //play style
            case 4:
              if(arp.playStyle<5 && utils.itsbeen(200)){
                arp.playStyle++;
                lastTime = millis();
              }
              break;
          }
        }
        else if(controls.joystickY == -1){
          switch(cursor){
            case 0:
              //editing min vel
              if(controls.SHIFT()){
                if(arp.minVelMod>10)
                  arp.minVelMod-=10;
                else
                  arp.minVelMod = 0;
              }
              else{
                if(arp.maxVelMod<117)
                  arp.maxVelMod+=10;
                else
                  arp.maxVelMod = 127;
              }
              lastTime = millis();
              break;
            //editing chance
            case 1:
              if(arp.chanceMod<90)
                arp.chanceMod+=10;
              else
                arp.chanceMod = 100;
              lastTime = millis();
              break;
            //editing repeats
            case 2:
              if(arp.repMod<90)
                arp.repMod+=10;
              else
                arp.repMod = 100;
              lastTime = millis();
              break;
            //pitch
            case 3:
              if(controls.SHIFT()){
                if(arp.minPitchMod>10)
                  arp.minPitchMod-=10;
                else
                  arp.minPitchMod = 0;
              }
              else{
                if(arp.maxPitchMod<117)
                  arp.maxPitchMod+=10;
                else
                  arp.maxPitchMod = 127;
              }
              lastTime = millis();
              break;
            case 4:
              if(arp.playStyle>0 && utils.itsbeen(200)){
                arp.playStyle--;
                lastTime = millis();
              }
              break;
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        controls.setMENU(false) ;
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
      if(controls.DELETE()){
        switch(cursor){
          case 0:
            arp.maxVelMod = 0;
            arp.minVelMod = 0;
            lastTime = millis();
            break;
          case 1:
            arp.chanceMod = 100;
            lastTime = millis();
            break;
          case 2:
            arp.repMod = 0;
            lastTime = millis();
            break;
          case 3:
            arp.maxPitchMod = 0;
            arp.minPitchMod = 0;
            lastTime = millis();
            break;
          case 4:
            arp.playStyle = 0;
            lastTime = millis();
            break;
        }
      }
      while(controls.counterA != 0){
        switch(cursor){
          //min vel
          case 0:
            if(controls.counterA < 0 && arp.minVelMod<127){
              arp.minVelMod++;
            }
            else if(controls.counterA > 0 && arp.minVelMod>0){
              arp.minVelMod--;
            }
            controls.counterA += controls.counterA<0?1:-1;
            break;
          //chance
          case 1:
            if(controls.counterA > 0 && arp.chanceMod<100){
              arp.chanceMod++;
            }
            else if(controls.counterA < 0 && arp.chanceMod>0){
              arp.chanceMod--;
            }
            controls.counterA += controls.counterA<0?1:-1;
            break;
          //reps
          case 2:
            if(controls.counterA > 0 && arp.repMod<100){
              arp.repMod++;
            }
            else if(controls.counterA < 0 && arp.repMod>0){
              arp.repMod--;
            }
            controls.counterA += controls.counterA<0?1:-1;
            break;
          case 3:
            if(controls.counterA < 0 && arp.minPitchMod<127){
              arp.minPitchMod++;
            }
            else if(controls.counterA > 0 && arp.minPitchMod>0){
              arp.minPitchMod--;
            }
            controls.counterA += controls.counterA<0?1:-1;
            break;
          case 4:
            if(controls.counterA < 0){
              if(arp.playStyle > 0)
                arp.playStyle--;
            }
            else{
              if(arp.playStyle < 5)
                arp.playStyle++;
            }
            controls.counterA += controls.counterA<0?1:-1;
            break;
        }
      }
      while(controls.counterB != 0){
        switch(cursor){
          case 0:
            if(controls.counterB > 0 && arp.maxVelMod<127){
              arp.maxVelMod++;
            }
            else if(controls.counterB < 0 && arp.maxVelMod>0){
              arp.maxVelMod--;
            }
            controls.counterB += controls.counterB<0?1:-1;
            break;
          case 1:
            controls.counterB = 0;
            break;
          case 2:
            controls.counterB = 0;
            break;
          case 3:
            if(controls.counterB > 0 && arp.maxPitchMod<127){
              arp.maxPitchMod++;
            }
            else if(controls.counterB < 0 && arp.maxPitchMod>0){
              arp.maxPitchMod--;
            }
            controls.counterB += controls.counterB<0?1:-1;
            break;
        }
      }
    }
    display.clearDisplay();
    drawArpMenu(2,true);
    drawModBoxes(cursor);
    display.display();
  }
}
void drawArpModeIcon(uint8_t x1, int8_t y1, uint8_t which,uint16_t c){
  which%=6;
  display.fillRect(x1,y1,11,11,0);
  display.drawBitmap(x1,y1,arpMode_icons[which],11,11,c);
}
void drawArpMenu(uint8_t cursor, bool justKeys){

  drawKeys(38,14,getOctave(36),14,false);//always start on a C, for simplicity

  //last note played
  String lastNote = pitchToString(arp.lastPitchSent,true,true);
  graphics.printPitch(115,0,lastNote,false,false, 1);
  if(arp.holding && millis()%800>400){
    printSmall(106,9,"[HOLD]",1);
  }
  else if(!arp.holding){
    printSmall(110,9,"cpy 2",1);
      printSmall(112,15,"hold",1);
  }

  if(justKeys)
    return;
  const uint8_t y1 = 15;
  //on/off
  graphics.drawSlider(0,y1,"on","off",!arp.isActive);

  //step lengths
  String stepLength = arp.uniformLength?stepsToMeasures(arp.arpSubDiv):"custom";
  graphics.drawSlider(0,y1+20,"custom","uniform",arp.uniformLength);
  printSmall(0,y1+14,"length:" + stepLength,1);

  //modulation
  printArp(0,52,"MOD",1);

  graphics.fillSquareVertically(20,50,11,float(arp.maxVelMod*100)/float(127));
  printSmall(24,53,"v",2);

  graphics.fillSquareVertically(33,50,11,float(arp.chanceMod));
  printSmall(37,53,"%",2);

  graphics.fillSquareVertically(46,50,11,float(arp.repMod*100)/float(127));
  printSmall(50,53,"x",2);

  graphics.fillSquareVertically(59,50,11,float(arp.maxPitchMod*100)/float(127));
  printSmall(63,53,"$",2);

  //arp mode icon
  drawArpModeIcon(72,50,arp.playStyle,1);

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
  graphics.drawSmallChannelIcon(93,1,arp.channel);

  //input icon
  printSmall(50,1,"src:",1);
  switch(arp.source){
    //external
    case EXTERNAL:
      display.drawBitmap(64,0,tiny_midi_bmp,7,7,1);
      break;
    //internal
    case INTERNAL:
      display.drawBitmap(64,0,tiny_stepchild_bmp,7,7,1);
      break;
    //both
    case BOTH:
      display.drawBitmap(64,0,tiny_midi_bmp,7,7,1);
      printSmall(72,1,"+",1);
      display.drawBitmap(76,0,tiny_stepchild_bmp,7,7,1);
      break;
  }  
  //title
  printArp_wiggly(0,3,"arpeggi",1);
}

void drawArpMenu(uint8_t cursor){
  drawArpMenu(cursor,false);
}

//"notes" option adds notes from the 1scale onto whatever notes are playing
void arpMenu(){
  keyboardAnimation(38,14,0,14,true);
  int spacing = 4;
  //step boxes
  uint8_t cursor = 0;
  while(true){
    display.clearDisplay();
    drawArpMenu(cursor);
    display.display();

    //controls
    controls.readButtons();
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
            if(controls.joystickX == -1 && arp.isActive){
              arp.isActive = false;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && !arp.isActive){
              arp.isActive = true;
              lastTime = millis();
            }
            break;
          //step lengths
          case 1:
            if(controls.joystickX == 1 && arp.uniformLength){
              arp.uniformLength = false;
              lastTime = millis();
            }
            else if(controls.joystickX == -1 && !arp.uniformLength){
              arp.uniformLength = true;
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
      if(controls.SELECT() ){
        switch(cursor){
          //on/off
          case 0:
            arp.isActive = !arp.isActive;
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
            // arp.playStyle++;
            // arp.playStyle%=6;
            // lastTime = millis();
            break;
        }
      }
      //'hold' locks notes that are currently in the arp
      if(controls.COPY()){
        arp.holding = !arp.holding;
        lastTime = millis();
      }
      //swap between external/internal input
      if(controls.PLAY() || controls.NEW()){
        arp.source++;
        arp.source%=3;
        lastTime = millis();
      }
      if(controls.MENU()){
        lastTime = millis();
        controls.setMENU(false) ;
        break;
      }
      while(controls.counterA != 0){
        switch(cursor){
          case 0:
            arp.isActive = !arp.isActive;
            controls.counterA += controls.counterA<0?1:-1;;
            break;
          case 1:
            if(controls.SHIFT()){
              arp.arpSubDiv = sequence.toggleTriplets(arp.arpSubDiv);
              controls.counterA += controls.counterA<0?1:-1;;
            }
            else{
              if(controls.counterA > 0){
                arp.arpSubDiv = sequence.changeSubDiv(true,arp.arpSubDiv,false);
                controls.counterA += controls.counterA<0?1:-1;;
              }
              else{
                arp.arpSubDiv = sequence.changeSubDiv(false,arp.arpSubDiv,false);
                controls.counterA += controls.counterA<0?1:-1;;
              }
            }
            break;
          case 2:
            if(controls.counterA > 0){
              arp.playStyle++;
              if(arp.playStyle == 6)
                arp.playStyle = 0;
            }
            else if(controls.counterA < 0){
              if(arp.playStyle == 0)
                arp.playStyle = 5;
              else
                arp.playStyle--;
            }
            controls.counterA += controls.counterA<0?1:-1;;
            break;
        }
      }
      while(controls.counterB != 0){
        if(controls.counterB < 0 && arp.channel>1){
          arp.channel--;
        }
        else if(controls.counterB > 0 && arp.channel<16){
          arp.channel++;
        }
        lastTime = millis();
        controls.counterB += controls.counterB<0?1:-1;;
      }
    }
  }
  // keyboardAnimation();
}
