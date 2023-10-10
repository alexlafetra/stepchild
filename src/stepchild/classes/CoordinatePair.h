//Stores two (x,y) coordinate pairs as x1, y1, x2, y2
struct CoordinatePair{
  int16_t x1;
  int16_t x2;
  int8_t y1;
  int8_t y2;
  CoordinatePair();
  CoordinatePair(uint16_t xStart, uint16_t xEnd);
  CoordinatePair(uint16_t xStart, uint16_t xEnd, uint8_t yStart, uint8_t yEnd);
  bool isEmpty();
};

CoordinatePair::CoordinatePair(){
  x1 = 0;
  x2 = 0;
  y1 = 0;
  y2 = 0;
}
CoordinatePair::CoordinatePair(uint16_t xStart, uint16_t xEnd){
  x1 = xStart;
  x2 = xEnd;
  y1 = 0;
  y2 = 0;
}
CoordinatePair::CoordinatePair(uint16_t xStart, uint16_t xEnd, uint8_t yStart, uint8_t yEnd){
  x1 = xStart;
  x2 = xEnd;
  y1 = yStart;
  y2 = yEnd;
}

bool CoordinatePair::isEmpty(){
  return x1 == x2;
}
