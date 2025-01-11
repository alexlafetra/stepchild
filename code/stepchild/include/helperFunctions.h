
//Enter note selection screen, and display a custom icon in the corner
//Used for the FX a lot
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
  while(true){
    controls.readJoystick();
    controls.readButtons();
    defaultEncoderControls();
    if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
      selBox.begun = true;
      selBox.coords.start.x = sequence.cursorPos;
      selBox.coords.start.y = sequence.activeTrack;
    }
    //if controls.SELECT()  is released, and there's a selection box
    if(!controls.SELECT()  && selBox.begun){
      selBox.coords.end.x = sequence.cursorPos;
      selBox.coords.end.y = sequence.activeTrack;
      selBox.begun = false;
      selectBox();
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        return true;
      }
      if(controls.MENU()){
        clearSelection();
        lastTime = millis();
        return false;
      }
      if(controls.SELECT() ){
        if(controls.SHIFT()){
          clearSelection();
          toggleSelectNote(sequence.activeTrack,sequence.IDAtCursor(),false);
        }
        else{
          toggleSelectNote(sequence.activeTrack,sequence.IDAtCursor(),true);
        }
        lastTime = millis();
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !controls.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(sequence.cursorPos%sequence.subDivision){
          sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          sequence.moveCursor(-sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickX == -1 && !controls.SHIFT()) {
        if(sequence.cursorPos%sequence.subDivision){
          sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          sequence.moveCursor(sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickY == 1) {
        if(sequence.recording())
          sequence.setActiveTrack(sequence.activeTrack + 1, false);
        else
          sequence.setActiveTrack(sequence.activeTrack + 1, false);
        lastTime = millis();
      }
      if (controls.joystickY == -1) {
        if(sequence.recording())
          sequence.setActiveTrack(sequence.activeTrack - 1, false);
        else
          sequence.setActiveTrack(sequence.activeTrack - 1, false);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(50)) {
      if (controls.joystickX == 1 && controls.SHIFT()) {
        sequence.moveCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && controls.SHIFT()) {
        sequence.moveCursor(1);
        lastTime = millis();
      }
    }
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    drawSeq(settings);
    if(!sequence.selectionCount){
      printSmall(trackDisplay,0,"select notes to "+text,1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to "+text+" "+stringify(sequence.selectionCount)+(sequence.selectionCount == 1?" note":" notes"),1);
    }
    iconFunction(7,1,14,true);
    display.display();
  }
}


vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(sequence.trackData.size());
  bool movingBetweenNotes = false;
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawLoopPoints = false;
  settings.trackSelection = false;
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
          sequence.setActiveTrack(sequence.activeTrack+1,false);
          lastTime = millis();
        }
        if(controls.joystickY == -1){
          sequence.setActiveTrack(sequence.activeTrack-1,false);
          lastTime = millis();
        }
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(!movingBetweenNotes){
          if (controls.joystickX == 1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              sequence.moveCursor(-sequence.subDivision);
              lastTime = millis();
            }
          }
          if (controls.joystickX == -1 && !controls.SHIFT()) {
            if(sequence.cursorPos%sequence.subDivision){
              sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
              lastTime = millis();
            }
            else{
              sequence.moveCursor(sequence.subDivision);
              lastTime = millis();
            }
          }
        }
        else{
          if(controls.joystickX == 1){
            sequence.moveToNextNote(false,false);
            lastTime = millis();
          }
          else if(controls.joystickX == -1){
            sequence.moveToNextNote(true,false);
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
    drawSeq(settings);
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
        drawNoteBracket(sequence.noteData[track][selectedNotes[track][note]],track,settings);
      }
    }
    display.display();
  }
  return selectedNotes;
}

CoordinatePair selectNotesAndArea(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
    CoordinatePair bounds;
    //storing a copy of the current loop, so we can edit the current loop to set our bounds
    //and then reset it back to the original loop
    while(true){
        controls.readJoystick();
        controls.readButtons();
        defaultSelectBoxControls();
        defaultJoystickControls(false);
        defaultEncoderControls();
        if(utils.itsbeen(200)){
            defaultLoopControls();
            defaultSelectControls();
            if(controls.MENU()){
                lastTime = millis();
                clearSelection();
                break;
            }
            if(controls.NEW()){
                lastTime = millis();
                break;
            }
        }
        display.clearDisplay();
        SequenceRenderSettings settings;
        settings.topLabels = false;
        settings.shadeOutsideLoop = true;
        settings.drawPram = false;
        settings.shrinkTopDisplay = false;
        drawSeq(settings);
        printSmall(trackDisplay,0,text,1);
        iconFunction(7,1,14,true);
        display.display();
    }
    bounds.start.x = sequence.loopData[sequence.activeLoop].start;
    bounds.end.x = sequence.loopData[sequence.activeLoop].end;
    return bounds;
}

//Functions and definitions for generating scales
// #define MAJOR 0
// #define DORIAN 1
// #define PHRYGIAN 2
// #define LYDIAN 3
// #define MIXOLYDIAN 4
// #define AEOLIAN 5
// #define LOCRIAN 6

// #define MELODIC_MINOR 7
// #define HARMONIC_MINOR 8
// #define MAJOR_PENTATONIC 9
// #define MINOR_PENTATONIC 10
// #define BLUE 10


String getScaleName(ScaleName scale){
  const String scales[12] = {"major","dorian","phrygian","lydian","mixolydian","aeolian","locrian","melodic minor","harmonic minor","major pentatonic","minor pentatonoic","blue"};
  return scales[static_cast<uint8_t>(scale)];
}

vector<uint8_t> genScale(ScaleName scale, uint8_t root, int8_t numOctaves, uint8_t octave){
  vector<uint8_t> newScale;
  for(int8_t i = octave; i<numOctaves+octave; i++){
    vector<uint8_t> temp = genScale(scale,root%12+(i*12));
    for(uint8_t j = 0; j<temp.size(); j++){
      newScale.push_back(temp[j]);
    }
  }
  return newScale;
}
//make sure 'root' isn't passed to this fn as a negative number
vector<uint8_t> genScale(ScaleName scale, uint8_t root){
  vector<uint8_t> newScale;
  switch(scale){
    case MAJOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case DORIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case PHRYGIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+6),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case MIXOLYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case AEOLIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LOCRIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+8),uint8_t(root+10)};
      break;
    case MELODIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case HARMONIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+11)};
    case MAJOR_PENTATONIC:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+7),uint8_t(root+9)};
      break;
    case MINOR_PENTATONIC:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+10)};
      break;
    case BLUE:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+7),uint8_t(root+10)};
      break;
  }
  return newScale;
}

vector<uint8_t> selectKeys(uint8_t startRoot) {
  uint8_t selected = 0;
  bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float animOffset = 0;
  bool done = false;
  uint8_t root = startRoot%12;
  //FUCK
  // const ScaleName scaleOptions[12] =  { MAJOR , HARMONIC_MINOR , MELODIC_MINOR , MAJOR_PENTATONIC , MINOR_PENTATONIC , BLUE , DORIAN , PHRYGIAN , LYDIAN , MIXOLYDIAN , AEOLIAN , LOCRIAN };
  ScaleName activeScale = MAJOR;
  bool showingScale = false;
  while (!done) {
    animOffset++;
    controls.readButtons();
    controls.readJoystick();
    if (utils.itsbeen(100)) {
      if (controls.joystickX == -1 && selected < 11) {
        selected++;
        lastTime = millis();
      }
      if (controls.joystickX == 1 && selected > 0) {
        selected--;
        lastTime = millis();
      }
    }
    if (utils.itsbeen(200)) {
      if (controls.SELECT() ) {
        controls.setSELECT(false);
        lastTime = millis();
        keys[selected] = !keys[selected];
      }
      if(controls.NEW()){
          controls.setNEW(false);
        lastTime = millis();
        done = true;
      }
      if(controls.MENU()){
        controls.setMENU(false) ;
        lastTime = millis();
        done = true;
      }
    }
    //changing root
    while(controls.counterA != 0){
      if(controls.counterA >= 1)
        root++;
      if(controls.counterA <= -1){
        if(root == 0)
          root = 11;
        else
          root--;
      }
      root%=12;
      controls.counterA += controls.counterA<0?1:-1;;
    }
    //changing scale
    while(controls.counterB != 0){
      if(!showingScale){
        showingScale = true;
        activeScale = MAJOR;
      }
      else{
        if(controls.counterB >= 1){
          activeScale++;
        }
        if(controls.counterB <= -1){
          activeScale--;
        }
      }
      vector<uint8_t> newScale = genScale(activeScale,root);
      //clear out old scale
      for(uint8_t i = 0; i<12; i++){
        keys[i] = false;
      }
      //putting in newScale
      for(uint8_t i = 0; i<newScale.size(); i++){
        keys[newScale[i]] = true;
      }
      controls.counterB += controls.counterB<0?1:-1;;
    }
    //also changing root and scale, but with joystick
    if(utils.itsbeen(200)){
      if(controls.joystickY != 0){
        if(controls.SHIFT()){
          if(controls.joystickY == 1){
            root++;
            root%=12;
            lastTime = millis();
          }
          if(controls.joystickY == -1){
            if(root == 0)
              root = 11;
            else
              root--;
            lastTime = millis();
          }
        }
        else{
          if(!showingScale){
            showingScale = true;
            activeScale = MAJOR;
            lastTime = millis();
          }
          else{
            if(controls.joystickY == 1){
              activeScale++;
              lastTime = millis();
            }
            if(controls.joystickY == -1){
              if(activeScale)
                activeScale = BLUE;
              else 
              activeScale--;
              lastTime = millis();
            }
          }
          vector<uint8_t> newScale = genScale(activeScale,0);
          //clear out old scale
          for(uint8_t i = 0; i<12; i++){
            keys[i] = false;
          }
          //putting in newScale
          for(uint8_t i = 0; i<(newScale.size()>12?12:newScale.size()); i++){
            keys[newScale[i]] = true;
          }
        }
      }
    }
    //-----------------------------------
    uint8_t xStart = 4;
    uint8_t yStart = 11;
    uint8_t keyHeight = 28;
    uint8_t keyWidth = 15;
    uint8_t offset = 3;
    uint8_t blackKeys = 0;
    uint8_t whiteKeys = 0;
    display.clearDisplay();
    //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
        if (i == selected) {
          display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys)-2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          display.print(getInterval(0, i));
        }
        else if (keys[i]) {
          graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
          display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
          if(i % 12 == 11)
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) - 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          else
            display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 3);
          display.print(getInterval(0, i));
        }
        else {
          display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
        }
        whiteKeys++;
      }
    }
    for (int i = 0; i < 12; i++) {
      if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
        //if it's highlighted
        if (i == selected) {
          display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          display.print(getInterval(0, i));
        }
        //if it's selected
        else if (keys[i]) {
            graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
          display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
          display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight - 2);
          display.print(getInterval(0, i));
        }
        //if it's normal
        else {
          display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
          display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
        }
        blackKeys++;
      }
    }
    if(showingScale){
      printCursive_centered(64, 1, getScaleName(activeScale),SSD1306_WHITE);
    }
    else{
      printCursive_centered(64, 1, "scale",SSD1306_WHITE);
    }
    printCursive(40, screenHeight-7,"root",SSD1306_WHITE);
    display.setCursor(69,screenHeight-7);
    display.print(" "+pitchToString(root,false,true));
    display.display();
  }
    vector<uint8_t> returnedKeys = {};
  for(uint8_t i = 0; i<12; i++){
    if(keys[i]){
      returnedKeys.push_back((i+root)%12);
    }
  }
  return returnedKeys;
}

void selectKeysAnimation(bool in) {
  if(in){
    uint8_t selected = 0;
    bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float animOffset = 0;
    int otherAnimOffset = 24;
    while (otherAnimOffset>0) {
      animOffset++;
      otherAnimOffset-=2;
      //-----------------------------------
      display.clearDisplay();
      uint8_t xStart = 4;
      uint8_t yStart = 11;
      uint8_t keyHeight = 28;
      uint8_t keyWidth = 15;
      uint8_t offset = 3;
      uint8_t blackKeys = 0;
      uint8_t whiteKeys = 0;
      int test = 6;
      //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
      for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));            
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      display.display();
    }
  }
  else{
    uint8_t selected = 0;
    bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float animOffset = 0;
    int otherAnimOffset = 0;
    while (otherAnimOffset<24) {
      animOffset++;
      otherAnimOffset+=2;
      //-----------------------------------
      display.clearDisplay();
      uint8_t xStart = 4;
      uint8_t yStart = 11;
      uint8_t keyHeight = 28;
      uint8_t keyWidth = 15;
      uint8_t offset = 3;
      uint8_t blackKeys = 0;
      uint8_t whiteKeys = 0;
      int test = 6;
      //moves through every key. if it's a whitekey, it uses the whiteKeys variable to step through each white key
      for (int i = 0; i < 12; i++) {
      if (i % 12 == 0 || i % 12 == 2 || i % 12 == 4 || i % 12 == 5 || i % 12 == 7 || i % 12 == 9 || i % 12 == 11) {
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)), keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7);
              display.print(getInterval(0, i));
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + 3 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (whiteKeys) + 2, yStart + 5 + 3 * sin(animOffset / float(10)) + keyHeight + 7 + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          else {
            if(otherAnimOffset*test-i*test<0){
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5, keyWidth, keyHeight - 10, SSD1306_WHITE);
            }
            else
              display.drawRect(xStart + (keyWidth + offset) * (whiteKeys), yStart + 5 + otherAnimOffset*test-i*test, keyWidth, keyHeight - 10, SSD1306_WHITE);
          }
          whiteKeys++;
        }
      }
      for (int i = 0; i < 12; i++) {
        if (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10) {
          //if it's highlighted
          if (i == selected) {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's selected
          else if (keys[i]) {
            if(otherAnimOffset*test-i*test<0){
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)), keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight);
              display.print(getInterval(0, i));            
            }
            else{
              graphics.shadeArea(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 11, 3);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
              display.setCursor(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 2 * sin(animOffset / float(10)) + keyHeight + otherAnimOffset*test-i*test);
              display.print(getInterval(0, i));
            }
          }
          //if it's normal
          else {
            if(otherAnimOffset*test-i*test<0){
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
            else{
              display.fillRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_BLACK);
              display.drawRect(xStart + (keyWidth + offset) * (i - blackKeys) - 7, yStart + 1 + otherAnimOffset*test-i*test, keyWidth - 3, keyHeight - 10, SSD1306_WHITE);
            }
          }
          blackKeys++;
        }
      }
      display.display();
    }
  }
}

vector<NoteID> getSelectedNoteIDs(){
  vector<NoteID> ids = {};
  for(uint16_t i = 0; i<sequence.noteData.size(); i++){
    for(uint16_t j = 1; j<sequence.noteData[i].size(); j++){
      if(sequence.noteData[i][j].isSelected()){
        ids.push_back(NoteID(i,j));
      }
    }
  }
  return ids;
}