#include <Arduino.h>
#include "StepchildCV.h"

StepchildCV::StepchildCV(){
        ports[0] = CV_JACK(CV1_PIN,CV_PITCH);
        ports[1] = CV_JACK(CV2_PIN,CV_GATE);
        ports[2] = CV_JACK(CV3_PIN,CV_CC);
        ports[3] = CV_JACK(CV4_PIN,CV_CLOCK);
    }
void StepchildCV::init(){
    #ifndef HEADLESS
    for(CV_JACK jack : ports){
        pinMode(jack.pin,OUTPUT);
    }
    //frequency of the PWM driver
    analogWriteFreq(PWM_FREQUENCY);
    //max value you can put into it (65535 @ 16bit res let's us do a 100% duty cycle)
    analogWriteRange(PWM_MAX_VAL);
    //16 bit resolution
    analogWriteResolution(PWM_RESOLUTION);
    #endif
}

//this converts a midi 8-bit pitch to a 12note/V CV voltage
float StepchildCV::pitchToVoltage(int8_t pitch){
    //the CV scale is 12 tones/volt
    if(pitch < 0){
        return -1;
    }
    //each semitone is a 1/12 of a volt, so semitones*1/12 = voltage
    float voltage = float(pitch)/12.0;
    return voltage;
}
uint16_t StepchildCV::getDutyCycleFromVoltage(float voltage){
    // float dutyCycle = voltage/3.3;
    // uint16_t twoByteNumber = dutyCycle*65535.0;
    // return twoByteNumber;
    return voltage/12.0 * float(PWM_MAX_VAL);
}
void StepchildCV::checkPitch(uint8_t pitch){
    for(CV_JACK jack : ports){
        if(jack.type == CV_PITCH){
            float V = pitchToVoltage(pitch);
            uint16_t dCycle = getDutyCycleFromVoltage(V);
            jack.write(dCycle);
        }
    }
}
void StepchildCV::checkCC(uint8_t val, uint8_t track){
    for(CV_JACK jack : ports){
        if(jack.type == CV_CC && jack.target == track){
            float V = pitchToVoltage(val);
            uint16_t dCycle = getDutyCycleFromVoltage(V);
            jack.write(dCycle);
        }
    }
}
void StepchildCV::checkGate(bool state, uint8_t track){
    for(CV_JACK jack : ports){
        if(jack.type == CV_GATE && jack.target == track){
            jack.write(state?jack.max_pwm_value:0);
        }
    }
}
void StepchildCV::writeGate(bool state){
    for(CV_JACK jack : ports){
        if(jack.type == CV_GATE){
            jack.write(state?jack.max_pwm_value:0);
        }
    }
}
void StepchildCV::checkClock(){
    for(CV_JACK jack : ports){
        if(jack.type == CV_CLOCK){
            jack.write(jack.max_pwm_value);
            jack.write(0);
        }
    }
}
void StepchildCV::off(){            
    for(CV_JACK jack : ports){
        jack.write(0);
    }
}