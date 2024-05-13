//timing vars
uint32_t timeLastStepPlayed,MicroSperTimeStep,startTime;
int32_t offBy,timeElapsed;
uint16_t bpm = 120;
bool swung = false;

//swing vars
//holds the amount that the notes swing
int32_t swingVal = 4000;
//holds the subdiv the notes are swung to
uint16_t swingSubDiv = 96;

class StepchildClock{
    public:

    bool isSwinging = false;
    int32_t swingAmplitude = 4000;
    uint16_t swingSubDiv = 96;

    uint16_t bpm = 120;

    uint32_t MicroSperTimeStep;
    uint32_t timeLastStepPlayed;
    uint32_t startTime;

    int32_t offBy;//Stores the amount of uSeconds that the last step over/undershot by
    int32_t timeElapsed;

    StepchildClock(){
    }

    //returns the amount a timestep should be shifted (in uSeconds) based on the swing curve
    float swingOffset(uint16_t step){
        return this->swingAmplitude*sin(2*PI/this->swingSubDiv * (step-this->swingSubDiv/4));
    }

    //this is a sloppy lil function that returns true if the time is within (x) of the subDiv
    //Currently only used to make the pram bounce! Should only be used for graphics
    //Or timing-non-crucial uses
    bool onBeat(uint16_t subDiv, int16_t fudge){
        uint16_t msPerB = (240000/(this->bpm*subDiv));
        uint16_t offVal = millis()%msPerB;
        //if it's within fudge of this beat or the next
        return(offVal<=fudge || offVal>= msPerB - fudge );
    }

    //returns true if it's been enough time since this function was last called
    //Slightly less accurate overtime than the "straight" version, since the "offBy"
    //amount is only updated when the timestep aligns with the swing subdivision
    //Errors are small, but can accumulate overtime
    bool hasItBeenEnoughTime_swing(uint16_t step){
        this->timeElapsed = micros()-this->timeLastStepPlayed;
        if(this->timeElapsed >= this->MicroSperTimeStep+this->swingOffset(step)){
            //if it's a multiple of the swing subDiv, it should be perfectly on time, so grab the offset from here
            if(!(step%this->swingSubDiv))
                this->offBy = (micros()-this->startTime)%(this->MicroSperTimeStep);
            //if it's dead on, reset the start timer
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
       this->timeElapsed = micros()-this->timeLastStepPlayed;
        if(this->timeElapsed + this->offBy >= this->MicroSperTimeStep){
            this->offBy = (micros()-this->startTime)%this->MicroSperTimeStep;
            if(this->offBy == 0)
                this->startTime = micros();
            this->timeLastStepPlayed = micros();
            return true;
        }
        return false;
    }
    //selects from either the swung or straight version of the timing function
    //this is called from the timing loop on CPU1
    bool hasItBeenEnoughTime(uint16_t step){
        return this->isSwinging?this->hasItBeenEnoughTime_swing(step):this->hasItBeenEnoughTime_straight();
    }

    //sets a new bpm and adjusts the uS/timestep and swing val accordingly
    void setBpm(int16_t newBpm) {
        //bounds checking
        if(newBpm<=0)
            newBpm = 1;
        else if(newBpm>999)
            newBpm = 999;
        //set new bpm
        this->bpm = newBpm;
        //get new uSeconds val
        this->MicroSperTimeStep = round(2500000/(this->bpm));
        //if the swing val would produce a sub-1uS offset, lower it
        //is this right?? setting it to negative uS/timestep? doesn't look right but not fixing it rn
        if(abs(this->swingAmplitude)>this->MicroSperTimeStep)
            this->swingAmplitude = this->swingAmplitude<0?-this->MicroSperTimeStep:this->MicroSperTimeStep;
    }

};

StepchildClock sequenceClock;

//returns the offset in microseconds at a given step
float swingOffset(unsigned short int step){
  return swingVal*sin(2*PI/swingSubDiv * (step-swingSubDiv/4));
}

//this is a sloppy lil function that returns true if the time is within (x) of the subDiv
bool onBeat(int subDiv, int fudge){
  long msPerB = (240000/(bpm*subDiv));
  long offVal = millis()%msPerB;
  //if it's within fudge of this beat or the next
  if(offVal<=fudge || offVal>= msPerB - fudge ){
    return true;
  }
  else 
    return false;
}

//old,working one without floats (uses %)
bool hasItBeenEnoughTime_swing(uint16_t step){
  timeElapsed = micros()-timeLastStepPlayed;
  if(timeElapsed >= MicroSperTimeStep+swingOffset(step)){
    if(!(step%swingSubDiv)){//if it's a multiple of the swing subDiv, it should be perfectly on time, so grab the offset from here
     offBy = (micros()-startTime)%(MicroSperTimeStep);
    }
    if(offBy == 0)
      startTime = micros();
    timeLastStepPlayed = micros();
    return true;
  }
  else
    return false;
}

bool hasItBeenEnoughTime(uint16_t step){
  if(swung){
    return hasItBeenEnoughTime_swing(step);
  }
  else{
    timeElapsed = micros()-timeLastStepPlayed;
    if(timeElapsed + offBy >= MicroSperTimeStep){
      offBy = (micros()-startTime)%MicroSperTimeStep;
      if(offBy == 0)
        startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    }
    else
      return false;
  }
}

void setBpm(int newBpm) {
  if(newBpm<=0){
    newBpm = 1;
  }
  else if(newBpm>999){
    newBpm = 999;
  }
  bpm = newBpm;
  MicroSperTimeStep = round(2500000/(bpm));
  if(abs(swingVal)>MicroSperTimeStep)
    swingVal = swingVal<0?-MicroSperTimeStep:MicroSperTimeStep;
}
