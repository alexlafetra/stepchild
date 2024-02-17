//Debug -------------------------------------------------------------------------
//counts notes
void debugNoteCount() {
  int totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes += seqData[i].size()-1;
  }
  //Serial.print(totalNotes + " notes stored in memory\n");
}

//prints the selection buffer, doe
void debugPrint() {
  //Serial.println("Printing out seqData...");
  for(int i = 0; i<trackData.size(); i++){
    for(int j = 1; j<seqData[i].size()-1; j++){
      //Serial.println("+------------+");
      //Serial.print("id: ");
      //Serial.println(j);
      //Serial.print("start: ");
      //Serial.println(seqData[i][j].startPos);
      //Serial.print("end: ");
      //Serial.println(seqData[i][j].endPos);
    }
  }
  //Serial.print("total notes on this track: ");
  //Serial.print(int(seqData[activeTrack].size()-1));
  //Serial.print("total notes in the sequence: ");
  int totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes = totalNotes + seqData[i].size()-1;
  }
  //Serial.print(totalNotes);
  //Serial.print("\nNotes in data: ");
  totalNotes = 0;
  for (int i = 0; i < trackData.size(); i++) {
    totalNotes = seqData[i].size() - 1 + totalNotes;
  }
  //Serial.print(totalNotes);
}

//prints out each bit of the byte
void printByte(uint8_t b){
  // Serial.print("\n");
  uint8_t i = 0;
  while(i<8){
    // Serial.print(b&1);
    b = b>>1;
    i++;
  }
}

//fills sequence with notes
void debugFillSeq() {
  //Serial.print("filling with notes\n");
  for (int i = 0; i < trackData.size(); i++) {
    for (int j = 0; j < 254; j++) {
      makeNote(i, j, subDivInt, 0);
    }
  }
  debugNoteCount();
}

//prints out memory allocation
void debugPrintMem(bool verby){
  if(verby){
    //Serial.print("tracks:");
    //Serial.println(seqData.size());
    //Serial.print("notes:");
    for(int i = 0; i<trackData.size(); i++){
      //Serial.print(i);
      //Serial.print(" - ");
      //Serial.println(seqData[i].size());
    }
  }
  //Serial.print("free:");
  //Serial.println(rp2040.getFreeHeap());
  //Serial.print("used:");
  //Serial.println(rp2040.getUsedHeap());
  // //Serial.print("total:");
  // //Serial.println(rp2040.getTotalHeap());
  //Serial.print("% used:");
  //Serial.println(float(rp2040.getUsedHeap())/float(rp2040.getTotalHeap()));
}

void debugPrintSelection(){
  //Serial.print("#:");
  //Serial.println(selectionCount);
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note<=seqData[track].size()-1; note++){
      if(seqData[track][note].isSelected){
        //Serial.print("note ");
        //Serial.print(note);
        //Serial.print(" on track ");
        //Serial.println(track);
        //Serial.print("[");
        //Serial.print(seqData[track][note].startPos);
        //Serial.print(",");
        //Serial.print(seqData[track][note].endPos);
        //Serial.println("]");
      }
    }
  }
}

//These two are both still pretty useful for debugging
void testLEDs(){
  bool leds[8] = {0,0,0,0,0,0,0,0};
  for(int i = 0; i<8; i++){
    if(step_buttons[i]){
      leds[i] = 1;
    }
  }
  writeLEDs(leds);
}

void testJoyStick(){
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(200) && menu_Press){
      lastTime = millis();
      return;
    }
    float X = analogRead(x_Pin);
    float Y = analogRead(y_Pin);
    display.clearDisplay();
    printSmall(0,0,"X: "+stringify(X)+","+stringify(x),1);
    printSmall(0,10,"Y: "+stringify(Y)+","+stringify(y),1);
    display.drawCircle(X/8.0,Y/16.0,4,1);
    display.display();
  }
}

//this is the big switch statement that listens for key inputs and runs the according functions
void keyListen() {
  unsigned char key = 0;
  String pitch;
  int track;
  key = Serial.read();
  switch (key) {
    case 'k':
      activeArp.start();
      break;
    case 'z':
      debugPrintSelection();
      break;
    case 'F':
      menuIsActive = true;
//      genFragment();
      // fragmentMenu();
      break;
    case 'C':
      copy();
      break;
    case 'N':
      constructMenu("EDIT");
      menuIsActive = true;
      break;
    case 'I':
      //Serial.println(decimalToNumeral(11));
      break;
    case 'c':
      break;
    case '`':
//      updateLookupData();
      displaySeqSerial();
      break;
    case 'u':
      track = getTrackWithPitch(24);
      //Serial.println(track);
      break;
    case '.'://>
      moveCursor(subDivInt);
      displaySeqSerial();
      break;
    case 'f':
      debugPrintLookup();
      break;
    case ','://<
      moveCursor(-subDivInt);
      displaySeqSerial();
      break;
    case '>'://precise movement
      moveCursor(1);
       displaySeqSerial();
      break;
    case '<':
      moveCursor(-1);
        displaySeqSerial();
      break;
    case 'n':
      //Serial.print("writing a note...\n");
      makeNote(activeTrack, cursorPos, subDivInt, 1); //default makes an 8th note
      displaySeqSerial();
      break;
    case ']':
      setActiveTrack(activeTrack - 1, false);
        displaySeqSerial();
      break;
    case '[':
      setActiveTrack(activeTrack + 1, false);
        displaySeqSerial();
      break;
    case 's'://select a note
        toggleSelectNote(activeTrack,getIDAtCursor(),true);
        displaySeqSerial();
      break;
    case 'p'://debug print selected notes
      // debugPrint();
        // displaySeqSerial();
      break;
    case 't': //add track (tricky! big moment if this works first time) LMAO
      addTrack(defaultPitch);
      defaultPitch++;
        displaySeqSerial();
      break;
    case 'd'://deleting the selected note(s)
      //Serial.print("attempting to delete ");
      if (selectionCount > 0) {
        //Serial.print("all selected notes...\n");
        deleteSelected();
      }
      else {
        //Serial.print("1 note...\n");
        deleteNote();
      }
      displaySeqSerial();
      break;
    case 'e'://erase a track
      eraseTrack();
        displaySeqSerial();
      break;
    case 'm':
      menuIsActive = true;
      break;
    case '1':
      constructMenu("MENU");
      mainMenu();
      break;
    case '2'://counts notes
      constructMenu("ARP");
      break;
    case '3'://counts notes
      echoMenu();
      break;
    case '4'://counts notes
      constructMenu("CURVE");
      break;
    case 'a'://selects all in track
      break;
    case ';':
      loopData[activeLoop].end -= 16;
        displaySeqSerial();
      break;
    case '\'':
      loopData[activeLoop].end += 16;
    case 'r':
      toggleRecordingMode(waitForNoteBeforeRec);
      break;
    case 'R':
        displaySeqSerial();
      break;
    case 'P':
      togglePlayMode();
      break;
    case '_':
        displaySeqSerial();
      break;
    case '+':
      zoom(true);
      break;
    case '-':
      zoom(false);
      break;
  }
}
void debugButtonPrint(){
  if(n)
    //Serial.println("n");
  if(sel)
    //Serial.println("sel");
  if(shift)
    //Serial.println("shift");
  if(del)
    //Serial.println("del");
  if(loop_Press)
    //Serial.println("loop");
  if(play)
    //Serial.println("play");
  if(copy_Press)
    //Serial.println("copy");
  if(menu_Press)
    //Serial.println("menu");

  for(int i = 0; i<8; i++){
    if(step_buttons[i]){
      //Serial.println("step "+stringify(i));
    }
  }

  if(x != 0){
    //Serial.println("x:"+stringify(x));
  }
  if(y != 0){
    //Serial.println("y:"+stringify(y));
  }
}

void zella(){
  WireFrame w = makeBox(10,10,10);
  w.xPos = 32;
  w.yPos = screenHeight/2;
  w.rotate(45,2);
  w.rotate(45,0);
  w.scale = 2;
  WireFrame z = makeBox(10,10,10);
  z.xPos = 96;
  z.yPos = screenHeight/2;
  z.rotate(45,2);
  z.rotate(45,0);
  z.scale = 2;
  while(true){
    w.rotate(5,1);
    z.rotate(5,1);
    display.clearDisplay();
    display.drawBitmap(0,0,zella_bmp,128,64,SSD1306_WHITE);
    w.render();
    z.render();
    display.display();
  }
}

void noBitches(){
    while(true){
    display.clearDisplay();
    display.drawBitmap(4,0,no_bitches_bmp,116,64,SSD1306_WHITE);
    display.display();
  }
}




//prints out lookupdata
void debugPrintLookup(){
  for(int i = 0; i<trackData.size();i++){
    //Serial.print(stringify(i)+":"+getTrackPitch(i));
    //Serial.print('|');
    for(int j = 0; j<= seqEnd; j++){
      // if(!j%subDivInt)
      //   //Serial.print('|');
      if(lookupData[i][j]==0){
        //Serial.print('-');
      }
      else{
        //Serial.print(lookupData[i][j]);
      }
    }
    //Serial.println('|');
  }
}

void debugFillSequence(){
  for(uint16_t step = 0; step<seqEnd; step++){
    Note debugNote(step,step,127,100,false,false);
    for(uint8_t track = 0; track<trackData.size(); track++){
      makeNote(debugNote,track,false);
    }
  }
}

//fills sequence with notes
void debugFillTrack(int division) {
  //Serial.print("placing note every ");
  //Serial.print(division);
  //Serial.print(" slices...\n");
  for (int j = 0; j < 254; j += division) {
    makeNote(activeTrack, j, subDivInt, 0);
  }
}

void debugTestSwing(){
  long int i;
  swingSubDiv = 24;//whole note swing
  long int  time;
  while(true){
    float t = swingOffset(i);
    i++;
    //Serial.print("step: ");
    //Serial.println(i);
    //Serial.print("swing offset: ");
    //Serial.println(t);
    //Serial.print("w/o swing: ");
    //Serial.println(MicroSperTimeStep);
    //Serial.print("w/swing: ");
    //Serial.println(t+MicroSperTimeStep);
    i%=96;
  }
}
void debugPrintCopyBuffer(){
  for(int track = 0; track<trackData.size(); track++){
    for(int i = 0; i<copyBuffer[track].size(); i++){
      //Serial.print("note ");
      //Serial.println(i);
      //Serial.print("s:");
      //Serial.println(copyBuffer[track][i].startPos);
      //Serial.print("e:");
      //Serial.println(copyBuffer[track][i].endPos);
      //Serial.print("on track:");
      //Serial.println(track);
    }
  }
}

//cassette-vibe buttons for the playBack menu
void drawCassetteButton(uint8_t x1, uint8_t y1, uint8_t which, bool state){
  // const uint8_t width = 20;
  const uint8_t width = 12;
  const uint8_t height = 12;

  drawBox(x1, y1, width, height, 2, -2, state?2:1);
  if(state)
    display.drawFastHLine(x1,y1+height+1,width,1);
  switch(which){
    //play
    case 0:
      // display.drawTriangle(x1+8,y1+height/2+2,x1+1,y1+5,x1+1,y1+11,state?0:1);
      if(state)
        display.drawTriangle(x1+8,y1+height/2+2,x1+1,y1+5,x1+1,y1+11,0);
      else
        display.fillTriangle(x1+6,y1+height/2+2,x1+2,y1+6,x1+2,y1+10,1);
      break;
    //rec
    case 1:
      if(state)
        display.drawCircle(x1+width/2-2,y1+height/2+2,3,0);
      else
        display.fillCircle(x1+width/2-2,y1+height/2+2,2,1);
      break;
    //stop
    case 2:
      if(state)
        display.drawRect(x1+1,y1+5,7,7,0);
      else
        display.fillRect(x1+2,y1+6,5,5,1);
      break;
  }
}

void drawPlaybackIcon(uint8_t x1, uint8_t y1, uint8_t which){
  uint8_t reps = (millis()/100)%16;
  if(reps>5){
    reps = 5;
  }
  const int8_t xGap = -6;
  const int8_t yGap = 0;

  const uint8_t width = 12;
  const uint8_t height = 12;
  switch(which){
    //play
    case 0:
      for(uint8_t i = 0; i<reps; i++){
        y1+=sin(PI*i);
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillTriangle(x1+xOffset,y1-yOffset,x1+width+xOffset,y1+height/2-yOffset,x1+xOffset,y1+height-yOffset,0);
        display.drawTriangle(x1+xOffset,y1-yOffset,x1+width+xOffset,y1+height/2-yOffset,x1+xOffset,y1+height-yOffset,1);
      }
      break;
    //rec
    case 1:
      // x1+=width/2;
      y1+=height;
      for(uint8_t i = 0; i<reps; i++){
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillCircle(x1+xOffset+width/2,y1-yOffset-height/2,width/2,0);
        display.drawCircle(x1+xOffset+width/2,y1-yOffset-height/2,width/2,1);
      }
      break;
    //stop
    case 2:
      for(uint8_t i = 0; i<reps; i++){
        int8_t xOffset = -i*xGap;
        int8_t yOffset = -i*yGap;
        display.fillRect(x1+xOffset,y1-yOffset,width,height,0);
        display.drawRect(x1+xOffset,y1-yOffset,width,height,1);
      }
      break;
  }
}

void printPlaybackOptions(uint8_t which, uint8_t cursor, bool active){
  drawPlaybackIcon(40,0,which-1);
  const uint8_t x1 = 20;
  uint8_t y1 = 14;
  switch(which){
    //play options
    case 1:
      printSmall(x1,y1+1,"loop:",1);
      //loop on/off
      if(isLooping){
        printSmall(x1+24,y1+1,"on",1);
      }
      else{
        printSmall(x1+24,y1+1,"off",1);
      }
      printSmall(x1,y1+11,"clock source:",1);
      //external clock
      if(clockSource == EXTERNAL_CLOCK){
        printSmall(x1+56,y1+11,"external",1);
      }
      else{
        printSmall(x1+56,y1+11,"internal",1);
      }
      //highlight
      if(active){
        switch(cursor){
          case 0:
            display.fillRoundRect(x1+22,y1-1,isLooping?11:15,9,3,2);
            drawArrow(x1+37+((millis()/200)%2),y1+3,3,1,false);
            break;
          case 1:
            display.fillRoundRect(x1+54,y1+9,35,9,3,2);
            drawArrow(x1+91+((millis()/200)%2),y1+13,3,1,false);
            break;
        }
      }
      break;
    //rec options
    case 2:
      //prime tracks
      //make new tracks
      //count-in (or wait for a note) "begin after"
      //rec until the end of current loop, rec over the current loop only, rec continuously through the loops, or rec for an arbitrary number of steps
      break;
    //stop options
    case 3:
      //menu cursor
      if(active){
        switch(cursor){
          case 0:
            drawArrow(x1+104+((millis()/200)%2),y1+10,3,1,true);
            break;
          case 1:
            drawArrow(x1+104+((millis()/200)%2),y1+32,3,1,true);
            break;
        }
      }
      //continue, restart, or stop
      display.drawRoundRect(x1,y1+3,105,15,5,1);
      display.fillRect(x1,y1,59,6,0);
      printSmall(x1,y1,"when stopped...",1);
      switch(onStop){
        case 0:
          printSmall(x1+5,y1+8,"reset playhead to loop",1);
          break;
        case 1:
          printSmall(x1+5,y1+8,"reset playhead to 1st loop",1);
          break;
        case 2:
          printSmall(x1+5,y1+8,"leave playhead in place",1);
          break;
      }
      y1+=22;
      display.drawRoundRect(x1,y1+3,105,15,5,1);
      display.fillRect(x1,y1,77,6,0);
      printSmall(x1,y1,"when rec finishes...",1);
      switch(postRec){
        case 0:
          printSmall(x1+5,y1+8,"stop.",1);
          break;
        case 1:
          printSmall(x1+5,y1+8,"continue.",1);
          break;
        case 2:
          printSmall(x1+5,y1+8,"restart current loop",1);
          break;
      }
      break;
  }
}

void playBackMenu(){
  uint8_t buttonCursor = 0;
  uint8_t menuCursor = 0;
  //0 is switching between menus, 1 is play, 2 is rec, 3 is stop
  uint8_t menuState = 0;
  while(true){
    readButtons();
    readJoystick();
    while(counterA != 0){
      switch(menuState){
        //play
        case 1:
          switch(menuCursor){
            case 0:
              isLooping = !isLooping;
              break;
            case 1:
              clockSource = !clockSource;
              break;
          }
          break;
        //stop
        case 3:
          switch(menuCursor){
            case 0:
              if(counterA >= 1 && onStop<2){
                onStop++;
              }
              else if(counterA <= -1 && onStop>0){
                onStop--;
              }
              break;
            case 1:
              if(counterA >= 1 && postRec<2){
                postRec++;
              }
              else if(counterA <= -1 && postRec>0){
                postRec--;
              }
              break;
          }
          break;
      }
      counterA+=counterA<0?1:-1;
    }
    if(itsbeen(200)){
      if(x != 0){
        switch(menuState){
          //play
          case 1:
            switch(menuCursor){
              case 0:
                isLooping = !isLooping;
                lastTime = millis();
                break;
              case 1:
                clockSource = !clockSource;
                lastTime = millis();
                break;
            }
            break;
          //stop
          case 3:
            switch(menuCursor){
              case 0:
                if(x == -1 && onStop<2){
                  onStop++;
                  lastTime = millis();
                }
                else if(x == 1 && onStop>0){
                  onStop--;
                  lastTime = millis();
                }
                break;
              case 1:
                if(x == -1 && postRec<2){
                  postRec++;
                  lastTime = millis();
                }
                else if(x == 1 && postRec>0){
                  postRec--;
                  lastTime = millis();
                }
                break;
            }
            break;
        }
      }
      if(y != 0){
        switch(menuState){
          //switching menus
          case 0:
            if(y == 1){
              if(buttonCursor<2){
                buttonCursor++;
              }
              lastTime = millis();
            }
            if(y == -1){
              if(buttonCursor>0){
                buttonCursor--;
              }
              lastTime = millis();
            }  
            break; 
          //play menu (same as stop menu)
          case 1:
          //rec menu (same as stop menu)
          case 2:
          //stop menu
          case 3:
            if(y == 1 && menuCursor<1){
              menuCursor++;
              lastTime = millis();
            }
            if(y == -1 && menuCursor>0){
              menuCursor--;
              lastTime = millis();
            }
            break;  
        }
      }
      if(menu_Press){
        lastTime = millis();
        //go back to menu-select
        if(menuState){
          menuState = 0;
          menuCursor = 0;
        }
        //or break out of menu
        else{
          menu_Press = false;
          return;
        }
      }
      if(sel){
        lastTime = millis();
        switch(menuState){
          //selecting a menu
          case 0:
            menuState = buttonCursor+1;
            menuCursor = 0;
            break;
        }
      }
    }
    display.clearDisplay();

    //drawing menu options
    if(!menuState){
      printPlaybackOptions(buttonCursor+1,menuCursor,false);
    }
    else{
      printPlaybackOptions(menuState,menuCursor,true);
    }

    String text;
    switch(buttonCursor){
      case 0:
        text = "ply";
        break;
      case 1:
        text = "rec";
        break;
      case 2:
        text = "stp";
        break;
    }
    printArp_wiggly(20,3,text,1);
    const uint8_t gap = 18;
    const uint8_t x1 = 3;
    const uint8_t y1 = 3;
    // drawBox(x1,y1,90,20,5,-5,1);
    display.drawFastVLine(x1+3,0,64,1);
    for(uint8_t i = 0; i<3; i++){
      drawCassetteButton(x1,y1+i*gap,i,buttonCursor==i);
      //if you're in menu-select mode
      if(buttonCursor == i && !menuState)
        drawArrow(x1+15+((millis()/200)%2),y1+i*gap+6,3,1,false);
    }
    display.display();
  }
}

void recMenu(){
  //main window controls rec mode, settings
  /*
  overwrite
  count-in (how many times to play thru seq before recording): waiting, none
  rec for: infinite (until the button is pressed), number of steps
  loop: current loop only, record through loops in sequence, ignore loop points
  after rec: stop, play from end position, play from first loop
  */
  bool whichWindow = false;
  uint8_t cursor = 0;
  int16_t currentVel;
  int16_t lastVel;
  float VUval;
  uint8_t yCursor = 0;
  uint8_t activeT = 0;
  // WireFrame cassette = makeCassette();
  // cassette.scale = 5;
  // cassette.rotate(90,2);
  // cassette.xPos = 120;
  while(menuIsActive && activeMenu.menuTitle == "REC"){
    readJoystick();
    readButtons();
    if(itsbeen(100)){
      if(y != 0){
        //if you're in priming mode
        if(whichWindow == 1){
          if(y == -1 && activeT>0){
            activeT--;
            if(activeT<yCursor)
              yCursor--;
            lastTime = millis();
          }
          else if(y == 1 && activeT<trackData.size()-1){
            activeT++;
            if(activeT>=yCursor+8)
              yCursor++;
            lastTime = millis();
          }
        }
        else{
          if(y == 1 && cursor<6){
            cursor++;
            lastTime = millis();
          }
          else if(y == -1 && cursor>0){
            cursor--;
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      if(x != 0){
        if(whichWindow){
          whichWindow = false;
          lastTime = millis();
        }
        else{
          switch(cursor){
            //prime
            case 0:
              whichWindow = true;
              lastTime = millis();
              break;
            //overwrite
            case 1:
              overwriteRecording = !overwriteRecording;
              lastTime = millis();
              break;
            //new tracks
            case 2:
              break;
            //count-in
            case 3:
              if(x == 1){
                if(shift){
                  if(recCountIn == 0)
                    waitForNoteBeforeRec = true;
                  else{
                    recCountIn--;
                  }
                  lastTime = millis();
                }
                else if(recCountIn>=24){
                  recCountIn-=24;
                  lastTime = millis();
                }
                else{
                  if(recCountIn == 0)
                    waitForNoteBeforeRec = true;
                  else
                    recCountIn = 0;
                  lastTime = 0;
                }
              }
              else if(x == -1){
                if(waitForNoteBeforeRec){
                  waitForNoteBeforeRec = false;
                  recCountIn = 0;
                  lastTime = millis();
                }
                else{
                  if(shift && recCountIn<65535){
                    recCountIn++;
                    lastTime = millis();
                  }
                  else if(recCountIn<=65535-24){
                    recCountIn+=24;
                    lastTime = millis();
                  }
                  else{
                    recCountIn = 65535;
                    lastTime = millis();
                  }
                }
              }
              break;
            //rec for
            case 4:
              if(x == 1){
                if(shift && recForNSteps>0){
                  recForNSteps--;
                  lastTime = millis();
                }
                else if(recForNSteps>=24){
                  recForNSteps-=24;
                  lastTime = millis();
                }
              }
              else if(x == -1){
                if(shift && recForNSteps<65535){
                  recForNSteps++;
                  lastTime = millis();
                }
                else if(recForNSteps<=65535-24){
                  recForNSteps+=24;
                  lastTime = millis();
                }
              }
              break;
            //loop behavior
            case 5:
              break;
            //post-rec behavior
            case 6:
              if(x == 1){
                if(postRec == 2)
                  postRec = 0;
                else
                  postRec++;
                lastTime = millis();
              }
              else if(x == -1){
                if(postRec == 0)
                  postRec = 2;
                else
                  postRec--;
                lastTime = millis();
              }
              break;
          }
        }
      }
      if(sel){
        if(whichWindow){
          trackData[activeT].isPrimed = !trackData[activeT].isPrimed;
          if(shift){
            for(int i = 0; i<trackData.size(); i++){
              trackData[i].isPrimed = trackData[activeT].isPrimed;
            }
          }
          lastTime = millis();
        }
        else{
          switch(cursor){
            case 0:
              whichWindow = true;
              break;
          }
        }
      }
      if(menu_Press){
        lastTime = millis();
        break;
      }
    }
    //get current VU destination value
    if(receivedNotes.notes.size()>0){
      currentVel = recentNote.vel;
    }
    else{
      currentVel = 0;
    }
    //set VU to the lastVel variable
    VUval = float(lastVel)/float(127);
    //update lastVel, incrementing it towards the currentVel
    lastVel = lastVel + (currentVel-lastVel)/5;
    if((whichWindow || cursor == 0) && activeMenu.page<28){
      activeMenu.page+=8;
      if(activeMenu.page>28)
        activeMenu.page = 28;
    }
    else if((!whichWindow && cursor != 0) && activeMenu.page>0){
      activeMenu.page-=8;
    }
    display.clearDisplay();
    drawVU(0,50,1-VUval);
    // cassette.render();
    activeMenu.displayRecMenu(cursor,yCursor,activeT);
    display.display();
    // cassette.rotate(4,1);
  }
  constructMenu("MENU");
}


void Menu::displayRecMenu(uint8_t menuCursor,uint8_t start, uint8_t active){
  //title
  display.setRotation(1);
  // display.setFont(&FreeSerifItalic12pt7b);
  display.setCursor(16,16);
  display.print("Rec");
  display.setFont();
  display.setRotation(UPRIGHT);
  if(millis()%1000>500){
    display.fillCircle(12,5,5,SSD1306_WHITE);
  }
  
  uint8_t x1 = screenWidth-activeMenu.page;

  //options---------
  //cursor highlight
  int8_t bitmap1 = -1;
  int8_t bitmap2 = -1;
  switch(menuCursor){
    //prime
    case 0:
      display.fillRoundRect(20,0,62,9,3,SSD1306_WHITE);
      break;
    //overwrite
    case 1:
      display.fillRoundRect(20,9,40,9,3,SSD1306_WHITE);
      display.drawBitmap(96,0,recmenu_overwrite_paper,24,21,1);
      display.drawBitmap(96+(millis()/200)%16,16-(millis()/400)%16,recmenu_overwrite_pencil,12,12,1);
      break;
    //make new tracks
    case 2:
      display.fillRoundRect(20,19,42,9,3,SSD1306_WHITE);
      display.drawBitmap(96,10,recmenu_new_tracks_tracks,29,19,1);
      display.drawBitmap(93,3+((millis()/200)%2),recmenu_new_tracks_plus,13,13,1);
      break;
    //count in
    case 3:
      display.fillRoundRect(20,28,36,9,3,SSD1306_WHITE);
      break;
    //rec-for
    case 4:
      display.fillRoundRect(20,37,30,9,3,SSD1306_WHITE);
      display.drawBitmap(96,0,recmenu_rec_for_cassette,20,27,1);
      display.drawBitmap(89+((millis()/200)%2),0,recmenu_rec_for_arrow,6,5,1);
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(36+((millis()/200)%2),37,recmenu_rec_for_arrow,6,5,1);
      display.setRotation(UPRIGHT);
      break;
    //loop
    case 5:
      display.fillRoundRect(20,46,20,9,3,SSD1306_WHITE);
      break;
    //after
    case 6:
      display.fillRoundRect(20,55,24,9,3,SSD1306_WHITE);
      display.drawBitmap(96,16,recmenu_stop,13,14,1);
      break;
  }
  //text
  printSmall(22,2,"prime tracks -->",2);
  printSmall(22,11,"overwrite: "+ (overwriteRecording?stringify("yes"):stringify("no")),2);
  // printSmall(22,21,"new tracks: "+ (alwaysMakeNewTracks?stringify("yes"):stringify("no")),2);
  printSmall(22,30,"count-in: "+ (waitForNoteBeforeRec?stringify("listen 4 note"):stepsToMeasures(recCountIn)),2);
  printSmall(22,39,"rec for: "+ (recForNSteps == 0?stringify("ever"):stepsToMeasures(recForNSteps)),2);
  switch(postRec){
    case 0:
      printSmall(22,57,"after: stop",2);
      break;
    case 1:
      printSmall(22,57,"after: continue",2);
      break;
    case 2:
      printSmall(22,57,"after: restart",2);
      break;
  }
  //if the prime menu is 'slid' onscreen
  if(activeMenu.page>0){
    if(start>0){
      drawArrow(x1-8,2+((millis()/400)%2),2,2,true);
    }
    if(start+8<trackData.size()){
      drawArrow(x1-8,62-((millis()/400)%2),2,3,true);
    }
    display.setRotation(1);
    printSmall(38,x1-10,"prime",SSD1306_WHITE);
    display.setRotation(UPRIGHT);
    //border
    display.fillRect(x1,0,screenWidth-x1,screenHeight,SSD1306_BLACK);
    display.drawFastVLine(x1,0,screenHeight,SSD1306_WHITE);
    //drawing each track
    for(uint8_t i = 0; i<8; i++){
      if(i+start<trackData.size()){
        uint8_t x2 = x1+3;
        if(i+start == active){
          x2-=2+((millis()/200)%2);
          drawArrow(x1-2,i*8+3,2,0,false);
        }
        //print track pitch
        printSmall(x2+10,i*8+1,getTrackPitchOctave(i+start),SSD1306_WHITE);
        //prime icon
        if(trackData[i+start].isPrimed){
          if((millis()+i*80)%1000>500){
            display.fillCircle(x2+5,i*8+3,3,SSD1306_WHITE);
          }
          // else{
          //   display.drawCircle(x2+5,i*8+3,3,SSD1306_WHITE);
          // }
        }
        else{
          display.drawCircle(x2+5,i*8+3,3,SSD1306_WHITE);
        }
      }
    }
  }
}

//DEPRECATED! i don't think i'll use this in any of the loop stuff
void drawLoopTimeLine(int xStart, int yStart){
  for(int step = 0; step <= seqEnd; step++){
    //if the step can be seen
    if(step*scale+xStart<screenWidth && step*scale+xStart>0){
      //draw a pixel every other
      if(!(step%2))
        display.drawPixel(step*scale+xStart,yStart,SSD1306_WHITE);
      //for the playhead
      if(step == playheadPos && playing){
        display.drawRoundRect(step*scale+xStart,yStart-5,step*scale+xStart+3,yStart+5,3,SSD1306_WHITE);
      }
      //if it's on a subDivInt
      if(!(step%subDivInt)){
        display.drawFastVLine(step*scale+xStart,yStart-3,6,SSD1306_WHITE);
      }
      if(!(step%96)){
        display.drawFastVLine(step*scale+xStart,yStart-5,10,SSD1306_WHITE);
      }
      if(step == seqEnd+1){
        display.setCursor(step*scale+xStart,yStart-3);
        display.print(")");
      }
      if(step == 0-1){
        display.setCursor(step*scale+xStart-4,yStart-3);
        display.print("(");
      }
      //drawing the loop start/ends
        //check if a loop starts or ends there, and if it does, draw it
      //loop through all loopData
      int flagHeight = 3;
      for(int loop = 0; loop<loopData.size(); loop++){
        //if the loop starts here
        if(step == loopData[loop].start){
          display.drawFastVLine(step*scale+xStart,yStart,(loop+1)*8,SSD1306_WHITE);
          display.fillTriangle(xStart+scale*step, yStart-(loop+1)*5,step*scale+xStart, yStart-(loop+1)*5-flagHeight,step*scale+xStart+flagHeight, xStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          if(loopData.size()-1 == loop){//if it's the top loop
            display.setCursor(step*scale+xStart, yStart-(loop+1)*8);
          }
          else{
            display.setCursor(step*scale+xStart, yStart-(loop+1)*8);
          }
          if(loop == activeLoop){
            display.print("{");
            display.print(loop);
            display.print("}");
          }
          else{
            display.print(loop);
          }
        }
        //if the loop ends here
        if(step == loopData[loop].end){
          display.drawLine(step*scale+xStart, yStart-(loop+1)*5, step*scale+xStart,yStart, SSD1306_WHITE);
          display.drawTriangle(step*scale+xStart, yStart-(loop+1)*5,step*scale+xStart, yStart-(loop+1)*5-flagHeight,step*scale+xStart-flagHeight, yStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          display.setCursor(step*scale+xStart+9, yStart-(loop+1)*8);
          display.print("{");
          display.print(loop);
          display.print("}");
        }
      }
    }
  }
}
void serialDispLoopData(){
  //Serial.print("activeLoop: ");
  //Serial.println(activeLoop);
  //Serial.print("loopData[activeLoop].start:");
  //Serial.println(loopData[activeLoop].start);
  //Serial.print("loopData[activeLoop].end:");
  //Serial.println(loopData[activeLoop].end);
  //Serial.print("iterations:");
  //Serial.println(loopData[activeLoop].reps);
  //Serial.print("count:");
  //Serial.println(loopCount);
}

//displays notes in all tracks from viewStart -> viewEnd
void displaySeqSerial() {
  // return;
  unsigned short int id = lookupData[activeTrack][cursorPos];
  //Serial.print("\n");
  for (int track = trackData.size() - 1; track > -1; track--) {
    //Serial.print(getTrackPitch(track));
    int len = getTrackPitch(track).length();
    for(int i = 0; i< 5-len; i++){
      //Serial.print(" ");
    }
    //Serial.print("(");
    //Serial.print(trackData[track].pitch);//prints the pitch before each track
    //Serial.print(")");
    for (int note = viewStart; note < viewEnd*scale; note++) {
      if (!(note % subDivInt)) { //if note is a multiple of subDivInt, print a divider
        //Serial.print("|");
      }
      //if no note
      if (lookupData[track][note] == 0) {
        if (note == cursorPos) { //if the cursor is there
          //Serial.print("#");
        }
        else if (track == activeTrack) { //if the track is active
          //Serial.print("=");
        }
        else {
          //Serial.print(".");//default track icon
        }
      }//if there's a tail_ID
      //if there is a note
      if (lookupData[track][note] != 0) {
        if (note == cursorPos && track == activeTrack) { //if the cursor is on it and the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            //Serial.print("{S}");
          }
          else {
            //Serial.print("[N]");
          }
        }
        else if (track == activeTrack) { //if the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            //Serial.print("s");
          }
          else {
            //Serial.print("n");
          }
        }
        else { //default display of a note
          if (seqData[track][lookupData[track][note]].isSelected) {
            //Serial.print("s");
          }
          else {
            //Serial.print("n");
          }
        }
      }
    }
    //Serial.print("|\n");
  }
}
