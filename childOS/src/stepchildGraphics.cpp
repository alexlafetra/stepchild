#include "StepchildGraphics.h"
#include "graphics/SequenceRenderSettings.h"
#include "commonEnums.h"
#include "classes/Note.h"
#include "graphics/bitmaps.h"
#include "Stepchild.h"
#include "graphics/WireFrame.h"
;

using namespace std;

// 'battery', 10x7px
const unsigned char batt_bmp []  = {
  0x7f, 0x80, 0x40, 0x40, 0x80, 0x40, 0x80, 0x40, 0x80, 0x40, 0x40, 0x40, 0x7f, 0x80
};


// 'child_small', 78x23px
const unsigned char child_blocks_bmp [] = {
	0x00, 0x00, 0x0f, 0xf8, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x18, 0x07, 0xc0, 0x01, 
	0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x40, 0x01, 0x38, 0x1c, 0x00, 0x00, 0x00, 0x00, 
	0x49, 0xef, 0x40, 0x01, 0x18, 0x12, 0x00, 0x00, 0x00, 0x00, 0x48, 0xc6, 0x40, 0x61, 0x18, 0x32, 
	0x00, 0x00, 0x00, 0x00, 0x48, 0xc6, 0x4f, 0xb3, 0x38, 0x32, 0x00, 0x00, 0x01, 0xfe, 0x49, 0xfe, 
	0x78, 0x2b, 0x30, 0x31, 0x00, 0x00, 0xfe, 0x0f, 0x59, 0xfe, 0x40, 0x27, 0x30, 0x37, 0xff, 0xc0, 
	0x80, 0x05, 0x91, 0x8c, 0xde, 0x32, 0x31, 0x3c, 0x00, 0x30, 0x87, 0x84, 0x91, 0x8c, 0x9e, 0x12, 
	0x7f, 0x3b, 0x00, 0x0c, 0x8f, 0xc4, 0x93, 0xcc, 0x8c, 0x12, 0x7f, 0x38, 0xff, 0xfc, 0x9c, 0xc4, 
	0x90, 0x1e, 0x8c, 0x12, 0x00, 0x38, 0x80, 0x04, 0x98, 0x04, 0x90, 0x00, 0x8c, 0x13, 0xff, 0xf8, 
	0x9f, 0x84, 0x98, 0x04, 0x9f, 0xc0, 0x8c, 0x13, 0xff, 0xf8, 0x9c, 0xc4, 0x9c, 0xc2, 0xff, 0xff, 
	0x8e, 0x10, 0xff, 0xf8, 0x8c, 0x64, 0x4f, 0xc2, 0xff, 0xff, 0x1e, 0x10, 0x9f, 0xf8, 0x8c, 0x24, 
	0x47, 0x82, 0x5f, 0xff, 0x00, 0x10, 0x80, 0x08, 0x8c, 0x24, 0x40, 0x06, 0x40, 0x01, 0x07, 0xf8, 
	0x80, 0x08, 0x8c, 0x64, 0x4f, 0xfe, 0x40, 0x01, 0xff, 0xf8, 0x80, 0x0c, 0x8c, 0xc4, 0x7f, 0xff, 
	0x40, 0x00, 0xff, 0xfc, 0x80, 0x06, 0x9f, 0x84, 0x7f, 0xff, 0x40, 0x00, 0x3f, 0xff, 0x80, 0x01, 
	0x80, 0x04, 0x3f, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0x00, 0x01, 0x80, 0x04, 0x0f, 0xff, 0xc0, 0x00, 
	0x07, 0x80, 0x00, 0x00, 0xff, 0xf8
};

// 'c_small', 18x18px
const unsigned char logo_c [] = {
	0x01, 0xfe, 0x00, 0xfe, 0x0f, 0x00, 0x80, 0x05, 0x80, 0x87, 0x84, 0x80, 0x8f, 0xc4, 0xc0, 0x9c, 
	0xc4, 0x40, 0x98, 0x04, 0x40, 0x98, 0x04, 0x40, 0x9c, 0xc2, 0x40, 0x4f, 0xc2, 0x40, 0x47, 0x82, 
	0x40, 0x40, 0x06, 0x40, 0x4f, 0xfe, 0x40, 0x7f, 0xff, 0x40, 0x7f, 0xff, 0x40, 0x3f, 0xff, 0xc0, 
	0x0f, 0xff, 0xc0, 0x03, 0xe0, 0x00
};

// 'i_small', 18x16px
const unsigned char logo_i [] = {
	0x90, 0x18, 0x00, 0x93, 0xec, 0x00, 0x9e, 0x0a, 0x00, 0x90, 0x09, 0x00, 0x37, 0x8c, 0x80, 0x27, 
	0x84, 0x40, 0x23, 0x04, 0x40, 0xa3, 0x04, 0x20, 0x23, 0x04, 0x20, 0x23, 0x04, 0x20, 0xe3, 0x84, 
	0x20, 0xc7, 0x84, 0x20, 0xc0, 0x04, 0x20, 0x41, 0xfe, 0x20, 0x7f, 0xfe, 0x20, 0x3f, 0xff, 0x20, 
	0x0f, 0xff, 0xe0, 0x03, 0xff, 0xc0, 0x01, 0xe0, 0x00
};
// 'h_small', 18x16px
const unsigned char logo_h [] = {
	0x0f, 0xf8, 0x00, 0x18, 0x07, 0xc0, 0x28, 0x00, 0x40, 0x49, 0xef, 0x40, 0x48, 0xc6, 0x40, 0x48, 
	0xc6, 0x40, 0x49, 0xfe, 0x40, 0x59, 0xfe, 0x40, 0x91, 0x8c, 0xc0, 0x91, 0x8c, 0x80, 0x93, 0xcc, 
	0x80, 0x90, 0x1e, 0x80, 0x90, 0x00, 0x80, 0x9f, 0xc0, 0x80, 0xff, 0xff, 0x80, 0xff, 0xff, 0x00, 
	0x5f, 0xff, 0x00
};

// 'l_small', 18x16px
const unsigned char logo_l [] = {
	0x7f, 0xfe, 0x00, 0x40, 0x07, 0x00, 0x4e, 0x07, 0x00, 0x46, 0x04, 0x80, 0x46, 0x0c, 0x80, 0xce, 
	0x0c, 0x80, 0xcc, 0x0c, 0x40, 0xcc, 0x08, 0x40, 0x8c, 0x48, 0x40, 0x9f, 0xc8, 0x40, 0x9f, 0xc8, 
	0x40, 0x80, 0x08, 0x40, 0xff, 0xf8, 0x40, 0xff, 0xfc, 0x40, 0x3f, 0xfc, 0x40, 0x27, 0xff, 0x80, 
	0x20, 0x00, 0x00
};

// 'd_small', 18x16px
const unsigned char logo_d [] = {
	0x7f, 0xfc, 0x00, 0xc0, 0x03, 0x00, 0xb0, 0x00, 0xc0, 0x8f, 0xff, 0xc0, 0x88, 0x00, 0x40, 0x89, 
	0xf8, 0x40, 0x89, 0xcc, 0x40, 0x88, 0xc6, 0x40, 0x88, 0xc2, 0x40, 0x88, 0xc2, 0x40, 0x88, 0xc6, 
	0x40, 0xc8, 0xcc, 0x40, 0x69, 0xf8, 0x40, 0x18, 0x00, 0x40, 0x18, 0x00, 0x40, 0x0f, 0xff, 0x80
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 320)
const unsigned char* child_blocks_logo[5] = {
	logo_c,
	logo_h,
	logo_i,
	logo_l,
	logo_d
};


uint8_t StepchildGraphics::getSmallTextLength(String t){
  return t.length()*4-countChar(t,' ')*2-countChar(t,':')*2+countChar(t,'#')*2;
}

void StepchildGraphics::drawNoteBracket(uint8_t x1, uint8_t y1, uint8_t w, uint8_t y2, bool animated){
  uint8_t offset = animated?((millis()/400)%2):0;
  //topL
  stepchild.display.drawLine(x1-offset,y1-1-offset,x1+3-offset,y1-1-offset,SSD1306_WHITE);
  stepchild.display.drawLine(x1-offset,y1-1-offset,x1-offset,y1+2-offset,SSD1306_WHITE);
  //topR
  stepchild.display.drawLine(x1+w+offset,y1-1-offset,x1+w-3+offset,y1-1-offset,SSD1306_WHITE);
  stepchild.display.drawLine(x1+w+offset,y1-1-offset,x1+w+offset,y1+2-offset,SSD1306_WHITE);
  //bottomL
  stepchild.display.drawLine(x1-offset,y2+offset+2,x1+3-offset,y2+offset+2,SSD1306_WHITE);
  stepchild.display.drawLine(x1-offset,y2+offset+2,x1-offset,y2-3+offset+2,SSD1306_WHITE);
  //bottomR
  stepchild.display.drawLine(x1+w+offset,y2+offset+2,x1+w-3+offset,y2+offset+2,SSD1306_WHITE);
  stepchild.display.drawLine(x1+w+offset,y2+offset+2,x1+w+offset,y2-3+offset+2,SSD1306_WHITE);
}

void StepchildGraphics::drawNoteBracket(NoteCoords& n, bool animated){
  uint8_t offset = animated?((millis()/400)%2):0;
  drawNoteBracket(n.x1,n.y1,n.length,n.y2,animated);
}

void StepchildGraphics::drawNoteBracket(Note& note, uint8_t track, SequenceRenderSettings& settings){
  NoteCoords n = getNoteScreenCoords(note,track,settings);
  drawNoteBracket(n,true);
}


void StepchildGraphics::drawSelectionBracket(SequenceRenderSettings& settings){
  vector<uint16_t> bounds  = stepchild.getSelectedNotesBoundingBox();
  //if the left side is in view
  if(bounds[0]>=stepchild.viewStart){
    //if the top L corner is in view
    uint8_t x1 = (bounds[0]-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-((millis()/200)%2);
    if(bounds[1]>=stepchild.startTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[1]-stepchild.startTrack)*stepchild.trackHeight+settings.startHeight-((millis()/200)%2);
      stepchild.display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1-1,x1+5,y1-1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      stepchild.display.drawLine(x1-1,y1,x1-1,y1+5,SSD1306_WHITE);
    }
    //if the bottom L corner is in view
    if(bounds[3]<=stepchild.endTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[3]-stepchild.startTrack+1)*stepchild.trackHeight+settings.startHeight+((millis()/200)%2);
      stepchild.display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1+1,x1+5,y1+1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      stepchild.display.drawLine(x1-1,y1,x1-1,y1-5,SSD1306_WHITE);
    }
  }
  //if the right corner is in view
  if(bounds[2]<stepchild.viewEnd){
    uint8_t x1 = (bounds[2]-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH+((millis()/200)%2)+1;
    //top R corner
    if(bounds[1]>=stepchild.startTrack){
      uint8_t y1 = (bounds[1]-stepchild.startTrack)*stepchild.trackHeight+settings.startHeight-((millis()/200)%2);
      stepchild.display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1-1,x1-5,y1-1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      stepchild.display.drawLine(x1+1,y1,x1+1,y1+5,SSD1306_WHITE);
    }
    //bottom R corner
    if(bounds[3]<=stepchild.endTrack){
      uint8_t y1 = (bounds[3]-stepchild.startTrack+1)*stepchild.trackHeight+settings.startHeight+((millis()/200)%2);
      stepchild.display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1+1,x1-5,y1+1,SSD1306_WHITE);
      stepchild.display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      stepchild.display.drawLine(x1+1,y1,x1+1,y1-5,SSD1306_WHITE);
    }
  }
}
void StepchildGraphics::drawSelectionBracket(){
  SequenceRenderSettings settings;
  drawSelectionBracket(settings);
}

//this function is a mess! especially the shrinktopdisplay logic
void StepchildGraphics::drawSeq(SequenceRenderSettings& settings){

  //loading in the settings for the shrunk top display
  if(settings.shrinkTopDisplay){
    settings.startHeight = 8;
    settings.drawLoopFlags = false;
    settings.maxTracksShown = 6;
  }
  //calc track height
  stepchild.trackHeight = (stepchild.SCREEN_HEIGHT-settings.startHeight)/settings.maxTracksShown;

  //if there are more tracks than you're going to draw (typically the case), figure out which to draw
  if(stepchild.trackData.size()>settings.maxTracksShown){
    //if you're already looking at the last track, lower your start track
    if(stepchild.endTrack == stepchild.trackData.size()){
        stepchild.startTrack = stepchild.endTrack-settings.maxTracksShown;
    }
    //if not, then increase your end track
    else{
        stepchild.endTrack = stepchild.startTrack + settings.maxTracksShown;
    }
    //if the active track is beyond the end track
    if(stepchild.activeTrack>=stepchild.endTrack){
      stepchild.endTrack = stepchild.activeTrack;
      stepchild.startTrack = stepchild.endTrack-settings.maxTracksShown+1;
    }
    else if(stepchild.activeTrack<stepchild.startTrack){
      stepchild.startTrack = stepchild.activeTrack;
      stepchild.endTrack = stepchild.startTrack+settings.maxTracksShown;
    }
  }
  else{
      stepchild.endTrack = stepchild.startTrack + stepchild.trackData.size();
  }
  //drawing selection box, since it needs to overlay stepSeq data
  if(stepchild.selectionBox.begun){
    stepchild.selectionBox.render(settings);
  }
  //calculating height
  uint8_t height = settings.startHeight+stepchild.trackHeight*settings.maxTracksShown;
  // if(stepchild.endTrack == stepchild.trackData.size())
  //     height = settings.startHeight+stepchild.trackHeight*settings.maxTracksShown;
  // else if(stepchild.trackData.size()>settings.maxTracksShown)
  //     height = settings.startHeight+stepchild.trackHeight*(settings.maxTracksShown+1);
  // else
  //     height = settings.startHeight+stepchild.trackHeight*stepchild.trackData.size();

  //drawing measure bars, loop points
  drawSeqBackground(settings, height);

  //drawing cursor
  if(settings.drawCursor){
      uint8_t cPos = stepchild.TRACK_LABEL_WIDTH+uint8_t((stepchild.cursorPos-settings.start)*stepchild.viewScale);
      if(cPos>127)
          cPos = 126;
      if(stepchild.endTrack == stepchild.trackData.size()){
          stepchild.display.drawFastVLine(cPos, settings.startHeight, height, SSD1306_WHITE);
          stepchild.display.drawFastVLine(cPos+1, settings.startHeight, height, SSD1306_WHITE);
      }
      else{
          stepchild.display.drawFastVLine(cPos, settings.startHeight, stepchild.SCREEN_HEIGHT-settings.startHeight, SSD1306_WHITE);
          stepchild.display.drawFastVLine(cPos+1, settings.startHeight, stepchild.SCREEN_HEIGHT-settings.startHeight, SSD1306_WHITE);
      }
  }

  //drawing active track highlight
  uint8_t y1 = (stepchild.activeTrack-stepchild.startTrack) * stepchild.trackHeight + settings.startHeight;
  // stepchild.display.drawRect(x1, y1, stepchild.SCREEN_WIDTH, stepchild.trackHeight, SSD1306_WHITE);
  stepchild.display.drawFastHLine(stepchild.TRACK_LABEL_WIDTH,y1,stepchild.SCREEN_WIDTH-stepchild.TRACK_LABEL_WIDTH,1);
  stepchild.display.drawFastHLine(stepchild.TRACK_LABEL_WIDTH,y1+stepchild.trackHeight-1,stepchild.SCREEN_WIDTH-stepchild.TRACK_LABEL_WIDTH,1);

  //top and bottom bounds
  if(stepchild.startTrack == 0){
    stepchild.display.drawFastHLine(stepchild.TRACK_LABEL_WIDTH,settings.startHeight,stepchild.SCREEN_WIDTH,SSD1306_WHITE);
  }
  //if the bottom is in view
  if(stepchild.endTrack == stepchild.trackData.size()){
    stepchild.display.drawFastHLine(stepchild.TRACK_LABEL_WIDTH,settings.startHeight+stepchild.trackHeight*settings.maxTracksShown,stepchild.SCREEN_WIDTH,SSD1306_WHITE);
  }
  //tiny trk label
  // stepchild.display.setRotation(DISPLAY_SIDEWAYS_R);
  // stepchild.display.drawBitmap(settings.startHeight,125,trk_tiny,10,3,1);
  // stepchild.display.setRotation(DISPLAY_UPRIGHT);


  //drawin all da steps
  //---------------------------------------------------
  for(uint8_t track = stepchild.startTrack; track < stepchild.endTrack; track++) {
      unsigned short int y1 = (track-stepchild.startTrack) * stepchild.trackHeight + settings.startHeight;
      uint8_t xCoord = 5;
      //track info display
      if(stepchild.activeTrack == track){
          xCoord = 9;
          if(settings.trackLabels)
              this->drawArrow(6+((millis()/400)%2),y1+stepchild.trackHeight/2+1,2,ARROW_RIGHT,true);
      }
      if(settings.trackLabels){
          //printing note names
          if(stepchild.pitchesOrNumbers){
              this->printTrackPitch(xCoord, y1+stepchild.trackHeight/2-2,track,false,settings.drawTrackChannel,SSD1306_WHITE);
          }
          //just printing pitch numbers
          else{
              stepchild.display.setCursor(xCoord,y1+2);
              stepchild.display.print(stepchild.trackData[track].pitch);
          }
      }
      //if you're drawing selected tracks highlight
      if(settings.trackSelection){
        //if this track isn't selected, shade it
        if(!stepchild.trackData[track].isSelected()){
          stepchild.display.fillRect(stepchild.TRACK_LABEL_WIDTH,y1,stepchild.SCREEN_WIDTH-stepchild.TRACK_LABEL_WIDTH,stepchild.trackHeight,0);
          this->shadeArea(stepchild.TRACK_LABEL_WIDTH,y1,stepchild.SCREEN_WIDTH-stepchild.TRACK_LABEL_WIDTH,stepchild.trackHeight,3);
          continue;
        }
        else{
          stepchild.display.fillRect(0,y1+1,stepchild.TRACK_LABEL_WIDTH,stepchild.trackHeight,2);
        }
      }
      //if the track is muted, just hatch it out (don't draw any notes)
      //if it's solo'd and muted, draw it normal (solo overrules mute)
      else if(stepchild.trackData[track].isMuted() && !stepchild.trackData[track].isSolo()){
          this->shadeArea(stepchild.TRACK_LABEL_WIDTH,y1,stepchild.SCREEN_WIDTH-stepchild.TRACK_LABEL_WIDTH,stepchild.trackHeight,9);
          continue;
      }
      else if(settings.drawSteps){
          //Check to see if you only want to render the region in the active loop, and shade everything else!
          for (uint16_t step = settings.shadeOutsideLoop?stepchild.loopData[stepchild.activeLoop].start:settings.start; step < (settings.shadeOutsideLoop?stepchild.loopData[stepchild.activeLoop].end:settings.end); step++) {
              uint16_t id = stepchild.lookupTable[track][step];
              //drawing note
              if (id != 0){
                Note n = stepchild.noteData[track][id];
                drawNote(n,track,settings);
                step = n.endPos;//skip to the end of the note
              }
          }
      }
  }
  //all the top icons/tooltips
  if(settings.topLabels){
    drawTopIcons(settings);
  }
  //drawing big or small pram in the corner
  if(settings.drawPram){
    drawPramIcon(settings);
  }
  //playhead/rechead
  if(stepchild.playing() && stepchild.isInView(stepchild.playheadPos))
      stepchild.display.drawRoundRect(stepchild.TRACK_LABEL_WIDTH+(stepchild.playheadPos-settings.start)*stepchild.viewScale,settings.startHeight,3, stepchild.SCREEN_HEIGHT-settings.startHeight, 3, SSD1306_WHITE);
  if(stepchild.recording() && stepchild.isInView(stepchild.recheadPos))
      stepchild.display.drawRoundRect(stepchild.TRACK_LABEL_WIDTH+(stepchild.recheadPos-settings.start)*stepchild.viewScale,settings.startHeight,3, stepchild.SCREEN_HEIGHT-settings.startHeight, 3, SSD1306_WHITE);

  int cursorX = stepchild.TRACK_LABEL_WIDTH+int((stepchild.cursorPos-settings.start)*stepchild.viewScale)-8;
  if(!stepchild.playing() && !stepchild.recording()){
      cursorX = 32;
  }
  else{
      //making sure it doesn't print over the subdiv info
      cursorX = 42;
  }


  //it's ok to call this in here bc the LB checks to make sure it doesn't redundantly write
  if(settings.stepSequencerLEDs)
    stepchild.displayMainSequenceLEDs();
}

void StepchildGraphics::drawSeq(){
  SequenceRenderSettings settings;
  drawSeq(settings);
}

void StepchildGraphics::drawNotePreviewOutline(Note& note, uint8_t track, NoteCoords noteCoords, SequenceRenderSettings& settings){
  if(noteCoords.y1>=stepchild.SCREEN_HEIGHT)
    return;
  if(noteCoords.x1>=stepchild.SCREEN_WIDTH)
    return;
  //if the noteCoords.length is less than 3, don't worry about shading it
  if(noteCoords.length<3){
    stepchild.display.fillRect(noteCoords.x1, noteCoords.y1+1, noteCoords.length+2, stepchild.trackHeight-2, SSD1306_WHITE);
  }
  else{
    // this->drawDottedRect(noteCoords.x1,noteCoords.y1,noteCoords.length,stepchild.trackHeight-2,2);
    this->shadeArea(noteCoords.x1,noteCoords.y1+1,noteCoords.length,stepchild.trackHeight-2,2);
  }
}

void StepchildGraphics::drawNotePreviewOutline(Note& note, uint8_t track, SequenceRenderSettings& settings){
  drawNotePreviewOutline(note, track, getNoteScreenCoords(note,track,settings), settings);
}

void StepchildGraphics::drawNote(Note& note, uint8_t track, NoteCoords noteCoords, SequenceRenderSettings& settings, bool renderSuperPosTooltip){
  
  //if the note is currently superpositioned, draw it where it should be, but not if it's out of view
  if(note.isSuperpositioned()){
    noteCoords.offsetY(stepchild.trackData[track].pitch - note.superposition.pitch);
  }
  if(noteCoords.y1>=stepchild.SCREEN_HEIGHT)
    return;
  if(noteCoords.x1>=stepchild.SCREEN_WIDTH)
    return;
  //if it's not actively in superposition, BUT it has one and the cursor is over it, draw a rounded rect behind it
  if(!note.isSuperpositioned() && note.superposition.pitch != 255 && stepchild.cursorPos<note.endPos && stepchild.cursorPos >= note.startPos && stepchild.activeTrack == track  && renderSuperPosTooltip){
    int8_t offset = note.superposition.pitch-stepchild.trackData[track].pitch;
    if(stepchild.cursorPos == note.startPos){
        offset += (millis()/100)%2;
    }
    this->fillRectWithMissingCorners(noteCoords.x1+((abs(offset)>5)?5:offset), noteCoords.y1-offset, noteCoords.length, stepchild.trackHeight, SSD1306_BLACK);
    this->drawRectWithMissingCorners(noteCoords.x1+((abs(offset)>5)?5:offset), noteCoords.y1-offset, noteCoords.length, stepchild.trackHeight, SSD1306_WHITE);
  }
  uint8_t shade = settings.displayingVel?getVelShade(note.velocity):getChanceShade(note.chance);
  //if the noteCoords.length is less than 3, don't worry about shading it
  if(noteCoords.length<3){
    stepchild.display.fillRect(noteCoords.x1, noteCoords.y1+1, noteCoords.length+2, stepchild.trackHeight-2, SSD1306_WHITE);
  }
  else{
    if(note.isMuted()){
      if(noteCoords.length > 7){
        this->fillRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_BLACK);
        this->drawRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_WHITE);
      }
      else{
        stepchild.display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_BLACK);
        stepchild.display.drawRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_WHITE);
      }
      stepchild.display.drawLine(noteCoords.x1+1,noteCoords.y1+1, noteCoords.x1+noteCoords.length-1, noteCoords.y1+stepchild.trackHeight-2,SSD1306_WHITE);
      stepchild.display.drawLine(noteCoords.x1+1,noteCoords.y1+stepchild.trackHeight-2,noteCoords.x1+noteCoords.length-1,noteCoords.y1+1,SSD1306_WHITE);
      // stepchild.display.drawFastVLine(noteCoords.x1+noteCoords.length,noteCoords.y1+1,stepchild.trackHeight-2,SSD1306_BLACK);
    }
    else if(note.isSuperpositioned()){
      this->fillRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_BLACK);
      this->drawRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, stepchild.trackHeight-2, SSD1306_WHITE);
    }
    else{
      //blink notes while they're playing
      if(note.isPlaying()){
        shade = 0;
      }
      //blink notes that the cursor is on
      if(note.startPos <= stepchild.cursorPos && note.endPos > stepchild.cursorPos && track == stepchild.activeTrack){
        if((millis()/400)%2){
          shade = 2;
        }
        drawNoteBracket(noteCoords, true);

      }
      drawNoteSprite(noteCoords,shade);
      //line at the end, if there's something at the end
      if(stepchild.lookupTable[track][note.endPos] != 0)
        stepchild.display.drawFastVLine(noteCoords.x1+noteCoords.length,noteCoords.y1+1,stepchild.trackHeight-2,SSD1306_BLACK);
    }
    if(note.isSelected()){
      stepchild.display.drawRect(noteCoords.x1,noteCoords.y1+1,noteCoords.length,stepchild.trackHeight-2,SSD1306_BLACK);
      stepchild.display.drawRect(noteCoords.x1+2,noteCoords.y1+2,noteCoords.length-3,stepchild.trackHeight-4,SSD1306_WHITE);
      stepchild.display.drawRect(noteCoords.x1,noteCoords.y1,noteCoords.length+1,stepchild.trackHeight,SSD1306_WHITE);
      stepchild.display.drawRect(noteCoords.x1+1,noteCoords.y1+1,noteCoords.length-1,stepchild.trackHeight-2,SSD1306_BLACK);
    }
  }
}
void StepchildGraphics::drawNote(Note& note, uint8_t track, NoteCoords noteCoords, SequenceRenderSettings& settings){
  drawNote(note, track, getNoteScreenCoords(note,track,settings), settings, true);
}
void StepchildGraphics::drawNote(Note& note, uint8_t track, SequenceRenderSettings& settings){
  drawNote(note, track, getNoteScreenCoords(note,track,settings), settings);
}

void StepchildGraphics::drawPramIcon(SequenceRenderSettings& settings){
  if(settings.shrinkTopDisplay){
      this->drawtinyPram_bmp(stepchild.liveLoop.active?4:5,0);
  }
  else{
      this->drawBigPram(stepchild.liveLoop.active?4:5,0);
  }
  //music symbol while receiving notes
  if(stepchild.isReceivingOrSending()){
    stepchild.display.drawChar(settings.shrinkTopDisplay?13:19+(stepchild.liveLoop.active?0:1),(millis()/200)%2,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  }
  if(stepchild.liveLoop.active){
    if(settings.shrinkTopDisplay){
      stepchild.display.fillCircle(20,3,2,1);
    }
    else{
      stepchild.display.fillCircle(21,11,2,1);
    }
  }
}

NoteCoords StepchildGraphics::getNoteScreenCoords(Note& note, uint8_t track, SequenceRenderSettings& settings){
  NoteCoords nCoords;
  if(note.startPos>settings.start){
    nCoords.length = (note.endPos - note.startPos)*stepchild.viewScale;
    nCoords.x1 = stepchild.TRACK_LABEL_WIDTH+int16_t((note.startPos-settings.start)*stepchild.viewScale);
  }
  else{
    nCoords.length = (note.endPos - settings.start)*stepchild.viewScale+1;
    nCoords.x1 = stepchild.TRACK_LABEL_WIDTH-1;
  }
  nCoords.y1 = (track-stepchild.startTrack) * stepchild.trackHeight + settings.startHeight;
  nCoords.y2 = nCoords.y1+stepchild.trackHeight-2;
  return nCoords;
}

NoteCoords StepchildGraphics::getNoteScreenCoords(Note& note, uint8_t track){
  SequenceRenderSettings settings;
  return getNoteScreenCoords(note,track,settings);
}

void StepchildGraphics::drawNoteSprite(NoteCoords& noteCoords, uint8_t shade){
  if(noteCoords.y1>=stepchild.SCREEN_HEIGHT)
    return;
  if(noteCoords.x1>=stepchild.SCREEN_WIDTH)
    return;
  if(shade > 1){//so it does this faster
    if(noteCoords.length > 7)
      fillRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_BLACK);//clearing out the note area
    else
      stepchild.display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_BLACK);//clearing out the note area
    for(uint8_t j = 1; j<stepchild.trackHeight-2; j++){//shading the note...
      for(uint8_t i = noteCoords.x1+1;i+j%shade<noteCoords.x1+noteCoords.length-1; i+=shade){
        stepchild.display.drawPixel(i+j%shade,noteCoords.y1+j,SSD1306_WHITE);
      }
    }
    if(noteCoords.length > 7)
      drawRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
    else
      stepchild.display.drawRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
  }
  //if it's a solid note, fill it quickly
  else if(shade == 1){
    if(noteCoords.length > 7)
      fillRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
    else
      stepchild.display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
  }
  else if(!shade){
    if(noteCoords.length > 7){
      fillRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_BLACK);//clearing out the note area
      drawRectWithMissingCorners(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
    }
    else{
      stepchild.display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_BLACK);//clearing out the note area
      stepchild.display.drawRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, noteCoords.y2 - noteCoords.y1, SSD1306_WHITE);
    }
  }
}


uint8_t StepchildGraphics::getVelShade(uint8_t vel){
  int8_t shade = 13-(vel/10);
  if(shade<=0)
    return 1;
  else
    return shade;
}

int8_t StepchildGraphics::getChanceShade(uint8_t odds){
  int8_t shade = 11-(odds/10);
  if(shade<=0)
    return 1;
  else
    return shade;
}

//draws pram, other icons (not loop points tho)
void StepchildGraphics::drawTopIcons(SequenceRenderSettings& settings){
  //note presence indicator(if notes are offscreen)
  if(stepchild.areThereMoreNotes(true)){
    uint8_t y1 = settings.shrinkTopDisplay?8:stepchild.HEADER_HEIGHT;
    if(millis()%1000>500){
      stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH-7,y1+3,stepchild.TRACK_LABEL_WIDTH-3,y1+3,stepchild.TRACK_LABEL_WIDTH-5,y1+1, SSD1306_WHITE);
    }
    else{
      stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH-7,y1+2,stepchild.TRACK_LABEL_WIDTH-3,y1+2,stepchild.TRACK_LABEL_WIDTH-5,y1, SSD1306_WHITE);
    }
  }
  if(stepchild.areThereMoreNotes(false)){
    if(millis()%1000>500){
      stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH-7,stepchild.SCREEN_HEIGHT-5,stepchild.TRACK_LABEL_WIDTH-3,stepchild.SCREEN_HEIGHT-5,stepchild.TRACK_LABEL_WIDTH-5,stepchild.SCREEN_HEIGHT-3, SSD1306_WHITE);
    }
    else{
      stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH-7,stepchild.SCREEN_HEIGHT-4,stepchild.TRACK_LABEL_WIDTH-3,stepchild.SCREEN_HEIGHT-4,stepchild.TRACK_LABEL_WIDTH-5,stepchild.SCREEN_HEIGHT-2, SSD1306_WHITE);
    }
  }
  uint8_t x1 = 32;
  //rec/play icon
  if(stepchild.recording()){
    if(stepchild.clock.clockSource == EXTERNAL_CLOCK && !stepchild.clock.receivedClockMessage){
      if(stepchild.idlingUntilNoteReceived){
        if(millis()%1000>500){
          stepchild.display.drawCircle(stepchild.TRACK_LABEL_WIDTH+3,3,3,SSD1306_WHITE);
        }
      }
      else
        stepchild.display.drawCircle(stepchild.TRACK_LABEL_WIDTH+3,3,3,SSD1306_WHITE);
    }
    else if((stepchild.clock.clockSource == EXTERNAL_CLOCK && stepchild.clock.receivedClockMessage) || stepchild.clock.clockSource == INTERNAL_CLOCK){
      if(stepchild.idlingUntilNoteReceived){
        if(millis()%1000>500){
          stepchild.display.fillCircle(stepchild.TRACK_LABEL_WIDTH+3,3,3,SSD1306_WHITE);
        }
      }
      else
        stepchild.display.fillCircle(stepchild.TRACK_LABEL_WIDTH+3,3,3,SSD1306_WHITE);
    }
    x1+=9;
    switch(stepchild.recMode){
      //if one-shot rec
      case ONESHOT:
        printSmall(x1,1,"1",1);
        x1+=4;
        if((millis()/10)%100>50)
          stepchild.display.drawBitmap(x1,1,caution_bmp,3,5,SSD1306_WHITE);
        x1+=4;
        break;
      //if continuous recording
      case CURRENT_LOOP:
        stepchild.display.drawBitmap(x1,1,continuous_bmp,9,5,SSD1306_WHITE);
        x1+=10;
        break;
      case LOOP_SEQUENCE:
        break;
    }
    if(stepchild.overwriteRecording){
      stepchild.display.drawBitmap(x1,0,((millis()/10)%100>50)?overwrite_1:overwrite_2,7,7,SSD1306_WHITE);
      x1+=8;
    }
  }
  else if(stepchild.playing()){
    this->drawPlayIcon(stepchild.TRACK_LABEL_WIDTH+((millis()/200)%2)+1,0);
    x1 += 10;
  }
  //draw a caution sign if the queue is overloaded
  if(stepchild.midi.queueIsFull){
    stepchild.display.drawBitmap(x1,1,caution_bmp,3,5,SSD1306_WHITE);
    x1+=4;
  }
  //Data track icon
  if(stepchild.autotrackData.size()>0){
    if(millis()%1600>800)
      stepchild.display.drawBitmap(x1,0,autotrack1,10,7,SSD1306_WHITE);
    else{
      stepchild.display.drawBitmap(x1,0,autotrack2,10,7,SSD1306_WHITE);
    }
    x1+=12;
  }

  //fragment gem
  if(stepchild.isFragmenting){
    this->drawTetra(x1,5,10+sin(float(millis())/float(500)),10+sin(float(millis())/float(500)),6+sin(float(millis())/float(500)),1+sin(float(millis())/float(500)),0,SSD1306_WHITE);
    x1+=8;
  }

  //velocity/chance indicator while shifting
  if(stepchild.buttons.SHIFT()){
    if(settings.displayingVel){
      if(stepchild.IDAtCursor() == 0){
        String vel = stringify(stepchild.defaultVel);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;;
      }
      else{
        String vel = stringify(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].velocity);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;
      }
    }
    else{
      if(stepchild.IDAtCursor() == 0){
        printSmall(x1,1,"c:"+stringify(stepchild.defaultChance),SSD1306_WHITE);
        x1+=20;
      }
      else{
        printSmall(x1,1,"c:"+stringify(stepchild.noteData[stepchild.activeTrack][stepchild.IDAtCursor()].chance),SSD1306_WHITE);
        x1+=20;
      }
    }
  }
  else{
    //power/battery indicator
    this->drawPower(stepchild.SCREEN_WIDTH-10,0);
  }
  if(stepchild.arpeggiator.isActive){
    stepchild.display.drawPixel(x1,3+2*sin(float(millis())/float(200)),1);
    stepchild.display.drawPixel(x1+2,3+2*sin(float(millis())/float(200)+100),1);
    stepchild.display.drawPixel(x1+4,3+2*sin(float(millis())/float(200)+200),1);
    stepchild.display.drawBitmap(x1+6,2,tiny_arp_bmp,9,3,1);
    x1+=17;
  }
  if(stepchild.buttons.SHIFT()){
    this->drawSequenceMemoryBar(stepchild.SCREEN_WIDTH-30,0,30);
  }
  else{
    //draw menu text
    printSmall(x1,1,stepchild.tooltipText,SSD1306_WHITE);
    x1+=stepchild.tooltipText.length()*4+2;
  }
}

//Start = step you're starting on, startheight is the y coord the sequence grid begins at
void StepchildGraphics::drawSeqBackground(SequenceRenderSettings& settings, uint8_t height){
  //drawing the measure bars
  for (uint16_t step = settings.start; step < settings.end; step++) {
    unsigned short int x1 = stepchild.TRACK_LABEL_WIDTH+int((step-settings.start)*stepchild.viewScale);
//    unsigned short int x2 = x1 + (step-settings.start)*stepchild.viewScale;

    //shade everything outside the loop
    if(settings.shadeOutsideLoop){
      if(step<stepchild.loopData[stepchild.activeLoop].start){
        this->shadeArea(x1,settings.startHeight,(stepchild.loopData[stepchild.activeLoop].start-step)*stepchild.viewScale,stepchild.SCREEN_HEIGHT-settings.startHeight,3);
        step = stepchild.loopData[stepchild.activeLoop].start;
        //ok, step shouldn't ever be zero in this case, since that would mean it was LESS than zero to begin
        //with. But, just for thoroughnesses sake, make sure step doesn't overflow when you subtract from it
        if(step != 0){
          step--;
        }
        continue;
      }
      else if(step>stepchild.loopData[stepchild.activeLoop].end){
        this->shadeArea(x1,settings.startHeight,(stepchild.viewEnd-stepchild.loopData[stepchild.activeLoop].end)*stepchild.viewScale,stepchild.SCREEN_HEIGHT-settings.startHeight,3);
        break;
      }
    }

    //if the last track is showing
    if(stepchild.endTrack == stepchild.trackData.size()){
      //measure bars
      if (!(step % stepchild.subDivision) && (step%96) && (stepchild.subDivision*stepchild.viewScale)>1) {
        this->drawDottedLineV(x1,settings.startHeight,height,2);
      }
      if(!(step%96)){
        this->drawDottedLineV2(x1,settings.startHeight,height,6);
      }
    }
    else{
      //measure bars
      if (!(step % stepchild.subDivision) && (step%96) && (stepchild.subDivision*stepchild.viewScale)>1) {
        this->drawDottedLineV(x1,settings.startHeight,height,2);
      }
      if(!(step%96)){
        this->drawDottedLineV2(x1,settings.startHeight,height,6);
      }
    }

    //drawing loop points/flags
    if(settings.drawLoopPoints){//check
      if(step == stepchild.loopData[stepchild.activeLoop].start){
        if(settings.drawLoopFlags){
          if(stepchild.movingLoop == MOVING_LOOP_END || stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
            stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-3-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-7-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale+4, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
            stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(stepchild.cursorPos == step){
              stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-3, stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-7, stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale+4, settings.startHeight-7,SSD1306_WHITE);
              stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
            }
            else{
              stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-1, stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-5, stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale+4, settings.startHeight-5,SSD1306_WHITE);
            }
          }
        }
        if((stepchild.movingLoop == MOVING_NO_LOOP_POINTS) || (stepchild.movingLoop != MOVING_LOOP_START && (millis()/400)%2)){
          stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale,settings.startHeight,stepchild.SCREEN_HEIGHT-settings.startHeight-(stepchild.endTrack == stepchild.trackData.size()),SSD1306_WHITE);
          stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale-1,settings.startHeight,stepchild.SCREEN_HEIGHT-settings.startHeight-(stepchild.endTrack == stepchild.trackData.size()),SSD1306_WHITE);
        }
      }
      if(step == stepchild.loopData[stepchild.activeLoop].end-1){
        if(settings.drawLoopFlags){
          if(stepchild.movingLoop == MOVING_LOOP_START || stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
            stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-3-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale-4, settings.startHeight-7-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
            stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(stepchild.cursorPos == step+1){
              stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-3, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale-4, settings.startHeight-7, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-7,SSD1306_WHITE);
              stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
            }
            else{
              stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-1, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale-4, settings.startHeight-5, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale, settings.startHeight-5,SSD1306_WHITE);
            }
          }
        }
        if((stepchild.movingLoop == MOVING_NO_LOOP_POINTS) || (stepchild.movingLoop != MOVING_LOOP_END && (millis()/400)%2)){
          stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale+1,settings.startHeight,stepchild.SCREEN_HEIGHT-settings.startHeight-(stepchild.endTrack == stepchild.trackData.size()),SSD1306_WHITE);
          stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale+2,settings.startHeight,stepchild.SCREEN_HEIGHT-settings.startHeight-(stepchild.endTrack == stepchild.trackData.size()),SSD1306_WHITE);
        }
      }
      if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
        if(step>stepchild.loopData[stepchild.activeLoop].start && step<stepchild.loopData[stepchild.activeLoop].end && settings.startHeight>8){
          stepchild.display.drawPixel(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(settings.drawLoopFlags && (step == stepchild.loopData[stepchild.activeLoop].start+(stepchild.loopData[stepchild.activeLoop].end-stepchild.loopData[stepchild.activeLoop].start)/2))
        printSmall(stepchild.TRACK_LABEL_WIDTH+(step-settings.start)*stepchild.viewScale-1,settings.startHeight-7,stringify(stepchild.activeLoop),SSD1306_WHITE);
    }
  }
}


//Shades a rectangle
void StepchildGraphics::shadeArea(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade){
    for(int j = 0; j<height; j++){
        for(int i = 0;(i+j%shade)<len; i+=shade){
        if((i+j%shade)>=0 && (i+j%shade)<stepchild.SCREEN_WIDTH){
            stepchild.display.drawPixel(x1+i+j%shade,y1+j,1);
        }
        else{
            stepchild.display.drawPixel(x1+i+j%shade,y1+j,0);
        }
        }
    }
}
void StepchildGraphics::drawDottedLineH(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t dot){
    if(x1<x2){
        for(int i = x1; i<=x2; i+=dot){
        stepchild.display.drawPixel(i,y1,SSD1306_WHITE);
        }
    }
    else if(x2<x1){
        for(int i = x2; i<=x1; i+=dot){
        stepchild.display.drawPixel(i,y1,SSD1306_WHITE);
        }
    }
}
void StepchildGraphics::drawDottedLineV(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t dot){
    if(y1<y2){
        for(int i = y1; i<=y2; i+=dot){
        stepchild.display.drawPixel(x1,i,SSD1306_WHITE);
        }
    }
    else if(y2>y1){
        for(int i = y2; i<=y1; i+=dot){
        stepchild.display.drawPixel(x1,i,SSD1306_WHITE);
        }
    }
}

//this one draws a black pixel every dot pixels
void StepchildGraphics::drawDottedLineV2(unsigned short int x1, unsigned short int y1, unsigned short int y2, unsigned short int dot){
    if(y1<y2){
        for(int i = y1; i<=y2; i++){
        if((i%dot))
            stepchild.display.drawPixel(x1,i,SSD1306_WHITE);
        }
    }
    else if(y2>y1){
        for(int i = y2; i<=y1; i++){
        if((i%dot))
            stepchild.display.drawPixel(x1,i,SSD1306_WHITE);
        }
    }
}

void StepchildGraphics::drawDottedLineDiagonal(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dot){
  	uint8_t count = 0;
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        uint8_t x2 = x0;
        x0 = y0;
        y0 = x2;
        x2 = x1;
        x1 = y1;
        y1 = x2;
    }

    if (x0 > x1) {
        uint8_t x2 = x0;
        x0 = x1;
        x1 = x2;
        x2 = y0;
        y0 = y1;
        y1 = x2;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    }
    else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if(!(count%dot)){
            if (steep) {
                stepchild.display.drawPixel(y0, x0, 2);
            }
            else {
                stepchild.display.drawPixel(x0, y0, 2);
            }
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
        count++;
    }
}

void StepchildGraphics::shadeRect(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade){
    stepchild.display.drawRect(x1, y1, len, height, SSD1306_WHITE);
    this->shadeArea(x1,y1,len,height,shade);
}

void StepchildGraphics::drawBinarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, bool state){
    const uint8_t height = 11;
    //getting the longest string so we know how long to make the box
    uint8_t maxLength;
    if(op1.length()>op2.length())
        maxLength = op1.length();
    else
        maxLength = op2.length();
    uint8_t length = (maxLength*4+3)*2;
    stepchild.display.fillRect(x1-length/2,y1-height/2,length,height,SSD1306_BLACK);
    this->drawSlider(x1-length/2,y1-height/2,length,height,state);
    printSmall(x1-length/4-op1.length()*2+1,y1-2,op1,2);
    printSmall(x1+length/4-op2.length()*2,y1-2,op2,2);
    if(title.length()>0){
        const uint8_t height2 = 9;
        uint8_t length2 = title.length()*4-countSpaces(title)*2;
        stepchild.display.fillRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_BLACK);
        stepchild.display.drawRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_WHITE);
        printSmall(x1-length2/2,y1-height2-4,title,SSD1306_WHITE);
    }
}
void StepchildGraphics::shadeLineV(int16_t xPos, int16_t yPos, int16_t len, uint8_t shade){
    for(uint8_t i = 0; i<=len; i++){
        if(!((yPos+i)%shade)){
        stepchild.display.drawPixel(xPos,yPos+i,SSD1306_WHITE);
        }
        else
        stepchild.display.drawPixel(xPos,yPos+i,SSD1306_BLACK);
    }
}

void StepchildGraphics::drawDottedRect(int8_t x1, int8_t y1, uint8_t w, uint8_t h, uint8_t dot){
    //top
    this->drawDottedLineH(x1,x1+w,y1,dot);
    //bottom
    this->drawDottedLineH(x1,x1+w,y1+h,dot);
    //left
    this->drawDottedLineV(x1,y1,y1+h,dot);
    //right
    this->drawDottedLineV(x1+w,y1,y1+h,dot);
}
void StepchildGraphics::drawRotatedRect(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, float angle, uint16_t c){
    int points[4][2] = {{-length/2,-height/2},{-length/2+length,-height/2},{-length/2+length,-height/2+height},{-length/2,-height/2+height}};
    int points2[4][2];

    //y' = -sin(angle)*x+cos(angle)*y
    //x' = cos(angle)*x+sin(angle)*y
    for(int i = 0; i<4; i++){
        points2[i][0] = cos(radians(angle))*points[i][0]+sin(radians(angle))*points[i][1];
        points2[i][1] = -sin(radians(angle))*points[i][0]+cos(radians(angle))*points[i][1];
    }
    if(points[0][0]<0)
        points[0][0] = 0;
    if(points[0][1]<0)
        points[0][1] = 0;
    if(points[1][0]<0)
        points[1][0] = 0;
    if(points[1][1]<0)
        points[1][1] = 0;
    if(points[2][0]<0)
        points[2][0] = 0;
    if(points[2][1]<0)
        points[2][1] = 0;
    if(points[3][0]<0)
        points[3][0] = 0;
    if(points[3][1]<0)
        points[3][1] = 0;

    stepchild.display.drawLine(points2[0][0]+x1, points2[0][1]+y1, points2[1][0]+x1, points2[1][1]+y1, c);
    stepchild.display.drawLine(points2[1][0]+x1, points2[1][1]+y1, points2[2][0]+x1, points2[2][1]+y1, c);
    stepchild.display.drawLine(points2[2][0]+x1, points2[2][1]+y1, points2[3][0]+x1, points2[3][1]+y1, c);
    stepchild.display.drawLine(points2[3][0]+x1, points2[3][1]+y1, points2[0][0]+x1, points2[0][1]+y1, c);
}
/*-------------------------------------------
                Fractions!
-------------------------------------------*/
//fractions are 7x11 (if it's 1 digit numerator)
void StepchildGraphics::printFraction(uint8_t x, uint8_t y, uint8_t numerator, uint8_t denominator){
    printSmall(x,y,stringify(numerator),SSD1306_WHITE);
    stepchild.display.drawLine(x+1+4*(stringify(numerator).length()-1),y+7,x+5+4*(stringify(numerator).length()-1),y+3,SSD1306_WHITE);
    printSmall(x+4+4*(stringify(numerator).length()-1),y+6,stringify(denominator),SSD1306_WHITE);
}
//feed it a number in the form "1 7/8"
void StepchildGraphics::printFraction(uint8_t x1, uint8_t y1, String fraction){
    String whole;
    String denominator = "";
    String numerator = "";
    bool foundSpaceOrSlash = false;
  	uint8_t wholeIndex = 0;
    for(uint8_t i = 0; i<fraction.length(); i++){
        if(fraction.charAt(i) == ' '){
        	foundSpaceOrSlash = true;
        	whole = fraction.substring(0,i);
        	wholeIndex = i;
        }
        if(fraction.charAt(i) == '/'){
        	foundSpaceOrSlash = true;
        	numerator = fraction.substring(wholeIndex,i);
        	denominator = fraction.substring(i+1,fraction.length());
        	break;
        }
    }
    //if you didn't find a space or a slash, treat the fraction as a wholenumber
    if(!foundSpaceOrSlash){
        whole = fraction;
    }
    stepchild.display.setCursor(x1,y1-1);
    stepchild.display.print(whole);
    //if there's no numerator, return after printing the whole number
    uint16_t num = toInt(numerator);
    uint16_t denom = toInt(denominator);
    //if there's actually a numerator and a denominator
    if(num != 0 && denom != 0)
        this->printFraction(x1+whole.length()*6, y1-3, num, denom);
}

void StepchildGraphics::printFractionCentered(uint8_t x1, uint8_t y1, String fraction){
    String whole;
    String denominator;
    String numerator;
  	uint8_t wholeIndex = 0;
    bool foundSpaceOrSlash = false;
    for(uint8_t i = 0; i<fraction.length(); i++){
        if(fraction.charAt(i) == ' '){
        whole = fraction.substring(0,i);
        wholeIndex = i;
        foundSpaceOrSlash = true;
        }
        if(fraction.charAt(i) == '/'){
        numerator = fraction.substring(wholeIndex,i);
        denominator = fraction.substring(i+1,fraction.length());
        foundSpaceOrSlash = true;
        break;
        }
    }
        //if you didn't find a space or a slash, treat the fraction as a wholenumber
    if(!foundSpaceOrSlash){
        whole = fraction;
    }
    uint16_t num = toInt(numerator);
    uint16_t denom = toInt(denominator);

    int16_t x2;
    //if there is a fractional part
    if(num != 0 && denom != 0)
        x2 = x1-whole.length()*3-numerator.length()*2-denominator.length()*2;
    //if not
    else
        x2 = x1-whole.length()*3;
    stepchild.display.setCursor(x2,y1-1);
    stepchild.display.print(whole);
    //if there's actually a numerator and a denominator
    if(num != 0 && denom != 0)
        this->printFraction(x2+whole.length()*6, y1-3, num, denom);
}

//feed it a number in the form "1 7/8"
uint8_t StepchildGraphics::printFraction_small(uint8_t x1, uint8_t y1, String fraction){
    uint8_t length = 0;
    String whole;
    String denominator;
    String numerator;
  	uint8_t wholeIndex = 0;
    bool foundSpaceOrSlash = false;
    for(uint8_t i = 0; i<fraction.length(); i++){
        if(fraction.charAt(i) == ' '){
        	whole = fraction.substring(0,i);
        	wholeIndex = i;
        	foundSpaceOrSlash = true;
        }
        if(fraction.charAt(i) == '/'){
        	numerator = fraction.substring(wholeIndex,i);
        	denominator = fraction.substring(i+1,fraction.length());
        	foundSpaceOrSlash = true;
        	break;
        }
    }
    if(!foundSpaceOrSlash){
        whole = fraction;
    }
    printSmall(x1,y1,whole,SSD1306_WHITE);

    length += (4*uint8_t(log10(toInt(whole))+1));
    uint16_t num = toInt(numerator);
    uint16_t denom = toInt(denominator);

    //if there's actually a numerator and a denominator
    if(num != 0 && denom != 0){
        this->printFraction(x1+whole.length()*4, y1-3, num, denom);
        length = length + uint8_t(log10(num)+1)*4 + uint8_t(log10(denom)+1)*4;
    }
    return length;
}

//centers the fractions
void StepchildGraphics::printFraction_small_centered(uint8_t x1, uint8_t y1, String fraction){
    uint8_t length = 0;
    String whole;
    String denominator;
    String numerator;
  	uint8_t wholeIndex = 0;
    bool foundSpaceOrSlash = false;
    for(uint8_t i = 0; i<fraction.length(); i++){
        if(fraction.charAt(i) == ' '){
        whole = fraction.substring(0,i);
        wholeIndex = i;
        foundSpaceOrSlash = true;
        }
        if(fraction.charAt(i) == '/'){
        numerator = fraction.substring(wholeIndex,i);
        denominator = fraction.substring(i+1,fraction.length());
        foundSpaceOrSlash = true;
        break;
        }
    }
    if(!foundSpaceOrSlash){
        whole = fraction;
    }
        
    uint16_t num = toInt(numerator);
    uint16_t denom = toInt(denominator);
        
    length = whole.length();
    //if there's actually a numerator and a denominator
    if(num != 0 && denom != 0){
        length += numerator.length()+denominator.length()-1;//-1 because there's a slash
        this->printFraction(x1-length*2+whole.length()*4, y1-3, num, denom);
    }
    printSmall(x1-length*2,y1,whole,SSD1306_WHITE);
}
void StepchildGraphics::drawCheckmark(int16_t x, int16_t y, uint8_t size, uint16_t c){
  stepchild.display.drawLine(x+2,y+2,x+size-3,y+size-3,c);
  stepchild.display.drawLine(x+size-3,y+size-3,x+size+3,y+size-9,c);
}
void StepchildGraphics::drawCheckbox(int x, int y, bool checked, bool selected){
    int size = 7;
    stepchild.display.fillRect(x,y,size,size,SSD1306_BLACK);
    stepchild.display.drawRect(x,y,size,size,SSD1306_WHITE);
    if(checked){
        drawCheckmark( x,  y,  size, 1);
    }
    if(selected){
        stepchild.display.drawRoundRect(x-2,y-2,size+4,size+4,3,SSD1306_WHITE);
    }
}
void StepchildGraphics::drawRadian(uint8_t h, uint8_t k, int a, int b, float angle, uint16_t c) {
    float x1 = h + a * cos(radians(angle));
    float y1;
    if (angle > 180) {
        y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
    }
    else {
        y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
    }
    stepchild.display.drawLine(x1, y1, h, k, c);
}
void StepchildGraphics::drawCircleRadian(uint8_t h, uint8_t k, int r, float angle, uint16_t c) {
    //can't % this bc it's a float
    while(angle>360){
        angle-=360;
    }
    float x1 = h + r * cos(radians(angle));
    float y1;
    if (angle > 180.0) {
        y1 = k - r * sqrt(1 - pow((x1 - h), 2) / pow(r, 2));
    }
    else {
        y1 = k + r * sqrt(1 - pow((x1 - h), 2) / pow(r, 2));
    }
    stepchild.display.drawLine(x1, y1, h, k, c);
}
void StepchildGraphics::drawPendulum(int16_t x2, int16_t y2, int8_t length, float val, uint8_t r){
    //pendulum
    int a = length;
    int h = x2;
    int k = y2;
    float x1 = h + a * cos(radians(val))/float(2.4);
    float y1 = k + a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
    stepchild.display.drawLine(x1,y1,h,k,SSD1306_WHITE);
    stepchild.display.fillCircle(x1,y1,r,SSD1306_BLACK);
    stepchild.display.drawCircle(x1,y1,r,SSD1306_WHITE);
}
//draws a pendulum where "val" is a the angle of the pendulum
void StepchildGraphics::drawPendulum(int16_t x2, int16_t y2, int8_t length, float val){
    drawPendulum(x2,y2,length,val,2);
}

void StepchildGraphics::fillEllipse(uint8_t h, uint8_t k, int a, int b,uint16_t c){
    for(int i = 0; i<360; i++){
        this->drawRadian(h,k,a,b,i,c);
    }
}
void StepchildGraphics::drawEllipse(uint8_t h, uint8_t k, int a, int b, uint8_t skip, uint16_t c) {
    //centerX = h
    //centerY = k
    //horizontal radius = a
    //vertical radius = b
    int y1;
    //if the ellipse is secretely a circle
    if (a == b) {
        stepchild.display.drawCircle(h, k, a, c);
    }
    //for every x that falls along the length of the ellipse, get a y and draw a point
    else if (a > 0 && b > 0) {
        for (int x1 = h - a; x1 <= h + a; x1++) {
            if (x1 < stepchild.SCREEN_WIDTH && x1 >= 0) {
                int root = b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
                y1 = k + root;
                if (y1 < stepchild.SCREEN_HEIGHT && !(y1%skip)) {
                    stepchild.display.drawPixel(x1, y1, c);
                }
                y1 = k - root;
                if (y1 >= 0  && !(y1%skip)) {
                    stepchild.display.drawPixel(x1, y1, c);
                }
              }
        }
    }
}
void StepchildGraphics::drawEllipse(uint8_t h, uint8_t k, int a, int b, uint16_t c) {
  drawEllipse(h,k,a,b,1,c);
}
void StepchildGraphics::drawStar(uint8_t centerX, uint8_t centerY, uint8_t r1, uint8_t r2, uint8_t points){
    uint8_t numberOfPoints = points*2;//the actual number of points (both convex and concave vertices)
    Coordinate coords[numberOfPoints];
    for(uint8_t pt = 0; pt<numberOfPoints; pt++){
        Coordinate pair;
        //if it's odd, it's a convex point
        if(!(pt%2))
        pair = getRadian(centerX, centerY, r1, r1, pt*360/numberOfPoints);
        else
        pair = getRadian(centerX, centerY, r2, r2, pt*360/numberOfPoints);
        coords[pt] = pair;
    }
    for(uint8_t pt = 0; pt<numberOfPoints; pt++){
        if(pt == numberOfPoints-1){
        stepchild.display.drawLine(coords[pt].x,coords[pt].y,coords[0].x,coords[0].y,SSD1306_WHITE);
        }
        else{
        stepchild.display.drawLine(coords[pt].x,coords[pt].y,coords[pt+1].x,coords[pt+1].y,SSD1306_WHITE);
        }
    }
}
void StepchildGraphics::printChannel(uint8_t xPos, uint8_t yPos, uint8_t channel, bool withBox){
    if(withBox){
        stepchild.display.fillRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_BLACK);
        stepchild.display.drawRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_WHITE);
    }
    printSmall(xPos,yPos,"Channel:"+stringify(channel),SSD1306_WHITE);
}
//draws a horizontal bar graph -- so far totally unused
void StepchildGraphics::drawBarGraphH(int xStart, int yStart, int thickness, int length, float progress){
    float percentage = progress*length;
    stepchild.display.fillRect(xStart,yStart,length,thickness,SSD1306_BLACK);
    stepchild.display.drawRect(xStart,yStart,length,thickness,SSD1306_WHITE);
    stepchild.display.fillRect(xStart,yStart,percentage,thickness,SSD1306_WHITE);
    stepchild.display.drawRect(xStart+1,yStart+1,length-2,thickness-2,SSD1306_BLACK);
}
void StepchildGraphics::drawBarGraphV(uint8_t xStart, uint8_t yStart, uint8_t thickness, uint8_t height, float progress){
    float percentFull = progress*height;
    stepchild.display.fillRect(xStart,yStart,thickness,height,0);
    stepchild.display.drawRect(xStart,yStart,thickness,height,1);
    stepchild.display.fillRect(xStart,yStart+height-percentFull,thickness,percentFull,1);
    stepchild.display.drawRect(xStart+1,yStart+1,thickness-2,height-2,0);
}
//Draws percentage bar showing how full the sequence is
void StepchildGraphics::drawSequenceMemoryBar(uint8_t x1, uint8_t y1, uint8_t length){
    float free = rp2040.getFreeHeap();
    float total = rp2040.getTotalHeap();
    float percentageUsed = 1.0 - free/total;
    // stepchild.display.fillRoundRect(x1-14,y1-2,length+16,9,3,0);
    // stepchild.display.drawRoundRect(x1-14,y1-2,length+16,9,3,1);
    this->drawBarGraphH(x1,y1,5,length,percentageUsed);
    printSmall(x1-12,y1,"mem",1);
}

void StepchildGraphics::drawArrow(uint8_t pointX, uint8_t pointY, uint8_t size, ArrowDirection direction, bool full){
    switch(direction){
        //right
        case ARROW_RIGHT:
        if(full)
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_WHITE);
        else{
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_BLACK);
            stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_WHITE);
        }
        break;
        //left
        case ARROW_LEFT:
        if(full)
            stepchild.display.fillTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_WHITE);
        else{
            stepchild.display.fillTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_BLACK);
            stepchild.display.drawTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_WHITE);
        }
        break;
        //up
        case ARROW_UP:
        if(full)
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_WHITE);
        else{
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_BLACK);
            stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_WHITE);
        }
        break;
        //down
        case ARROW_DOWN:
        if(full)
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_WHITE);
        else{
            stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_BLACK);
            stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_WHITE);
        }
        break;
    }
}
void StepchildGraphics::drawArrow(uint8_t pointX, uint8_t pointY, uint8_t size, ArrowDirection direction, uint8_t fillColor, uint8_t highlightColor, bool drawHighlight){
    switch(direction){
      //right
      case ARROW_RIGHT:
        stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,fillColor);
        if(drawHighlight)
          stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,highlightColor);
      break;
      //left
      case ARROW_LEFT:
        stepchild.display.fillTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,fillColor);
        if(drawHighlight)
          stepchild.display.drawTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,highlightColor);
      break;
      //up
      case ARROW_UP:
        stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, fillColor);
        if(drawHighlight)
          stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, highlightColor);
      break;
      //down
      case ARROW_DOWN:
        stepchild.display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, fillColor);
        if(drawHighlight)
          stepchild.display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, highlightColor);
      break;
  }
}


void StepchildGraphics::drawHighlightArrow(uint8_t pointX,uint8_t pointY, uint8_t size, ArrowDirection direction){
    this->drawArrow(pointX,pointY,size+2,direction,true);
    switch(direction){
        case ARROW_RIGHT:
        this->drawArrow(pointX-1,pointY,size,direction,false);
        break;
        case ARROW_LEFT:
        this->drawArrow(pointX+1,pointY,size,direction,false);
        break;
        case ARROW_UP:
        this->drawArrow(pointX,pointY+1,size,direction,false);
        break;
        case ARROW_DOWN:
        this->drawArrow(pointX,pointY-1,size,direction,false);
        break;
    }
}

void StepchildGraphics::drawBanner(uint8_t x1, uint8_t y1, String text){
  if(x1 > stepchild.SCREEN_WIDTH || y1 > stepchild.SCREEN_HEIGHT)
    return;
  stepchild.display.drawBitmap(x1-13,y1-4,bannerL_bmp,12,9,SSD1306_WHITE);
  // stepchild.display.drawBitmap(x1+text.length()*4-countSpaces(text)*2,y1,bannerR_bmp,11,9,SSD1306_WHITE);
  stepchild.display.setRotation(DISPLAY_UPSIDEDOWN);
  stepchild.display.drawBitmap(stepchild.SCREEN_WIDTH-(x1+text.length()*4-countSpaces(text)*2+countChar(text,'#')*2)-12,stepchild.SCREEN_HEIGHT-y1-9,bannerL_bmp,12,9,SSD1306_WHITE);
  stepchild.display.setRotation(DISPLAY_UPRIGHT);
  stepchild.display.fillRect(x1-1,y1-1,text.length()*4-countSpaces(text)*2+countChar(text,'#')*2+1,7,SSD1306_WHITE);
  printSmall(x1,y1,text,SSD1306_BLACK);
}

void StepchildGraphics::drawCenteredBanner(uint8_t x1, uint8_t y1, String text){
    uint8_t len = text.length()*4-countSpaces(text)*2+countChar(text,'#')*2;
    x1-=len/2;
    this->drawBanner(x1,y1,text);
}

//draws a VU meter, where val is the angle of the needle
void StepchildGraphics::drawVU(int8_t x1, int8_t y1, float val){
    stepchild.display.drawBitmap(x1,y1,VUmeter_bmp,19,14,SSD1306_WHITE);
    float angle = radians(5)+(PI-radians(5))*val;
    int8_t pY = 12*sin(angle);
    int8_t pX = 12*cos(angle);
    stepchild.display.drawLine(x1+9,y1+12,x1+10+pX,y1+12-pY,SSD1306_BLACK);
    stepchild.display.drawRect(x1,y1,19,14,SSD1306_WHITE);
}
void StepchildGraphics::drawLabel_outline(uint8_t x1, uint8_t y1, String text){
    stepchild.display.fillRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-2,text.length()*4-countSpaces(text)*2+5,9,3,0);
    stepchild.display.drawRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-2,text.length()*4-countSpaces(text)*2+5,9,3,1);
    printSmall(x1-text.length()*2+countSpaces(text),y1,text,2);
}
void StepchildGraphics::drawLabel(uint8_t x1, uint8_t y1, String text, bool wOrB){
    if(wOrB){
        stepchild.display.fillRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-1,text.length()*4-countSpaces(text)*2+5,7,3,wOrB == true ? 1:0 );
        printSmall(x1-text.length()*2+countSpaces(text),y1,text,2);
    }
    else{
        this->drawLabel_outline(x1,y1,text);
    }
}
void StepchildGraphics::drawOutlinedButton(uint8_t x, uint8_t y, String t, bool wOrB){
  if(wOrB){
    stepchild.display.fillRoundRect(x,y,getSmallTextLength(t)+4,9,3,1);
  }
  else{
    stepchild.display.drawRoundRect(x,y,getSmallTextLength(t)+4,9,3,1);
  }
  printSmall(x+2,y+2,t,!wOrB);
}
void StepchildGraphics::drawButton(uint8_t x, uint8_t y, String t, bool wOrB){
  if(wOrB){
    stepchild.display.fillRoundRect(x,y,getSmallTextLength(t)+3,7,3,1);
  }
  printSmall(x+2,y+1,t,!wOrB);
}
//draws a multi-option slider
void StepchildGraphics::drawSlider(uint8_t x, uint8_t y, vector<String> &options, uint8_t which){
  uint8_t textLength = 0;
  uint8_t lengthToHighlight = 0;
  for(uint8_t i = 0; i<options.size(); i++){
    if(i == which)
      lengthToHighlight = textLength;
    textLength += getSmallTextLength(options[i])+3;
  }
  stepchild.display.fillRoundRect(x,y,textLength,9,3,0);
  stepchild.display.drawRoundRect(x,y,textLength,9,3,1);
  stepchild.display.fillRoundRect(x+lengthToHighlight,y,getSmallTextLength(options[which])+4,9,3,1);
  textLength = 0;
  for(uint8_t i = 0; i<options.size(); i++){
    printSmall(x+2+textLength,y+2,options[i],2);
    textLength += getSmallTextLength(options[i])+3;
  }
}

void StepchildGraphics::drawInputIcon(uint8_t x, uint8_t y, String t){
  stepchild.display.fillCircle(x+1,y+2,3,1);
  printSmall(x,y,t,0);
}


//Draws a yes/no slider
void StepchildGraphics::drawSlider(uint8_t x1, uint8_t y1, uint8_t w, uint8_t h, bool state){
    stepchild.display.fillRect(x1,y1,w,h,0);
    stepchild.display.drawRect(x1,y1,w,h,SSD1306_WHITE);
    if(state){
        stepchild.display.fillRect(x1+w/2,y1+2,w/2-2,h-4,SSD1306_WHITE);
    }
    else{
        stepchild.display.fillRect(x1+2,y1+2,w/2-2,h-4,SSD1306_WHITE);
    }
}
//draws a yes/no slider w/two text labels
void StepchildGraphics::drawSlider(uint8_t x1, uint8_t y1, String a, String b, bool state){
    uint8_t length = a.length()*4+b.length()*4+9;
    //if length is odd, add 1
    length+=(length%2)?1:0;
    this->drawSlider(x1,y1,length,11,state);
    printSmall_centered(x1+length/4+2,y1+3,a,2);
    printSmall_centered(x1+3*length/4,y1+3,b,2);
}
void StepchildGraphics::drawFullKeyBed(uint8_t y1, vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
    //white keys
    const uint8_t keyWidth = 5;
    const uint8_t wKeyHeight = 13;
    const uint8_t wKeyPattern[21] = {0, 2, 4, 5, 7,9 ,11,
                                    12,14,16,17,19,21,23,
                                    24,26,28,29,31,33,35};
    //black keys
    const uint8_t bKeyHeight = 8;
    const uint8_t bKeyPattern[15] = {1, 3, 6, 8,10,
                                    13,15,18,20,22,
                                    25,27,30,32,34};
    String text = stepchild.pitchToString(activeKey,false,true);
    //these 'patterns' help reference the actual note value from the iterator i'm using to draw them
    //the reason i'm doing this in realtime, not with a bitmap, is so i can animate
    //keypresses and (maybe) even change which keys are displayed at all    
    //first draw white keys
    for(uint8_t i = 0; i<21; i++){
        //pixel indicating it's in the chord
        if(isInVector(wKeyPattern[i]+12*octave,pressList))
            stepchild.display.drawPixel(i*(keyWidth+1)+2,y1+15,SSD1306_WHITE);
        //draw cursor indicator
        if(wKeyPattern[i]+12*octave == activeKey){
            this->drawArrow(i*(keyWidth+1)+2,y1+17+((millis()/200)%2),3,ARROW_UP,true);
            printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
        }

        //if there's no mask, or if the key is in the mask
        if(mask.size() == 0 || isInVector(wKeyPattern[i]+12*octave,mask)){
        //if it's pressed, draw it blinking
            if(isInVector(wKeyPattern[i]+12*octave,pressList)){
                if(millis()%800>400)
                stepchild.display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
                else
                stepchild.display.fillRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
                //if it's highlighted
                if(wKeyPattern[i]+12*octave == activeKey)
                stepchild.display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
            }
            else{
                //if it's highlighted
                if(wKeyPattern[i]+12*octave == activeKey)
                stepchild.display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
                else
                stepchild.display.fillRect(i*(keyWidth+1),y1,keyWidth,wKeyHeight,SSD1306_WHITE);
            }
        }
    }
    //then draw black keys
    uint8_t xPos = 3;
    for(uint8_t i = 0; i<15; i++){
        //pixel indicating it's in the chord
        if(isInVector(bKeyPattern[i]+12*octave,pressList))
            stepchild.display.drawPixel(xPos+2,y1+15,SSD1306_WHITE);
        //draw cursor indicator
        if(bKeyPattern[i]+12*octave == activeKey){
            this->drawArrow(xPos+2,y1+17+((millis()/200)%2),3,ARROW_UP,false);
            printSmall(xPos+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
        }

        if(mask.size() == 0 || isInVector(bKeyPattern[i]+12*octave,mask)){
            if(isInVector(bKeyPattern[i]+12*octave,pressList) || bKeyPattern[i]+12*octave == activeKey){
                stepchild.display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
                //if it's pressed
                if(isInVector(bKeyPattern[i]+12*octave,pressList)){
                    if(millis()%800>400){
                        stepchild.display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
                    }
                }
                else
                    stepchild.display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
                stepchild.display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
            }
            else{
                stepchild.display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
                stepchild.display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
            }
        }
        //if it's a D# or a Bb, you're about to jump
        if(abs(bKeyPattern[i]%12) == 3 || abs(bKeyPattern[i]%12) == 10)
            xPos+= 2*(keyWidth+1); 
        //if it's not, just increment like normal
        else
            xPos+= 1+keyWidth;
    }
}

void StepchildGraphics::drawFullKeyBed(vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
    this->drawFullKeyBed(16,pressList,mask,activeKey,octave);
}
//draws the pram icon, bouncing to the beat of the sequence
void StepchildGraphics::drawPram(uint8_t x1, uint8_t y1){
    if(stepchild.clock.onBeat(2,30))
        stepchild.display.drawBitmap(x1,y1,carriage_2_bmp,14,15,SSD1306_WHITE);
        // stepchild.display.drawBitmap(x1,y1+1,carriage_bmp,14,15,SSD1306_WHITE);
    else
        stepchild.display.drawBitmap(x1,y1,carriage_bmp,14,15,SSD1306_WHITE);
}
void StepchildGraphics::drawBigPram(uint8_t x, uint8_t y){
    if(!stepchild.playing() && !stepchild.recording()){
        this->drawPram(x,y);
    }
    else if(stepchild.playing() || stepchild.recording()){
        //if the playhead/rechead is on a subdiv, bounce the pram 
        stepchild.display.drawBitmap(x,y,((stepchild.playheadPos%24/12)%2)?carriage_bmp:carriage_2_bmp,14,15,SSD1306_WHITE);
    }
}
void StepchildGraphics::drawtinyPram_bmp(uint8_t x, uint8_t y){
    if(!stepchild.playing() && !stepchild.recording()){
        if(stepchild.clock.onBeat(2,30))
            stepchild.display.drawBitmap(x,y+1,tinyPram_bmp,7,7,SSD1306_WHITE);
        else
            stepchild.display.drawBitmap(x,y,tinyPram_bmp,7,7,SSD1306_WHITE);
    }
    //pram bounces faster
    else if(stepchild.playing() || stepchild.recording()){
        //if the playhead/rechead is on a subdiv, bounce the pram
        stepchild.display.drawBitmap(x,y+!((stepchild.playheadPos%24/12)%2),tinyPram_bmp,7,7,SSD1306_WHITE);
    }
}
//draws a play icon shaded according to the sequence clock state
void StepchildGraphics::drawPlayIcon(int8_t x1, int8_t y1){
    if(stepchild.clock.clockSource == EXTERNAL_CLOCK && !stepchild.clock.receivedClockMessage){
        stepchild.display.drawTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
    }
    else{
        stepchild.display.fillTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
    }
}
//draws the power icon corresponding to USB/batt and batt level
void StepchildGraphics::drawPower(uint8_t x1, uint8_t y1){
    //check if USB is plugged in
    if(isConnectedToUSBPower()){
        stepchild.display.drawBitmap(x1,y1+1,tiny_usb,10,4,SSD1306_WHITE);
    }
    else{
        stepchild.display.drawBitmap(x1,y1,batt_bmp,10,7,SSD1306_WHITE);
        float batt = getBattLevel();
        //for printing batt level to the screen
        //input ranges from ~1.8 (lowest the Pico can run on) to 3.6v (with 3AA's @ 1.2v)
        //so the range is 1.8, thus u gotta do 6 increments of 0.3, 1.8 --> 
        if(batt<=1.8){
        }
        else if(batt<2.1){
            stepchild.display.fillRect(x1+2,y1+2,1,3,SSD1306_WHITE);
        }
        else if(batt<2.4){
            stepchild.display.fillRect(x1+2,y1+2,2,3,SSD1306_WHITE);
        }
        else if(batt<2.7){
            stepchild.display.fillRect(x1+2,y1+2,3,3,SSD1306_WHITE);
        }
        else if(batt<3.0){
            stepchild.display.fillRect(x1+2,y1+2,4,3,SSD1306_WHITE);
        }
        else if(batt<3.3){
            stepchild.display.fillRect(x1+2,y1+2,5,3,SSD1306_WHITE);
        }
        else{
            stepchild.display.fillRect(x1+2,y1+2,6,3,SSD1306_WHITE);
        }
    }
}

//drawing rects w/missing corners (instead of calling roundRect() with radius 2)
void StepchildGraphics::drawRectWithMissingCorners(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t c){
    stepchild.display.drawFastHLine(x+1,y,width-2,c);
    stepchild.display.drawFastHLine(x+1,y+height-1,width-2,c);
    stepchild.display.drawFastVLine(x,y+1,height-2,c);
    stepchild.display.drawFastVLine(x+width-1,y+1,height-2,c);
}
void StepchildGraphics::fillRectWithMissingCorners(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t c){
    stepchild.display.fillRect(x+1,y+1,width-2,height-2,c);
    drawRectWithMissingCorners(x,y,width,height,c);
}

//gen midi numbers taken from https://usermanuals.finalemusic.com/SongWriter2012Win/Content/PercussionMaps.htm
void StepchildGraphics::drawDrumIcon(uint8_t x1, uint8_t y1, uint8_t note){    
    int8_t which = -1;
    switch(note){
        //"bass drum" => 808
        case 35:
        which = 0;
        break;
        //"kick"
        case 36:
        which = 1;
        break;
        //cowbell
        case 34:
        case 56:
        which = 2;
        break;
        //clap
        case 39:
        which = 3;
        break;
        //crash cymbal
        case 49:
        which = 4;
        break;
        //"laser" ==> gun (sfx)
        case 27:
        case 28:
        case 29:
        case 30:
        which = 5;
        break;
        //closing hat pedal
        case 44:
        which = 6;
        break;
        //open hat
        case 46:
        which = 7;
        break;
        //closed hat
        case 42:
        which = 8;
        break;
        //rim
        case 31:
        case 32:
        case 37:
        which = 9;
        break;
        //shaker
        case 82:
        which = 10;
        break;
        //snare
        case 38:
        case 40:
        which = 11;
        break;
        //tomL
        case 45:
        which = 12;
        break;
        //tomM
        case 47:
        case 48:
        which = 13;
        break;
        //tomS
        case 50:
        which = 14;
        break;
        //triangle
        case 80:
        case 81:
        which = 15;
        break;
    }
    if(which != -1){
        stepchild.display.drawBitmap(x1,y1,drum_icons[which],16,8,1,0);
    }
}

void StepchildGraphics::fillSquareVertically(uint8_t x0, uint8_t y0, uint8_t width, uint8_t fillAmount){
  stepchild.display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  uint8_t maxLine = float(fillAmount)/float(100)*(width-4);
  for(uint8_t line = 0; line<maxLine; line++){
    stepchild.display.drawFastHLine(x0+2,y0+width-3-line,width-4,1);
  }
}
//fill amount is a percent out of maxFill
void StepchildGraphics::fillSquareDiagonally(uint8_t x0, uint8_t y0, uint8_t width,uint8_t fillAmount, uint8_t maxFill){
  stepchild.display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  // uint8_t maxLine = float(fillAmount)/float(maxFill)*width*sqrt(2);
  uint8_t maxLine = float(fillAmount)/float(maxFill)*width*1.4;
  for(uint8_t line = 0; line<maxLine; line++){
    //bottom right
    int8_t x1 = x0+2+line;
    int8_t y1 = y0+width-3;
    if(x1>x0+width-2)
      x1 = x0+width-2;

    //top left
    int8_t x2 = x0+2;
    int8_t y2 = y0+width-3-line;

    if(x1>=x0+width-2){
      x1 = x0+width-3;
      y1 = y0+width-3-(line-width+5);
    }
    if(y2<y0+2){
      y2 = y0+2;
      x2 = x0+2+(line-width+5);
    }
    if(y1<y0+2)
      y1 = y0+2;
    if(x2>x0+width-2)
      x2 = x0+width-2;
    stepchild.display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
  }
}

//overload to default to /100
void StepchildGraphics::fillSquareDiagonally(uint8_t x0, uint8_t y0, uint8_t width,uint8_t fillAmount){
  this->fillSquareDiagonally(x0,y0,width,fillAmount,100);
}


//prints pitch with a small # and either a large or small Octave number
void StepchildGraphics::printTrackPitch(uint8_t xCoord, uint8_t yCoord, uint8_t trackID,bool bigOct, bool channel, uint16_t c){
  String s = stepchild.trackData[trackID].getPitchAndOctave()+stringify(stepchild.trackData[trackID].channel)+((stepchild.trackData[trackID].noteLastSent != 255)?"$":"");
  uint8_t offset = printPitch(xCoord, yCoord, s, bigOct, channel, c);
  offset+=4;
  //if you want to show the track channel
  if(stepchild.buttons.SHIFT() || (channel)){
    String sx = ":";
    sx += stringify(stepchild.trackData[trackID].channel);
    // if(stepchild.trackData[trackID].isLatched()){
    //   sx += "<";
    // }
    // stepchild.display.drawBitmap(xCoord+offset+2,yCoord,ch_tiny,6,3,1);
    printSmall(xCoord+offset,yCoord,sx,1);
    offset+=sx.length()*4;
  }
  //if you want to show the track "primed" status for recording
  if(stepchild.recording() && stepchild.trackData[trackID].isPrimed()){
    if((millis()+trackID*10)%1000>500){
      stepchild.display.fillCircle(stepchild.TRACK_LABEL_WIDTH-5,yCoord+1,2,1);
    }
    else{
      stepchild.display.drawCircle(stepchild.TRACK_LABEL_WIDTH-5,yCoord+1,2,1);
    }
  }
}

//prints a formatted pitch and returns the pixel length of the printed pitch
uint8_t StepchildGraphics::printPitch(uint8_t xCoord, uint8_t yCoord, String pitch, bool bigOct, bool channel, uint16_t c){
  uint8_t offset = 0;
  stepchild.display.setCursor(xCoord,yCoord);
  stepchild.display.print(pitch.charAt(0));
  offset+=6;
  // printChunky(xCoord,yCoord,stringify(pitch.charAt(0)),c);
  //if it's a sharp
  if(pitch.charAt(1) == '#'){
    printSmall(xCoord+offset,yCoord,pitch.charAt(1),c);
    offset+=6;
    printSmall(xCoord+offset,yCoord,pitch.charAt(2),c);
    if(pitch.charAt(2) == '-'){
      offset+=4;
      printSmall(xCoord+offset,yCoord,pitch.charAt(3),c);
    }
  }
  //if it's not a sharp
  else{
    printSmall(xCoord+offset,yCoord,pitch.charAt(1),c);
    if(pitch.charAt(1) == '-'){
      offset+=4;
      printSmall(xCoord+offset,yCoord,pitch.charAt(2),c);
    }
  }
  if(pitch.charAt(pitch.length()-1) == '$'){
    offset+=4;
    printSmall(xCoord+offset,1+yCoord+sin(millis()/50),"$",c);
  }
  return offset;
}

void StepchildGraphics::drawBox(uint8_t cornerX, uint8_t cornerY, uint8_t width, uint8_t height, uint8_t depth, int8_t xSlant, uint8_t fill){
  // if(cornerX+width>stepchild.SCREEN_WIDTH || cornerY+height>stepchild.SCREEN_HEIGHT){
  //   return;
  // }
  int16_t point[4][2] = {{cornerX,cornerY},
                         {int16_t(cornerX+width),cornerY},
                         {int16_t(cornerX+width+xSlant),int16_t(cornerY+depth)},
                         {int16_t(cornerX+xSlant),int16_t(cornerY+depth)}};
  switch(fill){
    //transparent box (wireframe)
    case 0:
      //draw top face
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);
      //draw bottom face
      stepchild.display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges
      stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      break;
    //for a solid self-occluding box
    case 1:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        stepchild.display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);

      if(xSlant>=0){
        //draw bottom face
        stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
        //draw vertical edges
        stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      else if(xSlant<0){
        //draw bottom face
        stepchild.display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        //draw vertical edges
        stepchild.display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      break;
    //for a solid,filled box
    //draw top face again and again
    case 2:
      for(int i = 0; i<height; i ++){
        stepchild.display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      break;
    //for a solid box, with black edges
    case 3:
      for(int i = 0; i<height; i ++){
        stepchild.display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);

      //draw bottom face
      stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges
      stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;

    //for a box with a white border, but no internal lines
    case 4:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        stepchild.display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        stepchild.display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face (just the back two lines)
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      //draw bottom face (just the front two lines)
      stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges (just the two edges)
      stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      break;
    //this one is obscure, but basically a solid white box with a black edge (helps it stand out over white things)
    case 5:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        stepchild.display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        stepchild.display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face (just the back two lines)
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      //draw bottom face (just the front two lines)
      stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges (just the two edges)
      stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 6:
      //draw top face
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);
      //draw bottom face
      stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;
    //transparent box (wireframe) with inverted color
    case 7:
      //draw top face
      stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],2);
      stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],2);
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      stepchild.display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,2);
      stepchild.display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,2);
      stepchild.display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,2);
      stepchild.display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,2);
      //draw vertical edges
      stepchild.display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,2);
      stepchild.display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,2);
      stepchild.display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,2);
      stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 8:
      //draw top face
      stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      stepchild.display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      stepchild.display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
  }
}

void StepchildGraphics::drawBracket(uint8_t h, uint8_t k, uint8_t height, uint8_t width, uint8_t style, uint16_t c){
  int16_t point[4][2] = 
  {{int16_t(h-width/2),int16_t(k-height)},
   {int16_t(h-width/2),k},
   {int16_t(h+width/2),k},
   {int16_t(h+width/2),int16_t(k-height)}};
  //horizontal bracket facing up
  if(style == 0){
   //left side
   stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //bottom
   stepchild.display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],c);
   //right side
    stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
  //horizontal bracket facing down
  if(style == 1){
   //left side
   stepchild.display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //top
   stepchild.display.drawLine(point[0][0],point[0][1],point[3][0],point[3][1],c);
   //right side
    stepchild.display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}
void StepchildGraphics::drawTetra(uint8_t h, uint8_t k, uint8_t height, uint8_t width, int xDepth, int yDepth, uint8_t style, uint16_t c){
  //upside down, and transparent
  if(style == 0){
    int point[4][2] =
   {{h,k+height/2},
    {h-width/2,k-height/2},
    {h+width/2,k-height/2},
    {h-xDepth,k-yDepth}};
    for(int i = 0; i<4; i++){
      while(point[i][0]<0){
        point[i][0]++;
      }
      while(point[i][1]<0){
        point[i][1]++;
      }
    }
    //draw front triangle
    stepchild.display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[2][0],point[2][1],c);
    //draw left triangle
    stepchild.display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[3][0],point[3][1],c);
    //draw right triangle
    stepchild.display.drawTriangle(point[0][0],point[0][1],point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}

void StepchildGraphics::fillAroundCircle(float x, float y, float r, uint16_t c){
  for(float i = 0; i<(x-r+10); i++){
    stepchild.display.drawCircle(x,y,r+i,c);
  }
}

void StepchildGraphics::drawMoon(int phase, bool forward){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  // phase*=scale;
  int r = stepchild.SCREEN_HEIGHT/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase<0)
    phase = 0;
  //controls whether or not the moon is waxing or waning
  if(forward){
    if(phase>0)
      stepchild.display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    this->fillEllipse(stepchild.SCREEN_WIDTH-2*r+phase/2,yPos,r,r+phase,SSD1306_BLACK);//filling earth's shadow
    if(phase>58 && phase < 88)
      this->fillEllipse(xPos,yPos,phase-58,r,SSD1306_WHITE);//filling earth's shadow
    if(phase>=88){
      stepchild.display.fillCircle(xPos,yPos,r,SSD1306_WHITE);
    }
    else{
      fillAroundCircle(63,32,r+1,SSD1306_BLACK);
    }
    stepchild.display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
  else if(!forward){
    if(phase == 0)
      stepchild.display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    else{
      if(phase<88)
        this->fillEllipse(stepchild.SCREEN_WIDTH-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
      if(phase>58 && phase<88)
        this->fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
      if(phase>=88){
        stepchild.display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
      }
      else{
        fillAroundCircle(63,31,r,SSD1306_BLACK);
        fillAroundCircle(63,32,r+1,SSD1306_BLACK);
      }
      stepchild.display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
    }
  }
}
void StepchildGraphics::drawMoon_reverse(int phase){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  //  phase*=scale;
  int r = stepchild.SCREEN_HEIGHT/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase == 0)
    stepchild.display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
  else{
    if(phase<88)
      this->fillEllipse(stepchild.SCREEN_WIDTH-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
    if(phase>58)
      this->fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
    if(phase>=88){
      stepchild.display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
    }
    else{
      fillAroundCircle(63,31,r,SSD1306_BLACK);
    }
    stepchild.display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
}



void StepchildGraphics::drawProgBar(String text, float progress){
  stepchild.display.setCursor(stepchild.SCREEN_WIDTH-text.length()*10,stepchild.SCREEN_HEIGHT/2-8);
  stepchild.display.setTextSize(2);
  stepchild.display.print(text);
  stepchild.display.setTextSize(1);
  // stepchild.display.fillRect(32,stepchild.SCREEN_HEIGHT/2+10,64,8,SSD1306_BLACK);//clearing out the progress bar
  stepchild.display.drawRect(32,stepchild.SCREEN_HEIGHT/2+10,64,8,SSD1306_WHITE);//drawing outline
  stepchild.display.fillRect(32,stepchild.SCREEN_HEIGHT/2+10,64*progress,8,SSD1306_WHITE);//filling it
  stepchild.display.display();
}

void StepchildGraphics::bootscreen(){
  uint16_t frameCount = 0;
  stepchild.display.setTextColor(SSD1306_WHITE);
  char child[5] = {'c','h','i','l','d'};
    int16_t xCoord = 0;
    int16_t yCoord = 0;

  uint8_t xOffset = 30;
  uint8_t yOffset = 15;
  WireFrame pram = makePram();
  pram.offset.y = 40;
  pram.offset.x = 64;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<64){
    stepchild.display.clearDisplay();
    stepchild.display.setTextSize(2);
    pram.render();
    //CHILD
    for(uint8_t letter = 0; letter<5; letter++){
      xCoord = 20+letter*8;
      yCoord = stepchild.SCREEN_HEIGHT-frameCount*10+letter*10;
      if(yCoord<0)
        yCoord = 0;
      yCoord+=yOffset;
      stepchild.display.setCursor(xCoord,yCoord);
      printItalic(xCoord,yCoord,child[letter],1);
    }
    //OS
    if(frameCount>20){
      stepchild.display.setTextSize(1);
      stepchild.display.setFont(&FreeSerifItalic12pt7b);
      stepchild.display.setCursor(xCoord+10,yCoord+5);
      stepchild.display.print("OS");
      stepchild.display.setFont();

      this->drawStar(xOffset+68,yOffset-8,3,7,5);
    }
    printSmall(0,58,"v0.1",1);
    stepchild.display.display();
    pram.rotate(5,1);
    // writeLEDs(uint8_t(0),uint8_t(2*frameCount/15));
    uint16_t ledData = 0b1111111111111111;
    ledData>>=(16-frameCount/8);
    stepchild.buttons.writeLEDs(ledData);
    frameCount+=4;
  }
  stepchild.buttons.turnOffLEDs();
}

void StepchildGraphics::helloChild_1(){
  stepchild.display.clearDisplay();
  stepchild.display.setCursor(50, 20);
  // stepchild.display.setTextSize(3.5);
  stepchild.display.setTextColor(SSD1306_WHITE);
  // stepchild.display.setFont(&FreeSerifBoldItalic18pt7b);
  stepchild.display.print("hello");
  stepchild.display.display();
  delay(1000);
  stepchild.display.setCursor(10, 42);
  stepchild.display.setFont();
  stepchild.display.print("child.");
  stepchild.display.invertDisplay(true);
  stepchild.display.display();
  delay(1000);
  stepchild.display.invertDisplay(false);

  stepchild.display.setTextColor(SSD1306_WHITE);
  stepchild.display.setCursor(50, 20);
  // stepchild.display.setFont(&FreeSerifBoldItalic18pt7b);
  stepchild.display.print("hello");
  stepchild.display.setCursor(10, 42);
  stepchild.display.setFont();
  stepchild.display.print("child.");

  // stepchild.display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  stepchild.display.display();
  delay(1500);
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  stepchild.display.display();
  delay(1500);
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  delay(1500);
  // for(int i = 0; i<20; i++){
  //   stepchild.display.drawBitmap(16+i*10,i*10,carriage_bmp,14,15,SSD1306_BLACK);
  //   stepchild.display.display();
  //   delay(30);
  // }
}
void StepchildGraphics::helloChild_2(){
  stepchild.display.clearDisplay();
  stepchild.display.invertDisplay(true);
  stepchild.display.display();
  delay(1500);
  stepchild.display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  stepchild.display.invertDisplay(false);
  delay(1500);
  // stepchild.display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  stepchild.display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_BLACK);
  stepchild.display.display();
  delay(1500);
  stepchild.display.fillRect(0,0,128,64,SSD1306_BLACK);
  stepchild.display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  delay(1500);
}
void StepchildGraphics::helloChild_3(){
  stepchild.display.clearDisplay();
  // stepchild.display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  stepchild.display.display();
  delay(1000);
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  stepchild.display.display();
  delay(1500);
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  delay(1500);
}
void StepchildGraphics::helloChild_4(){
  stepchild.display.clearDisplay();
  // stepchild.display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  stepchild.display.display();
  delay(1000);
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  stepchild.display.display();
  delay(1500);
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  delay(500);
  for(float i = 0; i<20; i++){
    stepchild.display.clearDisplay();
    stepchild.display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    stepchild.display.display();
    delay(10);
  }
  delay(1000);
}
void StepchildGraphics::helloChild_5(){
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  stepchild.display.display();
  delay(1000);
  for(float i = 0; i<20; i++){
    stepchild.display.clearDisplay();
    stepchild.display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    stepchild.display.display();
    delay(10);
  }
  delay(500);
}

void StepchildGraphics::drawLogo(uint8_t x1, uint8_t y1){
  stepchild.display.setTextSize(2);
  printItalic(x1,y1,"child",1);
  x1 += 42;
  y1 += 5;
  stepchild.display.setTextSize(1);
  stepchild.display.setFont(&FreeSerifItalic12pt7b);
  stepchild.display.setCursor(x1,y1);
  stepchild.display.print("OS");
  stepchild.display.setFont();
}

void StepchildGraphics::bootscreen_2(){
  float frameCount = 0;
  int16_t xCoord;
  int16_t yCoord;

  uint8_t xOffset = 30;
  uint8_t yOffset = 15;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<80){
    stepchild.display.clearDisplay();
    stepchild.display.setTextSize(2);
    xCoord = 20;
    yCoord = 20;
    printItalic(xCoord,yCoord,"child",1);
    xCoord = 52;
    stepchild.display.setTextSize(1);
    stepchild.display.setFont(&FreeSerifItalic12pt7b);
    stepchild.display.setCursor(xCoord+10,yCoord+5);
    stepchild.display.print("OS");
    stepchild.display.setFont();
    // printCursive(xCoord,yCoord,"child",1);
    //OS
    if(frameCount>40){
      this->drawStar(xOffset+68,yOffset-8,3,7,5);
      uint8_t i = frameCount-42;
      stepchild.display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    }
    else{
      stepchild.display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
    }
    // stepchild.display.clearDisplay();
    // stepchild.display.drawBitmap(0,0,stepchild_logo_bmp,60,17,1);
    stepchild.display.display();
    stepchild.buttons.writeLEDs(0b1111111111111111>>(15-uint8_t(frameCount/8)));
    frameCount+=0.4;
  }
  stepchild.buttons.turnOffLEDs();
}
void StepchildGraphics::bootscreen_3(){
  float frameCount = 0;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<80){
    stepchild.display.clearDisplay();
    stepchild.display.setTextSize(2);
    // stepchild.display.drawBitmap(10,10,child_blocks_bmp,78,23,1);
    uint8_t maxLetters = frameCount/8;
    // const uint8_t x = 0;
    // const uint8_t y = 20;
    const uint8_t x = 25;
    const uint8_t y = 15;
    if(maxLetters>=1)//c
      stepchild.display.drawBitmap(x,y+6,child_blocks_logo[0],18,18,1,0);
    if(maxLetters>=2)//h
      stepchild.display.drawBitmap(x+16,y,child_blocks_logo[1],18,17,1,0);
    if(maxLetters>=3)//i
      stepchild.display.drawBitmap(x+30,y+4,child_blocks_logo[2],19,19,1,0);
    if(maxLetters>=4)//l
      stepchild.display.drawBitmap(x+46,y,child_blocks_logo[3],18,17,1,0);
    if(maxLetters>=5)//d
      stepchild.display.drawBitmap(x+59,y+8,child_blocks_logo[4],18,16,1,0);

    if(maxLetters >= 6){
      stepchild.display.setTextSize(1);
      stepchild.display.setFont(&FreeSerifItalic12pt7b);
      // stepchild.display.setCursor(x+80,y+18);
      stepchild.display.setCursor(x+24,y+40);
      stepchild.display.print("OS");
      stepchild.display.setFont();
      drawStar(x+80,y,3,7,5);
    }


    // drawStar(x+115,y,3,7,5);

    stepchild.display.display();
    stepchild.buttons.writeLEDs(0b1111111111111111>>(15-uint8_t(frameCount/8)));
    frameCount+=1;
  }
  stepchild.buttons.turnOffLEDs();
}

void StepchildGraphics::drawOSScreen(){
  stepchild.display.clearDisplay();
  stepchild.display.drawBitmap(0,0,childOS_bmp,128,64,SSD1306_WHITE);
  stepchild.display.display();
  stepchild.buttons.clearButtons();
  stepchild.lastTime = millis();
  while(true){
    if(stepchild.itsbeen(200)&&stepchild.buttons.anyActiveInputs()){
      stepchild.buttons.clearButtons();
      stepchild.lastTime = millis();
      return;
    }
  }
}


void StepchildGraphics::drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch){
  stepchild.display.drawBitmap(x1,y1,ch_tiny,6,3,SSD1306_WHITE);
  printSmall(x1+7,y1,stringify(ch),1);
}

void StepchildGraphics::drawMoveIcon(uint8_t x1, uint8_t y1, bool anim){
  if(anim && (millis()%600) > 300){
    stepchild.display.drawBitmap(x1+1,y1+1,arrow_small_bmp2,9,9,SSD1306_WHITE);
  }
  else{
    stepchild.display.drawBitmap(x1,y1,arrow_small_bmp1,11,11,SSD1306_WHITE);
  }
}

void StepchildGraphics::drawLengthIcon(uint8_t x1, uint8_t y1, uint8_t length, uint8_t animThing, bool anim){
  uint8_t offset=0;
  if(anim){
    offset+=(millis()/200)%(animThing);
  }
  //brackets
  printSmall(x1+offset,y1,"(",SSD1306_WHITE);
  printSmall(x1+length-offset,y1,")",SSD1306_WHITE);
  //rect
  stepchild.display.fillRect(x1+offset+3,y1,length-2*offset-3,5,SSD1306_WHITE);
}

void StepchildGraphics::drawFxIcon(uint8_t x1,uint8_t y1, uint8_t w, bool anim){
  stepchild.display.drawRect(x1,y1,w,w,SSD1306_WHITE);
  printSmall(x1+2,y1+3,"fx",1);
}

//two blocks
void StepchildGraphics::drawQuantIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim){
  //dotted
  this->drawDottedRect(x1,y1+size/2-1,size/2+1,size/2+1,2);
  //full
  if(anim){
    x1-=(millis()/200)%(size/2);
    y1+=(millis()/200)%(size/2);
  }
  stepchild.display.fillRect(x1+size/2-1,y1,size/2+2,size/2+2,SSD1306_WHITE);
}

//one square, one rotated square
void StepchildGraphics::drawHumanizeIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim){
  float angle = 15;
  if(anim){
    angle = (millis()/15)%90;
  }
  this->drawDottedRect(x1,y1,size,size,2);
  this->drawRotatedRect(x1+size/2,y1+size/2,size-2,size-2,angle,SSD1306_WHITE);
}
void StepchildGraphics::drawChopIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim){
  float state = 2;
  if(anim){
    state = (millis()/200)%4+1;
  }
  stepchild.display.fillRect(x1,y1,size,size,1);
  for(uint8_t i = 0; i<size; i+=float(size)/state){
    stepchild.display.drawFastVLine(x1+i,y1,size,0);
  }
}

void StepchildGraphics::drawVelIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  if(anim){
    stepchild.display.drawRect(x1,y1,w,w,SSD1306_WHITE);
    this->shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
    stepchild.display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
  }
  else{
    stepchild.display.fillRect(x1,y1,w,w,SSD1306_WHITE);
    stepchild.display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
    printSmall(x1+w/2-1,y1+3,"v",2);
  }
}

void StepchildGraphics::drawChanceIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  if(anim){
    stepchild.display.drawRect(x1,y1,w,w,SSD1306_WHITE);
    this->shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
    stepchild.display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
  }
  else{
    stepchild.display.fillRect(x1,y1,w,w,SSD1306_WHITE);
    stepchild.display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
    printSmall(x1+w/2-1,y1+3,"%",2);
  }
}

//inverting square
void StepchildGraphics::drawReverseIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  stepchild.display.drawRect(x1,y1,w,w,1);
  stepchild.display.fillRect(x1+2,y1+2,w-4,w-4,1);
  stepchild.display.fillRect(x1+4,y1+4,w-8,w-8,0);
  if(anim){
    if(millis()%400>200){
      stepchild.display.fillRect(x1+1,y1+1,w-2,w-2,2);
    }
  }
}

//concentric circles
void StepchildGraphics::drawEchoIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  // stepchild.display.drawRect(x1,y1,w,w,1);
  if(!anim){
    stepchild.display.drawCircle(x1+w/2,y1+w/2,w-6,1);
    // stepchild.display.drawCircle(x1+w/2,y1+w/2,w-7,1);
    stepchild.display.drawCircle(x1+w/2,y1+w/2,w-8,1);
    stepchild.display.drawPixel(x1+w/2,y1+w/2,1);
  }
  else{
    uint8_t r;
    if(millis()%800>600){
      r = w - 6;
      stepchild.display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
    if(millis()%800>400){
      r = w - 8;
      stepchild.display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
    if(millis()%800>200){
      r = 0;
      stepchild.display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
  }
}

//die
void StepchildGraphics::drawRandomIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  stepchild.display.fillRect(x1,y1,w,w,1);
  stepchild.display.drawRect(x1+1,y1+1,w-2,w-2,0);
  uint8_t dots = 6;
  if(anim){
    dots = (millis()%1200)/200+1;
  }
  switch(dots){
    case 1:
      stepchild.display.drawPixel(x1+5,y1+5,0);
      break;
    case 2:
      stepchild.display.drawPixel(x1+3,y1+5,0);
      stepchild.display.drawPixel(x1+7,y1+5,0);
      break;
    case 3:
      stepchild.display.drawPixel(x1+3,y1+3,0);
      stepchild.display.drawPixel(x1+5,y1+5,0);
      stepchild.display.drawPixel(x1+7,y1+7,0);
      break;
    case 4:
      stepchild.display.drawPixel(x1+3,y1+3,0);
      stepchild.display.drawPixel(x1+3,y1+7,0);
      stepchild.display.drawPixel(x1+7,y1+3,0);
      stepchild.display.drawPixel(x1+7,y1+7,0);
      break;
    case 5:
      stepchild.display.drawPixel(x1+3,y1+3,0);
      stepchild.display.drawPixel(x1+3,y1+7,0);
      stepchild.display.drawPixel(x1+7,y1+3,0);
      stepchild.display.drawPixel(x1+7,y1+7,0);
      stepchild.display.drawPixel(x1+5,y1+5,0);
      break;
    case 6:
      stepchild.display.drawPixel(x1+3,y1+3,0);
      stepchild.display.drawPixel(x1+3,y1+5,0);
      stepchild.display.drawPixel(x1+3,y1+7,0);

      stepchild.display.drawPixel(x1+7,y1+3,0);
      stepchild.display.drawPixel(x1+7,y1+5,0);
      stepchild.display.drawPixel(x1+7,y1+7,0);
      break;
  }
}

void StepchildGraphics::drawQuickFunctionIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  drawFxIcon(x1,y1,w,anim);
}

void StepchildGraphics::drawQuantBrackets(uint8_t x1, uint8_t y1){
  const uint8_t width = 24;
  const uint8_t height = 16;
  stepchild.display.drawFastVLine(x1,y1,height,1);
  stepchild.display.drawFastVLine(x1+1,y1,height,1);
  stepchild.display.drawFastHLine(x1+2,y1,4,1);
  stepchild.display.drawFastHLine(x1+2,y1+1,4,1);
  stepchild.display.drawFastHLine(x1+2,y1+height-1,4,1);
  stepchild.display.drawFastHLine(x1+2,y1+height,4,1);

  stepchild.display.drawFastVLine(x1+width,y1,height,1);
  stepchild.display.drawFastVLine(x1+width-1,y1,height,1);
  stepchild.display.drawFastHLine(x1+width-5,y1+height,4,1);
  stepchild.display.drawFastHLine(x1+width-5,y1+1+height,4,1);
}

void StepchildGraphics::ditherBackground(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
  //dithering background
//  for(uint8_t i = x0/4; i<x1/4; i++){
//    for(uint8_t j = y0/4; j<y1/4; j++){
//      stepchild.display.drawBitmap(i*4,j*4,dither_tile_2_bmp,4,4,1);
//    }
//  }
  uint8_t gap = 3;
  
  for(uint8_t i = x0; i<x1; i++){
    if(i%gap == 0){
      stepchild.display.drawLine(i,y0,x0,y0+i-x0,1);
    }
  }
  for(uint8_t j = y0; j<y1; j++){
    if((x1-j)%gap == 0){
      stepchild.display.drawLine(x1,j,x1-y1+j,y1,1);
    }
  }
}

StepchildGraphics graphics;

//this is used for some callbacks, not a clean solution!!
void drawSeq(){
  graphics.drawSeq();
}
