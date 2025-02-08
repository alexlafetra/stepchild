
//each of the modifiers stores a channel, and a value
//the parameter gets modified
//gets added to notes in the vel modifier channel
//0 is the global channel
struct ccChannelValue{
  uint8_t channel = 0;
  uint8_t value = 0;
};

struct GlobalModifiers{
  ccChannelValue velocity;
  ccChannelValue chance;
  ccChannelValue pitch;
};

GlobalModifiers globalModifiers;

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      globalModifiers.velocity.channel = channel;
      globalModifiers.velocity.value = val-63;
      break;
    //chance
    case 1:
      globalModifiers.chance.channel = channel;
      globalModifiers.chance.value = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      globalModifiers.pitch.channel = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      globalModifiers.pitch.value = float(val)/float(127) * 48 - 24;
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