
// #define SDA_PIN 0
// #define SCL_PIN 1
#define INTERRUPT_PIN 3

#define LB_ADDR 0x28 //0101000

/*
Super helpful thread from working with the MPR121 w/ Wire.h
https://forum.arduino.cc/t/how-to-read-a-register-value-using-the-wire-library/206123/2
*/

uint8_t status = 88;
//Reverse bits in a byte [CITE]
unsigned char reverseBits(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void print2BytesToSerial(uint16_t b){
	for(uint8_t i = 0; i<16; i++){
		Serial.print(b>>i & 0b0000000000000001);
	}
}

#ifndef HEADLESS

// Redundant Include
// #include <Wire.h>
// #include "CAP1214.h"
#include "libraries/MCP23017/src/MCP23017.cpp"

void debugDumpRegisters(MCP23017 mcp){
  	uint8_t conf = mcp.readRegister(MCP23017Register::IODIR_A);
	Serial.print("IODIR_A : ");
	Serial.print(conf, BIN);
	Serial.println();
	
	conf = mcp.readRegister(MCP23017Register::IODIR_B);
	Serial.print("IODIR_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IPOL_A);
	Serial.print("IPOL_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IPOL_B);
	Serial.print("IPOL_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPINTEN_A);
	Serial.print("GPINTEN_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPINTEN_B);
	Serial.print("GPINTEN_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::DEFVAL_A);
	Serial.print("DEFVAL_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::DEFVAL_B);
	Serial.print("DEFVAL_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCON_A);
	Serial.print("INTCON_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCON_B);
	Serial.print("INTCON_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::IOCON);
	Serial.print("IOCON : ");
	Serial.print(conf, BIN);
	Serial.println();

	//conf = mcp.readRegister(IOCONB);
	//Serial.print("IOCONB : ");
	//Serial.print(conf, BIN);
	//Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPPU_A);
	Serial.print("GPPU_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPPU_B);
	Serial.print("GPPU_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTF_A);
	Serial.print("INTF_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTF_B);
	Serial.print("INTF_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCAP_A);
	Serial.print("INTCAP_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::INTCAP_B);
	Serial.print("INTCAP_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPIO_A);
	Serial.print("GPIO_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::GPIO_B);
	Serial.print("GPIO_B : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::OLAT_A);
	Serial.print("OLAT_A : ");
	Serial.print(conf, BIN);
	Serial.println();

	conf = mcp.readRegister(MCP23017Register::OLAT_B);
	Serial.print("OLAT_B : ");
	Serial.print(conf, BIN);
	Serial.println();
}

/*
	These addresses are hardware defined by pulling A0, A1, A2 pins high&low on the
	LowerBoard PCB. 
					|A0|A1|A2|
	LED MCP23017:	 1	0  0
	Button MCP23017: 0  1  0

	As per the MCP23017 datasheet, the I2C addresses are encoded as
	7-bit addresses:
	0100{A2,A1,A0}

	so the LEDs:
	0b0100001
	and the Buttons:
	0b0100010
*/

#define LED_I2C_ADDR 0b0100001
#define BUTTON_I2C_ADDR 0b0100010

class LowerBoard{
  public:
  MCP23017 LEDs = MCP23017(LED_I2C_ADDR,Wire);
  MCP23017 Buttons = MCP23017(BUTTON_I2C_ADDR,Wire);
  LowerBoard(){}
  unsigned short int readButtons(){
	// uint8_t buttonsA = this->Buttons.readRegister(MCP23017Register::GPIO_A);
	// uint8_t buttonsB = this->Buttons.readRegister(MCP23017Register::GPIO_B);
	uint8_t buttonsA = this->Buttons.readRegister(MCP23017Register::GPIO_A);
	uint8_t buttonsB = this->Buttons.readRegister(MCP23017Register::GPIO_B);

	return uint16_t(buttonsB)<<8 | uint16_t(buttonsA);
  }
  void writeLEDs(unsigned short int state){
	//swap led 6 and 7, bc of a wiring fuckup!
	uint8_t led6 = 0b00100000 & uint8_t(state);
	uint8_t led7 = 0b01000000 & uint8_t(state);
	if(led6)
		state |= 0b0000000001000000;
	else
		state &= 0b1111111110111111;
	if(led7)
		state |= 0b0000000000100000;
	else
		state &= 0b1111111111011111;

	//Bank A is LEDs 8-15
    uint8_t bankAState = uint8_t(state>>8);
	this->LEDs.writeRegister(MCP23017Register::GPIO_A,bankAState);

	//Bank B is LEDs 0-7
	uint8_t bankBState = uint8_t(state);
	this->LEDs.writeRegister(MCP23017Register::GPIO_B,bankBState);
  }
  void reset(){}
  void initialize(){
    //default startup control state
	this->LEDs.writeRegister(MCP23017Register::IOCON, 0b00100000);
	//setting both banks of IO to outputs
	this->LEDs.writeRegister(MCP23017Register::IODIR_A, 0b00000000);
	this->LEDs.writeRegister(MCP23017Register::IODIR_B, 0b00000000);
	//writing the GPIO pins to high, which set the values the Output Latch Registers will turn on&off
	this->LEDs.writeRegister(MCP23017Register::GPIO_A,0b11111111);
	this->LEDs.writeRegister(MCP23017Register::GPIO_B,0b11111111);

	//default startup settings
	this->Buttons.writeRegister(MCP23017Register::IOCON, 0b00100000);
	//Setting all GPIO's to inputs
	this->Buttons.writeRegister(MCP23017Register::IODIR_A, 0b11111111);
	this->Buttons.writeRegister(MCP23017Register::IODIR_B, 0b11111111);
	//Disabling internal pullup resistors (since we have a pulldown resistor)
	// this->Buttons.writeRegister(MCP23017Register::GPPU_A, 0b00000000);
	// this->Buttons.writeRegister(MCP23017Register::GPPU_B, 0b00000000);
	this->Buttons.writeRegister(MCP23017Register::GPPU_A, 0b11111111);
	this->Buttons.writeRegister(MCP23017Register::GPPU_B, 0b11111111);

	this->Buttons.writeRegister(MCP23017Register::GPIO_A,0b11111111);
	this->Buttons.writeRegister(MCP23017Register::GPIO_B,0b11111111);

  }
  void test(){
	uint8_t i = 0;
	uint16_t oldState = 0;
    while(true){
		// uint16_t state = this->readButtons();
		uint16_t state = this->Buttons.read();
		this->writeLEDs(state);

		i++;
		i%=16;
    }
  }
};

LowerBoard lowerBoard;

#else
DummyLowerBoard lowerBoard;
#endif
