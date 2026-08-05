/*
Objects that hold the tracks that get loaded into a sequence when 
*/
// using namespace std;

#include <vector>
#include "classes/SequenceTemplate.h"
#include "classes/Track.h"

using namespace std;

SequenceTemplate::SequenceTemplate(){
    tracks = vector<Track>{Track()};
    type = UNDEFINED;
}
SequenceTemplate::SequenceTemplate(vector<Track> t){
    tracks = t;
    type = LOAD_FROM_VECTOR;
}
SequenceTemplate::SequenceTemplate(vector<Track> (*tBFn)(void)){
    trackBuildingFn = tBFn;
    type = GENERATE_FROM_FUNCTION;
}
vector<Track> SequenceTemplate::loadTemplate(){
    switch(type){
        case LOAD_FROM_VECTOR:
            return tracks;
        case GENERATE_FROM_FUNCTION:
            return trackBuildingFn();
      case UNDEFINED:
      default:
        return vector<Track>{Track()};
    }
}
