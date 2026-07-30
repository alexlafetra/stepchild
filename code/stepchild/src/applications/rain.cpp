#include "Stepchild.h"

#include "Arduino.h"
#include <algorithm>
#include "stringPatch.h"
#include "guiUtilities.h"
#include "utils.h"


;

using namespace std;

// 'rainSplash', 11x5px
const unsigned char splash_bmp [] = {
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x60, 0xc0
};
// 'drip', 7x10px
const unsigned char drip_bmp [] = {
	0x10, 0x10, 0x28, 0x44, 0x92, 0xa2, 0xa2, 0x82, 0x44, 0x38
};
// 'drip_mask', 5x7px
const unsigned char drip_mask_bmp [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void filterOutUnisonNotes(std::vector<uint8_t>& notes){
  std::vector<uint8_t> uniqueNotes;
  for(uint8_t i = 0; i<notes.size(); i++){
    if(!uniqueNotes.size())
      uniqueNotes.push_back(notes[i]%12);
    else{
      if(find(uniqueNotes.begin(),uniqueNotes.end(),notes[i]%12) == uniqueNotes.end()){
        uniqueNotes.push_back(notes[i]%12);
      }
    }
  }
}

#include "applications/Raindrop.h"

Raindrop::Raindrop(){
  vel = 0.7;
  length = vel*2;
  this->x = random(0,stepchild.SCREEN_WIDTH);
  this->y = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
Raindrop::Raindrop(uint8_t xPos, uint8_t maxVel, uint8_t minVel){
  vel = random(float(minVel),float(maxVel))/5.0;
  length = vel+3;
  this->x = xPos;
  this->y = -length;//starts so the bottom of the line is on the top pixel of the screen
  madeSound = false;
}
void Raindrop::render(bool text){
  stepchild.display.drawFastVLine(this->x,this->y,length,1);
  if(text)
    graphics.printSmall(this->x+2,this->y,stringify(vel),1);
}
void Raindrop::render(const unsigned char * bmp,uint8_t w, uint8_t h){
  stepchild.display.drawBitmap(this->x,this->y,bmp,w,h,1);
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
  if(this->y>=stepchild.SCREEN_HEIGHT+length){
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
  float pxPerOctave = float(stepchild.SCREEN_WIDTH)/max(float(abs(maxOct-minOct)),1);
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
    pitches+=stepchild.pitchToString(startPitch,false,true);
  }
  else{
    for(uint8_t pitch = 0; pitch<pitchList.size(); pitch++){
      pitches+=stepchild.pitchToString(pitchList[pitch],false,true);
    }
  }
  stepchild.display.setRotation(DISPLAY_SIDEWAYS_R);
  uint8_t length = pitches.length()*4+4+2*countChar(pitches,'#');
  stepchild.display.fillRoundRect(-2,-2,length,9,3,0);
  stepchild.display.drawRoundRect(-2,-2,length,9,3,1);
  graphics.printSmall(0,x1,pitches,1);
  stepchild.display.setRotation(DISPLAY_UPRIGHT);
}

//gradient on top representing rain flow, moveable 
//drops velocities correspond to note vel
//X axis is pitch bend? or pitch? and y-axis is intensity (can also be controlled with encoders)
bool rain(){
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
  vector<uint8_t> pitchList = stepchild.makePitchListFromScale(MAJOR,0);

  bool isPlaying = true;
  while(true){
    //controls
    //--------------------
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.PLAY()){
        stepchild.lastTime = millis();
        isPlaying = !isPlaying;
      }
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        pitchList = selectKeys(startPitch);
        stepchild.lastTime = millis();
      }
    }
    //messing with the rain as an instrument
    if(!menuState){
      //B changes the intensity
      while(stepchild.buttons.counterB != 0){
        if(stepchild.buttons.counterB > 0 && stormIntensity<maxIntensity){
          stormIntensity++;
          maxDrops++;
        }
        else if(stepchild.buttons.counterB < 0 && stormIntensity>1){
          stormIntensity--;
          maxDrops--;
        }
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
      }
      //A changes the width/spread
      while(stepchild.buttons.counterA != 0){
        if(stepchild.buttons.SHIFT()){
          if(stepchild.buttons.counterA>0 && startPitch<127){
            startPitch++;
          }
          else if(stepchild.buttons.counterA<0 && startPitch>0){
            startPitch--;
          }
        }
        else{
          if(stepchild.buttons.counterA > 0 && xVariance<stepchild.SCREEN_WIDTH){
            xVariance+=3;
          }
          if(stepchild.buttons.counterA < 0 && xVariance>10){
            xVariance-=3;
          }
        }
        stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
      }
      //X joystick changes the centerPoint
      if(stepchild.buttons.joystickX == -1 && xCoord<stepchild.SCREEN_WIDTH){
        xCoord+=2;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickX == 1 && xCoord>0){
        xCoord-=2;
        stepchild.lastTime = millis();
      }
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.SHIFT()){
          stepchild.lastTime = millis();
          menuState = !menuState;
        }
        if(stepchild.buttons.MENU()){
          stepchild.lastTime = millis();
          menuState = !menuState;
        }
      }
    }
    //editing the rain parameters
    else{
      if(stepchild.itsbeen(100)){
        if(stepchild.buttons.joystickX != 0){
          stepchild.lastTime = millis();
          menuState = false;
        }
      }
      switch(cursor){
          //changing startPitch w/encoders
        case 0:
          while(stepchild.buttons.counterA != 0){
            if(stepchild.buttons.counterA>0 && startPitch<127){
              startPitch++;
            }
            else if(stepchild.buttons.counterA<0 && startPitch>0){
              startPitch--;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
          }
          while(stepchild.buttons.counterB != 0){
            if(stepchild.buttons.counterB>0 && startPitch<127){
              startPitch++;
            }
            else if(stepchild.buttons.counterB<0 && startPitch>0){
              startPitch--;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
          }
          break;
          //changing octave range
        case 1:
          while(stepchild.buttons.counterB != 0){
            if(stepchild.buttons.counterB > 0 && maxOct<8){
              maxOct++;
            }
            if(stepchild.buttons.counterB < 0 && maxOct>1){
              maxOct--;
            }
            if(maxOct<minOct){
              minOct = maxOct-1;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
          }
          while(stepchild.buttons.counterA != 0){
            if(stepchild.buttons.counterA > 0 && minOct<7){
              minOct++;
            }
            if(stepchild.buttons.counterA < 0 && minOct>0){
              minOct--;
            }
            if(minOct>maxOct){
              maxOct = minOct+1;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
          }
          break;
          //changing velocity range
        case 2:
          while(stepchild.buttons.counterB != 0){
            if(stepchild.buttons.counterB > 0 && maxVel<127){
              maxVel++;
            }
            if(stepchild.buttons.counterB < 0 && maxVel>1){
              maxVel--;
            }
            if(maxVel<minVel){
              minVel = maxVel-1;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
          }
          while(stepchild.buttons.counterA != 0){
            if(stepchild.buttons.counterA > 0 && minVel<126){
              minVel++;
            }
            if(stepchild.buttons.counterA < 0 && minVel>0){
              minVel--;
            }
            if(minVel>maxVel){
              maxVel = minVel+1;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
          }
          break;
      }
      if(stepchild.itsbeen(100)){
        if(stepchild.buttons.joystickY != 0){
          if(stepchild.buttons.joystickY == 1 && cursor<5){
            cursor++;
            stepchild.lastTime = millis();
          }
          else if(stepchild.buttons.joystickY == -1 && cursor>0){
            cursor--;
            stepchild.lastTime = millis();
          }
        }
      }
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.SELECT() ){
          stepchild.lastTime = millis();
          switch(cursor){
            case 0:
              pitchList = selectKeys(startPitch);
              stepchild.lastTime = millis();
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
              return true;
          }
        }
        if(stepchild.buttons.MENU()){
          if(menuState && cursor != 5){
            cursor = 5;
            stepchild.lastTime = millis();
          }
          else{
            menuState = false;
            stepchild.lastTime = millis();
          }
        }
      }
    }
    //----------------------------

    stepchild.display.clearDisplay();

    //drawing cloud bounds
    stepchild.display.drawPixel(xCoord-xVariance,0,1);
    stepchild.display.drawPixel(xCoord+xVariance,0,1);

    //draw each raindrop
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render(showingText);
    }
    //loop for updating and playing sound from each raindrop
    if(isPlaying){
      uint16_t leds = 0;
      for(uint8_t i = 0; i<drops.size(); i++){
        if(!drops[i].update()){
          drops.erase(drops.begin()+i,drops.begin()+i+1);
          continue;
        }
        if(drops[i].madeSound && pitchList.size()){
          uint8_t pitch = positionToPitch(pitchList,startPitch,minOct,maxOct,drops[i].x);
          stepchild.midi.noteOn(pitch,dropVelToNoteVel(drops[i].vel/float(stormIntensity+intensityVariance),minVel,maxVel),channel);
          stepchild.midi.noteOff(pitch,0,channel);
          if(showingText)
            graphics.printSmall(drops[i].x,59,stepchild.pitchToString(pitch,true,true),1);
          else
            stepchild.display.drawBitmap(drops[i].x-5,59,splash_bmp,11,5,1);

          leds |= 1<<(drops[i].x/8);
        }
      }
      stepchild.buttons.writeLEDs(leds);
      // writeLEDs(leds);
    }

    //info
    String intensity = stringify(stormIntensity);
    // stepchild.display.fillRoundRect(-2,-2,18+intensity.length()*4,14,3,0);
    // stepchild.display.drawRoundRect(-2,-2,18+intensity.length()*4,14,3,1);
    if(isPlaying)//play icon
      stepchild.display.fillTriangle(0,7,0,3,4,5,SSD1306_WHITE);
    else if(((millis())%200)>100){//pause icon
      stepchild.display.fillRect(0,2,2,7,1);
      stepchild.display.fillRect(4,2,2,7,1);
    }
    stepchild.display.drawBitmap(7,0,drip_bmp,7,10,1);
    graphics.printSmall(15,3,intensity,1);
    printPitchList(pitchList,startPitch);

    //drawing menus
    const uint8_t menuWidth = 40;
    if(menuState || menuOffset < menuWidth){
      const uint8_t spacing = 6;
      const uint8_t y1 = 2;
      stepchild.display.fillRoundRect(-2-menuOffset,y1-2,menuWidth,57,3,0);
      stepchild.display.drawRoundRect(-2-menuOffset,y1-2,menuWidth,57,3,1);
      graphics.printSmall(-menuOffset,y1,"$"+stepchild.pitchToString(startPitch,false,true)+"[SEL]",1);
      graphics.printSmall(-menuOffset,y1+spacing,"octaves:",1);
      graphics.printSmall(2-menuOffset,y1+spacing*2,stringify(minOct)+" to "+stringify(maxOct),1);
      graphics.printSmall(-menuOffset,y1+spacing*3,"vel:",1);
      graphics.printSmall(2-menuOffset,y1+spacing*4,stringify(minVel)+" to "+stringify(maxVel),1);
      graphics.drawLabel(18-menuOffset,y1+spacing*5+2,"from seq",grabNotesFromPlaylist);
      graphics.drawLabel(18-menuOffset,y1+spacing*6+6,"show txt",showingText);
      graphics.drawLabel(18-menuOffset,y1+spacing*7+10,"quit",cursor==5);
      switch(cursor){
        //pitchlist
        case 0:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2,3,ARROW_LEFT,false);
          break;
        //octaves
        case 1:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*2,3,ARROW_LEFT,false);
          for(uint8_t i = 1; i<abs(maxOct-minOct); i++){
            stepchild.display.drawFastVLine(i*stepchild.SCREEN_WIDTH/abs(maxOct-minOct),24,16,1);
          }
          break;
        //vel
        case 2:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*4,3,ARROW_LEFT,false);
          break;
        //grab notes from playlist (wip)
        case 3:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*5+2,3,ARROW_LEFT,false);
          break;
        //show numbers
        case 4:
          graphics.drawArrow(menuWidth-menuOffset-3+((millis()/400)%2),y1+2+spacing*6+6,3,ARROW_LEFT,false);
          break;
        //exit
        case 5:
          graphics.drawArrow(menuWidth-menuOffset-10+((millis()/400)%2),y1+2+spacing*7+10,3,ARROW_LEFT,false);
          break;
      }
    }
    stepchild.display.display();

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
        drops.push_back(Raindrop(random(highest(float(xCoord-xVariance),0),lowest(float(xCoord+xVariance),128)),stormIntensity+intensityVariance,(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance)));
    }
    if(grabNotesFromPlaylist){
      pitchList = stepchild.getAllActivePitches();
      //removes octave from each pitch
      filterOutUnisonNotes(pitchList);
      std::sort(pitchList.begin(),pitchList.end());
    }
  }
  return true;
}
