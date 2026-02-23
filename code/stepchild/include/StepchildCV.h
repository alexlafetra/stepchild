#pragma once

#include "pins.h"

//this code enables the RP2040 to fake some analog output
//and send CV signals to 3 outputs

//basic idea is that the pico's PIO state machines can cycle from 3.3v+ to 0v
//so at a 50% duty cycle, you can have 1.65v+
//and at a 0% duty cycle, you can have 0v+

/*
analogWriteFreq(uint32_t frequency) to set the frequency of the PWM signal. It supports a frequency of 100Hz to 1MHz*

analogWriteRange(uint32_t range) to set the range of the PWM signal — the maximum value you’ll use to set 100% duty cycle. It supports a value from 16 to 65535.

analogWriteResolution (int resolution) to set the resolution of the PWM signal — up to 16-bit.

analogWrite(GPIO, duty cycle) to output a PWM signal to a specified pin with a defined duty cycle. It continuously outputs a PWM signal until a digitalWrite() or other digital output is performed.
*/

//Resolution = 16 - log2(PWM_FREQ)
#define PWM_RESOLUTION 16 //16-bit res
#define PWM_MAX_VAL 65535 //16-bit max resolution
// #define PWM_FREQUENCY 8312500 //133MHz/16 so you can use all 16-bits of resolution
// #define PWM_FREQUENCY 1000 //default freq, see: https://arduino-pico.readthedocs.io/en/latest/analog.html
// #define PWM_FREQUENCY 1024000//6-bit resolution
// #define PWM_FREQUENCY 4000000
   #define PWM_FREQUENCY 100000
// #define PWM_FREQUENCY 2000

/*
CV ports can grab data from:
a single track (gate)
global/specific channels (mono pitch -- only the most recent note will send though!)
an autotrack
the clock
*/

enum CV_TYPE:uint8_t{
    CV_PITCH,
    CV_GATE,
    CV_CC,
    CV_CLOCK
};

struct CV_JACK{
    CV_TYPE type = CV_GATE;
    //target is the track it's listening to for gate, or autotrack for CC
    uint8_t target = 0;
    uint16_t currentVal = 0;
    uint16_t max_pwm_value = PWM_MAX_VAL;
    uint8_t pin = 0;
    CV_JACK(){};
    CV_JACK(uint8_t p, CV_TYPE t){
        pin = p;
        type = t;
    }
    void write(uint16_t v){
        if(v <= max_pwm_value){
            analogWrite(pin,v);
            currentVal = v;
        }
    }
};

class StepchildCV{
    public:
    bool on = false;
    CV_JACK ports[4];
    StepchildCV();
    void init();
    //this converts a midi 8-bit pitch to a 12note/V CV voltage
    float pitchToVoltage(int8_t pitch);
    uint16_t getDutyCycleFromVoltage(float voltage);
    void checkPitch(uint8_t pitch);
    void checkCC(uint8_t val, uint8_t track);
    void checkGate(bool state, uint8_t track);
    void writeGate(bool state);
    void checkClock();
    void off();
};
