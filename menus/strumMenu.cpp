
// #define BYTRACK_DESC 0//sorts notes by track order
// #define BYTRACK_ASC 1
// #define BYPITCH_DESC 2
// #define BYPITCH_ASC 3
// #define RANDOM 4

// class NoteID{
//     public:
//         NoteID(uint8_t, uint16_t);
//         Note getNote();
//         uint8_t getPitch();
//         uint8_t track;
//         uint16_t id;
// };

// NoteID::NoteID(uint8_t t, uint16_t i){
//     track = t;
//     id = i;
// }
// Note NoteID::getNote(){
//     return seqData[track][id];
// }
// uint8_t NoteID::getPitch(){
//     return trackData[track].pitch;
// }
// bool compareTracks(NoteID n1, NoteID n2){
//     return n1.track>n2.track;
// }
// bool comparePitches(NoteID n1, NoteID n2){
//     return n1.getPitch()>n2.getPitch();
// }
// //sorts a list of [track,note] pairs
// vector<NoteID> sortNotes(vector<NoteID> ids, uint8_t type){
//     vector<NoteID> sortedVec = ids;
//     switch(type){
//         //sort by track, descending
//         case 0:
// //            sort(sortedVec.begin(), sortedVec.end(),compareTracks);
//             return sortedVec;
//         //sort by tracks, ascending
//         case 1:
// //            sort(sortedVec.begin(), sortedVec.end(),!compareTracks);
//             return sortedVec;
        
//     }
//     return sortedVec;
// }

// void drawStrum(uint8_t type, int16_t amount){

// }

// vector<NoteID> grabSelectedNotes(){
//     vector<NoteID> notes;
//     for(uint8_t i = 0; i<seqData.size(); i++){
//         for(uint8_t j = 1; j<seqData[i].size(); j++){
//             if(seqData[i][j].isSelected){
//                 NoteID newNote = NoteID(i,j);
//                 notes.push_back(newNote);
//             }
//         }
//     }
//     return notes;
// }
// void strumMenu(){
//     uint8_t strumType = 0;
//     int16_t offsetAmount = 0;
//     vector<NoteID> ids;
//     while(true){
//         joyRead();
//         readButtons();
//         if(menu_Press){
//             lastTime = millis();
//             return;
//         }
//     }
// }
