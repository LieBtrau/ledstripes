//#include "WProgram.h"
#include "SharpIR.h"
#include "IRremote.h"

SharpIR::SharpIR(int pinNumber): irrecv(pinNumber)
{
}

/* State machine for decoding incoming Sharp IR-data.
 * Protocol information can be found on: http://www.sbprojects.com/knowledge/ir/sharp.htm
 */
byte SharpIR::SharpDecode(byte* address, unsigned int* uiCommand)
{
	const byte STATE_IDLE=1;
	const byte STATE_CMD1=2;
	const byte STATE_DECODING=3;

  static unsigned int uiSharp1, uiSharp2;
  static byte decodeState=STATE_IDLE;
	//Constants for Sharp decoding state machine

  switch(decodeState)
  {
  case STATE_IDLE:
    if (irrecv.decode(&results))
    {
      uiSharp1=results.value;
      irrecv.resume(); // Receive the next value
      decodeState=STATE_CMD1;
    }
    break;
  case STATE_CMD1:
    if (irrecv.decode(&results))
    {
      uiSharp2=results.value;
      irrecv.resume(); // Receive the next value
      decodeState=STATE_DECODING;
    }
    break;
  case STATE_DECODING:
    decodeState=STATE_IDLE;
    //Check if both have the same address
    if((uiSharp1 & 0x1F) != (uiSharp2 & 0x1F))
    {
      return ERR;
    }
    *address=uiSharp1 & 0x1F;
    
    //Removing 5bit address, leaving only the 10bit data.
    uiSharp1=(uiSharp1>>5) & 0x3FF;
    uiSharp2=(uiSharp2>>5) & 0x3FF;
    //Check if consecutive commands are exact inverse
    if((uiSharp1 ^ uiSharp2)!=0x3FF)
    {
      return ERR;
    }
    //Return the command that has the first bit reset, not the inverted one
    if(uiSharp1&0x200)
    {
      *uiCommand=uiSharp2;
    }
    else
    {
      *uiCommand=uiSharp1;
    }
    return DECODED;
    break;
  }
  return BUSY;
}

void SharpIR::startDecoding()
{
	 irrecv.enableIRIn(); // Start the receiver
}