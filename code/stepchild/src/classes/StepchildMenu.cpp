#include "Stepchild.h"
#include "classes/StepchildMenu.h"

extern Stepchild stepchild;
using namespace std;

StepchildMenu::StepchildMenu(){};
void StepchildMenu::displayMenu(){};
void StepchildMenu::slideIn(SlideDirection origin,MenuSlideSpeed speed){
    if(origin == IN_FROM_RIGHT){
        //store original coords
        CoordinatePair targetCoords = coords;
        //then, offset the menu coordinates
        int16_t offset = stepchild.SCREEN_WIDTH-coords.start.x;
        coords.start.x = stepchild.SCREEN_WIDTH;
        coords.end.x = coords.start.x+offset;
        //continuously move the menu coords and display it, until it reaches original position
        while(coords.start.x>targetCoords.start.x){
            coords.end.x -= speed;
            coords.start.x -= speed;
            if(coords.start.x<targetCoords.start.x){
                coords = targetCoords;
                break;
            }
            displayMenu();
        }
        coords = targetCoords;
    }
    else if(origin == IN_FROM_BOTTOM){
        //store original coords
        CoordinatePair targetCoords = coords;
        //then, offset the menu coordinates
        int16_t offset = stepchild.SCREEN_HEIGHT-coords.start.y;
        coords.start.y += offset;
        coords.end.y += offset;
        //continuously move the menu coords and display it, until it reaches original position
        while(coords.start.y>targetCoords.start.y){
            coords.start.y-= speed;
            coords.end.y-= speed;
            if(coords.start.y<targetCoords.start.y){
                coords = targetCoords;
                break;
            }
            displayMenu();
        }
        coords = targetCoords;
    }
};
void StepchildMenu::slideOut(SlideDirection destination, MenuSlideSpeed speed){
    if(destination == OUT_FROM_RIGHT){
        CoordinatePair originalCoords = coords;
        while(coords.start.x<stepchild.SCREEN_WIDTH){
            coords.start.x+=speed;
            coords.end.x+=speed;
            displayMenu();
        }
        coords = originalCoords;
    }
    else if(destination == OUT_FROM_BOTTOM){
        CoordinatePair originalCoords = coords;
        while(coords.start.y<stepchild.SCREEN_HEIGHT){
            coords.start.y+=speed;
            coords.end.y+=speed;
            //make sure y bounds don't glitch out
            if(coords.end.y>stepchild.SCREEN_HEIGHT){
                coords.end.y = stepchild.SCREEN_HEIGHT;
            }
            displayMenu();
        }
        coords = originalCoords;
    }
}