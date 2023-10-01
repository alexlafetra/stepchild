class NoteID{
    public:
        NoteID(uint8_t, uint16_t);
        Note getNote();
        uint8_t getPitch();
        uint8_t track;
        uint16_t id;
};

NoteID::NoteID(uint8_t t, uint16_t i){
    track = t;
    id = i;
}
Note NoteID::getNote(){
    return seqData[track][id];
}
uint8_t NoteID::getPitch(){
    return trackData[track].pitch;
}