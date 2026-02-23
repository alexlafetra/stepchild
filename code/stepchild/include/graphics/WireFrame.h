#pragma once

#include "Stepchild.h"
extern Stepchild stepchild;

class Vertex{
  public:
  float x;
  float y;
  float z;
  Vertex();
  Vertex(float,float,float);
  void render(uint8_t, uint8_t, float);
  void render(uint8_t, uint8_t, float, uint8_t);
  void render(uint8_t, uint8_t, float, uint8_t, bool);
  void render(uint8_t, uint8_t,float, uint8_t, String);
  void rotate(float, uint8_t);
  void coordTransform(std::vector<std::vector<float>>);
};

class WireFrame{
  public:
  float currentAngle[3];
  std::vector<Vertex> verts;
  //this should also be a vector of arrays instead of a vec of vecs
  //bc each edge will only be between 2 points
  std::vector<std::vector<uint16_t>>edges;//should be 16-bit so it can handle more than 256 verts (just in case)
  std::vector<uint16_t> dots;
  // uint8_t offset.y;

  //struct for holding the screen-space offset of the wireframe
  struct Offset{
    int8_t x = 0;
    int8_t y = 0;
    Offset(int8_t x1, int8_t y1){
      x = x1;
      y = y1;
    }
  };
  Offset offset = Offset(0,0);
  uint8_t dotSize = 1;
  float scale;
  bool drawEdges;
  bool drawDots;
  WireFrame(){};
  WireFrame(std::vector<Vertex>);
  WireFrame(std::vector<Vertex>,std::vector<std::vector<uint8_t>>);
  WireFrame(std::vector<Vertex>,std::vector<std::vector<uint16_t>>);

  void render();
  void renderDie();
  void renderDotsIfInFrontOf(float zCutoff);
  void rotate(float,uint8_t);
  void setRotation(float,uint8_t);
  bool isFarthestVert(uint8_t);
  bool isClosestVert(uint8_t);
  uint8_t getFarthestVert();
  uint8_t getClosestVert();
  void reset(uint8_t);
  void resetExceptFor(uint8_t);
  void rotateVertRelative(uint8_t,float,uint8_t);
  void join(WireFrame);
  void addVerts(std::vector<Vertex>);
  void addEdges(std::vector<std::vector<uint16_t>>);
  void move(float,float,float);
};


void viewWireFrame(WireFrame& w);
void animateMonitor(WireFrame& wireframe, float mag, uint8_t yCoord);
std::vector<WireFrame> animateRotation(std::vector<WireFrame> w, float angle, uint8_t speed, uint8_t axis, float xA, float yA, float zA, WireFrame frame);
//used to calculate which side of the die you're on
uint8_t getSide(WireFrame die);
WireFrame makeCube(uint8_t size);
WireFrame makeHalfHouse(float width);
WireFrame makeHouse();
WireFrame genFrame();
WireFrame makeHammer();
WireFrame makeWrench();
//drawing a circular din jack
WireFrame makeMIDI();
WireFrame makeStretchedCircle(float x1, float x2, float y, float z, float r, float points, uint8_t vertOffset);
WireFrame makeDisc(float x1, float y1, float z1, float x2, float y2, float z2, float r, uint8_t points, uint8_t vertOffset);

//makes a disc, vertOffset is so that the disc edges stay in synch
WireFrame makeDisc_centered(float x1, float y1, float z1, float r, float xSpacing, uint8_t points, uint8_t vertOffset);

WireFrame makeCircle(float r1, uint8_t points);

WireFrame makeGyro(float angleX, float angleY, float angleZ,float angle2X, float angle2Y, float angle2Z);

WireFrame makeCycle();

//makes a pram outline
WireFrame makeHalfPramBody(float zOff);
WireFrame makeHalfPramLegs(float zOff);

WireFrame makePram();

WireFrame makeGear(float r1, float r2, uint8_t teeth, uint8_t points, bool center, float zPos);

WireFrame makeThickGear(float r1, float r2, uint8_t teeth, uint8_t points, bool center);

//makes a generic box centered on 0,0,0
WireFrame makeBox(uint8_t w, uint8_t h, uint8_t d);
WireFrame makeCassette();
WireFrame makeGraphBox_old(float offset);

WireFrame makeGraphBox(float offset);

//makes a metronome (with an adjustable angle)
WireFrame makeMetronome(float offset);
//this is busted
WireFrame makePencil();

WireFrame makeLoopArrows(float angle);

WireFrame makeMobius(float offset);

WireFrame makeLoopArrows_Old(float angle);
WireFrame makeKeys();
//hand for arp menu
WireFrame makeHand_flat(float f1, float f2, float f3, float f4, float f5);

//hand for arp menu
WireFrame makeHand(float f1, float f2, float f3, float f4, float f5);

WireFrame makeCD();

WireFrame makeHelix(uint8_t r, uint8_t revs, float length, uint8_t points, bool chirality);

//folder anim works correctly now :)
WireFrame makeFolder(float openAngle);

//builds a rectangle in the XY plane centered on c with height = h and width = w
WireFrame makeRect(float w, float h, Vertex c);

WireFrame makeMonitor();

//keys are three rectangular prisms that each ebb and flow
WireFrame makeArpBoxes(float keyOffsetTimer);


void openFolderAnimation(WireFrame& w,float amount);

//opening and closing
void folderAnimation(WireFrame& w);
void handAnimation(WireFrame* w);
void keysAnimation(WireFrame* w);
void loopArrowAnimation(WireFrame* w);
void metAnimation(WireFrame* w);
void graphAnimation(WireFrame* w);

void renderTest();

WireFrame makeDieDots(uint8_t x1, uint8_t y1, uint8_t distance, float scale);

//makes a cube with die dots!
//dots ~are~ accurate
WireFrame genRandMenuObjects(uint8_t x1, uint8_t y1, uint8_t distance, float scale);



