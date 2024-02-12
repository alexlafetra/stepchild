void drawPortIcon(uint8_t which){
  uint8_t x1,y1;
  switch(which){
    case 0:
      x1 = 64;
      y1 = 46;
      if(MIDI.midiChannelFilters[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),usb_logo_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),9,SSD1306_WHITE);
      drawDottedLineDiagonal(x1,y1+8,x1,35,3);
      break;
    case 1:
      x1 = 15;
      y1 = 7;
      if(MIDI.midiChannelFilters[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1+9,y1+8,x1+22,y1+8,3);
      break;
    case 2:
      x1 = 25;
      y1 = 37;
      if(MIDI.midiChannelFilters[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1+1,y1+10,x1+22,y1-4,3);
      break;
    case 3:
      x1 = screenWidth-25;
      y1 = 37;
      if(MIDI.midiChannelFilters[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1-2,y1+9,x1-22,y1-4,3);
      break;
    case 4:
      x1 = screenWidth-15;
      y1 = 7;
      if(MIDI.midiChannelFilters[which] != 0)
        display.drawBitmap(x1-8,y1+sin(millis()/100+which),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      display.drawCircle(x1,y1+8+sin(millis()/100+which),8,SSD1306_WHITE);
      drawDottedLineDiagonal(x1-8,y1+8,x1-22,y1+8,3);
      break;
  }
}
//a closeup menu on one port
void portMenu(uint8_t which){
  uint8_t cursors[3];
  uint8_t menuState = 0;
  //vu meter
  int16_t currentVel;
  int16_t lastVel;
  float VUval;
  uint8_t channelMenuStart = 0;
  while(true){
    display.clearDisplay();
    drawPortIcon(which);
    switch(which){
      case 0:
        drawVU(0,0,1-VUval);
        break;
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
    }
    display.display();
    //if something is being sent/received on this port
    bool something;
    for(uint8_t i = 0; i<receivedNotes.notes.size(); i++){
      if(MIDI.isChannelActive(receivedNotes.notes[i].channel,which)){
        currentVel = receivedNotes.notes[i].vel;
        something = true;
      }
    }
    if(!something)
      currentVel = 0;
    //set VU to the lastVel variable
    VUval = float(lastVel)/float(127);
    //update lastVel, incrementing it towards the currentVel
    lastVel = lastVel + (currentVel-lastVel)/5;

    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(del){
        lastTime = millis();
        MIDI.muteMidiPort(which);
      }
      if(n){
        lastTime = millis();
        MIDI.unmuteMidiPort(which);
      }
      if(sel){
        MIDI.toggleMidiPort(which);
        lastTime = millis();
      }
    }
  }
}

void thruMenu(){ //controls which midi port you're editing
  uint8_t xCursor = 1;
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1 && xCursor<4){
          xCursor++;
          lastTime = millis();
        }
        else if(x == 1 && xCursor>0){
          xCursor--;
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        bool isActive = MIDI.toggleThru(xCursor);
        if(shift){
          for(int i = 0; i<5; i++){
            MIDI.setThru(i, isActive);
          }
        }
        lastTime = millis();
      }
    }
    display.clearDisplay();
    // printSmall(40,48,"Thru routing",SSD1306_WHITE);
    printCursive(22,48, "set thru ports",SSD1306_WHITE);
    uint8_t xOffset = 6;
    for(uint8_t midiPort = 0; midiPort<5; midiPort++){
      if(midiPort == 0){
        printSmall(xOffset+3+midiPort*25,0,"USB",SSD1306_WHITE);
        if(xCursor == 0)
          display.drawBitmap(xOffset+midiPort*25,8+2*sin(midiPort+millis()/100),usb_logo_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xOffset+midiPort*25,8,usb_logo_bmp,17,17,SSD1306_WHITE);
      }
      else{
        printSmall(xOffset+7+midiPort*25,0,stringify(midiPort),SSD1306_WHITE);
        if(xCursor == midiPort)
          display.drawBitmap(xOffset+midiPort*25,8+2*sin(midiPort+millis()/100),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
        else
          display.drawBitmap(xOffset+midiPort*25,8,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      if(xCursor == midiPort)
        display.drawCircle(xOffset+midiPort*25+8,8+2*sin(midiPort+millis()/100)+8,8,SSD1306_WHITE);
      if(MIDI.isThru(midiPort)){
        display.fillRect(xOffset+midiPort*25,34,17,7,SSD1306_WHITE);
        printSmall(xOffset+1+midiPort*25,35,"Thru",SSD1306_BLACK);
      }
      else{
        printSmall(xOffset+2+midiPort*25,35,"off",SSD1306_WHITE);
      }
    }
    display.display();
  }
}

//this is a complex menu! it has two modes, and a very custom selection screen
void midiMenu(){
  uint8_t xCursor = 0;
  uint8_t yCursor = 0;
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      if(sel){
        lastTime = millis();
        //center menu mode
        if(yCursor == 0){
          switch(xCursor){
            //thru
            case 0:
              routeMenu();
              break;
            //CV
            case 1:
              CVMenu();
              break;
            case 2:
              thruMenu();
              break;
          }
        }
        else{
          // portMenu(xCursor);
          MIDI.toggleMidiPort(xCursor);
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        break;
      }
      if(play){
        lastTime = millis();
        togglePlayMode();
      }
      if(del && yCursor == 0){
        MIDI.muteMidiPort(xCursor);
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(y != 0){
        //center menu mode
        if(yCursor == 0){
          if(y == 1){
            if(xCursor == 2){
              xCursor = 0;
              yCursor = 1;
              lastTime = millis();
            }
            else{
              xCursor++;
              lastTime = millis();
            }
          }
          if(y == -1 && xCursor>0){
            xCursor--;
            lastTime = millis();
          }
        }
        //midi mute mode
        else{
          //moving up
          if(y == -1){
            //if it's on the center, jump back up to menu
            if(xCursor == 0){
              yCursor = 0;
              xCursor = 2;
              lastTime = millis();
            }
            //if not, then increment the cursor
            else if(xCursor == 3){
              xCursor = 4;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 1;
              lastTime = millis();
            }
          }
          //moving down
          else if(y == 1){
            if(xCursor == 1){
              xCursor = 2;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 4){
              xCursor = 3;
              lastTime = millis();
            }
          }
        }
      }
      //x jumps you off of and back into center menu
      if(x != 0){
        //if you were in center menu, jump off to the left or right
        if(yCursor == 0){
          yCursor = 1;
          if(x == -1){
            xCursor = 4;
          }
          else if(x == 1){
            xCursor = 1;
          }
          lastTime = millis();
        }
        //if you're in the mute menu
        else{
          //right
          if(x == -1){
            //go back to normal mode
            if(xCursor == 1){
              yCursor = 0;
              xCursor = 0;
              lastTime = millis();
            }
            //valid moves
            else if(xCursor == 0){
              xCursor = 3;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 3){
              xCursor = 4;
              lastTime = millis();
            }
          }
          //left
          else if(x == 1){
            //go back to normal mode
            if(xCursor == 4){
              yCursor = 0;
              xCursor = 0;
              lastTime = millis();
            }
            else if(xCursor == 0){
              xCursor = 2;
              lastTime = millis();
            }
            else if(xCursor == 2){
              xCursor = 1;
              lastTime = millis();
            }
            else if(xCursor == 3){
              xCursor = 0;
              lastTime = millis();
            }
          }
        }
      }
    }
    //getting active channels/pitches to draw note icons
    vector<vector<uint8_t>> activeChannels;
    for(uint8_t track = 0; track<trackData.size(); track++){
      if(trackData[track].noteLastSent != 255)
        activeChannels.push_back({trackData[track].channel,trackData[track].noteLastSent});
    }

    display.clearDisplay();

    uint8_t x1;
    uint8_t y1;
    //drawing each midi port icon and info
    for(uint8_t port = 0; port<5; port++){
      bool isCurrentlySending = false;
      uint8_t currentPitch;
      //drawing note icon for actively sending/rxing ports
      for(uint8_t i = 0; i<activeChannels.size(); i++){
        if(MIDI.isChannelActive(activeChannels[i][0],port)){
          isCurrentlySending = true;
          currentPitch = activeChannels[i][1];
        }
      }
      //getting the right coords for each port
      switch(port){
        //usb
        case 0:
        //usb is in the middle
          x1 = screenWidth/2;
          y1 = 46;
          drawDottedLineDiagonal(x1,y1+8,x1,35,3);
          break;
        //1 and 2 are on left
        case 1:
          x1 = 15;
          y1 = 7;
          drawDottedLineDiagonal(x1+9,y1+8,x1+22,y1+8,3);
          break;
        case 2:
          x1 = 25;
          y1 = 37;
          drawDottedLineDiagonal(x1+1,y1+10,x1+22,y1-4,3);
          break;
        //3 and 4 are on right
        case 3:
          x1 = screenWidth-25;
          y1 = 37;
          drawDottedLineDiagonal(x1-2,y1+9,x1-22,y1-4,3);
          break;
        case 4:
          x1 = screenWidth-15;
          y1 = 7;
          drawDottedLineDiagonal(x1-8,y1+8,x1-22,y1+8,3);
          break;
      }
      if(playing){
        uint8_t x2 = 95;
        uint8_t y2 = 4;
        display.fillTriangle(x2+((millis()/200)%2)+1,y2+6,x2+((millis()/200)%2)+1,y2,x2+6+((millis()/200)%2)+1,y2+3,SSD1306_WHITE);
      }
      //draw feed line
      //if it's currently sending a note, then draw the note pitch instead of the port #
      //AND the port bounces faster
      if(isCurrentlySending){
        //drawing bitmaps
        if(port == 0){
          //note icon
          display.drawChar(x1-5,y1-5+2*sin(millis()/100+port),0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
          //pitch
          printSmall(x1+1,y1-5+2*sin(millis()/100+port),pitchToString(currentPitch,true,true),SSD1306_WHITE);
          //bitmap
          if(!MIDI.isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+2*sin(millis()/100+port),usb_logo_bmp,17,17,SSD1306_WHITE);
          else if(xCursor != port)
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,SSD1306_WHITE);
          //'usb' doesn't display (it's replaced by note)
          // printSmall(x1-5,y1-4+2*sin(millis()/100+port),"usb",SSD1306_WHITE);
        }
        else{
          //note icon
          display.drawChar(x1-5,y1+18+2*sin(millis()/100+port),0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
          //pitch
          printSmall(x1+1,y1+18+2*sin(millis()/100+port),pitchToString(currentPitch,true,true),SSD1306_WHITE);
          //bitmap
          if(!MIDI.isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+2*sin(millis()/100+port),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
          else if(xCursor != port)
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,SSD1306_WHITE);
          //number
          printSmall(x1-1,y1-4+2*sin(millis()/100+port),stringify(port),SSD1306_WHITE);
        }
      }
      //if it's not sending, the ports have numbers and bounce slower
      else{
        //number
        if(port == 0){
          if(!MIDI.isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+sin(millis()/100+port),usb_logo_bmp,17,17,SSD1306_WHITE);
          else if(xCursor != port){
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),9,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,1);
          }
          printSmall(x1-5,y1-5+sin(millis()/100+port),"usb",SSD1306_WHITE);
        }
        else{
          if(!MIDI.isTotallyMuted(port))
            display.drawBitmap(x1-8,y1+sin(millis()/100+port),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
          else if(xCursor != port){
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),8,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,1);
          }
          printSmall(x1-1,y1-4+sin(millis()/100+port),stringify(port),SSD1306_WHITE);
        }
      }

      //if it's in mute mode, draw a circle around the active port
      if(yCursor == 1 && xCursor == port){
        if(isCurrentlySending){
          drawArrow(x1+9+2*sin(millis()/100+port),y1+8+((millis()/200)%2),2,1,true);
          //for the usb port, it's a little bigger
          if(port == 0){
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),9,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),9,SSD1306_WHITE);
          }
          else{
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),8,0);
            display.drawCircle(x1,y1+8+2*sin(millis()/100+port),8,SSD1306_WHITE);
          }
        }
        else{
          //usb port is a little bigger
          if(port == 0){
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),9,0);
            display.drawCircle(x1,y1+8+sin(millis()/100+port),9,SSD1306_WHITE);
            drawArrow(x1+9+((millis()/200)%2),y1+8+((millis()/200)%2),2,1,true);
          }
          else{
            display.fillCircle(x1,y1+8+2*sin(millis()/100+port),8,0);
            display.drawCircle(x1,y1+8+sin(millis()/100+port),8,SSD1306_WHITE);
            drawArrow(x1,y1+18+((millis()/200)%2),2,2,true);
          }
        }
      }
    }

    x1 = 51;
    y1 = 10;
    //menu title
    display.setRotation(1);
    printChunky(42-y1,x1-12,"MIDI",2);
    display.setRotation(UPRIGHT);

    display.setRotation(3);
    printChunky(y1-1,x1-13,"MENU",2);
    display.setRotation(UPRIGHT);
    //bounding box
    //menu options
    printSmall(x1,y1,"Routing",SSD1306_WHITE);
    printSmall(x1,y1+8,"CV",SSD1306_WHITE);
    printSmall(x1,y1+16,"Thru",SSD1306_WHITE);
      
    //center menu mode
    if(yCursor == 0){
      //bounding box
      display.drawRoundRect(x1-3,y1-3,33,27,3,SSD1306_WHITE);
      //selection arrows
      switch(xCursor){
        //port routing
        case 0:
          drawArrow(x1+29+((millis()/200)%2),y1+2,3,1,false);
          // display.drawRoundRect(54+((millis()/200)%2),40,22-int(2*cos(millis()/100)),9,3,SSD1306_WHITE);
          break;
        //input
        case 1:
          drawArrow(x1+29+((millis()/200)%2),y1+10,3,1,false);
          // display.drawRoundRect(32+((millis()/200)%2),47,66-int(2*cos(millis()/100)),9,3,SSD1306_WHITE);
          break;
        //clock source
        case 2:
          drawArrow(x1+29+((millis()/200)%2),y1+18,3,1,false);
          break;
      }
    }
    display.display();
  }
}

uint16_t midiChannelFilter = 65535;
uint8_t midiMessageFilter = 255;

void toggleFilter_channel(uint8_t channel){
  bool state = midiChannelFilter & (1<<channel);
  uint16_t mask = 1<<channel;//a 1 in the target place

  //invert mask if you're turning it off
  if(state){
    mask = ~mask;
    midiChannelFilter &= mask;
  }
  else{
    midiChannelFilter |= mask;
  }
}
void toggleFilter_message(uint8_t message){
  bool state = midiMessageFilter & (1<<message);
  uint8_t mask = 1<<message;//a 1 in the target place

  //invert mask if you're turning it off
  if(state){
    mask = ~mask;
    midiMessageFilter &= mask;
  }
  else{
    midiMessageFilter |= mask;
  }
}
//input menu can set a filter on the input
//two filters: one for channels, one for kinds of messages
//kinds of messages:
/*
- note
- cc
-clock (start, stop, timeframe)
*/
void inputMenu(){
  //for channel filter
  uint8_t x1 = 61;
  uint8_t y1 = 10;
  //for control filter
  uint8_t x2 = 90;
  uint8_t y2 = 35;

  uint8_t xCursor = 0;
  uint8_t yCursor = 0;
  uint8_t menuStart[2] = {0,0};

  const uint8_t maxPorts = 7;

  while(true){
    display.clearDisplay();
    //printing the channel filter
    for(int i = 0; i<maxPorts; i++){
      //print channel numbers
      printSmall(x1+12, y1+i*6, stringify(i+menuStart[0]+1),SSD1306_WHITE);
      //if the channel is 
      if(midiChannelFilter & (1<<(i+menuStart[0]))){
        //if this box is cursore'd, AND if it's the active midi port
        if(i == yCursor && xCursor == 0)
          drawCheckbox(x1+1, y1+i*6, true, true);
        else
          drawCheckbox(x1+1, y1+i*6, true, false);
      }
      else{
        if(i == yCursor && xCursor == 0)
          drawCheckbox(x1+1, y1+i*6, false, true);
        else
          drawCheckbox(x1+1, y1+i*6, false, false);
      }
    }
    // const vector<String> labels = {"note:","cc:","clock:"};
    const unsigned char* icons[3] = {epd_bitmap_small_note, small_clock, cc_small};
    //printing the control filter
    for(uint8_t i = 0; i<3; i++){
      display.drawBitmap(x2+11,y2+i*10+1,icons[i],5,5,SSD1306_WHITE);
      if(midiMessageFilter & (1<<i)){
        if(yCursor == i && xCursor == 1)
          drawCheckbox(x2+1, y2+i*10, true, true);
        else
          drawCheckbox(x2+1, y2+i*10, true, false);
      }
      else{
        if(yCursor == i && xCursor == 1)
          drawCheckbox(x2+1, y2+i*10, false, true);
        else
          drawCheckbox(x2+1, y2+i*10, false, false);
      }
    }
    int8_t bigOffset;
    if(isReceivingOrSending())
      bigOffset = 2*((millis()/200)%2);
    else
      bigOffset = 2*sin(millis()/300);
    display.drawBitmap(-12,-4+bigOffset,big_midi_inverse,45,45,SSD1306_WHITE);
    printCursive(69,0,"midi",SSD1306_WHITE);
    printCursive(83,8,"filters",SSD1306_WHITE);
    if(xCursor == 0){
      display.setRotation(1);
      printChunky(screenHeight-y1-45,x1-8,"channels",SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      drawDottedLineDiagonal(21,16+bigOffset,41,36+bigOffset,3);
      drawDottedLineDiagonal(41,36+bigOffset,50,36+bigOffset,3);
    }
    else if(xCursor == 1){
      display.setRotation(1);
      printChunky(screenHeight-y2-28,x2-8,"types",SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      drawDottedLineDiagonal(10,32+bigOffset,10,58+bigOffset,3);
      drawDottedLineDiagonal(10,56+bigOffset,x2-10,56+bigOffset,3);
    }
    display.display();

    readButtons();
    readJoystick();
    //controls
    if(itsbeen(200)){
      //moving xCursor
      if(x != 0){
        xCursor = !xCursor;
        if(xCursor == 1 && yCursor>=3){
          yCursor = 2;
        }
        lastTime = millis();
      }
      //exit
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        if(shift){

        }
        else{
          //toggle channel filter
          if(xCursor == 0){
            toggleFilter_channel(yCursor+menuStart[0]);
            lastTime = millis();
          }
          //toggle message filter
          else if(xCursor == 1){
            toggleFilter_message(yCursor);
            lastTime = millis();
          }
        }
      }
    }
    //moving vertical cursor
    if(itsbeen(80)){
      if(y != 0){
        if(y == 1){
          //for channel filter
          if(xCursor == 0){
            //if the cursor is already at the end of the menu, move that ish
            if(yCursor == maxPorts-1 && menuStart[0]<(16-maxPorts)){
              menuStart[0]++;
              lastTime = millis();
            }
            //if it's not at the end of the menu, move the cursor
            if(yCursor<maxPorts-1){
              yCursor++;
              lastTime = millis();
            }
          }
          //for message filter
          else if(xCursor == 1){
            if(yCursor<2){
              yCursor++;
              lastTime = millis();
            }
          }
        }
        else if(y == -1){
          //for channel filter
          if(xCursor == 0){
            //if the cursor is already at the end of the menu, move that ish
            if(yCursor == 0 && menuStart[0]>0){
              menuStart[0]--;
              lastTime = millis();
            }
            //if it's not at the end of the menu, move the cursor
            if(yCursor>0){
              yCursor--;
              lastTime = millis();
            }
          }
          //message filter
          else if(xCursor == 1){
            if(yCursor>0){
              yCursor--;
              lastTime = millis();
            }
          }
        }
      }
    }
  }
}

void routeMenu(){
  //controls which midi port you're editing
  uint8_t xCursor = 1;
  //controls which channel (relative to menuStart) you're looking at
  uint8_t yCursor = 0;
  //controls where each of the five menus starts
  uint8_t menuStart[5] = {0,0,0,0,0};
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(80)){
      if(y != 0){
        if(y == 1){
          //if the cursor is already at the end of the menu, move that ish
          if(yCursor == 4 && menuStart[xCursor]<11){
            menuStart[xCursor]++;
            lastTime = millis();
          }
          //if it's not at the end of the menu, move the cursor
          if(yCursor<4){
            yCursor++;
            lastTime = millis();
          }
        }
        else if(y == -1){
          //if the cursor is already at the end of the menu, move that ish
          if(yCursor == 0 && menuStart[xCursor]>0){
            menuStart[xCursor]--;
            lastTime = millis();
          }
          //if it's not at the end of the menu, move the cursor
          if(yCursor>0){
            yCursor--;
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1 && xCursor<4){
          xCursor++;
          lastTime = millis();
        }
        else if(x == 1 && xCursor>0){
          xCursor--;
          lastTime = millis();
        }
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        break;
      }
      if(sel){
        bool isActive = MIDI.toggleMidiChannel(yCursor+menuStart[xCursor]+1,xCursor);
        // //Serial.println("toggling "+stringify(yCursor+menuStart[xCursor])+" on out "+stringify(xCursor));
        if(shift){
          for(int i = 0; i<16; i++){
            MIDI.setMidiChannel(i+1,xCursor,isActive);
          }
        }
        lastTime = millis();
      }
    }
    display.clearDisplay();
    uint8_t xOffset = 2;
    for(uint8_t midiPort = 0; midiPort<5; midiPort++){
      uint8_t yCoord = 8;
      if(midiPort == 0){
        printSmall(xOffset+3+midiPort*25,0,"USB",SSD1306_WHITE);
        if(xCursor == 0)
          yCoord = 8+2*sin(midiPort+millis()/100);
        if(!MIDI.isTotallyMuted(0))
          display.drawBitmap(xOffset+midiPort*25,yCoord,usb_logo_bmp,17,17,SSD1306_WHITE);
      }
      else{
        printSmall(xOffset+7+midiPort*25,0,stringify(midiPort),SSD1306_WHITE);
        if(xCursor == midiPort)
          yCoord = 8+2*sin(midiPort+millis()/100);
        if(!MIDI.isTotallyMuted(midiPort))
          display.drawBitmap(xOffset+midiPort*25,yCoord,MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
      }
      if(xCursor == midiPort)
        display.drawCircle(xOffset+midiPort*25+8,8+2*sin(midiPort+millis()/100)+8,8,SSD1306_WHITE);

      for(int i = 0; i<5; i++){
        //print channel numbers
        printSmall(xOffset+12+midiPort*25, 30+i*6, stringify(i+menuStart[midiPort]+1),SSD1306_WHITE);
        //print channel values
        //if this box is cursore'd, AND if it's the active midi port
        if(i == yCursor && xCursor == midiPort)
          drawCheckbox(xOffset+1+midiPort*25, 29+i*6, MIDI.isChannelActive(i+menuStart[midiPort]+1,midiPort), true);
        else
          drawCheckbox(xOffset+1+midiPort*25, 29+i*6, MIDI.isChannelActive(i+menuStart[midiPort]+1,midiPort), false);
      }
      //"more channels" indicators
      //more channels above
      if(menuStart[midiPort]>0){
        drawArrow(xOffset+17+midiPort*25,26+((millis()/400)%2),2,2,true);
      }
      //more channels below
      if(menuStart[midiPort]<11){
        drawArrow(xOffset+17+midiPort*25,63-((millis()/400)%2),2,3,true);
      }
    }
    display.display();
  }
}