class Knob{
  public:
    Knob();
    Knob(uint8_t,uint8_t,uint8_t);
    uint8_t cc;
    uint8_t val;
    uint8_t channel;//0 is global
    void increment(int8_t);
    void send();
    void set(int16_t val);
};

Knob::Knob(){
  cc = 1;
  val = 0;
  channel = 1;
}
Knob::Knob(uint8_t c, uint8_t v, uint8_t ch){
  cc = c;
  val = v;
  channel = ch;
}
void Knob::increment(int8_t amt){
  if(amt>0){
    val+=amt;
    if(val>127){
      val = 127;
    }
  }
  else if(-amt<=val){
    val+=amt;
  }
  else{
    val = 0;
  }
}
//sets a knob value to a new value
void Knob::set(int16_t newVal){
  if(newVal<0 && newVal<127){
    newVal = 0;
  }
  else if(newVal>127){
    newVal = 127;
  }
  val = newVal;
}
void Knob::send(){
  MIDI.sendCC(cc,val,channel);
}