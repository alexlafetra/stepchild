/*
    +====================================+
    |                                    |
    |            an example!             |
    |                                    |
    +====================================+

    so, this function does very little. Basically, it's an effect that sets every other note on a track
    to have velocity 64
*/


//Import the stepchild class
#include "Stepchild.h"
//use the global instance of the stepchild object
;

using namespace std;

void exampleFunction(){
    for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
        for(uint16_t note = 1; note<stepchild.noteData[track].size(); note+=2){
            stepchild.noteData[track][note].velocity = 64;
        } 
    }
}


#define FX_APP7_ICON default_bmp         //giving it an icon
#define FX_APP7_TEXT "example"           //giving it title text
#define FX_APP7_FUNCTION exampleFunction //linking the funciton to the FX menu
