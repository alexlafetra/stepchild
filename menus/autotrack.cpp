bool isDataPointSelected(uint16_t);
bool dataTrackEditingControls(uint8_t *interpType);
bool dataTrackCurveEditingControls(bool* translation);
bool dataTrackViewerControls();
void drawDataTrackViewer(uint8_t firstTrack);
void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which);
void drawDataTrackEditor(uint8_t y,uint8_t interpType,bool translation);

//these could all be refactored into methods of the autotrack type... jus sayin
//OR you could pass a pointer to the DT as an arg, it would make it easier to use for other people
void regenDT(uint8_t which,uint16_t start, uint16_t end);
void regenDT(uint8_t which);
void randomInterp(uint16_t start, uint16_t end, uint8_t which);
void linearInterpolate(uint16_t start, uint16_t end, uint8_t id);
void ellipticalInterpolate(uint16_t start, uint16_t end, uint8_t id, bool up);
void smoothSelectedPoints(uint8_t type);

void createDataPoint(){
  dataTrackData[activeDataTrack].data[cursorPos] = 64;
}

void changeDataPoint(int8_t amount){
  if(dataTrackData[activeDataTrack].data[cursorPos] == 255){
    //if it's a blank point, go back until you hit a real point
    for(uint16_t point = cursorPos; point>=0; point--){
      //if there's a real point
      if(dataTrackData[activeDataTrack].data[point] != 255){
        dataTrackData[activeDataTrack].data[cursorPos] = dataTrackData[activeDataTrack].data[point];
        break;
      }
      //if it never hit a real point, just make one
      if(point == 0){
        dataTrackData[activeDataTrack].data[cursorPos] = 64;
        break;
      }
    }
    
  }
  int16_t newVal = dataTrackData[activeDataTrack].data[cursorPos]+amount;
  if(newVal<0)
    newVal = 0;
  else if(newVal > 127)
    newVal = 127;
  
  dataTrackData[activeDataTrack].data[cursorPos] = newVal;
}

void createDataTrack(uint8_t type){
  dataTrack newData(type);
  dataTrackData.push_back(newData);
}

void createDataTrack(uint8_t type, uint8_t cont){
  dataTrack newData(type);
  newData.control = cont;
  dataTrackData.push_back(newData);
}

void createDataTrack(uint8_t cont, uint8_t chan, uint8_t isOn, vector<uint8_t> points,uint8_t pType){
  dataTrack newData(0);
  newData.control = cont;
  newData.channel = chan;
  newData.isActive = isOn;
  newData.data.swap(points);
  newData.parameterType = pType;
  dataTrackData.push_back(newData);
}

void dupeDataTrack(uint8_t which){
  //if the track exists
  if(which<dataTrackData.size()){
    vector<dataTrack> temp;
    for(uint8_t track = 0; track<dataTrackData.size(); track++){
      temp.push_back(dataTrackData[track]);
      if(track == which)
        temp.push_back(dataTrackData[track]);
    }
    temp.swap(dataTrackData);
  }
}

void deleteDataTrack(uint8_t track){
  //only delete tracks if there'll be one left over
  vector<dataTrack> temp;
  for(uint8_t i = 0; i<dataTrackData.size(); i++){
    if(i != track)
      temp.push_back(dataTrackData[i]);
  }
  temp.swap(dataTrackData);
  if(activeDataTrack>=dataTrackData.size())
    activeDataTrack = dataTrackData.size()-1;
  if(dataTrackData.size() == 0){
    activeDataTrack = 0;
  }
}

void selectDataPoint(uint16_t index){
  //if the point is in bounds
  if(index>=0 && index<seqEnd){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size();i++){
      if(index == dataTrackData[activeDataTrack].selectedPoints[i]){
        alreadySelected = true;
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(dataTrackData[activeDataTrack].selectedPoints[i]);
      }
    }
    //if it was already selected, swap vectors with temp to delete it from the selection
    if(alreadySelected){
      dataTrackData[activeDataTrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(dataTrackData[activeDataTrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(dataTrackData[activeDataTrack].selectedPoints[i]>index){
          dataTrackData[activeDataTrack].selectedPoints.insert(dataTrackData[activeDataTrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      dataTrackData[activeDataTrack].selectedPoints.push_back(index);
    }
    //if it's the first vector
    else if(dataTrackData[activeDataTrack].selectedPoints.size() == 0){
      dataTrackData[activeDataTrack].selectedPoints.push_back(index);
    }
  }
}

//for editing a single dataTrack
void dataTrackEditor(){
  //type of curve to draw! 0 is linear, 1 is x^2
  uint8_t interpolatorType = 0;
  bool translation = false;
  while(true){
    readButtons();
    joyRead();
    //this is done just to pass the showingInfo param 
    if(dataTrackData[activeDataTrack].type == 0){
      if(!dataTrackEditingControls(&interpolatorType)){
        return;
      }
    }
    else{
      if(!dataTrackCurveEditingControls(&translation)){
        return;
      }
    }
    if(shift){
      sendMIDICC(dataTrackData[activeDataTrack].control,127,dataTrackData[activeDataTrack].channel);      
    }
    display.clearDisplay();
    drawDataTrackEditor(0,interpolatorType,translation);
    display.display();
  }
}

void dataTrackViewer(){
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
    joyRead();
    if(!dataTrackViewerControls()){
      return;
    }
    //handling menu bounds after activeDataTrack is changed
    if(activeDataTrack<firstTrack)
      firstTrack = activeDataTrack;
    else if(activeDataTrack>firstTrack+4)
      firstTrack = activeDataTrack-4;
    
    display.clearDisplay();
    drawDataTrackViewer(firstTrack);
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
    for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size();i++){
      if(index == dataTrackData[activeDataTrack].selectedPoints[i]){
        alreadySelected = true;
        //if it's already selected, and you want it to be selected, return here
        if(state){
          return;
        }
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(dataTrackData[activeDataTrack].selectedPoints[i]);
      }
    }
    //if it was already selected, and you don't want it to be, delete it
    if(!state){
      dataTrackData[activeDataTrack].selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(dataTrackData[activeDataTrack].selectedPoints.size()>0){
      for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(dataTrackData[activeDataTrack].selectedPoints[i]>index){
          dataTrackData[activeDataTrack].selectedPoints.insert(dataTrackData[activeDataTrack].selectedPoints.begin()+i,index);
          return;
        }
      }
      dataTrackData[activeDataTrack].selectedPoints.push_back(index);
    }
    //if it's the first in the vector, no need to sort it
    else if(dataTrackData[activeDataTrack].selectedPoints.size() == 0){
      dataTrackData[activeDataTrack].selectedPoints.push_back(index);
    }
  }
}

void deleteDataPoint(uint16_t point, uint8_t track){
  //if the track and point are in bounds
  if(track<dataTrackData.size() && point <= seqEnd){
    //deleting data stored there
    dataTrackData[track].data[point] = 255;
    //deselecting it (maybe it's better to not deselect it? idk)
    selectDataPoint(point, false);
  }
}
void regenDT(uint8_t which){
  //if there are any selected points, ONLY regen between those points
  if(dataTrackData[which].selectedPoints.size()>1){
    regenDT(which,dataTrackData[which].selectedPoints[0],dataTrackData[which].selectedPoints.back());
  }
  //regens entire DT
  else
    regenDT(which,0,seqEnd);
}

//"regenerates" datatrack from it's period, amplitude, phase, and yPosition, between start and end
void regenDT(uint8_t which,uint16_t start, uint16_t end){
  switch(dataTrackData[which].type){
    //default
    case 0:
      for(uint16_t point = start; point<=end; point++){
        dataTrackData[which].data[point] = 64;
      }
      break;
    //sinewave
    case 1:
      {
        for(uint16_t point = start; point <= end; point++){
          //this one is crazy, and cool, but busted and idk why
          // int pt = dataTrackData[which].amplitude*sin( (dataTrackData[which].period/(2*PI)) * (point+dataTrackData[which].phase) ) + dataTrackData[which].yPos;
          //format is A * sin((2pi/Per)*(x+Phase))+yPos
          int pt = dataTrackData[which].amplitude*sin(float(point+dataTrackData[which].phase)*float(2*PI)/float(dataTrackData[which].period)) + dataTrackData[which].yPos;
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          dataTrackData[which].data[point] = pt;
        }
      }
      break;
    //square wave
    case 2:
      {
        for(uint16_t point = start; point <= end; point++){
          int pt;
          //if you're less than half a period, it's high. Else, it's low
          if((point+dataTrackData[which].phase)%dataTrackData[which].period<dataTrackData[which].period/2)
            pt = dataTrackData[which].yPos+dataTrackData[which].amplitude;
          else
            pt = dataTrackData[which].yPos-dataTrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          dataTrackData[which].data[point] = pt;
        }
      }
      break;
    //saw
    case 3:
      {
        //the slope of each saw tooth is rise/run = amplitude/period
        float slope = float(dataTrackData[which].amplitude*2)/float(dataTrackData[which].period);
        for(uint16_t point = start; point < end; point++){
          // y = (x+offset)%period * m + yOffset
          int pt = ((point+dataTrackData[which].phase)%dataTrackData[which].period)*slope+dataTrackData[which].yPos-dataTrackData[which].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          dataTrackData[which].data[point] = pt;
        }
      }
      break;
    //triangle
    case 4:
      {
        float slope = float(dataTrackData[which].amplitude*2)/float(dataTrackData[which].period/2);
        int pt;
        for(uint16_t point = start; point < end; point++){
          //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
          if((point+dataTrackData[which].phase)%dataTrackData[which].period>(dataTrackData[which].period/2))
            pt = ((point+dataTrackData[which].phase)%dataTrackData[which].period) * (-slope) + dataTrackData[which].yPos + 3*dataTrackData[activeDataTrack].amplitude;
          else
            pt = ((point+dataTrackData[which].phase)%dataTrackData[which].period) * slope + dataTrackData[which].yPos - dataTrackData[activeDataTrack].amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          dataTrackData[which].data[point] = pt;
        }
      }
      break;

    //random patterns
    case 5:
      uint16_t pt = start;
      //always seed it with the dataTrack id
      srand(which);
      while(pt<end){
        //get a random number to divide the period by (it'll always be a factor of the period)
        // int8_t randomDiv = random(1,5);
        // uint16_t nextPoint = pt+dataTrackData[which].period/randomDiv;
        uint16_t nextPoint = pt+dataTrackData[which].period;
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
  if(end>=dataTrackData[which].data.size())
    end = dataTrackData[which].data.size()-1;
  //0 is linear interp, 1 is elliptical interp up, 2 is down, 3 is sin, 4 is sq, 5 is saw, 6 is triangle
  //for 0-2, generate random value for the second point
  //for 3-6, generate random amplitude
  //phase should be set so that it always lines up! how do we do this
  //phase = start, yPos = dataTrack[start]
  uint8_t randomFunction = random(0,7);
  Serial.println("made it here");
  Serial.println("function is: "+stringify(randomFunction));
  Serial.flush();
  uint16_t oldPhase = dataTrackData[which].phase;
  uint8_t oldYPos = dataTrackData[which].yPos;
  uint8_t oldAmp = dataTrackData[which].amplitude;
  if(randomFunction<=2){
    dataTrackData[which].data[end] = random(0,dataTrackData[which].amplitude+1);
  }
  else if(randomFunction>=3){
    dataTrackData[which].phase = start;
    dataTrackData[which].yPos = dataTrackData[which].data[start];
    // dataTrackData[which].amplitude = random(-dataTrackData[which].amplitude,dataTrackData[which].amplitude+1);
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
      dataTrackData[which].type = 1;
      regenDT(start,end,which);
      break;
    //sq
    case 4:
      dataTrackData[which].type = 2;
      regenDT(start,end,which);
      break;
    //saw
    case 5:
      dataTrackData[which].type = 3;
      regenDT(start,end,which);
      break;
    //triangle
    case 6:
      dataTrackData[which].type = 4;
      regenDT(start,end,which);
      break;
  }
  //reset it back to "random" type
  dataTrackData[which].type = 5;
  //reset the curve values
  dataTrackData[which].phase = oldPhase;
  dataTrackData[which].yPos = oldYPos;
  dataTrackData[which].amplitude = oldAmp;
}

bool dataTrackEditingControls(uint8_t *interpType){
  //zoom/subdiv zoom
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
      if(counterA >= 1 && dataTrackData[activeDataTrack].channel<16){
        dataTrackData[activeDataTrack].channel++;
      }
      else if(counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(dataTrackData[activeDataTrack].parameterType == 2 && dataTrackData[activeDataTrack].channel>0)
          dataTrackData[activeDataTrack].channel--;
        else if(dataTrackData[activeDataTrack].channel>1)
          dataTrackData[activeDataTrack].channel--;
      }
    }
    counterA += counterA<0?1:-1;
  }
  while(counterB != 0){
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
        dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,true,dataTrackData[activeDataTrack].parameterType);
      }
      else if(counterB <= -1){
        dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,false,dataTrackData[activeDataTrack].parameterType);
      }
    }
    counterB += counterB<0?1:-1;
  }
  if (itsbeen(50)) {
    //moving
    if (x == 1 && shift) {
      moveDataTrackCursor(-1);
      lastTime = millis();
    }
    if (x == -1 && shift) {
      moveDataTrackCursor(1);
      lastTime = millis();
    }
  }
  if (itsbeen(100)) {
    if (x == 1 && !shift) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(cursorPos%subDivInt){
        moveDataTrackCursor(-cursorPos%subDivInt);
        lastTime = millis();
      }
      else{
        moveDataTrackCursor(-subDivInt);
        lastTime = millis();
      }
    }
    if (x == -1 && !shift) {
      if(cursorPos%subDivInt){
        moveDataTrackCursor(subDivInt-cursorPos%subDivInt);
        lastTime = millis();
      }
      else{
        moveDataTrackCursor(subDivInt);
        lastTime = millis();
      }
    }
  }
  if(itsbeen(50)){
    if(y != 0 && !n){
      if(y == -1){
        if(shift){
          changeDataPoint(1);
          lastTime = millis();
        }
        else{
          changeDataPoint(8);
          lastTime = millis();
        }
      }
      if(y == 1){
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
  if(itsbeen(200)){
    //deleting data point(s)
    if(del){
      lastTime = millis();
      //deleting all selected points
      if(shift){
        while(dataTrackData[activeDataTrack].selectedPoints.size()>0){
          deleteDataPoint(dataTrackData[activeDataTrack].selectedPoints[0],activeDataTrack);
        }
      }
      else
        deleteDataPoint(cursorPos,activeDataTrack);
    }
    if(play){
      togglePlayMode();
      lastTime = millis();
    }
    if(n){
      //set data track type back to 0
      if(shift){
        dataTrackData[activeDataTrack].type = 0;
        lastTime = millis();
      }
      //change type of dataTrack
      else{
        dataTrackData[activeDataTrack].type++;
        if(dataTrackData[activeDataTrack].type>2)
          dataTrackData[activeDataTrack].type = 0;
        regenDT(activeDataTrack);
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
      selectDataPoint(cursorPos);

      //deselect all data points
      if(shift && !isDataPointSelected(cursorPos)){
        vector<uint16_t> temp;
        dataTrackData[activeDataTrack].selectedPoints.swap(temp);
      }
      //select all data points
      else if(shift && isDataPointSelected(cursorPos)){
        for(int i = 0; i<seqEnd; i++){
          if(dataTrackData[activeDataTrack].data[i] != 255)
            selectDataPoint(i,true);
        }
      }
      lastTime = millis();
    }
    if(track_Press){
      lastTime = millis();
      int8_t param = selectCCParam_dataTrack(dataTrackData[activeDataTrack].parameterType);
      if(param != 255)
        dataTrackData[activeDataTrack].control = param;
    }
    if(note_Press){
      lastTime = millis();
      toggleTriplets();
    }
    if(menu_Press){
      lastTime = millis();
      menu_Press = false;
      return false;
    }
  }
  return true;
}

//x is change period, y is change amplitude, shift x is change phase, shift y is vertically translate
bool dataTrackCurveEditingControls(bool* translation){
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
      if(counterA >= 1 && dataTrackData[activeDataTrack].channel<16){
        dataTrackData[activeDataTrack].channel++;
      }
      else if(counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(dataTrackData[activeDataTrack].parameterType == 2 && dataTrackData[activeDataTrack].channel>0)
          dataTrackData[activeDataTrack].channel--;
        else if(dataTrackData[activeDataTrack].channel>1)
          dataTrackData[activeDataTrack].channel--;
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
        dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,true,dataTrackData[activeDataTrack].parameterType);
      }
      else if(counterB <= -1){
        dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,false,dataTrackData[activeDataTrack].parameterType);
      }
    }
    counterB += counterB<0?1:-1;
  }
  if(itsbeen(200)){
    //toggling DT type
    if(n){
      if(shift){
        lastTime = millis();
        dataTrackData[activeDataTrack].type = 0;
      }
      else{
        lastTime = millis();
        dataTrackData[activeDataTrack].type++;
        if(dataTrackData[activeDataTrack].type > 5)
          dataTrackData[activeDataTrack].type = 0;
        regenDT(activeDataTrack);
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
    if(track_Press){
      lastTime = millis();
      uint8_t param = selectCCParam_dataTrack(dataTrackData[activeDataTrack].parameterType);
      if(param != 255)
        dataTrackData[activeDataTrack].control = param;
    }
    if(note_Press){
      lastTime = millis();
      toggleTriplets();
    }
    //swapping modes
    if(loop_Press){
      (*translation) = !(*translation);
      lastTime = millis();
    }
  }
  //translations (and changing Amp)
  //translation
  if(*translation){
    if(x != 0){
      //moving by 1
      if(shift && itsbeen(25)){
        if(x == -1){
          if(dataTrackData[activeDataTrack].phase == 0)
            dataTrackData[activeDataTrack].phase = dataTrackData[activeDataTrack].period;
          dataTrackData[activeDataTrack].phase--;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
        else if(x == 1){
          dataTrackData[activeDataTrack].phase++;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
        //if the phase = period, then phase resets to 0
        if(dataTrackData[activeDataTrack].phase == dataTrackData[activeDataTrack].period)
          dataTrackData[activeDataTrack].phase = 0;
      }
      //moving by subdivInt
      //phase will always be mod by period (so it can never be more/less than per)
      //storing it in "temp" here so it doesn't underflow/overflow
      else if(!shift && itsbeen(100)){
        if(x == -1){
          int16_t temp = dataTrackData[activeDataTrack].phase;
          if(temp<subDivInt)
            temp = abs(dataTrackData[activeDataTrack].period-subDivInt);
          else
            temp-=subDivInt;
          dataTrackData[activeDataTrack].phase=temp%dataTrackData[activeDataTrack].period;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
        else if(x == 1){
          dataTrackData[activeDataTrack].phase = (dataTrackData[activeDataTrack].phase+subDivInt)%dataTrackData[activeDataTrack].period;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
      }
    }
    //changing vertical translation
    if(y != 0 && itsbeen(25)){
      if(y == -1 && dataTrackData[activeDataTrack].yPos<127){
        dataTrackData[activeDataTrack].yPos++;
        regenDT(activeDataTrack);
        lastTime = millis();
      }
      else if(y == 1 && dataTrackData[activeDataTrack].yPos>0){
        dataTrackData[activeDataTrack].yPos--;
        regenDT(activeDataTrack);
        lastTime = millis();
      }
    }
  }
  //transformation
  else{
    //changing amplitude
    if(y != 0 && itsbeen(25)){
      if(y == -1 && dataTrackData[activeDataTrack].amplitude<127){
        dataTrackData[activeDataTrack].amplitude++;
        regenDT(activeDataTrack);
        lastTime = millis();
      }
      else if(y == 1 && dataTrackData[activeDataTrack].amplitude>-127){
        dataTrackData[activeDataTrack].amplitude--;
        regenDT(activeDataTrack);
        lastTime = millis();
      }
    }
    //changing by 1
    if(shift && itsbeen(25)){
      //changing period
      if(x != 0){
        if(x == -1){
          dataTrackData[activeDataTrack].period++;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
        else if(x == 1 && dataTrackData[activeDataTrack].period>1){
          dataTrackData[activeDataTrack].period--;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
      }
    }
    else if(itsbeen(100)){
      if(x != 0){
        if(x == -1){
          if(dataTrackData[activeDataTrack].period == 1)
            dataTrackData[activeDataTrack].period = 0;
          dataTrackData[activeDataTrack].period+=subDivInt;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
        else if(x == 1 && dataTrackData[activeDataTrack].period>subDivInt){
          dataTrackData[activeDataTrack].period-=subDivInt;
          regenDT(activeDataTrack);
          lastTime = millis();
        }
      }
    }
  }
  return true;
}


//using slope-intercept eq
void linearInterpolate(uint16_t start, uint16_t end, uint8_t id){
  if(start>=0 && end <= dataTrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(dataTrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          dataTrackData[id].data[start] = 63;
          break;
        }
        else if(dataTrackData[id].data[i] != 255){
          dataTrackData[id].data[start] = dataTrackData[id].data[i];
          break;
        }
      }
    }
    if(dataTrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          dataTrackData[id].data[end] = 63;
          break;
        }
        else if(dataTrackData[id].data[i] != 255){
          dataTrackData[id].data[end] = dataTrackData[id].data[i];
          break;
        }
      }
    }

    float m = float(dataTrackData[id].data[start] - dataTrackData[id].data[end])/float(start-end);
    uint8_t b = dataTrackData[id].data[start];
    for(uint16_t i = start; i<=end; i++){
      dataTrackData[id].data[i] = m*(i-start)+b;
    }
  }
}

//using ellipse equation
void ellipticalInterpolate(uint16_t start, uint16_t end, uint8_t id, bool up){
  //if they're the same point (which would be a bug) or the same value, return since you won't need to interpolate
  if(start == end || dataTrackData[id].data[start] == dataTrackData[id].data[end])
    return;
  //prepping dataTrack data
  //if it's in bounds
  if(start>=0 && end <= dataTrackData[id].data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(dataTrackData[id].data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          dataTrackData[id].data[start] = 63;
          break;
        }
        else if(dataTrackData[id].data[i] != 255){
          dataTrackData[id].data[start] = dataTrackData[id].data[i];
          break;
        }
      }
    }
    if(dataTrackData[id].data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          dataTrackData[id].data[end] = 63;
          break;
        }
        else if(dataTrackData[id].data[i] != 255){
          dataTrackData[id].data[end] = dataTrackData[id].data[i];
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
    uint8_t b = abs(dataTrackData[id].data[end]-dataTrackData[id].data[start]);

    //to store where (0,0) is
    uint8_t yOffset;
    uint16_t xOffset;

    //if "up", the yOffset is the highest of the two points since y will always be negative
    if(up){
      if(dataTrackData[id].data[end]<dataTrackData[id].data[start]){
        yOffset = dataTrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = dataTrackData[id].data[end];
        xOffset = start;
      }
    }
    else{
      if(dataTrackData[id].data[end]>dataTrackData[id].data[start]){
        yOffset = dataTrackData[id].data[start];
        xOffset = end;
      }
      else{
        yOffset = dataTrackData[id].data[end];
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
      dataTrackData[id].data[i] = pt + yOffset;
    }
  }
}

//this one smooths between selected points
void smoothSelectedPoints(uint8_t type){
  if(dataTrackData[activeDataTrack].selectedPoints.size()>=2){
    switch(type){
      //linear
      case 0:
        //move through each selected point
        for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size()-1; i++){
          linearInterpolate(dataTrackData[activeDataTrack].selectedPoints[i],dataTrackData[activeDataTrack].selectedPoints[i+1],activeDataTrack);
        }
        return;
      //elliptical UP
      case 1:
        //move through each selected point
        for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(dataTrackData[activeDataTrack].selectedPoints[i],dataTrackData[activeDataTrack].selectedPoints[i+1],activeDataTrack,true);
        }
        return;
      //elliptical DOWN
      case 2:
        //move through each selected point
        for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size()-1; i++){
          ellipticalInterpolate(dataTrackData[activeDataTrack].selectedPoints[i],dataTrackData[activeDataTrack].selectedPoints[i+1],activeDataTrack,false);
        }
        return;
    }
  }
}

bool isDataPointSelected(uint16_t index){
  if(index>=0 && index<seqEnd){
    //check each point to see if it matches index
    for(uint16_t i = 0; i<dataTrackData[activeDataTrack].selectedPoints.size(); i++){
      if(index == dataTrackData[activeDataTrack].selectedPoints[i]){
        return true;
      }
    }
  }
  //if it's out of bounds, or wasn't found in the selection, return false
  return false;
}

uint8_t getLastDTVal(uint16_t point, uint8_t id){
  //if params are in bounds
  if(id<dataTrackData.size() && point<=seqEnd){
    //step back thru DT and look for a non-255 value
    for(uint16_t i = point; i>=0; i--){
      if(dataTrackData[id].data[i] != 255){
        return dataTrackData[id].data[i];
      }
    }
  }
  //if something goes wrong, return 64
  return 64;
}

void drawDataTrackEditor(uint8_t y,uint8_t interpType,bool translation){
  //bounding box and 3D-effect
  display.drawRect(32,y,112,64,SSD1306_WHITE);
  display.drawLine(32,y,29,y+3,SSD1306_WHITE);
  display.drawLine(29,y+3,29,64,SSD1306_WHITE);

  //drawing all the points
  if(dataTrackData.size()>0){
    for(uint16_t step = viewStart; step<viewEnd; step++){
      if(step<seqEnd){
        //measure bars
        if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>2) {
          drawDottedLineV((step-viewStart)*scale+32,0,64,4);
        }
        if(!(step%96)){
          drawDottedLineV2((step-viewStart)*scale+32,0,64,6);
        }
        //gets the on-screen position of each point
        uint8_t yPos;
        //if it's a 255, display the last val
        if(dataTrackData[activeDataTrack].data[step] == 255){
          yPos = (64-float(getLastDTVal(step,activeDataTrack))/float(127)*64);
        }
        else{
          yPos = (64-float(dataTrackData[activeDataTrack].data[step])/float(127)*64);
        }
        if(step == viewStart){
          display.drawPixel(32+(step-viewStart)*scale,(yPos),SSD1306_WHITE);
        }
        else{
          display.drawLine(32+(step-viewStart)*scale,(yPos),32+(step-1-viewStart)*scale,(64-float(getLastDTVal(step-1,activeDataTrack))/float(127)*64),SSD1306_WHITE);
        }

        //drawing cursor info if in default editor mode
        if(dataTrackData[activeDataTrack].type == 0){
          //drawing selection arrow
          if(isDataPointSelected(step)){
            display.fillCircle(32+(step-viewStart)*scale,yPos,2,SSD1306_WHITE);
            if(dataTrackData[activeDataTrack].data[step]>=64)//bottom arrow if data point is high
              drawArrow(32+(step-viewStart)*scale,yPos+4+sin(millis()/100),2,2,true);
            else//top arrow if data point is low
              drawArrow(32+(step-viewStart)*scale,yPos-6+sin(millis()/100),2,3 ,true);
          }
          //drawing cursor
          if(step == cursorPos){
            display.drawFastVLine(32+(cursorPos-viewStart)*scale,0,64,SSD1306_WHITE);
            display.drawFastVLine(32+(cursorPos-viewStart)*scale+1,0,64,SSD1306_WHITE);
            display.drawCircle(32+(cursorPos-viewStart)*scale,yPos,3+sin(millis()/200),SSD1306_WHITE);
          }
        }

        //playhead
        if(playing && playheadPos == step)
          display.drawRoundRect(32+(step-viewStart)*scale,0,3,screenHeight,3,SSD1306_WHITE);
        
        //loop points
        if(step == loopData[activeLoop][0]){
          drawArrow(32+(step-viewStart)*scale-1+sin(millis()/100),59,4,0,true);
        }
        else if(step == loopData[activeLoop][1]){
          drawArrow(32+(step-viewStart)*scale+2+sin(millis()/100),59,4,1,false);
        }
      }
    }
    //middle line
    //if it's just the typical node curve, draw midline at middle of the screen (32)
    if(dataTrackData[activeDataTrack].type == 0)
      drawDottedLineH(32,128,32,3);
    else{
      drawDottedLineH(32,128,64-(dataTrackData[activeDataTrack].yPos-32),3);
    }
    //title
    if(!playing){
      printSmall(0,0,"trk",SSD1306_WHITE);
      printSmall(6-stringify(activeDataTrack+1).length()*2,7,stringify(activeDataTrack+1),SSD1306_WHITE);
    }
    //drawing curve icon
    if(dataTrackData[activeDataTrack].type == 0)
      drawNodeEditingIcon(12,14,interpType,animOffset,true);
    else
      drawCurveIcon(12,14,dataTrackData[activeDataTrack].type,animOffset);
    animOffset++;
    animOffset%=18;

    //menu info
    if(shift){
      printParam(16,2,dataTrackData[activeDataTrack].control,true,dataTrackData[activeDataTrack].parameterType,true);
      printChannel(16,12,dataTrackData[activeDataTrack].channel,true);
      display.drawChar(17,40+sin(millis()/50),0x0E,1,0,1);
    }
    else{
      printSmall(15,0,"CC"+stringify(dataTrackData[activeDataTrack].control),SSD1306_WHITE);
      printSmall(15,7,"CH"+stringify(dataTrackData[activeDataTrack].channel),SSD1306_WHITE);
    }
    //drawing curve info, for non-defaults
    if(dataTrackData[activeDataTrack].type != 0){
      if(translation){
        printSmall(12,26,"@:"+stringify(dataTrackData[activeDataTrack].phase),SSD1306_WHITE);
        printSmall(12,33,"Y:"+stringify(dataTrackData[activeDataTrack].yPos),SSD1306_WHITE);
      }
      else{
        printSmall(12,26,"A:"+stringify(dataTrackData[activeDataTrack].amplitude),SSD1306_WHITE);
        printSmall(12,33,"P:"+stringify(dataTrackData[activeDataTrack].period),SSD1306_WHITE);
      }
    }
    
    //play icon
    if(playing)
      display.fillTriangle(120+sin(millis()/100),9,120+sin(millis()/100),3,120+6+sin(millis()/100),6,SSD1306_WHITE);
    if(recording)
      display.fillCircle(124,6,3,SSD1306_WHITE);
    
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
      if(dataTrackData[activeDataTrack].data[playheadPos] == 255)
        val = getLastDTVal(playheadPos,activeDataTrack);
      else
        val = dataTrackData[activeDataTrack].data[playheadPos];
    }
    else{
      if(dataTrackData[activeDataTrack].data[cursorPos] == 255)
        val = getLastDTVal(cursorPos,activeDataTrack);
      else
        val = dataTrackData[activeDataTrack].data[cursorPos];
    }
    //filling the bar graphgraph
    height = abs(float(val)/float(127)*float(barHeight-4));
    if(height>0)
      display.fillRect(2,62-height,7,height,2);//inverted color!
    
    //drawing sent data
    display.setRotation(1);
    if(playing){
      printSmall(barHeight/2-stringify(getLastDTVal(playheadPos,activeDataTrack)).length()*2,3,stringify(getLastDTVal(playheadPos,activeDataTrack)),2);
    }
    else{
      printSmall(barHeight/2-stringify(getLastDTVal(cursorPos,activeDataTrack)).length()*2,3,stringify(getLastDTVal(cursorPos,activeDataTrack)),2);
    }
    display.setRotation(UPRIGHT);
  }
  else{
    printSmall(50,29,"no data, kid",SSD1306_WHITE);
  }
}

bool dataTrackViewerControls(){
  //changing cc
  while(counterB != 0){
    if(counterB >= 1 && dataTrackData[activeDataTrack].control<127){
      dataTrackData[activeDataTrack].control++;
      }
    if(counterB <= -1 && dataTrackData[activeDataTrack].control>0){
      dataTrackData[activeDataTrack].control--;
    }
    counterB += counterB<0?1:-1;
  }
  //changing ch
  while(counterA != 0){
    if(counterA >= 1 && dataTrackData[activeDataTrack].channel<16){
      dataTrackData[activeDataTrack].channel++;
      }
    if(counterA <= -1 && dataTrackData[activeDataTrack].channel>1){
      dataTrackData[activeDataTrack].channel--;
    }
    counterA += counterA<0?1:-1;
  }
  //sending CC message when shift is held
  if(shift){
    dataTrackData[activeDataTrack].play(0);
  }
  //scrolling up and down
  if(itsbeen(100)){
    if(y != 0){
      if(dataTrackData.size() != 0){
        if(y == -1 && activeDataTrack>0){
          activeDataTrack--;
          lastTime = millis();
        }
        if(y == 1 && activeDataTrack<dataTrackData.size()-1){
          activeDataTrack++;
          lastTime = millis();
        }
      }
    }
  }
  if(itsbeen(200)){
    if(x != 0){
      //changing control #
      if(!shift){
        if(x == -1){
          dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,true,dataTrackData[activeDataTrack].parameterType);
          lastTime = millis();
        }
        else if(x == 1){
          dataTrackData[activeDataTrack].control = moveToNextCCParam(dataTrackData[activeDataTrack].control,false,dataTrackData[activeDataTrack].parameterType);
          lastTime = millis();
        }
      }
      //changing channel #
      else{
        if(x == -1 && dataTrackData[activeDataTrack].channel<16){
          dataTrackData[activeDataTrack].channel++;
          lastTime = millis();
        }
        else if(x == 1 && dataTrackData[activeDataTrack].channel>1){
          dataTrackData[activeDataTrack].channel--;
          lastTime = millis();
        }
      }
    }
  }
  if(itsbeen(200)){
    //selecting a dataTrack
    if(sel){
      if(dataTrackData.size()>0){
        lastTime = millis();
        dataTrackEditor();
      }
    }
    if(n){
      if(dataTrackData.size() == 0)
        createDataTrack(0);
      else
        createDataTrack(0,(dataTrackData[dataTrackData.size()-1].control>=127)?0:(dataTrackData[dataTrackData.size()-1].control+1));
      lastTime = millis();
    }
    if(del){
      if(shift){
        dataTrackData[activeDataTrack].isActive = !dataTrackData[activeDataTrack].isActive;
        lastTime = millis();
      }
      else{
        deleteDataTrack(activeDataTrack);
        lastTime = millis();
      }
    }
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(play){
      togglePlayMode();
      lastTime = millis();
    }
    if(copy_Press){
      dupeDataTrack(activeDataTrack);
      lastTime = millis();
    }
    if(track_Press){
      track_Press = false;
      uint8_t param = selectCCParam_dataTrack(dataTrackData[activeDataTrack].parameterType);
      if(param != 255)
        dataTrackData[activeDataTrack].control = param;
      lastTime = millis();
    }
  }
  return true;
}

void drawDataTrackViewer(uint8_t firstTrack){
  //each track is 14 high, except the target which is 20
  const uint8_t height = 9;
  const uint8_t sideMenu = 32;

  //stores the current height of the track stack (so it can grow by different amounts)
  uint8_t currentHeight = 8;

  display.setRotation(1);
  printArp(2,0,"AUTOTRACKS",1);
  display.setRotation(UPRIGHT);

  //drawing tracks
  if(dataTrackData.size()>0){
    animOffset++;
    animOffset%=18;

    //play/rec
    if(playing){
      drawPlayIcon(64+sin(millis()/200),10);
    }
    for(uint8_t track = 0; track<5; track++){
      //if there's data for this track (if it exists)
      if(track+firstTrack<dataTrackData.size()){
        //if it's the activeDataTrack
        if(track+firstTrack == activeDataTrack){
          //track box
          display.drawRoundRect(9,currentHeight,119,23,3,1);
          //track data
          display.fillRoundRect(sideMenu-2,currentHeight+4,screenWidth-sideMenu,height+6,2,SSD1306_BLACK);
          display.drawRoundRect(sideMenu-3,currentHeight+3,screenWidth-sideMenu,height+8,2,SSD1306_WHITE);

          drawMiniDT(sideMenu-3,currentHeight+4,height+6,activeDataTrack);
          //cc/ch indicator
          display.drawBitmap(11,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          display.drawBitmap(11,currentHeight+9,ch_tiny,6,3,SSD1306_WHITE);
          printSmall(17,currentHeight+2,stringify(dataTrackData[activeDataTrack].control),1);
          printSmall(18,currentHeight+8,stringify(dataTrackData[track+firstTrack].channel),SSD1306_WHITE);
          //curve icon/send icon while shifting
          drawCurveIcon(12,currentHeight+14,dataTrackData[activeDataTrack].type,animOffset);
          
          //if shifting, draw note icon
          if(shift)
            display.drawChar(120-(firstTrack?8:1)+sin(millis()/50),0,0x0E,1,0,1);    
          //if not, print track number        
          else
            print7SegSmall(screenWidth-countDigits_byte(activeDataTrack)*4-(firstTrack?8:1),0,stringify(activeDataTrack),1);
          //arrow highlight
          drawArrow(110,currentHeight+2+cos(millis()/100),4,3,false);

          //increment height so the next track is drawn below
          currentHeight+=height+12;
        }
        //if it's a normal track (nonactive)
        else{
          //back up the cc label when there're more digits
          uint8_t x1 = 26 - countDigits_byte(dataTrackData[firstTrack+track].control)*4;

          display.drawBitmap(x1,currentHeight+3,cc_tiny,5,3,SSD1306_WHITE);
          printSmall(x1+6,currentHeight+2,stringify(dataTrackData[firstTrack+track].control),1);
          
          display.fillRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_BLACK);
          display.drawRect(sideMenu,currentHeight,screenWidth-sideMenu,height,SSD1306_WHITE);
          // drawMiniDT(sideMenu,currentHeight,height,track+firstTrack);
          if(dataTrackData[track+firstTrack].isActive)
            printParam_centered(track+firstTrack,sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,dataTrackData[track+firstTrack].control,false,dataTrackData[track+firstTrack].parameterType,false);
          else
            printSmall_centered(sideMenu+(screenWidth-sideMenu)/2,currentHeight+2,"[muted]",1);
          currentHeight+=height-1;
        }
      }
    }

    //data track label
    String p = getCCParam(dataTrackData[activeDataTrack].control,dataTrackData[activeDataTrack].channel,dataTrackData[activeDataTrack].parameterType);
    printCursive(10,0,p,1);

    //drawing 'more tracks' indicators
    if(firstTrack>0){
      drawArrow(124,1+sin(millis()/75),2,2,true);
    }
    if(firstTrack+4<dataTrackData.size()-1){
      drawArrow(124,63-sin(millis()/75),2,3,true);
    }
  }
  else{
    printSmall(11,3,"No tracks, kid",1);
    printSmall(10,12,"[n] to create",1);
    printSmall(11,21,"a new automation track",1);
    display.drawBitmap(110,16,robo[(millis()/25)%100>50],13,24,1);
  }
}

void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which){
  if(dataTrackData[which].isActive){
    float yScale = float(height-1)/float(127);
    // float xScale = scale;
    //i starts at start, goes from start to the end of the screen
    float sc = float(96)/float(loopData[activeLoop][1]-loopData[activeLoop][0]);
    //ends at viewEnd-1 because it draws lines 2 points at a time
    for(uint16_t i = loopData[activeLoop][0]; i<loopData[activeLoop][1]; i++){
      if(i<dataTrackData[which].data.size()){
        if(i == viewEnd - 1)
          display.drawLine(x1+(i-viewStart)*sc, y1+yScale*(127-getLastDTVal(i,which)),x1+(viewEnd-viewStart)*sc-1, y1+yScale*(127-getLastDTVal(i,which)),SSD1306_WHITE);
        else
          display.drawLine(x1+(i-viewStart)*sc, y1+yScale*(127-getLastDTVal(i,which)),x1+(i+1-viewStart)*sc, y1+yScale*(127-getLastDTVal(i+1,which)),SSD1306_WHITE);
        if(playing){
          if(i == playheadPos){
            display.drawFastVLine(x1+(i-viewStart)*sc,y1,height,SSD1306_WHITE);
          }
        }
        drawDottedLineH(x1,x1+(viewEnd-viewStart)*sc-1,y1+height/2,3);
      }
      else
        break;
    }
  }
  //if it's muted
  else{
    shadeRect(x1,y1,screenWidth-x1,height,5);
    display.fillRoundRect(x1+(screenWidth-x1)/2-13,y1+height/2-3,25,7,3,SSD1306_BLACK);
    printSmall(x1+(screenWidth-x1)/2-14,y1+height/2-2,"[muted]",SSD1306_WHITE);
  }
}
