#include "Stepchild.h"
#include "menus/superpositionMenu.h"

using namespace std;


SuperpositionMenu::SuperpositionMenu(Note& n, uint8_t t){
  note = n;
  track = t;
  //first, set the superpos to the track pitch (if it's unset)
  if(note.superposition.pitch == 255)
    note.superposition.pitch = stepchild.trackData[track].pitch;
}

bool SuperpositionMenu::setSuperpositionControls(){
  stepchild.buttons.readButtons();
  stepchild.buttons.readJoystick();
  while(stepchild.buttons.counterA){
    if(stepchild.buttons.counterA > 0 && note.superposition.odds > 0){
      note.superposition.odds -= 5;
    }
    if(stepchild.buttons.counterA < 0 && note.superposition.odds < 100){
      note.superposition.odds += 5;
    }
    stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
  }
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == -1) {
      if(note.superposition.pitch < 127){
        note.superposition.pitch++;
        if(!stepchild.playing()){
          stepchild.midi.noteOn(note.superposition.pitch, note.velocity, stepchild.trackData[track].channel);
          stepchild.midi.noteOff(note.superposition.pitch, 0, stepchild.trackData[track].channel);
        }
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.buttons.joystickY == 1) {
      if(note.superposition.pitch > 0){
        note.superposition.pitch--;
        if(!stepchild.playing()){
          stepchild.midi.noteOn(note.superposition.pitch, note.velocity, stepchild.trackData[track].channel);
          stepchild.midi.noteOff(note.superposition.pitch, 0, stepchild.trackData[track].channel);
        }
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.NEW() || stepchild.buttons.MENU() || stepchild.buttons.LOOP()){
      stepchild.lastTime = millis();
      if(note.superposition.pitch == stepchild.trackData[track].pitch){
        note.superposition.pitch = 255;//set to 'unset' if it's on the same pitch
      }
      return false;
    }
    if(stepchild.buttons.DELETE()){
      stepchild.lastTime = millis();
      note.superposition.pitch = stepchild.trackData[track].pitch;//reset
    }
  }
  return true;
}

void SuperpositionMenu::drawSuperposSelect(){
  SequenceRenderSettings settings;
  NoteCoords nCoords = graphics.getNoteScreenCoords(note, track, settings);

  const uint8_t headerHeight = 12;

  // NoteCoords n2Coords = nCoords;
  NoteCoords n2Coords = graphics.getNoteScreenCoords(note, track, settings);
  n2Coords.y1 = nCoords.y1+(int16_t(stepchild.trackData[track].pitch)-int16_t(note.superposition.pitch)) * stepchild.trackHeight;
  n2Coords.y2 = n2Coords.y1 + stepchild.trackHeight-3;

  if(n2Coords.y1<headerHeight){
    nCoords.offsetY(-n2Coords.y1+headerHeight);
    n2Coords.offsetY(-n2Coords.y1+headerHeight);
  }
  if(n2Coords.y2>(stepchild.SCREEN_HEIGHT-2)){
    nCoords.offsetY(stepchild.SCREEN_HEIGHT-n2Coords.y2-2);
    n2Coords.offsetY(stepchild.SCREEN_HEIGHT-n2Coords.y2-2);
  }

  //drawing horizontal pitch lines
  for(uint8_t i = headerHeight; i<stepchild.SCREEN_HEIGHT; i++){
    //octave
    if(abs(nCoords.y1 - i)%(12*stepchild.trackHeight) == 0)
      graphics.drawDottedLineH(0,stepchild.SCREEN_WIDTH,i+stepchild.trackHeight/2,2);
    //4 semitones
    else if(abs(nCoords.y1 - i)%(4*stepchild.trackHeight) == 0)
      graphics.drawDottedLineH(0,stepchild.SCREEN_WIDTH,i+stepchild.trackHeight/2,3);
    else if(abs(nCoords.y1 - i)%(stepchild.trackHeight) == 0)
      graphics.drawDottedLineH(0,stepchild.SCREEN_WIDTH,i+stepchild.trackHeight/2,12);
  }

  graphics.drawNoteBracket(n2Coords,true);

  String txt = stringify(note.superposition.pitch-stepchild.trackData[track].pitch);
  if(note.superposition.pitch>stepchild.trackData[track].pitch)
    txt = "+"+txt;

  n2Coords.x1++;
  n2Coords.length-=2;
  n2Coords.y1++;
  n2Coords.y2++;
  //draw superposition
  graphics.fillRectWithMissingCorners(n2Coords.x1, n2Coords.y1-1, n2Coords.length+1, stepchild.trackHeight, SSD1306_BLACK);
  graphics.drawRectWithMissingCorners(n2Coords.x1, n2Coords.y1-1, n2Coords.length+1, stepchild.trackHeight, SSD1306_WHITE);

  graphics.printSmall(n2Coords.x1+n2Coords.length+4,n2Coords.y1+2,txt+"("+stepchild.pitchToString(note.superposition.pitch,true,true)+")",1);
  graphics.drawNote(note, track, nCoords, settings, false);//draw the note (like normal, but no superpos ghost)

  //draw probability
  graphics.fillSquareDiagonally(0,0,15,note.superposition.odds);
  graphics.printSmall(6,5,"%",2);
  graphics.printSmall(17,5,stringify(note.superposition.odds)+"%",1);
  graphics.printCursive(50,4,"superposition",1);
}

void setSuperpositionMenu(Note& note,uint8_t originalTrack){
  SuperpositionMenu superposMenu(note,originalTrack);
  while(superposMenu.setSuperpositionControls()){
    stepchild.display.clearDisplay();
    superposMenu.drawSuperposSelect();
    stepchild.display.display();
  }
  note.superposition = superposMenu.note.superposition;
  stepchild.buttons.clearButtons();
}