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
    for(uint8_t track = 0; track<trackData.size(); track++){
        for(uint16_t note = 1; note<seqData[track].size(); note+=2){
            seqData[track][note].velocity = 64;
        } 
    }
}


#define FX_APP6_ICON default_bmp         //giving it an icon
#define FX_APP6_TEXT "example"           //giving it title text
#define FX_APP6_FUNCTION exampleFunction //linking it's main function