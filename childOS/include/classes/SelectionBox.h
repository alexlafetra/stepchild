#pragma once

#include "graphics/SequenceRenderSettings.h"
#include "commonStructs.h"
#include "Stepchild.h"

class Stepchild;
class SequenceRenderSettings;

//Holds coordinates and a flag set when the SelectionBox has been started
class SelectionBox{
  public:
  CoordinatePair coords;
  bool begun = false;
  Stepchild* stepchild;

  SelectionBox(Stepchild* ptr){
    stepchild = ptr;
  }

  void render(SequenceRenderSettings& settings);
  void select();
};
