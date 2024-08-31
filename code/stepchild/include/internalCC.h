
//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
struct GlobalModifiers{
  int8_t velocity[2] = {0,0};
  int8_t chance[2] = {0,0};
  int8_t pitch[2] = {0,0};
};

GlobalModifiers globalModifiers;

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      globalModifiers.velocity[0] = channel;
      globalModifiers.velocity[1] = val-63;
      break;
    //chance
    case 1:
      globalModifiers.chance[0] = channel;
      globalModifiers.chance[1] = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      globalModifiers.pitch[0] = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      globalModifiers.pitch[1] = float(val)/float(127) * 48 - 24;
      break;
    //sequenceClock.BPM
    case 3:
      sequenceClock.setBPM(float(val)*2);
      break;
    //swing amount
    case 4:
      sequenceClock.swingAmplitude += val-63;
    //track channel
    case 5:
      break;
  }
}