#ifdef HEADLESS
#include <string>
#else
#include "stringPatch.h"
#include "classes/Note.h"
#include <vector>
//Overloaded stringify function to create strings so that this code is compatible with both the Arduino String class and the C++ string class
//This works for the Stepchild version of ChildOS
String stringify(int a){
  return String(a);
}
String stringify(uint8_t a){
  return String(a);
}
String stringify(int8_t a){
  return String(a);
}
String stringify(uint16_t a){
  return String(a);
}
String stringify(int16_t a){
  return String(a);
}
String stringify(uint32_t a){
  return String(a);
}
String stringify(int32_t a){
  return String(a);
}
String stringify(std::vector<Note>::size_type a){
  return String(a);
}
String stringify(float a){
  return String(a);
}
// String stringify(long a){
//   return String(a);
// }
String stringify(const char * a){
  return String(a);
}
int toInt(String s){
  return s.toInt();
}
#endif

