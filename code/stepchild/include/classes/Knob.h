#pragma once
#include "Arduino.h"

class Knob{
  public:
    Knob();
    Knob(uint8_t,uint8_t,uint8_t);
    uint8_t cc;
    uint8_t val;
    uint8_t channel;//0 is global
    void increment(int8_t);
    void send();
    void set(int16_t val);
};
