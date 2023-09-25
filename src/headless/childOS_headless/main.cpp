#define HEADLESS

//headless wrapper which provides substitute classes/functions
//for the arduino library, and includes the openGL setup for emulating the
//screen
#include "headless.h"

//swapping the arduino "String" object for the C++ "string" object (kinda silly, but it works)
#define String string

//include the rest of the files like normal
#include "../../stepchild/stepchild.ino"
