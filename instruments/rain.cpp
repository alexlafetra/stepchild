// 'rainSplash', 11x5px
// const unsigned char splash_bmp [] = {
// 	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x60, 0xc0
// };
// 'drip', 7x10px
const unsigned char drip_bmp [] = {
	0x10, 0x10, 0x28, 0x44, 0x92, 0xa2, 0xa2, 0x82, 0x44, 0x38
};

class Raindrop{
  public:
    uint8_t length;
    uint8_t x1;
    float y1;
    float vel;//pixels/frame
    bool madeSound;
    Raindrop();
    Raindrop(uint8_t,uint8_t,uint8_t);
    void render();
    bool update();
};

Raindrop::Raindrop(){
  vel = 0.7;
  length = vel*2;
  x1 = random(0,screenWidth);
  y1 = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
Raindrop::Raindrop(uint8_t xPos, uint8_t maxVel, uint8_t minVel){
  vel = pow(float(random(minVel,maxVel)),1.1);
  length = vel+3;
  x1 = xPos;
  y1 = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
void Raindrop::render(){
  display.drawFastVLine(x1,y1,length,1);
}
bool Raindrop::update(){
  bool  hasNotCrossedYet = false;
  if(y1<64){
    hasNotCrossedYet = true;
  }
  y1+=vel;
  if(y1>64 &&  hasNotCrossedYet){
    madeSound = true;
  }
  else{
    madeSound = false;
  }
  if(y1>=screenHeight+length){
    return false;
  }
  else{
    return true;
  }
}

//takes the x-position of a raindrop and finds where in pitch list that occurs
uint8_t positionToPitch(vector<uint8_t> pitchList, int8_t minOct, int8_t maxOct, uint8_t xCoord){

  float pxPerOctave = float(screenWidth)/float(maxOct-minOct);
  float pxPerNote = pxPerOctave/float(pitchList.size());
                    
  //the octave of the note is it's position divided by pixels/octave
  int8_t octave = float(xCoord)/pxPerOctave;
  //the pitch of the note is it's position (relative to the start of the octave) divided by pixels/note
  uint8_t pitch =  float(xCoord%uint8_t(pxPerOctave))/pxPerNote;

  return pitchList[pitch]+12*octave;
}

void printPitchList(vector<uint8_t> pitchList,uint8_t startPitch){
  const uint8_t x1 = 0;
  String pitches = "$";
  if(pitchList.size()==0){
    pitches+=pitchToString(startPitch,false,true);
  }
  else{
    for(uint8_t pitch = 0; pitch<pitchList.size(); pitch++){
      pitches+=pitchToString(pitchList[pitch],false,true);
    }
  }
  display.setRotation(SIDEWAYS_R);
  printSmall(screenWidth-pitches.length()*4,x1,pitches,1);
  display.setRotation(UPRIGHT);
}

//gradient on top representing rain flow, moveable 
//drops velocities correspond to note vel
//X axis is pitch bend? or pitch? and y-axis is intensity (can also be controlled with encoders)
void rain(){
  vector<Raindrop> drops;
  int16_t xCoord = 64;

  //maxDrops and stormIntensity both increase as the storm gets worse
  uint8_t stormIntensity = 5;
  uint8_t maxDrops = 10;

  const uint8_t maxIntensity = 20;
  const uint8_t intensityVariance = 5;
  uint8_t xVariance = 20;

  uint8_t channel = 1;
  uint8_t startPitch = 42;
  int8_t minOct = 0;
  int8_t maxOct = 1;

  //0 allows modulating the raindrops
  //1 allows editing the parameters
  bool menuState = true;
  uint8_t cursor = 0;
  int8_t menuOffset = 0;

  vector<uint8_t> pitchList;

  while(true){
    //controls
    //--------------------
    readButtons();
    joyRead();
    switch(menuState){
      //messing with the rain as an instrument
      case 0:
        //B changes the intensity
        while(counterB != 0){
          if(counterB >= 1 && stormIntensity<maxIntensity){
            stormIntensity++;
            maxDrops++;
          }
          else if(stormIntensity>1){
            stormIntensity--;
            maxDrops--;
          }
          counterB += counterB<0?1:-1;;
        }
        //A changes the width/spread
        while(counterA != 0){
          if(counterA > 0 && xVariance<screenWidth){
            xVariance+=3;
          }
          if(counterA < 0 && xVariance>10){
            xVariance-=3;
          }
          counterA += counterA<0?1:-1;
        }
        //X joystick changes the centerPoint
        if(x == -1 && xCoord<screenWidth){
          xCoord+=2;
          lastTime = millis();
        }
        else if(x == 1 && xCoord>0){
          xCoord-=2;
          lastTime = millis();
        }
        if(itsbeen(200)){
          if(n){
            lastTime = millis();
            for(uint8_t i = 0; i<20; i++){
              if(drops.size()<maxDrops)
                drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?0.1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
            }
          }
          if(shift){
            lastTime = millis();
            menuState = !menuState;
          }
          if(menu_Press){
            lastTime  =  millis();
            return;
          }
        }
        break;
      //editing the rain parameters
      case 1:
        switch(cursor){
          case 1:
            //changing octave range
            while(counterA != 0){
              if(counterA > 0 && maxOct<8){
                maxOct++;
              }
              if(counterA < 0 && maxOct>1){
                maxOct--;
              }
              if(maxOct<minOct){
                minOct = maxOct-1;
              }
              counterA += counterA<0?1:-1;
            }
            while(counterB != 0){
              if(counterB > 0 && minOct<7){
                minOct++;
              }
              if(counterB < 0 && minOct>0){
                minOct--;
              }
              if(minOct>maxOct){
                maxOct = minOct+1;
              }
              counterB += counterB<0?1:-1;
            }
            break;
        }
        if(itsbeen(100)){
          if(y != 0){
            if(y == 1 && cursor<3){
              cursor++;
              lastTime = millis();
            }
            else if(y == -1 && cursor>0){
              cursor--;
              lastTime = millis();
            }
          }
        }
        if(itsbeen(200)){
          if(sel){
            lastTime = millis();
            switch(cursor){
              case 0:
                pitchList = selectKeys(startPitch);
                break;
              case 1:
                break;
            }
          }
          if(shift || menu_Press){
            lastTime = millis();
            menuState = !menuState;
          }
        }
        break;
    }
    //--------------------

    //rendering drops
    display.clearDisplay();

    //drawing cloud bounds
    display.drawPixel(xCoord-xVariance,0,1);
    display.drawPixel(xCoord+xVariance,0,1);

    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render();
    }
    //loop for updating and playing sound from each raindrop
    for(uint8_t i = 0; i<drops.size(); i++){
      if(!drops[i].update()){
        drops.erase(drops.begin()+i,drops.begin()+i+1);
        continue;
      }
      if(drops[i].madeSound){
        //if the user hasn't selected a pitch
        if(pitchList.size() == 0){
          sendMIDInoteOn(startPitch,drops[i].vel,channel);
          sendMIDInoteOff(startPitch,0,channel);
          printSmall(drops[i].x1,59,pitchToString(startPitch,true,true),1);
        }
        else{
          uint8_t pitch = positionToPitch(pitchList,minOct,maxOct, drops[i].x1);
          sendMIDInoteOn(pitch,drops[i].vel,channel);
          sendMIDInoteOff(pitch,0,channel);
          printSmall(drops[i].x1,59,pitchToString(pitch,true,true),1);
        }
        // display.drawBitmap(drops[i].x1-5,58,splash_bmp,11,5,1);
      }
    }

    //info
    display.drawBitmap(0,0,drip_bmp,7,10,1);
    printSmall(8,3,stringify(stormIntensity),1);
    printPitchList(pitchList,startPitch);

    for(uint8_t i = 0; i<abs(maxOct-minOct); i++){
      display.drawPixel(i*screenWidth/abs(maxOct-minOct),63,1);
    }

    //drawing menus
    const uint8_t menuWidth = 34;
    if(menuState || menuOffset < menuWidth){
      display.fillRoundRect(-2-menuOffset,8,menuWidth,56,3,0);
      display.drawRoundRect(-2-menuOffset,8,menuWidth,56,3,1);
      printSmall(-menuOffset+5,10,"$"+pitchToString(startPitch,false,true),1);
      printSmall(-menuOffset,16,"octaves:",1);
      printSmall(2-menuOffset,22,stringify(minOct)+" to "+stringify(maxOct),1);
      printSmall(-menuOffset,28,"vel:",1);
      printSmall(2-menuOffset,34,stringify((stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance))+" to "+stringify(stormIntensity+intensityVariance),1);
      switch(cursor){
        //pitchlist
        case 0:
          drawArrow(menuWidth-menuOffset-3+sin(millis()/200),12,3,1,false);
          break;
        case 1:
          drawArrow(menuWidth-menuOffset-3+sin(millis()/200),24,3,1,false);
          break;
        case 2:
          drawArrow(menuWidth-menuOffset-3+sin(millis()/200),36,3,1,false);
          break;
      }
    }
    display.display();

    //sliding menus in and out
    if(!menuState && menuOffset<menuWidth){
      menuOffset+=7;
    }
    if(menuState&&menuOffset>0){
      menuOffset-=7;
    }

    //making new drops
    for(uint8_t i = 0; i<random(0,maxDrops); i++){
      if(drops.size()<maxDrops)
        drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
    }
  }
}
