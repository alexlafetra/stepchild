#pragma once

class ProgramChange{
    public:
        //value message
        uint8_t val;

        //channel of the PC
        uint8_t channel;

        //bank message
        uint8_t bank;
        //sub bank message
        uint8_t subBank;

        //Sub bank and bank aren't real! but they seem like a logical way to implement it. MIDI actually calls these MSB and LSB
        //vals, and they need to be sent as CC messages
        //(not implemented yet)

        //timestep the PC occurs on
        Timestep timestep;
        ProgramChange(){
        }
        ProgramChange(uint8_t ch, uint8_t b, uint8_t sb, uint8_t v, Timestep t){
            channel = ch;
            bank = b;
            subBank = sb;
            val = v;
            timestep = t;
        }
};
