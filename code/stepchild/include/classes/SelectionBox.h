//Holds coordinates and a flag set when the SelectionBox has been started
class SelectionBox{
  public:
  CoordinatePair coords;
  bool begun;

  SelectionBox(){
    this->begun = false;
  }

  void render(){
    coords.end = Coordinate(sequence.cursorPos,sequence.activeTrack);

    unsigned short int startX;
    unsigned short int startY;
    unsigned short int len;
    unsigned short int height;

    unsigned short int X1;
    unsigned short int X2;
    unsigned short int Y1;
    unsigned short int Y2;

    if(this->coords.start.x>this->coords.end.x){
      X1 = this->coords.end.x;
      X2 = this->coords.start.x;
    }
    else{
      X1 = this->coords.start.x;
      X2 = this->coords.end.x;
    }
    if(this->coords.start.y>this->coords.end.y){
      Y1 = this->coords.end.y;
      Y2 = this->coords.start.y;
    }
    else{
      Y1 = this->coords.start.y;
      Y2 = this->coords.end.y;
    }

    startX = trackDisplay+(X1-sequence.viewStart)*sequence.viewScale;
    len = (X2-X1)*sequence.viewScale;

    //if box starts before view
    if(X1<sequence.viewStart){
      startX = trackDisplay;//box is drawn from beggining, to this->coords.end.x
      len = (X2-sequence.viewStart)*sequence.viewScale;
    }
    //if box ends past view
    if(X2>sequence.viewEnd){
      len = (sequence.viewEnd-X1)*sequence.viewScale;
    }

    //same, but for tracks
    uint8_t startHeight = (menuIsActive||maxTracksShown==5)?headerHeight:8;
    startY = (Y1-startTrack)*trackHeight+startHeight;
    height = ((Y2+1-startTrack)*trackHeight)+startHeight - startY;
    if(Y1<startTrack){
      startY = startHeight;
      height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
    }
    display.drawRect(startX, startY, len, height, SSD1306_WHITE);
    display.drawRect(startX+1, startY+1, len-2, height-2, SSD1306_WHITE);

    if(len>5 && height>=trackHeight){
      display.fillRect(startX+2,startY+2, len-4, height-4, SSD1306_BLACK);
      graphics.shadeArea(startX+2,startY+2, len-4, height-4,5);
    }
  }
};


SelectionBox selBox;


//returns a 2D vector containing a row for each track and a copy of each note that's currently selected on each track
vector<vector<Note>> grabSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<sequence.trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
            if(sequence.noteData[track][note].isSelected()){
                selectedNotesOnTrack.push_back(sequence.noteData[track][note]);
            }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}
vector<vector<Note>> grabAndDeleteSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<sequence.trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
          if(sequence.noteData[track][note].isSelected()){
            selectedNotesOnTrack.push_back(sequence.noteData[track][note]);
            sequence.deleteNote_byID(track,note);
          }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}

vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(sequence.trackData.size());
  menuIsActive = false;
  bool movingBetweenNotes = false;
  while(true){
    controls.readButtons();
    controls.readJoystick();
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
      if(selBox.coords.start.x>selBox.coords.end.x){
        unsigned short int x1_old = selBox.coords.start.x;
        selBox.coords.start.x = selBox.coords.end.x;
        selBox.coords.end.x = x1_old;
      }
      if(selBox.coords.start.y>selBox.coords.end.y){
        unsigned short int y1_old = selBox.coords.start.y;
        selBox.coords.start.y = selBox.coords.end.y;
        selBox.coords.end.y = y1_old;
      }
      for(int track = selBox.coords.start.y; track<=selBox.coords.end.y; track++){
        for(int time = selBox.coords.start.x; time<=selBox.coords.end.x; time++){
          if(sequence.lookupTable[track][time] != 0){
            //if the note isn't in the vector yet, add it
            if(!isInVector(sequence.lookupTable[track][time],selectedNotes[track]))
              selectedNotes[track].push_back(sequence.lookupTable[track][time]);
            time = sequence.noteData[track][sequence.lookupTable[track][time]].endPos;
          }
        }
      }
      selBox.begun = false;
    }
    if(utils.itsbeen(100)){
      if(!movingBetweenNotes){
        if(controls.joystickY == 1){
          setActiveTrack(sequence.activeTrack+1,false);
          lastTime = millis();
        }
        if(controls.joystickY == -1){
          setActiveTrack(sequence.activeTrack-1,false);
          lastTime = millis();
        }
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(!movingBetweenNotes){
          if (controls.joystickX == 1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              moveCursor(-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              moveCursor(-sequence.subDivision);
              lastTime = millis();
            }
          }
          if (controls.joystickX == -1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              moveCursor(sequence.subDivision);
              lastTime = millis();
            }
          }
        }
        else{
          if(controls.joystickX == 1){
            moveToNextNote(false,false);
            lastTime = millis();
          }
          else if(controls.joystickX == -1){
            moveToNextNote(true,false);
            lastTime = millis();
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      //select
      if(controls.SELECT()  && sequence.IDAtCursor() != 0 && !selBox.begun){
        unsigned short int id;
        id = sequence.IDAtCursor();
        if(controls.SHIFT()){
          //del old vec
          vector<vector<uint8_t>> temp;
          temp.resize(sequence.trackData.size());
          selectedNotes = temp;
          selectedNotes[sequence.activeTrack].push_back(sequence.IDAtCursor());
        }
        else{
          //if the note isn't in the vector yet, add it
          if(!isInVector(sequence.IDAtCursor(),selectedNotes[sequence.activeTrack]))
            selectedNotes[sequence.activeTrack].push_back(sequence.IDAtCursor());
          //if it is, remove it
          else{
            vector<uint8_t> temp;
            for(int i = 0; i<selectedNotes[sequence.activeTrack].size(); i++){
              //push back all the notes that aren't the one the cursor is on
              if(selectedNotes[sequence.activeTrack][i] != sequence.IDAtCursor()){
                temp.push_back(selectedNotes[sequence.activeTrack][i]);
              }
            }
            selectedNotes[sequence.activeTrack] = temp;
          }
        }
        lastTime = millis();
      }
      if(controls.DELETE()){
        lastTime = millis();
        selectedNotes.clear();
        break;
      }
      if(controls.NEW()){
        controls.setNEW(false);
        lastTime = millis();
        break;
      }
      if(controls.LOOP()){
        lastTime = millis();
        movingBetweenNotes = !movingBetweenNotes;
      }
    }
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    printSmall(0,0,text1,SSD1306_WHITE);
    printSmall(0,8,text2,SSD1306_WHITE);
    if(movingBetweenNotes){
      if(millis()%1000 >= 500){
        display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
      }
      else{
        display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
      }
    }
    //draw a note bracket on any note that's been added to the selection
    for(int track = 0; track<selectedNotes.size(); track++){
      for(int note = 0; note<selectedNotes[track].size(); note++){
        graphics.drawNoteBracket(sequence.noteData[track][selectedNotes[track][note]],track);
      }
    }
    display.display();
  }
  menuIsActive = true;
  return selectedNotes;
}

vector<uint16_t> getSelectedNotesBoundingBox(){
  //stored as xStart,yStart,xEnd,yEnd
  //(initially store it as dramatic as possible)
  vector<uint16_t> bounds = {sequence.sequenceLength,(uint16_t)sequence.trackData.size(),0,0};
  uint16_t checkedNotes = 0;
  for(uint8_t track = 0; track<sequence.noteData.size(); track++){
    for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
      if(sequence.noteData[track][note].isSelected()){
        //if it's the new highest track
        if(track<bounds[1])
          bounds[1] = track;
        //if it's the new lowest track
        if(track>bounds[3])
          bounds[3] = track;
        //if it's the new earliest note
        if(sequence.noteData[track][note].startPos<bounds[0])
          bounds[0] = sequence.noteData[track][note].startPos;
        //if it's the new latest note
        if(sequence.noteData[track][note].endPos>bounds[2])
          bounds[2] = sequence.noteData[track][note].endPos;

        //if you've checked all the selected notes, return
        checkedNotes++;
        if(checkedNotes == sequence.selectionCount)
          return bounds;
      }
    }
  }
  return bounds;
}

vector<uint8_t> getTracksWithSelectedNotes(){
  vector<uint8_t> list;
  uint16_t count;
  if(sequence.selectionCount>0){
    for(uint8_t track = 0; track<sequence.trackData.size(); track++){
      for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
        //once you find a selected note, jump to the next track
        if(sequence.noteData[track][note].isSelected()){
          list.push_back(track);
          track++;
          note = 1;
        }
      }
    }
  }
  return list;
}

void clearSelection(int track, int time) {
  if(sequence.lookupTable[track][time] != 0 && sequence.noteData[track][sequence.lookupTable[track][time]].isSelected()){
    sequence.selectionCount--;
    sequence.noteData[track][sequence.lookupTable[track][time]].setSelected(false);
  }
}

void clearSelection(){
  if(sequence.selectionCount>0){
    for(int track = 0; track<sequence.trackData.size(); track++){
      if(sequence.selectionCount<=0)
          return;
      for(int note = sequence.noteData[track].size()-1; note>0; note--){
        if(sequence.selectionCount<=0)
          return;
        if(sequence.noteData[track][note].isSelected()){
          sequence.noteData[track][note].setSelected(false);
          sequence.selectionCount--;
        }
      }
    }
  }
}

void deselectNote(uint8_t track, uint16_t id){
  if(sequence.noteData[track][id].isSelected()){
    sequence.selectionCount--;
    sequence.noteData[track][id].setSelected(false);
  }
}

void selectNotesInTrack(uint8_t track){
  for(uint16_t note = 1; note<sequence.noteData.size(); note++){
    if(!sequence.noteData[track][note].isSelected()){
      sequence.noteData[track][note].setSelected(true);
      sequence.selectionCount++;
    }
  }
}

//select a note
void selectNote(uint8_t track, uint16_t id){
  //if it's already selected
  if(sequence.noteData[track][id].isSelected())
    return;
  sequence.noteData[track][id].setSelected(true);
  sequence.selectionCount++;
}

//togglet a note's selection state  by it's track and ID
void toggleSelectNote(uint8_t track, uint16_t id, bool additive){
    //if id == 0, just return
    if(!id){
      return;
    }
    if(!additive&&!sequence.noteData[track][id].isSelected()){
      clearSelection();
      selectNote(track,id);
    }
    else{
      if(sequence.noteData[track][id].isSelected() && sequence.selectionCount>0)
        deselectNote(track,id);
      else if(!sequence.noteData[track][id].isSelected()){
        selectNote(track,id);
      }
    }
}


void selectAllNotesInTrack(){
  for(uint16_t i = 1; i<sequence.noteData[sequence.activeTrack].size();i++){
    selectNote(sequence.activeTrack, i);
  }
}

//selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
void selectAll() {
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    for(uint16_t id = 1; id<sequence.noteData[track].size(); id++){
      selectNote(track,id);
    }
  }
}

void selectBox(){
  if(selBox.coords.start.x>selBox.coords.end.x){
    unsigned short int x1_old = selBox.coords.start.x;
    selBox.coords.start.x = selBox.coords.end.x;
    selBox.coords.end.x = x1_old;
  }
  if(selBox.coords.start.y>selBox.coords.end.y){
    unsigned short int y1_old = selBox.coords.start.y;
    selBox.coords.start.y = selBox.coords.end.y;
    selBox.coords.end.y = y1_old;
  }
  for(int track = selBox.coords.start.y; track<=selBox.coords.end.y; track++){
    for(int time = selBox.coords.start.x; time<selBox.coords.end.x; time++){//< and not <= so it doesn't grab trailing notes
      if(sequence.lookupTable[track][time] != 0){
        //this is a little inconsistent with how select usually works, but it allows whatever's in the box to DEFINITELY be selected.
        //it makes sense (a little) because it seems rare that you would ever need to deselect notes using the box
        selectNote(track, sequence.lookupTable[track][time]);
        time = sequence.noteData[track][sequence.lookupTable[track][time]].endPos-1;
      }
    }
  }
}


//holds the copied note data
class ClipBoard{
  public:

  Coordinate relativeCursorPosition;
  vector<vector<Note>> buffer;

  ClipBoard(){};

  void copy(){
    //if no notes are selected, return
    if(!sequence.selectionCount)
      return;
    
    //clear out old copy buffer
    vector<vector<Note>> temp1;
    temp1.resize(sequence.trackData.size());
    this->buffer.swap(temp1);
    //get the new relative cursor pos
    this->relativeCursorPosition = Coordinate(sequence.cursorPos,sequence.activeTrack);

    uint16_t numberOfNotes = 0;

    //add all selected notes to the copy buffer
    if(sequence.selectionCount>0){
      for(int track = 0; track<sequence.trackData.size(); track++){
        for(int note = 1; note<=sequence.noteData[track].size()-1; note++){// <= bc notes aren't 0 indexed
          if(sequence.noteData[track][note].isSelected()){
            this->buffer[track].push_back(sequence.noteData[track][note]);
            numberOfNotes++;
          }
        }
      }
      clearSelection();
    }
    //or if there's a target note, but it's not selected
    else if(sequence.IDAtCursor() != 0){
      this->buffer[sequence.activeTrack].push_back(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()]);
      numberOfNotes = 1;
    }
    menuText = "copied "+stringify(numberOfNotes)+((stringify(numberOfNotes)=="1")?" note":" notes");
  }
  void copyLoop(uint8_t loopID){
    //clear copyBuffer
    while(this->buffer.size()>0){
      this->buffer.pop_back();
    }
    //making sure buffer has enough 'columns' to store the notes from each track
    this->buffer.resize(sequence.trackData.size());
    //treat copying the loop like you're copying it from the start of the loop
    this->relativeCursorPosition = Coordinate(sequence.loopData[loopID].start,0);
    //add all selected notes to the copy buffer
    if(sequence.loopData[loopID].end-sequence.loopData[loopID].start>0){
      for(uint8_t track = 0; track<sequence.trackData.size(); track++){
        for(uint16_t step = sequence.loopData[loopID].start; step<=sequence.loopData[loopID].end; step++){// <= bc notes aren't 0 indexed
          if(sequence.lookupTable[track][step] != 0){
            this->buffer[track].push_back(sequence.noteData[track][sequence.lookupTable[track][step]]);
            //move to the end of the note, so it's not double-counted
            step = sequence.noteData[track][sequence.lookupTable[track][step]].endPos;
          }
        }
      }
    }
  }
  void copyLoop(){
    this->copyLoop(sequence.activeLoop);
  }
  void pasteAt(uint8_t track, uint16_t step){
    if(this->buffer.size()>0){
      uint16_t pastedNotes;
      //offset of all the notes (relative to where they were copied from)
      int16_t yOffset = track - this->relativeCursorPosition.y;
      int16_t xOffset = step - this->relativeCursorPosition.x;
      //moves through each track and note in copyBuffer, places a note at those positions in the seq
      for(int tracks = 0; tracks<this->buffer.size(); tracks++){//for each track in the copybuffer
        if(this->buffer[tracks].size()>0 && tracks+yOffset>=0 && tracks+yOffset<sequence.trackData.size()){//if there's a note stored for this track, and it'd be copied according to the new sequence.activeTrack
          for(int notes = 0; notes<this->buffer[tracks].size(); notes++){
            if(this->buffer[tracks][notes].startPos + xOffset<= sequence.sequenceLength){
              //if the note will be copied to someWhere within the seqence, make note
              int start = xOffset+this->buffer[tracks][notes].startPos;
              int end = xOffset+this->buffer[tracks][notes].endPos;
              int track = tracks+yOffset;
              unsigned char vel = this->buffer[tracks][notes].velocity;
              unsigned char chance = this->buffer[tracks][notes].chance;
              bool mute = this->buffer[tracks][notes].isMuted();

              Note newNote = this->buffer[tracks][notes];
              newNote.startPos += xOffset;
              newNote.endPos += xOffset;

              //if note ends past seq, truncate it
              if(newNote.endPos>sequence.sequenceLength)
                newNote.endPos = sequence.sequenceLength;
              //if note begins before seq, but also extends into seq, truncate it
              if(newNote.startPos<0 && newNote.endPos>0)
                newNote.startPos = 0;
              else if(newNote.startPos<0 && newNote.endPos<=0)
                continue;
              if(track<0||track>=sequence.trackData.size())
                continue;
              sequence.makeNote(newNote,track,false);
              pastedNotes++;
            }
          }
        }
      }
      menuText = "pasted "+stringify(pastedNotes)+((stringify(pastedNotes)=="1")?" note":" notes");
    }
  }
  void paste(){
    this->pasteAt(sequence.activeTrack,sequence.cursorPos);
  }
};

ClipBoard clipboard;

