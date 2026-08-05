/*
    MENUS
*/
#pragma once
#include "commonEnums.h"

extern void settingsMenu();
extern void editMenu();
extern void trackMenu();
extern void trackEditMenu();
extern void clockMenu();
extern void loopMenu();
// extern void fragmentMenu();
extern bool fileMenu();
extern void midiMenu();
extern void chordBuilder();
extern void arpMenu();
extern void PCEditor();
extern bool liveLoopMenu();
extern void instrumentMenu();
extern void autotrackMenu();
extern MenuReturnValue fxMenu();
extern uint8_t quickFX();
extern void mainMenu();
extern void webInterface();



/*
    INSTRUMENTS
*/
extern bool rain();
extern bool testSolarSystem();
extern bool rattle();
extern bool xyGrid();
extern bool knobs();
extern bool chordDJ();

/*
    FX
*/
extern bool randomMenu();
extern bool quantize();
extern bool humanize();
extern bool strum();
extern bool echo();
extern bool reverse();
extern bool warp();
extern bool getNotesToChop();