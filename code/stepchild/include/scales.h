//Functions and definitions for generating scales
#define MAJOR_SCALE 0
#define DORIAN_MODE 1
#define PHRYGIAN_MODE 2
#define LYDIAN_MODE 3
#define MIXOLYDIAN_MODE_MODE 4
#define AEOLIAN_MODE 5
#define LOCRIAN_MODE 6

#define MELODIC_MINOR_SCALE 7
#define HARMONIC_MINOR_SCALE 8
#define MAJOR_PENTATONIC_SCALE 9
#define MINOR_PENTATONIC_SCALE 10
#define BLUE_SCALE 10


String getScaleName(uint8_t scale){
  const String scales[12] = {"major","dorian","phrygian","lydian","mixolydian","aeolian","locrian","melodic minor","harmonic minor","major pentatonic","minor pentatonoic","blue"};
  return scales[scale];
}

vector<uint8_t> genScale(uint8_t scale, uint8_t root, int8_t numOctaves, uint8_t octave){
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
vector<uint8_t> genScale(uint8_t scale, uint8_t root){
  vector<uint8_t> newScale;
  if(scale == MAJOR_SCALE)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11),uint8_t(root+12)};
  else if(scale == DORIAN_MODE)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == PHRYGIAN_MODE)
    newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == LYDIAN_MODE)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+6),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11),uint8_t(root+12)};
  else if(scale == MIXOLYDIAN_MODE_MODE)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == AEOLIAN_MODE)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == LOCRIAN_MODE)
    newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+8),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == MELODIC_MINOR_SCALE)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == HARMONIC_MINOR_SCALE)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+11),uint8_t(root+12)};
  else if(scale == MAJOR_PENTATONIC_SCALE)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+7),uint8_t(root+9),uint8_t(root+12)};
  else if(scale == MINOR_PENTATONIC_SCALE)
    newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+10),uint8_t(root+12)};
  else if(scale == BLUE_SCALE)
    newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+7),uint8_t(root+10),uint8_t(root+12)};
  
  return newScale;
}


vector<uint8_t> selectKeys(uint8_t startRoot) {
  uint8_t selected = 0;
  bool keys[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float animOffset = 0;
  bool done = false;
  uint8_t root = startRoot%12;
  //FUCK
  const uint8_t scaleOptions[12] =  { MAJOR_SCALE , HARMONIC_MINOR_SCALE , MELODIC_MINOR_SCALE , MAJOR_PENTATONIC_SCALE , MINOR_PENTATONIC_SCALE , BLUE_SCALE , DORIAN_MODE , PHRYGIAN_MODE , LYDIAN_MODE , MIXOLYDIAN_MODE_MODE , AEOLIAN_MODE , LOCRIAN_MODE };
  uint8_t activeScale;
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
      else if(controls.counterA <= -1){
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
        activeScale = 0;
      }
      else{
        if(controls.counterB >= 1){
          activeScale++;
        }
        else if(controls.counterB <= -1){
          if(activeScale == 0)
            activeScale = 11;
          else activeScale--;
        }
        activeScale %= 12;
      }
      vector<uint8_t> newScale = genScale(scaleOptions[activeScale],0);
      //clear out old scale
      for(int i = 0; i<12; i++){
        keys[i] = false;
      }
      //putting in newScale
      for(int i = 0; i<newScale.size(); i++){
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
          else if(controls.joystickY == -1){
            if(root == 0)
              root = 11;
            else
              root--;
            root%=12;
            lastTime = millis();
          }
        }
        else{
          if(!showingScale){
            showingScale = true;
            activeScale = 0;
            lastTime = millis();
          }
          else{
            if(controls.joystickY == 1){
              activeScale++;
              lastTime = millis();
            }
            else if(controls.joystickY == -1){
              if(activeScale == 0)
                activeScale = 11;
              else activeScale--;
              lastTime = millis();
            }
            activeScale %= 12;
          }
          vector<uint8_t> newScale = genScale(scaleOptions[activeScale],0);
          //clear out old scale
          for(int i = 0; i<12; i++){
            keys[i] = false;
          }
          //putting in newScale
          for(int i = 0; i<newScale.size(); i++){
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
      printCursive_centered(64, 1, getScaleName(scaleOptions[activeScale]),SSD1306_WHITE);
    }
    else{
      printCursive_centered(64, 1, "scale",SSD1306_WHITE);
    }
    printCursive(40, screenHeight-7,"root",SSD1306_WHITE);
    display.setCursor(69,screenHeight-7);
    display.print(" "+pitchToString(root,false,true));
    display.display();
  }
  vector<uint8_t> returnedKeys;
  for(int i = 0; i<12; i++){
    if(keys[i]){
      returnedKeys.push_back(i+root);
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
