void drumPad(){
  drumPadAnimation(screenWidth-25,5,36,16, true);
  while(true){
    controls.readButtons();
    stepButtons();//handles notes, and toggling
    //midi messages from encoders when in keys/drumpads mode
    while(controls.counterA != 0){
      if(controls.counterA >= 1 && keyboardPitch<127){
        keyboardPitch++;
      }
      else if(controls.counterA <= -1 && keyboardPitch>0){
        keyboardPitch--;
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