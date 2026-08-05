#pragma once

#include <vector>
#include "commonStructs.h"
#include "Note.h"

class Stepchild;

//holds the copied note data
class Clipboard{
  public:

    Stepchild* stepchild;
    Coordinate relativeCursorPosition;
    std::vector<std::vector<Note>> buffer;

    Clipboard(Stepchild* ptr){stepchild = ptr;}

    void copy();
    void copyLoop(uint8_t loopID);
    void copyLoop();
    void pasteAt(uint8_t track, uint16_t step);
    void paste();
};
