/*
    +====================================+
    |                                    |
    |            an example!             |
    |                                    |
    +====================================+

    so, this function does very little. Basically, it's an effect that sets every other note on a track
    to have velocity 64
*/

void exampleFunction(){
    for(uint8_t track = 0; track<sequence.trackData.size(); track++){
        for(uint16_t note = 1; note<sequence.noteData[track].size(); note+=2){
            sequence.noteData[track][note].velocity = 64;
        } 
    }
}


#define FX_APP7_ICON default_bmp         //giving it an icon
#define FX_APP7_TEXT "example"           //giving it title text
#define FX_APP7_FUNCTION exampleFunction //linking the funciton to the FX menu
