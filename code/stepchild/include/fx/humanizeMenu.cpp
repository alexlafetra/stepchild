
//stores the humanizer parameters.
/*
  timingAmount is the max % of the subDiv that a note can be moved
  eg: 100% means a note can be moved up to 1 subDiv away
  velocityAmount is the amount of change a note's vel data can change
  and chance is the same
*/
class Humanizer{
    public:
      Humanizer();
      Humanizer(int8_t,int8_t,int8_t);
      int8_t timingAmount;
      int8_t velocityAmount;
      int8_t chanceAmount;
      int8_t * get(uint8_t);
};
Humanizer::Humanizer(){
  timingAmount = 10;
  velocityAmount = 16;
  chanceAmount = 0;
}
Humanizer::Humanizer(int8_t t,int8_t v,int8_t c){
  timingAmount = t;
  velocityAmount = v;
  chanceAmount = c;
}
int8_t * Humanizer::get(uint8_t which){
  switch(which){
    case 0:
      return &timingAmount;
    case 1:
      return &velocityAmount;
    case 3:
      return &chanceAmount;
  }
  return 0;
}

Humanizer humanizerParameters = Humanizer();

int16_t humanizeNote(uint8_t track, uint16_t id){
  if(id == 0){
    return 0;
  }
  //position
  int16_t positionOffset = float(sequence.subDivision*humanizerParameters.timingAmount)/float(100);
  positionOffset = random(-positionOffset,positionOffset);
  //velocity
  int8_t velOffset = float(127*humanizerParameters.velocityAmount)/float(100);
  velOffset = random(-velOffset,velOffset);
  //chance
  int8_t chanceOffset = random(-humanizerParameters.chanceAmount,humanizerParameters.chanceAmount);
  if(velOffset != 0){
    sequence.changeVel_byID(id,track,velOffset);
  }
  if(chanceOffset != 0){
      sequence.changeChance_byID(id, track, chanceOffset);
  }
  //position offset last so the id doesn't change
  if(positionOffset != 0){
    sequence.moveNote(id,track,track,sequence.noteData[track][id].startPos+positionOffset);
  }
  return positionOffset;
}

void humanizeSelectedNotes(){
  //humanizing selected notes
  if(sequence.selectionCount>0){
    for(uint8_t track = 0; track<sequence.noteData.size(); track++){
      for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
        if(sequence.noteData[track][note].isSelected()){
          humanizeNote(track,note);
        }
      }
    }
  }
}

void humanize(bool move){
  humanizeSelectedNotes();
  //quantizing the note at the cursor
  if(sequence.IDAtCursor() != 0){
    int16_t amount = humanizeNote(sequence.activeTrack,sequence.IDAtCursor());
    sequence.moveCursor(amount);
  }
}

struct PolarVertex2D{
  float r;
  float theta;
};

class HumanizeBlob{
  public:
    HumanizeBlob(float radius, uint8_t numberOfPoints);
    vector<PolarVertex2D> points;
    float increment;
    void jiggle(float amount, float amount2, float amount3, float r);
};

HumanizeBlob::HumanizeBlob(float radius, uint8_t numberOfPoints){
  vector<PolarVertex2D> p;
  increment = 2.0*float(PI)/float(numberOfPoints);
  for(uint8_t i = 0; i<numberOfPoints; i++){
      PolarVertex2D v;
      v.r = radius;
      v.theta = float(i)*increment;
      p.push_back(v);
  }
  points = p;
}
float getPseudoRandom(float theta, float amp,float timeAmp){
    return amp*cos(theta)*sin(millis()/100.0)*sin(theta*PI)*cos(3.0*theta/(2.0*PI)*sin(float(millis())/1000.0*timeAmp));
}

float getX(PolarVertex2D a, float mod){
    return (a.r+mod)*cos(a.theta);
}
float getY(PolarVertex2D a,float mod){
    return (a.r+mod)*sin(a.theta);
}

void HumanizeBlob::jiggle(float radiusAmount, float timingAmount, float rotationAmount, float radius){
  const uint16_t s = points.size()-1;
  for(uint8_t i = 0; i<s+1; i++){
    points[i].theta+=rotationAmount/100.0;
    points[i].r = radius;
  }
  display.drawLine(getX(points[0],getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+64,getY(points[0],getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+32,getX(points[s],getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+64,getY(points[s],getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+32,1);
  for(uint8_t i = 1; i<s+1; i++){
    display.drawLine(getX(points[i-1],getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+64,getY(points[i-1],getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+32,getX(points[i],getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+64,getY(points[i],getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+32,1);
  }
}

void humanizeMenu(){
  //cursor can be time, velocity, or chance
  uint8_t cursor = 0;
  HumanizeBlob blob = HumanizeBlob(20,30);
  while(true){
    //timing, vel, chance
    controls.readJoystick();
    controls.readButtons();
    if(!humanizeMenuControls(&cursor)){
      break;
    }
    display.clearDisplay();
    
    //draw timing amount
    String q = stringify(humanizerParameters.timingAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(3,9,q+"%",1);
      
    String s = stepsToMeasures(sequence.subDivision);
    graphics.printFraction_small(screenWidth-s.length()*4,11,s);

    //velocity amount
    q = stringify(humanizerParameters.velocityAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(3,54,q+"%",1);

    //chance amount
    q = stringify(humanizerParameters.chanceAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(109,54,q+"%",1);

    //draw labels
    switch(cursor){
      //pos
      case 0:
        display.fillRoundRect(0,0,20,7,4,1);
        display.drawRoundRect(-5,3,29,16,3,1);
        break;
      //vel
      case 1:
        display.fillRoundRect(0,45,16,7,4,1);
        display.drawRoundRect(-5,48,29,16,3,1);
        break;
      //subDiv
      case 2:
        display.fillRoundRect(113,0,15,7,4,1);
        display.drawRoundRect(107,3,23,18,3,1);
        break;
      //chance
      case 3:
        display.fillRoundRect(121,45,7,7,4,1);
        display.drawRoundRect(107,48,23,16,3,1);
        break;
    }
    printSmall(2,1,"time",2);
    printSmall(115,1,"div",2);
    printSmall(2,46,"vel",2);
    printSmall(123,46,"%",2);
    blob.jiggle(float(humanizerParameters.velocityAmount+1)/10.0,float(humanizerParameters.chanceAmount)/10.0,float(humanizerParameters.timingAmount)/500.0+1, 10.0*float(sequence.subDivision)/24.0);
    //title
    // display.fillRoundRect(77,0,54,9,3,1);
    // printCursive(79,1,"humanize",0);
    const char title[8] = {'h','u','m','a','n','i','z','e'};
    for(uint8_t i = 0; i<8; i++){
      printSmall(i*125/7,29,title[i],2);
    }
    display.display();
  }
}
bool humanizeMenuControls(uint8_t* cursor){
  if(utils.itsbeen(100)){
    //moving cursor
    if(controls.joystickY != 0){
      //if cursor == 1 or 3
      if(controls.joystickY == -1 && (*cursor)%2){
        (*cursor)--;
        lastTime = millis();
      }
      //if cursor == 0 or 2
      else if(controls.joystickY == 1 && !((*cursor)%2)){
        (*cursor)++;
        lastTime = millis();
      }
    }
    if(controls.joystickX != 0){
      if(controls.joystickX == -1 && (*cursor)<2){
        (*cursor)+=2;
        lastTime = millis();
      }
      else if(controls.joystickX == 1 && (*cursor)>1){
        (*cursor)-=2;
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.NEW()){
        lastTime = millis();
        while(true){
            if(selectNotes("humanize",[](uint8_t a, uint8_t b, uint8_t c, bool d){graphics.drawHumanizeIcon(a,b,c,d);})){
                humanizeSelectedNotes();
            }
            else{
                break;
            }
        }
    }
  }
  //change amount
  //this is comically ugly. It's because i changed humanizerParameters to a class
  //which did NOT make it more convenient than a byte array,except i can store a subDiv in there if I wanted to
  while(controls.counterA != 0 ){
      //changing subDiv
      if((*cursor) == 2){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(controls.SHIFT()){
          sequence.toggleTriplets();
        }
        else if(controls.counterA >= 1){
          sequence.changeSubDivInt(true);
        }
        //changing subdivint
        else if(controls.counterA <= -1){
          sequence.changeSubDivInt(false);
        }
      }
      else{
        if(controls.counterA >= 1 && ((*humanizerParameters.get(*cursor))<100)){
          if(controls.SHIFT()){
            (*humanizerParameters.get(*cursor))++;
          }
          else{
            (*humanizerParameters.get(*cursor))+=5;
          }
          if((*humanizerParameters.get(*cursor))>100){
            (*humanizerParameters.get(*cursor)) = 100;
          }
        }
        if(controls.counterA <= -1 && (*humanizerParameters.get(*cursor))>0){
          if(controls.SHIFT()){
            (*humanizerParameters.get(*cursor))--;
          }
          else{
            (*humanizerParameters.get(*cursor))-=5;
          }
          if((*humanizerParameters.get(*cursor))<0){
            (*humanizerParameters.get(*cursor)) = 0;
          }
        }
      }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(controls.SHIFT()){
      sequence.toggleTriplets();
    }
    else if(controls.counterB >= 1){
      sequence.changeSubDivInt(true);
    }
    //changing subdivint
    else if(controls.counterB <= -1){
      sequence.changeSubDivInt(false);
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }

  return true;
}
