/*
    Grooves describe the way notes change across a rythmic phrase beyond their timing:
    chance, velocity

    Each groove has a number of notes, ie data points, and a value for each of those datapoints
*/

enum GrooveCommitType{
    Absolute,Relative
};
enum GrooveInterpType{
    None,Linear
};
enum GrooveTargetType{
    Velocity,Chance
};

class Groove{
    public:
        vector<uint8_t> data;
        Groove(){}
        Groove(uint8_t numberOfSteps){
            for(uint8_t i = 0; i<numberOfSteps; i++){
                this->data.push_back(127);
            }
        }
        Groove(vector<uint8_t> newData){
            this->data = newData;
        }
        //commits a groove to a range of timesteps with a specific commit type and interpolation type
        //"absolute" commit type sets the note data value to the groove value, "relative" 
        //"none" interpType means the groove will skip notes that don't begin exactly on a groove step, "linear" means it will linear interpolate between two groove steps if a note lands between them
        //"targetType" defines which note data param is edited by the groove
        void commitToSequence(uint8_t track, uint16_t start, uint16_t end, GrooveCommitType commitType, GrooveInterpType interpType, GrooveTargetType targetType){
            //check bounds
            if(start>end){
                uint16_t temp = start;
                start = end;
                end = temp;
            }
            else if(start == end){
                return;
            }
            
            uint16_t length = end-start;//length you're applying the groove to
            uint16_t stepSize = length/this->data.size();
            uint8_t currentGrooveStep = 0;

            //Not implemented yet!
            if(interpType == GrooveInterpType::Linear){
            }
            else{
                for(uint16_t i = start; i<end; i+=stepSize){
                    //if a note starts right here
                    if(sequence.IDAt(track,i) != 0 && sequence.noteAt(track,i).startPos == i){
                        Note targetNote = sequence.noteAt(track,i);
                        switch(targetType){
                            case GrooveTargetType::Velocity:
                                targetNote.velocity = this->data[currentGrooveStep];
                                break;
                            case GrooveTargetType::Chance:
                                targetNote.chance = this->data[currentGrooveStep];
                                break;
                        }
                        sequence.noteData[track][sequence.IDAt(track,i)] = targetNote;
                    }
                    currentGrooveStep++;
                }
            }
        }
};

Groove createGrooveFromAutotrack(uint8_t which, uint16_t start, uint16_t end, uint8_t steps){
    //check bounds
    if(start>end){
        uint16_t temp = start;
        start = end;
        end = temp;
    }
    else if(start == end){
        return Groove();
    }
    uint16_t stepLength = (end-start)/steps;
    vector<uint8_t> newData;
    for(uint16_t i = start; i<end; i+=stepLength){
        newData.push_back(sequence.autotrackData[which].data[i]);
    }
    return Groove(newData);
}
// Groove createGrooveFromSequence(uint8_t track, uint16_t start, uint16_t end, uint8_t steps){

// }