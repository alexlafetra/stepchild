#pragma once
#include <Arduino.h>
#include "commonEnums.h"

//unimplemented
// enum TargetType:uint8_t{GENERAL_MIDI,SP404MKII,INTERNAL};

class Autotrack{
  public:
    //holds values in the range 0-127, 255 ==> NO value and a control number should not be sent
    std::vector<uint8_t> data;
    //control number
    uint8_t control = 1;
    //channel val
    uint8_t channel = 1;
    //type of track
    //0 is default, line-based, 1 is sine/cosine based, 2 is square, 3 is saw, 4 is was, 5 is triangle and 6 is random
    CurveType type = LINEAR_CURVE;
    //this is just for the osc-based waveforms
    uint16_t period = 96;
    uint16_t phase = 0;
    int8_t amplitude = 48;
    uint8_t yPos = 64;

    //controls whether it's a 0 (default) or 1 (sp404mkII) or 2 (internal) parameter track
    uint8_t parameterType = 0;

    //for recording to it
    bool isPrimed = true;

    // 0 is from external, 1 is encoder A, 2 is encoder B, 3 is X, 4 is Y
    RecordingFrom recordFrom = EXTERNAL_MIDI;

    //can be global, track, or channel
    TriggerSource triggerSource = GLOBAL_TRIGGER;
    //ID of the track, or channel, that the AT will trigger on
    uint8_t triggerTarget = 0;

    //gated means the track turns on/off with it's trigger (if it's a trigger track)
    //if not gated, the track will play continuously
    bool gated = true;
    uint16_t playheadPos = 0;

    //for muting/unmuting it
    bool isActive = true;

    std::vector<uint16_t> selectedPoints;
    Autotrack();
    Autotrack(CurveType,uint16_t);
    void sendData(uint16_t);
    void play(uint16_t);
    void setTrigger(TriggerSource trigSource, uint8_t trigTarget);
    void createDataPoint(uint16_t pos);
    void changeDataPoint(uint16_t pos, int8_t amount);
    void deleteDataPoint(uint16_t point);
    void selectDataPoint(uint16_t index,bool state);
    void toggleSelectDataPoint(uint16_t index);
    bool isDataPointSelected(uint16_t index);
    uint8_t getLastDTVal(uint16_t point);
    //linear interpolate between two points on the curve
    void linearInterpolate(uint16_t start, uint16_t end);
    //interpolate between two points as though they're on an ellipse
    void ellipticalInterpolate(uint16_t start, uint16_t end, bool up);
    //randomly perform different interpolations on different parts of the track
    //(this doesn't give a great result)
    void randomInterp(uint16_t start, uint16_t end);
    void regenCurve(uint16_t start, uint16_t end);
    void regenCurve();
    //this one smooths between selected points
    void smoothSelectedPoints(uint8_t type);
};
