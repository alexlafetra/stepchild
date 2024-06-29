enum SlideDirection:uint8_t{
    IN_FROM_RIGHT,
    IN_FROM_BOTTOM,
    OUT_FROM_RIGHT,
    OUT_FROM_BOTTOM
};
//generic menu class
class StepchildMenu{
    public:
        //stores coordinate pairs for (start, end) of the menu rectangle (not used by all menus!)
        CoordinatePair coords;
        //stores the current cursor position/active item
        uint8_t cursor = 0;
        //a function that needs to be overridden by each menu's derived class
        virtual void displayMenu(){};
        StepchildMenu(){}
        void slideIn(SlideDirection origin,uint8_t speed){
            if(origin == IN_FROM_RIGHT){
                //store original coords
                CoordinatePair targetCoords = coords;
                //then, offset the menu coordinates
                int16_t offset = screenWidth-coords.start.x;
                coords.start.x = screenWidth;
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
                int16_t offset = screenHeight-coords.start.y;
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
        void slideOut(SlideDirection destination, uint8_t speed){
            if(destination == OUT_FROM_RIGHT){
                CoordinatePair originalCoords = coords;
                while(coords.start.x<screenWidth){
                    coords.start.x+=speed;
                    coords.end.x+=speed;
                    displayMenu();
                }
                coords = originalCoords;
            }
            else if(destination == OUT_FROM_BOTTOM){
                CoordinatePair originalCoords = coords;
                while(coords.start.y<screenHeight){
                    coords.start.y+=speed;
                    coords.end.y+=speed;
                    //make sure y bounds don't glitch out
                    if(coords.end.y>screenHeight){
                        coords.end.y = screenHeight;
                    }
                    displayMenu();
                }
                coords = originalCoords;
            }
        }
};
