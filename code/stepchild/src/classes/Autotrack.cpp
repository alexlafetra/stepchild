#include "classes/Autotrack.h"
#include "Stepchild.h"

extern Stepchild stepchild;

using namespace std;

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition);


Autotrack::Autotrack(){
}

Autotrack::Autotrack(CurveType t, uint16_t length){
  data.push_back(63);
  for(uint16_t i = 1; i<length; i++){
    data.push_back(255);
  }
  type = t;
}

void Autotrack::sendData(uint16_t timestep){
  if(!data.size())
    return;
  //bounds check for timestep --> timestep will "loop" around if bigger than data
  if(timestep>=data.size())
    timestep%=data.size();
  if(data[timestep] == 255)
    return;
  if(parameterType == 2)
    handleInternalCC(control,data[timestep],channel,yPos);
  else if(data[timestep] != 255)
    stepchild.midi.sendCC(control,data[timestep],channel);
}
void Autotrack::play(uint16_t timestep){
  if(!this->isActive)
    return;
  //normal Autotracks that use the global timestep
  if(this->triggerSource == GLOBAL_TRIGGER){
    this->sendData(timestep);
  }
  //trigger autotracks that use internal playheads
  else{
    this->sendData(this->playheadPos);
    this->playheadPos++;
  }
}

void Autotrack::setTrigger(TriggerSource trigSource, uint8_t trigTarget){
  triggerSource = trigSource;
  playheadPos = 0;
  switch(triggerSource){
    case GLOBAL_TRIGGER:
      triggerTarget = 0;
      isActive = true;
      break;
    case TRACK_TRIGGER:
      isActive = false;
      triggerTarget = trigTarget;
      break;
    case CHANNEL_TRIGGER:
      isActive = false;
      if(trigTarget<=16 && trigTarget>=1)
        triggerTarget = trigTarget;
      else
        triggerTarget = 1;
      break;
  }
}

void Autotrack::createDataPoint(uint16_t pos){
  data[pos] = 64;
}
void Autotrack::changeDataPoint(uint16_t pos, int8_t amount){
  if(data[pos] == 255){
    //if it's a blank point, go back until you hit a real point
    for(int32_t point = pos-1; point>=0; point--){
      //if there's a real point
      if(data[point] != 255){
        data[pos] = data[point];
        break;
      }
      //if it never hit a real point, just make one
      if(point == 0){
        data[pos] = 64;
        break;
      }
    }
  }
  int16_t newVal = data[pos]+amount;
  if(newVal<0)
    newVal = 0;
  else if(newVal > 127)
    newVal = 127;
  
  data[pos] = newVal;
}
void Autotrack::deleteDataPoint(uint16_t point){
  //if the track and point are in bounds
  if(point <= data.size()){
    //deleting data stored there
    data[point] = 255;
    //deselecting it (maybe it's better to not deselect it? idk)
    selectDataPoint(point, false);
  }
}
void Autotrack::selectDataPoint(uint16_t index,bool state){
  //if the point is in bounds
  if(index<data.size()){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<selectedPoints.size();i++){
      if(index == selectedPoints[i]){
        alreadySelected = true;
        //if it's already selected, break outta here
        if(state)
          return;
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(selectedPoints[i]);
      }
    }
    ///if it was already selected, and you don't want it to be, del it
    if(!state){
      selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(selectedPoints.size()>0){
      for(uint16_t i = 0; i<selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(selectedPoints[i]>index){
          selectedPoints.insert(selectedPoints.begin()+i,index);
          return;
        }
      }
      selectedPoints.push_back(index);
    }
    //if it's the first vector
    else if(selectedPoints.size() == 0){
      selectedPoints.push_back(index);
    }
  }
}
void Autotrack::toggleSelectDataPoint(uint16_t index){
  //if the point is in bounds
  if(index<data.size()){
    //check to see if the point is already selected by looping thru all selected id's
    bool alreadySelected = false;
    vector<uint16_t> temp;
    for(uint16_t i = 0; i<selectedPoints.size();i++){
      if(index == selectedPoints[i]){
        alreadySelected = true;
      }
      //if that point hasn't been found, then add all the other points to the temp vector
      else{
        temp.push_back(selectedPoints[i]);
      }
    }
    ///if it was already selected, and you don't want it to be, del it
    if(alreadySelected){
      selectedPoints.swap(temp);
    }
    //if it wasn't, add it to the selection INSERTED SO IT'S IN ORDER
    else if(selectedPoints.size()>0){
      for(uint16_t i = 0; i<selectedPoints.size(); i++){
        //if you find a value bigger than index, insert index before that value (so the vector stays ordered)
        if(selectedPoints[i]>index){
          selectedPoints.insert(selectedPoints.begin()+i,index);
          return;
        }
      }
      selectedPoints.push_back(index);
    }
    //if it's the first vector
    else if(selectedPoints.size() == 0){
      selectedPoints.push_back(index);
    }
  }
}
bool Autotrack::isDataPointSelected(uint16_t index){
  if(index>=0 && index<data.size()){
    //check each point to see if it matches index
    for(uint16_t i = 0; i<selectedPoints.size(); i++){
      if(index == selectedPoints[i]){
        return true;
      }
    }
  }
  //if it's out of bounds, or wasn't found in the selection, return false
  return false;
}
uint8_t Autotrack::getLastDTVal(uint16_t point){
  //if params are in bounds
  if(point<=data.size()){
    //step back thru DT and look for a non-255 value
    for(uint16_t i = point; i>=0; i--){
      if(data[i] != 255){
        return data[i];
      }
    }
  }
  //if something goes wrong, return 64
  return 64;
}
//linear interpolate between two points on the curve
void Autotrack::linearInterpolate(uint16_t start, uint16_t end){
  if(start>=0 && end <= data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          data[start] = 63;
          break;
        }
        else if(data[i] != 255){
          data[start] = data[i];
          break;
        }
      }
    }
    if(data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          data[end] = 63;
          break;
        }
        else if(data[i] != 255){
          data[end] = data[i];
          break;
        }
      }
    }
    float m = float(data[start] - data[end])/float(start-end);
    uint8_t b = data[start];
    for(uint16_t i = start; i<=end; i++){
      data[i] = m*(i-start)+b;
    }
  }
}
//interpolate between two points as though they're on an ellipse
void Autotrack::ellipticalInterpolate(uint16_t start, uint16_t end, bool up){
  //if they're the same point (which would be a bug) or the same value, return since you won't need to interpolate
  if(start == end || data[start] == data[end])
    return;
  //prepping autotrack data
  //if it's in bounds
  if(start>=0 && end <= data.size()){
    //if it's a 255 unwritten val, set it to the most recent point
    if(data[start] == 255){
      for(uint16_t i = start; i>=0; i--){
        if(i == 0){
          data[start] = 63;
          break;
        }
        else if(data[i] != 255){
          data[start] = data[i];
          break;
        }
      }
    }
    if(data[end] == 255){
      for(uint16_t i = end; i>=0; i--){
        if(i == 0){
          data[end] = 63;
          break;
        }
        else if(data[i] != 255){
          data[end] = data[i];
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
    uint8_t b = abs(data[end]-data[start]);

    //to store where (0,0) is
    uint8_t yOffset;
    uint16_t xOffset;

    //if "up", the yOffset is the highest of the two points since y will always be negative
    if(up){
      if(data[end]<data[start]){
        yOffset = data[start];
        xOffset = end;
      }
      else{
        yOffset = data[end];
        xOffset = start;
      }
    }
    else{
      if(data[end]>data[start]){
        yOffset = data[start];
        xOffset = end;
      }
      else{
        yOffset = data[end];
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
      data[i] = pt + yOffset;
    }
  }
}
//randomly perform different interpolations on different parts of the track
//(this doesn't give a great result)
void Autotrack::randomInterp(uint16_t start, uint16_t end){
  if(end>=data.size())
    end = data.size()-1;
  //0 is linear interp, 1 is elliptical interp up, 2 is down, 3 is sin, 4 is sq, 5 is saw, 6 is triangle
  //for 0-2, generate random value for the second point
  //for 3-6, generate random amplitude
  //phase should be set so that it always lines up! how do we do this
  //phase = start, yPos = autotrack[start]
  uint8_t randomFunction = random(0,7);

  uint16_t oldPhase = phase;
  uint8_t oldYPos = yPos;
  uint8_t oldAmp = amplitude;
  if(randomFunction<=2){
    data[end] = random(0,amplitude+1);
  }
  else if(randomFunction>=3){
    phase = start;
    yPos = data[start];
    // amplitude = random(-amplitude,amplitude+1);
  }
  switch(randomFunction){
    //linear
    case 0:
      linearInterpolate(start,end);
      break;
    //elliptical up
    case 1:
      ellipticalInterpolate(start,end,true);
      break;
    //elliptical down
    case 2:
      ellipticalInterpolate(start,end,false);
      break;
    //sine
    case 3:
      type = SINEWAVE_CURVE;
      regenCurve(start,end);
      break;
    //sq
    case 4:
      type = SQUAREWAVE_CURVE;
      regenCurve(start,end);
      break;
    //saw
    case 5:
      type = SAWTOOTH_CURVE;
      regenCurve(start,end);
      break;
    //triangle
    case 6:
      type = TRIANGLE_CURVE;
      regenCurve(start,end);
      break;
  }
  //reset it back to "random" type
  type = RANDOM_CURVE;
  //reset the curve values
  phase = oldPhase;
  yPos = oldYPos;
  amplitude = oldAmp;
}
void Autotrack::regenCurve(uint16_t start, uint16_t end){
  switch(type){
    //default
    case LINEAR_CURVE:
      for(uint16_t point = start; point<end; point++){
        data[point] = yPos;
      }
      break;
    //sinewave
    case SINEWAVE_CURVE:
      {
        for(uint16_t point = start; point < end; point++){
          //this one is crazy, and cool, but busted and idk why
          // int pt = amplitude*sin( (period/(2*PI)) * (point+phase) ) + yPos;
          //format is A * sin((2pi/Per)*(x+Phase))+yPos
          int pt = amplitude*sin(float(point+phase)*float(2.0*PI)/float(period)) + yPos;
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          data[point] = pt;
        }
      }
      break;
    //square wave
    case SQUAREWAVE_CURVE:
      {
        for(uint16_t point = start; point < end; point++){
          int pt;
          //if you're less than half a period, it's high. Else, it's low
          if((point+phase)%period<period/2)
            pt = yPos+amplitude;
          else
            pt = yPos-amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          data[point] = pt;
        }
      }
      break;
    //saw
    case SAWTOOTH_CURVE:
      {
        //the slope of each saw tooth is rise/run = amplitude/period
        float slope = float(amplitude*2)/float(period);
        for(uint16_t point = start; point < end; point++){
          // y = (x+offset)%period * m + yOffset
          int pt = ((point+phase)%period)*slope+yPos-amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          data[point] = pt;
        }
      }
      break;
    //triangle
    case TRIANGLE_CURVE:
      {
        float slope = float(amplitude*2)/float(period/2);
        int pt;
        for(uint16_t point = start; point < end; point++){
          //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
          if((point+phase)%period>(period/2))
            pt = ((point+phase)%period) * (-slope) + yPos + 3*amplitude;
          else
            pt = ((point+phase)%period) * slope + yPos - amplitude;
          //bounds
          if(pt>127)
            pt=127;
          if(pt<0)
            pt=0;
          //load pt into data
          data[point] = pt;
        }
      }
      break;

    //random patterns
    case RANDOM_CURVE:{
      uint16_t pt = start;
      //always seed it with the autotrack control #
      srand(control);
      while(pt<end){
        //get a random number to divide the period by (it'll always be a factor of the period)
        uint16_t nextPoint = pt+period;
        if(nextPoint>end){
          nextPoint = end;
        }
        randomInterp(pt,nextPoint);
        pt = nextPoint;
      }
      }
      break;
    case NOISE_CURVE:
      srand(control);
      for(uint16_t point = start; point<end; point++){
        int16_t temp = random(-amplitude/2,amplitude/2)+yPos;
        if(temp<0)
          temp = 0;
        else if(temp>127)
          temp = 127;
        data[point] = temp;
      }
      break;
  }
}
void Autotrack::regenCurve(){
  //if there are any selected points, ONLY regen between those points
  if(selectedPoints.size()>1){
    regenCurve(selectedPoints[0],selectedPoints.back());
  }
  //regens entire DT
  else
    regenCurve(0,data.size());
}
//this one smooths between selected points
void Autotrack::smoothSelectedPoints(uint8_t type){
  if(selectedPoints.size()>=2){
    switch(type){
      //linear
      case 0:
        //move through each selected point
        for(uint16_t i = 0; i<selectedPoints.size()-1; i++){
          linearInterpolate(selectedPoints[i],selectedPoints[i+1]);
        }
        return;
      //elliptical UP
      case 1:
        //move through each selected point
        for(uint16_t i = 0; i<selectedPoints.size()-1; i++){
          ellipticalInterpolate(selectedPoints[i],selectedPoints[i+1],true);
        }
        return;
      //elliptical DOWN
      case 2:
        //move through each selected point
        for(uint16_t i = 0; i<selectedPoints.size()-1; i++){
          ellipticalInterpolate(selectedPoints[i],selectedPoints[i+1],false);
        }
        return;
    }
  }
}
