#include "Stepchild.h"
#include "menus/superpositionMenu.h"
#include "StepchildGraphics.h"
#include "mainSequence.h"

extern StepchildGraphics graphics;
extern Stepchild stepchild;
extern void chop();
extern bool (*const fxApplicationFunctions[])();
extern const String fxApplicationInfo[];
extern const unsigned char* fxApplicationIcons[];
extern const String fxApplicationTitles[];
extern MenuReturnValue fxMenu();

// 'pen', 15x15px
const unsigned char pen_bmp []  = {
  0x00, 0x0c, 0x00, 0x12, 0x00, 0x32, 0x00, 0x7c, 0x00, 0xf8, 0x01, 0xb0, 0x03, 0x60, 0x06, 0xc0, 
  0x0d, 0x80, 0x1f, 0x00, 0x3e, 0x00, 0x5c, 0x00, 0x88, 0x00, 0x90, 0x00, 0xe0, 0x00
};

using namespace std;

class NoteEditMenu:public StepchildMenu{
  public:
    //value determining how many subDivs are skipped when stencilling notes
    uint8_t stencil = 1;
    bool editingNote = false;
    uint8_t currentQuickFunction = 0;
    NoteEditMenu(){
      coords = CoordinatePair(stepchild.TRACK_LABEL_WIDTH-7,0,stepchild.SCREEN_WIDTH,stepchild.HEADER_HEIGHT);
    }
    void displayMenu();
    // bool fxListControls();
    bool editMenuControls_editing();
    bool editMenuControls_normal();
    bool editMenuControls();
};

void NoteEditMenu::displayMenu(){
  //drawing the sequence
  SequenceRenderSettings settings;
  bool doneMoving = (coords.start.x == stepchild.TRACK_LABEL_WIDTH-7);
  settings.trackLabels = editingNote||!doneMoving;
  settings.topLabels = !doneMoving;
  settings.drawLoopFlags = !doneMoving;
  settings.drawPram = !doneMoving;
  settings.drawCursor = false;
  settings.displayingVel = cursor != 3;//show chance when cursor is on chance edit icon

  stepchild.display.clearDisplay();
  graphics.drawSeq(settings);

  if(!editingNote){
    //pencil
    if(stepchild.buttons.SHIFT()){
      stepchild.display.fillRoundRect(-2,coords.start.x-32,31,30,3,1);
      stepchild.display.drawBitmap(coords.start.x-20,coords.start.y+1+((millis()/200)%2),pen_bmp,15,15,0);
      graphics.printSmall(coords.start.x-6,coords.start.y+((millis()/200)%2)+12,stringify(stencil),0);
    }
    else{
      //back rect for stencil icon
      stepchild.display.fillRoundRect(-2,coords.start.x-32,31,30,3,0);
      stepchild.display.drawRoundRect(-2,coords.start.x-32,31,30,3,1);
      stepchild.display.drawBitmap(coords.start.x-20,coords.start.y+1+((millis()/200)%2),pen_bmp,15,15,SSD1306_WHITE);
      graphics.printSmall(coords.start.x-6,coords.start.y+((millis()/200)%2)+12,stringify(stencil),SSD1306_WHITE);
    }

    //drawing menu box
    stepchild.display.fillRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, SSD1306_BLACK);
    stepchild.display.drawRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_WHITE);


    //draw edit icons if the tool bar is onscreen
    if(coords.start.x<stepchild.SCREEN_WIDTH){
      //move
      graphics.drawMoveIcon(coords.start.x+3,1,cursor==0);
      //change length
      graphics.drawLengthIcon(coords.start.x+17,4,10,3,cursor == 1);
      //change vel
      graphics.drawVelIcon(coords.start.x+33,1,11,cursor == 2);
      //change chance
      graphics.drawChanceIcon(coords.start.x+47,1,11,cursor == 3);
      //quantize
      graphics.drawQuantIcon(coords.start.x+61,1,11,cursor == 4);
      //humanize
      graphics.drawChopIcon(coords.start.x+75,1,10,cursor == 5);
      //warp
      graphics.drawQuickFunctionIcon(coords.start.x+90,1,11,cursor == 6);
    }

    //arrow highlight
    String txt;
    uint8_t xLoc = 0;
    if(coords.start.x<stepchild.SCREEN_WIDTH){
      switch(cursor){
        //moving notes
        case 0:
          txt = "MOVE";
          xLoc = 8;
          break;
        //length
        case 1:
          txt = "LENGTH";
          xLoc = 23;
          break;
        //vel
        case 2:
          txt = "VEL";
          xLoc = 38;
          break;
        //chance
        case 3:
          txt = "CHANCE";
          xLoc = 52;
          break;
        //quantize
        case 4:
          txt = "SUPOS";
          xLoc = 66;
          break;
        //humanize
        case 5:
          txt = "CHOP";
          xLoc = 80;
          break;
        //warp
        case 6:
          txt = "FX";
          xLoc = 95;
          break;
        default:
          txt = "idk";
          xLoc = 8;
      }
    }
    graphics.drawArrow(coords.start.x+xLoc,13+((millis()/400)%2),stepchild.buttons.SHIFT()?3:4,ARROW_UP,stepchild.buttons.SHIFT());
    //target parameter text (just shows what param you're gonna edit)
    stepchild.display.fillRect(0,coords.start.x-2,31,stepchild.SCREEN_HEIGHT-coords.start.x,0);
    // stepchild.display.drawFastHLine(0,coords.start.x-3,32,1);
    stepchild.display.fillRoundRect(15-txt.length()*2-3,coords.start.x-6,txt.length()*4+5,7,3,SSD1306_WHITE);
    graphics.printSmall(15-txt.length()*2,coords.start.x-5,txt,SSD1306_BLACK);

    //draw note info when you're on a note
    if(stepchild.IDAtCursor() != 0){
      Note activeNote = stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()];
      //length
      stepchild.display.fillCircle(6,coords.start.x+9,3,1);
      graphics.printSmall(5,coords.start.x+7,"L",2);
      graphics.printFractionCentered(18,coords.start.x+7,stepchild.stepsToMeasures(activeNote.endPos-activeNote.startPos));
      //vel
      stepchild.display.fillCircle(6,coords.start.x+20,3,1);
      graphics.printSmall(5,coords.start.x+18,"V",2);
      graphics.print007SegSmall(11,coords.start.x+17,stringify(activeNote.velocity),1);
      //chance
      stepchild.display.fillCircle(6,coords.start.x+31,3,1);
      graphics.printSmall(5,coords.start.x+29,"%",2);
      graphics.print007SegSmall(11,coords.start.x+28,stringify(activeNote.chance),1);
    }
    //if there's no note here...
    else{
      //note count display
      stepchild.display.fillRoundRect(4,coords.start.x+11,22,7,3,SSD1306_WHITE);
      stepchild.display.fillRoundRect(4,coords.start.x+26,22,7,3,SSD1306_WHITE);
      uint8_t noteCount = stepchild.getNoteCount();
      graphics.printSmall(9,coords.start.x+5,"seq",SSD1306_WHITE);
      graphics.printSmall(15-stringify(noteCount).length()*2,coords.start.x+12,stringify(noteCount),SSD1306_BLACK);
      graphics.printSmall(9,coords.start.x+20,"trk",SSD1306_WHITE);
      graphics.printSmall(15-stringify(stepchild.noteData[stepchild.activeTrack].size()-1).length()*2,coords.start.x+27,stringify(stepchild.noteData[stepchild.activeTrack].size()-1),SSD1306_BLACK);
    }
  }
  //editing icons that appear when the note is being edited
  //"while editing note"
  else{
    //back rect for icon
    stepchild.display.fillRoundRect(-2,coords.start.x-32,31,30,3,0);
    stepchild.display.drawRoundRect(-2,coords.start.x-32,31,30,3,1);
    //drawing menu box
    stepchild.display.fillRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_BLACK);
    stepchild.display.drawRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_WHITE);

    String txt;
    switch(cursor){
      //moving notes
      case 0:
        if(millis()%1000 >= 500){
          stepchild.display.drawBitmap(3,1,arrow_1_bmp,16,16,SSD1306_WHITE);
        }
        else{
          stepchild.display.drawBitmap(3,1,arrow_3_bmp,16,16,SSD1306_WHITE);
        }
        if(stepchild.IDAtCursor() != 0){
          //location points
          graphics.printSmall(56,3,stepchild.stepsToPosition(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos,true)+","+stepchild.stepsToPosition(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos,true),SSD1306_WHITE);
        }
        txt = "MOVE";
        break;
      //changing length
      case 1:
        graphics.drawLengthIcon(2,6,16,6,true);
        if(stepchild.IDAtCursor() != 0){
          graphics.printFraction(66,3,stepchild.stepsToMeasures(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos-stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos));
        }
        txt = "LENGTH";
        break;
      //vel
      case 2:
        if(stepchild.IDAtCursor() != 0){
          graphics.printSmall(53,3,"v:"+stringify(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].velocity),SSD1306_WHITE);
          graphics.fillSquareDiagonally(4,2,15,stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].velocity,127);
          graphics.printSmall(10,7,"v",2);
        }
        else{
          graphics.fillSquareDiagonally(4,2,15,0);
          graphics.printSmall(10,7,"v",2);
        }
        graphics.printSmall(78,3,"^/&+[sh]",1);

        txt = "VEL";
        break;
      //chance
      case 3:
        if(stepchild.IDAtCursor() != 0){
          graphics.printSmall(64,3,stringify(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].chance)+"%",SSD1306_WHITE);
          graphics.fillSquareDiagonally(4,2,15,stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].chance,100);
          graphics.printSmall(10,7,"%",2);
        }
        else{
          graphics.fillSquareDiagonally(4,2,15,0);
          graphics.printSmall(10,7,"%",2);
        }
        //control tooltip
        graphics.printSmall(80,3,"^/&+[sh]",1);
        txt = "CHANCE";
        break;
      //setting superposition happens in its own window
      case 4:
        break;
      //chopping notes happens in its own window
      case 5:
        break;
      case 6:
        txt = fxApplicationTitles[currentQuickFunction];
        break;
    }
    //drawing edit param info
    stepchild.display.fillRoundRect(32,2,txt.length()*4+5,7,3,SSD1306_WHITE);
    graphics.printSmall(35,3,txt,SSD1306_BLACK);
  }
  //draw brackets around the selection
  if(stepchild.selectionCount > 0)
    graphics.drawSelectionBracket(settings);
  //or draw cursor bracket
  else if(stepchild.IDAtCursor() != 0)
    graphics.drawNoteBracket(stepchild.noteData[stepchild.activeTrack][stepchild.lookupTable[stepchild.activeTrack][stepchild.cursorPos]],stepchild.activeTrack,settings);

  stepchild.display.display();
}

bool NoteEditMenu::editMenuControls_editing(){
  while(stepchild.buttons.counterB != 0){
    //changing vel
    if(cursor == 2){
      if(stepchild.buttons.counterB<0)
        stepchild.changeVel(-1);
      else{
        stepchild.changeVel(1);
      }
    }
    //changing chance
    else if(cursor == 3){
      if(stepchild.buttons.counterB<0)
          stepchild.changeChance(-1);
      else{
          stepchild.changeChance(1);
      }
    }
    stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
  }
  stepchild.buttons.counterA = 0;

  //joystick
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == 1){
      //highlight = 0 ==> moving notes, 1==> changing vel, 2==> changing chance, 3==> changing length
      if(cursor == 0){
        if(stepchild.moveNotes(0,1)){
          stepchild.setActiveTrack(stepchild.activeTrack + 1, true);
        }
      }
      //vel
      else if(cursor == 2){
        if(stepchild.buttons.SHIFT())
          stepchild.changeVel(-8);
        else
          stepchild.moveToNextNote_inTrack(true);
      }
      //chance
      else if(cursor == 3){
        if(stepchild.buttons.SHIFT())
            stepchild.changeChance(-5);
        else
          stepchild.moveToNextNote_inTrack(true);
      }
      //quant
      else if(cursor == 4){
        if(stepchild.buttons.SHIFT() && stepchild.quantizeAmount>0){
          stepchild.quantizeAmount--;
        }
        else{
          stepchild.moveToNextNote_inTrack(true);
        }
      }
      //humanize
      else if(cursor == 5){
        stepchild.moveToNextNote_inTrack(true);
      }
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1){
      if(cursor == 0){
        if(stepchild.moveNotes(0,-1))
          stepchild.setActiveTrack(stepchild.activeTrack - 1, true);
      }
      else if(cursor == 2){
        if(stepchild.buttons.SHIFT())
          stepchild.changeVel(8);
        else
          stepchild.moveToNextNote_inTrack(false);
      }
      else if(cursor == 3){
        if(stepchild.buttons.SHIFT())
            stepchild.changeChance(5);
        else
          stepchild.moveToNextNote_inTrack(false);
      }
      //quant
      else if(cursor == 4){
        if(stepchild.buttons.SHIFT() && stepchild.quantizeAmount<100){
          stepchild.quantizeAmount++;
        }
        else{
          stepchild.moveToNextNote_inTrack(false);
        }
      }
      //humanize
      else if(cursor == 5){
        stepchild.moveToNextNote_inTrack(false);
      }
      stepchild.drawingNote = false;
      stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickX == 1){
      if(!stepchild.buttons.SHIFT()){
        if(cursor == 1){
          //if it's not on a subDiv
          if(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos%stepchild.subDivision)
            stepchild.changeNoteLength_jumpToEnds(-(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos%stepchild.subDivision));
          //if it is
          else
            stepchild.changeNoteLength_jumpToEnds(-stepchild.subDivision);
          stepchild.lastTime = millis();
        }
        else if(cursor == 0){
          //if it's not on a subDiv
          if(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos%stepchild.subDivision){
            if(stepchild.moveNotes(-(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos%stepchild.subDivision),0)){
              stepchild.moveCursor(stepchild.noteData[stepchild.activeTrack][stepchild.noteData[stepchild.activeTrack].size()-1].startPos-stepchild.cursorPos);
              stepchild.lastTime = millis();
            }
          }
          else{
            if(stepchild.moveNotes(-stepchild.subDivision,0)){
              stepchild.moveCursor(-stepchild.subDivision);
              stepchild.lastTime = millis();
            }
          }
        }
        else{
          stepchild.moveToNextNote(false,false);
          stepchild.lastTime = millis();
        }
      }
      else{
        if(cursor == 0){
          if(stepchild.moveNotes(-1,0)){
            stepchild.moveCursor(-1);
            stepchild.lastTime = millis();
          }
        }
        else if(cursor == 1){
          stepchild.changeNoteLength_jumpToEnds(-1);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveToNextNote(false,false);
          stepchild.lastTime = millis();
        }
      }
    }
    if (stepchild.buttons.joystickX == -1){
      if(!stepchild.buttons.SHIFT()){  
        //special moves (while editing notes) 
        //if it's not on a subDiv
        if(cursor == 1){
          if(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos%stepchild.subDivision){     
            stepchild.changeNoteLength_jumpToEnds(stepchild.subDivision-(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].endPos%stepchild.subDivision));
            stepchild.lastTime = millis();
          }
          else{
            stepchild.changeNoteLength_jumpToEnds(stepchild.subDivision);
            stepchild.lastTime = millis();
          }
        }
        else if(cursor == 0){
          if(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos%stepchild.subDivision){     
            if(stepchild.moveNotes(stepchild.subDivision-stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].startPos%stepchild.subDivision,0)){
              stepchild.moveCursor(stepchild.noteData[stepchild.activeTrack][stepchild.noteData[stepchild.activeTrack].size()-1].startPos-stepchild.cursorPos);
              stepchild.lastTime = millis();
            }
          }
          else{
            if(stepchild.moveNotes(stepchild.subDivision,0)){
              stepchild.moveCursor(stepchild.subDivision);
              stepchild.lastTime = millis();
            }
          }
        }
        else{
          stepchild.moveToNextNote(true,false);
          stepchild.lastTime = millis();
        }
      }
      else{
        if(cursor == 0){
          if(stepchild.moveNotes(1,0)){
            stepchild.moveCursor(1);
            stepchild.lastTime = millis();
          }
        }
        else if(cursor == 1){
          stepchild.changeNoteLength_jumpToEnds(1);
          stepchild.lastTime = millis();
        }
        else{
          stepchild.moveToNextNote(true,false);
          stepchild.lastTime = millis();
        }
      }
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.MENU() || stepchild.buttons.B()){
      editingNote = false;
      stepchild.lastTime = millis();
      return true;
    }
    if(stepchild.buttons.LOOP()){
      switch(cursor){
        //in move/length/vel/chance mode, "loop" just toggles editing
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          editingNote = false;
          stepchild.lastTime = millis();
          return true;
        case 6:
          stepchild.lastTime = millis();
          stepchild.buttons.setLOOP(false);
          fxApplicationFunctions[currentQuickFunction]();
          break;
      }
    }
  }
  return true;
}

bool NoteEditMenu::editMenuControls_normal(){
  //selectionBox
  //when stepchild.buttons.SELECT()  is pressed and stick is moved, and there's no selection box
  if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun && (stepchild.buttons.joystickX != 0 || stepchild.buttons.joystickY != 0)){
    stepchild.selectionBox.begun = true;
    stepchild.selectionBox.coords.start.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.start.y = stepchild.activeTrack;
  }
  //if stepchild.buttons.SELECT()  is released, and there's a selection box
  if(!stepchild.buttons.SELECT()  && stepchild.selectionBox.begun){
    stepchild.selectionBox.coords.end.x = stepchild.cursorPos;
    stepchild.selectionBox.coords.end.y = stepchild.activeTrack;
    stepchild.selectionBox.select();
    stepchild.selectionBox.begun = false;
  }
  if(!stepchild.buttons.NEW()){
    stepchild.drawingNote = false;
  }
  //encoderA changes zoom AND +stepchild.buttons.SHIFT() changes the stencil
  while(stepchild.buttons.counterA != 0){
    if(!stepchild.buttons.SHIFT()){
      //changing zoom
      if(stepchild.buttons.counterA >= 1){
          stepchild.zoom(true);
        }
      if(stepchild.buttons.counterA <= -1){
        stepchild.zoom(false);
      }
    }
    else{
      if(stepchild.buttons.counterA >= 1 && (stencil)<16){
        (stencil)++;
      }
      else if(stepchild.buttons.counterA <= -1 && (stencil)>1){
        (stencil)--;
      }
    }
    stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
  }
  //encoder B steps through menu options AND increments them by one (unlike joystick) when shifted
  while(stepchild.buttons.counterB != 0){
    if(!stepchild.buttons.SHIFT()){
      if(stepchild.buttons.counterB >= 1 && !stepchild.buttons.SHIFT()){
        stepchild.changeSubDivInt(true);
      }
      //changing subdivint
      if(stepchild.buttons.counterB <= -1 && !stepchild.buttons.SHIFT()){
        stepchild.changeSubDivInt(false);
      }
    }
    //if shifting, toggle between 1/3 and 1/4 mode
    else while(stepchild.buttons.counterB != 0 && stepchild.buttons.B()){
      stepchild.toggleTriplets();
    }
    stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
  }
  //joystick
  if(stepchild.itsbeen(100)){
    if (stepchild.buttons.joystickY == 1){
      stepchild.setActiveTrack(stepchild.activeTrack+1,false);
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickY == -1){
      stepchild.setActiveTrack(stepchild.activeTrack-1,false);
      stepchild.lastTime = millis();
    }
    if (stepchild.buttons.joystickX == 1){
      if(!stepchild.buttons.SHIFT()){
        stepchild.moveToNextNote(false,false);
        stepchild.lastTime = millis();
      }
      //stepchild.buttons.SHIFT()
      else if(cursor>0){
        cursor--;
        stepchild.lastTime = millis();
      }
    }
    if (stepchild.buttons.joystickX == -1){
      if(!stepchild.buttons.SHIFT()){  
        stepchild.moveToNextNote(true,false);
        stepchild.lastTime = millis();
      }
      else if(cursor<6){
        cursor++;
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    //this is the 'move and place' key (might make sense to have it be a diff key)
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      return false;
    }
    if(stepchild.buttons.B()){
      stepchild.lastTime = millis();
      return false;
    }
    if(stepchild.buttons.A()){
      stepchild.lastTime = millis();
      return false;
    }
    //moving menu cursor
    if(stepchild.buttons.joystickY != 0 && stepchild.buttons.SHIFT()){
      if(stepchild.buttons.joystickY == 1){
        if(cursor>0){
          cursor--;
          stepchild.lastTime = millis();
        }
        else if(cursor == 0){
          cursor = 6;
          stepchild.lastTime = millis();
        }
      }
      else if(stepchild.buttons.joystickY == -1){
        if(cursor<6){
          cursor++;
          stepchild.lastTime = millis();
        }
        else if(cursor == 6){
          cursor = 0;
          stepchild.lastTime = millis();
        }
      }
    }
    if(stepchild.buttons.LOOP()){
      if(stepchild.buttons.SHIFT()){
        switch(cursor){
          //quick fx
          case 6:
            //trigger fx selection
            stepchild.lastTime = millis();
            stepchild.buttons.setLOOP(false);
            slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
            fxMenu();
            slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
            break;
        }
      }
      else{
        //set superposition
        if(cursor == 4){
          if(stepchild.IDAtCursor()){
            stepchild.lastTime = millis();
            slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
            setSuperpositionMenu(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()],stepchild.activeTrack);
            slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
          }
        }
        //chop
        else if(cursor == 5){
          if(stepchild.IDAtCursor()){
            //trigger fx
            stepchild.buttons.setLOOP(false);
            stepchild.lastTime = millis();
            slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
            chop();
            slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
          }
        }
        //if it's a quickFX
        else if(cursor == 6){
          //trigger fx
          stepchild.buttons.setLOOP(false);
          stepchild.lastTime = millis();
          slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
          fxMenu();
          slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
        }
        //if there are ANY notes jump into edit mode
        else if(stepchild.areThereAnyNotes()){
          if(stepchild.IDAtCursor() == 0){
            stepchild.setCursorToNearestNote();
          }
          editingNote = true;
          stepchild.lastTime = millis();
        }
      }
    }
    if(stepchild.buttons.NEW() && !stepchild.drawingNote && !stepchild.buttons.SELECT() ){
      if(stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        stepchild.stencilNotes(stencil);
      }
    }
    if(stepchild.buttons.SELECT()  && !stepchild.selectionBox.begun){
      unsigned short int id;
      id = stepchild.IDAtCursor();
      //select all
      if(stepchild.buttons.NEW()){
        stepchild.selectAll();
      }
      //select only one
      else if(stepchild.buttons.SHIFT()){
        stepchild.clearSelection();
        stepchild.toggleSelectNote(stepchild.activeTrack, id, false);
      }
      //normal select
      else{
        stepchild.toggleSelectNote(stepchild.activeTrack, id, true);          
      }
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.DELETE()){
      if(stepchild.buttons.SHIFT()){
          stepchild.muteNote(stepchild.activeTrack, stepchild.IDAtCursor(), true);
        stepchild.lastTime = millis();
      }
      else{
        if(stepchild.selectionCount>0){
            stepchild.deleteSelected();
        }
        stepchild.deleteNote();
      }
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.PLAY()){
      if(stepchild.buttons.SHIFT())
        stepchild.toggleRecording(stepchild.waitForNoteBeforeRec);
      else
        stepchild.togglePlay();
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.COPY()){
      stepchild.lastTime = millis();
      if(stepchild.buttons.SHIFT())
        stepchild.clipboard.paste();
      else
        stepchild.clipboard.copy();
    }
  }
  mainSequencerStepButtons();
  return true;
}

bool NoteEditMenu::editMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(editingNote)
    return editMenuControls_editing();
  else
    return editMenuControls_normal();
}

void editMenu(){
  NoteEditMenu noteEditMenu;
  noteEditMenu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
  while(noteEditMenu.editMenuControls()){
    //draw seq without top info, side info, or menus
    noteEditMenu.displayMenu();
  }
  noteEditMenu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
}
