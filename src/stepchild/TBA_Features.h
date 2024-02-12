void muteGroups(int callingTrack, int group){
  for(int track = 0; track<trackData.size(); track++){
    if(track != callingTrack && trackData[track].muteGroup == group && trackData[track].noteLastSent != 255){
      MIDI.noteOff(trackData[track].noteLastSent,0,trackData[track].channel);
      sentNotes.subNote(trackData[track].noteLastSent);
      trackData[track].noteLastSent = 255;
    }
  }
}

void makeMuteGroup(){
  // vector<unsigned short int> trackIDs = selectMultipleTracks("Tracks in the group...");
}

//fragmenting works on top of the loops, by literally moving the playhead, so it doesn't mess with loop data at all
//data is stored as
//(current size, min size, max size) -- how long the seq plays before re-fragmenting
//(current iterations, min iterations, max iterations) -- how many times the fragment repeats 
//(start position, min start, max start) -- where the fragment sets the cursor to
//default is it starts within the loop
vector<vector<unsigned short int>> fragmentData = {{0,1,4},{0,0,3},{0,0,96}};

uint8_t fragmentIterations;
unsigned short int fragmentStep;
//default is a half note
unsigned short int fragmentSubDiv = 12;

void genFragment(){
  //new fragment length
  fragmentData[0][0] = random(fragmentData[0][1],fragmentData[0][2]+1);
  //new fragment iterations
  fragmentData[1][0] = random(fragmentData[1][1],fragmentData[1][2]+1);
  //fragment start position (always a clean multiple of the subDiv)
  fragmentData[2][0] = fragmentSubDiv * random((fragmentData[2][1],fragmentData[2][2]+1)/fragmentSubDiv);
  fragmentStep = 0;

  //set the active playhead to the start of the fragment
  if(playing)
    playheadPos = fragmentData[2][0];
  else if(recording)
    recheadPos = fragmentData[2][0];
  fragmentStep = 0;
}

void checkFragment(){
  if(isFragmenting){
    fragmentStep++;
    //if the seq is past the end of the fragment
    if(fragmentStep>=fragmentData[0][0]*fragmentSubDiv){
      //if that was the final iteration, generate a new fragment
      if(fragmentData[1][0] == 0){
        genFragment();
      }
      else{
        //Decrease iterations
        fragmentData[1][0]--;
        //resetting fragment step
        fragmentStep = 0;
      }
    }
  }
}

void changeFragmentSubDivInt(bool down){
  if(down){
    if(fragmentSubDiv>3)
      fragmentSubDiv /= 2;
    else if(fragmentSubDiv == 3)
      fragmentSubDiv = 1;
  }
  else{
    if(fragmentSubDiv == 1)//if it's one, set it to 3
      fragmentSubDiv = 3;
    else if(fragmentSubDiv !=  96 && fragmentSubDiv != 32){
      //if triplet mode
      if(!(fragmentSubDiv%2))
        fragmentSubDiv *= 2;
      else if(!(fragmentSubDiv%3))
        fragmentSubDiv *=2;
    }
  }
}


void toggleFragmentTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(fragmentSubDiv == 192){
    fragmentSubDiv = 32;
  }
  else if(!(fragmentSubDiv%3)){//if it's in 1/4 mode...
    fragmentSubDiv = 2*fragmentSubDiv/3;//set it to triplet mode
  }
  else if(!(fragmentSubDiv%2)){//if it was in triplet mode...
    fragmentSubDiv = 3*fragmentSubDiv/2;//set it to 1/4 mode
  }
}

void fragmentAnimation(bool in){
  if(in){
    int width = 20;
    int height = 20;
    int xDepth = 10;
    int yDepth = 10;
    int maxWidth = 100;
    int maxHeight = 100;
    int h = 64;
    int k = 40;
    while(maxWidth>1){
      if(millis()%1000>10){
        height = random(maxHeight-10,maxHeight);
        width = random(maxWidth-10,maxWidth);
      }
      display.clearDisplay();
      drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
      maxWidth-=15;
      maxHeight-=15;
      k-=3;
      display.display();
    }
  }
  else if(!in){
    int width = 20;
    int height = 20;
    int xDepth = 10;
    int yDepth = 10;
    int maxWidth = 0;
    int maxHeight = 0;
    int h = 64;
    int k = 20;
    while(maxWidth<100){
      if(millis()%1000>10){
        height = random(maxHeight-10,maxHeight);
        width = random(maxWidth-10,maxWidth);
      }
      display.clearDisplay();
      drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
      maxWidth+=15;
      maxHeight+=15;
      k+=3;
      display.display();
    }
  }
}

//this should restart USB serial/midi if it's become disconnected
void checkSerial(){
  #ifndef HEADLESS
  if(digitalRead(usbPin) && !TinyUSBDevice.mounted()){
  }
  #endif
}
