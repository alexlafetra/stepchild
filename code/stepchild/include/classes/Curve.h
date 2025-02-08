enum CurveType:uint8_t{LINEAR_CURVE,SINEWAVE_CURVE,SQUAREWAVE_CURVE,SAWTOOTH_CURVE,TRIANGLE_CURVE,RANDOM_CURVE,NOISE_CURVE};
CurveType operator++(CurveType &c,int) {
  c = static_cast<CurveType>(static_cast<uint8_t>(c) + 1);
  return c;
}
CurveType operator--(CurveType &c,int) {
  c = static_cast<CurveType>(static_cast<uint8_t>(c) - 1);
  return c;
}

class Curve{
    public:
    CurveType type = LINEAR_CURVE;
    //this is just for the osc-based waveforms
    uint16_t period = 96;
    uint16_t phase = 0;
    int8_t amplitude = 48;
    uint8_t yPos = 64;
    Curve(){}
    Curve(CurveType t, uint16_t per, uint16_t ph, int8_t a, uint8_t y){
        type = t;
        period = per;
        phase = ph;
        amplitude = a;
        yPos = y;
    }
    int16_t getValueAt(uint16_t point){
        switch(type){
            //default
            case LINEAR_CURVE:
                break;
            //sinewave
            case SINEWAVE_CURVE:
                return amplitude*sin(float(point+phase)*float(2*PI)/float(period)) + yPos;
            //square wave
            case SQUAREWAVE_CURVE:
                //if you're less than half a period, it's high. Else, it's low
                if((point+phase)%period<period/2)
                    return yPos+amplitude;
                else
                    return yPos-amplitude;
            //saw
            case SAWTOOTH_CURVE:{
                //the slope of each saw tooth is rise/run = amplitude/period
                float slope = float(amplitude*2)/float(period);
                // y = (x+offset)%period * m + yOffset
                return ((point+phase)%period)*slope+yPos-amplitude;
            }
            //triangle
            case TRIANGLE_CURVE:{
                float slope = float(amplitude*2)/float(period/2);
                //point gets % by period (gives position within period). If pos within period is greater than period/2, then the slope should be inverted and you should +amplitude
                if((point+phase)%period>(period/2))
                   return ((point+phase)%period) * (-slope) + yPos + 3*amplitude;
                else
                   return ((point+phase)%period) * slope + yPos - amplitude;
            }
            //random patterns
            case RANDOM_CURVE:
                break;
        }
        return 0;
    }
};