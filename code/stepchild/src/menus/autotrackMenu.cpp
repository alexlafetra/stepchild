#include "Stepchild.h"
#include "ControlChange.h"
#include "graphics/bitmaps.h"
#include "StepchildGraphics.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;

using namespace std;

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
  if(stepchild.cursorPos==0 && moveAmount < 0){
    return;
  }
  if(moveAmount<0 && abs(moveAmount)>=stepchild.cursorPos)
    stepchild.cursorPos = 0;
  else
    stepchild.cursorPos += moveAmount;
  if(stepchild.cursorPos >= stepchild.sequenceLength) {
    stepchild.cursorPos = stepchild.sequenceLength-1;
  }
  if(stepchild.cursorPos<0){
    stepchild.cursorPos = 0;
  }
  //Move the view along with the cursor
  if(stepchild.cursorPos<stepchild.viewStart+stepchild.subDivision && stepchild.viewStart>0){
    stepchild.moveView(stepchild.cursorPos - (stepchild.viewStart+stepchild.subDivision));
  }
  else if(stepchild.cursorPos > stepchild.viewEnd-stepchild.subDivision && stepchild.viewEnd<stepchild.sequenceLength){
    stepchild.moveView(stepchild.cursorPos - (stepchild.viewEnd-stepchild.subDivision));
  }
}



void duplicateAutotrack(uint8_t which){
  //if the track exists
  if(which<stepchild.autotrackData.size()){
    vector<Autotrack> temp;
    for(uint8_t track = 0; track<stepchild.autotrackData.size(); track++){
      temp.push_back(stepchild.autotrackData[track]);
      if(track == which)
        temp.push_back(stepchild.autotrackData[track]);
    }
    temp.swap(stepchild.autotrackData);
  }
}

void deleteAutotrack(uint8_t track){
  //only del tracks if there'll be one left over
  vector<Autotrack> temp;
  for(uint8_t i = 0; i<stepchild.autotrackData.size(); i++){
    if(i != track)
      temp.push_back(stepchild.autotrackData[i]);
  }
  temp.swap(stepchild.autotrackData);
  if(stepchild.activeAutotrack>=stepchild.autotrackData.size())
    stepchild.activeAutotrack = stepchild.autotrackData.size()-1;
  if(stepchild.autotrackData.size() == 0){
    stepchild.activeAutotrack = 0;
  }
}

//draws an animated icon representing the type of interpolation algorithm being used
//0 = linear, 1 = elliptical up, 2 = elliptical down
void drawNodeEditingIcon(uint8_t xPos, uint8_t yPos, uint8_t type, uint8_t frame, bool text){
  const uint8_t width = 19;
  const uint8_t height = 10;
  stepchild.display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);

  //in this context, type means the kind of interpolation algorithm you're using
  switch(type){
    //linear
    case 0:
      {
      uint8_t p1[2] = {3,(uint8_t)(5+((millis()/200)%2))};
      uint8_t p2[2] = {15,(uint8_t)(5+cos(millis()/100))};
      stepchild.display.drawLine(xPos+p1[0]+2,yPos+p1[1],xPos+p2[0]-2,yPos+p2[1],SSD1306_WHITE);
      stepchild.display.drawCircle(xPos+p1[0],yPos+p1[1],1,SSD1306_WHITE);
      stepchild.display.drawCircle(xPos+p2[0],yPos+p2[1],1,SSD1306_WHITE);
      if(text)
        graphics.printSmall(xPos+width/2-6,yPos+height+2,"lin",SSD1306_WHITE);
      break;
      }
    //elliptical UP
    case 1:
      {
      uint8_t lastPoint = 0;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos+sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+1;
        if(point == 0){
          stepchild.display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        }
        else{
          stepchild.display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        }
        lastPoint = pt;
      }
      if(text)
        graphics.printSmall(xPos+width/2-4,yPos+height+2,"up",SSD1306_WHITE);
      break;
      }
    //elliptical DOWN
    case 2:
      {
      uint8_t lastPoint = 0;
      for(uint8_t point = 0; point<width; point++){
        uint8_t pt = sin(millis()/100+point)+yPos-sqrt(1-pow(point-width/2,2)/pow(width/2,2))*(height/2+2)+height-1;
        if(point == 0)
          stepchild.display.drawPixel(point+xPos,pt,SSD1306_WHITE);
        else
          stepchild.display.drawLine(point+xPos,pt,point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = pt;
      }
      if(text)
        graphics.printSmall(xPos+width/2-8,yPos+height+2,"down",SSD1306_WHITE);
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
  stepchild.display.fillRect(xPos,yPos,width,height,SSD1306_BLACK);
  switch(type){
    //custom curve (default)
    case LINEAR_CURVE:
      drawNodeEditingIcon(xPos,yPos,0,frame,false);
      break;
    //sin curve
    case SINEWAVE_CURVE:
      stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      //drawing each sinPoint
      for(uint8_t point = 0; point<width; point++){
        if(point == 0){
          stepchild.display.drawPixel(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),SSD1306_WHITE);
        }
        else
          stepchild.display.drawLine(point+xPos,yPos+height/2+(height/2-3)*sin(PI*float(point+frame)/float(width/2)),point-1+xPos,lastPoint,SSD1306_WHITE);
        lastPoint = yPos+height/2+(height/2-3)*sin(PI*(point+frame)/(width/2));
      }
      break;
    //square
    case SQUAREWAVE_CURVE:
      stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      for(uint8_t point = 0; point<width; point++){
        //if it's less than, it's low
        if((point+frame)%width<width/2)
          stepchild.display.drawPixel(point+xPos,yPos+height-3,SSD1306_WHITE);
        //if it's greater than half a period, it's high
        else if((point+frame)%width>width/2 && (point+frame)%width<width-1)
          stepchild.display.drawPixel(point+xPos,yPos+2,SSD1306_WHITE);
        //if it's equal to half a period, then it's a vert line
        else
          stepchild.display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
      }
      break;
    //saw
    case SAWTOOTH_CURVE:
      {
        stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        for(uint8_t point = 0; point<width; point++){
          uint8_t pt = (point+frame)%int(width/3);
          if(pt == 0)
            stepchild.display.drawFastVLine(point+xPos,yPos+2,height-4,SSD1306_WHITE);
          else
            stepchild.display.drawPixel(point+xPos,yPos+pt+2,SSD1306_WHITE);
        }
      }
      break;
    //triangle
    case TRIANGLE_CURVE:
      {
        stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        //slope is just 1/1
        uint8_t pt = 0;
        for(uint8_t point = 0; point<width; point++){
          if((point+frame)%(width/2)>=width/4)
            pt = -(point+frame)%(width/2)+2*height/2;
          else
            pt = (point+frame)%(width/2)+3;
          stepchild.display.drawPixel(xPos+point,yPos+pt+height/8,SSD1306_WHITE);
        }
      }
      break;
    //random
    case RANDOM_CURVE:
      {
        stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
        graphics.printSmall(xPos+4,yPos+3+((millis()/200)%2),"R",SSD1306_WHITE);
        graphics.printSmall(xPos+8,yPos+3+sin(millis()/100+1),"N",SSD1306_WHITE);
        graphics.printSmall(xPos+12,yPos+3+sin(millis()/100+2),"D",SSD1306_WHITE);
      }
      break;
    case NOISE_CURVE:
      stepchild.display.drawRect(xPos,yPos,width,height,SSD1306_WHITE);
      for(uint8_t i = xPos; i<xPos+width;i++){
        stepchild.display.drawPixel(i,random(yPos,yPos+height),1);
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
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(settingRecInput){   
      //once copy press is released, after at least one param has been selected
      if(!stepchild.buttons.COPY()){
        recInputSet = false;
        settingRecInput = false;
      }   
      //if any of these controls are pressed, then set the current data track param to it
      if(stepchild.buttons.counterA != 0){
        stepchild.buttons.counterA = 0;
        stepchild.autotrackData[whichAutotrack].recordFrom = ENCODER_A;
        recInputSet = true;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.counterB != 0){
        stepchild.buttons.counterB = 0;
        stepchild.autotrackData[whichAutotrack].recordFrom = ENCODER_B;
        recInputSet = true;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickX != 0){
        stepchild.autotrackData[whichAutotrack].recordFrom = JOY_X;
        recInputSet = true;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickY != 0){
        stepchild.autotrackData[whichAutotrack].recordFrom = JOY_Y;
        recInputSet = true;
        stepchild.lastTime = millis();
      }
    }
    else{
      //this is done just to pass the showingInfo param 
      if(stepchild.autotrackData[whichAutotrack].type == LINEAR_CURVE){
        if(!autotrackEditingControls(&interpolatorType,&settingRecInput,whichAutotrack)){
          return;
        }
      }
      else{
        if(!autotrackCurveEditingControls(&translation,&settingRecInput,whichAutotrack)){
          return;
        }
      }
      if(stepchild.buttons.SHIFT()){
        stepchild.midi.sendCC(stepchild.autotrackData[whichAutotrack].control,127,stepchild.autotrackData[whichAutotrack].channel);      
      }
    }
    stepchild.display.clearDisplay();
    drawAutotrackEditor(0,interpolatorType,translation,settingRecInput,whichAutotrack);
    stepchild.display.display();
  }
}

bool autotrackEditingControls(uint8_t *interpType, bool *settingRecInput, uint8_t& whichAutotrack){
  //zoom/subdiv zoom
  while(stepchild.buttons.counterA != 0 && !stepchild.recordingToAutotrack){
    //changing zoom
    if(!stepchild.buttons.SHIFT()){
      if(stepchild.buttons.counterA >= 1){
          stepchild.zoom(true);
        }
      if(stepchild.buttons.counterA <= -1){
        stepchild.zoom(false);
      }
    }
    else{
      if(stepchild.buttons.counterA >= 1 && stepchild.autotrackData[whichAutotrack].channel<16){
        stepchild.autotrackData[whichAutotrack].channel++;
      }
      else if(stepchild.buttons.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(stepchild.autotrackData[whichAutotrack].parameterType == 2 && stepchild.autotrackData[whichAutotrack].channel>0)
          stepchild.autotrackData[whichAutotrack].channel--;
        else if(stepchild.autotrackData[whichAutotrack].channel>1)
          stepchild.autotrackData[whichAutotrack].channel--;
      }
    }
    stepchild.buttons.countDownA();
  }
  while(stepchild.buttons.counterB != 0 && !stepchild.recordingToAutotrack){
    if(!stepchild.buttons.SHIFT()){   
      if(stepchild.buttons.counterB >= 1){
        stepchild.changeSubDivInt(true);
      }
      //changing subdivint
      if(stepchild.buttons.counterB <= -1){
        stepchild.changeSubDivInt(false);
      }
    }
    else{
      if(stepchild.buttons.counterB >= 1){
        stepchild.autotrackData[whichAutotrack].control = moveToNextCCParam(stepchild.autotrackData[whichAutotrack].control,true,stepchild.autotrackData[whichAutotrack].parameterType);
      }
      else if(stepchild.buttons.counterB <= -1){
        stepchild.autotrackData[whichAutotrack].control = moveToNextCCParam(stepchild.autotrackData[whichAutotrack].control,false,stepchild.autotrackData[whichAutotrack].parameterType);
      }
    }
    stepchild.buttons.countDownB();
  }

  if(!stepchild.recordingToAutotrack || stepchild.autotrackData[whichAutotrack].recordFrom != JOY_X){
    if (stepchild.itsbeen(50)) {
      //moving
      if (stepchild.buttons.LEFT() && stepchild.buttons.SHIFT()) {
        moveAutotrackCursor(-1);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.RIGHT() && stepchild.buttons.SHIFT()) {
        moveAutotrackCursor(1);
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.itsbeen(100)) {
      if (stepchild.buttons.LEFT() && !stepchild.buttons.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(stepchild.cursorPos%stepchild.subDivision){
          moveAutotrackCursor(-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          moveAutotrackCursor(-stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
      if (stepchild.buttons.RIGHT() && !stepchild.buttons.SHIFT()) {
        if(stepchild.cursorPos%stepchild.subDivision){
          moveAutotrackCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else{
          moveAutotrackCursor(stepchild.subDivision);
          stepchild.lastTime = millis();
        }
      }
    }
  }
  if(!stepchild.recordingToAutotrack || stepchild.autotrackData[whichAutotrack].recordFrom != JOY_Y){
    if(stepchild.itsbeen(50)){
      if(stepchild.buttons.DOWN()){
        if(stepchild.buttons.SHIFT()){
          stepchild.autotrackData[whichAutotrack].changeDataPoint(stepchild.cursorPos,1);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.autotrackData[whichAutotrack].changeDataPoint(stepchild.cursorPos,8);
          stepchild.lastTime = millis();
        }
      }
      if(stepchild.buttons.UP()){
        if(stepchild.buttons.SHIFT()){
          stepchild.autotrackData[whichAutotrack].changeDataPoint(stepchild.cursorPos,-1);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.autotrackData[whichAutotrack].changeDataPoint(stepchild.cursorPos,-8);
          stepchild.lastTime = millis();
        }
      }
    }
  }
  if(stepchild.itsbeen(200)){
    //stepchild.buttons.DELETE()eting data point(s)
    if(stepchild.buttons.DELETE()){
      stepchild.lastTime = millis();
      //stepchild.buttons.DELETE()eting all selected points
      if(stepchild.buttons.SHIFT()){
        while(stepchild.autotrackData[whichAutotrack].selectedPoints.size()>0){
          stepchild.autotrackData[whichAutotrack].deleteDataPoint(stepchild.autotrackData[whichAutotrack].selectedPoints[0]);
        }
      }
      else
        stepchild.autotrackData[whichAutotrack].deleteDataPoint(stepchild.cursorPos);
    }
    if(stepchild.buttons.PLAY()){
      if(stepchild.buttons.SHIFT() || stepchild.recording()){
        stepchild.lastTime = millis();
        stepchild.recordingToAutotrack = true;
        stepchild.recentCC.val = 0;
        stepchild.buttons.counterA = 64;
        stepchild.buttons.counterB = 64;
        stepchild.toggleRecording(true);
      }
      else{
        stepchild.togglePlay();
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.buttons.NEW()){
      //set data track type back to 0
      if(stepchild.buttons.SHIFT()){
        stepchild.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        stepchild.lastTime = millis();
      }
      //change type of autotrack
      else{
        stepchild.autotrackData[whichAutotrack].type++;
        if(stepchild.autotrackData[whichAutotrack].type>NOISE_CURVE)
          stepchild.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.buttons.LOOP()){
      if(stepchild.buttons.SHIFT()){
        (*interpType)++;
        if((*interpType)>2)
          *interpType = 0;
        stepchild.lastTime = millis();
      }
      else{
        stepchild.lastTime = millis();
        stepchild.autotrackData[whichAutotrack].smoothSelectedPoints(*interpType);
      }
    }
    if(stepchild.buttons.SELECT() ){
      //deselect all data points
      if(stepchild.buttons.SHIFT()){
        vector<uint16_t> temp;
        stepchild.autotrackData[whichAutotrack].selectedPoints.swap(temp);
      }
      //select a data point
      else
        stepchild.autotrackData[whichAutotrack].toggleSelectDataPoint(stepchild.cursorPos);
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.A()){
      stepchild.lastTime = millis();
      uint8_t param = selectCCParam_autotrack(stepchild.autotrackData[whichAutotrack].parameterType);
      if(param != 255)
        stepchild.autotrackData[whichAutotrack].control = param;
    }
    if(stepchild.buttons.B()){
      stepchild.lastTime = millis();
      stepchild.toggleTriplets();
    }
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      stepchild.buttons.setMENU(false) ;
      return false;
    }
    if(stepchild.buttons.COPY()){
      *settingRecInput = true;
      //by default, set it to midi input
      stepchild.autotrackData[whichAutotrack].recordFrom = EXTERNAL_MIDI;
    }
    else{
      *settingRecInput = false;
    }
  }
  return true;
}

//x is change period, y is change amplitude, stepchild.buttons.SHIFT() x is change phase, stepchild.buttons.SHIFT() y is vertically translate
bool autotrackCurveEditingControls(bool* translation, bool* settingRecInput, uint8_t& whichAutotrack){
  //zoom/subdiv zoom (this is the same to the normal DT editor)
  while(stepchild.buttons.counterA != 0){
    //changing zoom
    if(!stepchild.buttons.SHIFT()){
      if(stepchild.buttons.counterA >= 1){
          stepchild.zoom(true);
        }
      if(stepchild.buttons.counterA <= -1){
        stepchild.zoom(false);
      }
    }
    else{
      if(stepchild.buttons.counterA >= 1 && stepchild.autotrackData[whichAutotrack].channel<16){
        stepchild.autotrackData[whichAutotrack].channel++;
      }
      else if(stepchild.buttons.counterA <= -1){
        //if it's an internal parameter type, then it can go to channel 0
        if(stepchild.autotrackData[whichAutotrack].parameterType == 2 && stepchild.autotrackData[whichAutotrack].channel>0)
          stepchild.autotrackData[whichAutotrack].channel--;
        else if(stepchild.autotrackData[whichAutotrack].channel>1)
          stepchild.autotrackData[whichAutotrack].channel--;
      }
    }
    stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
  }
  while(stepchild.buttons.counterB != 0){
    if(!stepchild.buttons.SHIFT()){   
      if(stepchild.buttons.counterB >= 1 && !stepchild.buttons.SHIFT()){
        stepchild.changeSubDivInt(true);
      }
      //changing subdivint
      if(stepchild.buttons.counterB <= -1 && !stepchild.buttons.SHIFT()){
        stepchild.changeSubDivInt(false);
      }
    }
    else{
      if(stepchild.buttons.counterB >= 1){
        stepchild.autotrackData[whichAutotrack].control = moveToNextCCParam(stepchild.autotrackData[whichAutotrack].control,true,stepchild.autotrackData[whichAutotrack].parameterType);
      }
      else if(stepchild.buttons.counterB <= -1){
        stepchild.autotrackData[whichAutotrack].control = moveToNextCCParam(stepchild.autotrackData[whichAutotrack].control,false,stepchild.autotrackData[whichAutotrack].parameterType);
      }
    }
    stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
  }
  if(stepchild.itsbeen(200)){
    //toggling DT type
    if(stepchild.buttons.NEW()){
      if(stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        stepchild.autotrackData[whichAutotrack].type = LINEAR_CURVE;
      }
      else{
        stepchild.lastTime = millis();
        stepchild.autotrackData[whichAutotrack].type++;
        if(stepchild.autotrackData[whichAutotrack].type > NOISE_CURVE)
          stepchild.autotrackData[whichAutotrack].type = LINEAR_CURVE;
        stepchild.autotrackData[whichAutotrack].regenCurve();
      }
    }
    if(stepchild.buttons.PLAY()){
      stepchild.togglePlay();
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      stepchild.buttons.setMENU(false) ;
      return false;
    }
    if(stepchild.buttons.A()){
      stepchild.lastTime = millis();
      uint8_t param = selectCCParam_autotrack(stepchild.autotrackData[whichAutotrack].parameterType);
      if(param != 255)
        stepchild.autotrackData[whichAutotrack].control = param;
    }
    if(stepchild.buttons.B()){
      stepchild.lastTime = millis();
      stepchild.toggleTriplets();
    }
    //swapping modes
    if(stepchild.buttons.LOOP()){
      (*translation) = !(*translation);
      stepchild.lastTime = millis();
    }
    //Toggling to 'listen for controls' mode
    if(stepchild.buttons.COPY()){
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
    if(stepchild.buttons.SHIFT() && stepchild.itsbeen(25)){
      if(stepchild.buttons.LEFT()){
        if(stepchild.autotrackData[whichAutotrack].phase == 0)
          stepchild.autotrackData[whichAutotrack].phase = stepchild.autotrackData[whichAutotrack].period;
        stepchild.autotrackData[whichAutotrack].phase--;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.RIGHT()){
        stepchild.autotrackData[whichAutotrack].phase++;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
      //if the phase = period, then phase resets to 0
      if(stepchild.autotrackData[whichAutotrack].phase == stepchild.autotrackData[whichAutotrack].period)
        stepchild.autotrackData[whichAutotrack].phase = 0;
    }
    //moving by subdivInt
    //phase will always be mod by period (so it can never be more/less than per)
    //storing it in "temp" here so it doesn't underflow/overflow
    else if(!stepchild.buttons.SHIFT() && stepchild.itsbeen(100)){
      if(stepchild.buttons.LEFT()){
        int16_t temp = stepchild.autotrackData[whichAutotrack].phase;
        if(temp<stepchild.subDivision)
          temp = abs(stepchild.autotrackData[whichAutotrack].period-stepchild.subDivision);
        else
          temp-=stepchild.subDivision;
        stepchild.autotrackData[whichAutotrack].phase=temp%stepchild.autotrackData[whichAutotrack].period;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.RIGHT()){
        stepchild.autotrackData[whichAutotrack].phase = (stepchild.autotrackData[whichAutotrack].phase+stepchild.subDivision)%stepchild.autotrackData[whichAutotrack].period;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
    }
    //changing vertical translation
    if(stepchild.itsbeen(25)){
      if(stepchild.buttons.UP() && stepchild.autotrackData[whichAutotrack].yPos<127){
        stepchild.autotrackData[whichAutotrack].yPos++;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.DOWN() && stepchild.autotrackData[whichAutotrack].yPos>0){
        stepchild.autotrackData[whichAutotrack].yPos--;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
    }
  }
  //transformation
  else{
    //changing amplitude
    if(stepchild.buttons.joystickY != 0 && stepchild.itsbeen(25)){
      if(stepchild.buttons.joystickY == -1 && stepchild.autotrackData[whichAutotrack].amplitude<127){
        stepchild.autotrackData[whichAutotrack].amplitude++;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickY == 1 && stepchild.autotrackData[whichAutotrack].amplitude>-127){
        stepchild.autotrackData[whichAutotrack].amplitude--;
        stepchild.autotrackData[whichAutotrack].regenCurve();
        stepchild.lastTime = millis();
      }
    }
    //changing by 1
    if(stepchild.buttons.SHIFT() && stepchild.itsbeen(25)){
      //changing period
      if(stepchild.buttons.joystickX != 0){
        if(stepchild.buttons.joystickX == -1){
          stepchild.autotrackData[whichAutotrack].period++;
          stepchild.autotrackData[whichAutotrack].regenCurve();
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == 1 && stepchild.autotrackData[whichAutotrack].period>1){
          stepchild.autotrackData[whichAutotrack].period--;
          stepchild.autotrackData[whichAutotrack].regenCurve();
          stepchild.lastTime = millis();
        }
      }
    }
    else if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickX != 0){
        if(stepchild.buttons.joystickX == -1){
          if(stepchild.autotrackData[whichAutotrack].period == 1)
            stepchild.autotrackData[whichAutotrack].period = 0;
          stepchild.autotrackData[whichAutotrack].period+=stepchild.subDivision;
          stepchild.autotrackData[whichAutotrack].regenCurve();
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == 1 && stepchild.autotrackData[whichAutotrack].period>stepchild.subDivision){
          stepchild.autotrackData[whichAutotrack].period-=stepchild.subDivision;
          stepchild.autotrackData[whichAutotrack].regenCurve();
          stepchild.lastTime = millis();
        }
      }
    }
  }
  return true;
}

void drawAutotrackInputIcon(uint8_t x1, uint8_t y1, uint8_t id){
  switch(stepchild.autotrackData[id].recordFrom){
    //external midi
    case EXTERNAL_MIDI:
      stepchild.display.drawBitmap(x1,y1,tiny_midi_bmp,7,7,1);
      break;
    //encoder A
    case ENCODER_A:
      graphics.printItalic(x1,y1,'A',1);
      break;
    //encoder B
    case ENCODER_B:
      graphics.printItalic(x1,y1,'B',1);
      break;
    //joy X
    case JOY_X:
      graphics.printItalic(x1,y1,'X',1);
      break;
    //joy Y
    case JOY_Y:
      graphics.printItalic(x1,y1,'Y',1);
      break;
  }
}

void drawAutotrackEditor(uint8_t y,uint8_t interpType,bool translation, bool settingRecInput, uint8_t whichAutotrack){
  //bounding box and 3D-effect
  stepchild.display.drawRect(32,y,112,64,SSD1306_WHITE);
  stepchild.display.drawLine(32,y,29,y+3,SSD1306_WHITE);
  stepchild.display.drawLine(29,y+3,29,64,SSD1306_WHITE);

  //drawing all the points
  if(stepchild.autotrackData.size()>0){
    for(uint16_t step = stepchild.viewStart; step<stepchild.viewEnd; step++){
      if(step<stepchild.sequenceLength){
        //measure bars
        if (!(step % stepchild.subDivision) && (step%96) && (stepchild.subDivision*stepchild.viewScale)>2) {
          graphics.drawDottedLineV((step-stepchild.viewStart)*stepchild.viewScale+32,0,64,4);
        }
        if(!(step%96)){
          graphics.drawDottedLineV2((step-stepchild.viewStart)*stepchild.viewScale+32,0,64,6);
        }
        //gets the on-screen position of each point
        uint8_t yPos;
        //if it's a 255, display the last val
        if(stepchild.autotrackData[whichAutotrack].data[step] == 255){
          yPos = (64-float(stepchild.autotrackData[whichAutotrack].getLastDTVal(step))/float(127)*64);
        }
        else{
          yPos = (64-float(stepchild.autotrackData[whichAutotrack].data[step])/float(127)*64);
        }
        if(step == stepchild.viewStart){
          stepchild.display.drawPixel(32+(step-stepchild.viewStart)*stepchild.viewScale,(yPos),SSD1306_WHITE);
        }
        else{
          stepchild.display.drawLine(32+(step-stepchild.viewStart)*stepchild.viewScale,(yPos),32+(step-1-stepchild.viewStart)*stepchild.viewScale,(64-float(stepchild.autotrackData[whichAutotrack].getLastDTVal(step-1))/float(127)*64),SSD1306_WHITE);
        }

        //drawing cursor info if in default editor mode
        if(stepchild.autotrackData[whichAutotrack].type == LINEAR_CURVE){
          //drawing selection arrow
          if(stepchild.autotrackData[whichAutotrack].isDataPointSelected(step)){
            stepchild.display.fillCircle(32+(step-stepchild.viewStart)*stepchild.viewScale,yPos,2,SSD1306_WHITE);
            if(stepchild.autotrackData[whichAutotrack].data[step]>=64)//bottom arrow if data point is high
              graphics.drawArrow(32+(step-stepchild.viewStart)*stepchild.viewScale,yPos+4+((millis()/200)%2),2,ARROW_UP,true);
            else//top arrow if data point is low
              graphics.drawArrow(32+(step-stepchild.viewStart)*stepchild.viewScale,yPos-6+((millis()/200)%2),2,ARROW_DOWN,true);
          }
          //drawing cursor
          if(step == stepchild.cursorPos){
            stepchild.display.drawFastVLine(32+(stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale,0,64,SSD1306_WHITE);
            stepchild.display.drawFastVLine(32+(stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+1,0,64,SSD1306_WHITE);
            stepchild.display.drawCircle(32+(stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale,yPos,3+((millis()/400)%2),SSD1306_WHITE);
          }
        }

        //playhead
        if(stepchild.playing() && ((stepchild.autotrackData[whichAutotrack].triggerSource == GLOBAL_TRIGGER && step == stepchild.playheadPos) || (stepchild.autotrackData[whichAutotrack].triggerSource != GLOBAL_TRIGGER && step == stepchild.autotrackData[whichAutotrack].playheadPos)))
          stepchild.display.drawRoundRect(32+(step-stepchild.viewStart)*stepchild.viewScale,0,3,stepchild.SCREEN_HEIGHT,3,SSD1306_WHITE);
        
        //rechead
        if(stepchild.recording() && stepchild.recheadPos == step)
          stepchild.display.drawRoundRect(32+(step-stepchild.viewStart)*stepchild.viewScale,0,3,stepchild.SCREEN_HEIGHT,3,SSD1306_WHITE);

        //loop points
        if(step == stepchild.loopData[stepchild.activeLoop].start){
          graphics.drawArrow(32+(step-stepchild.viewStart)*stepchild.viewScale-1+((millis()/200)%2),59,4,ARROW_RIGHT,true);
        }
        else if(step == stepchild.loopData[stepchild.activeLoop].end){
          graphics.drawArrow(32+(step-stepchild.viewStart)*stepchild.viewScale+2-((millis()/200)%2),59,4,ARROW_LEFT,false);
        }
      }
    }
    //middle line
    //if it's just the typical node curve, draw midline at middle of the screen (32)
    if(stepchild.autotrackData[whichAutotrack].type == LINEAR_CURVE)
      graphics.drawDottedLineH(32,128,32,3);
    else{
      graphics.drawDottedLineH(32,128,64-(stepchild.autotrackData[whichAutotrack].yPos-32),3);
    }
    //drawing curve icon
    if(stepchild.autotrackData[whichAutotrack].type == LINEAR_CURVE)
      drawNodeEditingIcon(12,14,interpType,millis()/50,true);
    else
      drawCurveIcon(12,14,stepchild.autotrackData[whichAutotrack].type,millis()/50);

    //menu info
    if(stepchild.buttons.SHIFT()){
      printParam(16,2,stepchild.autotrackData[whichAutotrack].control,true,stepchild.autotrackData[whichAutotrack].parameterType,true);
      graphics.printChannel(16,12,stepchild.autotrackData[whichAutotrack].channel,true);
      stepchild.display.drawChar(3,2+sin(millis()/50),0x0E,1,0,1);
    }
    else{
      //title
      if(!stepchild.playing()){
        graphics.printSmall(0,0,"trk",SSD1306_WHITE);
        graphics.printSmall(6-stringify(whichAutotrack+1).length()*2,7,stringify(whichAutotrack+1),SSD1306_WHITE);
      }
      graphics.printSmall(15,0,"CC"+stringify(stepchild.autotrackData[whichAutotrack].control),SSD1306_WHITE);
      graphics.printSmall(15,7,"CH"+stringify(stepchild.autotrackData[whichAutotrack].channel),SSD1306_WHITE);
    }
    //drawing curve info, for non-defaults
    if(stepchild.autotrackData[whichAutotrack].type != 0){
      if(translation){
        graphics.printSmall(12,26,"@:"+stringify(stepchild.autotrackData[whichAutotrack].phase),SSD1306_WHITE);
        graphics.printSmall(12,33,"Y:"+stringify(stepchild.autotrackData[whichAutotrack].yPos),SSD1306_WHITE);
      }
      else{
        graphics.printSmall(12,26,"A:"+stringify(stepchild.autotrackData[whichAutotrack].amplitude),SSD1306_WHITE);
        graphics.printSmall(12,33,"P:"+stringify(stepchild.autotrackData[whichAutotrack].period),SSD1306_WHITE);
      }
    }
    
    //play icon
    if(stepchild.playing())
      stepchild.display.fillTriangle(120+((millis()/200)%2),9,120+((millis()/200)%2),3,120+6+((millis()/200)%2),6,SSD1306_WHITE);
    if(stepchild.recording()){
      //flash it while waiting
      if(stepchild.idlingUntilNoteReceived && (millis()%1000>500))
        stepchild.display.drawCircle(124,6,3,SSD1306_WHITE);
      //draw it solid if the rec has started
      else if(!stepchild.idlingUntilNoteReceived)
        stepchild.display.fillCircle(124,6,3,SSD1306_WHITE);
    }
    
    //drawing the bargraph
    uint8_t barHeight;
    if(!stepchild.playing())
      barHeight = 50;
    else
     barHeight = 64;
    stepchild.display.drawRect(0,stepchild.SCREEN_HEIGHT-barHeight,11,barHeight,SSD1306_WHITE);
    uint8_t height;
    uint8_t val;
    if(stepchild.playing()){
      if(stepchild.autotrackData[whichAutotrack].data[stepchild.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?stepchild.playheadPos:stepchild.autotrackData[whichAutotrack].playheadPos] == 255)
        val = stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?stepchild.playheadPos:stepchild.autotrackData[whichAutotrack].playheadPos);
      else
        val = stepchild.autotrackData[whichAutotrack].data[stepchild.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?stepchild.playheadPos:stepchild.autotrackData[whichAutotrack].playheadPos];
    }
    else{
      if(stepchild.autotrackData[whichAutotrack].data[stepchild.cursorPos] == 255)
        val = stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.cursorPos);
      else
        val = stepchild.autotrackData[whichAutotrack].data[stepchild.cursorPos];
    }
    //filling the bar graphgraph
    height = abs(float(val)/float(127)*float(barHeight-4));
    if(height>0)
      stepchild.display.fillRect(2,62-height,7,height,2);//inverted color!
    
    //drawing sent data
    stepchild.display.setRotation(1);
    if(stepchild.playing()){
      graphics.printSmall(barHeight/2-stringify(stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?stepchild.playheadPos:stepchild.autotrackData[whichAutotrack].playheadPos)).length()*2,3,stringify(stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.autotrackData[whichAutotrack].triggerSource==GLOBAL_TRIGGER?stepchild.playheadPos:stepchild.autotrackData[whichAutotrack].playheadPos)),2);
    }
    else{
      graphics.printSmall(barHeight/2-stringify(stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.cursorPos)).length()*2,3,stringify(stepchild.autotrackData[whichAutotrack].getLastDTVal(stepchild.cursorPos)),2);
    }
    stepchild.display.setRotation(DISPLAY_UPRIGHT);

    //drawing source icon
    graphics.printSmall(15,42,"src",1);
    graphics.drawArrow(20,52+sin(millis()/400),2,ARROW_DOWN,false);
    if(!settingRecInput || (millis()/400)%2)
      drawAutotrackInputIcon(17,55,whichAutotrack);
    if(settingRecInput){
      stepchild.display.fillRoundRect(36,3,63,17,3,0);
      stepchild.display.drawRoundRect(36,3,63,17,3,1);
      graphics.printSmall(38,6,"touch A,B,X or Y",1);
      graphics.printSmall(38,12,"to set rec input",1);
    }
  }
  else{
    graphics.printSmall(50,29,"no data, kid",SSD1306_WHITE);
  }
}

void drawMiniDT(uint8_t x1, uint8_t y1, uint8_t height, uint8_t which){
  if(stepchild.autotrackData[which].isActive){
    float yScale = float(height-1)/float(127);
    // float xScale = scale;
    //i starts at start, goes from start to the end of the screen
    // float sc = float(96)/float(stepchild.loopData[stepchild.activeLoop].end-stepchild.loopData[stepchild.activeLoop].start);
    float sc = float(74)/float(stepchild.viewEnd-stepchild.viewStart);
    //ends at stepchild.viewEnd-1 because it draws lines 2 points at a time
    for(uint16_t i = stepchild.viewStart; i<stepchild.viewEnd; i++){
      if(i<stepchild.autotrackData[which].data.size()){
        if(i >= stepchild.viewEnd - 1){
          stepchild.display.drawLine(x1+(i-stepchild.viewStart)*sc-1, y1+yScale*(127-stepchild.autotrackData[which].getLastDTVal(i)),x1+(stepchild.viewEnd-stepchild.viewStart)*sc-1, y1+yScale*(127-stepchild.autotrackData[which].getLastDTVal(i)),SSD1306_WHITE);
          break;
        }
        else
          stepchild.display.drawLine(x1+(i-stepchild.viewStart)*sc, y1+yScale*(127-stepchild.autotrackData[which].getLastDTVal(i)),x1+(i+1-stepchild.viewStart)*sc-1, y1+yScale*(127-stepchild.autotrackData[which].getLastDTVal(i+1)),SSD1306_WHITE);
        if(stepchild.playing()){
          if((stepchild.autotrackData[which].triggerSource == GLOBAL_TRIGGER && i == stepchild.playheadPos) || (stepchild.autotrackData[which].triggerSource != GLOBAL_TRIGGER && i == stepchild.autotrackData[which].playheadPos)){
            stepchild.display.drawFastVLine(x1+(i-stepchild.viewStart)*sc,y1,height,SSD1306_WHITE);
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
    stepchild.display.fillRoundRect(x1+20,y1+2,25,7,3,0);
    graphics.printSmall(x1+22,y1+3,"[off]",SSD1306_WHITE);
  }
}

void drawRobotSprite(uint8_t x, uint8_t y,uint8_t whichAutotrack){
  //Drawing our robot bud
  if(stepchild.playing()){
    uint16_t position = 0;
    //if the autotrack is triggering, then use its internal playhead
    if(stepchild.autotrackData[whichAutotrack].triggerSource != GLOBAL_TRIGGER)
      position = stepchild.autotrackData[whichAutotrack].playheadPos;
    else
      position = stepchild.playheadPos;
    switch((position/24)%4){
      case 0:
        stepchild.display.drawBitmap(x+1,y+3,robo3_mask,18,17,0);
        stepchild.display.drawBitmap(x,y,robo3,21,24,1);
        break;
      case 1:
        stepchild.display.drawBitmap(x+9,y+2,robo1_mask,12,17,0);
        stepchild.display.drawBitmap(x+3,y,robo4,18,24,1);
        break;
      case 2:
        stepchild.display.drawBitmap(x+9,y+2,robo5_mask,12,17,0);
        stepchild.display.drawBitmap(x+9,y,robo5,18,24,1);
        break;
      case 3:
        stepchild.display.drawBitmap(x+9,y+2,robo5_mask,12,17,0);
        stepchild.display.drawBitmap(x+9,y,robo6,18,24,1);
        break;
    }
  }
  else{
    stepchild.display.drawBitmap(x+9,y+2,robo1_mask,12,17,0);
    stepchild.display.drawBitmap(x+8,y,robo[(millis()/500)%2],13,24,1);
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
      stepchild.buttons.readButtons();
      stepchild.buttons.readJoystick();
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.MENU()){
          stepchild.lastTime = millis();
          if(popupMenuActive){
            popupMenuActive = false;
          }
          else{
            return false;
          }
        }
        if(stepchild.buttons.DELETE() && !popupMenuActive){
          deleteAutotrack(cursor);
          if(cursor>=stepchild.autotrackData.size())
            cursor = stepchild.autotrackData.size()-1;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.NEW()){
          if(stepchild.autotrackData.size() == 0)
            stepchild.createAutotrack(LINEAR_CURVE);
          else
            stepchild.createAutotrack(LINEAR_CURVE,(stepchild.autotrackData[stepchild.autotrackData.size()-1].control>=127)?0:(stepchild.autotrackData[stepchild.autotrackData.size()-1].control+1));
          cursor = stepchild.autotrackData.size()-1;
          if(cursor>menuStart+5)
            menuStart++;
          else if(cursor<menuStart)
            menuStart--;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.PLAY()){
          stepchild.togglePlay();
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.SELECT()){
          stepchild.lastTime = millis();
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
                stepchild.autotrackData[cursor].isPrimed = !stepchild.autotrackData[cursor].isPrimed;
                break;
              //on/off
              case 3:
                stepchild.autotrackData[cursor].isActive = !stepchild.autotrackData[cursor].isActive;
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
          if(stepchild.buttons.UP()){
            if(popupCursor > 2){
              popupCursor+=2;
            }
            else{
              popupCursor = 4;
            }
            stepchild.lastTime = millis();
          }
          if(stepchild.buttons.DOWN()){
            if(popupCursor > 3){
              popupCursor-=2;
              stepchild.lastTime = millis();
            }
          }
          if(stepchild.buttons.RIGHT()){
            if(popupCursor == 3 || popupCursor == 5 || popupCursor<2){
              popupCursor++;
              stepchild.lastTime = millis();
            }
          }
          if(stepchild.buttons.LEFT()){
            if(popupCursor == 0 || popupCursor == 3 || popupCursor == 5){
              popupMenuActive = false;
            }
            else{
              popupCursor--;
            }
            stepchild.lastTime = millis();
          }
        }
      }
      if(stepchild.itsbeen(100)){
        if(!popupMenuActive){
          if(stepchild.buttons.UP() && cursor < stepchild.autotrackData.size()-1){
            cursor++;
            stepchild.lastTime = millis();
            if(cursor>(menuStart+5))
              menuStart++;
          }
          if(stepchild.buttons.DOWN() && cursor>0){
            cursor--;
            stepchild.lastTime = millis();
            if(cursor<menuStart)
              menuStart--;
          }
          if(stepchild.buttons.RIGHT()){
            popupMenuActive = true;
            stepchild.lastTime = millis();
          }
        }
      }
      if(popupMenuActive){
        if(stepchild.buttons.counterA){
          switch(popupCursor){
            //CC
            case 0:{
              int16_t temp = stepchild.autotrackData[cursor].control;
              temp+=stepchild.buttons.counterA<0?-1:1;
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterA = 0;
              }
              else if(temp>127){
                temp = 127;
                stepchild.buttons.counterA = 0;
              }
              stepchild.autotrackData[cursor].control = temp;
            }
              break;
            //ch
            case 1:{
              int16_t temp = stepchild.autotrackData[cursor].channel;
              temp+=stepchild.buttons.counterA<0?-1:1;
              if(temp < 0){
                temp = 0;
                stepchild.buttons.counterA = 0;
              }
              else if(temp>15){
                temp = 15;
                stepchild.buttons.counterA = 0;
              }
              stepchild.autotrackData[cursor].channel = temp;
            }
              break;
          }
          stepchild.buttons.countDownA();
        }
      }
      return true;
    }
    String getHelpText(){
      if(popupMenuActive){
        switch(popupCursor){
          //cc
          case 0: return "edit the CC val sent";
          //ch
          case 1: return "edit the channel data is sent on";
          //primed
          case 2: return "whether or not the track will be written to while recording";
          //on/off
          case 3: return "whether or not the track sends data";
          //edit
          case 4: return "edit track data/curve";
          //apply
          case 5: return "apply data to notes in sequence";
          //set trigger
          case 6: return "link track to note trigger";
          default: return "";
        }
      }
      else{
        return "viewing track "+stringify(cursor);
      }
    }
    void displayMenu(bool clearDisplay){
      const uint8_t width = 24;
      const uint8_t height = 7;

      if(clearDisplay)
        stepchild.display.clearDisplay();
      graphics.drawSeq(settings);
      graphics.ditherBackground(32,16,128,64);
      stepchild.display.fillRect(coords.start.x,coords.start.y+16,32,48-coords.start.y,0);

      //border
      stepchild.display.fillRoundRect(coords.start.x+3+width+1,coords.start.y+16,80,44,3,0);
      stepchild.display.drawRoundRect(coords.start.x+3+width+1,coords.start.y+16,80,44,3,1);

      //title
      if(coords.start.x+coords.start.y-1<stepchild.SCREEN_WIDTH){
        stepchild.display.setCursor(coords.start.x+coords.start.y+28,8);
        stepchild.display.setFont(&FreeSerifItalic9pt7b);
        stepchild.display.print("Automation");
        stepchild.display.setFont();
      }
      // graphics.printCursive(coords.start.x+coords.start.y+25,0,"automation tracks",1);
      drawRobotSprite(coords.start.x+101,coords.start.y+40,cursor);
      // stepchild.display.drawBitmap(coords.start.x+109,coords.start.y+8,autobot_speech_bubble_bmp,18,22,1);
      // stepchild.display.fillRect(coords.start.x+102,coords.start.y+9,16,20,0);
      // graphics.printSmall_overflow(coords.start.x+102,coords.start.y+9,5,getHelpText(),1);

      //display autotracks that are in the sequence
      if(stepchild.autotrackData.size()){
        for(uint8_t i = 0; i<min(6,stepchild.autotrackData.size()); i++){
          if(i+menuStart== cursor){
            stepchild.display.fillRoundRect(coords.start.x+3,coords.start.y+16+i*(height+1),width+4,height,3,1);
            if(!popupMenuActive){
              graphics.drawArrow(coords.start.x+4+(millis()/400)%2,coords.start.y+16+i*(height+1)+height/2,3,ARROW_RIGHT,false);
              graphics.drawArrow(coords.start.x+3+width+4,coords.start.y+16+i*(height+1)+height/2,3,ARROW_RIGHT,true);
            }
          }
          else{
            stepchild.display.fillRoundRect(coords.start.x+3,coords.start.y+16+i*(height+1),width,height,3,1);
          }
          graphics.printSmall(coords.start.x+6,coords.start.y+16+i*(height+1)+1,"#"+stringify(i+menuStart),0);
        }
        if(stepchild.autotrackData.size()<6){
          graphics.drawButton(coords.start.x+10,coords.start.y+16+stepchild.autotrackData.size()*8,"n",1);
          graphics.printSmall(coords.start.x+12,coords.start.y+23+stepchild.autotrackData.size()*8,"+",1);
        }
        //if there are more than 6 tracks (and therefore some are offscreen), draw arrows indicating where they are
        //if there's some 'above'
        if(menuStart > 0){
          graphics.drawArrow(coords.start.x+24,coords.start.y+2+millis()/200%2,2,ARROW_UP,true);
        }
        //if there's some below
        if(stepchild.autotrackData.size() > menuStart + 6){
          graphics.drawArrow(coords.start.x+24,coords.start.y+12-millis()/200%2,2,ARROW_DOWN,true);
        }

        //holder for the AT display
        stepchild.display.drawRoundRect(coords.start.x+30,coords.start.y+18,76,12,3,1);
        drawMiniDT(coords.start.x+31,coords.start.y+19,12,cursor);

        //AT info
        /*
        type, trigger, cc, ch
        */
        //curve icon
        drawCurveIcon(coords.start.x+30,coords.start.y+32,stepchild.autotrackData[cursor].type,millis()/50);

        //cc and ch indicators
        stepchild.display.drawBitmap(coords.start.x+50,coords.start.y+32,cc_tiny,5,3,SSD1306_WHITE);
        graphics.printSmall(coords.start.x+56,coords.start.y+32,stringify(stepchild.autotrackData[cursor].control),1);
        uint8_t offset = 4*stringify(stepchild.autotrackData[cursor].control).length();
        stepchild.display.drawBitmap(coords.start.x+56+offset,coords.start.y+32,ch_tiny,6,3,SSD1306_WHITE);
        graphics.printSmall(coords.start.x+63+offset,coords.start.y+32,stringify(stepchild.autotrackData[cursor].channel),1);
        offset+=4*stringify(stepchild.autotrackData[cursor].channel).length();

        //primed
        graphics.drawButton(coords.start.x+67+offset,coords.start.y+35,stepchild.autotrackData[cursor].isPrimed?"primed":"prime",stepchild.autotrackData[cursor].isPrimed);
        if(stepchild.autotrackData[cursor].isPrimed && (millis()/600)%2){
          stepchild.display.fillCircle(coords.start.x+offset+66,coords.start.y+34,2,1);
        }
        else{
          stepchild.display.drawCircle(coords.start.x+offset+66,coords.start.y+34,2,1);
        }
        //buttons
        graphics.drawButton(coords.start.x+30,coords.start.y+43,stepchild.autotrackData[cursor].isActive?"on":"off",true);
        graphics.drawButton(coords.start.x+46,coords.start.y+43,"edit curve",true);
        graphics.drawButton(coords.start.x+54,coords.start.y+51,"trigger",true);
        //trigger
        switch(stepchild.autotrackData[cursor].triggerSource){
          case GLOBAL_TRIGGER:
            graphics.printSmall(coords.start.x+89,coords.start.y+52,"glbl",1);
            break;
          case TRACK_TRIGGER:
            stepchild.display.drawBitmap(coords.start.x+89,coords.start.y+51,trk_tiny,10,3,1);
            graphics.printSmall(coords.start.x+100,coords.start.y+51,stringify(stepchild.autotrackData[cursor].triggerTarget),1);
            break;
          case CHANNEL_TRIGGER:
            stepchild.display.drawBitmap(coords.start.x+89,coords.start.y+51,ch_tiny,6,3,1);
            graphics.printSmall(coords.start.x+96,coords.start.y+51,stringify(stepchild.autotrackData[cursor].triggerTarget+1),1);
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
              graphics.drawArrow(coords.start.x+60+4*stringify(stepchild.autotrackData[cursor].control).length(),coords.start.y+36+((millis()/300)%2),3,ARROW_UP,false);
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
        graphics.drawButton(coords.start.x+10,coords.start.y+17,"n",1);
        graphics.printSmall(coords.start.x+12,coords.start.y+24,"+",1);
        graphics.printSmall(coords.start.x+33,coords.start.y+35,"no automation data!",1);

      }
      if(clearDisplay)
        stepchild.display.display();
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
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU() || stepchild.buttons.SELECT()){
        stepchild.lastTime = millis();
        return false;
      }
      if(stepchild.buttons.RIGHT()){
        switch(stepchild.autotrackData[underlyingMenu->cursor].triggerSource){
          case GLOBAL_TRIGGER:
            stepchild.autotrackData[underlyingMenu->cursor].setTrigger(TRACK_TRIGGER,0);
            break;
          case TRACK_TRIGGER:
            stepchild.autotrackData[underlyingMenu->cursor].setTrigger(CHANNEL_TRIGGER,0);
            break;
          case CHANNEL_TRIGGER:
            break;
        }
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.LEFT()){
        switch(stepchild.autotrackData[underlyingMenu->cursor].triggerSource){
          case GLOBAL_TRIGGER:
            break;
          case TRACK_TRIGGER:
            stepchild.autotrackData[underlyingMenu->cursor].setTrigger(GLOBAL_TRIGGER,0);
            break;
          case CHANNEL_TRIGGER:
            stepchild.autotrackData[underlyingMenu->cursor].setTrigger(TRACK_TRIGGER,0);
            break;
        }
        stepchild.lastTime = millis();
      }
    }
    //changing the trigger target
    while(stepchild.buttons.counterA){
      if(stepchild.autotrackData[underlyingMenu->cursor].triggerSource == GLOBAL_TRIGGER){
        stepchild.buttons.counterA = 0;
        break;
      }
      int16_t temp = stepchild.autotrackData[underlyingMenu->cursor].triggerTarget;
      temp+=stepchild.buttons.counterA<0?-1:1;
      switch(stepchild.autotrackData[underlyingMenu->cursor].triggerSource){
        case TRACK_TRIGGER:
          if(temp < 0){
            temp = 0;
            stepchild.buttons.counterA = 0;
          }
          else if(temp >= stepchild.trackData.size()){
            temp = stepchild.trackData.size()-1;
            stepchild.buttons.counterA = 0;
          }
          break;
        case CHANNEL_TRIGGER:
          if(temp < 0){
            temp = 0;
            stepchild.buttons.counterA = 0;
          }
          else if(temp > 15){
            temp = 15;
            stepchild.buttons.counterA = 0;
          }
          break;
      }
      stepchild.autotrackData[underlyingMenu->cursor].triggerTarget = temp;
      stepchild.buttons.countDownA();
    }
    //changing the gate behavior
    if(stepchild.buttons.counterB){
      stepchild.autotrackData[underlyingMenu->cursor].gated = !stepchild.autotrackData[underlyingMenu->cursor].gated;
      stepchild.buttons.counterB = 0;
    }
    return true;
  }
  void displayMenu(){
    stepchild.display.clearDisplay();
    underlyingMenu->displayMenu(false);
    stepchild.display.fillRoundRect(coords.start.x,coords.start.y,131-coords.start.x,40,3,0);
    stepchild.display.drawRoundRect(coords.start.x,coords.start.y,96,40,3,1);

    //slider
    vector<String> options = {"global","track","channel"};
    graphics.drawSlider(coords.start.x+3,coords.start.y+2,options,stepchild.autotrackData[underlyingMenu->cursor].triggerSource);
    //arrow+label
    String s = "";
    switch(stepchild.autotrackData[underlyingMenu->cursor].triggerSource){
      //global
      case 0:
        s = "any track";
        graphics.drawArrow(coords.start.x+17,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
      //track
      case 1:
        s = "track "+stringify(stepchild.autotrackData[underlyingMenu->cursor].triggerTarget);
        graphics.drawArrow(coords.start.x+41,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
      //channel
      case 2:
        s = "channel "+stringify(stepchild.autotrackData[underlyingMenu->cursor].triggerTarget+1);
        graphics.drawArrow(coords.start.x+68,coords.start.y+10+(millis()/400)%2,3,ARROW_UP,false);
        break;
    }
    graphics.printSmall(coords.start.x+2,coords.start.y+17,"triggers when a note is"+s,1);
    graphics.printSmall(coords.start.x+2,coords.start.y+23,"sent on "+s,1);
    stepchild.display.drawFastHLine(coords.start.x+30,coords.start.y+29,graphics.getSmallTextLength(s),1);
    graphics.drawInputIcon(coords.start.x+30+graphics.getSmallTextLength(s)+3,coords.start.y+24,"A");

    //gate display
    graphics.printSmall(coords.start.x+2,coords.start.y+32,"gated:",1);
    graphics.drawButton(coords.start.x+24,coords.start.y+31,stepchild.autotrackData[underlyingMenu->cursor].gated?"true":"false",stepchild.autotrackData[underlyingMenu->cursor].gated);
    graphics.drawInputIcon(coords.start.x+48,coords.start.y+32,"B");
    stepchild.display.display();
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
