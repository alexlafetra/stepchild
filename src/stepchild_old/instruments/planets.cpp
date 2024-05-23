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
    uint8_t size;
    bool fill;
    bool ring;
    float orbit;
    vector<Moon> moons;
    Planet();
    Planet(Vertex,uint8_t,bool);
    void rotate(float,uint8_t);
    void draw(uint8_t,uint8_t,float);
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
void Planet::rotate(float angle, uint8_t axis){
  loc.rotate(angle,axis);
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
  if(ring)
      graphics.drawEllipse(temp.x+xOff,temp.y+yOff,size+6,size,SSD1306_WHITE);
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
};

SolarSystem::SolarSystem(){
}
SolarSystem::SolarSystem(vector<Planet> p){
  planets = p;
}
void SolarSystem::rotate(float a, uint8_t axis){
  for(uint8_t p = 0; p<planets.size(); p++){
    planets[p].rotate(a+p,axis);
    for(uint8_t m = 0; m<planets[p].moons.size(); m++){
      planets[p].moons[m].rotate(-a*3+m,axis);
    }
  }
}

void SolarSystem::draw(){
  for(uint8_t p = 0; p<planets.size(); p++){
    planets[p].draw(xOff,yOff,angle);
  }
}


void testSolarSystem(){
  Planet sun = Planet(Vertex(0,0,0),8,true);
  Planet mercury = Planet(Vertex(15,0,0),1,false);
  Planet venus = Planet(Vertex(30,0,0),2,false);
  Planet earth = Planet(Vertex(45,0,0),3,true);
  Planet jupiter = Planet(Vertex(60,0,0),5,true);
  //moon targeting earth
  Moon moon = Moon(Vertex(10,0,0),1);
  Moon io = Moon(Vertex(15,0,0),1);
  Moon europa = Moon(Vertex(22,0,0),2);
  Moon ganymede = Moon(Vertex(19,0,0),1);

  earth.moons.push_back(moon);
  jupiter.moons.push_back(io);
  jupiter.moons.push_back(europa);
  jupiter.moons.push_back(ganymede);
  venus.ring = true;

  SolarSystem system = vector<Planet> {sun, mercury, earth, venus, jupiter};
  
  system.xOff = screenWidth/2;
  system.yOff = screenHeight/2;
  system.angle = -15;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(controls.joystickX != 0){
      if(controls.joystickX == 1){
        system.rotate(-1,1);
      }
      else if(controls.joystickX == -1){
        system.rotate(1,1);
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
        return;
      }
    }
    display.clearDisplay();
    system.draw();
    display.display();
  }
}
