//Live Loop Mode!
/*

    This little app lets you do performance-focused looping, with reduced editing controls.
    While the seq is playing, the encoders are also used to send CC data.

    when the livelooper is enabled, some changes occur in the way the stepchild handles midi data:
    - recording is triggered on/off by the trigger note
    - seq immediately starts playing when recording ends
    - (optional) the loop points are ignored and the loop end is set when the 

*/

struct LiveLooper{
    //on/off
    bool active = false;
    //makes it so the recordhead ignores the loop bounds, and the loop end is set when recording ends
    bool setLoopBoundsAfterRec = false;
    bool setLoopBoundsAfterRec_flag = setLoopBoundsAfterRec;

    //quantize (not implemented yet)
    bool quantize = false;
    uint16_t quantizeToNearest = 1;

    //note that starts/stops recording (should you have a separate var for CC triggers?)
    NoteData triggerNote = NoteData(60,0,1);
    
    LiveLooper(){}
    //runs when a start message is sent, or when the start trigger is sent
    void start(bool fromTrigger);
    void stop();
    bool checkNote(uint8_t channel, uint8_t note, uint8_t vel);
};

LiveLooper liveLoop;