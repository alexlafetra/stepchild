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
  vel = pow(float(random(minVel,maxVel))/float(5),2);
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

//gradient on top representing rain flow, moveable 
//drops velocities correspond to note vel
//X axis is pitch bend? or pitch? and y-axis is intensity (can also be controlled with encoders)
void rain(){
  vector<Raindrop> drops;
  //dropsPerFrame and stormIntensity both increase as the storm gets worse
  uint8_t stormIntensity = 5;
  uint8_t maxDrops = 10;
  const uint8_t maxIntensity = 20;
  uint8_t xCoord = 64;
  const uint8_t intensityVariance = 5;
  const uint8_t xVariance = 20;
  uint8_t frames = 0;
  while(true){
    readButtons();
    joyRead();
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
    while(counterA != 0){
      if(counterA >= 1 && xCoord<(screenWidth-xVariance)){
        xCoord++;
      }
      else if(xCoord>xVariance){
        xCoord--;
      }
      counterA += counterA<0?1:-1;
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        for(uint8_t i = 0; i<20; i++){
          if(drops.size()<maxDrops)
            drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?0.1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
        }
      }
      if(menu_Press){
        lastTime  =  millis();
        return;
      }
    }
    //rendering drops
    display.clearDisplay();
    printSmall(0,0,stringify(stormIntensity),1);
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render();
    }
    display.display();
    for(uint8_t i = 0; i<drops.size(); i++){
      if(!drops[i].update()){
        drops.erase(drops.begin()+i,drops.begin()+i+1);
        continue;
      }
      if(drops[i].madeSound){
        sendMIDInoteOn(60,drops[i].vel,1);
        sendMIDInoteOff(60,0,1);
      }
    }
    //making new drops
    for(uint8_t i = 0; i<random(0,maxDrops); i++){
      if(drops.size()<maxDrops)
        drops.push_back(Raindrop(random(xCoord-xVariance,xCoord+xVariance),(stormIntensity<=intensityVariance)?1:(stormIntensity-intensityVariance),stormIntensity+intensityVariance));
    }
  }
}