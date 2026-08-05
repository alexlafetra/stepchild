#pragma once

#include <vector>
#include "classes/Track.h"

class SequenceTemplate{
    public:
    enum SequenceTemplateType:uint8_t{
        GENERATE_FROM_FUNCTION,
        LOAD_FROM_VECTOR,
        UNDEFINED
    };
    std::vector<Track> tracks;
    std::vector<Track> (*trackBuildingFn)(void);
    SequenceTemplateType type = UNDEFINED;
    
    SequenceTemplate();
    SequenceTemplate(std::vector<Track> t);
    SequenceTemplate(std::vector<Track> (*tBFn)(void));
    std::vector<Track> loadTemplate();
};
