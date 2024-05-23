void debugDumpRegisters(MCP23017 mcp){
  	uint8_t conf = mcp.readRegister(MCP23017Register::IODIR_A);
	Serial.print("IODIR_A : ");
	Serial.print(conf, BIN);
	Serial.println();
	
	conf = mcp.readRegister(MCP23017Register::IODIR_B);
	Serial.print("IODIR_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IPOL_A);
	Serial.print("IPOL_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IPOL_B);
	Serial.print("IPOL_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPINTEN_A);
	Serial.print("GPINTEN_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPINTEN_B);
	Serial.print("GPINTEN_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::DEFVAL_A);
	Serial.print("DEFVAL_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::DEFVAL_B);
	Serial.print("DEFVAL_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCON_A);
	Serial.print("INTCON_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCON_B);
	Serial.print("INTCON_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IOCON);
	Serial.print("IOCON : ");
	Serial.print(conf, BIN);
	Serial.println();

	//conf = mcp.readRegister(IOCONB);
	//Serial.print("IOCONB : ");
	//Serial.print(conf, BIN);
	//Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPPU_A);
	Serial.print("GPPU_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPPU_B);
	Serial.print("GPPU_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTF_A);
	Serial.print("INTF_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTF_B);
	Serial.print("INTF_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCAP_A);
	Serial.print("INTCAP_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCAP_B);
	Serial.print("INTCAP_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPIO_A);
	Serial.print("GPIO_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPIO_B);
	Serial.print("GPIO_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::OLAT_A);
	Serial.print("OLAT_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::OLAT_B);
	Serial.print("OLAT_B : ");
	Serial.print(conf, BIN);
	Serial.println();
}


//Debug -------------------------------------------------------------------------
//counts notes
void debugNoteCount() {
  int totalNotes = 0;
  for (int i = 0; i < sequence.trackData.size(); i++) {
    totalNotes += sequence.noteData[i].size()-1;
  }
  //Serial.print(totalNotes + " notes stored in memory\n");
}

//prints the selection buffer, doe
void debugPrint() {
  //Serial.println("Printing out sequence.noteData...");
  for(int i = 0; i<sequence.trackData.size(); i++){
    for(int j = 1; j<sequence.noteData[i].size()-1; j++){
      //Serial.println("+------------+");
      //Serial.print("id: ");
      //Serial.println(j);
      //Serial.print("start: ");
      //Serial.println(sequence.noteData[i][j].startPos);
      //Serial.print("end: ");
      //Serial.println(sequence.noteData[i][j].endPos);
    }
  }
  //Serial.print("total notes on this track: ");
  //Serial.print(int(sequence.noteData[sequence.activeTrack].size()-1));
  //Serial.print("total notes in the sequence: ");
  int totalNotes = 0;
  for (int i = 0; i < sequence.trackData.size(); i++) {
    totalNotes = totalNotes + sequence.noteData[i].size()-1;
  }
  //Serial.print(totalNotes);
  //Serial.print("\nNotes in data: ");
  totalNotes = 0;
  for (int i = 0; i < sequence.trackData.size(); i++) {
    totalNotes = sequence.noteData[i].size() - 1 + totalNotes;
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
  for (int i = 0; i < sequence.trackData.size(); i++) {
    for (int j = 0; j < 254; j++) {
      sequence.makeNote(i, j, sequence.subDivision, 0);
    }
  }
  debugNoteCount();
}

//prints out memory allocation
void debugPrintMem(bool verby){
  if(verby){
    //Serial.print("tracks:");
    //Serial.println(sequence.noteData.size());
    //Serial.print("notes:");
    for(int i = 0; i<sequence.trackData.size(); i++){
      //Serial.print(i);
      //Serial.print(" - ");
      //Serial.println(sequence.noteData[i].size());
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
  //Serial.println(sequence.selectionCount);
  for(int track = 0; track<sequence.trackData.size(); track++){
    for(int note = 1; note<=sequence.noteData[track].size()-1; note++){
      if(sequence.noteData[track][note].isSelected){
        //Serial.print("note ");
        //Serial.print(note);
        //Serial.print(" on track ");
        //Serial.println(track);
        //Serial.print("[");
        //Serial.print(sequence.noteData[track][note].startPos);
        //Serial.print(",");
        //Serial.print(sequence.noteData[track][note].endPos);
        //Serial.println("]");
      }
    }
  }
}

void testJoyStick(){
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200) && controls.MENU()){
      lastTime = millis();
      return;
    }
    float X = analogRead(JOYSTICK_X);
    float Y = analogRead(JOYSTICK_Y);
    display.clearDisplay();
    printSmall(0,0,"X: "+stringify(X)+","+stringify(controls.joystickX),1);
    printSmall(0,10,"Y: "+stringify(Y)+","+stringify(controls.joystickY),1);
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
      arp.start();
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
      clipboard.copy();
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
      moveCursor(sequence.subDivision);
      displaySeqSerial();
      break;
    case 'f':
      debugPrintLookup();
      break;
    case ','://<
      moveCursor(-sequence.subDivision);
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
      sequence.makeNote(sequence.activeTrack, sequence.cursorPos, sequence.subDivision, 1); //default makes an 8th note
      displaySeqSerial();
      break;
    case ']':
      setActiveTrack(sequence.activeTrack - 1, false);
        displaySeqSerial();
      break;
    case '[':
      setActiveTrack(sequence.activeTrack + 1, false);
        displaySeqSerial();
      break;
    case 's'://select a note
        toggleSelectNote(sequence.activeTrack,sequence.IDAtCursor(),true);
        displaySeqSerial();
      break;
    case 'p'://debug print selected notes
      // debugPrint();
        // displaySeqSerial();
      break;
    case 't': //add track (tricky! big moment if this works first time) LMAO
      addTrack(sequence.defaultPitch);
      sequence.defaultPitch++;
        displaySeqSerial();
      break;
    case 'd'://controls.DELETE()eting the selected note(s)
      //Serial.print("attempting to del ");
      if (sequence.selectionCount > 0) {
        //Serial.print("all selected notes...\n");
        sequence.deleteSelected();
      }
      else {
        //Serial.print("1 note...\n");
        sequence.deleteNote();
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
      sequence.loopData[sequence.activeLoop].end -= 16;
        displaySeqSerial();
      break;
    case '\'':
      sequence.loopData[sequence.activeLoop].end += 16;
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
  if(controls.NEW())
    //Serial.println("n");
  if(controls.SELECT() )
    //Serial.println("controls.SELECT() ");
  if(controls.SHIFT())
    //Serial.println("controls.SHIFT()");
  if(controls.DELETE())
    //Serial.println("controls.DELETE()");
  if(controls.LOOP())
    //Serial.println("loop");
  if(controls.PLAY())
    //Serial.println("play");
  if(controls.COPY())
    //Serial.println("copy");
  if(controls.MENU())
    //Serial.println("menu");

  for(int i = 0; i<8; i++){
    if(controls.stepButton(i)){
      //Serial.println("step "+stringify(i));
    }
  }

  if(controls.joystickX != 0){
    //Serial.println("x:"+stringify(x));
  }
  if(controls.joystickY != 0){
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
  for(int i = 0; i<sequence.trackData.size();i++){
    //Serial.print(stringify(i)+":"+sequence.trackData[i].getPitch());
    //Serial.print('|');
    for(int j = 0; j<= sequence.sequenceLength; j++){
      // if(!j%sequence.subDivision)
      //   //Serial.print('|');
      if(sequence.lookupTable[i][j]==0){
        //Serial.print('-');
      }
      else{
        //Serial.print(sequence.lookupTable[i][j]);
      }
    }
    //Serial.println('|');
  }
}

void debugFillSequence(){
  for(uint16_t step = 0; step<sequence.sequenceLength; step++){
    Note debugNote(step,step,127,100,false,false);
    for(uint8_t track = 0; track<sequence.trackData.size(); track++){
      sequence.makeNote(debugNote,track,false);
    }
  }
}

//fills sequence with notes
void debugFillTrack(int division) {
  //Serial.print("placing note every ");
  //Serial.print(division);
  //Serial.print(" slices...\n");
  for (int j = 0; j < 254; j += division) {
    sequence.makeNote(sequence.activeTrack, j, sequence.subDivision, 0);
  }
}

void debugTestSwing(){
  long int i;
  sequenceClock.swingSubDiv = 24;//whole note swing
  long int  time;
  while(true){
    float t = sequenceClock.swingOffset(i);
    i++;
    //Serial.print("step: ");
    //Serial.println(i);
    //Serial.print("swing offset: ");
    //Serial.println(t);
    //Serial.print("w/o swing: ");
    //Serial.println(sequenceClock.uSecPerStep);
    //Serial.print("w/swing: ");
    //Serial.println(t+sequenceClock.uSecPerStep);
    i%=96;
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
      if(sequence.isLooping){
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
            display.fillRoundRect(x1+22,y1-1,sequence.isLooping?11:15,9,3,2);
            graphics.drawArrow(x1+37+((millis()/200)%2),y1+3,3,1,false);
            break;
          case 1:
            display.fillRoundRect(x1+54,y1+9,35,9,3,2);
            graphics.drawArrow(x1+91+((millis()/200)%2),y1+13,3,1,false);
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
            graphics.drawArrow(x1+104+((millis()/200)%2),y1+10,3,1,true);
            break;
          case 1:
            graphics.drawArrow(x1+104+((millis()/200)%2),y1+32,3,1,true);
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
    controls.readButtons();
    controls.readJoystick();
    while(controls.counterA != 0){
      switch(menuState){
        //play
        case 1:
          switch(menuCursor){
            case 0:
              sequence.isLooping = !sequence.isLooping;
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
              if(controls.counterA >= 1 && onStop<2){
                onStop++;
              }
              else if(controls.counterA <= -1 && onStop>0){
                onStop--;
              }
              break;
            case 1:
              if(controls.counterA >= 1 && postRec<2){
                postRec++;
              }
              else if(controls.counterA <= -1 && postRec>0){
                postRec--;
              }
              break;
          }
          break;
      }
      controls.counterA+=controls.counterA<0?1:-1;
    }
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0){
        switch(menuState){
          //play
          case 1:
            switch(menuCursor){
              case 0:
                sequence.isLooping = !sequence.isLooping;
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
                if(controls.joystickX == -1 && onStop<2){
                  onStop++;
                  lastTime = millis();
                }
                else if(controls.joystickX == 1 && onStop>0){
                  onStop--;
                  lastTime = millis();
                }
                break;
              case 1:
                if(controls.joystickX == -1 && postRec<2){
                  postRec++;
                  lastTime = millis();
                }
                else if(controls.joystickX == 1 && postRec>0){
                  postRec--;
                  lastTime = millis();
                }
                break;
            }
            break;
        }
      }
      if(controls.joystickY != 0){
        switch(menuState){
          //switching menus
          case 0:
            if(controls.joystickY == 1){
              if(buttonCursor<2){
                buttonCursor++;
              }
              lastTime = millis();
            }
            if(controls.joystickY == -1){
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
            if(controls.joystickY == 1 && menuCursor<1){
              menuCursor++;
              lastTime = millis();
            }
            if(controls.joystickY == -1 && menuCursor>0){
              menuCursor--;
              lastTime = millis();
            }
            break;  
        }
      }
      if(controls.MENU()){
        lastTime = millis();
        //go back to menu-select
        if(menuState){
          menuState = 0;
          menuCursor = 0;
        }
        //or break out of menu
        else{
          controls.setMENU(false) ;
          return;
        }
      }
      if(controls.SELECT() ){
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
        graphics.drawArrow(x1+15+((millis()/200)%2),y1+i*gap+6,3,1,false);
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
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        //if you're in priming mode
        if(whichWindow == 1){
          if(controls.joystickY == -1 && activeT>0){
            activeT--;
            if(activeT<yCursor)
              yCursor--;
            lastTime = millis();
          }
          else if(controls.joystickY == 1 && activeT<sequence.trackData.size()-1){
            activeT++;
            if(activeT>=yCursor+8)
              yCursor++;
            lastTime = millis();
          }
        }
        else{
          if(controls.joystickY == 1 && cursor<6){
            cursor++;
            lastTime = millis();
          }
          else if(controls.joystickY == -1 && cursor>0){
            cursor--;
            lastTime = millis();
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0){
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
              if(controls.joystickX == 1){
                if(controls.SHIFT()){
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
              else if(controls.joystickX == -1){
                if(waitForNoteBeforeRec){
                  waitForNoteBeforeRec = false;
                  recCountIn = 0;
                  lastTime = millis();
                }
                else{
                  if(controls.SHIFT() && recCountIn<65535){
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
              if(controls.joystickX == 1){
                if(controls.SHIFT() && recForNSteps>0){
                  recForNSteps--;
                  lastTime = millis();
                }
                else if(recForNSteps>=24){
                  recForNSteps-=24;
                  lastTime = millis();
                }
              }
              else if(controls.joystickX == -1){
                if(controls.SHIFT() && recForNSteps<65535){
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
              if(controls.joystickX == 1){
                if(postRec == 2)
                  postRec = 0;
                else
                  postRec++;
                lastTime = millis();
              }
              else if(controls.joystickX == -1){
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
      if(controls.SELECT() ){
        if(whichWindow){
          sequence.trackData[activeT].isPrimed = !sequence.trackData[activeT].isPrimed;
          if(controls.SHIFT()){
            for(int i = 0; i<sequence.trackData.size(); i++){
              sequence.trackData[i].isPrimed = sequence.trackData[activeT].isPrimed;
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
      if(controls.MENU()){
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
    graphics.drawVU(0,50,1-VUval);
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
      graphics.drawArrow(x1-8,2+((millis()/400)%2),2,2,true);
    }
    if(start+8<sequence.trackData.size()){
      graphics.drawArrow(x1-8,62-((millis()/400)%2),2,3,true);
    }
    display.setRotation(1);
    printSmall(38,x1-10,"prime",SSD1306_WHITE);
    display.setRotation(UPRIGHT);
    //border
    display.fillRect(x1,0,screenWidth-x1,screenHeight,SSD1306_BLACK);
    display.drawFastVLine(x1,0,screenHeight,SSD1306_WHITE);
    //drawing each track
    for(uint8_t i = 0; i<8; i++){
      if(i+start<sequence.trackData.size()){
        uint8_t x2 = x1+3;
        if(i+start == active){
          x2-=2+((millis()/200)%2);
          graphics.drawArrow(x1-2,i*8+3,2,0,false);
        }
        //print track pitch
        printSmall(x2+10,i*8+1,sequence.trackData[i+start].getPitchAndOctave(),SSD1306_WHITE);
        //prime icon
        if(sequence.trackData[i+start].isPrimed){
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
  for(int step = 0; step <= sequence.sequenceLength; step++){
    //if the step can be seen
    if(step*sequence.viewScale+xStart<screenWidth && step*sequence.viewScale+xStart>0){
      //draw a pixel every other
      if(!(step%2))
        display.drawPixel(step*sequence.viewScale+xStart,yStart,SSD1306_WHITE);
      //for the playhead
      if(step == playheadPos && playing){
        display.drawRoundRect(step*sequence.viewScale+xStart,yStart-5,step*sequence.viewScale+xStart+3,yStart+5,3,SSD1306_WHITE);
      }
      //if it's on a sequence.subDivision
      if(!(step%sequence.subDivision)){
        display.drawFastVLine(step*sequence.viewScale+xStart,yStart-3,6,SSD1306_WHITE);
      }
      if(!(step%96)){
        display.drawFastVLine(step*sequence.viewScale+xStart,yStart-5,10,SSD1306_WHITE);
      }
      if(step == sequence.sequenceLength+1){
        display.setCursor(step*sequence.viewScale+xStart,yStart-3);
        display.print(")");
      }
      if(step == 0-1){
        display.setCursor(step*sequence.viewScale+xStart-4,yStart-3);
        display.print("(");
      }
      //drawing the loop start/ends
        //check if a loop starts or ends there, and if it does, draw it
      //loop through all sequence.loopData
      int flagHeight = 3;
      for(int loop = 0; loop<sequence.loopData.size(); loop++){
        //if the loop starts here
        if(step == sequence.loopData[loop].start){
          display.drawFastVLine(step*sequence.viewScale+xStart,yStart,(loop+1)*8,SSD1306_WHITE);
          display.fillTriangle(xStart+sequence.viewScale*step, yStart-(loop+1)*5,step*sequence.viewScale+xStart, yStart-(loop+1)*5-flagHeight,step*sequence.viewScale+xStart+flagHeight, xStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          if(sequence.loopData.size()-1 == loop){//if it's the top loop
            display.setCursor(step*sequence.viewScale+xStart, yStart-(loop+1)*8);
          }
          else{
            display.setCursor(step*sequence.viewScale+xStart, yStart-(loop+1)*8);
          }
          if(loop == sequence.activeLoop){
            display.print("{");
            display.print(loop);
            display.print("}");
          }
          else{
            display.print(loop);
          }
        }
        //if the loop ends here
        if(step == sequence.loopData[loop].end){
          display.drawLine(step*sequence.viewScale+xStart, yStart-(loop+1)*5, step*sequence.viewScale+xStart,yStart, SSD1306_WHITE);
          display.drawTriangle(step*sequence.viewScale+xStart, yStart-(loop+1)*5,step*sequence.viewScale+xStart, yStart-(loop+1)*5-flagHeight,step*sequence.viewScale+xStart-flagHeight, yStart-(loop+1)*5-flagHeight,SSD1306_WHITE);
          display.setCursor(step*sequence.viewScale+xStart+9, yStart-(loop+1)*8);
          display.print("{");
          display.print(loop);
          display.print("}");
        }
      }
    }
  }
}
void serialDispLoopData(){
  //Serial.print("sequence.activeLoop: ");
  //Serial.println(sequence.activeLoop);
  //Serial.print("sequence.loopData[sequence.activeLoop].start:");
  //Serial.println(sequence.loopData[sequence.activeLoop].start);
  //Serial.print("sequence.loopData[sequence.activeLoop].end:");
  //Serial.println(sequence.loopData[sequence.activeLoop].end);
  //Serial.print("iterations:");
  //Serial.println(sequence.loopData[sequence.activeLoop].reps);
  //Serial.print("count:");
  //Serial.println(sequence.loopCount);
}

//displays notes in all tracks from sequence.viewStart -> sequence.viewEnd
void displaySeqSerial() {
  // return;
  unsigned short int id = sequence.IDAtCursor();
  //Serial.print("\n");
  for (int track = sequence.trackData.size() - 1; track > -1; track--) {
    //Serial.print(sequence.trackData[track].getPitch());
    int len = sequence.trackData[track].getPitch().length();
    for(int i = 0; i< 5-len; i++){
      //Serial.print(" ");
    }
    //Serial.print("(");
    //Serial.print(sequence.trackData[track].pitch);//prints the pitch before each track
    //Serial.print(")");
    for (int note = sequence.viewStart; note < sequence.viewEnd*sequence.viewScale; note++) {
      if (!(note % sequence.subDivision)) { //if note is a multiple of sequence.subDivision, print a divider
        //Serial.print("|");
      }
      //if no note
      if (sequence.lookupTable[track][note] == 0) {
        if (note == sequence.cursorPos) { //if the cursor is there
          //Serial.print("#");
        }
        else if (track == sequence.activeTrack) { //if the track is active
          //Serial.print("=");
        }
        else {
          //Serial.print(".");//default track icon
        }
      }//if there's a tail_ID
      //if there is a note
      if (sequence.lookupTable[track][note] != 0) {
        if (note == sequence.cursorPos && track == sequence.activeTrack) { //if the cursor is on it and the track is active
          if (sequence.noteData[track][sequence.lookupTable[track][note]].isSelected) {
            //Serial.print("{S}");
          }
          else {
            //Serial.print("[N]");
          }
        }
        else if (track == sequence.activeTrack) { //if the track is active
          if (sequence.noteData[track][sequence.lookupTable[track][note]].isSelected) {
            //Serial.print("s");
          }
          else {
            //Serial.print("n");
          }
        }
        else { //default display of a note
          if (sequence.noteData[track][sequence.lookupTable[track][note]].isSelected) {
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
