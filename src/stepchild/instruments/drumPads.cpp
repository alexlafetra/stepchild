void drumPad(){
  drumPadAnimation(screenWidth-25,5,36,16, true);
  while(true){
    readButtons();
    stepButtons();//handles notes, and toggling
    //midi messages from encoders when in keys/drumpads mode
    while(counterA != 0){
      if(counterA >= 1 && keyboardPitch<127){
        keyboardPitch++;
      }
      else if(counterA <= -1 && keyboardPitch>0){
        keyboardPitch--;
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1 && defaultChannel<16){
        defaultChannel++;
        counterB += counterB<0?1:-1;;
      }
      else if(counterB <= -1 && defaultChannel>0){
        defaultChannel--;
        counterB += counterB<0?1:-1;;
      }
    }
    //play/rec
    if(utils.itsbeen(200)){
      if(controls.PLAY() && !recording){
        if(!controls.SHIFT()){
          lastTime = millis();
          togglePlayMode();
        }
        else if(controls.SHIFT()){
          lastTime = millis();
          toggleRecordingMode(waitForNoteBeforeRec);
        }
      }
      if(controls.PLAY() && recording){
        lastTime = millis();
        toggleRecordingMode(waitForNoteBeforeRec);
      }
      if(controls.MENU()){
        lastTime = millis();
        break;
      }
    }
    display.clearDisplay();
    drawDrumPads(screenWidth-25,5,keyboardPitch - keyboardPitch%12,16);
    display.display();
  }
  drumPadAnimation(screenWidth-25,5,36,16, false);
}