void drumPad(){
  drumPadAnimation(screenWidth-25,5,36,16, true);
  while(true){
    controls.readButtons();
    mainSequencerStepButtons();
    //changing pitch range
    while(controls.counterA != 0){
      if(controls.counterA >= 1 && 36<127){
        // 36++;
      }
      else if(controls.counterA <= -1 && 36>0){
        // 36--;
      }
      controls.counterA += controls.counterA<0?1:-1;
    }
    while(controls.counterB != 0){
      if(controls.counterB >= 1 && sequence.defaultChannel<16){
        sequence.defaultChannel++;
        controls.counterB += controls.counterB<0?1:-1;;
      }
      else if(controls.counterB <= -1 && sequence.defaultChannel>0){
        sequence.defaultChannel--;
        controls.counterB += controls.counterB<0?1:-1;;
      }
    }
    //play/rec
    if(utils.itsbeen(200)){
      if(controls.PLAY() && !sequence.recording()){
        if(!controls.SHIFT()){
          lastTime = millis();
          togglePlayMode();
        }
        else if(controls.SHIFT()){
          lastTime = millis();
          toggleRecordingMode(waitForNoteBeforeRec);
        }
      }
      if(controls.PLAY() && sequence.recording()){
        lastTime = millis();
        toggleRecordingMode(waitForNoteBeforeRec);
      }
      if(controls.MENU()){
        lastTime = millis();
        break;
      }
    }
    display.clearDisplay();
    drawDrumPads(screenWidth-25,5,36 - 36%12,16);
    display.display();
  }
  drumPadAnimation(screenWidth-25,5,36,16, false);
}