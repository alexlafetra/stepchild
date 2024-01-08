void restartSerial(unsigned int baud){
  Serial.end();
  delay(1000);
  Serial.begin(baud);
}

void hardReset(){
  rp2040.reboot();
}

void resetUSBInterface(){
}

//update mode
void enterBootsel(){
  // display.clearDisplay();
  // display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  // display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}