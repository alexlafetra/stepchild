/*
Objects that hold the tracks that get loaded into a sequence when 
*/
class SequenceTemplate{
    public:
    enum SequenceTemplateType:uint8_t{
        GENERATE_FROM_FUNCTION,
        LOAD_FROM_VECTOR,
        UNDEFINED
    };
    vector<Track> tracks;
    vector<Track> (*trackBuildingFn)(void);
    SequenceTemplateType type = UNDEFINED;
    
    SequenceTemplate(){
        tracks = vector<Track>{Track()};
        type = UNDEFINED;
    }
    SequenceTemplate(vector<Track> t){
        tracks = t;
        type = LOAD_FROM_VECTOR;
    }
    SequenceTemplate(vector<Track> (*tBFn)(void)){
        trackBuildingFn = tBFn;
        type = GENERATE_FROM_FUNCTION;
    }
    vector<Track> loadTemplate(){
        switch(type){
            case LOAD_FROM_VECTOR:
                return tracks;
            case GENERATE_FROM_FUNCTION:
                return trackBuildingFn();
        }
        return vector<Track>{Track()};
    }
};

/*
Generic Keyboard
tracks on a single channel from pitch 1-127
*/

vector<Track> genericKeyboardTracks(){
    vector<Track> tracks = {};
    for(int8_t i = 127; i>=0; i--){
        tracks.push_back(Track(i,1));
    }
    return tracks;
}

const SequenceTemplate GENERIC_KEYBOARD_TEMPLATE(genericKeyboardTracks);

/*
SP404mk2
16 tracks on one channel, from pitch 36 to 52
*/

vector<Track> sp404mk2Tracks(){
    vector<Track> tracks = {};
    for(uint8_t i = 0; i<16; i++){
        tracks.push_back(Track(51-i,1));
    }
    return tracks;
}

const SequenceTemplate SP404MK2_TEMPLATE(sp404mk2Tracks);

/*
Korg Volca
*/

/*
Alesis SR16
*/

