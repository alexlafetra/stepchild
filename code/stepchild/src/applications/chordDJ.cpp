#include <algorithm>
#include <vector>
#include <Arduino.h>
#include "graphics/SequenceRenderSettings.h"
#include "guiUtilities.h"
#include "StepchildGraphics.h"
#include "Stepchild.h"
#include "stringPatch.h"
#include "mainSequence.h"
#include "utils.h"

extern StepchildGraphics graphics;
extern Stepchild stepchild;

using namespace std;

extern void drawSeq();

//QChord -- "quick chord"
//Only stores the root and the intervals
class QChord{
    public:
    uint8_t root;
    bool playing;
    vector<uint8_t> intervals;
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
    QChord(uint8_t r,vector<uint8_t> i){
        root = r;
        intervals = i;
        playing = false;
    }
    //this one gets built from midi notes
    QChord(vector<uint8_t> midiNotes){
        if(midiNotes.size()){
            //set the root to the lowest note
            sort(midiNotes.begin(),midiNotes.end());
            root = midiNotes[0];
            for(uint8_t i = 0; i<midiNotes.size(); i++){
                intervals.push_back(midiNotes[i]-root);
            }
        }
        else{
            root = 0;
            intervals = {0};
        }
        playing = false;
    }
    //converts the interval list to midi note numbers
    vector<uint8_t> toMIDINotes(){
        vector<uint8_t> notes;
        for(uint8_t i = 0; i<this->intervals.size(); i++){
            notes.push_back(this->intervals[i]+this->root);
        }
        return notes;
    }
    uint8_t octave(){
        return this->root/12;
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
            stepchild.midi.noteOn(intervals[note]+root,v,ch);
        }
    }
    void stop(uint8_t ch){
        if(!playing)
            return;
        playing = false;
        for(uint8_t note = 0; note<intervals.size(); note++){
            stepchild.midi.noteOff(intervals[note]+root,0,ch);
        }
    }

    vector<String> getPitchList(){
        vector<String> pitchList;
        for(uint8_t i = 0; i<this->intervals.size(); i++){
            pitchList.push_back(stepchild.pitchToString(this->intervals[i]+this->root,false,true));
        }
        return pitchList;
    }
};

//chord builder
QChord editChord(QChord& originalChord){
  vector<uint8_t> pressedKeys = originalChord.toMIDINotes();
  uint8_t octave = originalChord.octave();

  uint8_t keyCursor = 0;
  if(pressedKeys.size())
    keyCursor = pressedKeys[0];

  vector<uint8_t> mask;
  uint8_t maskRoot = 0;
    
  bool playedChord = false;

  while(true){
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    if(stepchild.buttons.joystickX != 0 && ((!stepchild.buttons.SHIFT() && stepchild.itsbeen(50)) || (stepchild.buttons.SHIFT() && stepchild.itsbeen(150)))){
        if(stepchild.buttons.joystickX == -1 && keyCursor<12*octave+35){
            keyCursor++;
            stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == 1 && keyCursor>12*octave){
            keyCursor--;
            stepchild.lastTime = millis();
        }
    }
    if(stepchild.buttons.PLAY() && !playedChord){
        for(uint8_t i = 0; i<pressedKeys.size(); i++){
            stepchild.midi.noteOn(pressedKeys[i]+octave*12,100,1);
        }
        playedChord = true;
    }
    else if(playedChord && !stepchild.buttons.PLAY()){
        for(uint8_t i = 0; i<pressedKeys.size(); i++){
            stepchild.midi.noteOff(pressedKeys[i]+octave*12,100,1);
        }
        playedChord = false;
    }

    if(stepchild.itsbeen(200)){
        if(stepchild.buttons.LOOP()){
            stepchild.lastTime = millis();
            //if there's no mask, make one
            //(mask uses highlit note as root)
            if(mask.size() == 0){
                mask = stepchild.makePitchListFromScale(MAJOR,keyCursor%12,4,octave?(octave-1):0);
                // mask = makePitchListFromScale(keyCursor%12,0,3,octave);
                maskRoot = keyCursor%12;
            }
            //if there is, unmake one
            else{
                vector<uint8_t> temp;
                mask.swap(temp);
            }
        }
        //selecting/deselecting notes
        if(stepchild.buttons.SELECT() ){
            //making new chord
            stepchild.lastTime = millis();
            //deselect all notes
            if(stepchild.buttons.SHIFT()){
                vector<uint8_t> temp;
                pressedKeys.swap(temp);
            }
            else{
                //if the key is already selected, remove it
                if(isInVector(keyCursor,pressedKeys)){
                    vector<uint8_t> temp;
                    //removing keys from pressedKeys;
                    for(uint8_t i = 0; i<pressedKeys.size(); i++){
                    if(pressedKeys[i] != keyCursor){
                        temp.push_back(pressedKeys[i]);
                    }
                    }
                    pressedKeys.swap(temp);
                }
                else{
                    pressedKeys.push_back(keyCursor);
                }
            }
        }
        //making a chord
        if(stepchild.buttons.NEW() || stepchild.buttons.MENU()){
            stepchild.lastTime = millis();
            return QChord(pressedKeys);
        }
    }
    stepchild.display.clearDisplay();
    //normal, make-new-chord mode
    graphics.drawFullKeyBed(pressedKeys,mask,keyCursor,octave);
    if(playedChord)
        graphics.printSmall(0,59,"Playing...",1);
    else
        graphics.printSmall(0,59,"[Play] to hear Chord",1);
    //if there's a mask
    if(mask.size()>0){
      graphics.drawCenteredBanner(64,48,stepchild.pitchToString(maskRoot,false,true)+" major");
    }
    else{
        graphics.printSmall(0,52,"[Loop] to choose notes from a scale",1);
    }
    stepchild.display.display();
  }
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

QChord getChordFromPlaylist(){
    vector<uint8_t> pitches;
    for (int i = 0; i < stepchild.receivedNotes.notes.size(); i++) {
        pitches.push_back(stepchild.receivedNotes.notes[i].pitch);
    }
    return QChord(pitches);
}

//Inserts a chord and makes a new track for each pitch
void insertChordAt(uint16_t timestep,uint16_t length, uint8_t trackLoc, QChord chord, uint8_t channel, uint8_t velocity){
    for(uint8_t i = 0; i<chord.intervals.size(); i++){
        int16_t trackID = stepchild.insertTrack_return(chord.intervals[i]+chord.octave()*12,channel,false, trackLoc);
        stepchild.makeNote(trackID,timestep,length,velocity,100,false,false,false);
    }
}

void insertChordIntoSequence(QChord chord){
    uint16_t length = stepchild.subDivision;
    uint8_t channel = 1;
    uint8_t velocity = 127;
    stepchild.selectionBox.begun = true;
    while(true){
        stepchild.buttons.readButtons();
        stepchild.buttons.readJoystick();
        defaultJoystickControls(false);

        stepchild.selectionBox.coords.start.x = stepchild.cursorPos+length;
        stepchild.selectionBox.coords.start.y = stepchild.activeTrack+chord.intervals.size();
        if(stepchild.selectionBox.coords.end.y>=stepchild.trackData.size()){
            stepchild.selectionBox.coords.end.y = stepchild.trackData.size()-1;
        }

        //Encoders
        while(stepchild.buttons.counterA != 0){
            //changing zoom
            if(stepchild.buttons.counterA >= 1){
                if(stepchild.buttons.SHIFT() && (length<(192-stepchild.subDivision))){
                    length+=stepchild.subDivision;
                }
                else if(!stepchild.buttons.SHIFT()){
                    stepchild.zoom(true);
                }
            }
            if(stepchild.buttons.counterA <= -1){
                if(stepchild.buttons.SHIFT() && (length-stepchild.subDivision)>0){
                    length-=stepchild.subDivision;
                }
                else if(!stepchild.buttons.SHIFT()){
                    stepchild.zoom(false);
                }
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
        }
        while(stepchild.buttons.counterB != 0){
            //if shifting, toggle between 1/3 and 1/4 mode
            if(stepchild.buttons.SHIFT()){
                stepchild.toggleTriplets();
            }
            else if(stepchild.buttons.counterB >= 1){
                stepchild.changeSubDivInt(true);
            }
            //changing subdivint
            else if(stepchild.buttons.counterB <= -1){
                stepchild.changeSubDivInt(false);
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
        }
        if(stepchild.itsbeen(200)){
            //quit
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                break;
            }
            if(stepchild.buttons.NEW()){
                stepchild.lastTime = millis();
                insertChordAt(stepchild.cursorPos,length,stepchild.activeTrack,chord,channel,velocity);
                stepchild.selectionBox.begun = false;
                break;
            }
        }
        stepchild.display.clearDisplay();
        SequenceRenderSettings settings;
        settings.topLabels = false;
        graphics.drawSeq(settings);
        stepchild.display.fillRect(0,0,55,15,0);
        stepchild.display.drawRect(0,0,55,15,1);
        graphics.printSmall(0,0,"[Sh]+[B] for +/-",1);
        graphics.printSmall(0,7,"[N] to commit!",1);
        stepchild.display.display();
    }
}

bool chordDJ(){
    //at most, 16 QChords that can be toggled by the step buttons (w/stepchild.buttons.SHIFT() for the second 8)
    //init with the major and minor chords in C
    vector<QChord> chords = initChordDjList(1,50);
    uint8_t vel = 100;
    uint8_t channel = 0;
    uint8_t activeChord = 0;
    while(true){
        stepchild.display.clearDisplay();
        graphics.printItalic(5,0,"chord dj",1);
        stepchild.display.drawFastHLine(0,3,5,1);
        stepchild.display.drawFastHLine(44,3,7,1);
        stepchild.display.drawFastHLine(66,3,128-66,1);

        //idea is to have 8 vertical columns with the root root pitch and interval numbers in them, as well as a play indicator
        //128/8 = 16,so 12px will give you a 4px gap between each
        for(uint8_t i = 0; i<8; i++){
            uint8_t startY = 24;
            if(i<chords.size()){
                if(i == activeChord){
                    graphics.drawArrow(8+i*16,startY+(millis()/200)%2-7,3,ARROW_DOWN,true);
                    startY = 20;
                }
                graphics.printSmall_centered(9+i*16,startY-12,stringify(i+1),1);
                for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
                    if(j>4){
                        stepchild.display.drawPixel(6+i*16,startY+1+j*6,1);
                        stepchild.display.drawPixel(6+i*16,startY+3+j*6,1);
                        stepchild.display.drawPixel(6+i*16,startY+5+j*6,1);
                        break;
                    }
                    graphics.printSmall_centered(8+i*16,startY+j*6,stepchild.pitchToString(chords[i].intervals[j]+chords[i].root,false,true),1);
                }
                if(chords[i].playing){
                    stepchild.display.fillRect(3+i*16,startY-2,11,66-startY,2);
                }
                else{
                    stepchild.display.drawRect(2+i*16,startY-2,13,66-startY,1);
                }
            }
        }
        stepchild.display.display();
        stepchild.buttons.readButtons();
        stepchild.buttons.readJoystick();

        //while shifting, set the active chord to whatever keys are held
        if(stepchild.buttons.SHIFT()){
            chords[activeChord] = getChordFromPlaylist();
        }
        if(stepchild.itsbeen(200)){
            //Exiting
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                if(binarySelectionBox(64,32,"no","yes","exit?",drawSeq) == 1){
                    break;
                }
            }
            //Editing a chord
            if(stepchild.buttons.SELECT() ){
                stepchild.lastTime = millis();
                chords[activeChord] = editChord(chords[activeChord]);
            }
            //Moving cursor around
            if(stepchild.buttons.joystickX != 0){
                if(stepchild.buttons.joystickX == 1 && activeChord>0){
                    activeChord--;
                    stepchild.lastTime = millis();
                }
                else if(stepchild.buttons.joystickX == -1 && activeChord<7){
                    activeChord++;
                    stepchild.lastTime = millis();
                }
            }
            //Committing a chord to the main sequence
            if(stepchild.buttons.NEW()){
                stepchild.lastTime = millis();
                insertChordIntoSequence(chords[activeChord]);
            }
        }
        //checking step buttons to see if a chord should be played
        for(uint8_t i = 0; i<chords.size(); i++){
            if(i<8){
                if(!stepchild.buttons.SHIFT() && stepchild.buttons.stepButton(i)){
                    chords[i].play(vel,channel);
                }
                else{
                    chords[i].stop(channel);
                }
            }
            else{
                if(stepchild.buttons.SHIFT() && stepchild.buttons.stepButton(i-8)){
                    chords[i].play(vel,channel);
                }
                else{
                    chords[i].stop(channel);
                }
            }
        }
    }
    return true;
}
