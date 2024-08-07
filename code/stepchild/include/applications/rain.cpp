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
    int8_t x;
    float y;
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
  this->x = random(0,screenWidth);
  this->y = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
Raindrop::Raindrop(uint8_t xPos, uint8_t maxVel, uint8_t minVel){
  vel = pow(float(random(float(minVel),float(maxVel))),1.1);
  length = vel+3;
  this->x = xPos;
  this->y = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
void Raindrop::render(bool text){
  display.drawFastVLine(this->x,this->y,length,1);
  if(text)
    printSmall(this->x+2,this->y,stringify(vel),1);
}
void Raindrop::render(const unsigned char * bmp,uint8_t w, uint8_t h){
  display.drawBitmap(this->x,this->y,bmp,w,h,1);
}

bool Raindrop::update(){
  bool  hasNotCrossedYet = false;
  if(this->y<64){
    hasNotCrossedYet = true;
  }
  this->y+=vel;
  if(this->y>64 &&  hasNotCrossedYet){
    madeSound = true;
  }
  else{
    madeSound = false;
  }
  if(this->y>=screenHeight+length){
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
  display.setRotation(DISPLAY_SIDEWAYS_R);
  uint8_t length = pitches.length()*4+4+2*countChar(pitches,'#');
  display.fillRoundRect(-2,-2,length,9,3,0);
  display.drawRoundRect(-2,-2,length,9,3,1);
  printSmall(0,x1,pitches,1);
  display.setRotation(DISPLAY_UPRIGHT);
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

  //start off w/ C major scale
  vector<uint8_t> pitchList = genScale(MAJOR,0);

  bool isPlaying = true;
  while(true){
    //controls
    //--------------------
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.PLAY()){
        lastTime = millis();
        isPlaying = !isPlaying;
      }
      if(controls.NEW()){
        lastTime = millis();
        pitchList = selectKeys(startPitch);
        lastTime = millis();
      }
    }
    switch(menuState){
      //messing with the rain as an instrument
      case 0:
        //B changes the intensity
        while(controls.counterB != 0){
          if(controls.counterB > 0 && stormIntensity<maxIntensity){
            stormIntensity++;
            maxDrops++;
          }
          else if(controls.counterB < 0 && stormIntensity>1){
            stormIntensity--;
            maxDrops--;
          }
          controls.counterB += controls.counterB<0?1:-1;;
        }
        //A changes the width/spread
        while(controls.counterA != 0){
          if(controls.SHIFT()){
            if(controls.counterA>0 && startPitch<127){
              startPitch++;
            }
            else if(controls.counterA<0 && startPitch>0){
              startPitch--;
            }
          }
          else{
            if(controls.counterA > 0 && xVariance<screenWidth){
              xVariance+=3;
            }
            if(controls.counterA < 0 && xVariance>10){
              xVariance-=3;
            }
          }
          controls.counterA += controls.counterA<0?1:-1;
        }
        //X joystick changes the centerPoint
        if(controls.joystickX == -1 && xCoord<screenWidth){
          xCoord+=2;
          lastTime = millis();
        }
        else if(controls.joystickX == 1 && xCoord>0){
          xCoord-=2;
          lastTime = millis();
        }
        if(utils.itsbeen(200)){
          if(controls.SHIFT()){
            lastTime = millis();
            menuState = !menuState;
          }
          if(controls.MENU()){
            lastTime = millis();
            menuState = !menuState;
          }
        }
        break;
      //editing the rain parameters
      case 1:
        if(utils.itsbeen(100)){
          if(controls.joystickX != 0){
            lastTime = millis();
            menuState = false;
          }
        }
        switch(cursor){
          //changing startPitch w/encoders
          case 0:
            while(controls.counterA != 0){
              if(controls.counterA>0 && startPitch<127){
                startPitch++;
              }
              else if(controls.counterA<0 && startPitch>0){
                startPitch--;
              }
              controls.counterA += controls.counterA<0?1:-1;
            }
            while(controls.counterB != 0){
              if(controls.counterB>0 && startPitch<127){
                startPitch++;
              }
              else if(controls.counterB<0 && startPitch>0){
                startPitch--;
              }
              controls.counterB += controls.counterB<0?1:-1;
            }
            break;
          //changing octave range
          case 1:
            while(controls.counterB != 0){
              if(controls.counterB > 0 && maxOct<8){
                maxOct++;
              }
              if(controls.counterB < 0 && maxOct>1){
                maxOct--;
              }
              if(maxOct<minOct){
                minOct = maxOct-1;
              }
              controls.counterB += controls.counterB<0?1:-1;
            }
            while(controls.counterA != 0){
              if(controls.counterA > 0 && minOct<7){
                minOct++;
              }
              if(controls.counterA < 0 && minOct>0){
                minOct--;
              }
              if(minOct>maxOct){
                maxOct = minOct+1;
              }
              controls.counterA += controls.counterA<0?1:-1;
            }
            break;
          //changing velocity range
          case 2:
            while(controls.counterB != 0){
              if(controls.counterB > 0 && maxVel<127){
                maxVel++;
              }
              if(controls.counterB < 0 && maxVel>1){
                maxVel--;
              }
              if(maxVel<minVel){
                minVel = maxVel-1;
              }
              controls.counterB += controls.counterB<0?1:-1;
            }
            while(controls.counterA != 0){
              if(controls.counterA > 0 && minVel<126){
                minVel++;
              }
              if(controls.counterA < 0 && minVel>0){
                minVel--;
              }
              if(minVel>maxVel){
                maxVel = minVel+1;
              }
              controls.counterA += controls.counterA<0?1:-1;
            }
            break;
        }
        if(utils.itsbeen(100)){
          if(controls.joystickY != 0){
            if(controls.joystickY == 1 && cursor<5){
              cursor++;
              lastTime = millis();
            }
            else if(controls.joystickY == -1 && cursor>0){
              cursor--;
              lastTime = millis();
            }
          }
        }
        if(utils.itsbeen(200)){
          if(controls.SELECT() ){
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
          if(controls.MENU()){
            if(menuState && cursor != 5){
              cursor = 5;
              lastTime = millis();
            }
            else{
              menuState = false;
              lastTime = millis();
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
      // bool leds[8] = {false,false,false,false,false,false,false,false};
      uint16_t leds = 0;
      for(uint8_t i = 0; i<drops.size(); i++){
        if(!drops[i].update()){
          drops.erase(drops.begin()+i,drops.begin()+i+1);
          continue;
        }
        if(drops[i].madeSound && pitchList.size()){
          uint8_t pitch = positionToPitch(pitchList,startPitch,minOct,maxOct,drops[i].x);
          MIDI.noteOn(pitch,dropVelToNoteVel(drops[i].vel/float(stormIntensity+intensityVariance),minVel,maxVel),channel);
          MIDI.noteOff(pitch,0,channel);
          if(showingText)
            printSmall(drops[i].x,59,pitchToString(pitch,true,true),1);
          else
            display.drawBitmap(drops[i].x-5,59,splash_bmp,11,5,1);

          leds |= 1<<(drops[i].x/8);
        }
      }
      controls.writeLEDs(leds);
      // writeLEDs(leds);
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
      printSmall(-menuOffset,y1,"$"+pitchToString(startPitch,false,true)+"[controls.SELECT() ]",1);
      printSmall(-menuOffset,y1+spacing,"octaves:",1);
      printSmall(2-menuOffset,y1+spacing*2,stringify(minOct)+" to "+stringify(maxOct),1);
      printSmall(-menuOffset,y1+spacing*3,"vel:",1);
      printSmall(2-menuOffset,y1+spacing*4,stringify(minVel)+" to "+stringify(maxVel),1);
      graphics.drawLabel(18-menuOffset,y1+spacing*5+2,"from seq",grabNotesFromPlaylist);
      graphics.drawLabel(18-menuOffset,y1+spacing*6+6,"show txt",showingText);
      graphics.drawLabel(18-menuOffset,y1+spacing*7+10,"quit",cursor==5);
      switch(cursor){
        //pitchlist
        case 0:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2,3,1,false);
          break;
        //octaves
        case 1:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*2,3,1,false);
          for(uint8_t i = 1; i<abs(maxOct-minOct); i++){
            display.drawFastVLine(i*screenWidth/abs(maxOct-minOct),24,16,1);
          }
          break;
        //vel
        case 2:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*4,3,1,false);
          break;
        //grab notes from playlist (wip)
        case 3:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*5+2,3,1,false);
          break;
        //show numbers
        case 4:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*6+6,3,1,false);
          break;
        //exit
        case 5:
          graphics.drawArrow(menuWidth-menuOffset-10+((millis()/400)%2),y1+2+spacing*7+10,3,1,false);
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
  controls.turnOffLEDs();
}
