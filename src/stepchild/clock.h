//Encapsulates all the special timing functions!

class StepchildClock{
    public:

    bool isSwinging = false;
    int32_t swingAmplitude = 4000;
    uint16_t swingSubDiv = 96;

    uint16_t BPM = 120;

    //Maximum val in a 4byte number is 4294967296 uS
    //==> ~4295 S
    //==> ~71 hours
    uint32_t uSecPerStep = 20833;//length of a timestep in uSeconds (corresponds to bpm), rounded to nearest uS
    uint32_t timeLastStepPlayed;//stores the time in uSeconds of the last timing fn call
    uint32_t startTime;//stores uS since playing/recording started

    int32_t offBy;//Stores the amount of uSeconds that the last step over/undershot by

    StepchildClock(){}

    //returns the amount a timestep should be shifted (in uSeconds) based on the swing curve
    float swingOffset(uint16_t step){
        return this->swingAmplitude*sin(2*PI/this->swingSubDiv * (step-this->swingSubDiv/4));
    }

    //this is a sloppy lil function that returns true if the time is within (x) of the subDiv
    //Currently only used to make the pram bounce! Should only be used for graphics
    //Or timing-non-crucial uses
    bool onBeat(uint16_t subDiv, int16_t fudge){
        uint16_t msPerB = (240000/(this->BPM*subDiv));
        uint16_t offVal = millis()%msPerB;
        //if it's within fudge of this beat or the next
        return(offVal<=fudge || offVal>= msPerB - fudge );
    }

    //returns true if it's been enough time since this function was last called
    //Slightly less accurate overtime than the "straight" version, since the "offBy"
    //amount is only updated when the timestep aligns with the swing subdivision
    //Errors are small, but can accumulate overtime
    bool hasItBeenEnoughTime_swing(uint16_t step){
        int32_t timeElapsed = micros()-this->timeLastStepPlayed;
        if(timeElapsed >= this->uSecPerStep+this->swingOffset(step)){
            //if it's a multiple of the swing subDiv, it should be perfectly on time, so grab the offset from here
            if(!(step%this->swingSubDiv))
                this->offBy = (micros()-this->startTime)%(this->uSecPerStep);
            //if it's dead on, reset the start timer so it doesn't overflow (it'll prob overflow sometimes)
            if(this->offBy == 0)
                this->startTime = micros();

            this->timeLastStepPlayed = micros();
            return true;
        }
        return false;
    }
    //Returns true if it's been enough time since this function was last
    //called. This one is more accurate because the "offBy" val is updated
    //each time the function returns true
    bool hasItBeenEnoughTime_straight(){
       int32_t timeElapsed = micros()-this->timeLastStepPlayed;
        if(timeElapsed + this->offBy >= this->uSecPerStep){
            this->offBy = (micros()-this->startTime)%this->uSecPerStep;
            if(this->offBy == 0)
                this->startTime = micros();
            this->timeLastStepPlayed = micros();
            return true;
        }
        return false;
    }
    //selects from either the sequenceClock.isSwinging or straight version of the timing function
    //this is called from the timing loop on CPU1
    bool hasItBeenEnoughTime(uint16_t step){
        return this->isSwinging?this->hasItBeenEnoughTime_swing(step):this->hasItBeenEnoughTime_straight();
    }

    //sets a new BPM and adjusts the uS/timestep and swing val accordingly
    void setBPM(int16_t newBpm) {
        //bounds checking
        if(newBpm<=0)
            newBpm = 1;
        else if(newBpm>999)
            newBpm = 999;
        //set new BPM
        this->BPM = newBpm;
        //get new uSeconds val
        this->uSecPerStep = round(2500000/(this->BPM));
        //if the swing val would produce a sub-1uS offset, lower it
        //is this right?? setting it to negative uS/timestep? doesn't look right but not fixing it rn
        if(abs(this->swingAmplitude)>this->uSecPerStep)
            this->swingAmplitude = this->swingAmplitude<0?-this->uSecPerStep:this->uSecPerStep;
    }

};

StepchildClock sequenceClock;
