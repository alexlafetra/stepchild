/*

  Contains a lot of commonly reused miscellaneous functions for grabbing notes from the sequence or performing 
  stepchild-related tasks. The reason these aren't in "utils.h" is that these need access to the sequence object

*/

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
    settings.shrinkTopDisplay = false;
    drawSeq(settings);
    if(!sequence.selectionCount){
      printSmall(trackDisplay,0,"select notes to "+text,1);
    }
    else{
      graphics.drawButton(trackDisplay,0,"n",1);
      printSmall(trackDisplay+9,1,"to "+text+" "+stringify(sequence.selectionCount)+(sequence.selectionCount == 1?" note":" notes"),1);
    }
    iconFunction(7,1,14,true);
    display.display();
  }
}

void defaultIconFunction(uint8_t x, uint8_t y, uint8_t w, bool s){return;}
bool selectNotes(String t){
  return selectNotes(t,defaultIconFunction);
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

vector<uint8_t> selectKeys(uint8_t startRoot) {
  uint8_t selected = 0;
  bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float animOffset = 0;
  bool done = false;
  uint8_t root = startRoot%12;
  //FUCK
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
        if(controls.counterB > 0){
          activeScale++;
        }
        if(controls.counterB < 0){
          activeScale--;
        }
      }
      vector<uint8_t> newScale = makePitchListFromScale(activeScale,0);
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
              activeScale--;
              lastTime = millis();
            }
          }
          vector<uint8_t> newScale = makePitchListFromScale(activeScale,0);
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

void drawCoordinateBox(CoordinatePair coords, SequenceRenderSettings& settings){
  if(!selBox.begun && (coords.start.x != coords.end.x)){
    //correcting bounds for view
    unsigned short int X1;
    unsigned short int X2;
    unsigned short int Y1;
    unsigned short int Y2;

    if(coords.start.x>coords.end.x){
      X1 = coords.end.x;
      X2 = coords.start.x;
    }
    else{
      X1 = coords.start.x;
      X2 = coords.end.x;
    }
    if(coords.start.y>coords.end.y){
      Y1 = coords.end.y;
      Y2 = coords.start.y;
    }
    else{
      Y1 = coords.start.y;
      Y2 = coords.end.y;
    }

    //if it's offscreen, return
    if(X2<=sequence.viewStart || X1>=sequence.viewEnd || Y1 > sequence.startTrack+sequence.endTrack || Y2<sequence.startTrack){
      return;
    }

    if(X1<sequence.viewStart){
      X1 = sequence.viewStart;
    }
    if(X2>sequence.viewEnd){
      X2 = sequence.viewEnd;
    }
    if(Y1<sequence.startTrack){
      Y1 = sequence.startTrack;
    }
    if(Y2>(sequence.startTrack+sequence.maxTracksShown)){
      Y2 = sequence.startTrack+sequence.maxTracksShown;
    }
    uint8_t startX = trackDisplay+(X1-sequence.viewStart)*sequence.viewScale;
    uint8_t length = (X2-X1)*sequence.viewScale;
    uint8_t startHeight = sequence.shrinkTopDisplay?8:headerHeight;
    uint8_t startY = (Y1-sequence.startTrack)*trackHeight+startHeight;
    uint8_t height = ((Y2 - sequence.startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
   
   if((millis())%400>200){
      graphics.shadeRect(startX,startY,length,height,3);
    }
    else{
      display.drawRect(startX,startY,length,height,1);
    }
  }
}

bool compareTracks_NoteID(NoteID n1, NoteID n2){
    return n1.track>n2.track;
}
bool comparePitches_NoteID(NoteID n1, NoteID n2){
    return n1.getPitch()>n2.getPitch();
}
bool compareTracks_NoteTrackPair(NoteTrackPair n1, NoteTrackPair n2){
  return n1.trackID>n2.trackID;
}
bool comparePitches_NoteTrackPair(NoteTrackPair n1, NoteTrackPair n2){
  return n1.getPitch()>n2.getPitch();
}

vector<NoteTrackPair> sortNotes(vector<NoteTrackPair> notes, uint8_t sortBy, uint8_t type){
  vector<NoteTrackPair> sortedVec = notes;
  switch(sortBy){
    //sort by pitch
    case 0:
      sort(sortedVec.begin(), sortedVec.end(),comparePitches_NoteTrackPair);
      break;
    //sort by track
    case 1:
      sort(sortedVec.begin(), sortedVec.end(),compareTracks_NoteTrackPair);
      break;
  }
  //if it's ascending
  if(type == 0){
    return sortedVec;
  }
  //if it's descending, reverse it
  else{
    reverse(sortedVec.begin(),sortedVec.end());
    return sortedVec;
  }
}

//sorts a list of [track,note] pairs
vector<NoteID> sortNotes(vector<NoteID> ids, uint8_t sortBy, uint8_t type){
  vector<NoteID> sortedVec = ids;
  switch(sortBy){
    //sort by pitch
    case 0:
      sort(sortedVec.begin(), sortedVec.end(),comparePitches_NoteID);
      break;
    //sort by track
    case 1:
      sort(sortedVec.begin(), sortedVec.end(),compareTracks_NoteID);
      break;
  }
  //if it's ascending
  if(type == 0){
    return sortedVec;
  }
  //if it's descending, reverse it
  else{
    reverse(sortedVec.begin(),sortedVec.end());
    return sortedVec;
  }
}

vector<NoteID> grabSelectedNotesAsNoteIDs(){
    vector<NoteID> notes;
    for(uint8_t i = 0; i<sequence.noteData.size(); i++){
        for(uint8_t j = 1; j<sequence.noteData[i].size(); j++){
            if(sequence.noteData[i][j].isSelected()){
                NoteID newNote = NoteID(i,j);
                notes.push_back(newNote);
            }
        }
    }
    return notes;
}

/*
    Functions that involve custom GUI layouts for user input/output
*/
void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}

String enterText(String title){
  return enterText(title, 10);
}
String enterText(String title, uint8_t count){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  alphabet = alpha1;
  while(!done){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.SHIFT()){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }
      if(controls.DELETE() && text.length()>0){
        controls.setDELETE(false);
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(controls.SELECT() ){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<count){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        lastTime = millis();
      }
      if(controls.MENU()){
        text = "";
        done = true;
        lastTime = millis();
      }
      if(controls.NEW()){
        lastTime = millis();
        highlight = alphabet.size()-1;
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(controls.joystickX == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(controls.joystickY == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    //title
    display.clearDisplay();
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();

    //text tooltip
    printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      display.print(text+(text.length()<count?alphabet[highlight]:""));
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,14,9,SSD1306_WHITE);
      }
    }
    else
      display.print(text);

    //blank spaces
    for(uint8_t i = text.length(); i<count; i++){
      display.drawFastHLine(10+i*6,22,5,1);
    }
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // display.print(alphabet[count]);
              // display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              display.print(alphabet[count]);
              display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    display.display();
  }
  controls.clearButtons();
  lastTime = millis();
  return text;
}


void binarySelectionBoxPlaceholderDisplayFunction(){}

//this one has a title, but displays a blank screen
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title){
  return binarySelectionBox(x1,y1,op1,op2,title,binarySelectionBoxPlaceholderDisplayFunction);
}

//Binary Selection box w no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
//Binary Selection box w a title, returns -1 for 
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //storing the state
  bool state = false;
  lastTime = millis();
  
  while(true){
    display.clearDisplay();
    drawingFunction();
    graphics.drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      //x to select option
      if(controls.joystickX != 0){
        if(controls.joystickX == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(controls.joystickX == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/del to cancel
      if(controls.MENU() || controls.DELETE()){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(controls.NEW() || controls.SELECT() ){
        lastTime = millis();
          controls.setNEW(false);
        controls.setSELECT(false);
        return state;
      }
    }
  }
  return false;
}
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0 || controls.joystickY != 0){
        if(controls.joystickX == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(controls.joystickX == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(controls.SELECT() ){
        selected = true;
        time = millis();
      }
    }
  }
  controls.setSELECT(false);
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}

uint8_t dropDownMenu(vector<String> options, void (*drawingFunction)()){
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t menuStart = 0;
  uint8_t cursor = 0;
  uint8_t longestOptionLength = 0;
  for(String s:options){
    if(getSmallTextLength(s)>longestOptionLength)
      longestOptionLength = getSmallTextLength(s);
  }
  //each is 9px tall ==> 64/9 = 7
  uint8_t maxNumberOfOptionsShown = options.size()>7?7:options.size();
  while(true){
    display.clearDisplay();
    drawingFunction();
    display.fillRect(x,y,longestOptionLength,maxNumberOfOptionsShown*9,0);
    display.drawRect(x,y,longestOptionLength,maxNumberOfOptionsShown*9,1);
    for(uint8_t i = 0; i<maxNumberOfOptionsShown; i++){
      printSmall(x,y+i*9,options[i+menuStart],1);
      if(i == cursor-menuStart)
        graphics.drawArrow(x,y+i*9+3,3,ARROW_RIGHT,false);
    }
    display.display();

    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.DOWN() && cursor < options.size()){
        cursor++;
        if(cursor-menuStart>7)
          menuStart++;
        lastTime = millis();
      }
      if(controls.UP() && cursor > 0){
        cursor--;
        while(cursor<menuStart)
          menuStart--;
        lastTime = millis();
      }
    }
  }
  return cursor;
}

