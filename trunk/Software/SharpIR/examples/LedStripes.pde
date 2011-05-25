#include <SharpIR.h>

int RECV_PIN = 26;

SharpIR sharpIR(RECV_PIN);

void setup()
{
  Serial.begin(9600);
  sharpIR.startDecoding();
}

void loop() 
{
  unsigned int uiCommand;
  byte yAddress;
  
  if(sharpIR.SharpDecode(&yAddress, &uiCommand)==sharpIR.DECODED)
  {
    Serial.print("Decoded address: ");
    Serial.println(yAddress, HEX);
    Serial.print("Decoded data: ");
    Serial.println(uiCommand, HEX);
  }
}












