#pragma once

#include "commonEnums.h"
#include "commonStructs.h"

//generic menu class
class StepchildMenu{
    public:
        //stores coordinate pairs for (start, end) of the menu rectangle (not used by all menus!)
        CoordinatePair coords;
        //stores the current cursor position/active item
        uint8_t cursor = 0;
        //a function that needs to be overridden by each menu's derived class
        virtual void displayMenu();
        StepchildMenu();
        void slideIn(SlideDirection origin,MenuSlideSpeed speed);
        void slideOut(SlideDirection destination, MenuSlideSpeed speed);
};
