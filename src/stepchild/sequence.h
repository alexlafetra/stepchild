
//Providing all the data and functions that the stepchild needs to run the sequence, but none of the graphics
class StepchildSequence{
    public:
    vector<Note> noteData;
    vector<vector<uint16_t>> lookupData;

    uint16_t activeTrack;
    uint16_t cursorPos;
    uint16_t playheadPos;
    uint16_t recheadPos;

    vector<Loop> loopData;
    uint8_t activeLoop;
    uint8_t isLooping;

    uint16_t bpm;

    uint16_t length;//seqEnd became length
    uint16_t viewStart;
    uint16_t viewEnd;

    uint8_t subDivInt;

    uint8_t playState;
    uint8_t defaultChannel;
    uint8_t defaultPitch;
    uint8_t defaultVel;
    StepchildSequence(){}

    void init(uint8_t numberOfTracks,uint16_t length){

        this->defaultChannel = 1;
        this->defaultPitch = 36;
        this->defaultVel = 127;
        this->length = length;
        this->viewStart = 0;
        this->viewEnd = 192;
        this->subDivInt = 24;

        Loop firstLoop = Loop(0,96,0,0);
        this->loopData.push_back(firstLoop);
    }
    void init(){

    }
};
