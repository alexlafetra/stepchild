//returns a byteCount as a more recognizeable value, like 7kB or 5mB
String bytesToString(uint32_t bytes){
  //if it's less than 500, print bytes as-is
  if(bytes<500){
    return stringify(bytes)+"B";
  }
  //probably never going to go over this
  else if(bytes<500000){
    //divide by 100 so the decimal value is now in the 1's place
    bytes/= 100;
    String byteCount = stringify(bytes);
    byteCount += byteCount[byteCount.length()-1];
    byteCount[byteCount.length()-2] = '.';
    return byteCount+"kB";
  }
  else{
    return "bruh";
  }
}

#ifdef HEADLESS
  void flashTest(){
    return;
  }
  void writeSeqFile(String fname){
    return;
  }
  void writeBytes(uint8_t* byteArray, uint16_t number){
    return;
  }
  void writeCurrentSeqToSerial(bool w){
    return;
  }
  void sendByteCount(String filename){
    return;
  }
  void sendFileName(String filename){
    return;
  }
  void exportSeqFileToSerial_standAlone(String filename){
    return;
  }
  void exportSeqFileToSerial(String filename){
    return;
  }
  void dumpFilesToSerial(){
    return;
  }
  uint32_t getByteCount_standAlone(String filename){
    return 0;
  }
  uint32_t getByteCount(String filename){
    return 0;
  }
  void loadSeqFile(String filename){
    return;
  }
  vector<String> loadFilesAndSizes(){
    vector<String> fileSizes = {"0"};
    return fileSizes;
  }
  vector<String> loadFiles(){
    vector<String> filenames = {"*new*","test","beebebebbe","idk","i love you","sp404","microKORG"};
    return filenames;
  }
  void renameSeqFile(String filename){
    return;
  }
  bool deleteSeqFile(String filename){
    return false;
  }
  void duplicateSeqFile(String filename){
    return;
  }
  void writeCurrentSettingsToFile(){
  }

  void loadSavedSettingsFromFile(){
  }
  void loadSettings(){
  }
#endif
#ifndef HEADLESS

//flash mem
void flashTest(){
  char totalNotes;
  for(int i = 0; i<trackData.size(); i++){
    totalNotes += seqData[i].size()-1;
  }

  LittleFS.begin();

  //reading
  unsigned char buff[32];
  File saveFile = LittleFS.open("test.txt","r");
  if(saveFile){
    //Serial.println("Reading:");
    saveFile.read(buff, 31);
    for(int i = 0; i<32; i++){
      //Serial.println(buff[i]);
    }
  }
  else
    //Serial.println("couldn't open save :/");
  saveFile.close();

  //writing
  if(LittleFS.exists("test.txt")){
    //Serial.println("attempting...");
    File file2 = LittleFS.open("test.txt","w");
    if(file2){
      //Serial.println("Writing...");
      for(int i = 0; i<32; i++){
        buff[i] = millis();
      }
      file2.write(buff,32);
      file2.close();
    }
  }
  //Serial.println("ending...");
  LittleFS.end();
}

void writeSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"w");

  //writing seq info
  //start, end
  // uint8_t start[2] = {uint8_t(seqStart>>8),uint8_t(seqStart)};
    uint8_t start[2] = {0,0};
  uint8_t end[2] = {uint8_t(seqEnd>>8),uint8_t(seqEnd)};
  seqFile.write(start,2);
  seqFile.write(end,2);
  //writing track info
  //(number of tracks)
  uint8_t t[1] = {uint8_t(trackData.size())};
  seqFile.write(t,1);
  for(int track = 0; track<trackData.size(); track++){
    //(noteCount1)(noteCount2),(pitch),(channel),(latched),(muteGroup),(primed)
    uint8_t trackDat[7] = {uint8_t((seqData[track].size()-1)>>8),uint8_t(seqData[track].size()-1),uint8_t(trackData[track].pitch),uint8_t(trackData[track].channel),uint8_t(trackData[track].isLatched),uint8_t(trackData[track].muteGroup),uint8_t(trackData[track].isPrimed)};
    seqFile.write(trackDat,7);
  }
  //writing note info
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note < seqData[track].size(); note++){
      //notes are stored start, end,
      //and then vel, chance, selected, muted
      //                              first 8 bits                          last 8 bits
      uint8_t notePosData[4] = {uint8_t(seqData[track][note].startPos>>8),uint8_t(seqData[track][note].startPos),uint8_t(seqData[track][note].endPos>>8),uint8_t(seqData[track][note].endPos)};
      uint8_t noteData[4] = {uint8_t(seqData[track][note].velocity), uint8_t(seqData[track][note].chance), uint8_t(seqData[track][note].isSelected), uint8_t(seqData[track][note].muted)};
      seqFile.write(notePosData,4);
      seqFile.write(noteData,4);
    }
  }
  
  //writing autotrack info, if there are any autotracks
  //number of bytes is split in two! because it's a uint16_t
  //stored as (number of autotracks),(control),(channel),(isActive),(number of bytes1),(number of bytes2),(data)...
  //number of autotracks is always written, even if it's zero!
  uint8_t numOfAutotracks[1] = {uint8_t(autotrackData.size())};
  seqFile.write(numOfAutotracks,1);
  if(autotrackData.size()>0){      
    //writing dt data
    for(uint8_t dt = 0; dt<numOfAutotracks[0]; dt++){
      uint8_t autotrackInfoData[4] = {autotrackData[dt].control,autotrackData[dt].channel,autotrackData[dt].isActive,autotrackData[dt].parameterType};
      uint8_t numberOfDataPoints[2] = {uint8_t(autotrackData[dt].data.size()>>8),uint8_t(autotrackData[dt].data.size())};
      uint8_t autotrackRawData[autotrackData[dt].data.size()];
      for(uint16_t dataPoint = 0; dataPoint<autotrackData[dt].data.size(); dataPoint++){
        autotrackRawData[dataPoint] = autotrackData[dt].data[dataPoint];
      }
      seqFile.write(autotrackInfoData,4);
      seqFile.write(numberOfDataPoints,2);
      seqFile.write(autotrackRawData,autotrackData[dt].data.size());
    }
  }

  //writing loop data
  uint8_t numberOfLoops[1] = {uint8_t(loopData.size())}; 
  seqFile.write(numberOfLoops,1);
  if(loopData.size()>0){
    //loops are stored as
    //(number of loops),(start1),(start2),(end1),(end2),(iterations),(style)
    for(uint8_t loop = 0; loop<loopData.size(); loop++){
      uint8_t start[2] = {uint8_t(loopData[loop].start>>8),uint8_t(loopData[loop].start)};
      uint8_t end[2] = {uint8_t(loopData[loop].end>>8),uint8_t(loopData[loop].end)};
      uint8_t loopDat[2] = {uint8_t(loopData[loop].reps),uint8_t(loopData[loop].type)};
      seqFile.write(start,2);
      seqFile.write(end,2);
      seqFile.write(loopDat,2);
    }
  }
  //writing clock data
  //bpm
  uint8_t bpmBytes[2] = {uint8_t(bpm>>8),uint8_t(bpm)};
  seqFile.write(bpmBytes,2);
  //swing amount
  uint8_t swingAmountBytes[2] = {uint8_t(swingVal>>8),uint8_t(swingVal)};
  seqFile.write(swingAmountBytes,2);
  //swing subDiv
  uint8_t swingSubDivBytes[2] = {uint8_t(swingSubDiv>>8),uint8_t(swingSubDiv)};
  seqFile.write(swingSubDivBytes,2);
  //swing on/off
  uint8_t swingByte[1] = {uint8_t(swung)};
  seqFile.write(swingByte,1);

  //writing routing data
  uint8_t midiChannelBytes[10] = {uint8_t(midiChannels[0]>>8),uint8_t(midiChannels[0]),
                          uint8_t(midiChannels[1]>>8),uint8_t(midiChannels[1]),
                          uint8_t(midiChannels[2]>>8),uint8_t(midiChannels[2]),
                          uint8_t(midiChannels[3]>>8),uint8_t(midiChannels[3]),
                          uint8_t(midiChannels[4]>>8),uint8_t(midiChannels[4])};
  seqFile.write(midiChannelBytes,10);
  
  //writing thru data
  uint8_t midiThruBytes[5];
  for(uint8_t port = 0; port<5; port++){
    midiThruBytes[port] = isThru(port);
  }
  seqFile.write(midiThruBytes,5);

  //writing PC Sequence data
  uint8_t PCSeqData[2];
  for(uint8_t port = 0; port<5; port++){
    PCSeqData[0] = uint8_t(PCData[port].size()>>8);
    PCSeqData[1] = uint8_t(PCData[port].size());
    seqFile.write(PCSeqData,2);
    for(uint16_t message = 0; message<PCData[port].size(); message++){
      //channel, bank, subbank, val, timestep(2)
      uint8_t messageData[6] = {PCData[port][message].channel,PCData[port][message].bank,PCData[port][message].subBank,PCData[port][message].val,uint8_t(PCData[port][message].timestep>>8),uint8_t(PCData[port][message].timestep)};
      seqFile.write(messageData,6);
    }
  }

  // //Serial.println("done");
  // Serial.flush();
  seqFile.close();
  LittleFS.end();
}

//helper function for writing bytes to the serial buffer
void writeBytes(uint8_t* byteArray, uint16_t number){
  Serial.write(byteArray,number);
}

void writeCurrentSeqToSerial(bool waitForResponse){
  //clearing serial buffer
  Serial.flush();

  //writing track info
  //(number of tracks)
  uint8_t t[1] = {uint8_t(trackData.size())};
  writeBytes(t,1);
  for(int track = 0; track<trackData.size(); track++){
    //(noteCount1)(noteCount2),(pitch),(channel),(latched),(muteGroup),(primed)
    uint8_t trackDat[7] = {uint8_t((seqData[track].size()-1)>>8),uint8_t(seqData[track].size()-1),uint8_t(trackData[track].pitch),uint8_t(trackData[track].channel),uint8_t(trackData[track].isLatched),uint8_t(trackData[track].muteGroup),uint8_t(trackData[track].isPrimed)};
    writeBytes(trackDat,7);
  }
  //writing note info
  for(int track = 0; track<trackData.size(); track++){
    for(int note = 1; note < seqData[track].size(); note++){
      //notes are stored start, end,
      //and then vel, chance, selected, muted
      //                              first 8 bits                          last 8 bites
      uint8_t notePosData[4] = {uint8_t(seqData[track][note].startPos>>8),uint8_t(seqData[track][note].startPos),uint8_t(seqData[track][note].endPos>>8),uint8_t(seqData[track][note].endPos)};
      uint8_t noteData[4] = {uint8_t(seqData[track][note].velocity), uint8_t(seqData[track][note].chance), uint8_t(seqData[track][note].isSelected), uint8_t(seqData[track][note].muted)};
      writeBytes(notePosData,4);
      writeBytes(noteData,4);
    }
  }
  
  //writing autotrack info, if there are any autotracks
  //number of bytes is split in two! because it's a uint16_t
  //stored as (number of autotracks),(control),(channel),(isActive),(number of bytes1),(number of bytes2),(data)...
  //number of autotracks is always written, even if it's zero!
  uint8_t numOfAutotracks[1] = {uint8_t(autotrackData.size())};
  writeBytes(numOfAutotracks,1);
  if(autotrackData.size()>0){      
    //writing dt data
    for(uint8_t dt = 0; dt<numOfAutotracks[0]; dt++){
      uint8_t autotrackInfoData[3] = {autotrackData[dt].control,autotrackData[dt].channel,autotrackData[dt].isActive};
      uint8_t numberOfDataPoints[2] = {uint8_t(autotrackData[dt].data.size()>>8),uint8_t(autotrackData[dt].data.size())};
      uint8_t autotrackRawData[autotrackData[dt].data.size()];
      for(uint16_t dataPoint = 0; dataPoint<autotrackData[dt].data.size(); dataPoint++){
        autotrackRawData[dataPoint] = autotrackData[dt].data[dataPoint];
      }
      writeBytes(autotrackInfoData,3);
      writeBytes(numberOfDataPoints,2);
      writeBytes(autotrackRawData,autotrackData[dt].data.size());
    }
  }

  //writing loop data
  uint8_t numberOfLoops[1] = {uint8_t(loopData.size())}; 
  writeBytes(numberOfLoops,1);
  if(loopData.size()>0){
    //loops are stored as
    //(number of loops),(start1),(start2),(end1),(end2),(iterations),(style)
    for(uint8_t loop = 0; loop<loopData.size(); loop++){
      uint8_t start[2] = {uint8_t(loopData[loop].start>>8),uint8_t(loopData[loop].start)};
      uint8_t end[2] = {uint8_t(loopData[loop].end>>8),uint8_t(loopData[loop].end)};
      uint8_t loopDat[2] = {uint8_t(loopData[loop].reps),uint8_t(loopData[loop].type)};
      writeBytes(start,2);
      writeBytes(end,2);
      writeBytes(loopDat,2);
    }
  }

  //writing clock data
  //bpm
  uint8_t bpmBytes[2] = {uint8_t(bpm>>8),uint8_t(bpm)};
  writeBytes(bpmBytes,2);
  //swing amount
  uint8_t swingAmountBytes[2] = {uint8_t(swingVal>>8),uint8_t(swingVal)};
  writeBytes(swingAmountBytes,2);
  //swing subDiv
  uint8_t swingSubDivBytes[2] = {uint8_t(swingSubDiv>>8),uint8_t(swingSubDiv)};
  writeBytes(swingSubDivBytes,2);
  //swing on/off
  uint8_t swingByte[1] = {uint8_t(swung)};
  writeBytes(swingByte,1);

  //writing routing data
  uint8_t midiChannelBytes[10] = {uint8_t(midiChannels[0]>>8),uint8_t(midiChannels[0]),
                          uint8_t(midiChannels[1]>>8),uint8_t(midiChannels[1]),
                          uint8_t(midiChannels[2]>>8),uint8_t(midiChannels[2]),
                          uint8_t(midiChannels[3]>>8),uint8_t(midiChannels[3]),
                          uint8_t(midiChannels[4]>>8),uint8_t(midiChannels[4])};
  writeBytes(midiChannelBytes,10);
  
  //writing thru data
  uint8_t midiThruBytes[5];
  for(uint8_t port = 0; port<5; port++){
    midiThruBytes[port] = isThru(port);
  }
  writeBytes(midiThruBytes,5);
}

//sends the number of bytes in the file
void sendByteCount(String filename){
  uint32_t byteCount = getByteCount(filename);
  uint8_t bytes[4] = {uint8_t(byteCount>>24),uint8_t(byteCount>>16),uint8_t(byteCount>>8),uint8_t(byteCount)};
  Serial.write(bytes,4);
}

void sendFileName(String filename){
  //Serial.print('\n'+filename+'\n');
}

/*
  Exports a seqFile as a series of bytes via USB Serial, the same as the non-standalone version,
  except that this function handles opening and closing the filesystem
*/
void exportSeqFileToSerial_standAlone(String filename){
  LittleFS.begin();
  exportSeqFileToSerial(filename);
  LittleFS.end(); 
}

/*
  Exports a seqFile as a series of bytes via USB Serial.
  ONLY call this function after LittleFS.begin()! Otherwise, use exportSeqFile_standalone.
*/
void exportSeqFileToSerial(String filename){
  //first, send the filename with a newline on either end
  // sendFileName(filename);

  //second, send the number of bytes in the file
  sendByteCount(filename);

  //now you're ready to send ya data!
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    //start, end
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    writeBytes(start,2);
    writeBytes(end,2);

    //loading tracks
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    writeBytes(tracks,1);

    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      writeBytes(trackDat,7);
      noteCount.push_back((uint16_t(trackDat[0])<<8)+trackDat[1]);
    }

    //loading notes
    for(int track = 0; track<tracks[0]; track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        writeBytes(notePosData,4);
        writeBytes(noteData,4);
      }
    }
    
    //loading DT's
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    writeBytes(numberOfDts,1);
    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        writeBytes(dtInfo,4);
        writeBytes(numberOfPoints,2);

        //getting points
        uint16_t numberOfDtPoints = (numberOfPoints[0]<<8)+numberOfPoints[1];
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        writeBytes(points,numberOfDtPoints);
      }
    }
    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    writeBytes(numberOfLoops,1);
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      writeBytes(start,2);
      writeBytes(end,2);
      writeBytes(loopInfo,2);
    }
    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    writeBytes(bpmBytes,2);
    writeBytes(swingAmountBytes,2);
    writeBytes(swingSubDivBytes,2);
    writeBytes(swingByte,1);

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    writeBytes(midiChannelBytes,10);
    writeBytes(midiThruBytes,5);

    seqFile.close();
  }
}

void dumpFilesToSerial(){
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    //write each file to Serial
    while(saves.next()){
      exportSeqFileToSerial(saves.fileName());
    }
  }
  LittleFS.end();
}

//returns Bytes of a file (must be called within LittleFS)
uint32_t getByteCount(String filename){
  uint32_t byteCount = 0;
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    return seqFile.size();
    
    //2 bytes for start and end each
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    byteCount+=4;
    //one byte for number of tracks;
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    byteCount++;
    
    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      //7 bytes per track
      byteCount+=7;
      noteCount.push_back((uint16_t(trackDat[0])<<8)+uint16_t(trackDat[1]));
    }
    //loading notes
    for(int track = 0; track<tracks[0]; track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        //8 bytes per note
        byteCount+=8;
      }
    }
    
    //one byte for number of DTs
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    byteCount++;
    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        //six bytes per DT
        byteCount+=6;

        //getting points
        uint16_t numberOfDtPoints = (uint16_t(numberOfPoints[0])<<8)+uint16_t(numberOfPoints[1]);
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        //one byte for each DT point
        byteCount+=numberOfDtPoints;
      }
    }
    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    //one byte for the number of loops;
    byteCount++;
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      //6 bytes per loop
      byteCount+=6;
    }
    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    //7 bytes for clock data
    byteCount+=7;

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    //15 bytes for routing data
    byteCount+=15;

    seqFile.close();
  }
  return byteCount;
}

uint32_t getByteCount_standAlone(String filename){
  LittleFS.begin();
  uint32_t byteCount = getByteCount(filename);
  LittleFS.end();
  return byteCount;
}

//this makes notes without updating the noteCount, and doesn't check bounds n stuff
void loadNote(int id, int track, int start, int velocity, bool isMuted, int chance, int end, bool selected){
  Note newNoteOn(start, end, velocity, chance, isMuted, false);
  newNoteOn.isSelected = selected;
  if(selected){
    selectionCount++;
  }
  loadNote(newNoteOn, track);
}

void loadNote(Note newNote, uint8_t track){
  //adding to seqData
  seqData[track].push_back(newNote);
  //adding to lookupData
  for (uint16_t i =  newNote.startPos; i < newNote.endPos; i++) { //sets id
    lookupData[track][i] = seqData[track].size()-1;
  }
}


//load a sequence, from a file
void loadSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File seqFile = LittleFS.open(path,"r");
  if(seqFile){
    //replacing sequence with empty data
    newSeq();

    //loading start and end
    uint8_t start[2];
    uint8_t end[2];
    seqFile.read(start,2);
    seqFile.read(end,2);
    // seqStart = (uint16_t(start[0])<<8)+uint16_t(start[1]);
    seqEnd = (uint16_t(end[0])<<8)+uint16_t(end[1]);

    //loading tracks
    uint8_t tracks[1];
    seqFile.read(tracks,1);
    vector<uint16_t> noteCount;
    for(int i = 0; i<tracks[0]; i++){
      uint8_t trackDat[7];//notecount1, notecount2, pitch, channel, isLatched, muteGroup, isPrimed
      seqFile.read(trackDat,7);
      Track newTrack;

      newTrack.pitch = trackDat[2];
      newTrack.channel = trackDat[3];
      newTrack.isLatched = trackDat[4];
      newTrack.muteGroup = trackDat[5];
      newTrack.isPrimed = trackDat[6];

      addTrack(newTrack,false);

      noteCount.push_back((uint16_t(trackDat[0])<<8)+trackDat[1]);
    }

    //loading notes
    for(int track = 0; track<trackData.size(); track++){
      for(int note = 0; note<noteCount[track]; note++){
        //notes are stored start, end,
        //and then vel, chance, selected, muted
        uint8_t notePosData[4];
        uint8_t noteData[4];
        seqFile.read(notePosData,4);
        seqFile.read(noteData,4);
        uint16_t notePos[2] = {uint16_t((notePosData[0]<<8)+notePosData[1]),uint16_t((notePosData[2]<<8)+notePosData[3])};
        loadNote(note+1, track, notePos[0], noteData[0], noteData[3], noteData[1], notePos[1], noteData[2]);
      }
    }
    
    //loading DT's
    uint8_t numberOfDts[1];
    seqFile.read(numberOfDts,1);
    //clear out autotrackData
    vector<Autotrack> newData;
    newData.swap(autotrackData);

    if(numberOfDts[0]>0){
      for(uint8_t dt = 0; dt<numberOfDts[0]; dt++){
        //control, channel, isActive
        uint8_t dtInfo[4];
        uint8_t numberOfPoints[2];
        seqFile.read(dtInfo,4);
        seqFile.read(numberOfPoints,2);
        uint16_t numberOfDtPoints = uint16_t((numberOfPoints[0]<<8)+numberOfPoints[1]);

        //getting points
        uint8_t points[numberOfDtPoints];
        seqFile.read(points,numberOfDtPoints);
        vector<uint8_t> temp;
        for(uint16_t point = 0; point<numberOfDtPoints; point++){
          temp.push_back(points[point]);
        }
        createAutotrack(dtInfo[0],dtInfo[1],dtInfo[2],temp,dtInfo[3]);
      }
    }

    //loading loops
    uint8_t numberOfLoops[1];
    seqFile.read(numberOfLoops,1);
    vector<Loop> newLoopData;
    for(uint8_t loop = 0; loop<numberOfLoops[0]; loop++){
      uint8_t start[2];
      uint8_t end[2];
      uint8_t loopInfo[2];
      seqFile.read(start,2);
      seqFile.read(end,2);
      seqFile.read(loopInfo,2);
      Loop loopDat;
      loopDat.start = uint16_t((start[0]<<8)+start[1]);
      loopDat.end = uint16_t((end[0]<<8)+end[1]);
      loopDat.reps = loopInfo[0];
      loopDat.type = loopInfo[1];
      newLoopData.push_back(loopDat);
    }
    loopData.swap(newLoopData);

    //loading clock data
    uint8_t bpmBytes[2];
    uint8_t swingAmountBytes[2];
    uint8_t swingSubDivBytes[2];
    uint8_t swingByte[1];
    seqFile.read(bpmBytes,2);
    seqFile.read(swingAmountBytes,2);
    seqFile.read(swingSubDivBytes,2);
    seqFile.read(swingByte,1);
    bpm = uint16_t(bpmBytes[0]<<8) + uint16_t(bpmBytes[1]);
    setBpm(bpm);
    swingVal = uint16_t(swingAmountBytes[0]<<8) + uint16_t(swingAmountBytes[1]);
    swingSubDiv = uint16_t(swingSubDivBytes[0]<<8) + uint16_t(swingSubDivBytes[1]);
    swung = swingByte[0];

    //loading routing/thru data
    uint8_t midiChannelBytes[10];
    uint8_t midiThruBytes[5];
    seqFile.read(midiChannelBytes,10);
    seqFile.read(midiThruBytes,5);
    for(uint8_t port = 0; port<5; port++){
      midiChannels[port] = uint16_t(midiChannelBytes[port*2]<<8)+uint16_t(midiChannelBytes[port*2+1]);
      setThru(port,bool(midiThruBytes[port]));
    }

    //loading PC sequence data
    uint8_t PCSeqData[2];
    for(uint8_t port = 0; port<5; port++){
      seqFile.read(PCSeqData,2);
      uint16_t messageCount = (PCSeqData[0]<<8)+PCSeqData[1];
      for(uint16_t message = 0; message<messageCount; message++){
        //channel, bank, subbank, val, timestep(2)
        uint8_t messageData[6];
        seqFile.read(messageData,6);
        PCData[port].push_back(ProgramChange(messageData[0],messageData[1],messageData[2],messageData[3],(messageData[4]<<8)+messageData[5]));
      }
    }

    seqFile.close();
    LittleFS.end();
    updateLEDs();
    setActiveTrack(0,false);
  }
  else{
    //Serial.println("failed! weird.");
  }
}
//loads files into menu, for browsing
//also returns each file's filesize
vector<String> loadFilesAndSizes(){
  vector<String> fileSizes;
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
      fileSizes.push_back(saves.fileName()+" "+bytesToString(getByteCount(saves.fileName())));
    }
  }
  else{
  }
  LittleFS.end();
  return fileSizes;
}

vector<String> loadFiles(){
  vector<String> filenames = {"*new*"};
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
      filenames.push_back(saves.fileName());
    }
  }
  else{
  }
  LittleFS.end();
  return filenames;
}

void renameSeqFile(String filename){
  String newName = enterText("new name?");
  filename = "/SAVES/"+filename;
  newName = "/SAVES/"+newName;
  LittleFS.begin();
  LittleFS.rename(filename,newName);
  LittleFS.end();
}

bool deleteSeqFile(String filename){
  int  choice = binarySelectionBox(64,32,"NO","YEA",fileMenuDisplayWrapper);
  if(choice == 1){
    filename = "/SAVES/"+filename;
    LittleFS.begin();
    LittleFS.remove(filename);
    LittleFS.end();
    return true;
  }
  return false;
}

void duplicateSeqFile(String filename){
  LittleFS.begin();
  String path = "/SAVES/"+filename;
  File targetFile = LittleFS.open(path,"r");
  //copy file data into an array
  uint16_t size = targetFile.size();
  uint8_t data[size];
  targetFile.read(data,size);
  targetFile.close();
  String newPath = "/SAVES/"+filename+"_copy";
  File newFile = LittleFS.open(newPath,"w");
  //write array into new file
  newFile.write(data,size);
  newFile.close();
  LittleFS.end();
}

//Settings ------
/*
display pitches/numbers for tracks
leds on/off
external/internal clock
continue playing after recording y/n
wait for note, or count in, for recording
time before screensaver/sleep --> this needs to be a 16-bit number
*/

void writeCurrentSettingsToFile(){
  LittleFS.begin();
  File f = LittleFS.open("/settings","w");
  uint8_t pOrN[1] = {pitchesOrNumbers};
  uint8_t leds[1] = {LEDsOn};
  uint8_t clock[1] = {clockSource};
  uint8_t playAfterRec[1] = {0};
  uint8_t countIn[1] = {waitForNoteBeforeRec};
  uint8_t idleTime[2] = {uint8_t(sleepTime>>8),uint8_t(sleepTime)};
  f.write(pOrN,1);
  f.write(leds,1);
  f.write(clock,1);
  f.write(playAfterRec,1);
  f.write(countIn,1);
  f.write(idleTime,2);
  f.close();
  LittleFS.end();
}

void loadSavedSettingsFromFile(){
  LittleFS.begin();
  File f = LittleFS.open("/settings","r");
  uint8_t pOrN[1];
  uint8_t leds[1];
  uint8_t clock[1];
  uint8_t playAfterRec[1];
  uint8_t countIn[1];
  uint8_t idleTime[2];
  f.read(pOrN,1);
  f.read(leds,1);
  f.read(clock,1);
  f.read(playAfterRec,1);
  f.read(countIn,1);
  f.read(idleTime,2);
  f.close();
  LittleFS.end();

  pitchesOrNumbers = pOrN[0];
  LEDsOn = leds[0];
  clockSource = clock[0];
  //playAfterRec
  waitForNoteBeforeRec = countIn[0];
  //idleTime
}

//create the default settings file

//loads settings from settings.txt
void loadSettings(){
  LittleFS.begin();
  if(LittleFS.exists("/settings")){
    loadSavedSettingsFromFile();
  }
  //if there isn't a file yet, create one with the current parameters
  else{
    writeCurrentSettingsToFile();
  }
  LittleFS.end();
}

#endif