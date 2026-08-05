#pragma once
#include <cstdint>
#include "commonEnums.h"
#include "Arduino.h"


enum ClockSource:uint8_t{
    INTERNAL_CLOCK,
    EXTERNAL_CLOCK
};

class SwingCurve{
    public:
    CurveType type = LINEAR_CURVE;
    uint16_t period = 96;
    uint16_t phase = 0;
    int16_t amplitude = 1;
    SwingCurve(CurveType t, float per, uint16_t ph, float a){
        type = t;
        period = per;
        phase = ph;
        amplitude = a;
    }
    int16_t getValueAt(uint16_t point){
        switch(type){
            //default
            case LINEAR_CURVE:
                return 0;
            //sinewave
            case SINEWAVE_CURVE:
                return amplitude*sin(float(point+phase)*float(2.0*PI)/float(period));   
            //square wave
            case SQUAREWAVE_CURVE:
                //if you're less than half a period, it's high. Else, it's low
                if((point+phase)%period<period/2)
                    return amplitude;
                else
                    return -amplitude;
            //saw
            case SAWTOOTH_CURVE:{
                float slope = float(amplitude*2)/float(period);
                return ((point+phase)%period)*slope-amplitude;
            }
            case TRIANGLE_CURVE:{
                float slope = float(amplitude*2)/float(period/2);
                //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
                if((point+phase)%period>(period/2))
                    return ((point+phase)%period) * (-slope) + 3*amplitude;
                else
                    return ((point+phase)%period) * slope - amplitude;
            }
          	case RANDOM_CURVE:
          	case NOISE_CURVE:
          	default:
            	return 0;
        }
        return 0;

    }
};

//Encapsulates all the special timing functions!
class StepchildClock{
    public:

    ClockSource clockSource = INTERNAL_CLOCK;
    bool receivedClockMessage = false;
    bool isSwinging = false;

    SwingCurve swingCurve = SwingCurve(SINEWAVE_CURVE,96.0,0,4208);
    int16_t swingCurveSource = -1; //-1 ==> swing curve, >= 0 ==> autotrack
    uint16_t BPM = 120;

    //Maximum val in a 4byte number is 4294967296 uS
    //==> ~4295 S
    //==> ~71 hours
    uint32_t uSecPerStep = 20833;//length of a timestep in uSeconds (corresponds to bpm), rounded to nearest uS
    uint32_t timeLastStepPlayed = 0;//stores the time in uSeconds of the last timing fn call
    uint32_t startTime = 0;//stores uS since playing/recording started

    int32_t offBy = 0;//Stores the amount of uSeconds that the last step over/undershot by

    StepchildClock();
    uint16_t mSecPerStep();
    //returns the amount a timestep should be shifted (in uSeconds) based on the swing curve
    int16_t swingOffset(uint16_t step);
    //this is a sloppy lil function that returns true if the time is within (x) of the subDiv
    //Currently only used to make the pram bounce! Should only be used for graphics
    //Or timing-non-crucial uses
    bool onBeat(uint16_t subDiv, int16_t fudge);
    //returns true if it's been enough time since this function was last called
    //Slightly less accurate overtime than the "straight" version, since the "offBy"
    //amount is only updated when the timestep aligns with the swing subdivision
    //Errors are small, but can accumulate overtime
    bool hasItBeenEnoughTime_swing(uint16_t step);
    //Returns true if it's been enough time since this function was last
    //called. This one is more accurate because the "offBy" val is updated
    //each time the function returns true
    bool hasItBeenEnoughTime_straight();
    //selects from either the sequenceClock.isSwinging or straight version of the timing function
    //this is called from the timing loop on CPU1
    bool hasItBeenEnoughTime(uint16_t step);
    //sets a new BPM and adjusts the uS/timestep and swing val accordingly
    void setBPM(int16_t newBpm);
};
