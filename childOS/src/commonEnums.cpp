#include "commonEnums.h"

LoopType operator++(LoopType &c,int) {
  c = static_cast<LoopType>(static_cast<uint8_t>(c) + 1);
  return c;
}
LoopType operator--(LoopType &c,int) {
  c = static_cast<LoopType>(static_cast<uint8_t>(c) - 1);
  return c;
}
CurveType operator++(CurveType &c,int) {
    if(c == NOISE_CURVE)
        return LINEAR_CURVE;
    c = static_cast<CurveType>(static_cast<uint8_t>(c) + 1);
    return c;
}
CurveType operator--(CurveType &c,int) {
    if(c == LINEAR_CURVE)
        return NOISE_CURVE;
    c = static_cast<CurveType>(static_cast<uint8_t>(c) - 1);
    return c;
}
ScaleName& operator++(ScaleName& e) {
    // Wrap-around logic
    if (e == LOCRIAN) {
        e = MAJOR;
    }
    else{
        e = static_cast<ScaleName>(static_cast<uint8_t>(e) + 1);
    }
    return e;
}
ScaleName operator++(ScaleName& e, int) {
    ScaleName result = e; // Make a copy of the current value
    ++e;               // Increment the original value
    return result;     // Return the copy (the original value before increment)
}

// Define a free-standing function to overload --
ScaleName& operator--(ScaleName& e) {
    if (e == MAJOR) {
        e = LOCRIAN;
    }
    else{
        e = static_cast<ScaleName>(static_cast<uint8_t>(e) - 1);
    }
    return e;
}

// Define a free-standing function to overload postfix --
ScaleName operator--(ScaleName& e, int) {
    ScaleName result = e; // Make a copy of the current value
    --e;               // Decrement the original value
    return result;     // Return the copy (the original value before decrement)
}
TriggerSource operator++(TriggerSource &c,int) {
  if(c == CHANNEL_TRIGGER)
    return GLOBAL_TRIGGER;
  c = static_cast<TriggerSource>(static_cast<uint8_t>(c) + 1);
  return c;
}
TriggerSource operator--(TriggerSource &c,int) {
  if(c == GLOBAL_TRIGGER)
    return CHANNEL_TRIGGER;
  c = static_cast<TriggerSource>(static_cast<uint8_t>(c) - 1);
  return c;
}