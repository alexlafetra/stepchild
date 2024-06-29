//Fit 'A' into 'B'

// 'into', 11x13px
const unsigned char into_bmp [] = {
	0xe0, 0x00, 0x42, 0x00, 0x47, 0x00, 0xe9, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x20, 0x01, 0xe0, 
	0x00, 0x20, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x20, 0x00, 0xc0
};


struct NoteTrackPair{
public:
    Note note;
    uint8_t trackID;
    NoteTrackPair(Note n, uint8_t t);
};
NoteTrackPair::NoteTrackPair(Note n, uint8_t t){
  note = n;
  trackID = t;
}


CoordinatePair selectArea_warp(bool AorB){
  CoordinatePair coords;
  coords.start.x = 0;
  coords.end.x = 0;
  coords.start.y = 0;
  coords.end.y = 0;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    defaultEncoderControls();
    if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
      selBox.begun = true;
      selBox.coords.start.x = sequence.cursorPos;
      selBox.coords.start.y = sequence.activeTrack;
      coords.start.x = sequence.cursorPos;
      coords.start.y = sequence.activeTrack;
    }
    //if controls.SELECT()  is released, and there's a selection box
    if(!controls.SELECT()  && selBox.begun){
      selBox.coords.end.x = sequence.cursorPos;
      selBox.coords.end.y = sequence.activeTrack;
      selBox.begun = false;
      coords.end.x = sequence.cursorPos;
      coords.end.y = sequence.activeTrack;
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        return coords;
      }
      if(controls.MENU()){
        coords.start.x = 0;
        coords.end.x = 0;
        coords.start.y = 0;
        coords.end.y = 0;
        lastTime = millis();
        return coords;
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !controls.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
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
      if (controls.joystickY == 1) {
        if(recording)
          setActiveTrack(sequence.activeTrack + 1, false);
        else
          setActiveTrack(sequence.activeTrack + 1, true);
        lastTime = millis();
      }
      if (controls.joystickY == -1) {
        if(recording)
          setActiveTrack(sequence.activeTrack - 1, false);
        else
          setActiveTrack(sequence.activeTrack - 1, true);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(50)) {
      if (controls.joystickX == 1 && controls.SHIFT()) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && controls.SHIFT()) {
        moveCursor(1);
        lastTime = millis();
      }
    }
    display.clearDisplay();
    drawSeq(true, false, true, false, false, false, sequence.viewStart, sequence.viewEnd);
    drawCoordinateBox(coords);
    if(coords.start.x == 0 && coords.end.x == 0 && coords.start.y == 0 && coords.end.y == 0){
      printSmall(trackDisplay,0,"warp",1);
      if(AorB){
        if(millis()%500>250)
          printItalic(trackDisplay+16,0,"A",1);
        printItalic(trackDisplay+42,0,"B",1);
      }
      else{
        printItalic(trackDisplay+16,0,"A",1);
        if(millis()%500>250)
          printItalic(trackDisplay+42,0,"B",1);
      }
      printSmall(trackDisplay+25,0,"into",1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to submit",1);
      if(millis()%500>250)
        printItalic(trackDisplay+50,0,AorB?"A":"B",1);
    }
    drawWarpIcon(8,2,11,true);
    display.display();
  }
}

//Warps the notes contained within the 'A' coordinates into the 'B' coordinates
bool warpAintoB(CoordinatePair A, CoordinatePair B, bool onlySelected){
  //checking bounds
  if(A.start.x == A.end.x || B.start.x == B.end.x){
      return false;
  }
  if(A.start.x>A.end.x){
      uint16_t temp = A.end.x;
      A.end.x = A.start.x;
      A.start.x = temp;
  }
  if(B.start.x>B.end.x){
      uint16_t temp = B.end.x;
      B.end.x = B.start.x;
      B.start.x = temp;
  }

  //this is the scale of the two areas
  float scaleFactor = float(abs(B.end.x-B.start.x))/float(abs(A.end.x-A.start.x));

  //you need to multiply every note length by that value
  //and also:
  //set the start of the note to be relative
  //to  the start of coordinatePair B * the scaleFactor

  vector<NoteTrackPair> newNotes;
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
      for(uint8_t noteID = 1; noteID<sequence.noteData[track].size(); noteID++){
          //if the note starts SOMEWHERE within A, warp it!
          //you might want to change this/add an option
          //to ONLY warp notes that are entirely contained within A
          if(sequence.noteData[track][noteID].startPos>=A.start.x && sequence.noteData[track][noteID].startPos<A.end.x && ((onlySelected && sequence.noteData[track][noteID].isSelected()) || !onlySelected)){
              Note targetNote = sequence.noteData[track][noteID];
              uint16_t oldLength = targetNote.getLength();
              uint16_t distanceFromStartOfA = targetNote.startPos - A.start.x;

              Note newNote = targetNote;
              newNote.startPos = distanceFromStartOfA*scaleFactor+B.start.x;
              newNote.endPos = newNote.startPos+(oldLength-1)*scaleFactor;
              //if the note will be less than 1 timestep long, don't warp it
              if(newNote.endPos<=newNote.startPos+1){
                continue;
              }

              //controls.DELETE()eting old note
              sequence.deleteNote_byID(track,noteID);
              //make sure to decrement noteID! so you don't warp the same note twice or skip a note
              noteID--;

              // sequence.makeNote(newNote,track,false);
              // newNote.push_back(newNote);
              NoteTrackPair newNT(newNote,track);
              newNotes.push_back(newNT);
          }
      }
  }
  //iterating over the newNotes vector and making each note
  for(uint8_t note = 0; note<newNotes.size(); note++){
    sequence.makeNote(newNotes[note].note,newNotes[note].trackID,false);
  }
  return true;
}

//flag for only warping selected notes
void selectAreasAndWarp(bool onlySelected){
    CoordinatePair A = selectArea_random();
    CoordinatePair B = selectArea_random();
    warpAintoB(A,B,onlySelected);
}

void warp(){
  CoordinatePair A;
  CoordinatePair B;
  //0 is A, 1 is B, 2 is warp
  uint8_t cursor = 0;
  while(true){
    A = selectArea_warp(true);
    if(A.isVertical())
      break;
    B = selectArea_warp(false);
    if(B.isVertical())
      break;
    //only warps selected notes, if any are selected
    if(!warpAintoB(A,B,sequence.selectionCount != 0)){
      break;
    }
  }
}
