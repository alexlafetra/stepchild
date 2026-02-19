class Moon{
  public:
    //the target planet, as an index in the solar system
    Vertex loc;
    uint8_t size;
    Moon();
    Moon(Vertex,uint8_t);
    void rotate(float,uint8_t);
    void draw(uint8_t, uint8_t, float);
};

Moon::Moon(){

}
Moon::Moon(Vertex v, uint8_t s){
  loc = v;
  size = s;
}

void Moon::rotate(float angle, uint8_t axis){
  loc.rotate(angle,axis);
}
//vert p is the planet
//translates the moon to the loc of the planet
void Moon::draw(uint8_t xOff, uint8_t yOff, float offset){
  Vertex temp = loc;
  temp.rotate(offset,0);
  temp.render(xOff,yOff,1,false);
}

class Planet{
  public:
    //each planet is a vertex
    Vertex loc;
    //size of the planet
    uint8_t size;
    //how many steps to complete a rotation
    uint16_t stepsPerPeriod = 6*random(1,8);
    //phase
    uint8_t currentStep = random(0,stepsPerPeriod);
    //midi pitch
    uint8_t pitch = 60;
    //if the planet is filled or nah
    bool fill;

    bool ring;

    bool sentNote = false;

    //distance from the sun
    float orbit;
    //list of moons
    vector<Planet> moons;

    Planet();
    Planet(Vertex,uint8_t,bool);
    Planet(Vertex,uint8_t);
    void rotate(float,uint8_t);
    void draw(uint8_t,uint8_t,float);
    void advanceOneStep();
    //Common util function for getting the distance between two vertices
    float get3Ddistance(Vertex v1, Vertex v2){
      return sqrt(pow(v1.x-v2.x,2)+pow(v1.y-v2.y,2)+pow(v1.z-v2.z,2));
    }
};
Planet::Planet(){
  loc = Vertex(0,0,0);
  size = 3;
  fill = false;
  orbit = get3Ddistance(loc,Vertex(0,0,0));
}
Planet::Planet(Vertex v, uint8_t s, bool f){
  loc = v;
  size = s;
  fill = f;
  orbit = get3Ddistance(loc,Vertex(0,0,0));
}
Planet::Planet(Vertex v, uint8_t s){
  loc = v;
  size = s;
  fill = true;
  orbit = get3Ddistance(loc,Vertex(0,0,0));
}
void Planet::rotate(float angle, uint8_t axis){
  loc.rotate(angle,axis);
}
void Planet::advanceOneStep(){
  float amount = 360.0 / float(stepsPerPeriod);
  rotate(amount,1);
  currentStep++;
  if(currentStep == stepsPerPeriod){
    currentStep = 0;
    MIDI.noteOn(pitch,127,0);
    MIDI.noteOff(pitch,127,0);
    sentNote = true;
  }
  else{
    sentNote = false;
  }
  for(uint8_t m = 0; m<moons.size(); m++){
    moons[m].advanceOneStep();
  }
}
void Planet::draw(uint8_t xOff, uint8_t yOff,float offset){
  // loc.render(xOff,yOff,1,size,fill);
  Vertex temp = loc;
  for(uint8_t i = 0; i<moons.size(); i++){
    //moon inherits planets position as its 0,0
    Vertex temp2 = moons[i].loc;
    temp2.x += temp.x;
    temp2.y += temp.y;
    temp2.z += temp.z;
    temp2.rotate(offset,0);
    temp2.render(xOff,yOff,1,moons[i].size,false);
  }
  temp.rotate(offset,0);
  temp.render(xOff,yOff,1,size,fill);
  // if(ring)
  //   graphics.drawEllipse(temp.x+xOff,temp.y+yOff,size+6,size,SSD1306_WHITE);
}

class SolarSystem{
  public:
    uint8_t xOff;
    uint8_t yOff;
    float angle;
    vector<Planet> planets;
    SolarSystem();
    SolarSystem(vector<Planet>);
    void rotate(float,uint8_t);
    void draw();
    void playOneStep();
};

SolarSystem::SolarSystem(){
}
SolarSystem::SolarSystem(vector<Planet> p){
  planets = p;
}
void SolarSystem::rotate(float a, uint8_t axis){
  for(uint8_t p = 0; p<planets.size(); p++){
    planets[p].rotate(a,axis);
    for(uint8_t m = 0; m<planets[p].moons.size(); m++){
      planets[p].moons[m].rotate(-a,axis);
    }
  }
}
void SolarSystem::playOneStep(){
  for(uint8_t p = 0; p<planets.size(); p++){
    planets[p].advanceOneStep();
  }
}

void SolarSystem::draw(){
  for(uint8_t p = 0; p<planets.size(); p++){
    planets[p].draw(xOff,yOff,angle);
  }
}

void drawPlanetHighlight(Planet &p){
  display.drawCircle(p.loc.x,p.loc.z,p.size+4+(millis()/200)%2,1);
}

bool testSolarSystemControls(){
  return true;
}

bool testSolarSystem(){
  Planet sun = Planet(Vertex(0,0,0),4,true);
  Planet mercury = Planet(Vertex(7.5,0,0),1,false);
  Planet venus = Planet(Vertex(15,0,0),2,false);
  Planet earth = Planet(Vertex(22.5,0,0),3,true);
  Planet jupiter = Planet(Vertex(30,0,0),5,true);
  //moon targeting earth
  Planet moon = Planet(Vertex(5,0,0),1);
  Planet io = Planet(Vertex(7.5,0,0),1);
  Planet europa = Planet(Vertex(11,0,0),2);
  Planet ganymede = Planet(Vertex(9.5,0,0),1);

  earth.moons.push_back(moon);
  jupiter.moons.push_back(io);
  jupiter.moons.push_back(europa);
  jupiter.moons.push_back(ganymede);
  venus.ring = true;

  SolarSystem system = vector<Planet> {sun, mercury, earth, venus, jupiter};
  
  system.xOff = screenWidth/2;
  system.yOff = screenHeight/2;
  system.angle = 90;

  uint8_t highlightedPlanet = 0;
  bool selectingPlanet = true;

  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(controls.joystickX != 0){
      if(selectingPlanet && utils.itsbeen(200)){
        highlightedPlanet++;
        highlightedPlanet%=system.planets.size();
        lastTime = millis();
      }
      else{
        if(controls.SHIFT()){
          if(controls.joystickX == 1){
            system.rotate(-1,1);
          }
          else if(controls.joystickX == -1){
            system.rotate(1,1);
          }
        }
      }
    }
    if(controls.joystickY != 0){
      if(controls.joystickY == 1){
        system.angle += 1;
      }
      else if(controls.joystickY == -1){
        system.angle -= 1;
      }
    }
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        return false;
      }
      if(controls.PLAY()){
        lastTime = millis();
        system.playOneStep();
      }
    }
    display.clearDisplay();
    system.draw();
    display.drawCircle(system.planets[highlightedPlanet].loc.x+system.xOff,system.planets[highlightedPlanet].loc.y+system.yOff,system.planets[highlightedPlanet].size+4+(millis()/200)%2,1);
    display.display();
  }
  return true;
}
