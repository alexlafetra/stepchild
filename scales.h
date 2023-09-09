//Functions and definitions for generating scales
#define MAJOR 0
#define MELODIC_MINOR 1
#define HARMONIC_MINOR 2
#define MAJOR_PENTATONIC 3
#define MINOR_PENTATONIC 4
#define BLUE 5
#define DORIAN 6
#define PHRYGIAN 7
#define LYDIAN 8
#define MIXOLYDIAN 9
#define AEOLIAN 10
#define LOCRIAN 11

String getScaleName(uint8_t scale){
  const String scales[12] = {"major","melodic minor","harmonic minor","blue","dorian","phrygian","lydian","mixolydian","aeolian","locrian"};
  return scales[scale];
}

vector<uint8_t> genScale(uint8_t scale, uint8_t root, int8_t numOctaves, uint8_t octave){
  vector<uint8_t> newScale;
  for(int8_t i = octave; i<numOctaves+octave; i++){
    vector<uint8_t> temp = genScale(scale,root+(i*12));
    for(uint8_t j = 0; j<temp.size(); j++){
      newScale.push_back(temp[j]);
    }
  }
  return newScale;
}
//make sure 'root' isn't passed to this fn as a negative number
vector<uint8_t> genScale(uint8_t scale, uint8_t root){
  vector<uint8_t> newScale;
  if(scale == MAJOR)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
  else if(scale == MELODIC_MINOR)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
  else if(scale == HARMONIC_MINOR)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+11)};
  else if(scale == MAJOR_PENTATONIC)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+7),uint8_t(root+9)};
  else if(scale == MINOR_PENTATONIC)
    newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+10)};
  else if(scale == BLUE)
    newScale = {root,uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+7),uint8_t(root+10)};
  else if(scale == DORIAN)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
  else if(scale == PHRYGIAN)
    newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
  else if(scale == LYDIAN)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+6),uint8_t(root+7),uint8_t(root+9),uint8_t(root+11)};
  else if(scale == MIXOLYDIAN)
    newScale = {root,uint8_t(root+2),uint8_t(root+4),uint8_t(root+5),uint8_t(root+7),uint8_t(root+9),uint8_t(root+10)};
  else if(scale == AEOLIAN)
    newScale = {root,uint8_t(root+2),uint8_t(root+3),uint8_t(root+5),uint8_t(root+7),uint8_t(root+8),uint8_t(root+10)};
  else if(scale == LOCRIAN)
    newScale = {root,uint8_t(root+1),uint8_t(root+3),uint8_t(root+5),uint8_t(root+6),uint8_t(root+8),uint8_t(root+10)};
  return newScale;
}