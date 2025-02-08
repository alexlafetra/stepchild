bool autotrackEditingControls(uint8_t *interpType, bool* settingRecInput, uint8_t& whichAutotrack);
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput, uint8_t& whichAutotrack);
void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which);
void drawAutotrackEditor(uint8_t y,uint8_t interpType,bool translation, bool settingRecInput, uint8_t whichAutotrack);
void moveAutotrackCursor(int moveAmount);

class AutotrackMenu;
void autotrackTriggerMenu(AutotrackMenu * underlyingMenu);

//moving cursor around while in autotrack mode
//This could probably be combined with "moveCursor"
void moveAutotrackCursor(int moveAmount){
  //if you're trying to move back at the start
  if(sequence.cursorPos==0 && moveAmount < 0){
    return;
  }
  if(moveAmount<0 && abs(moveAmount)>=sequence.cursorPos)
    sequence.cursorPos = 0;
  else
    sequence.cursorPos += moveAmount;
  if(sequence.cursorPos >= sequence.sequenceLength) {
    sequence.cursorPos = sequence.sequenceLength-1;
  }
  if(sequence.cursorPos<0){
    sequence.cursorPos = 0;
  }
  //Move the view along with the cursor
  if(sequence.cursorPos<sequence.viewStart+sequence.subDivision && sequence.viewStart>0){
    sequence.moveView(sequence.cursorPos - (sequence.viewStart+sequence.subDivision));
  }
  else if(sequence.cursorPos > sequence.viewEnd-sequence.subDivision && sequence.viewEnd<sequence.sequenceLength){
    sequence.moveView(sequence.cursorPos - (sequence.viewEnd-sequence.subDivision));
  }
}

void createAutotrack(CurveType type){
  Autotrack newData(type,sequence.sequenceLength);
  sequence.autotrackData.push_back(newData);
}

void createAutotrack(CurveType type, uint8_t cont){
  Autotrack newData(type,sequence.sequenceLength);
  newData.control = cont;
  sequence.autotrackData.push_back(newData);
}

void createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType){
  Autotrack newData(LINEAR_CURVE,sequence.sequenceLength);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  sequence.autotrackData.push_back(newData);
}

void duplicateAutotrack(uint8_t which){
  //if the track exists
  if(which<sequence.autotrackData.size()){
    vector<Autotrack> temp;
    for(uint8_t track = 0; track<sequence.autotrackData.size(); track++){
      temp.push_back(sequence.autotrackData[track]);
      if(track == which)
        temp.push_back(sequence.autotrackData[track]);
    }
    temp.swap(sequence.autotrackData);
  }
}

void deleteAutotrack(uint8_t track){
  //only del tracks if there'll be one left over
  vector<Autotrack> temp;
  for(uint8_t i = 0; i<sequence.autotrackData.size(); i++){
    if(i != track)
      temp.push_back(sequence.autotrackData[i]);
  }
  temp.swap(sequence.autotrackData);
  if(sequence.activeAutotrack>=sequence.autotrackData.size())
    sequence.activeAutotrack = sequence.autotrackData.size()-1;
  if(sequence.autotrackData.size() == 0){
    sequence.activeAutotrack = 0;
  }
}

//draws an animated icon representing the type of interpolation algorithm being used
//0 = linear, 1 = elliptical up, 2 = elliptical down
void drawNodeEditingIcon(uint8_t xPos, uint8_t yPos, uint8_t type, uint8_t frame, bool text){
  const uint8_t width = 19;
  const uint8_t height = 10;
  display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);

  //in this context, type means the kind of interpolation algorithm you're using
  switch(type){
    //linear
    case 0:
      {
      uint8_t p1[2] = {3,(uint8_t)(5+((millis()/200)%2))};
      uint8_t p2[2] = {15,(uint8_t)(5+cos(millis()/100))};
      display.drawLine(xPos+p1[0]+2,yPos+p1[1],xPos+p2[0]-2,yPos+p2[1],SSD1306_WHITE);
      display.drawCircle(xPos+p1[0],yPos+p1[1],1,SSD1306_WHITE);
      display.drawCircle(xPos+p2[0],yPos+p2[1],1,SSD1306_WHITE);
      if(text)
        printSmall(xPos+width/2-6,yPos+height+2,"lin",SSD1306_WHITE);
      break;
      }
    //elliptical UP
    case 1:
      {
      uint8_t lastPoint = 0;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos+sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+1;
        if(point == 0){
          display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        }
        else{
          display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        }
        lastPoint = pt;
      }
      if(text)
        printSmall(xPos+width/2-4,yPos+height+2,"up",SSD1306_WHITE);
      break;
      }
    //elliptical DOWN
    case 2:
      {
      uint8_t lastPoint = 0;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos-sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+height-1;
        if(point == 0)
          display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        else
          display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = pt;
      }
      if(text)
        printSmall(xPos+width/2-8,yPos+height+2,"down",SSD1306_WHITE);
      break;
      }
  }
}

//draws a curve. Frame is to animated it! basically, sets x-offset. Pass a constant to it for no animation.
//Type: 0 = custom, 1 = sinewave, 2 = square wave, 3 = saw, 4 = triangle, , 5 = random
void drawCurveIcon(uint8_t xPos, uint8_t yPos, uint8_t type, uint8_t frame){
  //width
  const uint8_t width = 19;
  const uint8_t height = 10;
  uint8_t lastPoint = 0;
  display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  switch(type){
    //custom curve (default)
    case LINEAR_CURVE:
      drawNodeEditingIcon(xPos,yPos,0,frame,false);
      break;
    //sin curve
    case SINEWAVE_CURVE:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      //drawing each sinPoint
      for(uint8_t point = 0; point<width; point++){
        if(point == 0){
          display.drawPixel(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),SSD1306_WHITE);
        }
        else
          display.drawLine(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = yPos+height/2+(height/2-3)*sin(PI*(point+frame)/(width/2));
      }
      break;
    //square
    case SQUAREWAVE_CURVE:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      for(uint8_t point = 0; point<width; point++){
        //if it's less than, it's low
        if((point+frame)%width<width/2)
          display.drawPixel(point+xPos,yPos+height-3,SSD1306_WHITE);
        //if it's greater than half a period, it's high
        else if((point+frame)%width>width/2 && (point+frame)%width<width-1)
          display.drawPixel(point+xPos,yPos+2,SSD1306_WHITE);
        //if it's equal to half a period, then it's a vert line
        else
          display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
      }
      break;
    //saw
    case SAWTOOTH_CURVE:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        for(uint8_t point = 0; point<width; point++){
          uint8_t pt = (point+frame)%int(width/3);
          if(pt == 0)
            display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
          else
            display.drawPixel(point+xPos,yPos+pt+2,SSD1306_WHITE);
        }
      }
      break;
    //triangle
    case TRIANGLE_CURVE:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        uint8_t pt = 0;
        for(uint8_t point = 0; point<width; point++){
          if((point+frame)%(width/2)>=width/4)
            pt = -(point+frame)%(width/2)+2*height/2;
          else
            pt = (point+frame)%(width/2)+3;
          display.drawPixel(xPos+point,yPos+pt+height/8,SSD1306_WHITE);
        }
      }
      break;
    //random
    case RANDOM_CURVE:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        printSmall(xPos+4,yPos+3+((millis()/200)%2),"R",SSD1306_WHITE);
        printSmall(xPos+8,yPos+3+sin(millis()/100+1),"N",SSD1306_WHITE);
        printSmall(xPos+12,yPos+3+sin(millis()/100+2),"D",SSD1306_WHITE);
      }
      break;
    case NOISE_CURVE:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      for(uint8_t i = xPos; i<xPos+width;i++){
        display.drawPixel(i,random(yPos,yPos+height),1);
      }
      break;
  }
}

//for editing a single Autotrack
void autotrackEditor(uint8_t whichAutotrack){
  //type of curve to draw! 0 is linear, 1 is x^2
  uint8_t interpolatorType = 0;
  bool translation = false;
  //flag that gets set while "copy" is held, so the controls
  //know to listen for x, y, A, and B 
  bool settingRecInput = false;
  bool recInputSet = false;

  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(settingRecInput){   
      //once copy press is released, after at least one param has been selected
      if(!controls.COPY()){
        recInputSet = false;
        settingRecInput = false;
      }   
      //if any of these controls are pressed, then set the current data track param to it
      if(controls.counterA != 0){
        controls.counterA = 0;
        sequence.autotrackData[whichAutotrack].recordFrom = 1;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.counterB != 0){
        controls.counterB = 0;
        sequence.autotrackData[whichAutotrack].recordFrom = 2;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickX != 0){
        sequence.autotrackData[whichAutotrack].recordFrom = 3;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickY != 0){
        sequence.autotrackData[whichAutotrack].recordFrom = 4;
        recInputSet = true;
        lastTime = millis();
      }
    }
    else{
      //this is done just to pass the showingInfo param 
      if(sequence.autotrackData[whichAutotrack].type == LINEAR_CURVE){
        if(!autotrackEditingControls(&interpolatorType,&settingRecInput,whichAutotrack)){
          return;
        }
      }
      else{
        if(!autotrackCurveEditingControls(&translation,&settingRecInput,whichAutotrack)){
          return;
        }
      }
      if(controls.SHIFT()){
        MIDI.sendCC(sequence.autotrackData[whichAutotrack].control,127,sequence.autotrackData[whichAutotrack].channel);      
      }
    }
    display.clearDisplay();
    drawAutotrackEditor(0,interpolatorType,translation,settingRecInput,whichAutotrack);
    display.display();
  }
}

bool autotrackEditingControls(uint8_t *interpType, bool *settingRecInput, uint8_t& whichAutotrack){
  //zoom/subdiv zoom
  while(controls.counterA != 0 && !recordingToAutotrack){
    //changing zoom
    if(!controls.SHIFT()){
      if(controls.counterA >= 1){
          sequence.zoom(true);
        }
      if(controls.counterA <= -1){
        sequence.zoom(false);
      }
    }
    else{
      if(controls.counterA >= 1 && sequence.autotrackData[whichAutotrack].channel<16){
        sequence.autotrackData[whichAutotrack].channel++;
      }
      else if(controls.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(sequence.autotrackData[whichAutotrack].parameterType == 2 && sequence.autotrackData[whichAutotrack].channel>0)
          sequence.autotrackData[whichAutotrack].channel--;
        else if(sequence.autotrackData[whichAutotrack].channel>1)
          sequence.autotrackData[whichAutotrack].channel--;
      }
    }
    controls.countDownA();
  }
  while(controls.counterB != 0 && !recordingToAutotrack){
    if(!controls.SHIFT()){   
      if(controls.counterB >= 1){
        sequence.changeSubDivInt(true);
      }
      //changing subdivint
      if(controls.counterB <= -1){
        sequence.changeSubDivInt(false);
      }
    }
    else{
      if(controls.counterB >= 1){
        sequence.autotrackData[whichAutotrack].control = moveToNextCCParam(sequence.autotrackData[whichAutotrack].control,true,sequence.autotrackData[whichAutotrack].parameterType);
      }
      else if(controls.counterB <= -1){
        sequence.autotrackData[whichAutotrack].control = moveToNextCCParam(sequence.autotrackData[whichAutotrack].control,false,sequence.autotrackData[whichAutotrack].parameterType);
      }
    }
    controls.countDownB();
  }

  if(!recordingToAutotrack || sequence.autotrackData[whichAutotrack].recordFrom != 3){
    if (utils.itsbeen(50)) {
      //moving
      if (controls.LEFT() && controls.SHIFT()) {
        moveAutotrackCursor(-1);
        lastTime = millis();
      }
      if (controls.RIGHT() && controls.SHIFT()) {
        moveAutotrackCursor(1);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.LEFT() && !controls.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(sequence.cursorPos%sequence.subDivision){
          moveAutotrackCursor(-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          moveAutotrackCursor(-sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.RIGHT() && !controls.SHIFT()) {
        if(sequence.cursorPos%sequence.subDivision){
          moveAutotrackCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          moveAutotrackCursor(sequence.subDivision);
          lastTime = millis();
        }
      }
    }
  }
  if(!recordingToAutotrack || sequence.autotrackData[whichAutotrack].recordFrom != 4){
    if(utils.itsbeen(50)){
      if(controls.DOWN()){
        if(controls.SHIFT()){
          sequence.autotrackData[whichAutotrack].changeDataPoint(sequence.cursorPos,1);
          lastTime = millis();
        }
        else{
          sequence.autotrackData[whichAutotrack].changeDataPoint(sequence.cursorPos,8);
          lastTime = millis();
        }
      }
      if(controls.UP()){
        if(controls.SHIFT()){
          sequence.autotrackData[whichAutotrack].changeDataPoint(sequence.cursorPos,-1);
          lastTime = millis();
        }
        else{
          sequence.autotrackData[whichAutotrack].changeDataPoint(sequence.cursorPos,-8);
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    //controls.DELETE()eting data point(s)
    if(controls.DELETE()){
      lastTime = millis();
      //controls.DELETE()eting all selected points
      if(controls.SHIFT()){
        while(sequence.autotrackData[whichAutotrack].selectedPoints.size()>0){
          sequence.autotrackData[whichAutotrack].deleteDataPoint(sequence.autotrackData[whichAutotrack].selectedPoints[0]);
        }
      }
      else
        sequence.autotrackData[whichAutotrack].deleteDataPoint(sequence.cursorPos);
    }
    if(controls.PLAY()){
      if(controls.SHIFT() || sequence.recording()){
        lastTime = millis();
        recordingToAutotrack = true;
        recentCC.val = 0;
        controls.counterA = 64;
        controls.counterB = 64;
        toggleRecordingMode(true);
      }
      else{
        togglePlayMode();
        lastTime = millis();
      }
    }
    if(controls.NEW()){
      //set data track type back to 0
      if(controls.SHIFT()){
        sequence.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        lastTime = millis();
      }
      //change type of autotrack
      else{
        sequence.autotrackData[whichAutotrack].type++;
        if(sequence.autotrackData[whichAutotrack].type>NOISE_CURVE)
          sequence.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
    }
    if(controls.LOOP()){
      if(controls.SHIFT()){
        (*interpType)++;
        if((*interpType)>2)
          *interpType = 0;
        lastTime = millis();
      }
      else{
        lastTime = millis();
        sequence.autotrackData[whichAutotrack].smoothSelectedPoints(*interpType);
      }
    }
    if(controls.SELECT() ){
      //deselect all data points
      if(controls.SHIFT()){
        vector<uint16_t> temp;
        sequence.autotrackData[whichAutotrack].selectedPoints.swap(temp);
      }
      //select a data point
      else
        sequence.autotrackData[whichAutotrack].toggleSelectDataPoint(sequence.cursorPos);
      lastTime = millis();
    }
    if(controls.A()){
      lastTime = millis();
      uint8_t param = selectCCParam_autotrack(sequence.autotrackData[whichAutotrack].parameterType);
      if(param != 255)
        sequence.autotrackData[whichAutotrack].control = param;
    }
    if(controls.B()){
      lastTime = millis();
      sequence.toggleTriplets();
    }
    if(controls.MENU()){
      lastTime = millis();
      controls.setMENU(false) ;
      return false;
    }
    if(controls.COPY()){
      *settingRecInput = true;
      //by default, set it to midi input
      sequence.autotrackData[whichAutotrack].recordFrom = 0;
    }
    else{
      *settingRecInput = false;
    }
  }
  return true;
}

//x is change period, y is change amplitude, controls.SHIFT() x is change phase, controls.SHIFT() y is vertically translate
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput, uint8_t& whichAutotrack){
  //zoom/subdiv zoom (this is the same to the normal DT editor)
  while(controls.counterA != 0){
    //changing zoom
    if(!controls.SHIFT()){
      if(controls.counterA >= 1){
          sequence.zoom(true);
        }
      if(controls.counterA <= -1){
        sequence.zoom(false);
      }
    }
    else{
      if(controls.counterA >= 1 && sequence.autotrackData[whichAutotrack].channel<16){
        sequence.autotrackData[whichAutotrack].channel++;
      }
      else if(controls.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(sequence.autotrackData[whichAutotrack].parameterType == 2 && sequence.autotrackData[whichAutotrack].channel>0)
          sequence.autotrackData[whichAutotrack].channel--;
        else if(sequence.autotrackData[whichAutotrack].channel>1)
          sequence.autotrackData[whichAutotrack].channel--;
      }
    }
    controls.counterA += controls.counterA<0?1:-1;
  }
  while(controls.counterB != 0){
    if(!controls.SHIFT()){   
      if(controls.counterB >= 1 && !controls.SHIFT()){
        sequence.changeSubDivInt(true);
      }
      //changing subdivint
      if(controls.counterB <= -1 && !controls.SHIFT()){
        sequence.changeSubDivInt(false);
      }
    }
    else{
      if(controls.counterB >= 1){
        sequence.autotrackData[whichAutotrack].control = moveToNextCCParam(sequence.autotrackData[whichAutotrack].control,true,sequence.autotrackData[whichAutotrack].parameterType);
      }
      else if(controls.counterB <= -1){
        sequence.autotrackData[whichAutotrack].control = moveToNextCCParam(sequence.autotrackData[whichAutotrack].control,false,sequence.autotrackData[whichAutotrack].parameterType);
      }
    }
    controls.counterB += controls.counterB<0?1:-1;
  }
  if(utils.itsbeen(200)){
    //toggling DT type
    if(controls.NEW()){
      if(controls.SHIFT()){
        lastTime = millis();
        sequence.autotrackData[whichAutotrack].type = LINEAR_CURVE;
      }
      else{
        lastTime = millis();
        sequence.autotrackData[whichAutotrack].type++;
        if(sequence.autotrackData[whichAutotrack].type > NOISE_CURVE)
          sequence.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        sequence.autotrackData[whichAutotrack].regenCurve();
      }
    }
    if(controls.PLAY()){
      togglePlayMode();
      lastTime = millis();
    }
    if(controls.MENU()){
      lastTime = millis();
      controls.setMENU(false) ;
      return false;
    }
    if(controls.A()){
      lastTime = millis();
      uint8_t param = selectCCParam_autotrack(sequence.autotrackData[whichAutotrack].parameterType);
      if(param != 255)
        sequence.autotrackData[whichAutotrack].control = param;
    }
    if(controls.B()){
      lastTime = millis();
      sequence.toggleTriplets();
    }
    //swapping modes
    if(controls.LOOP()){
      (*translation) = !(*translation);
      lastTime = millis();
    }
    //Toggling to 'listen for controls' mode
    if(controls.COPY()){
      *settingRecInput = true;
    }
    else{
      *settingRecInput = false;
    }
  }
  //translations (and changing Amp)
  //translation
  if(*translation){
    //moving by 1
    if(controls.SHIFT() && utils.itsbeen(25)){
      if(controls.LEFT()){
        if(sequence.autotrackData[whichAutotrack].phase == 0)
          sequence.autotrackData[whichAutotrack].phase = sequence.autotrackData[whichAutotrack].period;
        sequence.autotrackData[whichAutotrack].phase--;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
      else if(controls.RIGHT()){
        sequence.autotrackData[whichAutotrack].phase++;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
      //if the phase = period, then phase resets to 0
      if(sequence.autotrackData[whichAutotrack].phase == sequence.autotrackData[whichAutotrack].period)
        sequence.autotrackData[whichAutotrack].phase = 0;
    }
    //moving by subdivInt
    //phase will always be mod by period (so it can never be more/less than per)
    //storing it in "temp" here so it doesn't underflow/overflow
    else if(!controls.SHIFT() && utils.itsbeen(100)){
      if(controls.LEFT()){
        int16_t temp = sequence.autotrackData[whichAutotrack].phase;
        if(temp<sequence.subDivision)
          temp = abs(sequence.autotrackData[whichAutotrack].period-sequence.subDivision);
        else
          temp-=sequence.subDivision;
        sequence.autotrackData[whichAutotrack].phase=temp%sequence.autotrackData[whichAutotrack].period;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
      else if(controls.RIGHT()){
        sequence.autotrackData[whichAutotrack].phase = (sequence.autotrackData[whichAutotrack].phase+sequence.subDivision)%sequence.autotrackData[whichAutotrack].period;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
    }
    //changing vertical translation
    if(utils.itsbeen(25)){
      if(controls.UP() && sequence.autotrackData[whichAutotrack].yPos<127){
        sequence.autotrackData[whichAutotrack].yPos++;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
      else if(controls.DOWN() && sequence.autotrackData[whichAutotrack].yPos>0){
        sequence.autotrackData[whichAutotrack].yPos--;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
    }
  }
  //transformation
  else{
    //changing amplitude
    if(controls.joystickY != 0 && utils.itsbeen(25)){
      if(controls.joystickY == -1 && sequence.autotrackData[whichAutotrack].amplitude<127){
        sequence.autotrackData[whichAutotrack].amplitude++;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
      else if(controls.joystickY == 1 && sequence.autotrackData[whichAutotrack].amplitude>-127){
        sequence.autotrackData[whichAutotrack].amplitude--;
        sequence.autotrackData[whichAutotrack].regenCurve();
        lastTime = millis();
      }
    }
    //changing by 1
    if(controls.SHIFT() && utils.itsbeen(25)){
      //changing period
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          sequence.autotrackData[whichAutotrack].period++;
          sequence.autotrackData[whichAutotrack].regenCurve();
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && sequence.autotrackData[whichAutotrack].period>1){
          sequence.autotrackData[whichAutotrack].period--;
          sequence.autotrackData[whichAutotrack].regenCurve();
          lastTime = millis();
        }
      }
    }
    else if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          if(sequence.autotrackData[whichAutotrack].period == 1)
            sequence.autotrackData[whichAutotrack].period = 0;
          sequence.autotrackData[whichAutotrack].period+=sequence.subDivision;
          sequence.autotrackData[whichAutotrack].regenCurve();
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && sequence.autotrackData[whichAutotrack].period>sequence.subDivision){
          sequence.autotrackData[whichAutotrack].period-=sequence.subDivision;
          sequence.autotrackData[whichAutotrack].regenCurve();
          lastTime = millis();
        }
      }
    }
  }
  return true;
}

void drawAutotrackInputIcon(uint8_t x1, uint8_t y1, uint8_t id){
  switch(sequence.autotrackData[id].recordFrom){
    //external midi
    case 0:
      display.drawBitmap(x1,y1,tiny_midi_bmp,7,7,1);
      break;
    //encoder A
    case 1:
      printItalic(x1,y1,'A',1);
      break;
    //encoder B
    case 2:
      printItalic(x1,y1,'B',1);
      break;
    //joy X
    case 3:
      printItalic(x1,y1,'X',1);
      break;
    //joy Y
    case 4:
      printItalic(x1,y1,'Y',1);
      break;
  }
}

void drawAutotrackEditor(uint8_t y,uint8_t interpType,bool translation, bool settingRecInput, uint8_t whichAutotrack){
  //bounding box and 3D-effect
  display.drawRect(32,y,112,64,SSD1306_WHITE);
  display.drawLine(32,y,29,y+3,SSD1306_WHITE);
  display.drawLine(29,y+3,29,64,SSD1306_WHITE);

  //drawing all the points
  if(sequence.autotrackData.size()>0){
    for(uint16_t step = sequence.viewStart; step<sequence.viewEnd; step++){
      if(step<sequence.sequenceLength){
        //measure bars
        if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>2) {
          graphics.drawDottedLineV((step-sequence.viewStart)*sequence.viewScale+32,0,64,4);
        }
        if(!(step%96)){
          graphics.drawDottedLineV2((step-sequence.viewStart)*sequence.viewScale+32,0,64,6);
        }
        //gets the on-screen position of each point
        uint8_t yPos;
        //if it's a 255, display the last val
        if(sequence.autotrackData[whichAutotrack].data[step] == 255){
          yPos = (64-float(sequence.autotrackData[whichAutotrack].getLastDTVal(step))/float(127)*64);
        }
        else{
          yPos = (64-float(sequence.autotrackData[whichAutotrack].data[step])/float(127)*64);
        }
        if(step == sequence.viewStart){
          display.drawPixel(32+(step-sequence.viewStart)*sequence.viewScale,(yPos),SSD1306_WHITE);
        }
        else{
          display.drawLine(32+(step-sequence.viewStart)*sequence.viewScale,(yPos),32+(step-1-sequence.viewStart)*sequence.viewScale,(64-float(sequence.autotrackData[whichAutotrack].getLastDTVal(step-1))/float(127)*64),SSD1306_WHITE);
        }

        //drawing cursor info if in default editor mode
        if(sequence.autotrackData[whichAutotrack].type == LINEAR_CURVE){
          //drawing selection arrow
          if(sequence.autotrackData[whichAutotrack].isDataPointSelected(step)){
            display.fillCircle(32+(step-sequence.viewStart)*sequence.viewScale,yPos,2,SSD1306_WHITE);
            if(sequence.autotrackData[whichAutotrack].data[step]>=64)//bottom arrow if data point is high
              graphics.drawArrow(32+(step-sequence.viewStart)*sequence.viewScale,yPos+4+((millis()/200)%2),2,2,true);
            else//top arrow if data point is low
              graphics.drawArrow(32+(step-sequence.viewStart)*sequence.viewScale,yPos-6+((millis()/200)%2),2,3 ,true);
          }
          //drawing cursor
          if(step == sequence.cursorPos){
            display.drawFastVLine(32+(sequence.cursorPos-sequence.viewStart)*sequence.viewScale,0,64,SSD1306_WHITE);
            display.drawFastVLine(32+(sequence.cursorPos-sequence.viewStart)*sequence.viewScale+1,0,64,SSD1306_WHITE);
            display.drawCircle(32+(sequence.cursorPos-sequence.viewStart)*sequence.viewScale,yPos,3+((millis()/400)%2),SSD1306_WHITE);
          }
        }

        //playhead
        if(sequence.playing() && ((sequence.autotrackData[whichAutotrack].triggerSource == GLOBAL_TRIGGER && step == sequence.playheadPos) || (sequence.autotrackData[whichAutotrack].triggerSource != GLOBAL_TRIGGER && step == sequence.autotrackData[whichAutotrack].playheadPos)))
          display.drawRoundRect(32+(step-sequence.viewStart)*sequence.viewScale,0,3,screenHeight,3,SSD1306_WHITE);
        
        //rechead
        if(sequence.recording() && sequence.recheadPos == step)
          display.drawRoundRect(32+(step-sequence.viewStart)*sequence.viewScale,0,3,screenHeight,3,SSD1306_WHITE);

        //loop points
        if(step == sequence.loopData[sequence.activeLoop].start){
          graphics.drawArrow(32+(step-sequence.viewStart)*sequence.viewScale-1+((millis()/200)%2),59,4,0,true);
        }
        else if(step == sequence.loopData[sequence.activeLoop].end){
          graphics.drawArrow(32+(step-sequence.viewStart)*sequence.viewScale+2-((millis()/200)%2),59,4,1,false);
        }
      }
    }
    //middle line
    //if it's just the typical node curve, draw midline at middle of the screen (32)
    if(sequence.autotrackData[whichAutotrack].type == LINEAR_CURVE)
      graphics.drawDottedLineH(32,128,32,3);
    else{
      graphics.drawDottedLineH(32,128,64-(sequence.autotrackData[whichAutotrack].yPos-32),3);
    }
    //drawing curve icon
    if(sequence.autotrackData[whichAutotrack].type == LINEAR_CURVE)
      drawNodeEditingIcon(12,14,interpType,millis()/50,true);
    else
      drawCurveIcon(12,14,sequence.autotrackData[whichAutotrack].type,millis()/50);
    animOffset++;
    animOffset%=18;

    //menu info
    if(controls.SHIFT()){
      printParam(16,2,sequence.autotrackData[whichAutotrack].control,true,sequence.autotrackData[whichAutotrack].parameterType,true);
      graphics.printChannel(16,12,sequence.autotrackData[whichAutotrack].channel,true);
      display.drawChar(3,2+sin(millis()/50),0x0E,1,0,1);
    }
    else{
      //title
      if(!sequence.playing()){
        printSmall(0,0,"trk",SSD1306_WHITE);
        printSmall(6-stringify(whichAutotrack+1).length()*2,7,stringify(whichAutotrack+1),SSD1306_WHITE);
      }
      printSmall(15,0,"CC"+stringify(sequence.autotrackData[whichAutotrack].control),SSD1306_WHITE);
      printSmall(15,7,"CH"+stringify(sequence.autotrackData[whichAutotrack].channel),SSD1306_WHITE);
    }
    //drawing curve info, for non-defaults
    if(sequence.autotrackData[whichAutotrack].type != 0){
      if(translation){
        printSmall(12,26,"@:"+stringify(sequence.autotrackData[whichAutotrack].phase),SSD1306_WHITE);
        printSmall(12,33,"Y:"+stringify(sequence.autotrackData[whichAutotrack].yPos),SSD1306_WHITE);
      }
      else{
        printSmall(12,26,"A:"+stringify(sequence.autotrackData[whichAutotrack].amplitude),SSD1306_WHITE);
        printSmall(12,33,"P:"+stringify(sequence.autotrackData[whichAutotrack].period),SSD1306_WHITE);
      }
    }
    
    //play icon
    if(sequence.playing())
      display.fillTriangle(120+((millis()/200)%2),9,120+((millis()/200)%2),3,120+6+((millis()/200)%2),6,SSD1306_WHITE);
    if(sequence.recording()){
      //flash it while waiting
      if(waitingToReceiveANote && (millis()%1000>500))
        display.drawCircle(124,6,3,SSD1306_WHITE);
      //draw it solid if the rec has started
      else if(!waitingToReceiveANote)
        display.fillCircle(124,6,3,SSD1306_WHITE);
    }
    
    //drawing the bargraph
    uint8_t barHeight;
    if(!sequence.playing())
      barHeight = 50;
    else
     barHeight = 64;
    display.drawRect(0,screenHeight-barHeight,11,barHeight,SSD1306_WHITE);
    uint8_t height;
    uint8_t val;
    if(sequence.playing()){
      if(sequence.autotrackData[whichAutotrack].data[sequence.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?sequence.playheadPos:sequence.autotrackData[whichAutotrack].playheadPos] == 255)
        val = sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?sequence.playheadPos:sequence.autotrackData[whichAutotrack].playheadPos);
      else
        val = sequence.autotrackData[whichAutotrack].data[sequence.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?sequence.playheadPos:sequence.autotrackData[whichAutotrack].playheadPos];
    }
    else{
      if(sequence.autotrackData[whichAutotrack].data[sequence.cursorPos] == 255)
        val = sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.cursorPos);
      else
        val = sequence.autotrackData[whichAutotrack].data[sequence.cursorPos];
    }
    //filling the bar graphgraph
    height = abs(float(val)/float(127)*float(barHeight-4));
    if(height>0)
      display.fillRect(2,62-height,7,height,2);//inverted color!
    
    //drawing sent data
    display.setRotation(1);
    if(sequence.playing()){
      printSmall(barHeight/2-stringify(sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?sequence.playheadPos:sequence.autotrackData[whichAutotrack].playheadPos)).length()*2,3,stringify(sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?sequence.playheadPos:sequence.autotrackData[whichAutotrack].playheadPos)),2);
    }
    else{
      printSmall(barHeight/2-stringify(sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.cursorPos)).length()*2,3,stringify(sequence.autotrackData[whichAutotrack].getLastDTVal(sequence.cursorPos)),2);
    }
    display.setRotation(DISPLAY_UPRIGHT);

    //drawing source icon
    printSmall(15,42,"src",1);
    graphics.drawArrow(20,52+sin(millis()/400),2,3,false);
    if(!settingRecInput || (millis()/400)%2)
      drawAutotrackInputIcon(17,55,whichAutotrack);
    if(settingRecInput){
      display.fillRoundRect(36,3,63,17,3,0);
      display.drawRoundRect(36,3,63,17,3,1);
      printSmall(38,6,"touch A,B,X or Y",1);
      printSmall(38,12,"to set rec input",1);
    }
  }
  else{
    printSmall(50,29,"no data, kid",SSD1306_WHITE);
  }
}

void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which){
  if(sequence.autotrackData[which].isActive){
    float yScale = float(height-1)/float(127);
    // float xScale = scale;
    //i starts at start, goes from start to the end of the screen
    // float sc = float(96)/float(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start);
    float sc = float(74)/float(sequence.viewEnd-sequence.viewStart);
    //ends at sequence.viewEnd-1 because it draws lines 2 points at a time
    for(uint16_t i = sequence.viewStart; i<sequence.viewEnd; i++){
      if(i<sequence.autotrackData[which].data.size()){
        if(i >= sequence.viewEnd - 1){
          display.drawLine(x1+(i-sequence.viewStart)*sc-1, y1+yScale*(127-sequence.autotrackData[which].getLastDTVal(i)),x1+(sequence.viewEnd-sequence.viewStart)*sc-1, y1+yScale*(127-sequence.autotrackData[which].getLastDTVal(i)),SSD1306_WHITE);
          break;
        }
        else
          display.drawLine(x1+(i-sequence.viewStart)*sc, y1+yScale*(127-sequence.autotrackData[which].getLastDTVal(i)),x1+(i+1-sequence.viewStart)*sc-1, y1+yScale*(127-sequence.autotrackData[which].getLastDTVal(i+1)),SSD1306_WHITE);
        if(sequence.playing()){
          if((sequence.autotrackData[which].triggerSource == GLOBAL_TRIGGER && i == sequence.playheadPos) || (sequence.autotrackData[which].triggerSource != GLOBAL_TRIGGER && i == sequence.autotrackData[which].playheadPos)){
            display.drawFastVLine(x1+(i-sequence.viewStart)*sc,y1,height,SSD1306_WHITE);
          }
        }
      }
      else
        break;
    }
    graphics.drawDottedLineH(x1,x1+74,y1+height/2,3);
  }
  //if it's muted/inactive
  else{
    graphics.shadeRect(x1,y1,74,height-2,5);
    display.fillRoundRect(x1+20,y1+2,25,7,3,0);
    printSmall(x1+22,y1+3,"[off]",SSD1306_WHITE);
  }
}

void drawRobotSprite(uint8_t x, uint8_t y,uint8_t whichAutotrack){
  //Drawing our robot bud
  if(sequence.playing()){
    uint16_t position = 0;
    //if the autotrack is triggering, then use its internal playhead
    if(sequence.autotrackData[whichAutotrack].triggerSource != GLOBAL_TRIGGER)
      position = sequence.autotrackData[whichAutotrack].playheadPos;
    else
      position = sequence.playheadPos;
    switch((position/24)%4){
      case 0:
        display.drawBitmap(x+1,y+3,robo3_mask,18,17,0);
        display.drawBitmap(x,y,robo3,21,24,1);
        break;
      case 1:
        display.drawBitmap(x+9,y+2,robo1_mask,12,17,0);
        display.drawBitmap(x+3,y,robo4,18,24,1);
        break;
      case 2:
        display.drawBitmap(x+9,y+2,robo5_mask,12,17,0);
        display.drawBitmap(x+9,y,robo5,18,24,1);
        break;
      case 3:
        display.drawBitmap(x+9,y+2,robo5_mask,12,17,0);
        display.drawBitmap(x+9,y,robo6,18,24,1);
        break;
    }
  }
  else{
    display.drawBitmap(x+9,y+2,robo1_mask,12,17,0);
    display.drawBitmap(x+8,y,robo[(millis()/500)%2],13,24,1);
  }
}


/*

In the viewer, selecting an autotrack should give you the options:
edit curve, set trigger, apply as function

edit curve --> normal curve editor

set trigger --> trigger editor, more compact. Should slide in, and doesn't display both track and channel triggers
(instead you should flip between the two) and also has a tooltip describing what it does eg "track will play when a note is sent on "

apply as function --> let's you apply the autotrack to note timing, velocity, or chance


curve editor should show tooltips when shift is held!

*/

class AutotrackMenu:public StepchildMenu{
  public:
    uint8_t menuStart = 0;
    bool popupMenuActive = false;
    uint8_t popupCursor = 0;
    SequenceRenderSettings settings;
    AutotrackMenu(){
      coords = CoordinatePair(0,0,128,64);
      settings.trackLabels = true;
      settings.topLabels = false;
      settings.drawLoopPoints = true;
      settings.shrinkTopDisplay = false;
      settings.drawLoopFlags = false;
    }
    bool autotrackMenuControls(){
      controls.readButtons();
      controls.readJoystick();
      if(utils.itsbeen(200)){
        if(controls.MENU()){
          lastTime = millis();
          if(popupMenuActive){
            popupMenuActive = false;
          }
          else{
            return false;
          }
        }
        if(controls.DELETE() && !popupMenuActive){
          deleteAutotrack(cursor);
          if(cursor>=sequence.autotrackData.size())
            cursor = sequence.autotrackData.size()-1;
          lastTime = millis();
        }
        if(controls.NEW()){
          if(sequence.autotrackData.size() == 0)
            createAutotrack(LINEAR_CURVE);
          else
            createAutotrack(LINEAR_CURVE,(sequence.autotrackData[sequence.autotrackData.size()-1].control>=127)?0:(sequence.autotrackData[sequence.autotrackData.size()-1].control+1));
          cursor = sequence.autotrackData.size()-1;
          if(cursor>menuStart+5)
            menuStart++;
          else if(cursor<menuStart)
            menuStart--;
          lastTime = millis();
        }
        if(controls.PLAY()){
          togglePlayMode();
          lastTime = millis();
        }
        if(controls.SELECT()){
          lastTime = millis();
          if(popupMenuActive){
            switch(popupCursor){
              //cc
              case 0:
                break;
              //ch
              case 1:
                break;
              //prime
              case 2:
                sequence.autotrackData[cursor].isPrimed = !sequence.autotrackData[cursor].isPrimed;
                break;
              //on/off
              case 3:
                sequence.autotrackData[cursor].isActive = !sequence.autotrackData[cursor].isActive;
                break;
              //edit
              case 4:
                autotrackEditor(cursor);
                break;
              //apply
              case 5:
                break;
              //set trigger
              case 6:
                // setAutotrackTrigger(cursor);
                autotrackTriggerMenu(this);
                break;
            }
          }
          else{
            popupMenuActive = true;
          }
        }
        if(popupMenuActive){
          if(controls.UP()){
            if(popupCursor > 2){
              popupCursor+=2;
            }
            else{
              popupCursor = 4;
            }
            lastTime = millis();
          }
          if(controls.DOWN()){
            if(popupCursor > 3){
              popupCursor-=2;
              lastTime = millis();
            }
          }
          if(controls.RIGHT()){
            if(popupCursor == 3 || popupCursor == 5 || popupCursor<2){
              popupCursor++;
              lastTime = millis();
            }
          }
          if(controls.LEFT()){
            if(popupCursor == 0 || popupCursor == 3 || popupCursor == 5){
              popupMenuActive = false;
            }
            else{
              popupCursor--;
            }
            lastTime = millis();
          }
        }
      }
      if(utils.itsbeen(100)){
        if(!popupMenuActive){
          if(controls.UP() && cursor < sequence.autotrackData.size()-1){
            cursor++;
            lastTime = millis();
            if(cursor>(menuStart+5))
              menuStart++;
          }
          if(controls.DOWN() && cursor>0){
            cursor--;
            lastTime = millis();
            if(cursor<menuStart)
              menuStart--;
          }
          if(controls.RIGHT()){
            popupMenuActive = true;
            lastTime = millis();
          }
        }
      }
      if(popupMenuActive){
        if(controls.counterA){
          switch(popupCursor){
            //CC
            case 0:{
              int16_t temp = sequence.autotrackData[cursor].control;
              temp+=controls.counterA<0?-1:1;
              if(temp < 0){
                temp = 0;
                controls.counterA = 0;
              }
              else if(temp>127){
                temp = 127;
                controls.counterA = 0;
              }
              sequence.autotrackData[cursor].control = temp;
            }
              break;
            //ch
            case 1:{
              int16_t temp = sequence.autotrackData[cursor].channel;
              temp+=controls.counterA<0?-1:1;
              if(temp < 0){
                temp = 0;
                controls.counterA = 0;
              }
              else if(temp>15){
                temp = 15;
                controls.counterA = 0;
              }
              sequence.autotrackData[cursor].channel = temp;
            }
              break;
          }
          controls.countDownA();
        }
      }
      return true;
    }
    void displayMenu(bool clearDisplay){
      const uint8_t width = 24;
      const uint8_t height = 7;

      if(clearDisplay)
        display.clearDisplay();
      drawSeq(settings);
      display.fillRect(coords.start.x,coords.start.y+16,32,48-coords.start.y,0);

      //border
      display.fillRoundRect(coords.start.x+3+width+1,coords.start.y+16,80,44,3,0);
      display.drawRoundRect(coords.start.x+3+width+1,coords.start.y+16,80,44,3,1);

      //title
      if(coords.start.x+coords.start.y-1<screenWidth){
        display.setCursor(coords.start.x+coords.start.y+28,8);
        display.setFont(&FreeSerifItalic9pt7b);
        display.print("Automation");
        display.setFont();
      }
      // printCursive(coords.start.x+coords.start.y+25,0,"automation tracks",1);
      drawRobotSprite(coords.start.x+101,coords.start.y+40,cursor);

      //display autotracks that are in the sequence
      if(sequence.autotrackData.size()){
        for(uint8_t i = 0; i<min(6,sequence.autotrackData.size()); i++){
          if(i+menuStart== cursor){
            display.fillRoundRect(coords.start.x+3,coords.start.y+16+i*(height+1),width+4,height,3,1);
            if(!popupMenuActive){
              graphics.drawArrow(coords.start.x+4+(millis()/400)%2,coords.start.y+16+i*(height+1)+height/2,3,ARROW_RIGHT,false);
              graphics.drawArrow(coords.start.x+3+width+4,coords.start.y+16+i*(height+1)+height/2,3,ARROW_RIGHT,true);
            }
          }
          else{
            display.fillRoundRect(coords.start.x+3,coords.start.y+16+i*(height+1),width,height,3,1);
          }
          printSmall(coords.start.x+6,coords.start.y+16+i*(height+1)+1,"#"+stringify(i+menuStart),0);
        }
        if(sequence.autotrackData.size()<6){
          printSmall(coords.start.x+8,coords.start.y+16+sequence.autotrackData.size()*8,"[N]",1);
          printSmall(coords.start.x+12,coords.start.y+21+sequence.autotrackData.size()*8,"+",1);
        }

        //holder for the AT display
        display.drawRoundRect(coords.start.x+30,coords.start.y+18,76,12,3,1);
        drawMiniDT(coords.start.x+31,coords.start.y+19,12,cursor);

        //AT info
        /*
        type, trigger, cc, ch
        */
        //curve icon
        drawCurveIcon(coords.start.x+30,coords.start.y+32,sequence.autotrackData[cursor].type,millis()/50);

        //cc and ch indicators
        display.drawBitmap(coords.start.x+50,coords.start.y+32,cc_tiny,5,3,SSD1306_WHITE);
        printSmall(coords.start.x+56,coords.start.y+32,stringify(sequence.autotrackData[cursor].control),1);
        uint8_t offset = 4*stringify(sequence.autotrackData[cursor].control).length();
        display.drawBitmap(coords.start.x+56+offset,coords.start.y+32,ch_tiny,6,3,SSD1306_WHITE);
        printSmall(coords.start.x+63+offset,coords.start.y+32,stringify(sequence.autotrackData[cursor].channel+1),1);
        offset+=4*stringify(sequence.autotrackData[cursor].channel).length();

        //primed
        graphics.drawButton(coords.start.x+67+offset,coords.start.y+35,sequence.autotrackData[cursor].isPrimed?"primed":"prime",sequence.autotrackData[cursor].isPrimed);
        if(sequence.autotrackData[cursor].isPrimed && (millis()/600)%2){
          display.fillCircle(coords.start.x+offset+66,coords.start.y+34,2,1);
        }
        else{
          display.drawCircle(coords.start.x+offset+66,coords.start.y+34,2,1);
        }
        //buttons
        graphics.drawButton(coords.start.x+30,coords.start.y+43,sequence.autotrackData[cursor].isActive?"on":"off",true);
        graphics.drawButton(coords.start.x+46,coords.start.y+43,"edit curve",true);
        graphics.drawButton(coords.start.x+54,coords.start.y+51,"trigger",true);
        //trigger
        switch(sequence.autotrackData[cursor].triggerSource){
          case GLOBAL_TRIGGER:
            printSmall(coords.start.x+89,coords.start.y+52,"glbl",1);
            break;
          case TRACK_TRIGGER:
            display.drawBitmap(coords.start.x+89,coords.start.y+51,trk_tiny,10,3,1);
            printSmall(coords.start.x+100,coords.start.y+51,stringify(sequence.autotrackData[cursor].triggerTarget),1);
            break;
          case CHANNEL_TRIGGER:
            display.drawBitmap(coords.start.x+89,coords.start.y+51,ch_tiny,6,3,1);
            printSmall(coords.start.x+96,coords.start.y+51,stringify(sequence.autotrackData[cursor].triggerTarget+1),1);
            break;
        }
        graphics.drawButton(coords.start.x+30,coords.start.y+51,"apply",true);

        //popup items
        if(popupMenuActive){
          switch(popupCursor){
            //cc
            case 0:
              graphics.drawArrow(coords.start.x+53,coords.start.y+36+((millis()/300)%2),3,ARROW_UP,false);
              break;
            //ch
            case 1:
              graphics.drawArrow(coords.start.x+60+4*stringify(sequence.autotrackData[cursor].control).length(),coords.start.y+36+((millis()/300)%2),3,ARROW_UP,false);
              break;
            //primed
            case 2:
              graphics.drawArrow(coords.start.x+94,coords.start.y+42+((millis()/300)%2),3,ARROW_UP,false);
              break;
            //on/off
            case 3:
              graphics.drawArrow(coords.start.x+30+((millis()/300)%2),coords.start.y+46,3,ARROW_RIGHT,false);
              break;
            //edit
            case 4:
              graphics.drawArrow(coords.start.x+85+((millis()/300)%2),coords.start.y+46,3,ARROW_LEFT,false);
              break;
            //apply
            case 5:
              graphics.drawArrow(coords.start.x+30+((millis()/300)%2),coords.start.y+54,3,ARROW_RIGHT,false);
              break;
            //set trigger
            case 6:
              graphics.drawArrow(coords.start.x+83+((millis()/300)%2),coords.start.y+54,3,ARROW_LEFT,false);
              break;
          }
        }
      }
      //if there are no autotracks
      else{
        printSmall(coords.start.x+8,coords.start.y+17,"[N]",1);
        printSmall(coords.start.x+12,coords.start.y+22,"+",1);
        // printSmall(coords.start.x+48,coords.start.y+40,"zero tracks!",1);
        printSmall(coords.start.x+33,coords.start.y+35,"no automation data!",1);

      }
      if(clearDisplay)
        display.display();
    }
    void displayMenu(){
      displayMenu(true);
    }
};

void autotrackMenu(){
  AutotrackMenu menu;
  menu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  while(menu.autotrackMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
}

class AutotrackTriggerMenu:public StepchildMenu{
  public:
  AutotrackMenu * underlyingMenu = NULL;
  AutotrackTriggerMenu(){}
  AutotrackTriggerMenu(AutotrackMenu* u){
    underlyingMenu = u;
    coords = CoordinatePair(43,14,128,64);
  }
  bool autotrackTriggerMenuControls(){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.MENU() || controls.SELECT()){
        lastTime = millis();
        return false;
      }
      if(controls.RIGHT()){
        switch(sequence.autotrackData[underlyingMenu->cursor].triggerSource){
          case GLOBAL_TRIGGER:
            sequence.autotrackData[underlyingMenu->cursor].setTrigger(TRACK_TRIGGER,0);
            break;
          case TRACK_TRIGGER:
            sequence.autotrackData[underlyingMenu->cursor].setTrigger(CHANNEL_TRIGGER,0);
            break;
        }
        lastTime = millis();
      }
      if(controls.LEFT()){
        switch(sequence.autotrackData[underlyingMenu->cursor].triggerSource){
          case TRACK_TRIGGER:
            sequence.autotrackData[underlyingMenu->cursor].setTrigger(GLOBAL_TRIGGER,0);
            break;
          case CHANNEL_TRIGGER:
            sequence.autotrackData[underlyingMenu->cursor].setTrigger(TRACK_TRIGGER,0);
            break;
        }
        lastTime = millis();
      }
    }
    //changing the trigger target
    while(controls.counterA){
      if(sequence.autotrackData[underlyingMenu->cursor].triggerSource == GLOBAL_TRIGGER){
        controls.counterA = 0;
        break;
      }
      int16_t temp = sequence.autotrackData[underlyingMenu->cursor].triggerTarget;
      temp+=controls.counterA<0?-1:1;
      switch(sequence.autotrackData[underlyingMenu->cursor].triggerSource){
        case TRACK_TRIGGER:
          if(temp < 0){
            temp = 0;
            controls.counterA = 0;
          }
          else if(temp >= sequence.trackData.size()){
            temp = sequence.trackData.size()-1;
            controls.counterA = 0;
          }
          break;
        case CHANNEL_TRIGGER:
          if(temp < 0){
            temp = 0;
            controls.counterA = 0;
          }
          else if(temp > 15){
            temp = 15;
            controls.counterA = 0;
          }
          break;
      }
      sequence.autotrackData[underlyingMenu->cursor].triggerTarget = temp;
      controls.countDownA();
    }
    //changing the gate behavior
    if(controls.counterB){
      sequence.autotrackData[underlyingMenu->cursor].gated = !sequence.autotrackData[underlyingMenu->cursor].gated;
      controls.counterB = 0;
    }
    return true;
  }
  void displayMenu(){
    display.clearDisplay();
    underlyingMenu->displayMenu(false);
    display.fillRoundRect(coords.start.x,coords.start.y,131-coords.start.x,40,3,0);
    display.drawRoundRect(coords.start.x,coords.start.y,96,40,3,1);

    //slider
    vector<String> options = {"global","track","channel"};
    graphics.drawSlider(coords.start.x+3,coords.start.y+2,options,sequence.autotrackData[underlyingMenu->cursor].triggerSource);
    //arrow+label
    String s = "";
    switch(sequence.autotrackData[underlyingMenu->cursor].triggerSource){
      //global
      case 0:
        s = "any track";
        graphics.drawArrow(coords.start.x+17,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
      //track
      case 1:
        s = "track "+stringify(sequence.autotrackData[underlyingMenu->cursor].triggerTarget);
        graphics.drawArrow(coords.start.x+41,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
      //channel
      case 2:
        s = "channel "+stringify(sequence.autotrackData[underlyingMenu->cursor].triggerTarget+1);
        graphics.drawArrow(coords.start.x+68,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
    }
    printSmall(coords.start.x+2,coords.start.y+17,"triggers when a note is"+s,1);
    printSmall(coords.start.x+2,coords.start.y+23,"sent on "+s,1);
    display.drawFastHLine(coords.start.x+30,coords.start.y+29,getSmallTextLength(s),1);
    graphics.drawInputIcon(coords.start.x+30+getSmallTextLength(s)+3,coords.start.y+24,"A");

    //gate display
    printSmall(coords.start.x+2,coords.start.y+32,"gated:",1);
    graphics.drawButton(coords.start.x+24,coords.start.y+31,sequence.autotrackData[underlyingMenu->cursor].gated?"true":"false",sequence.autotrackData[underlyingMenu->cursor].gated);
    graphics.drawInputIcon(coords.start.x+48,coords.start.y+32,"B");
    display.display();
  }
};

void autotrackTriggerMenu(AutotrackMenu * underlyingMenu){
  AutotrackTriggerMenu menu = AutotrackTriggerMenu(underlyingMenu);
  menu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
  while(menu.autotrackTriggerMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
}