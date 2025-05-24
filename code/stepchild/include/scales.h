
enum ScaleName:uint8_t{
  MAJOR,
  HARMONIC_MINOR,
  MELODIC_MINOR,
  MAJOR_PENTATONIC,
  MINOR_PENTATONIC,
  BLUE,
  DORIAN,
  PHRYGIAN,
  LYDIAN,
  MIXOLYDIAN,
  AEOLIAN,
  LOCRIAN
};

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

String getScaleName(ScaleName scale){
  const String scales[12] = {"major","dorian","phrygian","lydian","mixolydian","aeolian","locrian","melodic minor","harmonic minor","major pentatonic","minor pentatonoic","blue"};
  return scales[static_cast<uint8_t>(scale)];
}

//make sure 'root' isn't passed to this fn as a negative number
vector<uint8_t> genScale(ScaleName scale, uint8_t root){
  vector<uint8_t> newScale;
  switch(scale){
    case MAJOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case DORIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case PHRYGIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+6),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
      break;
    case MIXOLYDIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
      break;
    case AEOLIAN:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case LOCRIAN:
      newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+8),uint8_t(root+10)};
      break;
    case MELODIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
      break;
    case HARMONIC_MINOR:
      newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+11)};
    case MAJOR_PENTATONIC:
      newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+7),uint8_t(root+9)};
      break;
    case MINOR_PENTATONIC:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+10)};
      break;
    case BLUE:
      newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+7),uint8_t(root+10)};
      break;
  }
  return newScale;
}

//Functions and definitions for generating scales
vector<uint8_t> genScale(ScaleName scale, uint8_t root, int8_t numOctaves, uint8_t octave){
  vector<uint8_t> newScale;
  for(int8_t i = octave; i<numOctaves+octave; i++){
    vector<uint8_t> temp = genScale(scale,root%12+(i*12));
    for(uint8_t j = 0; j<temp.size(); j++){
      newScale.push_back(temp[j]);
    }
  }
  return newScale;
}