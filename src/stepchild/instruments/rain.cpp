// 'rainSplash', 11x5px
const unsigned char splash_bmp [] = {
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x60, 0xc0
};
// 'drip', 7x10px
const unsigned char drip_bmp [] = {
	0x10, 0x10, 0x28, 0x44, 0x92, 0xa2, 0xa2, 0x82, 0x44, 0x38
};

class Raindrop{
  public:
    uint8_t length;
    int8_t x1;
    float y1;
    float vel;//pixels/frame
    bool madeSound;
    Raindrop();
    Raindrop(uint8_t,uint8_t,uint8_t);
    void render(bool);
    //draws a bitmap instead of the droplet
    void render(const unsigned char * bmp,uint8_t w, uint8_t h);
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
  vel = pow(float(random(float(minVel),float(maxVel))),1.1);
  length = vel+3;
  x1 = xPos;
  y1 = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
void Raindrop::render(bool text){
  display.drawFastVLine(x1,y1,length,1);
  if(text)
    printSmall(x1+2,y1,stringify(vel),1);
}
void Raindrop::render(const unsigned char * bmp,uint8_t w, uint8_t h){
  display.drawBitmap(x1,y1,bmp,w,h,1);
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
uint8_t positionToPitch(vector<uint8_t> pitchList, uint8_t startPitch, int8_t minOct, int8_t maxOct, uint8_t xCoord){
  if(!pitchList.size())
    return 0;
  float pxPerOctave = float(screenWidth)/float(abs(maxOct-minOct));
  float pxPerNote = pxPerOctave/float(pitchList.size());
                    
  //the octave of the note is it's position divided by pixels/octave
  int8_t octave = float(xCoord)/pxPerOctave+minOct;
  //the pitch of the note is it's position (relative to the start of the octave) divided by pixels/note
  uint8_t pitch =  float(xCoord%uint8_t(pxPerOctave))/pxPerNote;
  if(pitchList.size() == 0)
    return startPitch+12*octave;
  else
    return pitchList[pitch]+12*octave;
}

uint8_t dropVelToNoteVel(float vel,uint8_t minVel, uint8_t maxVel){
  int16_t velocity = float(abs(maxVel-minVel))*vel+minVel;
  if(velocity>127)
    velocity = 127;
  else if(velocity<1)
    velocity = 1;
  return uint8_t(velocity);
}

int lowest(int a, int b){
  return a<b?a:b;
}
int highest(int a, int b){
  return a>b?a:b;
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
  uint8_t length = pitches.length()*4+4+2*countChar(pitches,'#');
  display.fillRoundRect(-2,-2,length,9,3,0);
  display.drawRoundRect(-2,-2,length,9,3,1);
  printSmall(0,x1,pitches,1);
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

  uint8_t minVel = 0;
  uint8_t maxVel = 127;

  //0 allows modulating the raindrops
  //1 allows editing the parameters
  bool menuState = true;
  uint8_t cursor = 0;
  int8_t menuOffset = 0;

  bool showingText = false;
  bool grabNotesFromPlaylist = false;

  vector<uint8_t> pitchList;

  bool isPlaying = true;
  while(true){
    //controls
    //--------------------
    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(play){
        lastTime = millis();
        isPlaying = !isPlaying;
      }
      if(n){
        lastTime = millis();
        pitchList = selectKeys(startPitch);
        lastTime = millis();
      }
    }
    switch(menuState){
      //messing with the rain as an instrument
      case 0:
        //B changes the intensity
        while(counterB != 0){
          if(counterB > 0 && stormIntensity<maxIntensity){
            stormIntensity++;
            maxDrops++;
          }
          else if(counterB < 0 && stormIntensity>1){
            stormIntensity--;
            maxDrops--;
          }
          counterB += counterB<0?1:-1;;
        }
        //A changes the width/spread
        while(counterA != 0){
          if(shift){
            if(counterA>0 && startPitch<127){
              startPitch++;
            }
            else if(counterA<0 && startPitch>0){
              startPitch--;
            }
          }
          else{
            if(counterA > 0 && xVariance<screenWidth){
              xVariance+=3;
            }
            if(counterA < 0 && xVariance>10){
              xVariance-=3;
            }
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
          if(shift){
            lastTime = millis();
            menuState = !menuState;
          }
          if(menu_Press){
            lastTime = millis();
            menuState = !menuState;
          }
        }
        break;
      //editing the rain parameters
      case 1:
        if(itsbeen(100)){
          if(x != 0){
            lastTime = millis();
            menuState = false;
          }
        }
        switch(cursor){
          //changing startPitch w/encoders
          case 0:
            while(counterA != 0){
              if(counterA>0 && startPitch<127){
                startPitch++;
              }
              else if(counterA<0 && startPitch>0){
                startPitch--;
              }
              counterA += counterA<0?1:-1;
            }
            while(counterB != 0){
              if(counterB>0 && startPitch<127){
                startPitch++;
              }
              else if(counterB<0 && startPitch>0){
                startPitch--;
              }
              counterB += counterB<0?1:-1;
            }
            break;
          //changing octave range
          case 1:
            while(counterB != 0){
              if(counterB > 0 && maxOct<8){
                maxOct++;
              }
              if(counterB < 0 && maxOct>1){
                maxOct--;
              }
              if(maxOct<minOct){
                minOct = maxOct-1;
              }
              counterB += counterB<0?1:-1;
            }
            while(counterA != 0){
              if(counterA > 0 && minOct<7){
                minOct++;
              }
              if(counterA < 0 && minOct>0){
                minOct--;
              }
              if(minOct>maxOct){
                maxOct = minOct+1;
              }
              counterA += counterA<0?1:-1;
            }
            break;
          //changing velocity range
          case 2:
            while(counterB != 0){
              if(counterB > 0 && maxVel<127){
                maxVel++;
              }
              if(counterB < 0 && maxVel>1){
                maxVel--;
              }
              if(maxVel<minVel){
                minVel = maxVel-1;
              }
              counterB += counterB<0?1:-1;
            }
            while(counterA != 0){
              if(counterA > 0 && minVel<126){
                minVel++;
              }
              if(counterA < 0 && minVel>0){
                minVel--;
              }
              if(minVel>maxVel){
                maxVel = minVel+1;
              }
              counterA += counterA<0?1:-1;
            }
            break;
        }
        if(itsbeen(100)){
          if(y != 0){
            if(y == 1 && cursor<5){
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
                lastTime = millis();
                break;
              case 1:
                break;
              case 2:
                break;
              case 3:
                grabNotesFromPlaylist = !grabNotesFromPlaylist;
                break;
              case 4:
                showingText = !showingText;
                break;
              case 5:
                return;
            }
          }
          if(menu_Press){
            if(menuState && cursor != 5){
              cursor = 5;
              lastTime = millis();
            }
            else{
              menu_Press = false;
              lastTime = millis();
              return;
            }
          }
        }
        break;
    }
    //----------------------------

    display.clearDisplay();

    //drawing cloud bounds
    display.drawPixel(xCoord-xVariance,0,1);
    display.drawPixel(xCoord+xVariance,0,1);

    //draw each raindrop
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render(showingText);
    }
    //loop for updating and playing sound from each raindrop
    if(isPlaying){
      bool leds[8] = {false,false,false,false,false,false,false,false};
      for(uint8_t i = 0; i<drops.size(); i++){
        if(!drops[i].update()){
          drops.erase(drops.begin()+i,drops.begin()+i+1);
          continue;
        }
        if(drops[i].madeSound && pitchList.size()){
          uint8_t pitch = positionToPitch(pitchList,startPitch,minOct,maxOct,drops[i].x1);
          sendMIDInoteOn(pitch,dropVelToNoteVel(drops[i].vel/float(stormIntensity+intensityVariance),minVel,maxVel),channel);
          sendMIDInoteOff(pitch,0,channel);
          if(showingText)
            printSmall(drops[i].x1,59,pitchToString(pitch,true,true),1);
          else
            display.drawBitmap(drops[i].x1-5,59,splash_bmp,11,5,1);

          leds[drops[i].x1/16] = true;
        }
      }
      writeLEDs(leds);
    }

    //info
    String intensity = stringify(stormIntensity);
    display.fillRoundRect(-2,-2,18+intensity.length()*4,14,3,0);
    display.drawRoundRect(-2,-2,18+intensity.length()*4,14,3,1);
    if(isPlaying)//play icon
      display.fillTriangle(0,7,0,3,4,5,SSD1306_WHITE);
    else if(((millis())%200)>100){//pause icon
      display.fillRect(0,2,2,7,1);
      display.fillRect(4,2,2,7,1);
    }
    display.drawBitmap(7,0,drip_bmp,7,10,1);
    printSmall(15,3,intensity,1);
    printPitchList(pitchList,startPitch);

    //drawing menus
    const uint8_t menuWidth = 40;
    if(menuState || menuOffset < menuWidth){
      const uint8_t spacing = 6;
      const uint8_t y1 = 2;
      display.fillRoundRect(-2-menuOffset,y1-2,menuWidth,57,3,0);
      display.drawRoundRect(-2-menuOffset,y1-2,menuWidth,57,3,1);
      printSmall(-menuOffset,y1,"$"+pitchToString(startPitch,false,true)+"[sel]",1);
      printSmall(-menuOffset,y1+spacing,"octaves:",1);
      printSmall(2-menuOffset,y1+spacing*2,stringify(minOct)+" to "+stringify(maxOct),1);
      printSmall(-menuOffset,y1+spacing*3,"vel:",1);
      printSmall(2-menuOffset,y1+spacing*4,stringify(minVel)+" to "+stringify(maxVel),1);
      drawLabel(18-menuOffset,y1+spacing*5+2,"from seq",grabNotesFromPlaylist);
      drawLabel(18-menuOffset,y1+spacing*6+6,"show txt",showingText);
      drawLabel(18-menuOffset,y1+spacing*7+10,"exit",cursor==5);
      switch(cursor){
        //pitchlist
        case 0:
          drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2,3,1,false);
          break;
        //octaves
        case 1:
          drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*2,3,1,false);
          for(uint8_t i = 1; i<abs(maxOct-minOct); i++){
            display.drawFastVLine(i*screenWidth/abs(maxOct-minOct),24,16,1);
          }
          break;
        //vel
        case 2:
          drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*4,3,1,false);
          break;
        //grab notes from playlist (wip)
        case 3:
          drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*5+2,3,1,false);
          break;
        //show numbers
        case 4:
          drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*6+6,3,1,false);
          break;
        //exit
        case 5:
          drawArrow(menuWidth-menuOffset-10+((millis()/400)%2),y1+2+spacing*7+10,3,1,false);
          break;
      }
    }
    display.display();

    //sliding menus in and out
    if(!menuState && menuOffset<menuWidth){
      menuOffset+=10;
    }
    if(menuState&&menuOffset>0){
      menuOffset-=10;
    }

    //making new drops
    for(uint8_t i = 0; i<random(0,maxDrops); i++){
      if(drops.size()<maxDrops)
        // drops.push_back(Raindrop(random(highest(float(xCoord-xVariance),0),lowest(float(xCoord+xVariance),128)),(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
        drops.push_back(Raindrop(random(highest(float(xCoord-xVariance),0),lowest(float(xCoord+xVariance),128)),stormIntensity+intensityVariance,(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance)));
        // drops.push_back(Raindrop(random(highest(float(xCoord-xVariance),0),lowest(float(xCoord+xVariance),128)),1,1));
    }
    if(grabNotesFromPlaylist){
      pitchList = getUnionPitchList();
      //removes octave from each pitch
      filterOutUnisonNotes(pitchList);
      sort(pitchList.begin(),pitchList.end());
    }
  }
  turnOffLEDs();
}
