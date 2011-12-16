/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef SharpIR_h
#define SharpIR_h

typedef unsigned char byte;
#include "IRremote.h"

class SharpIR
{
  public:
	static const byte ERR=0;
	static const byte DECODED=1;
	static const byte BUSY=2;
	SharpIR(int pinNumber);
	byte SharpDecode(byte* address, unsigned int* uiCommand);
	void startDecoding();
  private:
	IRrecv irrecv;
	decode_results results;
};

#endif