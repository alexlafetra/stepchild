#include <Arduino.h>

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