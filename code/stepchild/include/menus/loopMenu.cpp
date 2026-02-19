// 'loop_next_reverse', 13x16px
const unsigned char loop_next_reverse_bmp [] = {
	0x0f, 0xf8, 0x37, 0xf8, 0x43, 0xf8, 0x25, 0xf8, 0xa2, 0xf8, 0xd5, 0x78, 0xcf, 0xf8, 0xe0, 0x00, 
	0xff, 0x80, 0xff, 0xd8, 0xff, 0xe8, 0x7f, 0xf0, 0x7f, 0xe0, 0x3f, 0xc8, 0x0f, 0x98, 0x00, 0x38
};
// 'loop_next_reverse_overlay', 14x16px
const unsigned char loop_next_reverse_overlay_bmp [] = {
	0x07, 0xfc, 0x1b, 0xfc, 0x21, 0xfc, 0x12, 0xfc, 0x51, 0x7c, 0x6a, 0xbc, 0x67, 0xfc, 0x70, 0x00, 
	0x7f, 0xc0, 0x7f, 0xec, 0x7f, 0xf4, 0xbf, 0xf8, 0xbf, 0xf0, 0xdf, 0xe4, 0xe7, 0xcc, 0xf8, 0x1c
};
// 'loop_next', 13x16px
const unsigned char loop_next_bmp [] = {
	0xff, 0x80, 0xff, 0x60, 0xfe, 0x10, 0xfd, 0x20, 0xfa, 0x28, 0xf5, 0x58, 0xff, 0x98, 0x00, 0x38, 
	0x0f, 0xf8, 0xdf, 0xf8, 0xbf, 0xf8, 0x7f, 0xf0, 0x3f, 0xf0, 0x9f, 0xe0, 0xcf, 0x80, 0xe0, 0x00
};
// 'corner_bottom', 8x8px
const unsigned char loop_corner_bottom_bmp []  = {
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x0f
};
// 'arrow_top', 15x8px
const unsigned char loop_arrow_top_bmp []  = {
	0x0f, 0xe0, 0x3f, 0xf6, 0x7f, 0xfa, 0x7f, 0xfc, 0xff, 0xf8, 0xff, 0xf2, 0xff, 0xe6, 0xff, 0x0e
};
// 'loop_arrow_workaround', 8x16px
const unsigned char loop_arrow_workaround_bmp [] = {
	0xf0, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf0
};
// 'loop_point_right', 3x5px
const unsigned char loop_point_right_bmp [] = {
	0x80, 0xc0, 0xe0, 0xc0, 0x80
};
// 'corner_bottom_overlay', 8x9px
const unsigned char corner_bottom_overlay_bmp [] = {
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0xbf, 0x8f, 0xe0
};


void moveCursorWithinLoop(int amount, uint8_t whichLoop){
  //if the cursor is going to move before the loop
  if(amount<0 && (sequence.cursorPos + amount)<sequence.loopData[whichLoop].start)
    sequence.moveCursor(sequence.cursorPos - sequence.loopData[whichLoop].start);
  //if the cursor is going to move past the loop
  else if(amount>0 && (sequence.cursorPos+amount)>=sequence.loopData[whichLoop].end)
    sequence.moveCursor(sequence.loopData[whichLoop].end-sequence.cursorPos);
  //if not, then it's moving within the loop
  else
    sequence.moveCursor(amount);
}

bool isWithinLoop(int val, int loop, int amount, bool inclusive){
  if(!inclusive){
    if(val<(sequence.loopData[loop].end-amount) && val>(sequence.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(val<=(sequence.loopData[loop].end-amount) && val>=(sequence.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
}

void insertLoop(int afterThis, Loop newLoop){
  vector<Loop> tempData;
  for(int i = 0; i<sequence.loopData.size(); i++){
    tempData.push_back(sequence.loopData[i]);
    if(i == afterThis)
      tempData.push_back(newLoop);
  }
  sequence.loopData.swap(tempData);
}

void dupeLoop(int loop){
  vector<Loop> tempData;
  for(int i = 0; i<sequence.loopData.size(); i++){
    tempData.push_back(sequence.loopData[i]);
    if(i == loop)
      tempData.push_back(sequence.loopData[i]);
  }
  sequence.loopData.swap(tempData);
}

uint16_t getLongestLoop(){
  uint16_t longestLength = 0;
  for(uint8_t i = 0; i<sequence.loopData.size(); i++){
    uint16_t l = sequence.loopData[i].end - sequence.loopData[i].start;
    if(l>longestLength)
      longestLength = l;
  }
  return longestLength;
}

//just your normal editing controls, but you can't move the view past the loop
bool viewLoopControls(uint8_t which){
  //selectionBox
  //when controls.SELECT()  is pressed and stick is moved, and there's no selection box
  if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
    selBox.begun = true;
    selBox.coords.start.x = sequence.cursorPos;
    selBox.coords.start.y = sequence.activeTrack;
  }
  //if controls.SELECT()  is released, and there's a selection box
  if(!controls.SELECT()  && selBox.begun){
    selBox.coords.end.x = sequence.cursorPos;
    selBox.coords.end.y = sequence.activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!controls.NEW())
    sequence.drawingNote = false;
  mainSequencerEncoders();
  if (utils.itsbeen(100)) {
    if (controls.joystickX == 1 && !controls.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(sequence.cursorPos%sequence.subDivision){
        if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(-sequence.cursorPos%sequence.subDivision,which);
        else
          sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
          sequence.moveLoop(-sequence.cursorPos%sequence.subDivision);
      }
      else{
        if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(-sequence.subDivision,which);
        else
          sequence.moveCursor(-sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
          sequence.moveLoop(-sequence.subDivision);
      }
      //moving loop start/end
      if(sequence.movingLoop == MOVING_LOOP_END){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(sequence.movingLoop == MOVING_LOOP_START){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
    if (controls.joystickX == -1 && !controls.SHIFT()) {
      if(sequence.cursorPos%sequence.subDivision){
        if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision,which);
        else
          sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
          sequence.moveLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
      }
      else{
        if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
          moveCursorWithinLoop(sequence.subDivision,which);
        else
          sequence.moveCursor(sequence.subDivision);
        lastTime = millis();
        if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
          sequence.moveLoop(sequence.subDivision);
      }
      //moving loop start/end
      if(sequence.movingLoop == MOVING_LOOP_END){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(sequence.movingLoop == MOVING_LOOP_START){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1 && !controls.SHIFT() && !controls.LOOP()) {
      if(sequence.recording())//if you're not in normal mode, you don't want it to be loud
        sequence.setActiveTrack(sequence.activeTrack + 1, false);
      else
        sequence.setActiveTrack(sequence.activeTrack + 1, true);
      sequence.drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1 && !controls.SHIFT() && !controls.LOOP()) {
      if(sequence.recording())//if you're not in normal mode, you don't want it to be loud
        sequence.setActiveTrack(sequence.activeTrack - 1, false);
      else
        sequence.setActiveTrack(sequence.activeTrack - 1, true);
      sequence.drawingNote = false;
      lastTime = millis();
    }
  }
  if (utils.itsbeen(50)) {
    //moving
    if (controls.joystickX == 1 && controls.SHIFT()) {
      if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
        moveCursorWithinLoop(-1,which);
      else
        sequence.moveCursor(-1);
      lastTime = millis();
      if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
        sequence.moveLoop(-1);
      else if(sequence.movingLoop == MOVING_LOOP_END)
        sequence.setLoopPoint(sequence.cursorPos,true);
      else if(sequence.movingLoop == MOVING_LOOP_START)
        sequence.setLoopPoint(sequence.cursorPos,false);
    }
    if (controls.joystickX == -1 && controls.SHIFT()) {
      if(sequence.movingLoop == MOVING_NO_LOOP_POINTS)
        moveCursorWithinLoop(1,which);
      else
        sequence.moveCursor(1);
      lastTime = millis();
      if(sequence.movingLoop == MOVING_BOTH_LOOP_POINTS)
        sequence.moveLoop(1);
      else if(sequence.movingLoop == MOVING_LOOP_END)
        sequence.loopData[sequence.activeLoop].start = sequence.cursorPos;
      else if(sequence.movingLoop == MOVING_LOOP_START)
        sequence.loopData[sequence.activeLoop].end = sequence.cursorPos;
    }
    //changing vel
    if (controls.joystickY == 1 && controls.SHIFT()) {
      sequence.changeVel(-10);
      lastTime = millis();
    }
    if (controls.joystickY == -1 && controls.SHIFT()) {
      sequence.changeVel(10);
      lastTime = millis();
    }

    if(sequence.IDAtCursor()==0){
      if(controls.joystickY == 1 && controls.SHIFT()){
        sequence.defaultVel-=10;
        if(sequence.defaultVel<1)
          sequence.defaultVel = 1;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && controls.SHIFT()){
        sequence.defaultVel+=10;
        if(sequence.defaultVel>127)
          sequence.defaultVel = 127;
        lastTime = millis();
      }
    }
  }
  //del happens a liitle faster (so you can draw/erase fast)
  if(utils.itsbeen(75)){
    //del
    if(controls.DELETE() && !controls.SHIFT()){
      if (sequence.selectionCount > 0){
        sequence.deleteSelected();
        lastTime = millis();
      }
      else if(sequence.IDAtCursor() != 0){
        sequence.deleteNote(sequence.activeTrack,sequence.cursorPos);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //new
    if(controls.NEW() && !controls.A() && !sequence.drawingNote && !controls.SELECT() ){
      if((!controls.SHIFT())&&(sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos)){
        sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision,true);
        sequence.drawingNote = true;
        lastTime = millis();
        moveCursorWithinLoop(sequence.subDivision,which);
      }
      if(controls.SHIFT()){
        sequence.addTrack(sequence.defaultPitch, sequence.defaultChannel,false);
        lastTime = millis();
      }
    }
    //select
    if(controls.SELECT()  && !selBox.begun){
      uint16_t id = sequence.IDAtCursor();
      //select all
      if(controls.NEW()){
        selectAll();
      }
      //select only one
      else if(controls.SHIFT()){
        clearSelection();
        toggleSelectNote(sequence.activeTrack, id, false);
      }
      //normal select
      else{
        toggleSelectNote(sequence.activeTrack, id, true);          
      }
      lastTime = millis();
    }
    if(controls.DELETE() && controls.SHIFT()){
      sequence.muteNote(sequence.activeTrack, sequence.IDAtCursor(), true);
      lastTime = millis();
    }
 
    //loop
    if(controls.LOOP()){
      //if you're not moving a loop, start
      if(sequence.movingLoop == MOVING_NO_LOOP_POINTS){
        //if you're on the start, move the start
        if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].start){
          sequence.movingLoop = MOVING_LOOP_END;
        }
        //if you're on the end
        else if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].end){
          sequence.movingLoop = MOVING_LOOP_START;
        }
        //if you're not on either, move the whole loop
        else{
          sequence.movingLoop = MOVING_BOTH_LOOP_POINTS;
        }
        lastTime = millis();
      }
      //if you were moving, stop
      else{
        sequence.movingLoop = MOVING_NO_LOOP_POINTS;
        lastTime = millis();
      }
    }
    //menu press
    if(controls.MENU()){
      if(controls.SHIFT()){
        lastTime = millis();
        return false;
      }
      else{
        lastTime = millis();
        controls.setMENU(false) ;
        return false;
      }
    }
    //copy/pasate
    if(controls.COPY()){
      if(controls.SHIFT())
        clipboard.paste();
      else{
        clipboard.copy();
      }
      lastTime = millis();
    }
  }
  return true;
}

void viewLoop(uint8_t which){
  sequence.setCursor(sequence.loopData[which].start);
  sequence.setActiveLoop(which);
  String tempText;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(!viewLoopControls(which))
      return;
    tempText = menuText;
    menuText = "lp"+stringify(which+1)+" "+menuText;
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.shadeOutsideLoop = true;
    drawSeq(settings);
    display.display();
    menuText = tempText;
  }
}

vector<uint8_t> getTracksWithNotes(){
  vector<uint8_t> list;
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    if(sequence.noteData[track].size()>1)
      list.push_back(track);
  }
  return list;
}

vector<uint8_t> getTracksWithNotesInLoop(uint8_t loop){
  vector<uint8_t> list1 = getTracksWithNotes();
  vector<uint8_t> list2;
  for(uint8_t track = 0; track<list1.size(); track++){
    for(uint16_t i = sequence.loopData[loop].start; i<sequence.loopData[loop].end; i++){
      if(sequence.lookupTable[list1[track]][i] != 0){
        list2.push_back(list1[track]);
        break;
      }
    }
  }
  return list2;
}

void setLoopToInfinite(uint8_t targetL){
  //if it's already a 3, set it to 0
  if(sequence.loopData[targetL].type == INFINITE){
    sequence.loopData[targetL].type = NORMAL;
  }
  //if not, set this loop to 3
  else{
    sequence.loopData[targetL].type = INFINITE;
  }
  //set all other inf loops to 0
  for(uint8_t l = 0; l<sequence.loopData.size(); l++){
    if(l != targetL){
      if(sequence.loopData[l].type == INFINITE)
        sequence.loopData[l].type = NORMAL;
    }
  }
}


//creates a new loop immediately to the right of the current loop and copies the loop over into it (useful for on the fly beatmaking)
//then sets the OG loop to that one
bool pushToNewLoop(){
  Loop newLoop = sequence.loopData[sequence.activeLoop];
  uint16_t length = newLoop.length();
  //if you're out of sequence length
  if(newLoop.end+length >= sequence.sequenceLength){
    alert("extend seq to add more loops!",500);
    return false;
  }
  //else, make a new loop to the right
  else{
    newLoop.start+=length;
    newLoop.end+=length;
    sequence.addLoop(newLoop);//add the loop to the chain

    //duplicate the loop contents and paste them next to the current loop
    clipboard.copyLoop(sequence.activeLoop);
    clipboard.pasteAt(0,newLoop.start);

    //move the view to the start of the current loop
    sequence.setCursor(newLoop.start);
    sequence.setViewStart(newLoop.start);
    return true;
  }
}

String getLoopType(LoopType type){
  switch(type){
    case NORMAL:
      return "normal";
    case RANDOM:
      return "rnd";
    case RANDOM_SAME_LENGTH:
      return "rnd same";
    case RETURN:
      return "reset";
    case INFINITE:
      return "infinite";
    default:
      return "idk bruh";
  }
}

String getLoopBehavior(LoopType type){
  switch(type){
    case NORMAL:
      return "the next loop";
    case RANDOM:
      return "a random loop";
    case RANDOM_SAME_LENGTH:
      return "a random loop of same length";
    case RETURN:
      return "the 1st loop";
    case INFINITE:
      return "this loop";
    default:
      return "idk bruh";
  }
}

class LoopMenu:public StepchildMenu{
  public:
  WireFrame icon;
  uint8_t menuStart = 0;
  uint8_t numberOfLoopsShown = 5;
  int8_t yCoordOffset = 0;
  const uint8_t loopHeight = 9;

  LoopMenu(){
    icon = makeLoopArrows(0);
    icon.offset.x = 10;
    icon.offset.y = 5;
    // icon.scale = 1.5;
    icon.scale = 1;
    coords = CoordinatePair(0,0,128,64);
  }
  uint8_t lastReturnLoopID(){
    uint8_t id = 0;
    for(uint8_t i = 0; i<sequence.loopData.size(); i++){
      if(sequence.loopData[i].type == RETURN || (sequence.loopData[i].type == NORMAL && (i == (sequence.loopData.size()-1)))){
        id = i;
      }
    }
    return id;
  }
  bool isThisATrickyLoop(uint8_t loopID){
    return !(loopID%2) && (sequence.loopData[loopID].type == RETURN || (sequence.loopData[loopID].type == NORMAL && sequence.loopData.size()-1 == loopID));
  }
  bool thatOneGraphicalCondition(uint8_t loopID){
    if(loopID == sequence.loopData.size()-1)
      return false;
    return isThisATrickyLoop(loopID+1);
  }
  bool loopAboveIsPointingToThisLoop(uint8_t id){
    if(id){
      return (sequence.loopData[id - 1].type == NORMAL);
    }
    return false;
  }
  void drawLoopChunk(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t loopID, bool selected){
    if(sequence.playing() && sequence.activeLoop == loopID){
      uint8_t progress = float(w) * float(sequence.playheadPos - sequence.loopData[loopID].start)/float(sequence.loopData[loopID].end - sequence.loopData[loopID].start);
      display.drawRect(x,y,w,h,1);
      display.fillRect(x,y,progress,h,1);
    }
    else{
      display.fillRect(x,y,w,h,1);
    }
    if(!loopID && sequence.loopData.size() == 1 && (sequence.loopData[loopID].type == NORMAL || sequence.loopData[loopID].type == RETURN)){
    }
    else if(!loopID && sequence.loopData[loopID].type == RETURN){
    }
    else
    switch(sequence.loopData[loopID].type){
      case NORMAL:
        //if it's the last loop, treat it like a return loop instead
        if(!(loopID == sequence.loopData.size()-1)){
          if(loopID%2){
            if(thatOneGraphicalCondition(loopID))
              display.drawBitmap(x-14,y,loop_next_reverse_overlay_bmp,14,16,1,0);
            else
              display.drawBitmap(x-13,y,loop_next_reverse_bmp,13,16,1,0);
            if(!loopAboveIsPointingToThisLoop(loopID)){
              display.drawFastVLine(x+w,y,h,1);
              display.drawFastVLine(x+w+1,y+1,h-2,1);
              display.drawFastVLine(x+w+2,y+2,h-4,1);
            }
          }
          else{
            display.drawBitmap(x+w,y,loop_next_bmp,13,16,1,0);
            if(!loopAboveIsPointingToThisLoop(loopID)){
              display.drawFastVLine(x-1,y,h,1);
              display.drawFastVLine(x-2,y+1,h-2,1);
              display.drawFastVLine(x-3,y+2,h-4,1);
            }
          }
          break;
        }
      case RETURN:
        //if you need to snake underneath the loop to draw the arrow
        if(isThisATrickyLoop(loopID) || !loopAboveIsPointingToThisLoop(loopID)){
          display.drawFastVLine(x+w,y+1,h-2,1);
          display.drawFastVLine(x+w+1,y+2,h-4,1);
        }
        //tube out to bottom corner
        display.fillRect(x-14,y,14,h,1);
        //bottom corner
        if(!(lastReturnLoopID() == loopID))
          display.drawBitmap(x-22,y-1,corner_bottom_overlay_bmp,8,9,1,0);
        else
          display.drawBitmap(x-22,y-1,loop_corner_bottom_bmp,8,8,1);
        break;
      case RANDOM_SAME_LENGTH:
        if(loopID%2){
          display.drawFastHLine(x-14,y+2,3,1);
          display.drawFastHLine(x-14,y+4,3,1);
        }
        else{
          display.drawFastHLine(x+w+11,y+2,3,1);
          display.drawFastHLine(x+w+11,y+4,3,1);
        }
      case RANDOM:
        if(loopID%2){
          display.drawBitmap(x-10,y,rnd_bmp,7,7,SSD1306_WHITE);
          display.drawFastVLine(x-1,y+1,h-2,1);
          display.drawFastVLine(x-2,y+2,h-4,1);
          display.drawPixel(x-3,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            display.drawFastVLine(x+w,y,h,1);
            display.drawFastVLine(x+w+1,y+1,h-2,1);
            display.drawFastVLine(x+w+2,y+2,h-4,1);
          }
        }
        else{
          display.drawBitmap(x+w+3,y,rnd_bmp,7,7,SSD1306_WHITE);
          display.drawFastVLine(x+w,y+1,h-2,1);
          display.drawFastVLine(x+w+1,y+2,h-4,1);
          display.drawPixel(x+w+2,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            display.drawFastVLine(x-1,y,h,1);
            display.drawFastVLine(x-2,y+1,h-2,1);
            display.drawFastVLine(x-3,y+2,h-4,1);
          }
        }
        
        break;
      case INFINITE:
        if(loopID%2){
          display.drawBitmap(x-10,y+1,inf_bmp,9,5,SSD1306_WHITE);
          display.drawFastVLine(x-1,y+1,h-2,1);
          display.drawFastVLine(x-2,y+2,h-4,1);
          display.drawPixel(x-3,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            display.drawFastVLine(x+w,y,h,1);
            display.drawFastVLine(x+w+1,y+1,h-2,1);
            display.drawFastVLine(x+w+2,y+2,h-4,1);
          }
        }
        else{
          display.drawBitmap(x+w+1,y+1,inf_bmp,9,5,SSD1306_WHITE);
          display.drawFastVLine(x+w,y+1,h-2,1);
          display.drawFastVLine(x+w+1,y+2,h-4,1);
          display.drawPixel(x+w+2,y+3,1);
          if(!loopAboveIsPointingToThisLoop(loopID)){
            display.drawFastVLine(x-1,y,h,1);
            display.drawFastVLine(x-2,y+1,h-2,1);
            display.drawFastVLine(x-3,y+2,h-4,1);
          }
        }
        break;
    }
    printSmall(x+w/2-2,y+1,stringify(loopID+1),2);
  }
  void drawReturnBar(uint8_t yStart, uint8_t firstLoopToDraw){
    uint8_t lastReturnID = lastReturnLoopID();
    //if there's just one loop
    if(sequence.loopData.size() == 1 && lastReturnID == 0 && (sequence.loopData[0].type == NORMAL || sequence.loopData[0].type == RETURN)){
      display.drawBitmap(coords.start.x+2,yStart-1,loop_arrow_top_bmp,15,8,1);
      display.drawBitmap(coords.start.x+2,yStart+8,loop_corner_bottom_bmp,8,8,1);
      display.drawBitmap(coords.start.x+32,yStart,loop_arrow_workaround_bmp,8,16,1);
      display.fillRect(coords.start.x+10,yStart+9,22,7,1);
      display.fillRect(coords.start.x+17,yStart,7,7,1);
      display.drawFastHLine(coords.start.x+2,yStart+7,7,1);
    }
    //if there's a return loop below & offscreen
    else if(lastReturnID > (firstLoopToDraw+menuStart)){
      //if the start is onscreen, draw the corner arrow and connect it
      if(menuStart == 0){
        display.drawBitmap(2,yStart-1,loop_arrow_top_bmp,15,8,1);
        display.fillRect(17,yStart,7,7,1);
      }
      else{
        display.fillRect(2,yStart,7,8,1);
      }
      display.fillRect(2,yStart+7,7,57-yStart,1);
    }
    //if the return loop is onscreen
    else if(lastReturnID <= (menuStart+firstLoopToDraw) && lastReturnID){
      //if the first loop is onscreen too
      if(menuStart == 0){
        display.drawBitmap(2,yStart-1,loop_arrow_top_bmp,15,8,1);
        display.fillRect(17,yStart,7,7,1);
        display.fillRect(2,yStart+7,7,(lastReturnID-1) * loopHeight+1,1);
      }
      //if not
      else{
        display.fillRect(2,yStart,7,(lastReturnID-menuStart) * loopHeight,1);
      }
    }
    //if neither the return loop or the start is onscreen
    else{
      display.fillRect(2,yStart,7,64-yStart,1);
    }
  }
  void displayMenu(){
    icon.rotate(-1,2);
    display.clearDisplay();

    //if the last loop drawn is a return loop, and it's a snake-around loop,
    //then move it up and draw one less loop
    uint8_t firstLoopToDraw = min(numberOfLoopsShown,sequence.loopData.size()-1);
    uint8_t yStart = coords.start.y + 12 + yCoordOffset;

    //drawing return bar
    drawReturnBar(yStart,firstLoopToDraw);

    //draw the loop blocks
    for(int8_t i = firstLoopToDraw; i>=0; i--){
      drawLoopChunk(coords.start.x+24,yStart+(i)*loopHeight,8,7,i+menuStart,cursor == (i+menuStart));
    }
    //draw one extra above, just for visual continuity
    if(menuStart || (!menuStart && yCoordOffset)){
      if(menuStart)
        drawLoopChunk(coords.start.x+24,yStart-loopHeight,8,7,menuStart-1,false);
      //crop it
      display.fillRect(coords.start.x,coords.start.y,48,12,0);
    }
    display.drawFastHLine(coords.start.x+16,coords.start.y+10,48,1);

    //title stuff
    display.drawBitmap(coords.start.x+20,coords.start.y,loop_L,7,9,SSD1306_WHITE);
    display.drawBitmap(coords.start.x+27,coords.start.y,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(coords.start.x+34,coords.start.y,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(coords.start.x+41,coords.start.y,loop_P,7,9,SSD1306_WHITE);
    printSmall(coords.start.x+51,coords.start.y+2,"menu",1);
    icon.render();

    //draw the highlight arrow
    if(cursor <=  menuStart + firstLoopToDraw && cursor >= menuStart)
      graphics.drawArrow(coords.start.x+24+millis()/200%2,yStart + 3 + (cursor-menuStart)*loopHeight,2,ARROW_RIGHT,0,0,false);

    //decrement the interpolate distance var
    if(yCoordOffset)
      yCoordOffset += ((yCoordOffset < 0)?1:-1);

    //drawing loop info
    #define LOOPINFO coords.start.x+70

    //arrow for loops above
    if(menuStart)
      graphics.drawArrow(LOOPINFO,millis()/200%2,2,ARROW_UP,1);
    if(sequence.loopData.size() > menuStart + 6)
      graphics.drawArrow(LOOPINFO,7-millis()/200%2,2,ARROW_DOWN,1);
    
    //loop number
    printSmall(LOOPINFO+8,coords.start.y,"loop #"+stringify(cursor),1);
    //loop type
    graphics.drawButton(LOOPINFO,coords.start.y+7,"type",1);
    printSmall(LOOPINFO+23,coords.start.y+8,getLoopType(sequence.loopData[cursor].type),1);
    uint8_t height = printSmall_overflow(LOOPINFO,coords.start.y+15,0,"leads to "+getLoopBehavior(sequence.loopData[cursor].type),1);
    //# of reps
    graphics.drawButton(LOOPINFO,coords.start.y+15+height*7,"reps",1);
    // graphics.drawButton(LOOPINFO+28,coords.start.y+15+height*7,stringify(sequence.loopData[cursor].reps),1);
    //length

    //buttons
    //set active/is active
    //delete
    //duplicate

    display.display();
  }
  bool loopMenuControls(){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        return false;
      }
      if(controls.SELECT()){
        //should trigger a dropdown menu for: set active, move loop points, delete
        lastTime = millis();
        sequence.activeLoop = cursor;
        return false;
      }
      if(controls.DELETE() && sequence.loopData.size()>1){
        if(binarySelectionBox(64,32,"NO","YEA","delete loop?")==1){
          sequence.deleteLoop(cursor);
          if(cursor >= sequence.loopData.size()){
            cursor = sequence.loopData.size()-1;
            if(menuStart + numberOfLoopsShown > sequence.loopData.size()-1 && menuStart)
              menuStart--;
          }
        }
        lastTime = millis();
      }
      if(controls.NEW()){
        dupeLoop(cursor);
        lastTime = millis();
      }
      if(controls.DOWN() && cursor > 0){
        cursor--;
        if(cursor<menuStart){
          yCoordOffset = -9;
          menuStart--;
        }
        lastTime = millis();
      }
      if(controls.UP() && cursor < sequence.loopData.size()-1){
        cursor++;
        if(cursor > (menuStart+numberOfLoopsShown) && (menuStart + numberOfLoopsShown < sequence.loopData.size()-1)){
          menuStart++;
          yCoordOffset = 9;
        }
        lastTime = millis();
      }
    }
    //changing the loop type
    while(controls.counterB != 0){
      if(controls.counterB>0){
        if(sequence.loopData[cursor].type<INFINITE)
          sequence.loopData[cursor].type++;
        else
          sequence.loopData[cursor].type = NORMAL;
      }
      else{
        if(sequence.loopData[cursor].type>NORMAL)
          sequence.loopData[cursor].type--;
        else
          sequence.loopData[cursor].type = INFINITE;
      }
      controls.counterB += controls.counterB<0?1:-1;
    }
    return true;
  }
};

void loopMenu(){
  LoopMenu menu;
  while(menu.loopMenuControls()){
    menu.displayMenu();
  }
}
