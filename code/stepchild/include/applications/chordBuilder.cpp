//this generator makes a chord from a scale (or all twelve keys), and can be specified to include notes
//"mask" is the scale and "definiteKeys" are the keys that will be in the chord
vector<uint8_t> genRandomChord(vector<uint8_t> mask,vector<uint8_t> definiteKeys, uint8_t numberOfNotes,uint8_t octave){
  //if mask is empty, just make it all the notes
  if(mask.size() == 0){
    for(uint8_t i = 0; i<36; i++){
      mask.push_back(i+12*octave);
    }
  }
  //notes starts off already including definiteKeys
  vector<uint8_t> notes = definiteKeys;
  for(uint8_t i = notes.size(); i<numberOfNotes; i++){
    uint8_t note = mask[random(0,mask.size())];
    //if this note is already present, keep genning new notes
    while(isInVector(note,notes)){
      note = mask[random(0,mask.size())];
      //if all the notes are already in the mask
      if(notes.size() == mask.size()){
        break;
      }
    }
    notes.push_back(note);
  }
  return notes;
}

//chord builder
void chordBuilder(){
  Progression chordSequence;
  uint8_t activeChord = 0;
  uint8_t keyCursor = 0;
  vector<uint8_t> pressedKeys;
  vector<uint8_t> mask;
  uint16_t length = 24;
  uint8_t octave = 0;
  uint8_t maskIndex = 0;
  //0 = selecting notes for a new chord
  //1 = selecting a chord to edit
  //2 = editing a chord
  uint8_t editorState = 0;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    while(controls.counterA != 0){
      uint16_t *lengthPointer;
      //switch so you can edit the new length val, or the length of an existing chord
      if(editorState == 1){
        lengthPointer = &(chordSequence.chords[activeChord].length);
      }
      else{
        lengthPointer = &length;
      }
      //changing length
      if(!controls.SHIFT()){
        if(controls.counterA >= 1 && (*lengthPointer)<96){
          (*lengthPointer)+=sequence.subDivision;
          if((*lengthPointer)>96){
            (*lengthPointer) = 96;
          }
          controls.counterA += controls.counterA<0?1:-1;;
        }
        if(controls.counterA <= -1 && (*lengthPointer)>sequence.subDivision){
          (*lengthPointer)-=sequence.subDivision;
          if((*lengthPointer)<sequence.subDivision){
            (*lengthPointer) = sequence.subDivision;
          }
          controls.counterA += controls.counterA<0?1:-1;;  
        }
      }
    }
    while(controls.counterB != 0){
      if(!controls.SHIFT()){   
        if(controls.counterB >= 1){
          sequence.changeSubDivInt(true);
          controls.counterB += controls.counterB<0?1:-1;;
        }
        //changing subdivint
        if(controls.counterB <= -1){
          sequence.changeSubDivInt(false);
          controls.counterB += controls.counterB<0?1:-1;;
        }
      }
    }
    if(utils.itsbeen(150)){
      if(controls.joystickY != 0){
        lastTime = millis();
        if(controls.joystickY == 1 && editorState == 0 && chordSequence.chords.size()>0){
          activeChord = 0;
          editorState = 1;
        }
        else if(controls.joystickY == -1 && editorState == 1){
          editorState = 0;
        }
      }
      if(controls.joystickX != 0){
        //moving between chords in the chord sequence
        if(editorState == 1){
          if(chordSequence.chords.size()>0){
            if(controls.joystickX == -1 && activeChord<(chordSequence.chords.size()-1)){
              activeChord++;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && activeChord>0){
              activeChord--;
              lastTime = millis();
            }
          }
        }
        else if(editorState == 0 || editorState == 2){
          //if controls.SHIFT(), move by an octave
          if(controls.SHIFT()){
            if(controls.joystickX == -1 &&keyCursor<=24){
              keyCursor+=12;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && keyCursor>=12){
              keyCursor-=12;
              lastTime = millis();
            }
          }
          //if it's masked, it moves to the next masked note
          if(mask.size()>0){
            if(controls.joystickX == -1 && maskIndex<mask.size()-1){
              keyCursor = mask[maskIndex++];
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && maskIndex>0){
              keyCursor = mask[maskIndex--];
              lastTime = millis();
            }
            if(keyCursor<0)
              keyCursor = mask[maskIndex++];
            if(keyCursor>=36)
              keyCursor = mask[maskIndex--];
          }
          else{
            if(controls.joystickX == -1 && keyCursor<35){
              keyCursor++;
              lastTime = millis();
            }
            else if(controls.joystickX == 1 && keyCursor>0){
              keyCursor--;
              lastTime = millis();
            }
          }
        }
      }
    }
    if(utils.itsbeen(200)){
      if(controls.LOOP()){
        lastTime = millis();
        //gen a random sequence
        if(controls.SHIFT()){
          //always adds 4 random notes to the selection
          pressedKeys = genRandomChord(mask,pressedKeys,pressedKeys.size()+4,octave);
        }
        else{
          //if there's no mask, make one
          //(mask uses highlit note as root)
          if(mask.size() == 0){
            //if it's within the first octave
            // if(keyCursor/12 == 0)
              mask = genScale(MAJOR,keyCursor%12,3,octave);
          }
          //if there is, unmake one
          else{
            vector<uint8_t> temp;
            mask.swap(temp);
          }
        }
      }
      //selecting/deselecting notes
      if(controls.SELECT() ){
        //making new chord
        if(editorState == 0 || editorState == 2){
          lastTime = millis();
          //deselect all notes
          if(controls.SHIFT()){
            vector<uint8_t> temp;
            pressedKeys.swap(temp);
          }
          else{
            //if the key is already selected, remove it
            if(isInVector(keyCursor,pressedKeys)){
              vector<uint8_t> temp;
              //removing keys from pressedKeys;
              for(uint8_t i = 0; i<pressedKeys.size(); i++){
                if(pressedKeys[i] != keyCursor){
                  temp.push_back(pressedKeys[i]);
                }
              }
              pressedKeys.swap(temp);
            }
            else{
              pressedKeys.push_back(keyCursor);
            }
          }
          //setting the active chord intervals to the newly pressed keys if you're in edit mode
          if(editorState == 2){
            uint8_t root = getLowestVal(pressedKeys)+12*octave;
            vector<uint8_t> intervals;
            for(uint8_t i = 0; i<pressedKeys.size(); i++){
              intervals.push_back(pressedKeys[i]-root);
            }
            chordSequence.chords[activeChord].root = root;
            chordSequence.chords[activeChord].intervals = intervals;
          }
        }
        //transitioning to edit chord mode
        else if(editorState == 1){
          lastTime = millis();
          editorState = 2;
          //swap pressed keys for the chord intervals
          vector<uint8_t> tempNotes;
          for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
            tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
          }
          pressedKeys.swap(tempNotes);
        }
      }
      //making a chord
      if(controls.NEW()){
        //new chord mode
        if(editorState == 0){
          lastTime = millis();
          //commit chord
          if(controls.SHIFT()){
            chordSequence.commit();
            return;
          }
          else{
            if(pressedKeys.size()>0){
              uint8_t root = getLowestVal(pressedKeys)+12*octave;
              vector<uint8_t> intervals;
              for(uint8_t i = 0; i<pressedKeys.size(); i++){
                intervals.push_back(pressedKeys[i]-root);
              }
              Chord newChord = Chord(root,intervals,length);
              chordSequence.add(newChord);
              vector<uint8_t> temp;
              pressedKeys.swap(temp);
            }
          }
        }
        //edit chord mode (commits keys to chord)
        if(editorState == 2){
          lastTime = millis();
          if(pressedKeys.size()>0){
            uint8_t root = getLowestVal(pressedKeys)+12*octave;
            vector<uint8_t> intervals;
            for(uint8_t i = 0; i<pressedKeys.size(); i++){
              intervals.push_back(pressedKeys[i]-root);
            }
            //setting chord intervals to the new, edited intervals
            chordSequence.chords[activeChord].intervals = intervals;
            vector<uint8_t> temp;
            pressedKeys.swap(temp);
            editorState = 0;
          }
          else{
            chordSequence.remove(activeChord);
            if(chordSequence.chords.size() == 0)
              activeChord = 0;
            else if(activeChord>=chordSequence.chords.size())
              activeChord = chordSequence.chords.size()-1;
            editorState = 0;
          }
        }
      }
      if(controls.DELETE()){
        if(editorState == 1){
          chordSequence.remove(activeChord);
          lastTime = millis();
          //if there're no chords left, set activeC to 0
          //else, lower it by one
          chordSequence.chords.size() > 0 ? activeChord-- : activeChord = 0;
        }
      }
      if(controls.MENU()){
        lastTime = millis();
        if(editorState == 0 || editorState == 1){
          return;
        }
        else if(editorState == 2){
          editorState = 0;
        }
      }
    }
    display.clearDisplay();
    //normal, make-new-chord mode
    if(editorState == 0){
      graphics.drawFullKeyBed(pressedKeys,mask,keyCursor,octave);
      chordSequence.drawProg(32, 50, -1);

      //if you're not editing a chord, display length var and subDiv
      //making nice boxes
      //top
      display.drawRoundRect(101,33,13,12,3,SSD1306_WHITE);
      //bottom
      display.drawRoundRect(101,47,13,11,3,SSD1306_WHITE);
      //side
      display.fillRoundRect(111,30,21,30,3,SSD1306_BLACK);
      display.drawRoundRect(111,30,21,30,3,SSD1306_WHITE);

      //display sequence.subDivision
      String text = stepsToMeasures(sequence.subDivision);
      printSmall(103,37,"~",SSD1306_WHITE);
      graphics.printFractionCentered(121,36,stepsToMeasures(sequence.subDivision));
      
      //length
      text = stepsToMeasures(length);
      printSmall(105,50,"L",SSD1306_WHITE);
      graphics.printFractionCentered(121,49,stepsToMeasures(length));
      graphics.drawCenteredBanner(64,55,convertVectorToPitches(pressedKeys));
    }
    else if(editorState == 1){
      //draw the keys that are in the chord as "selected"
      //send a temp vector that's adjusted so the notes display correctly
      vector<uint8_t> tempNotes;
      for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
        tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
      }
      graphics.drawFullKeyBed(tempNotes,mask,keyCursor,0);
      chordSequence.drawProg(32, 50, activeChord);
      //if you are editing a chord, display it's length
      graphics.printFraction(115,32,stepsToMeasures(chordSequence.chords[activeChord].length));
    }
    else if(editorState == 2){
      vector<uint8_t> tempNotes;
      // for(uint8_t i = 0; i<chordSequence.chords[activeChord].intervals.size(); i++){
      //   tempNotes.push_back(chordSequence.chords[activeChord].intervals[i]+chordSequence.chords[activeChord].root);
      // }
      graphics.drawFullKeyBed(pressedKeys,mask,keyCursor,0);
      //draw a second keybed lower, JUST showing the notes that are in-key
      graphics.drawFullKeyBed(32,tempNotes,pressedKeys,255,0);
      graphics.drawCenteredBanner(64,55,chordSequence.chords[activeChord].getPitchList(0,59));
    }
    //if there's a mask
    if(mask.size()>0){
      graphics.drawCenteredBanner(64,55,pitchToString(mask[0],false,true)+" major");
    }
    display.display();
  }
}
