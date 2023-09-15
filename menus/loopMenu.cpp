void printLoopTitle(uint8_t, uint8_t);
void drawLoopBlocksVertically(int,int,int);
void drawLoopDiagram(uint8_t x1, uint8_t y1, uint8_t loop, int blockZoom);

void moveCursorWithinLoop(int amount, uint8_t whichLoop){
  //if the cursor is going to move before the loop
  if(amount<0 && (cursorPos + amount)<loopData[whichLoop][0])
    moveCursor(cursorPos - loopData[whichLoop][0]);
  //if the cursor is going to move past the loop
  else if(amount>0 && (cursorPos+amount)>=loopData[whichLoop][1])
    moveCursor(loopData[whichLoop][1]-cursorPos);
  //if not, then it's moving within the loop
  else
    moveCursor(amount);
}

bool isWithinLoop(int val, int loop, int amount, bool inclusive){
  if(!inclusive){
    if(val<(loopData[loop][1]-amount) && val>(loopData[loop][0]+amount)){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(val<=(loopData[loop][1]-amount) && val>=(loopData[loop][0]+amount)){
      return true;
    }
    else{
      return false;
    }
  }
}

void insertLoop(int afterThis, vector<unsigned short int> newLoop){
  vector<vector<unsigned short int>> tempData;
  for(int i = 0; i<loopData.size(); i++){
    tempData.push_back(loopData[i]);
    if(i == afterThis)
      tempData.push_back(newLoop);
  }
  loopData.swap(tempData);
}

void dupeLoop(int loop){
  vector<vector<unsigned short int>> tempData;
  for(int i = 0; i<loopData.size(); i++){
    tempData.push_back(loopData[i]);
    if(i == loop)
      tempData.push_back(loopData[i]);
  }
  loopData.swap(tempData);
}

void setLoopPoint(int32_t start, bool which){
  //set start
  if(which){
    if(start<=loopData[activeLoop][1] && start>=0)
      loopData[activeLoop][0] = start;
    else if(start < 0)
      loopData[activeLoop][0] = 0;
    else if(start>loopData[activeLoop][1])
      loopData[activeLoop][0] = loopData[activeLoop][1];
    loopData[activeLoop][0] = loopData[activeLoop][0];
    menuText = "loop start: "+stepsToPosition(loopData[activeLoop][0],true);
  }
  //set end
  else{
    if(start>=loopData[activeLoop][0] && start <= seqEnd)
      loopData[activeLoop][1] = start;
    else if(start>seqEnd)
      loopData[activeLoop][1] = seqEnd;
    else if(start<loopData[activeLoop][0])
      loopData[activeLoop][1] = loopData[activeLoop][0];
    loopData[activeLoop][1] = loopData[activeLoop][1];
    menuText = "loop end: "+stepsToPosition(loopData[activeLoop][1],true);
  }
}

uint16_t getLongestLoop(){
  uint16_t longestLength = 0;
  for(uint8_t i = 0; i<loopData.size(); i++){
    uint16_t l = loopData[i][1] - loopData[i][0];
    if(l>longestLength)
      longestLength = l;
  }
  return longestLength;
}

//just your normal editing controls, but you can't move the view past the loop
bool viewLoopControls(uint8_t which){
  //selectionBox
  //when sel is pressed and stick is moved, and there's no selection box
  if(sel && !selBox.begun && (x != 0 || y != 0)){
    selBox.begun = true;
    selBox.x1 = cursorPos;
    selBox.y1 = activeTrack;
  }
  //if sel is released, and there's a selection box
  if(!sel && selBox.begun){
    selBox.x2 = cursorPos;
    selBox.y2 = activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!n)
    drawingNote = false;
  mainSequencerEncoders();
  if (itsbeen(100)) {
    if (x == 1 && !shift) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(cursorPos%subDivInt){
        if(!movingLoop)
          moveCursorWithinLoop(-cursorPos%subDivInt,which);
        else
          moveCursor(-cursorPos%subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-cursorPos%subDivInt);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(-subDivInt,which);
        else
          moveCursor(-subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
    if (x == -1 && !shift) {
      if(cursorPos%subDivInt){
        if(!movingLoop)
          moveCursorWithinLoop(subDivInt-cursorPos%subDivInt,which);
        else
          moveCursor(subDivInt-cursorPos%subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt-cursorPos%subDivInt);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(subDivInt,which);
        else
          moveCursor(subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
  }
  if(itsbeen(100)){
    if (y == 1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack + 1, false);
      else
        setActiveTrack(activeTrack + 1, true);
      drawingNote = false;
      lastTime = millis();
    }
    if (y == -1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack - 1, false);
      else
        setActiveTrack(activeTrack - 1, true);
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (itsbeen(50)) {
    //moving
    if (x == 1 && shift) {
      if(!movingLoop)
        moveCursorWithinLoop(-1,which);
      else
        moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(-1);
      else if(movingLoop == -1)
        setLoopPoint(cursorPos,true);
      else if(movingLoop == 1)
        setLoopPoint(cursorPos,false);
    }
    if (x == -1 && shift) {
      if(!movingLoop)
        moveCursorWithinLoop(1,which);
      else
        moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(1);
      else if(movingLoop == -1)
        loopData[activeLoop][0] = cursorPos;
      else if(movingLoop == 1)
        loopData[activeLoop][1] = cursorPos;
    }
    //changing vel
    if (y == 1 && shift) {
      changeVel(-10);
      lastTime = millis();
    }
    if (y == -1 && shift) {
      changeVel(10);
      lastTime = millis();
    }

    if(lookupData[activeTrack][cursorPos]==0){
      if(y == 1 && shift){
        defaultVel-=10;
        if(defaultVel<1)
          defaultVel = 1;
        lastTime = millis();
      }
      if(y == -1 && shift){
        defaultVel+=10;
        if(defaultVel>127)
          defaultVel = 127;
        lastTime = millis();
      }
    }
  }
  //delete happens a liitle faster (so you can draw/erase fast)
  if(itsbeen(75)){
    //delete
    if(del && !shift){
      if (selectionCount > 0){
        deleteSelected();
        lastTime = millis();
      }
      else if(lookupData[activeTrack][cursorPos] != 0){
        deleteNote(activeTrack,cursorPos);
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    //new
    if(n && !track_Press && !drawingNote && !sel){
      if((!shift)&&(lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos)){
        makeNote(activeTrack,cursorPos,subDivInt,true);
        drawingNote = true;
        lastTime = millis();
        moveCursorWithinLoop(subDivInt,which);
        // debugPrintLookup();
      }
      if(shift){
        addTrack(defaultPitch, defaultChannel);
        lastTime = millis();
      }
    }
    //select
    if(sel && !selBox.begun){
      uint16_t id = lookupData[activeTrack][cursorPos];
      //select all
      if(n){
        selectAll();
      }
      //select only one
      else if(shift){
        clearSelection();
        toggleSelectNote(activeTrack, id, false);
      }
      //normal select
      else{
        toggleSelectNote(activeTrack, id, true);          
      }
      lastTime = millis();
    }
    if(del && shift){
      muteNote(activeTrack, lookupData[activeTrack][cursorPos], true);
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(play && !shift && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+shift, or if play and it's already recording
    if(play && shift || play && recording){
      toggleRecordingMode(waitForNote);
      lastTime = millis();
    }
 
    //loop
    if(loop_Press){
      if(shift){
        lastTime = millis();
        loopMenu();
      }
      else{
        //if you're not moving a loop, start
        if(movingLoop == 0){
          //if you're on the start, move the start
          if(cursorPos == loopData[activeLoop][0]){
            movingLoop = -1;
          }
          //if you're on the end
          else if(cursorPos == loopData[activeLoop][1]){
            movingLoop = 1;
          }
          //if you're not on either, move the whole loop
          else{
            movingLoop = 2;
          }
          lastTime = millis();
        }
        //if you were moving, stop
        else{
          movingLoop = 0;
          lastTime = millis();
        }
      }
    }
    //menu press
    if(menu_Press){
      if(shift){
        debugPrintLookup();
        lastTime = millis();
        displayHelpText(0);
        return false;
      }
      else{
        lastTime = millis();
        menu_Press = false;
        return false;
      }
    }
    //copy/pasate
    if(copy_Press){
      if(shift)
        paste();
      else{
        copy();
      }
      lastTime = millis();
    }
  }
  return true;
}

void viewLoop(uint8_t which){
  setCursor(loopData[which][0]);
  setLoop(which);
  String tempText;
  while(true){
    joyRead();
    readButtons();
    if(!viewLoopControls(which))
      return;
    tempText = menuText;
    menuText = "loop-"+stringify(which)+" "+menuText;
    display.clearDisplay();
    drawSeq(true,true,true,false,false,loopData[which][0]>viewStart?loopData[which][0]:viewStart,loopData[which][1]<viewEnd?loopData[which][1]:viewEnd);
    display.display();
    menuText = tempText;
  }
}

void loopMenu(){
  uint8_t xStart = 32;
  uint8_t targetL = activeLoop;
  uint8_t dispStart = activeLoop;
  int blockZoom = getLongestLoop()/20;
  vector<short unsigned int> storedLoop = {loopData[targetL][0],loopData[targetL][1],loopData[targetL][2],loopData[targetL][3]};
  bool loopStored = false;
  WireFrame w = makeMobius();
  w.xPos = 100;
  w.yPos = 50;
  w.rotate(75,0);
  uint8_t wfCounter = activeLoop;
  while(true){
    joyRead();
    readButtons();
    while(counterA != 0){
      if(counterA >= 1 && blockZoom>1){
        blockZoom--;
      }
      else if(counterA <= -1){
        blockZoom++;
      }
      counterA += counterA<0?1:-1;
    }

    if(itsbeen(100)){
      if(y != 0){
        if(y == 1){
          if(targetL<loopData.size()-1){
            targetL++;
          }
        }
        else if(y == -1){
          if(targetL>0){
            targetL--;
          }
        }
        //if targetL is more than 4 loops away from starting loop, then move menu
        while(targetL-dispStart > 3){
          dispStart++;
        }
        while(targetL<dispStart){
          dispStart--;
        }
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      //changing loop type and iterations
      if(x != 0){
        //iterations
        if(shift){
          if(x == -1){
            loopData[targetL][2]++;
            lastTime = millis();
          }
          else if(x == 1 && loopData[targetL][2]>0){
            loopData[targetL][2]--;
            lastTime = millis();
          }
        }
        //loop type
        else{
          if(x == -1 && loopData[targetL][3]<3){
            loopData[targetL][3]++;
            lastTime = millis();
          }
          if(x == 1 && loopData[targetL][3]>0){
            loopData[targetL][3]--;
            lastTime = millis();
          }
        }
      }
      //copying a loop
      if(copy_Press && !shift){
        loopStored = true;
        storedLoop[0] = loopData[targetL][0];
        storedLoop[1] = loopData[targetL][1];
        storedLoop[2] = loopData[targetL][2];
        storedLoop[3] = loopData[targetL][3];

        lastTime = millis();
      }
      //pasting loop
      if(copy_Press && shift && loopStored){
        insertLoop(targetL,storedLoop);
        lastTime = millis();
      }
      //deleting a loop
      if(del && loopData.size()>1){
        deleteLoop(targetL);
        if(targetL>=loopData.size())
          targetL--;
        lastTime = millis();
      }
      //playing
      if(play){
        togglePlayMode();
        lastTime = millis();
      }
      //new loop
      if(n){
        dupeLoop(targetL);
        lastTime = millis();
      }
      //infinite loop toggle
      if(loop_Press){
        if(shift){
          isLooping = !isLooping;
          lastTime = millis();
        }
        else{
          lastTime = millis();
          setLoopToInfinite(targetL);
        }
      }
      //select loop
      if(sel){
        lastTime = millis();
        sel = false;
        viewLoop(targetL);
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
    }
    animOffset++;
    if(animOffset>=100){
      animOffset = 0;
    }
    display.clearDisplay();

    //title
    printLoopTitle(80,1);

    if(isLooping){
      //loops
      drawLoopBlocksVertically(dispStart,targetL,blockZoom);

      //info
      drawLoopDiagram(100,36,targetL,blockZoom*1.5);
    }
    else{
      display.drawRect(16,8,40,54,1);
      shadeArea(17,9,38,52,4);
      if(millis()%1000>500){
        display.fillRoundRect(28,28,16,9,3,0);
        display.drawRoundRect(28,28,16,9,3,1);
        printSmall(30,30,"off",1);
      }
    }

    w.render();
    display.display();
    if(isLooping){
      w.rotate(5,1);
    }
    if(wfCounter != activeLoop){
      w.rotate(90,random(0,3));
      w.scale = float(random(5,9))/float(3);
      wfCounter = activeLoop;
    }
  }
}
const uint8_t maxLoopsShown = 6;

//x1 is start of the first loop, y1 is the midpoint of the first loop height,
//l is length of the first loop, 
//and h is the distance between the two loop-block midpoints
void drawLoopArrow(uint8_t x1, uint8_t y1, uint8_t l, uint8_t h, uint8_t type, uint8_t number){
  //normal 'lead to next loop' loop
  switch(type){
    case 0:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      display.drawLine(x1+l,y1+5,x1,y1+5,SSD1306_WHITE);
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-11,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      break;
    //reflexive
    case 1:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      display.drawLine(x1+l,y1+5,x1,y1+5,SSD1306_WHITE);
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-6,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      break;
    //return
    case 2:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      //flat
      display.drawLine(x1+l,y1+5,x1-2,y1+5,SSD1306_WHITE);
      //bottom elbow
      display.drawLine(x1-2,y1+5,x1-5,y1+2,SSD1306_WHITE);
      //if the first loop isn't on screen, draw a line to the top
      if(number>maxLoopsShown)
        display.drawLine(x1-5,y1+2,x1-5,0,SSD1306_WHITE);
      else{
        display.drawLine(x1-5,y1+2,x1-5,y1+2-11*number,SSD1306_WHITE);
        display.drawLine(x1-5,y1+2-11*number,x1-2,y1-1-11*number,SSD1306_WHITE);
        display.drawLine(x1-2,y1-1-11*number,x1,y1-1-11*number,SSD1306_WHITE);
      }
      break;
    //rnd of same size
    case 3:
      display.drawBitmap(x1+l,y1-3,loop_arrow_special_bmp,6,6,SSD1306_WHITE);
      break;
    //totally random
    case 4:
      display.drawBitmap(x1+l,y1-3,loop_arrow_special_bmp,6,6,SSD1306_WHITE);
      break;
  }
}

void printLoopTitle(uint8_t x1, uint8_t y1){
  display.setRotation(UPSIDEDOWN);
  display.drawBitmap(screenWidth-x1-5,screenHeight-y1-9-sin(millis()/100),loop_parenth,5,9,SSD1306_WHITE);
  display.setRotation(UPRIGHT);
  display.drawBitmap(x1+5,y1-sin(millis()/100+1),loop_L,7,9,SSD1306_WHITE);
  display.drawBitmap(x1+12,y1-sin(millis()/100+2),loop_O,7,9,SSD1306_WHITE);
  display.drawBitmap(x1+19,y1-sin(millis()/100+3),loop_O,7,9,SSD1306_WHITE);
  display.drawBitmap(x1+26,y1-sin(millis()/100+4),loop_P,7,9,SSD1306_WHITE);
  display.drawBitmap(x1+33,y1-sin(millis()/100+5),loop_parenth,5,9,SSD1306_WHITE);
}

void printLoopType(uint8_t x1, uint8_t y1, uint8_t type){
  switch(type){
    case 0:
      printSmall(x1,y1,"normal",SSD1306_WHITE);
      break;
    case 2:
      printSmall(x1,y1,"rnd similar",SSD1306_WHITE);
      break;
    case 1:
      printSmall(x1,y1,"rnd any",SSD1306_WHITE);
      break;
    case 3:
      printSmall(x1,y1,"return",SSD1306_WHITE);
      break;
    case 4:
      display.drawBitmap(x1,y1,inf_bmp,9,5,SSD1306_WHITE);
      break;
  }
}

String getLoopType(uint8_t type){
  String text;
  switch(type){
    case 0:
      text = "normal";
      break;
    case 2:
      text = "rnd similar";
      break;
    case 1:
      text = "rnd";
      break;
    case 3:
      text = "return";
      break;
    case 4:
      text = "infinite";
      break;
  }
  return text;
}

vector<uint8_t> getTracksWithNotes(){
  vector<uint8_t> list;
  for(uint8_t track = 0; track<trackData.size(); track++){
    if(seqData[track].size()>1)
      list.push_back(track);
  }
  return list;
}

vector<uint8_t> getTracksWithNotesInLoop(uint8_t loop){
  vector<uint8_t> list1 = getTracksWithNotes();
  vector<uint8_t> list2;
  for(uint8_t track = 0; track<list1.size(); track++){
    for(uint16_t i = loopData[loop][0]; i<loopData[loop][1]; i++){
      if(lookupData[list1[track]][i] != 0){
        list2.push_back(list1[track]);
        break;
      }
    }
  }
  return list2;
}

//draws mini sequence in a 32x16 grid
void drawMiniLoopArea(uint8_t x1, uint8_t y1, uint8_t loop){
  //scale
  float s = float(32)/float(loopData[loop][1]-loopData[loop][0]);

  //figure out how many tracks to draw
  vector<uint8_t> tracksWithNotes = getTracksWithNotesInLoop(loop);

  //start
  display.fillTriangle(x1, y1-6, x1+4,y1-6,x1,y1-2,SSD1306_WHITE);
  //end
  display.drawTriangle(x1+32,y1-6,x1+28,y1-6,x1+32,y1-2,SSD1306_WHITE);


  //bounds
  if(tracksWithNotes.size()>0){
    display.drawFastVLine(x1+32,y1-2,2+tracksWithNotes.size(),SSD1306_WHITE);
    display.drawFastVLine(x1,y1-2,2+tracksWithNotes.size(),SSD1306_WHITE);
    display.drawFastHLine(x1,y1+tracksWithNotes.size(),3,SSD1306_WHITE);
    display.drawFastHLine(x1+30,y1+tracksWithNotes.size(),3,SSD1306_WHITE);
  }
  else{
    display.drawFastVLine(x1+32,y1-2,8,SSD1306_WHITE);
    display.drawFastVLine(x1,y1-2,8,SSD1306_WHITE);
    display.drawFastHLine(x1,y1+6,3,SSD1306_WHITE);
    display.drawFastHLine(x1+30,y1+6,3,SSD1306_WHITE);
  }


  //if there are notes in the loop
  if(tracksWithNotes.size()>0){
    //move through each step of the loop
    for(uint8_t tr = 0; tr<tracksWithNotes.size(); tr++){
      uint8_t t = tracksWithNotes[tr];
      for(uint16_t i = loopData[loop][0]; i<loopData[loop][1]; i++){
        //if i finds the start positino of a note
        if(lookupData[t][i] != 0 && i == seqData[t][lookupData[t][i]].startPos){
          //get scaled length
          uint16_t length = (seqData[t][lookupData[t][i]].endPos - seqData[t][lookupData[t][i]].startPos+1);
          //draw line representing note
          display.drawFastHLine(x1+float(i-loopData[loop][0])*s,y1+tr,length*s,SSD1306_WHITE);
          i = seqData[t][lookupData[t][i]].endPos;
        }
      }
    }
    //drawing playhead, if it's in view
    if(playing && playheadPos>loopData[loop][0] && playheadPos<loopData[loop][1]){
      display.drawFastVLine(x1+(playheadPos-loopData[loop][0])*s,y1-1,tracksWithNotes.size()+2,SSD1306_WHITE);
    }
  }
  //if there are no notes
  else{
    printSmall(x1+3,y1,"[Empty]",SSD1306_WHITE);
  }
}


//draws a loop diagram that shows the start, end, length
void drawLoopDiagram(uint8_t x1, uint8_t y1, uint8_t loop, int blockZoom){
  drawMiniLoopArea(x1-17,y1-14,loop);
}

void drawLoopBlocksVertically(int firstLoop,int highlight, int z){
  const int yStart = 9;
  const int xStart = 16;
  const int loopHeight = 8;

  //drawing all the loops
  //starts from -1 so you draw one "before" loop (just for looks)
  for(int loop = -1; loop<maxLoopsShown+1; loop++){
    if(loop+firstLoop<loopData.size()){
      printSmall(1,yStart+(loopHeight+3)*loop+2,stringify(loop+firstLoop),SSD1306_WHITE);

      //scale is 1:12, so every 1/4 note is 2px
      int length = (loopData[loop+firstLoop][1]-loopData[loop+firstLoop][0])/z;
      int shade = (loopData[loop+firstLoop][0]+loopData[loop+firstLoop][1]+loop+firstLoop)%12+2;
      //playhead, if playing
      if(playing && loop+firstLoop == activeLoop)
        display.fillRect(xStart, yStart+(loopHeight+3)*loop, float(length)*float(playheadPos-loopData[activeLoop][0])/float(loopData[activeLoop][1]-loopData[activeLoop][0]), loopHeight, SSD1306_WHITE);

      display.drawRect(xStart, yStart+(loopHeight+3)*loop, length, loopHeight, SSD1306_WHITE);
      shadeArea(xStart, yStart+(loopHeight+3)*loop, length, loopHeight,shade);
      
      uint8_t x1 = xStart+length+3;

      //active loop note
      if(loop+firstLoop == activeLoop){
        //highlight arrow
        if(highlight == activeLoop){
          drawArrow(xStart-9+sin(millis()/100)+2, yStart+(loopHeight+3)*loop+4,2,0,true);
        }
        //loop types
        switch(loopData[activeLoop][3]){
          //normal
          case 0:
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,down_arrow_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd any
          case 1:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd of same length
          case 2:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_equal_bmp,11,7,SSD1306_WHITE);
            x1+=13;
            break;
          //return
          case 3:
            x1++;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,return_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //if it's an infinite loop
          case 4:
            //infinity sign
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop+1,inf_bmp,9,5,SSD1306_WHITE);
            x1+=11;
            break;
        }
        //length
        x1+=printFraction_small(x1,yStart+(loopHeight+3)*loop+2,stepsToMeasures(loopData[activeLoop][1]-loopData[activeLoop][0]))+2;
        if(playing){
          //play/iterations
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "("+stringify(loopCount)+"/"+stringify(loopData[activeLoop][2]+1)+")", SSD1306_WHITE);
          x1+=4*(stringify(loopCount).length()+stringify(loopData[activeLoop][2]+1).length()+3);
        }
        else{
          // play * iterations
          drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
          x1+=4;
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(loopData[activeLoop][2]+1), SSD1306_WHITE);
          x1+=4*(stringify(loopData[activeLoop][2]+1).length()+1)+1;
        }
        //note icon
        printSmall(x1,yStart+(loopHeight+3)*loop+2+sin(millis()/100), "$", SSD1306_WHITE);
        x1+=4;
      }
      //highlighted loop
      else if(loop+firstLoop == highlight){
        //highlight arrow
        drawArrow(xStart-9+sin(millis()/100)+2, yStart+(loopHeight+3)*loop+4,2,0,true);
        //loop types
        switch(loopData[highlight][3]){
          //normal
          case 0:
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,down_arrow_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd any
          case 1:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd of same length
          case 2:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_equal_bmp,11,7,SSD1306_WHITE);
            x1+=13;
            break;
          //return
          case 3:
            x1++;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,return_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //if it's an infinite loop
          case 4:
            //infinity sign
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop+1,inf_bmp,9,5,SSD1306_WHITE);
            x1+=11;
            break;
        }
        //length
        x1+=printFraction_small(x1,yStart+(loopHeight+3)*loop+1,stepsToMeasures(loopData[highlight][1]-loopData[highlight][0]))+2;
        // play symbol
        drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
        x1+=4;
        // iterations
        printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(loopData[highlight][2]+1), SSD1306_WHITE);
        x1+=4*(stringify(loopData[highlight][2]+1).length()+1);
      }
      //if there is a return loop offscreen
      //draw a return line from the first loop (or top of screen) down
      for(uint8_t l = firstLoop+maxLoopsShown-1; l<loopData.size(); l++){
        //if there is a return loop offscreen
        //or if the last loop is normal, and offscreen
        if(loopData[l][3] == 3 || (l == loopData.size()-1 && loopData[l][3] == 0)){
          //if the first loop is off screen too
          //just draw a vertical line
          if(firstLoop>0){
            display.drawFastVLine(xStart-5,0,screenHeight,SSD1306_WHITE);
          }
          //if the first loop is on screen, draw a line to it
          else{
            // display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2,xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2-11*(loop+firstLoop),SSD1306_WHITE);
            display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2,xStart-5,screenHeight,SSD1306_WHITE);
            display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2-11*(loop+firstLoop),xStart-2,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),SSD1306_WHITE);
            display.drawLine(xStart-2,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),xStart,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),SSD1306_WHITE);
          }
        }
      }
      //drawing loop connectors
      switch(loopData[loop+firstLoop][3]){
        //normal loop
        case 0:
          //if it's the last loop on screen, but there are more loops
          if(loop == 5){

          }
          //if it's the last loop in the sequence, make it a return
          else if(loop+firstLoop == loopData.size()-1){
            //if there's just one loop, draw it as an infinite loop (j so it looks nice)
            if(loopData.size() == 1)
              drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,1,loop+firstLoop);
            else
              drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,2,loop+firstLoop);
          }
          //if it's just another loop
          else
            drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,0,loop+firstLoop);
          break;
        //random loop, of same length
        case 1:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,3,loop+firstLoop);
          break;
        //any random loop
        case 2:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,4,loop+firstLoop);
          break;
        //return
        case 3:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,2,loop+firstLoop);
          break;
        //infinite
        case 4:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,1,loop+firstLoop);
          break;
      }
    }
  }
}
