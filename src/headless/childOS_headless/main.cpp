#define HEADLESS
#include <string>

//headless wrapper which provides substitute classes/functions
//for the arduino library, and includes the openGL setup for emulating the
//screen

//swapping the arduino "String" object for the C++ "string" object (kinda silly, but it works)

using namespace std;

string stringify(int a){
    return to_string(a);
}
string stringify(string s){
    return s;
}
int toInt(string s){
    if(s == "")
        return 0;
    else
        return stoi(s);
}

#define String string

#include "../../stepchild/prototypes.h"

#include "headless.h"

//include the rest of the files like normal
#include "../../stepchild/stepchild.ino"
