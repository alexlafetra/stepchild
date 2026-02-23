#pragma once

#include "classes/Note.h"

class SuperpositionMenu{
  public:
    Note note;
    uint8_t track;
    SuperpositionMenu(Note& n, uint8_t t);
    bool setSuperpositionControls();
    void drawSuperposSelect();
};

void setSuperpositionMenu(Note& note,uint8_t originalTrack);