#pragma once

#include "Stepchild.h"

struct SequenceRenderSettings{
    uint16_t start;
    uint16_t end;
    uint8_t startHeight;
    uint8_t maxTracksShown = 6;
    bool onlyWithinLoop;
    bool drawLoopFlags;
    bool drawLoopPoints;
    bool drawSteps = true;
    bool trackLabels;
    bool topLabels;
    bool trackSelection = false;
    bool shadeOutsideLoop = false;
    bool shrinkTopDisplay = false;
    bool drawPram = true;
    bool drawCursor = true;
    bool displayingVel = true;
    bool drawTrackChannel = false;
    bool drawSuperposition = false;
    bool stepSequencerLEDs = true;//controls whether or not drawSeq sets the LEDs to display notes

    SequenceRenderSettings();
    void makeViewInBounds();
    uint16_t getViewLength();
    void updateView();
};