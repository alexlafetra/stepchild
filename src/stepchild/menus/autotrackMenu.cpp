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
  if(cursorPos==0 && moveAmount < 0){
    return;
  }
  if(moveAmount<0 && abs(moveAmount)>=cursorPos)
    cursorPos = 0;
  else
    cursorPos += moveAmount;
  if(cursorPos >= seqEnd) {
    cursorPos = seqEnd-1;
  }
  if(cursorPos<0){
    cursorPos = 0;
  }
  //Move the view along with the cursor
  if(cursorPos<viewStart+subDivInt && viewStart>0){
    moveView(cursorPos - (viewStart+subDivInt));
  }
  else if(cursorPos > viewEnd-subDivInt && viewEnd<seqEnd){
    moveView(cursorPos - (viewEnd-subDivInt));
  }
}

void createDataPoint(){
  autotrackData[activeAutotrack].data[cursorPos] = 64;
}

void changeDataPoint(int8_t amount){
  if(autotrackData[activeAutotrack].data[cursorPos] == 255){
    //if it's a blank point, go back until you hit a real point
    for(uint16_t point = cursorPos; point>=0; point--){
      //if there's a real point
      if(autotrackData[activeAutotrack].data[point] != 255){
        autotrackData[activeAutotrack].data[cursorPos] = autotrackData[activeAutotrack].data[point];
        break;
      }
      //if it never hit a real point, just make one
      if(point == 0){
        autotrackData[activeAutotrack].data[cursorPos] = 64;
        break;
      }
    }
    
  }
  int16_t newVal = autotrackData[activeAutotrack].data[cursorPos]+amount;
  if(newVal<0)
    newVal = 0;
  else if(newVal > 127)
    newVal = 127;
  
  autotrackData[activeAutotrack].data[cursorPos] = newVal;
}

void createAutotrack(uint8_t type){
  Autotrack newData(type);
  autotrackData.push_back(newData);
}

void createAutotrack(uint8_t type, uint8_t cont){
  Autotrack newData(type);
  newData.control = cont;
  autotrackData.push_back(newData);
}

void createAutotrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType){
  Autotrack newData(0);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  autotrackData.push_back(newData);
}

void dupeAutotrack(uint8_t which){
  //if the track exists
  if(which<autotrackData.size()){
    vector<Autotrack> temp;
    for(uint8_t track = 0; track<autotrackData.size(); track++){
      temp.push_back(autotrackData[track]);
      if(track == which)
        temp.push_back(autotrackData[track]);
    }
    temp.swap(autotrackData);
  }
}

void deleteAutotrack(uint8_t track){
  //only delete tracks if there'll be one left over
  vector<Autotrack> temp;
  for(uint8_t i = 0; i<autotrackData.size(); i++){
    if(i != track)
      temp.push_back(autotrackData[i]);
  }
  temp.swap(autotrackData);
  if(activeAutotrack>=autotrackData.size())
    activeAutotrack = autotrackData.size()-1;
  if(autotrackData.size() == 0){
    activeAutotrack = 0;
  }
}

void selectDataPoint(uint16_t index){
  //if the point is in bounds
  if(index>=0 && index<seqEnd){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size();i++){
      if(index == autotrackData[activeAutotrack].selectedPoints[i]){
        alreadySelected = true;
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(autotrackData[activeAutotrack].selectedPoints[i]);
      }
    }
    //if it was already selected, swap vectors with temp to delete it from the selection
    if(alreadySelected){
      autotrackData[activeAutotrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(autotrackData[activeAutotrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(autotrackData[activeAutotrack].selectedPoints[i]>index){
          autotrackData[activeAutotrack].selectedPoints.insert(autotrackData[activeAutotrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      autotrackData[activeAutotrack].selectedPoints.push_back(index);
    }
    //if it's the first vector
    else if(autotrackData[activeAutotrack].selectedPoints.size() == 0){
      autotrackData[activeAutotrack].selectedPoints.push_back(index);
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
      uint8_t lastPoint;
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
        int8_t coef = 1;
        uint8_t pt;
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
  uint8_t trigSource = autotrackData[whichAT].triggerSource;
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
    (trigSource ||togglingGate)?display.drawRect(8,22,27,9,1):display.fillRect(8,22,27,9,1);
    printSmall(10,24,"global",2);
    if(!trigSource && !togglingGate)
      graphics.drawArrow(22,20+((millis()/400)%2),3,3,false);


    //drawing gate label
    graphics.drawBinarySelectionBox(22, 52, "on", "off", "", !autotrackData[whichAT].gated);
    printSmall_centered(22,40,"gate",1);

    if(togglingGate){
      graphics.drawArrow(22,60+((millis()/400)%2),3,2,false);
    }

    //drawing tracks
    for(uint8_t i = 0; i<=numberOfTracks; i++){
      if(i+trackViewStart>=trackData.size())
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
    if(trackViewStart+numberOfTracks<trackData.size()){
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
    display.setRotation(SIDEWAYS_L);
    printSmall(48-y1,x1-6,"trck",1);
    display.setRotation(SIDEWAYS_R);
    printSmall(y1,screenWidth-x2-colWidth-6,"chnl",1);
    display.setRotation(UPRIGHT);

    display.display();
    controls.readJoystick();
    readButtons();
    if(utils.itsbeen(200)){
      if(shift || loop_Press){
        autotrackData[whichAT].gated = !autotrackData[whichAT].gated;
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
      if(sel){
        lastTime = millis();  
        if(togglingGate){
          autotrackData[whichAT].gated = !autotrackData[whichAT].gated;
        }
        else{
          switch(trigSource){
            //global
            case 0:
              autotrackData[whichAT].setTrigger(global,0);
              break;
            //track
            case 1:
              autotrackData[whichAT].setTrigger(track,trackNumber);
              break;
            //channel
            case 2:
              autotrackData[whichAT].setTrigger(channel,channelNumber);
              break;
          }
          return;
        }
      }
      if(menu_Press){
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
              if(trackNumber<trackData.size()-1){
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
    readButtons();
    controls.readJoystick();
    if(settingRecInput){   
      //once copy press is released, after at least one param has been selected
      if(!copy_Press){
        recInputSet = false;
        settingRecInput = false;
      }   
      //if any of these controls are pressed, then set the current data track param to it
      if(counterA != 0){
        counterA = 0;
        autotrackData[activeAutotrack].recordFrom = 1;
        recInputSet = true;
        lastTime = millis();
      }
      else if(counterB != 0){
        counterB = 0;
        autotrackData[activeAutotrack].recordFrom = 2;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickX != 0){
        autotrackData[activeAutotrack].recordFrom = 3;
        recInputSet = true;
        lastTime = millis();
      }
      else if(controls.joystickY != 0){
        autotrackData[activeAutotrack].recordFrom = 4;
        recInputSet = true;
        lastTime = millis();
      }
    }
    else{
      //this is done just to pass the showingInfo param 
      if(autotrackData[activeAutotrack].type == 0){
        if(!autotrackEditingControls(&interpolatorType,&settingRecInput)){
          return;
        }
      }
      else{
        if(!autotrackCurveEditingControls(&translation,&settingRecInput)){
          return;
        }
      }
      if(shift){
        MIDI.sendCC(autotrackData[activeAutotrack].control,127,autotrackData[activeAutotrack].channel);      
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
  //where the data displays from (the cursorPos)
  uint16_t start = 0;
  // WireFrame pram = makePram();
  // pram.xPos = 110;
  // pram.yPos = 10;
  // pram.scale = 1;
  while(true){
    readButtons();
    controls.readJoystick();
    if(!autotrackViewerControls()){
      return;
    }
    //handling menu bounds after activeAutotrack is changed
    if(activeAutotrack<firstTrack)
      firstTrack = activeAutotrack;
    else if(activeAutotrack>firstTrack+4)
      firstTrack = activeAutotrack-4;
    
    display.clearDisplay();
    drawAutotrackViewer(firstTrack);
    display.display();
  }
  resetEncoders();
}
//selects OR deselects a point, instead of toggling its selection state
void selectDataPoint(uint16_t index, bool state){
    //if the point is in bounds
  if(index>=0 && index<seqEnd){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size();i++){
      if(index == autotrackData[activeAutotrack].selectedPoints[i]){
        alreadySelected = true;
        //if it's already selected, and you want it to be selected, return here
        if(state){
          return;
        }
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(autotrackData[activeAutotrack].selectedPoints[i]);
      }
    }
    //if it was already selected, and you don't want it to be, delete it
    if(!state){
      autotrackData[activeAutotrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(autotrackData[activeAutotrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(autotrackData[activeAutotrack].selectedPoints[i]>index){
          autotrackData[activeAutotrack].selectedPoints.insert(autotrackData[activeAutotrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      autotrackData[activeAutotrack].selectedPoints.push_back(index);
    }
    //if it's the first in the vector, no need to sort it
    else if(autotrackData[activeAutotrack].selectedPoints.size() == 0){
      autotrackData[activeAutotrack].selectedPoints.push_back(index);
    }
  }
}

void deleteDataPoint(uint16_t point, uint8_t track){
  //if the track and point are in bounds
  if(track<autotrackData.size() && point <= seqEnd){
    //deleting data stored there
    autotrackData[track].data[point] = 255;
    //deselecting it (maybe it's better to not deselect it? idk)
    selectDataPoint(point, false);
  }
}
void regenDT(uint8_t which){
  //if there are any selected points, ONLY regen between those points
  if(autotrackData[which].selectedPoints.size()>1){
    regenDT(which,autotrackData[which].selectedPoints[0],autotrackData[which].selectedPoints.back());
  }
  //regens entire DT
  else
    regenDT(which,0,seqEnd);
}

//"regenerates" autotrack from it's period, amplitude, phase, and yPosition, between start and end
void regenDT(uint8_t which,uint16_t start, uint16_t end){
  switch(autotrackData[which].type){
    //default
    case 0:
      for(uint16_t point = start; point<=end; point++){
        autotrackData[which].data[point] = 64;
      }
      break;
    //sinewave
    case 1:
      {
        for(uint16_t point = start; point <= end; point++){
          //this one is crazy, and cool, but busted and idk why
          // int pt = autotrackData[which].amplitude*sin( (autotrackData[which].period/(2*PI)) * (point+autotrackData[which].phase) ) + autotrackData[which].yPos;
          //format is A * sin((2pi/Per)*(x+Phase))+yPos
          int pt = autotrackData[which].amplitude*sin(float(point+autotrackData[which].phase)*float(2*PI)/float(autotrackData[which].period)) + autotrackData[which].yPos;
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          autotrackData[which].data[point] = pt;
        }
      }
      break;
    //square wave
    case 2:
      {
        for(uint16_t point = start; point <= end; point++){
          int pt;
          //if you're less than half a period, it's high. Else, it's low
          if((point+autotrackData[which].phase)%autotrackData[which].period<autotrackData[which].period/2)
            pt = autotrackData[which].yPos+autotrackData[which].amplitude;
          else
            pt = autotrackData[which].yPos-autotrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          autotrackData[which].data[point] = pt;
        }
      }
      break;
    //saw
    case 3:
      {
        //the slope of each saw tooth is rise/run = amplitude/period
        float slope = float(autotrackData[which].amplitude*2)/float(autotrackData[which].period);
        for(uint16_t point = start; point < end; point++){
          // y = (x+offset)%period * m + yOffset
          int pt = ((point+autotrackData[which].phase)%autotrackData[which].period)*slope+autotrackData[which].yPos-autotrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          autotrackData[which].data[point] = pt;
        }
      }
      break;
    //triangle
    case 4:
      {
        float slope = float(autotrackData[which].amplitude*2)/float(autotrackData[which].period/2);
        int pt;
        for(uint16_t point = start; point < end; point++){
          //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
          if((point+autotrackData[which].phase)%autotrackData[which].period>(autotrackData[which].period/2))
            pt = ((point+autotrackData[which].phase)%autotrackData[which].period) * (-slope) + autotrackData[which].yPos + 3*autotrackData[activeAutotrack].amplitude;
          else
            pt = ((point+autotrackData[which].phase)%autotrackData[which].period) * slope + autotrackData[which].yPos - autotrackData[activeAutotrack].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          autotrackData[which].data[point] = pt;
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
        // uint16_t nextPoint = pt+autotrackData[which].period/randomDiv;
        uint16_t nextPoint = pt+autotrackData[which].period;
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
  if(end>=autotrackData[which].data.size())
    end = autotrackData[which].data.size()-1;
  //0 is linear interp, 1 is elliptical interp up, 2 is down, 3 is sin, 4 is sq, 5 is saw, 6 is triangle
  //for 0-2, generate random value for the second point
  //for 3-6, generate random amplitude
  //phase should be set so that it always lines up! how do we do this
  //phase = start, yPos = autotrack[start]
  uint8_t randomFunction = random(0,7);

  uint16_t oldPhase = autotrackData[which].phase;
  uint8_t oldYPos = autotrackData[which].yPos;
  uint8_t oldAmp = autotrackData[which].amplitude;
  if(randomFunction<=2){
    autotrackData[which].data[end] = random(0,autotrackData[which].amplitude+1);
  }
  else if(randomFunction>=3){
    autotrackData[which].phase = start;
    autotrackData[which].yPos = autotrackData[which].data[start];
    // autotrackData[which].amplitude = random(-autotrackData[which].amplitude,autotrackData[which].amplitude+1);
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
      autotrackData[which].type = 1;
      regenDT(start,end,which);
      break;
    //sq
    case 4:
      autotrackData[which].type = 2;
      regenDT(start,end,which);
      break;
    //saw
    case 5:
      autotrackData[which].type = 3;
      regenDT(start,end,which);
      break;
    //triangle
    case 6:
      autotrackData[which].type = 4;
      regenDT(start,end,which);
      break;
  }
  //reset it back to "random" type
  autotrackData[which].type = 5;
  //reset the curve values
  autotrackData[which].phase = oldPhase;
  autotrackData[which].yPos = oldYPos;
  autotrackData[which].amplitude = oldAmp;
}

bool autotrackEditingControls(uint8_t *interpType, bool *settingRecInput){
  //zoom/subdiv zoom
  while(counterA != 0 && !recordingToAutotrack){
    //changing zoom
    if(!shift){
      if(counterA >= 1){
          zoom(true);
        }
      if(counterA <= -1){
        zoom(false);
      }
    }
    else{
      if(counterA >= 1 && autotrackData[activeAutotrack].channel<16){
        autotrackData[activeAutotrack].channel++;
      }
      else if(counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(autotrackData[activeAutotrack].parameterType == 2 && autotrackData[activeAutotrack].channel>0)
          autotrackData[activeAutotrack].channel--;
        else if(autotrackData[activeAutotrack].channel>1)
          autotrackData[activeAutotrack].channel--;
      }
    }
    counterA += counterA<0?1:-1;
  }
  while(counterB != 0 && !recordingToAutotrack){
    if(!shift){   
      if(counterB >= 1){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(counterB <= -1){
        changeSubDivInt(false);
      }
    }
    else{
      if(counterB >= 1){
        autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,true,autotrackData[activeAutotrack].parameterType);
      }
      else if(counterB <= -1){
        autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,false,autotrackData[activeAutotrack].parameterType);
      }
    }
    counterB += counterB<0?1:-1;
  }

  if(!recordingToAutotrack || autotrackData[activeAutotrack].recordFrom != 3){
    if (utils.itsbeen(50)) {
      //moving
      if (controls.joystickX == 1 && shift) {
        moveAutotrackCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && shift) {
        moveAutotrackCursor(1);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveAutotrackCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveAutotrackCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (controls.joystickX == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveAutotrackCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveAutotrackCursor(subDivInt);
          lastTime = millis();
        }
      }
    }
  }
  if(!recordingToAutotrack || autotrackData[activeAutotrack].recordFrom != 4){
    if(utils.itsbeen(50)){
      if(controls.joystickY != 0 && !n){
        if(controls.joystickY == -1){
          if(shift){
            changeDataPoint(1);
            lastTime = millis();
          }
          else{
            changeDataPoint(8);
            lastTime = millis();
          }
        }
        if(controls.joystickY == 1){
          if(shift){
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
    //deleting data point(s)
    if(del){
      lastTime = millis();
      //deleting all selected points
      if(shift){
        while(autotrackData[activeAutotrack].selectedPoints.size()>0){
          deleteDataPoint(autotrackData[activeAutotrack].selectedPoints[0],activeAutotrack);
        }
      }
      else
        deleteDataPoint(cursorPos,activeAutotrack);
    }
    if(play){
      if(shift || recording){
        lastTime = millis();
        recordingToAutotrack = true;
        recentCC.val = 0;
        counterA = 64;
        counterB = 64;
        toggleRecordingMode(true);
      }
      else{
        togglePlayMode();
        lastTime = millis();
      }
    }
    if(n){
      //set data track type back to 0
      if(shift){
        autotrackData[activeAutotrack].type = 0;
        lastTime = millis();
      }
      //change type of autotrack
      else{
        autotrackData[activeAutotrack].type++;
        if(autotrackData[activeAutotrack].type>2)
          autotrackData[activeAutotrack].type = 0;
        regenDT(activeAutotrack);
        lastTime = millis();
      }
    }
    if(loop_Press){
      if(shift){
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
    if(sel){
      //deselect all data points
      if(shift){
        vector<uint16_t> temp;
        autotrackData[activeAutotrack].selectedPoints.swap(temp);
      }
      //select a data point
      else
        selectDataPoint(cursorPos);
      lastTime = millis();
    }
    if(controls.A()){
      lastTime = millis();
      int8_t param = selectCCParam_autotrack(autotrackData[activeAutotrack].parameterType);
      if(param != 255)
        autotrackData[activeAutotrack].control = param;
    }
    if(controls.B()){
      lastTime = millis();
      toggleTriplets();
    }
    if(menu_Press){
      lastTime = millis();
      menu_Press = false;
      return false;
    }
    if(copy_Press){
      *settingRecInput = true;
      //by default, set it to midi input
      autotrackData[activeAutotrack].recordFrom = 0;
    }
    else{
      *settingRecInput = false;
    }
  }
  return true;
}

//x is change period, y is change amplitude, shift x is change phase, shift y is vertically translate
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput){
  //zoom/subdiv zoom (this is the same to the normal DT editor)
  while(counterA != 0){
    //changing zoom
    if(!shift){
      if(counterA >= 1){
          zoom(true);
        }
      if(counterA <= -1){
        zoom(false);
      }
    }
    else{
      if(counterA >= 1 && autotrackData[activeAutotrack].channel<16){
        autotrackData[activeAutotrack].channel++;
      }
      else if(counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(autotrackData[activeAutotrack].parameterType == 2 && autotrackData[activeAutotrack].channel>0)
          autotrackData[activeAutotrack].channel--;
        else if(autotrackData[activeAutotrack].channel>1)
          autotrackData[activeAutotrack].channel--;
      }
    }
    counterA += counterA<0?1:-1;
  }
  while(counterB != 0){
    if(!shift){   
      if(counterB >= 1 && !shift){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(counterB <= -1 && !shift){
        changeSubDivInt(false);
      }
    }
    else{
      if(counterB >= 1){
        autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,true,autotrackData[activeAutotrack].parameterType);
      }
      else if(counterB <= -1){
        autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,false,autotrackData[activeAutotrack].parameterType);
      }
    }
    counterB += counterB<0?1:-1;
  }
  if(utils.itsbeen(200)){
    //toggling DT type
    if(n){
      if(shift){
        lastTime = millis();
        autotrackData[activeAutotrack].type = 0;
      }
      else{
        lastTime = millis();
        autotrackData[activeAutotrack].type++;
        if(autotrackData[activeAutotrack].type > 5)
          autotrackData[activeAutotrack].type = 0;
        regenDT(activeAutotrack);
      }
    }
    if(play){
      togglePlayMode();
      lastTime = millis();
    }
    if(menu_Press){
      lastTime = millis();
      menu_Press = false;
      return false;
    }
    if(controls.A()){
      lastTime = millis();
      uint8_t param = selectCCParam_autotrack(autotrackData[activeAutotrack].parameterType);
      if(param != 255)
        autotrackData[activeAutotrack].control = param;
    }
    if(controls.B()){
      lastTime = millis();
      toggleTriplets();
    }
    //swapping modes
    if(loop_Press){
      (*translation) = !(*translation);
      lastTime = millis();
    }
    //Toggling to 'listen for controls' mode
    if(copy_Press){
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
      if(shift && utils.itsbeen(25)){
        if(controls.joystickX == -1){
          if(autotrackData[activeAutotrack].phase == 0)
            autotrackData[activeAutotrack].phase = autotrackData[activeAutotrack].period;
          autotrackData[activeAutotrack].phase--;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          autotrackData[activeAutotrack].phase++;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
        //if the phase = period, then phase resets to 0
        if(autotrackData[activeAutotrack].phase == autotrackData[activeAutotrack].period)
          autotrackData[activeAutotrack].phase = 0;
      }
      //moving by subdivInt
      //phase will always be mod by period (so it can never be more/less than per)
      //storing it in "temp" here so it doesn't underflow/overflow
      else if(!shift && utils.itsbeen(100)){
        if(controls.joystickX == -1){
          int16_t temp = autotrackData[activeAutotrack].phase;
          if(temp<subDivInt)
            temp = abs(autotrackData[activeAutotrack].period-subDivInt);
          else
            temp-=subDivInt;
          autotrackData[activeAutotrack].phase=temp%autotrackData[activeAutotrack].period;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          autotrackData[activeAutotrack].phase = (autotrackData[activeAutotrack].phase+subDivInt)%autotrackData[activeAutotrack].period;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
      }
    }
    //changing vertical translation
    if(controls.joystickY != 0 && utils.itsbeen(25)){
      if(controls.joystickY == -1 && autotrackData[activeAutotrack].yPos<127){
        autotrackData[activeAutotrack].yPos++;
        regenDT(activeAutotrack);
        lastTime = millis();
      }
      else if(controls.joystickY == 1 && autotrackData[activeAutotrack].yPos>0){
        autotrackData[activeAutotrack].yPos--;
        regenDT(activeAutotrack);
        lastTime = millis();
      }
    }
  }
  //transformation
  else{
    //changing amplitude
    if(controls.joystickY != 0 && utils.itsbeen(25)){
      if(controls.joystickY == -1 && autotrackData[activeAutotrack].amplitude<127){
        autotrackData[activeAutotrack].amplitude++;
        regenDT(activeAutotrack);
        lastTime = millis();
      }
      else if(controls.joystickY == 1 && autotrackData[activeAutotrack].amplitude>-127){
        autotrackData[activeAutotrack].amplitude--;
        regenDT(activeAutotrack);
        lastTime = millis();
      }
    }
    //changing by 1
    if(shift && utils.itsbeen(25)){
      //changing period
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          autotrackData[activeAutotrack].period++;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && autotrackData[activeAutotrack].period>1){
          autotrackData[activeAutotrack].period--;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
      }
    }
    else if(utils.itsbeen(100)){
      if(controls.joystickX != 0){
        if(controls.joystickX == -1){
          if(autotrackData[activeAutotrack].period == 1)
            autotrackData[activeAutotrack].period = 0;
          autotrackData[activeAutotrack].period+=subDivInt;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && autotrackData[activeAutotrack].period>subDivInt){
          autotrackData[activeAutotrack].period-=subDivInt;
          regenDT(activeAutotrack);
          lastTime = millis();
        }
      }
    }
  }
  return true;
}


//using slope-intercept eq
void linearInterpolate(uint16_t start, uint16_t end, uint8_t id){
  if(start>=0 && end <= autotrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(autotrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          autotrackData[id].data[start] = 63;
          break;
        }
        else if(autotrackData[id].data[i] != 255){
          autotrackData[id].data[start] = autotrackData[id].data[i];
          break;
        }
      }
    }
    if(autotrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          autotrackData[id].data[end] = 63;
          break;
        }
        else if(autotrackData[id].data[i] != 255){
          autotrackData[id].data[end] = autotrackData[id].data[i];
          break;
        }
      }
    }

    float m = float(autotrackData[id].data[start] - autotrackData[id].data[end])/float(start-end);
    uint8_t b = autotrackData[id].data[start];
    for(uint16_t i = start; i<=end; i++){
      autotrackData[id].data[i] = m*(i-start)+b;
    }
  }
}

//using ellipse equation
void ellipticalInterpolate(uint16_t start, uint16_t end, uint8_t id, bool up){
  //if they're the same point (which would be a bug) or the same value, return since you won't need to interpolate
  if(start == end || autotrackData[id].data[start] == autotrackData[id].data[end])
    return;
  //prepping autotrack data
  //if it's in bounds
  if(start>=0 && end <= autotrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(autotrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          autotrackData[id].data[start] = 63;
          break;
        }
        else if(autotrackData[id].data[i] != 255){
          autotrackData[id].data[start] = autotrackData[id].data[i];
          break;
        }
      }
    }
    if(autotrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          autotrackData[id].data[end] = 63;
          break;
        }
        else if(autotrackData[id].data[i] != 255){
          autotrackData[id].data[end] = autotrackData[id].data[i];
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
    uint8_t b = abs(autotrackData[id].data[end]-autotrackData[id].data[start]);

    //to store where (0,0) is
    uint8_t yOffset;
    uint16_t xOffset;

    //if "up", the yOffset is the highest of the two points since y will always be negative
    if(up){
      if(autotrackData[id].data[end]<autotrackData[id].data[start]){
        yOffset = autotrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = autotrackData[id].data[end];
        xOffset = start;
      }
    }
    else{
      if(autotrackData[id].data[end]>autotrackData[id].data[start]){
        yOffset = autotrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = autotrackData[id].data[end];
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
      autotrackData[id].data[i] = pt + yOffset;
    }
  }
}

//this one smooths between selected points
void smoothSelectedPoints(uint8_t type){
  if(autotrackData[activeAutotrack].selectedPoints.size()>=2){
    switch(type){
      //linear
      case 0:
        //move through each selected point
        for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size()-1; i++){
          linearInterpolate(autotrackData[activeAutotrack].selectedPoints[i],autotrackData[activeAutotrack].selectedPoints[i+1],activeAutotrack);
        }
        return;
      //elliptical UP
      case 1:
        //move through each selected point
        for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(autotrackData[activeAutotrack].selectedPoints[i],autotrackData[activeAutotrack].selectedPoints[i+1],activeAutotrack,true);
        }
        return;
      //elliptical DOWN
      case 2:
        //move through each selected point
        for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(autotrackData[activeAutotrack].selectedPoints[i],autotrackData[activeAutotrack].selectedPoints[i+1],activeAutotrack,false);
        }
        return;
    }
  }
}

bool isDataPointSelected(uint16_t index){
  if(index>=0 && index<seqEnd){
    //check each point to see if it matches index
    for(uint16_t i = 0; i<autotrackData[activeAutotrack].selectedPoints.size(); i++){
      if(index == autotrackData[activeAutotrack].selectedPoints[i]){
        return true;
      }
    }
  }
  //if it's out of bounds, or wasn't found in the selection, return false
  return false;
}

uint8_t getLastDTVal(uint16_t point, uint8_t id){
  //if params are in bounds
  if(id<autotrackData.size() && point<=seqEnd){
    //step back thru DT and look for a non-255 value
    for(uint16_t i = point; i>=0; i--){
      if(autotrackData[id].data[i] != 255){
        return autotrackData[id].data[i];
      }
    }
  }
  //if something goes wrong, return 64
  return 64;
}

void drawAutotrackInputIcon(uint8_t x1, uint8_t y1, uint8_t id){
  switch(autotrackData[id].recordFrom){
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
  if(autotrackData.size()>0){
    for(uint16_t step = viewStart; step<viewEnd; step++){
      if(step<seqEnd){
        //measure bars
        if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>2) {
          graphics.drawDottedLineV((step-viewStart)*scale+32,0,64,4);
        }
        if(!(step%96)){
          graphics.drawDottedLineV2((step-viewStart)*scale+32,0,64,6);
        }
        //gets the on-screen position of each point
        uint8_t yPos;
        //if it's a 255, display the last val
        if(autotrackData[activeAutotrack].data[step] == 255){
          yPos = (64-float(getLastDTVal(step,activeAutotrack))/float(127)*64);
        }
        else{
          yPos = (64-float(autotrackData[activeAutotrack].data[step])/float(127)*64);
        }
        if(step == viewStart){
          display.drawPixel(32+(step-viewStart)*scale,(yPos),SSD1306_WHITE);
        }
        else{
          display.drawLine(32+(step-viewStart)*scale,(yPos),32+(step-1-viewStart)*scale,(64-float(getLastDTVal(step-1,activeAutotrack))/float(127)*64),SSD1306_WHITE);
        }

        //drawing cursor info if in default editor mode
        if(autotrackData[activeAutotrack].type == 0){
          //drawing selection arrow
          if(isDataPointSelected(step)){
            display.fillCircle(32+(step-viewStart)*scale,yPos,2,SSD1306_WHITE);
            if(autotrackData[activeAutotrack].data[step]>=64)//bottom arrow if data point is high
              graphics.drawArrow(32+(step-viewStart)*scale,yPos+4+((millis()/200)%2),2,2,true);
            else//top arrow if data point is low
              graphics.drawArrow(32+(step-viewStart)*scale,yPos-6+((millis()/200)%2),2,3 ,true);
          }
          //drawing cursor
          if(step == cursorPos){
            display.drawFastVLine(32+(cursorPos-viewStart)*scale,0,64,SSD1306_WHITE);
            display.drawFastVLine(32+(cursorPos-viewStart)*scale+1,0,64,SSD1306_WHITE);
            display.drawCircle(32+(cursorPos-viewStart)*scale,yPos,3+((millis()/400)%2),SSD1306_WHITE);
          }
        }

        //playhead
        if(playing && ((autotrackData[activeAutotrack].triggerSource == global && step == playheadPos) || (autotrackData[activeAutotrack].triggerSource != global && step == autotrackData[activeAutotrack].playheadPos)))
          display.drawRoundRect(32+(step-viewStart)*scale,0,3,screenHeight,3,SSD1306_WHITE);
        
        //rechead
        if(recording && recheadPos == step)
          display.drawRoundRect(32+(step-viewStart)*scale,0,3,screenHeight,3,SSD1306_WHITE);

        //loop points
        if(step == loopData[activeLoop].start){
          graphics.drawArrow(32+(step-viewStart)*scale-1+((millis()/200)%2),59,4,0,true);
        }
        else if(step == loopData[activeLoop].end){
          graphics.drawArrow(32+(step-viewStart)*scale+2-((millis()/200)%2),59,4,1,false);
        }
      }
    }
    //middle line
    //if it's just the typical node curve, draw midline at middle of the screen (32)
    if(autotrackData[activeAutotrack].type == 0)
      graphics.drawDottedLineH(32,128,32,3);
    else{
      graphics.drawDottedLineH(32,128,64-(autotrackData[activeAutotrack].yPos-32),3);
    }
    //drawing curve icon
    if(autotrackData[activeAutotrack].type == 0)
      drawNodeEditingIcon(12,14,interpType,animOffset,true);
    else
      drawCurveIcon(12,14,autotrackData[activeAutotrack].type,animOffset);
    animOffset++;
    animOffset%=18;

    //menu info
    if(shift){
      printParam(16,2,autotrackData[activeAutotrack].control,true,autotrackData[activeAutotrack].parameterType,true);
      graphics.printChannel(16,12,autotrackData[activeAutotrack].channel,true);
      display.drawChar(3,2+sin(millis()/50),0x0E,1,0,1);
    }
    else{
      //title
      if(!playing){
        printSmall(0,0,"trk",SSD1306_WHITE);
        printSmall(6-stringify(activeAutotrack+1).length()*2,7,stringify(activeAutotrack+1),SSD1306_WHITE);
      }
      printSmall(15,0,"CC"+stringify(autotrackData[activeAutotrack].control),SSD1306_WHITE);
      printSmall(15,7,"CH"+stringify(autotrackData[activeAutotrack].channel),SSD1306_WHITE);
    }
    //drawing curve info, for non-defaults
    if(autotrackData[activeAutotrack].type != 0){
      if(translation){
        printSmall(12,26,"@:"+stringify(autotrackData[activeAutotrack].phase),SSD1306_WHITE);
        printSmall(12,33,"Y:"+stringify(autotrackData[activeAutotrack].yPos),SSD1306_WHITE);
      }
      else{
        printSmall(12,26,"A:"+stringify(autotrackData[activeAutotrack].amplitude),SSD1306_WHITE);
        printSmall(12,33,"P:"+stringify(autotrackData[activeAutotrack].period),SSD1306_WHITE);
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
      if(autotrackData[activeAutotrack].data[autotrackData[activeAutotrack].triggerSource==global?playheadPos:autotrackData[activeAutotrack].playheadPos] == 255)
        val = getLastDTVal(autotrackData[activeAutotrack].triggerSource==global?playheadPos:autotrackData[activeAutotrack].playheadPos,activeAutotrack);
      else
        val = autotrackData[activeAutotrack].data[autotrackData[activeAutotrack].triggerSource==global?playheadPos:autotrackData[activeAutotrack].playheadPos];
    }
    else{
      if(autotrackData[activeAutotrack].data[cursorPos] == 255)
        val = getLastDTVal(cursorPos,activeAutotrack);
      else
        val = autotrackData[activeAutotrack].data[cursorPos];
    }
    //filling the bar graphgraph
    height = abs(float(val)/float(127)*float(barHeight-4));
    if(height>0)
      display.fillRect(2,62-height,7,height,2);//inverted color!
    
    //drawing sent data
    display.setRotation(1);
    if(playing){
      printSmall(barHeight/2-stringify(getLastDTVal(autotrackData[activeAutotrack].triggerSource==global?playheadPos:autotrackData[activeAutotrack].playheadPos,activeAutotrack)).length()*2,3,stringify(getLastDTVal(autotrackData[activeAutotrack].triggerSource==global?playheadPos:autotrackData[activeAutotrack].playheadPos,activeAutotrack)),2);
    }
    else{
      printSmall(barHeight/2-stringify(getLastDTVal(cursorPos,activeAutotrack)).length()*2,3,stringify(getLastDTVal(cursorPos,activeAutotrack)),2);
    }
    display.setRotation(UPRIGHT);

    //drawing source icon
    printSmall(15,42,"src",1);
    graphics.drawArrow(20,52+sin(millis()/400),2,3,false);
    if(!settingRecInput || (millis()%40>20))
      drawAutotrackInputIcon(17,55,activeAutotrack);
  }
  else{
    printSmall(50,29,"no data, kid",SSD1306_WHITE);
  }

}

bool autotrackViewerControls(){
  //changing cc
  while(counterB != 0){
    if(counterB >= 1 && autotrackData[activeAutotrack].control<127){
      autotrackData[activeAutotrack].control++;
      }
    if(counterB <= -1 && autotrackData[activeAutotrack].control>0){
      autotrackData[activeAutotrack].control--;
    }
    counterB += counterB<0?1:-1;
  }
  //changing ch
  while(counterA != 0){
    if(counterA >= 1 && autotrackData[activeAutotrack].channel<16){
      autotrackData[activeAutotrack].channel++;
      }
    if(counterA <= -1 && autotrackData[activeAutotrack].channel>1){
      autotrackData[activeAutotrack].channel--;
    }
    counterA += counterA<0?1:-1;
  }
  //sending CC message when shift is held
  if(shift && autotrackData.size()>activeAutotrack){
    autotrackData[activeAutotrack].play(0);
  }
  //scrolling up and down
  if(utils.itsbeen(100)){
    if(controls.joystickY != 0){
      if(autotrackData.size() != 0){
        if(controls.joystickY == -1 && activeAutotrack>0){
          activeAutotrack--;
          lastTime = millis();
        }
        if(controls.joystickY == 1 && activeAutotrack<autotrackData.size()-1){
          activeAutotrack++;
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.joystickX != 0){
      //changing control #
      if(!shift){
        if(controls.joystickX == -1){
          autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,true,autotrackData[activeAutotrack].parameterType);
          lastTime = millis();
        }
        else if(controls.joystickX == 1){
          autotrackData[activeAutotrack].control = moveToNextCCParam(autotrackData[activeAutotrack].control,false,autotrackData[activeAutotrack].parameterType);
          lastTime = millis();
        }
      }
      //changing channel #
      else{
        if(controls.joystickX == -1 && autotrackData[activeAutotrack].channel<16){
          autotrackData[activeAutotrack].channel++;
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && autotrackData[activeAutotrack].channel>1){
          autotrackData[activeAutotrack].channel--;
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    //selecting a autotrack
    if(sel){
      if(autotrackData.size()>0){
        lastTime = millis();
        autotrackEditor();
      }
    }
    if(n){
      if(autotrackData.size() == 0)
        createAutotrack(0);
      else
        createAutotrack(0,(autotrackData[autotrackData.size()-1].control>=127)?0:(autotrackData[autotrackData.size()-1].control+1));
      lastTime = millis();
    }
    if(del){
      if(shift && activeAutotrack<autotrackData.size()){
        autotrackData[activeAutotrack].isActive = !autotrackData[activeAutotrack].isActive;
        lastTime = millis();
      }
      else{
        deleteAutotrack(activeAutotrack);
        lastTime = millis();
      }
    }
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(play){
      if(shift && activeAutotrack<autotrackData.size()){
        autotrackData[activeAutotrack].isPrimed = !autotrackData[activeAutotrack].isPrimed;
        lastTime = millis();
      }
      else{
        togglePlayMode();
        lastTime = millis();
      }
    }
    if(copy_Press){
      dupeAutotrack(activeAutotrack);
      lastTime = millis();
    }
    if(controls.A()){
      controls.setA(false);
      uint8_t param = selectCCParam_autotrack(autotrackData[activeAutotrack].parameterType);
      if(param != 255)
        autotrackData[activeAutotrack].control = param;
      lastTime = millis();
    }
    if(loop_Press && activeAutotrack<autotrackData.size()){
      lastTime = millis();
      setAutotrackTrigger(activeAutotrack);
    }
  }
  return true;
}

bool atLeastOneActiveAutotrack(){
  for(uint8_t i = 0; i<autotrackData.size(); i++){
    if(autotrackData[i].isActive)
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
  display.setRotation(UPRIGHT);
  //drawing tracks
  if(autotrackData.size()>0){
    animOffset++;
    animOffset%=18;

    for(uint8_t track = 0; track<5; track++){
      //if there's data for this track (if it exists)
      if(track+firstTrack<autotrackData.size()){
        //if it's the activeAutotrack
        if(track+firstTrack == activeAutotrack){
          //track box
          display.drawRoundRect(9,currentHeight,119,23,3,1);
          //track data
          display.fillRoundRect(sideMenu-2,currentHeight+4,screenWidth-sideMenu,height+6,2,SSD1306_BLACK);
          display.drawRoundRect(sideMenu-3,currentHeight+3,screenWidth-sideMenu,height+8,2,SSD1306_WHITE);

          drawMiniDT(sideMenu-3,currentHeight+4,height+6,activeAutotrack);
          //cc/ch indicator
          display.drawBitmap(11,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          display.drawBitmap(11,currentHeight+9,ch_tiny,6,3,SSD1306_WHITE);
          printSmall(17,currentHeight+2,stringify(autotrackData[activeAutotrack].control),1);
          printSmall(18,currentHeight+8,stringify(autotrackData[track+firstTrack].channel),SSD1306_WHITE);
          //curve icon/send icon while shifting
          drawCurveIcon(12,currentHeight+14,autotrackData[activeAutotrack].type,animOffset);
          
          //if shifting, draw note icon
          if(shift)
            display.drawChar(120-(firstTrack?8:1)+sin(millis()/50),0,0x0E,1,0,1);    
          //if not, print track number        
          else
            print7SegSmall(screenWidth-countDigits_byte(activeAutotrack)*4-(firstTrack?8:1),0,stringify(activeAutotrack),1);

          String trigger;
          switch(autotrackData[activeAutotrack].triggerSource){
            case global:
              trigger = "global";
              break;
            case TriggerSource::track:
              trigger = "trck: "+stringify(autotrackData[activeAutotrack].triggerTarget);
              break;
              case channel:
              trigger = "chnl: "+stringify(autotrackData[activeAutotrack].triggerTarget);
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
          uint8_t x1 = 26 - countDigits_byte(autotrackData[firstTrack+track].control)*4;

          display.drawBitmap(x1,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          printSmall(x1+6,currentHeight+2,stringify(autotrackData[firstTrack+track].control),1);
          
          display.fillRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_BLACK);
          display.drawRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_WHITE);
          if(autotrackData[track+firstTrack].isActive)
            printParam_centered(track+firstTrack,sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,autotrackData[track+firstTrack].control,false,autotrackData[track+firstTrack].parameterType,false);
          else
            printSmall_centered(sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,"[inactive]",1);
          currentHeight+=height-1;
        }
      }
    }

    //data track label
    String p = getCCParam(autotrackData[activeAutotrack].control,autotrackData[activeAutotrack].channel,autotrackData[activeAutotrack].parameterType);
    printCursive(10,0,p,1);

    //drawing 'more tracks' indicators
    if(firstTrack>0){
      graphics.drawArrow(124,1+sin(millis()/75),2,2,true);
    }
    if(firstTrack+4<autotrackData.size()-1){
      graphics.drawArrow(124,63-sin(millis()/75),2,3,true);
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
  if((playing || recording) && atLeastOneActiveAutotrack())
    // switch(millis()/500%4){
    switch((playing?autotrackData[activeAutotrack].playheadPos:recheadPos)/24%4){
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
  else{
    display.drawBitmap(x1+9,y1+2,robo1_mask,12,17,0);
    display.drawBitmap(x1+8,y1,robo[(millis()/500)%2],13,24,1);
  }
}

void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which){
  if(autotrackData[which].isActive){
    float yScale = float(height-1)/float(127);
    // float xScale = scale;
    //i starts at start, goes from start to the end of the screen
    float sc = float(96)/float(loopData[activeLoop].end-loopData[activeLoop].start);
    //ends at viewEnd-1 because it draws lines 2 points at a time
    for(uint16_t i = loopData[activeLoop].start; i<loopData[activeLoop].end; i++){
      if(i<autotrackData[which].data.size()){
        if(i >= loopData[activeLoop].end - 1){
          display.drawLine(x1+(i-loopData[activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i,which)),x1+(loopData[activeLoop].end-loopData[activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i,which)),SSD1306_WHITE);
          break;
        }
        else
          display.drawLine(x1+(i-loopData[activeLoop].start)*sc, y1+yScale*(127-getLastDTVal(i,which)),x1+(i+1-loopData[activeLoop].start)*sc-1, y1+yScale*(127-getLastDTVal(i+1,which)),SSD1306_WHITE);
        if(playing){
          if((autotrackData[which].triggerSource == global && i == playheadPos) || (autotrackData[which].triggerSource != global && i == autotrackData[which].playheadPos)){
            display.drawFastVLine(x1+(i-loopData[activeLoop].start)*sc,y1,height,SSD1306_WHITE);
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
  if(autotrackData[which].isPrimed){
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
