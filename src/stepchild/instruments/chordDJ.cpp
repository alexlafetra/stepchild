//QChord -- "quick chord"
//Only stores the root and the intervals
class QChord{
    public:
    uint8_t root;
    bool playing;
    vector<int8_t> intervals;
    QChord(){
        root = 0;
        intervals = {0};
        playing = false;
    }
    QChord(uint8_t r){
        root = r;
        intervals = {0};
        playing = false;
    }
    QChord(uint8_t r,vector<int8_t> i){
        root = r;
        intervals = i;
        playing = false;
    }
    //add an interval onto the chord
    void addInterval(int8_t value){
        intervals.push_back(value);

    }
    void subInterval(uint8_t index){
        //make sure index is in bounds
        if(index<intervals.size())
            intervals.erase(intervals.begin()+index);
    }
    void play(uint8_t v, uint8_t ch){
        if(playing)
            return;
        playing = true;
        for(uint8_t note = 0; note<intervals.size(); note++){
            sendMIDInoteOn(intervals[note]+root,v,ch);
        }
    }
    void stop(uint8_t ch){
        if(!playing)
            return;
        playing = false;
        for(uint8_t note = 0; note<intervals.size(); note++){
            sendMIDInoteOff(intervals[note]+root,0,ch);
        }
    }
};

void addChord(){

}

vector<QChord> initChordDjList(uint8_t preset, uint8_t root){
    vector<QChord> chords;
    switch(preset){
        //major and minor triads
        case 0:
            //major triads
            for(uint8_t i = 0; i<8; i++){
                chords.push_back(QChord(i+root,{0,4,7}));
            }
            //minor triads
            for(uint8_t i = 0; i<8; i++){
                chords.push_back(QChord(i+root,{0,3,7}));
            }
            break;
        //major and minor 7th chords (Nmaj7, Nmin7 not Nmaj/min7)
        case 1:
            //maj7
            for(uint8_t i = 0; i<8; i++){
                chords.push_back(QChord(i+root,{0,4,7,9}));
            }
            //min7
            for(uint8_t i = 0; i<8; i++){
                chords.push_back(QChord(i+root,{0,3,7,8}));
            }
            break;
    }
    return chords;
}

void chordDJ(){
    //at most, 16 QChords that can be toggled by the step buttons (w/shift for the second 8)
    //init with the major and minor chords in C
    vector<QChord> chords = initChordDjList(1,50);
    uint8_t vel = 100;
    uint8_t channel = 0;
    while(true){
        display.clearDisplay();
        //idea is to have 8 vertical columns with the root root pitch and interval numbers in them, as well as a play indicator
        //128/8 = 16,so 12px will give you a 4px gap between each
        const uint8_t startY = 24;
        for(uint8_t i = 0; i<8; i++){
            printSmall_centered(8+i*16,startY-24,stringify(i),1);
            display.drawFastVLine(2+i*16,startY,64-startY,1);
            display.drawFastVLine(14+i*16,startY,64-startY,1);
            if(i<chords.size()){
                printSmall_centered(8+i*16,startY-16,pitchToString(chords[i].root,true,true),1);
                for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
                    printSmall_centered(8+i*16,startY+j*6,pitchToString(chords[i].intervals[j]+chords[i].root,false,true),1);
                }
                if(chords[i].playing){
                    display.fillRect(3+i*16,startY,11,64-startY,1);
                }
            }
        }
        display.display();
        readButtons();
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis();
                break;
            }
            if(n){
                lastTime = millis();
                addChord();
            }
        }
        for(uint8_t i = 0; i<chords.size(); i++){
            if(i<8){
                if(!shift && step_buttons[i]){
                    chords[i].play(vel,channel);
                }
                else{
                    chords[i].stop(channel);
                }
            }
            else{
                if(shift && step_buttons[i-8]){
                    chords[i].play(vel,channel);
                }
                else{
                    chords[i].stop(channel);
                }
            }
        }
    }
}

#define INSTRUMENT_APP9_FUNCTION chordDJ
#define INSTRUMENT_APP9_TEXT "chord dj"
#define INSTRUMENT_APP9_ICON chord_dj_bmp
