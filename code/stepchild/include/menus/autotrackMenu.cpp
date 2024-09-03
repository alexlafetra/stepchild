bool isDataPointSelected(uint16_t);
bool autotrackEditingControls(uint8_t *interpType, bool* settingRecInput);
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput);
bool autotrackViewerControls();
void drawAutotrackViewer(uint8_t firstTrack);
void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which);
void drawAutotrackEditor(uint8_t y,uint8_t interpType,bool translation, bool settingRecInput);

//these could all be refactored into methods of the autotrack type... jus sayin
//OR you could pass a pointer to the DT as an arg, it would make it easier to use for other people
void regenDT(uint8_t which,uint16_t start, uint16_t end);
void regenDT(uint8_t which);
void randomInterp(uint16_t start, uint16_t end, uint8_t which);
void linearInterpolate(uint16_t start, uint16_t end, uint8_t id);
void ellipticalInterpolate(uint16_t start, uint16_t end, uint8_t id, bool up);
void smoothSelectedPoints(uint8_t type);


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

void createDataPoint(){
  sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] = 64;
}

void changeDataPoint(int8_t amount){
  if(sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] == 255){
    //if it's a blank point, go back until you hit a real point
    for(uint16_t point = sequence.cursorPos; point>=0; point--){
      //if there's a real point
      if(sequence.autotrackData[sequence.activeAutotrack].data[point] != 255){
        sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] = sequence.autotrackData[sequence.activeAutotrack].data[point];
        break;
      }
      //if it never hit a real point, just make one
      if(point == 0){
        sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] = 64;
        break;
      }
    }
    
  }
  int16_t newVal = sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos]+amount;
  if(newVal<0)
    newVal = 0;
  else if(newVal > 127)
    newVal = 127;
  
  sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] = newVal;
}

void createAutotrack(uint8_t type){
  Autotrack newData(type,sequence.sequenceLength);
  newData.setTitle(stringify(sequence.autotrackData.size()));
  sequence.autotrackData.push_back(newData);
}

void createAutotrack(uint8_t type, uint8_t cont){
  Autotrack newData(type,sequence.sequenceLength);
  newData.control = cont;
  newData.setTitle(stringify(sequence.autotrackData.size()));
  sequence.autotrackData.push_back(newData);
}

void createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType){
  Autotrack newData(0,sequence.sequenceLength);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  newData.setTitle(stringify(sequence.autotrackData.size()));
  sequence.autotrackData.push_back(newData);
}

void createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType, String title){
  Autotrack newData(0,sequence.sequenceLength);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  newData.setTitle(title);
  sequence.autotrackData.push_back(newData);
}

void dupeAutotrack(uint8_t which){
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

void selectDataPoint(uint16_t index){
  //if the point is in bounds
  if(index>=0 && index<sequence.sequenceLength){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size();i++){
      if(index == sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]){
        alreadySelected = true;
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]);
      }
    }
    //if it was already selected, swap vectors with temp to del it from the selection
    if(alreadySelected){
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]>index){
          sequence.autotrackData[sequence.activeAutotrack].selectedPoints.insert(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.push_back(index);
    }
    //if it's the first vector
    else if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size() == 0){
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.push_back(index);
    }
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
      uint8_t lastPoint;
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
  display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  switch(type){
    //custom curve (default)
    case 0:
      drawNodeEditingIcon(xPos,yPos,0,frame,false);
      break;
    //sin curve
    case 1:
      display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      uint8_t lastPoint;
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
    case 2:
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
    case 3:
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
    case 4:
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
    case 5:
      {
        display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        printSmall(xPos+4,yPos+3+((millis()/200)%2),"R",SSD1306_WHITE);
        printSmall(xPos+8,yPos+3+sin(millis()/100+1),"N",SSD1306_WHITE);
        printSmall(xPos+12,yPos+3+sin(millis()/100+2),"D",SSD1306_WHITE);
      }
      break;
  }
}

//Sets the target track or channel for a trigger, and the track's gate status
void setAutotrackTrigger(uint8_t whichAT){
  //0 for global, 1 for tracks, 2 for channels
  uint8_t trigSource = sequence.autotrackData[whichAT].triggerSource;
  //track number
  int8_t trackNumber = 0;
  //channel number
  int8_t channelNumber = 1;
  uint8_t trackViewStart = 0;
  uint8_t channelViewStart = 1;

  const uint8_t numberOfTracks = 6;
  const uint8_t x0 = 16;
  const uint8_t x1 = 48;
  const uint8_t x2 = 82;
  const uint8_t colWidth = 30;
  const uint8_t colHeight = 9;
  const uint8_t y1 = 10;

  bool togglingGate = false;

  while(true){
    display.clearDisplay();
    
    //drawing title
    printSmall(0,0,"set autotrack trigger:",1);

    //drawing global label
    (trigSource||togglingGate)?display.drawRect(8,28,27,9,1):display.fillRect(8,28,27,9,1);
    printSmall(10,30,"global",2);
    if(!trigSource && !togglingGate)
      graphics.drawArrow(22,26+((millis()/400)%2),3,3,false);


    //drawing gate label
    graphics.drawBinarySelectionBox(22, 52, "on", "off", "", !sequence.autotrackData[whichAT].gated);
    printSmall_centered(22,40,"gate",1);

    if(togglingGate){
      graphics.drawArrow(22,59+((millis()/400)%2),3,2,false);
    }

    //drawing tracks
    for(uint8_t i = 0; i<=numberOfTracks; i++){
      if(i+trackViewStart>=sequence.trackData.size())
        break;
      if(trigSource == 1 && trackNumber == i+trackViewStart){
        display.fillRect(x1,y1+i*colHeight,colWidth,colHeight,1);
        graphics.drawArrow(x1+colWidth-6+((millis()/400)%2),y1+i*colHeight+colHeight/2,4,1,false);
      }
      else
        display.drawRect(x1,y1+i*colHeight,colWidth,colHeight,1);
      printSmall(x1+2,y1+i*colHeight+2,stringify(i+trackViewStart),2);
    }
    //arrow indicators
    if(trackViewStart+numberOfTracks<sequence.trackData.size()){
      graphics.drawArrow(x1-5,60+((millis()/400)%2),3,3,false);
    }
    if(trackViewStart>0){
      graphics.drawArrow(x1-5,50-((millis()/400)%2),3,2,false);
    }
    //drawing channels
    for(uint8_t i = 0; i<=numberOfTracks; i++){
      if(i+channelViewStart>16)
        break;
      if(trigSource == 2 && channelNumber == i+channelViewStart){
        display.fillRect(x2,y1+i*colHeight,colWidth,colHeight,1);
        graphics.drawArrow(x2+6+((millis()/400)%2),y1+i*colHeight+colHeight/2,4,0,false);
      }
      else
        display.drawRect(x2,y1+i*colHeight,colWidth,colHeight,1);
      printSmall(x2+colWidth-((i+channelViewStart)/10+1)*4-1,y1+i*colHeight+2,stringify(i+channelViewStart),2);
    }
    //arrow indicators
    if(channelViewStart+numberOfTracks<=16){
      graphics.drawArrow(x2+colWidth+5,60+((millis()/400)%2),3,3,false);
    }
    if(channelViewStart>1){
      graphics.drawArrow(x2+colWidth+5,50-((millis()/400)%2),3,2,false);
    }
    display.setRotation(DISPLAY_SIDEWAYS_L);
    printSmall(48-y1,x1-6,"trck",1);
    display.setRotation(DISPLAY_SIDEWAYS_R);
    printSmall(y1,screenWidth-x2-colWidth-6,"chnl",1);
    display.setRotation(DISPLAY_UPRIGHT);

    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.SHIFT() || controls.LOOP()){
        sequence.autotrackData[whichAT].gated = !sequence.autotrackData[whichAT].gated;
        lastTime = millis();
      }
      if(controls.joystickX != 0){
        if(controls.joystickX == -1 && trigSource<2){
          if(togglingGate)
            togglingGate = false;
          lastTime = millis();
          trigSource++;
        }
        else if(controls.joystickX == 1 && trigSource>0){
          lastTime = millis();
          trigSource--;
        }
      }
      if(controls.SELECT() ){
        lastTime = millis();  
        if(togglingGate){
          sequence.autotrackData[whichAT].gated = !sequence.autotrackData[whichAT].gated;
        }
        else{
          switch(trigSource){
            //global
            case 0:
              sequence.autotrackData[whichAT].setTrigger(global,0);
              break;
            //track
            case 1:
              sequence.autotrackData[whichAT].setTrigger(track,trackNumber);
              break;
            //channel
            case 2:
              sequence.autotrackData[whichAT].setTrigger(channel,channelNumber);
              break;
          }
          return;
        }
      }
      if(controls.MENU()){
        lastTime = millis();
        return;
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        if(togglingGate && controls.joystickY == -1){
          togglingGate = false;
          trigSource = 0;
          lastTime = millis();
        }
        else if(controls.joystickY == 1){
          switch(trigSource){
            //global
            case 0:
              togglingGate = true;
              lastTime = millis();
              break;
            //track
            case 1:
              if(trackNumber<sequence.trackData.size()-1){
                trackNumber++;
                if(trackViewStart<=trackNumber - numberOfTracks)
                  trackViewStart++;
                lastTime = millis();
              }
              break;
            //channel
            case 2:
              if(channelNumber<16){
                channelNumber++;
                if(channelViewStart<=channelNumber - numberOfTracks)
                  channelViewStart++;
                lastTime = millis();
              }
              break;
          }
        }
        else if(controls.joystickY == -1){
          switch(trigSource){
            //global
            case 0:
              break;
            //track
            case 1:
              if(trackNumber>0){
                trackNumber--;
                if(trackNumber<trackViewStart)
                  trackViewStart--;
                lastTime = millis();
              }
              break;
            //channel
            case 2:
              if(channelNumber>1){
                channelNumber--;
                if(channelViewStart>channelNumber)
                  channelViewStart--;
                lastTime = millis();
              }
              break;
          }
        }
      }
    }
  }
}

//for editing a single Autotrack
void autotrackEditor(){
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
        sequence.autotrackData[sequence.activeAutotrack].recordFrom = 1;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.counterB != 0){
        controls.counterB = 0;
        sequence.autotrackData[sequence.activeAutotrack].recordFrom = 2;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickX != 0){
        sequence.autotrackData[sequence.activeAutotrack].recordFrom = 3;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickY != 0){
        sequence.autotrackData[sequence.activeAutotrack].recordFrom = 4;
        recInputSet = true;
        lastTime = millis();
      }
    }
    else{
      //this is done just to pass the showingInfo param 
      if(sequence.autotrackData[sequence.activeAutotrack].type == 0){
        if(!autotrackEditingControls(&interpolatorType,&settingRecInput)){
          return;
        }
      }
      else{
        if(!autotrackCurveEditingControls(&translation,&settingRecInput)){
          return;
        }
      }
      if(controls.SHIFT()){
        MIDI.sendCC(sequence.autotrackData[sequence.activeAutotrack].control,127,sequence.autotrackData[sequence.activeAutotrack].channel);      
      }
    }
    display.clearDisplay();
    drawAutotrackEditor(0,interpolatorType,translation,settingRecInput);
    display.display();
  }
}

void autotrackViewer(){
  //display...4 tracks? idk
  uint8_t firstTrack = 0;
  //where the data displays from (the sequence.cursorPos)
  uint16_t start = 0;
  // WireFrame pram = makePram();
  // pram.xPos = 110;
  // pram.yPos = 10;
  // pram.scale = 1;
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(!autotrackViewerControls()){
      return;
    }
    //handling menu bounds after sequence.activeAutotrack is changed
    if(sequence.activeAutotrack<firstTrack)
      firstTrack = sequence.activeAutotrack;
    else if(sequence.activeAutotrack>firstTrack+4)
      firstTrack = sequence.activeAutotrack-4;
    
    display.clearDisplay();
    drawAutotrackViewer(firstTrack);
    display.display();
  }
  controls.resetEncoders();
}
//selects OR deselects a point, instead of toggling its selection state
void selectDataPoint(uint16_t index, bool state){
    //if the point is in bounds
  if(index>=0 && index<sequence.sequenceLength){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size();i++){
      if(index == sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]){
        alreadySelected = true;
        //if it's already selected, and you want it to be selected, return here
        if(state){
          return;
        }
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]);
      }
    }
    //if it was already selected, and you don't want it to be, del it
    if(!state){
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]>index){
          sequence.autotrackData[sequence.activeAutotrack].selectedPoints.insert(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.push_back(index);
    }
    //if it's the first in the vector, no need to sort it
    else if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size() == 0){
      sequence.autotrackData[sequence.activeAutotrack].selectedPoints.push_back(index);
    }
  }
}

void deleteDataPoint(uint16_t point, uint8_t track){
  //if the track and point are in bounds
  if(track<sequence.autotrackData.size() && point <= sequence.sequenceLength){
    //controls.DELETE()eting data stored there
    sequence.autotrackData[track].data[point] = 255;
    //deselecting it (maybe it's better to not deselect it? idk)
    selectDataPoint(point, false);
  }
}
void regenDT(uint8_t which){
  //if there are any selected points, ONLY regen between those points
  if(sequence.autotrackData[which].selectedPoints.size()>1){
    regenDT(which,sequence.autotrackData[which].selectedPoints[0],sequence.autotrackData[which].selectedPoints.back());
  }
  //regens entire DT
  else
    regenDT(which,0,sequence.sequenceLength);
}

//"regenerates" autotrack from it's period, amplitude, phase, and yPosition, between start and end
void regenDT(uint8_t which,uint16_t start, uint16_t end){
  switch(sequence.autotrackData[which].type){
    //default
    case 0:
      for(uint16_t point = start; point<=end; point++){
        sequence.autotrackData[which].data[point] = 64;
      }
      break;
    //sinewave
    case 1:
      {
        for(uint16_t point = start; point <= end; point++){
          //this one is crazy, and cool, but busted and idk why
          // int pt = sequence.autotrackData[which].amplitude*sin( (sequence.autotrackData[which].period/(2*PI)) * (point+sequence.autotrackData[which].phase) ) + sequence.autotrackData[which].yPos;
          //format is A * sin((2pi/Per)*(x+Phase))+yPos
          int pt = sequence.autotrackData[which].amplitude*sin(float(point+sequence.autotrackData[which].phase)*float(2*PI)/float(sequence.autotrackData[which].period)) + sequence.autotrackData[which].yPos;
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          sequence.autotrackData[which].data[point] = pt;
        }
      }
      break;
    //square wave
    case 2:
      {
        for(uint16_t point = start; point <= end; point++){
          int pt;
          //if you're less than half a period, it's high. Else, it's low
          if((point+sequence.autotrackData[which].phase)%sequence.autotrackData[which].period<sequence.autotrackData[which].period/2)
            pt = sequence.autotrackData[which].yPos+sequence.autotrackData[which].amplitude;
          else
            pt = sequence.autotrackData[which].yPos-sequence.autotrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          sequence.autotrackData[which].data[point] = pt;
        }
      }
      break;
    //saw
    case 3:
      {
        //the slope of each saw tooth is rise/run = amplitude/period
        float slope = float(sequence.autotrackData[which].amplitude*2)/float(sequence.autotrackData[which].period);
        for(uint16_t point = start; point < end; point++){
          // y = (x+offset)%period * m + yOffset
          int pt = ((point+sequence.autotrackData[which].phase)%sequence.autotrackData[which].period)*slope+sequence.autotrackData[which].yPos-sequence.autotrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          sequence.autotrackData[which].data[point] = pt;
        }
      }
      break;
    //triangle
    case 4:
      {
        float slope = float(sequence.autotrackData[which].amplitude*2)/float(sequence.autotrackData[which].period/2);
        int pt;
        for(uint16_t point = start; point < end; point++){
          //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
          if((point+sequence.autotrackData[which].phase)%sequence.autotrackData[which].period>(sequence.autotrackData[which].period/2))
            pt = ((point+sequence.autotrackData[which].phase)%sequence.autotrackData[which].period) * (-slope) + sequence.autotrackData[which].yPos + 3*sequence.autotrackData[sequence.activeAutotrack].amplitude;
          else
            pt = ((point+sequence.autotrackData[which].phase)%sequence.autotrackData[which].period) * slope + sequence.autotrackData[which].yPos - sequence.autotrackData[sequence.activeAutotrack].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          sequence.autotrackData[which].data[point] = pt;
        }
      }
      break;

    //random patterns
    case 5:
      uint16_t pt = start;
      //always seed it with the autotrack id
      srand(which);
      while(pt<end){
        //get a random number to divide the period by (it'll always be a factor of the period)
        // int8_t randomDiv = random(1,5);
        // uint16_t nextPoint = pt+sequence.autotrackData[which].period/randomDiv;
        uint16_t nextPoint = pt+sequence.autotrackData[which].period;
        if(nextPoint>end){
          nextPoint = end;
        }
        randomInterp(pt,nextPoint,which);
        pt = nextPoint;
      }
      break;
  }
}

void randomInterp(uint16_t start, uint16_t end, uint8_t which){
  if(end>=sequence.autotrackData[which].data.size())
    end = sequence.autotrackData[which].data.size()-1;
  //0 is linear interp, 1 is elliptical interp up, 2 is down, 3 is sin, 4 is sq, 5 is saw, 6 is triangle
  //for 0-2, generate random value for the second point
  //for 3-6, generate random amplitude
  //phase should be set so that it always lines up! how do we do this
  //phase = start, yPos = autotrack[start]
  uint8_t randomFunction = random(0,7);

  uint16_t oldPhase = sequence.autotrackData[which].phase;
  uint8_t oldYPos = sequence.autotrackData[which].yPos;
  uint8_t oldAmp = sequence.autotrackData[which].amplitude;
  if(randomFunction<=2){
    sequence.autotrackData[which].data[end] = random(0,sequence.autotrackData[which].amplitude+1);
  }
  else if(randomFunction>=3){
    sequence.autotrackData[which].phase = start;
    sequence.autotrackData[which].yPos = sequence.autotrackData[which].data[start];
    // sequence.autotrackData[which].amplitude = random(-sequence.autotrackData[which].amplitude,sequence.autotrackData[which].amplitude+1);
  }
  switch(randomFunction){
    //linear
    case 0:
      linearInterpolate(start,end,which);
      break;
    //elliptical up
    case 1:
      ellipticalInterpolate(start,end,which,true);
      break;
    //elliptical down
    case 2:
      ellipticalInterpolate(start,end,which,false);
      break;
    //sine
    case 3:
      sequence.autotrackData[which].type = 1;
      regenDT(start,end,which);
      break;
    //sq
    case 4:
      sequence.autotrackData[which].type = 2;
      regenDT(start,end,which);
      break;
    //saw
    case 5:
      sequence.autotrackData[which].type = 3;
      regenDT(start,end,which);
      break;
    //triangle
    case 6:
      sequence.autotrackData[which].type = 4;
      regenDT(start,end,which);
      break;
  }
  //reset it back to "random" type
  sequence.autotrackData[which].type = 5;
  //reset the curve values
  sequence.autotrackData[which].phase = oldPhase;
  sequence.autotrackData[which].yPos = oldYPos;
  sequence.autotrackData[which].amplitude = oldAmp;
}

bool autotrackEditingControls(uint8_t *interpType, bool *settingRecInput){
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
      if(controls.counterA >= 1 && sequence.autotrackData[sequence.activeAutotrack].channel<16){
        sequence.autotrackData[sequence.activeAutotrack].channel++;
      }
      else if(controls.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(sequence.autotrackData[sequence.activeAutotrack].parameterType == 2 && sequence.autotrackData[sequence.activeAutotrack].channel>0)
          sequence.autotrackData[sequence.activeAutotrack].channel--;
        else if(sequence.autotrackData[sequence.activeAutotrack].channel>1)
          sequence.autotrackData[sequence.activeAutotrack].channel--;
      }
    }
    controls.counterA += controls.counterA<0?1:-1;
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
        sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,true,sequence.autotrackData[sequence.activeAutotrack].parameterType);
      }
      else if(controls.counterB <= -1){
        sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,false,sequence.autotrackData[sequence.activeAutotrack].parameterType);
      }
    }
    controls.counterB += controls.counterB<0?1:-1;
  }

  if(!recordingToAutotrack || sequence.autotrackData[sequence.activeAutotrack].recordFrom != 3){
    if (utils.itsbeen(50)) {
      //moving
      if (controls.joystickX == 1 && controls.SHIFT()) {
        moveAutotrackCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && controls.SHIFT()) {
        moveAutotrackCursor(1);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !controls.SHIFT()) {
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
      if (controls.joystickX == -1 && !controls.SHIFT()) {
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
  if(!recordingToAutotrack || sequence.autotrackData[sequence.activeAutotrack].recordFrom != 4){
    if(utils.itsbeen(50)){
      if(controls.joystickY != 0 && !controls.NEW()){
        if(controls.joystickY == -1){
          if(controls.SHIFT()){
            changeDataPoint(1);
            lastTime = millis();
          }
          else{
            changeDataPoint(8);
            lastTime = millis();
          }
        }
        if(controls.joystickY == 1){
          if(controls.SHIFT()){
            changeDataPoint(-1);
            lastTime = millis();
          }
          else{
            changeDataPoint(-8);
            lastTime = millis();
          }
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
        while(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()>0){
          deleteDataPoint(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[0],sequence.activeAutotrack);
        }
      }
      else
        deleteDataPoint(sequence.cursorPos,sequence.activeAutotrack);
    }
    if(controls.PLAY()){
      if(controls.SHIFT() || recording){
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
        sequence.autotrackData[sequence.activeAutotrack].type = 0;
        lastTime = millis();
      }
      //change type of autotrack
      else{
        sequence.autotrackData[sequence.activeAutotrack].type++;
        if(sequence.autotrackData[sequence.activeAutotrack].type>2)
          sequence.autotrackData[sequence.activeAutotrack].type = 0;
        regenDT(sequence.activeAutotrack);
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
        smoothSelectedPoints(*interpType);
      }
    }
    if(controls.SELECT() ){
      //deselect all data points
      if(controls.SHIFT()){
        vector<uint16_t> temp;
        sequence.autotrackData[sequence.activeAutotrack].selectedPoints.swap(temp);
      }
      //select a data point
      else
        selectDataPoint(sequence.cursorPos);
      lastTime = millis();
    }
    if(controls.A()){
      lastTime = millis();
      int8_t param = selectCCParam_autotrack(sequence.autotrackData[sequence.activeAutotrack].parameterType);
      if(param != 255)
        sequence.autotrackData[sequence.activeAutotrack].control = param;
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
      sequence.autotrackData[sequence.activeAutotrack].recordFrom = 0;
    }
    else{
      *settingRecInput = false;
    }
  }
  return true;
}

//x is change period, y is change amplitude, controls.SHIFT() x is change phase, controls.SHIFT() y is vertically translate
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput){
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
      if(controls.counterA >= 1 && sequence.autotrackData[sequence.activeAutotrack].channel<16){
        sequence.autotrackData[sequence.activeAutotrack].channel++;
      }
      else if(controls.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(sequence.autotrackData[sequence.activeAutotrack].parameterType == 2 && sequence.autotrackData[sequence.activeAutotrack].channel>0)
          sequence.autotrackData[sequence.activeAutotrack].channel--;
        else if(sequence.autotrackData[sequence.activeAutotrack].channel>1)
          sequence.autotrackData[sequence.activeAutotrack].channel--;
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
        sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,true,sequence.autotrackData[sequence.activeAutotrack].parameterType);
      }
      else if(controls.counterB <= -1){
        sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,false,sequence.autotrackData[sequence.activeAutotrack].parameterType);
      }
    }
    controls.counterB += controls.counterB<0?1:-1;
  }
  if(utils.itsbeen(200)){
    //toggling DT type
    if(controls.NEW()){
      if(controls.SHIFT()){
        lastTime = millis();
        sequence.autotrackData[sequence.activeAutotrack].type = 0;
      }
      else{
        lastTime = millis();
        sequence.autotrackData[sequence.activeAutotrack].type++;
        if(sequence.autotrackData[sequence.activeAutotrack].type > 5)
          sequence.autotrackData[sequence.activeAutotrack].type = 0;
        regenDT(sequence.activeAutotrack);
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
      uint8_t param = selectCCParam_autotrack(sequence.autotrackData[sequence.activeAutotrack].parameterType);
      if(param != 255)
        sequence.autotrackData[sequence.activeAutotrack].control = param;
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
    if(controls.joystickX != 0){
      //moving by 1
      if(controls.SHIFT() && utils.itsbeen(25)){
        if(controls.joystickX == -1){
          if(sequence.autotrackData[sequence.activeAutotrack].phase == 0)
            sequence.autotrackData[sequence.activeAutotrack].phase = sequence.autotrackData[sequence.activeAutotrack].period;
          sequence.autotrackData[sequence.activeAutotrack].phase--;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          sequence.autotrackData[sequence.activeAutotrack].phase++;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
        //if the phase = period, then phase resets to 0
        if(sequence.autotrackData[sequence.activeAutotrack].phase == sequence.autotrackData[sequence.activeAutotrack].period)
          sequence.autotrackData[sequence.activeAutotrack].phase = 0;
      }
      //moving by subdivInt
      //phase will always be mod by period (so it can never be more/less than per)
      //storing it in "temp" here so it doesn't underflow/overflow
      else if(!controls.SHIFT() && utils.itsbeen(100)){
        if(controls.joystickX == -1){
          int16_t temp = sequence.autotrackData[sequence.activeAutotrack].phase;
          if(temp<sequence.subDivision)
            temp = abs(sequence.autotrackData[sequence.activeAutotrack].period-sequence.subDivision);
          else
            temp-=sequence.subDivision;
          sequence.autotrackData[sequence.activeAutotrack].phase=temp%sequence.autotrackData[sequence.activeAutotrack].period;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          sequence.autotrackData[sequence.activeAutotrack].phase = (sequence.autotrackData[sequence.activeAutotrack].phase+sequence.subDivision)%sequence.autotrackData[sequence.activeAutotrack].period;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
      }
    }
    //changing vertical translation
    if(controls.joystickY != 0 && utils.itsbeen(25)){
      if(controls.joystickY == -1 && sequence.autotrackData[sequence.activeAutotrack].yPos<127){
        sequence.autotrackData[sequence.activeAutotrack].yPos++;
        regenDT(sequence.activeAutotrack);
        lastTime = millis();
      }
      else if(controls.joystickY == 1 && sequence.autotrackData[sequence.activeAutotrack].yPos>0){
        sequence.autotrackData[sequence.activeAutotrack].yPos--;
        regenDT(sequence.activeAutotrack);
        lastTime = millis();
      }
    }
  }
  //transformation
  else{
    //changing amplitude
    if(controls.joystickY != 0 && utils.itsbeen(25)){
      if(controls.joystickY == -1 && sequence.autotrackData[sequence.activeAutotrack].amplitude<127){
        sequence.autotrackData[sequence.activeAutotrack].amplitude++;
        regenDT(sequence.activeAutotrack);
        lastTime = millis();
      }
      else if(controls.joystickY == 1 && sequence.autotrackData[sequence.activeAutotrack].amplitude>-127){
        sequence.autotrackData[sequence.activeAutotrack].amplitude--;
        regenDT(sequence.activeAutotrack);
        lastTime = millis();
      }
    }
    //changing by 1
    if(controls.SHIFT() && utils.itsbeen(25)){
      //changing period
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          sequence.autotrackData[sequence.activeAutotrack].period++;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && sequence.autotrackData[sequence.activeAutotrack].period>1){
          sequence.autotrackData[sequence.activeAutotrack].period--;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
      }
    }
    else if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          if(sequence.autotrackData[sequence.activeAutotrack].period == 1)
            sequence.autotrackData[sequence.activeAutotrack].period = 0;
          sequence.autotrackData[sequence.activeAutotrack].period+=sequence.subDivision;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && sequence.autotrackData[sequence.activeAutotrack].period>sequence.subDivision){
          sequence.autotrackData[sequence.activeAutotrack].period-=sequence.subDivision;
          regenDT(sequence.activeAutotrack);
          lastTime = millis();
        }
      }
    }
  }
  return true;
}


//using slope-intercept eq
void linearInterpolate(uint16_t start, uint16_t end, uint8_t id){
  if(start>=0 && end <= sequence.autotrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(sequence.autotrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          sequence.autotrackData[id].data[start] = 63;
          break;
        }
        else if(sequence.autotrackData[id].data[i] != 255){
          sequence.autotrackData[id].data[start] = sequence.autotrackData[id].data[i];
          break;
        }
      }
    }
    if(sequence.autotrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          sequence.autotrackData[id].data[end] = 63;
          break;
        }
        else if(sequence.autotrackData[id].data[i] != 255){
          sequence.autotrackData[id].data[end] = sequence.autotrackData[id].data[i];
          break;
        }
      }
    }

    float m = float(sequence.autotrackData[id].data[start] - sequence.autotrackData[id].data[end])/float(start-end);
    uint8_t b = sequence.autotrackData[id].data[start];
    for(uint16_t i = start; i<=end; i++){
      sequence.autotrackData[id].data[i] = m*(i-start)+b;
    }
  }
}

//using ellipse equation
void ellipticalInterpolate(uint16_t start, uint16_t end, uint8_t id, bool up){
  //if they're the same point (which would be a bug) or the same value, return since you won't need to interpolate
  if(start == end || sequence.autotrackData[id].data[start] == sequence.autotrackData[id].data[end])
    return;
  //prepping autotrack data
  //if it's in bounds
  if(start>=0 && end <= sequence.autotrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(sequence.autotrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          sequence.autotrackData[id].data[start] = 63;
          break;
        }
        else if(sequence.autotrackData[id].data[i] != 255){
          sequence.autotrackData[id].data[start] = sequence.autotrackData[id].data[i];
          break;
        }
      }
    }
    if(sequence.autotrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          sequence.autotrackData[id].data[end] = 63;
          break;
        }
        else if(sequence.autotrackData[id].data[i] != 255){
          sequence.autotrackData[id].data[end] = sequence.autotrackData[id].data[i];
          break;
        }
      }
    }

    //interpolation time

    //ellipse eq
    //x^2/a^2 + y^2/b^2 = 1
    //solved for y = +- root(1 - x^2/a^2)/b
    
    //'a' is the horizontal distance
    uint16_t a = abs(end - start);
    //'b' is the vertical distance
    uint8_t b = abs(sequence.autotrackData[id].data[end]-sequence.autotrackData[id].data[start]);

    //to store where (0,0) is
    uint8_t yOffset;
    uint16_t xOffset;

    //if "up", the yOffset is the highest of the two points since y will always be negative
    if(up){
      if(sequence.autotrackData[id].data[end]<sequence.autotrackData[id].data[start]){
        yOffset = sequence.autotrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = sequence.autotrackData[id].data[end];
        xOffset = start;
      }
    }
    else{
      if(sequence.autotrackData[id].data[end]>sequence.autotrackData[id].data[start]){
        yOffset = sequence.autotrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = sequence.autotrackData[id].data[end];
        xOffset = start;
      }
    }
    
    //setting each datapoint to a point along the arc
    for(uint16_t i = start; i<=end; i++){
      //subtract xOffset
      uint16_t pt = sqrt(1-pow(i-xOffset,2)/pow(a,2))*b;
      //if it's an "up" arc, then y is negative and the yOffset is the higher coord
      if(up)
        pt = -pt;
      sequence.autotrackData[id].data[i] = pt + yOffset;
    }
  }
}

//this one smooths between selected points
void smoothSelectedPoints(uint8_t type){
  if(sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()>=2){
    switch(type){
      //linear
      case 0:
        //move through each selected point
        for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()-1; i++){
          linearInterpolate(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i],sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i+1],sequence.activeAutotrack);
        }
        return;
      //elliptical UP
      case 1:
        //move through each selected point
        for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i],sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i+1],sequence.activeAutotrack,true);
        }
        return;
      //elliptical DOWN
      case 2:
        //move through each selected point
        for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i],sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i+1],sequence.activeAutotrack,false);
        }
        return;
    }
  }
}

bool isDataPointSelected(uint16_t index){
  if(index>=0 && index<sequence.sequenceLength){
    //check each point to see if it matches index
    for(uint16_t i = 0; i<sequence.autotrackData[sequence.activeAutotrack].selectedPoints.size(); i++){
      if(index == sequence.autotrackData[sequence.activeAutotrack].selectedPoints[i]){
        return true;
      }
    }
  }
  //if it's out of bounds, or wasn't found in the selection, return false
  return false;
}

uint8_t getLastDTVal(uint16_t point, uint8_t id){
  //if params are in bounds
  if(id<sequence.autotrackData.size() && point<=sequence.sequenceLength){
    //step back thru DT and look for a non-255 value
    for(uint16_t i = point; i>=0; i--){
      if(sequence.autotrackData[id].data[i] != 255){
        return sequence.autotrackData[id].data[i];
      }
    }
  }
  //if something goes wrong, return 64
  return 64;
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

void drawAutotrackEditor(uint8_t y,uint8_t interpType,bool translation, bool settingRecInput){
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
        if(sequence.autotrackData[sequence.activeAutotrack].data[step] == 255){
          yPos = (64-float(getLastDTVal(step,sequence.activeAutotrack))/float(127)*64);
        }
        else{
          yPos = (64-float(sequence.autotrackData[sequence.activeAutotrack].data[step])/float(127)*64);
        }
        if(step == sequence.viewStart){
          display.drawPixel(32+(step-sequence.viewStart)*sequence.viewScale,(yPos),SSD1306_WHITE);
        }
        else{
          display.drawLine(32+(step-sequence.viewStart)*sequence.viewScale,(yPos),32+(step-1-sequence.viewStart)*sequence.viewScale,(64-float(getLastDTVal(step-1,sequence.activeAutotrack))/float(127)*64),SSD1306_WHITE);
        }

        //drawing cursor info if in default editor mode
        if(sequence.autotrackData[sequence.activeAutotrack].type == 0){
          //drawing selection arrow
          if(isDataPointSelected(step)){
            display.fillCircle(32+(step-sequence.viewStart)*sequence.viewScale,yPos,2,SSD1306_WHITE);
            if(sequence.autotrackData[sequence.activeAutotrack].data[step]>=64)//bottom arrow if data point is high
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
        if(playing && ((sequence.autotrackData[sequence.activeAutotrack].triggerSource == global && step == sequence.playheadPos) || (sequence.autotrackData[sequence.activeAutotrack].triggerSource != global && step == sequence.autotrackData[sequence.activeAutotrack].playheadPos)))
          display.drawRoundRect(32+(step-sequence.viewStart)*sequence.viewScale,0,3,screenHeight,3,SSD1306_WHITE);
        
        //rechead
        if(recording && sequence.recheadPos == step)
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
    if(sequence.autotrackData[sequence.activeAutotrack].type == 0)
      graphics.drawDottedLineH(32,128,32,3);
    else{
      graphics.drawDottedLineH(32,128,64-(sequence.autotrackData[sequence.activeAutotrack].yPos-32),3);
    }
    //drawing curve icon
    if(sequence.autotrackData[sequence.activeAutotrack].type == 0)
      drawNodeEditingIcon(12,14,interpType,animOffset,true);
    else
      drawCurveIcon(12,14,sequence.autotrackData[sequence.activeAutotrack].type,animOffset);
    animOffset++;
    animOffset%=18;

    //menu info
    if(controls.SHIFT()){
      printParam(16,2,sequence.autotrackData[sequence.activeAutotrack].control,true,sequence.autotrackData[sequence.activeAutotrack].parameterType,true);
      graphics.printChannel(16,12,sequence.autotrackData[sequence.activeAutotrack].channel,true);
      display.drawChar(3,2+sin(millis()/50),0x0E,1,0,1);
    }
    else{
      //title
      if(!playing){
        printSmall(0,0,"trk",SSD1306_WHITE);
        printSmall(6-stringify(sequence.activeAutotrack+1).length()*2,7,stringify(sequence.activeAutotrack+1),SSD1306_WHITE);
      }
      printSmall(15,0,"CC"+stringify(sequence.autotrackData[sequence.activeAutotrack].control),SSD1306_WHITE);
      printSmall(15,7,"CH"+stringify(sequence.autotrackData[sequence.activeAutotrack].channel),SSD1306_WHITE);
    }
    //drawing curve info, for non-defaults
    if(sequence.autotrackData[sequence.activeAutotrack].type != 0){
      if(translation){
        printSmall(12,26,"@:"+stringify(sequence.autotrackData[sequence.activeAutotrack].phase),SSD1306_WHITE);
        printSmall(12,33,"Y:"+stringify(sequence.autotrackData[sequence.activeAutotrack].yPos),SSD1306_WHITE);
      }
      else{
        printSmall(12,26,"A:"+stringify(sequence.autotrackData[sequence.activeAutotrack].amplitude),SSD1306_WHITE);
        printSmall(12,33,"P:"+stringify(sequence.autotrackData[sequence.activeAutotrack].period),SSD1306_WHITE);
      }
    }
    
    //play icon
    if(playing)
      display.fillTriangle(120+((millis()/200)%2),9,120+((millis()/200)%2),3,120+6+((millis()/200)%2),6,SSD1306_WHITE);
    if(recording){
      //flash it while waiting
      if(waitingToReceiveANote && (millis()%1000>500))
        display.drawCircle(124,6,3,SSD1306_WHITE);
      //draw it solid if the rec has started
      else if(!waitingToReceiveANote)
        display.fillCircle(124,6,3,SSD1306_WHITE);
    }
    
    //drawing the bargraph
    uint8_t barHeight;
    if(!playing)
      barHeight = 50;
    else
     barHeight = 64;
    display.drawRect(0,screenHeight-barHeight,11,barHeight,SSD1306_WHITE);
    uint8_t height;
    uint8_t val;
    if(playing){
      if(sequence.autotrackData[sequence.activeAutotrack].data[sequence.autotrackData[sequence.activeAutotrack].triggerSource==global?sequence.playheadPos:sequence.autotrackData[sequence.activeAutotrack].playheadPos] == 255)
        val = getLastDTVal(sequence.autotrackData[sequence.activeAutotrack].triggerSource==global?sequence.playheadPos:sequence.autotrackData[sequence.activeAutotrack].playheadPos,sequence.activeAutotrack);
      else
        val = sequence.autotrackData[sequence.activeAutotrack].data[sequence.autotrackData[sequence.activeAutotrack].triggerSource==global?sequence.playheadPos:sequence.autotrackData[sequence.activeAutotrack].playheadPos];
    }
    else{
      if(sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos] == 255)
        val = getLastDTVal(sequence.cursorPos,sequence.activeAutotrack);
      else
        val = sequence.autotrackData[sequence.activeAutotrack].data[sequence.cursorPos];
    }
    //filling the bar graphgraph
    height = abs(float(val)/float(127)*float(barHeight-4));
    if(height>0)
      display.fillRect(2,62-height,7,height,2);//inverted color!
    
    //drawing sent data
    display.setRotation(1);
    if(playing){
      printSmall(barHeight/2-stringify(getLastDTVal(sequence.autotrackData[sequence.activeAutotrack].triggerSource==global?sequence.playheadPos:sequence.autotrackData[sequence.activeAutotrack].playheadPos,sequence.activeAutotrack)).length()*2,3,stringify(getLastDTVal(sequence.autotrackData[sequence.activeAutotrack].triggerSource==global?sequence.playheadPos:sequence.autotrackData[sequence.activeAutotrack].playheadPos,sequence.activeAutotrack)),2);
    }
    else{
      printSmall(barHeight/2-stringify(getLastDTVal(sequence.cursorPos,sequence.activeAutotrack)).length()*2,3,stringify(getLastDTVal(sequence.cursorPos,sequence.activeAutotrack)),2);
    }
    display.setRotation(DISPLAY_UPRIGHT);

    //drawing source icon
    printSmall(15,42,"src",1);
    graphics.drawArrow(20,52+sin(millis()/400),2,3,false);
    if(!settingRecInput || (millis()%40>20))
      drawAutotrackInputIcon(17,55,sequence.activeAutotrack);
  }
  else{
    printSmall(50,29,"no data, kid",SSD1306_WHITE);
  }

}

bool autotrackViewerControls(){
  //changing cc
  while(controls.counterB != 0){
    if(controls.counterB >= 1 && sequence.autotrackData[sequence.activeAutotrack].control<127){
      sequence.autotrackData[sequence.activeAutotrack].control++;
      }
    if(controls.counterB <= -1 && sequence.autotrackData[sequence.activeAutotrack].control>0){
      sequence.autotrackData[sequence.activeAutotrack].control--;
    }
    controls.counterB += controls.counterB<0?1:-1;
  }
  //changing ch
  while(controls.counterA != 0){
    if(controls.counterA >= 1 && sequence.autotrackData[sequence.activeAutotrack].channel<16){
      sequence.autotrackData[sequence.activeAutotrack].channel++;
      }
    if(controls.counterA <= -1 && sequence.autotrackData[sequence.activeAutotrack].channel>1){
      sequence.autotrackData[sequence.activeAutotrack].channel--;
    }
    controls.counterA += controls.counterA<0?1:-1;
  }
  //sending CC message when controls.SHIFT() is held
  if(controls.SHIFT() && sequence.autotrackData.size()>sequence.activeAutotrack){
    sequence.autotrackData[sequence.activeAutotrack].play(0);
  }
  //scrolling up and down
  if(utils.itsbeen(100)){
    if(controls.joystickY != 0){
      if(sequence.autotrackData.size() != 0){
        if(controls.joystickY == -1 && sequence.activeAutotrack>0){
          sequence.activeAutotrack--;
          lastTime = millis();
        }
        if(controls.joystickY == 1 && sequence.activeAutotrack<sequence.autotrackData.size()-1){
          sequence.activeAutotrack++;
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.joystickX != 0){
      //changing control #
      if(!controls.SHIFT()){
        if(controls.joystickX == -1){
          sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,true,sequence.autotrackData[sequence.activeAutotrack].parameterType);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          sequence.autotrackData[sequence.activeAutotrack].control = moveToNextCCParam(sequence.autotrackData[sequence.activeAutotrack].control,false,sequence.autotrackData[sequence.activeAutotrack].parameterType);
          lastTime = millis();
        }
      }
      //changing channel #
      else{
        if(controls.joystickX == -1 && sequence.autotrackData[sequence.activeAutotrack].channel<16){
          sequence.autotrackData[sequence.activeAutotrack].channel++;
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && sequence.autotrackData[sequence.activeAutotrack].channel>1){
          sequence.autotrackData[sequence.activeAutotrack].channel--;
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    //selecting a autotrack
    if(controls.SELECT() ){
      if(sequence.autotrackData.size()){
        //if shifting, and if there are any autotracks
        if(controls.SHIFT()){
          lastTime = millis();
          sequence.autotrackData[sequence.activeAutotrack].setTitle(enterText("name?",4));
        }
        else{
          lastTime = millis();
          autotrackEditor();
        }
      }
    }
    if(controls.NEW()){
      if(sequence.autotrackData.size() == 0)
        createAutotrack(0);
      else
        createAutotrack(0,(sequence.autotrackData[sequence.autotrackData.size()-1].control>=127)?0:(sequence.autotrackData[sequence.autotrackData.size()-1].control+1));
      lastTime = millis();
    }
    if(controls.DELETE()){
      if(controls.SHIFT() && sequence.activeAutotrack<sequence.autotrackData.size()){
        sequence.autotrackData[sequence.activeAutotrack].isActive = !sequence.autotrackData[sequence.activeAutotrack].isActive;
        lastTime = millis();
      }
      else{
        deleteAutotrack(sequence.activeAutotrack);
        lastTime = millis();
      }
    }
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.PLAY()){
      if(controls.SHIFT() && sequence.activeAutotrack<sequence.autotrackData.size()){
        sequence.autotrackData[sequence.activeAutotrack].isPrimed = !sequence.autotrackData[sequence.activeAutotrack].isPrimed;
        lastTime = millis();
      }
      else{
        togglePlayMode();
        lastTime = millis();
      }
    }
    if(controls.COPY()){
      dupeAutotrack(sequence.activeAutotrack);
      lastTime = millis();
    }
    if(controls.A()){
      controls.setA(false);
      uint8_t param = selectCCParam_autotrack(sequence.autotrackData[sequence.activeAutotrack].parameterType);
      if(param != 255)
        sequence.autotrackData[sequence.activeAutotrack].control = param;
      lastTime = millis();
    }
    if(controls.LOOP() && sequence.activeAutotrack<sequence.autotrackData.size()){
      lastTime = millis();
      setAutotrackTrigger(sequence.activeAutotrack);
    }
  }
  return true;
}

bool atLeastOneActiveAutotrack(){
  for(uint8_t i = 0; i<sequence.autotrackData.size(); i++){
    if(sequence.autotrackData[i].isActive)
      return true;
  }
  return false;
}

void drawAutotrackViewer(uint8_t firstTrack){
  //each track is 14 high, except the target which is 20
  const uint8_t height = 9;
  const uint8_t sideMenu = 32;

  //stores the current height of the track stack (so it can grow by different amounts)
  uint8_t currentHeight = 8;

  display.setRotation(1);
  printArp(2,0,"AUTOTRACKS",1);
  display.setRotation(DISPLAY_UPRIGHT);
  //drawing tracks
  if(sequence.autotrackData.size()>0){
    animOffset++;
    animOffset%=18;

    for(uint8_t track = 0; track<5; track++){
      //if there's data for this track (if it exists)
      if(track+firstTrack<sequence.autotrackData.size()){
        //if it's the sequence.activeAutotrack
        if(track+firstTrack == sequence.activeAutotrack){
          //track box
          display.drawRoundRect(9,currentHeight,119,23,3,1);
          //track data
          graphics.fillRectWithMissingCorners(sideMenu-2,currentHeight+4,screenWidth-sideMenu,height+6,SSD1306_BLACK);
          graphics.drawRectWithMissingCorners(sideMenu-3,currentHeight+3,screenWidth-sideMenu,height+8,SSD1306_WHITE);

          drawMiniDT(sideMenu-3,currentHeight+4,height+6,sequence.activeAutotrack);
          //cc/ch indicator
          display.drawBitmap(11,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          display.drawBitmap(11,currentHeight+9,ch_tiny,6,3,SSD1306_WHITE);
          printSmall(17,currentHeight+2,stringify(sequence.autotrackData[sequence.activeAutotrack].control),1);
          printSmall(18,currentHeight+8,stringify(sequence.autotrackData[track+firstTrack].channel),SSD1306_WHITE);
          //curve icon/send icon while shifting
          drawCurveIcon(12,currentHeight+14,sequence.autotrackData[sequence.activeAutotrack].type,animOffset);
          
          //if shifting, draw note icon
          if(controls.SHIFT())
            display.drawChar(120-(firstTrack?8:1)+sin(millis()/50),0,0x0E,1,0,1);    
          //if not, print track number        
          else
            print7SegSmall(screenWidth-countDigits_byte(sequence.activeAutotrack)*4-(firstTrack?8:1),0,stringify(sequence.activeAutotrack),1);

          String trigger;
          switch(sequence.autotrackData[sequence.activeAutotrack].triggerSource){
            case global:
              trigger = "global";
              break;
            case TriggerSource::track:
              trigger = "trck: "+stringify(sequence.autotrackData[sequence.activeAutotrack].triggerTarget);
              break;
              case channel:
              trigger = "chnl: "+stringify(sequence.autotrackData[sequence.activeAutotrack].triggerTarget);
              break;
          }
          graphics.drawLabel(112,currentHeight+2,trigger,false);

          //arrow highlight
          graphics.drawArrow(110,currentHeight+2+cos(millis()/100),4,3,false);

          //increment height so the next track is drawn below
          currentHeight+=height+12;
        }
        //if it's a normal track (nonactive)
        else{
          //back up the cc label when there're more digits
          uint8_t x1 = 26 - countDigits_byte(sequence.autotrackData[firstTrack+track].control)*4;

          display.drawBitmap(x1,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          printSmall(x1+6,currentHeight+2,stringify(sequence.autotrackData[firstTrack+track].control),1);
          
          display.fillRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_BLACK);
          display.drawRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_WHITE);
          if(sequence.autotrackData[track+firstTrack].isActive){
            //if it doesn't have a label, just print the param name
            if(sequence.autotrackData[firstTrack+track].title[0] == -1)
              printParam_centered(track+firstTrack,sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,sequence.autotrackData[track+firstTrack].control,false,sequence.autotrackData[track+firstTrack].parameterType,false);
            else{
              String label = "";
              for(uint8_t i = 0; i<4; i++){
                label += sequence.autotrackData[firstTrack+track].title[i];
              }
              printSmall_centered(sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,label,1);
            }
          }
          else
            printSmall_centered(sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,"[inactive]",1);
          currentHeight+=height-1;
        }
      }
    }

    printCursive(10,0,"track - "+sequence.autotrackData[sequence.activeAutotrack].getTitle(),1);

    //drawing 'more tracks' indicators
    if(firstTrack>0){
      graphics.drawArrow(14,9+sin(millis()/75),2,2,true);
    }
    if(firstTrack+4<sequence.autotrackData.size()-1){
      graphics.drawArrow(14,63-sin(millis()/75),2,3,true);
    }
  }
  else{
    printSmall(11,3,"No tracks, kid",1);
    printSmall(11,12,"press [n] to create",1);
    printSmall(11,21,"a new automation track",1);
    display.drawLine(93,27,105,39,1);
    display.drawFastHLine(9,27,84,1);
    display.drawFastVLine(9,1,26,1);
    display.drawLine(94,17,105,39,1);
    display.drawFastVLine(94,1,16,1);
    display.drawFastHLine(9,1,85,1);
  }

  const uint8_t x1 = 101;
  const uint8_t y1 = 38;
  //Drawing our robot bud
  if(playing && atLeastOneActiveAutotrack()){
    uint16_t position = 0;
    //if the autotrack is triggering, then use its internal playhead
    if(sequence.autotrackData[sequence.activeAutotrack].triggerSource != global)
      position = sequence.autotrackData[sequence.activeAutotrack].playheadPos;
    else
      position = sequence.playheadPos;
    switch((position/24)%4){
      case 0:
        display.drawBitmap(x1+1,y1+3,robo3_mask,18,17,0);
        display.drawBitmap(x1,y1,robo3,21,24,1);
        break;
      case 1:
        display.drawBitmap(x1+9,y1+2,robo1_mask,12,17,0);
        display.drawBitmap(x1+3,y1,robo4,18,24,1);
        break;
      case 2:
        display.drawBitmap(x1+9,y1+2,robo5_mask,12,17,0);
        display.drawBitmap(x1+9,y1,robo5,18,24,1);
        break;
      case 3:
        display.drawBitmap(x1+9,y1+2,robo5_mask,12,17,0);
        display.drawBitmap(x1+9,y1,robo6,18,24,1);
        break;
    }
  }
  else{
    display.drawBitmap(x1+9,y1+2,robo1_mask,12,17,0);
    display.drawBitmap(x1+8,y1,robo[(millis()/500)%2],13,24,1);
  }
}

void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which){
  if(sequence.autotrackData[which].isActive){
    float yScale = float(height-1)/float(127);
    // float xScale = scale;
    //i starts at start, goes from start to the end of the screen
    float sc = float(96)/float(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start);
    //ends at sequence.viewEnd-1 because it draws lines 2 points at a time
    for(uint16_t i = sequence.loopData[sequence.activeLoop].start; i<sequence.loopData[sequence.activeLoop].end; i++){
      if(i<sequence.autotrackData[which].data.size()){
        if(i >= sequence.loopData[sequence.activeLoop].end - 1){
          display.drawLine(x1+(i-sequence.loopData[sequence.activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i,which)),x1+(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i,which)),SSD1306_WHITE);
          break;
        }
        else
          display.drawLine(x1+(i-sequence.loopData[sequence.activeLoop].start)*sc, y1+yScale*(127-getLastDTVal(i,which)),x1+(i+1-sequence.loopData[sequence.activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i+1,which)),SSD1306_WHITE);
        if(playing){
          if((sequence.autotrackData[which].triggerSource == global && i == sequence.playheadPos) || (sequence.autotrackData[which].triggerSource != global && i == sequence.autotrackData[which].playheadPos)){
            display.drawFastVLine(x1+(i-sequence.loopData[sequence.activeLoop].start)*sc,y1,height,SSD1306_WHITE);
          }
        }
      }
      else
        break;
    }
    graphics.drawDottedLineH(x1,x1+94,y1+height/2,3);
  }
  //if it's muted/inactive
  else{
    graphics.shadeRect(x1,y1,screenWidth-x1-3,height,5);
    display.fillRoundRect(x1+(screenWidth-x1)/2-13,y1+height/2-3,25,7,3,SSD1306_BLACK);
    printSmall(x1+(screenWidth-x1)/2-14,y1+height/2-2,"[muted]",SSD1306_WHITE);
  }
  if(sequence.autotrackData[which].isPrimed){
    if(millis()/700%2)
      display.fillCircle(x1,y1,2,1);
    else{
      display.fillCircle(x1,y1,2,0);
      display.drawCircle(x1,y1,2,1);
    }
  }
  else{
    display.drawLine(x1-2,y1-2,x1+2,y1+2,1);
    display.drawLine(x1+2,y1-2,x1-2,y1+2,1);
  }
}
