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
    case 'x':
      printSelectionBounds();
      break;
    case 'z':
      debugPrintSelection();
      break;
    case 'F':
      menuIsActive = true;
      genFragment();
      // fragmentMenu();
      break;
    case '/':
      // writeSeqSerial_plain();
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
      updateLookupData();
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
    case 'E'://erases the sequence
      eraseSeq();
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
      toggleRecordingMode(waitForNote);
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



//prints out lookupdata
void debugPrintLookup(){
  for(int i = 0; i<trackData.size();i++){
    //Serial.print(stringify(i)+":"+getTrackPitch(i));
    //Serial.print('|');
    for(int j = seqStart; j<= seqEnd; j++){
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