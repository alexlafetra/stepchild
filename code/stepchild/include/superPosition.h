class SuperpositionMenu{
  public:
    Note note;
    uint8_t track;
    SuperpositionMenu(Note& n, uint8_t t){
      note = n;
      track = t;
      //first, set the superpos to the track pitch (if it's unset)
      if(note.superposition.pitch == 255)
        note.superposition.pitch = sequence.trackData[track].pitch;
    }
    bool setSuperpositionControls();
    void drawSuperposSelect();
};

bool SuperpositionMenu::setSuperpositionControls(){
  controls.readButtons();
  controls.readJoystick();
  while(controls.counterA){
    if(controls.counterA > 0 && note.superposition.odds > 0){
      note.superposition.odds -= 5;
    }
    if(controls.counterA < 0 && note.superposition.odds < 100){
      note.superposition.odds += 5;
    }
    controls.counterA += controls.counterA<0?1:-1;
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == -1) {
      if(note.superposition.pitch < 127){
        note.superposition.pitch++;
        if(!sequence.playing()){
          MIDI.noteOn(note.superposition.pitch, note.velocity, sequence.trackData[track].channel);
          MIDI.noteOff(note.superposition.pitch, 0, sequence.trackData[track].channel);
        }
        lastTime = millis();
      }
    }
    if (controls.joystickY == 1) {
      if(note.superposition.pitch > 0){
        note.superposition.pitch--;
        if(!sequence.playing()){
          MIDI.noteOn(note.superposition.pitch, note.velocity, sequence.trackData[track].channel);
          MIDI.noteOff(note.superposition.pitch, 0, sequence.trackData[track].channel);
        }
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.NEW() || controls.MENU() || controls.LOOP()){
      lastTime = millis();
      if(note.superposition.pitch == sequence.trackData[track].pitch){
        note.superposition.pitch = 255;//set to 'unset' if it's on the same pitch
      }
      return false;
    }
    if(controls.PLAY()){
      if(controls.SHIFT() || sequence.recording()){
        toggleRecordingMode(waitForNoteBeforeRec);
        lastTime = millis();
        if(sequence.recording())
          menuText = "ready!";
      }
      else{
        togglePlayMode();
        lastTime = millis();
        menuText = sequence.playing()?"playing!":"stopped";
      }
    }
    if(controls.DELETE()){
      lastTime = millis();
      note.superposition.pitch = sequence.trackData[track].pitch;//reset
    }
  }
  return true;
}

void SuperpositionMenu::drawSuperposSelect(){
  SequenceRenderSettings settings;
  NoteCoords nCoords = getNoteCoords(note, track, settings);

  const uint8_t headerHeight = 12;

  // NoteCoords n2Coords = nCoords;
  NoteCoords n2Coords = getNoteCoords(note, track, settings);
  n2Coords.y1 = nCoords.y1+(int16_t(sequence.trackData[track].pitch)-int16_t(note.superposition.pitch)) * trackHeight;
  n2Coords.y2 = n2Coords.y1 + trackHeight;

  if(n2Coords.y1<headerHeight){
    nCoords.offsetY(-n2Coords.y1+headerHeight);
    n2Coords.offsetY(-n2Coords.y1+headerHeight);
  }
  if(n2Coords.y2>(SCREEN_HEIGHT-2)){
    nCoords.offsetY(SCREEN_HEIGHT-n2Coords.y2-2);
    n2Coords.offsetY(SCREEN_HEIGHT-n2Coords.y2-2);
  }

  //drawing horizontal pitch lines
  for(uint8_t i = headerHeight; i<SCREEN_HEIGHT; i++){
    //octave
    if(abs(nCoords.y1 - i)%(12*trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,2);
    //4 semitones
    else if(abs(nCoords.y1 - i)%(4*trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,3);
    else if(abs(nCoords.y1 - i)%(trackHeight) == 0)
      graphics.drawDottedLineH(0,SCREEN_WIDTH,i+trackHeight/2,12);
  }

  drawNoteBracket(n2Coords,true);

  String txt = stringify(note.superposition.pitch-sequence.trackData[track].pitch);
  if(note.superposition.pitch>sequence.trackData[track].pitch)
    txt = "+"+txt;

  //draw superposition
  graphics.fillRectWithMissingCorners(n2Coords.x1, n2Coords.y1-1, n2Coords.length+1, trackHeight, SSD1306_BLACK);
  graphics.drawRectWithMissingCorners(n2Coords.x1, n2Coords.y1-1, n2Coords.length+1, trackHeight, SSD1306_WHITE);

  printSmall(n2Coords.x1+n2Coords.length+4,n2Coords.y1+2,txt+"("+pitchToString(note.superposition.pitch,true,true)+")",1);
  drawNote(note, track, nCoords, settings);//draw the note (like normal)

  //draw probability
  graphics.fillSquareDiagonally(0,0,15,note.superposition.odds);
  printSmall(6,5,"%",2);
  printSmall(17,5,stringify(note.superposition.odds)+"%",1);
  printCursive(50,4,"superposition",1);
}

void setSuperposition(Note& note,uint8_t originalTrack){
  SuperpositionMenu superposMenu(note,originalTrack);
  while(superposMenu.setSuperpositionControls()){
    display.clearDisplay();
    superposMenu.drawSuperposSelect();
    display.display();
  }
  note.superposition = superposMenu.note.superposition;
  controls.clearButtons();
}