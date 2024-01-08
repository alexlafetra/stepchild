//Overloaded stringify function to create strings
#ifndef HEADLESS
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
  String stringify(const char * a){
    return String(a);
  }
  int toInt(String s){
    return s.toInt();
  }
#else
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
#endif

//Formats a number 1-13 into its Roman Numeral equivalent
//This is really only used for printing intervals in Roman Numeral notation
//eg. printing the 3rd degree of a scale as "III"
String decimalToNumeral(int dec){
  String numeral = "";
  if(dec < 4){
    for(int i = 0; i<dec; i++){
      numeral+="I";
    }
  }
  if(dec == 4)
    numeral = "IV";
  if(dec>4 && dec<9){
    numeral = "V";
    for(int i = 5; i<dec; i++){
      numeral += "I";
    }
  }
  if(dec == 9)
    numeral = "IX";
  if(dec>9 && dec<13){
    numeral = "X";
    for(int i = 10; i<dec; i++){
      numeral += "I";
    }
  }
  return numeral;
}