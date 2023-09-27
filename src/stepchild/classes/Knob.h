class Knob{
  public:
    Knob();
    Knob(uint8_t,uint8_t,uint8_t);
    uint8_t cc;
    uint8_t val;
    uint8_t channel;//0 is global
    void increment(int8_t);
    void send();
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
void Knob::send(){
  sendMIDICC(cc,val,channel);
}