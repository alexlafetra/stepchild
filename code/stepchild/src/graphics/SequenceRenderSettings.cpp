#include "graphics/SequenceRenderSettings.h"
#include "Stepchild.h"
extern Stepchild stepchild;

SequenceRenderSettings::SequenceRenderSettings(){
        shadeOutsideLoop = false;
        onlyWithinLoop = false;
        start = stepchild.viewStart;
        end = stepchild.viewEnd;
        trackLabels = true;
        topLabels = true;
        drawLoopPoints = true;
        trackSelection = false;
        shrinkTopDisplay = stepchild.shrinkTopDisplay;
        maxTracksShown = 6;
        startHeight = stepchild.HEADER_HEIGHT;
        drawLoopFlags = true;
        drawTrackChannel = stepchild.buttons.SHIFT();
        makeViewInBounds();
    }

void SequenceRenderSettings::makeViewInBounds(){
    if(end>stepchild.sequenceLength){
        end = stepchild.sequenceLength;
    }
    if(start>end){
        start = 0;
    }
}
uint16_t SequenceRenderSettings::getViewLength(){
    return end-start;
}
void SequenceRenderSettings::updateView(){
    start = stepchild.viewStart;
    end = stepchild.viewEnd;
}