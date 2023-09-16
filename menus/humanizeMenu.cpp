int16_t humanizeNote(uint8_t track, uint16_t id){
  if(id == 0){
    return 0;
  }
  //position
  int16_t positionOffset = float(subDivInt*humanizeParameters[0])/float(100);
  positionOffset = random(-positionOffset,positionOffset);
  //velocity
  int8_t velOffset = float(127*humanizeParameters[1])/float(100);
  velOffset = random(-velOffset,velOffset);
  //chance
  int8_t chanceOffset = random(-humanizeParameters[2],humanizeParameters[2]);
  if(velOffset != 0){
    changeVel(id,track,velOffset);
  }
  if(chanceOffset != 0){
    changeChance(id, track, chanceOffset);
  }
  //position offset last so the id doesn't change
  if(positionOffset != 0){
    moveNote(id,track,track,seqData[track][id].startPos+positionOffset);
  }
  return positionOffset;
}

void humanizeSelectedNotes(){
  //humanizing selected notes
  if(selectionCount>0){
    for(uint8_t track = 0; track<seqData.size(); track++){
      for(uint16_t note = 1; note<seqData[track].size(); note++){
        if(seqData[track][note].isSelected){
          humanizeNote(track,note);
        }
      }
    }
  }
}

void humanize(bool move){
  humanizeSelectedNotes();
  //quantizing the note at the cursor
  if(lookupData[activeTrack][cursorPos] != 0){
    int16_t amount = humanizeNote(activeTrack,lookupData[activeTrack][cursorPos]);
    moveCursor(amount);
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
    void jiggle(float amount, float amount2, float amount3);
    void render();
};

HumanizeBlob::HumanizeBlob(float radius, uint8_t numberOfPoints){
  vector<PolarVertex2D> p;
  increment = 2*PI/float(numberOfPoints);
  for(uint8_t i = 0; i<numberOfPoints; i++){
      PolarVertex2D v;
      v.r = radius;
      v.theta = float(i)*increment;
      p.push_back(v);
  }
  points = p;
}
float getPseudoRandom(float theta, float amp,float timeAmp){
    return amp*(cos((theta+float(timeAmp*millis())/100.0)*111.0)*cos(theta*3.0+float(timeAmp*millis())/1000.0)*sin(theta/7.0+float(timeAmp*millis())/100.0)*cos(theta/7.0+float(timeAmp*millis())/10.0)*sin(theta/5.0+float(timeAmp*millis()/100)));
}

float getX(PolarVertex2D a, float mod){
    return (a.r+mod)*cos(a.theta);
}
float getY(PolarVertex2D a,float mod){
    return (a.r+mod)*sin(a.theta);
}

void HumanizeBlob::jiggle(float radiusAmount, float timingAmount, float rotationAmount){
  float pseudoRand = getPseudoRandom(points[0].theta,radiusAmount,timingAmount);
  display.drawLine(getX(points[0],pseudoRand)+64,getY(points[0],pseudoRand)+32,getX(points[points.size()-1],pseudoRand)+64,getY(points[points.size()-1],pseudoRand)+32,1);
  points[0].theta+=increment*rotationAmount;
  for(uint8_t i = 1; i<points.size(); i++){
    pseudoRand = getPseudoRandom(points[i].theta,radiusAmount,timingAmount);
    display.drawLine(getX(points[i],pseudoRand)+64,getY(points[i],pseudoRand)+32,getX(points[i-1],pseudoRand)+64,getY(points[i-1],pseudoRand)+32,1);
    points[i].theta+=increment*rotationAmount;
  }
}



void HumanizeBlob::render(){
//  display.drawLine(getX(points[0])+64,getY(points[0])+32,getX(points[points.size()-1])+64,getY(points[points.size()-1])+32,1);
//  for(uint8_t i = 1; i<points.size(); i++){
//    display.drawLine(getX(points[i])+64,getY(points[i])+32,getX(points[i-1])+64,getY(points[i-1])+32,1);
//  }
}

void humanizeMenu(){
  //cursor can be time, velocity, or chance
  uint8_t cursor = 0;
  HumanizeBlob blob = HumanizeBlob(20,30);
  while(true){
    //timing, vel, chance
    joyRead();
    readButtons();
    if(!humanizeMenuControls(&cursor)){
      break;
    }
    display.clearDisplay();
    
    //draw humanize amount
    String q = stringify(humanizeParameters[0]);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(3,9,q+"%",1);
    q = stringify(humanizeParameters[1]);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(3,29,q+"%",1);
    q = stringify(humanizeParameters[2]);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(3,49,q+"%",1);

    printFraction_small(30,8,stepsToMeasures(subDivInt));

    //draw labels
    switch(cursor){
      //pos
      case 0:
        display.fillRoundRect(0,0,20,7,4,1);
        display.drawRoundRect(-5,3,29,16,3,1);
        printSmall(2,1,"time",0);
        printSmall(2,21,"vel",1);
        printSmall(2,41,"%",1);
        display.setRotation(3);
        printSmall(1,78,"(div)",1);
        display.setRotation(UPRIGHT);
        break;
      //vel
      case 1:
        display.fillRoundRect(0,20,16,7,4,1);
        display.drawRoundRect(-5,23,29,16,3,1);
        printSmall(2,1,"time",1);
        printSmall(2,21,"vel",0);
        printSmall(2,41,"%",1);
        break;
      //chance
      case 2:
        display.fillRoundRect(0,40,8,7,4,1);
        display.drawRoundRect(-5,43,29,16,3,1);
        printSmall(2,1,"time",1);
        printSmall(2,21,"vel",1);
        printSmall(2,41,"%",0);
        break;
    }
    //wormhole graphic
//    drawWormhole();
//    blob.render();
      blob.jiggle(float(humanizeParameters[1]+1)/10.0,float(humanizeParameters[2])/10.0,float(humanizeParameters[0])/500.0+1);
    //title
    display.fillRoundRect(77,0,54,9,3,1);
    printCursive(79,1,"humanize",0);
    display.display();
  }
}

bool humanizeMenuControls(uint8_t* cursor){
  if(itsbeen(100)){
    //moving cursor
    if(y != 0){
      if(y == -1 && (*cursor)>0){
        (*cursor)--;
        lastTime = millis();
      }
      else if(y == 1 && (*cursor)<2){
        (*cursor)++;
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(n){
        lastTime = millis();
        while(true){
            if(selectNotes("humanize",drawHumanizeIcon)){
                humanizeSelectedNotes();
            }
            else{
                break;
            }
        }
    }
  }
  //change amount
  while(counterA != 0 ){
    if(counterA >= 1 && humanizeParameters[(*cursor)]<100){
      if(shift){
        humanizeParameters[(*cursor)]++;
      }
      else{
        humanizeParameters[(*cursor)]+=5;
      }
      if(humanizeParameters[(*cursor)]>100){
        humanizeParameters[(*cursor)] = 100;
      }
    }
    if(counterA <= -1 && humanizeParameters[(*cursor)]>0){
      if(shift){
        humanizeParameters[(*cursor)]--;
      }
      else{
        humanizeParameters[(*cursor)]-=5;
      }
      if(humanizeParameters[(*cursor)]<0){
        humanizeParameters[(*cursor)] = 0;
      }
    }
    counterA += counterA<0?1:-1;;
  }
  switch((*cursor)){
    case 0:
      while(counterB != 0){
        //if shifting, toggle between 1/3 and 1/4 mode
        if(shift){
          toggleTriplets();
        }
        else if(counterB >= 1){
          changeSubDivInt(true);
        }
        //changing subdivint
        else if(counterB <= -1){
          changeSubDivInt(false);
        }
        counterB += counterB<0?1:-1;;
      }
      break;
    case 1:
      break;
    case 2:
      break;
  }
  return true;
}
