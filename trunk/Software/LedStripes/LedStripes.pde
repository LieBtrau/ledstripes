/* Leduino
 * 
 * This programs receives commands from a Sharp remote control and uses that to control the colors and intensity of a three strings of LEDs.
 * Using the remote is simple:
 *    red standby button : all LEDs off
 *    Number 0 to 9 : set a predefined color (hard coded in the code below)
 *    Volume buttons: change intensity of the LEDs
 *    OK-button: use it to enable/disable automatic color changing mode.
 *    P up/down: enable/disable different LED strings
 */

#include <SharpIR.h>
#include <RGBcontrol.h>

int RECV_PIN = 22;
int PS_ON_PIN = 26;
int _5V_ON_PIN = 28;

//IR data constants - Buttons of remote control
const long BT_STANDBY=0x01116;
const long BT_1=0x1101;
const long BT_0=0x110A;
const long BT_VOL_UP=0x1114;
const long BT_VOL_DOWN=0x1115;
const long BT_OK=0x1152;
const long BT_P_UP=0x1111;
const long BT_P_DOWN=0x1112;
const long BT_KEY_UP=0x1157;
const long BT_KEY_DOWN=0x1120;
const long BT_KEY_LEFT=0x11D7;
const long BT_KEY_RIGHT=0x11D8;

//Timeout in milliseconds is used during random mode.  It determines how fast the LEDs change color.  Now it's set to 1000, so every second the intensity of 
//the LEDs will be updated.
const unsigned long TIMEOUT_MS=10000;

//Using exponential intensity changes, because the eye response is logarithmically. (Use linear changes for output values near 255 and you'll see what I mean).
const byte nrOfIntensities=40;
const byte intensity[nrOfIntensities]={
  0 ,  1,  2,  3,  4,  5,  6,  7,  8,  9,
  10, 11, 12, 13, 15, 17, 19, 21, 24, 26,
  30, 33, 37, 42, 47, 52, 58, 65, 73, 82,
  92,103,115,129,144,162,181,203,227,255};

//The color map defines ten colors, selectable by the numeric buttons on the remote control.
const byte colormap[][3]={
  {0,0,39},{0,39,0},{0,39,39},{39,0,0},{39,0,39},{39,39,0},{39,39,39},{19,39,39},{39,19,39},{39,39,19}
};

//state constants
const byte SLEEPING=0;    //All LEDs off
const byte ACTIVE=1;      //LEDs are on and color is constant

////Used LED pins on the Leduino
//const byte Rpin=5;
//const byte Gpin=6;
//const byte Bpin=13;

//Initialize the IR-remote receiver
SharpIR sharpIR(RECV_PIN);

//Set up LED strings
//RGBcontrol string1(5,6,13,nrOfIntensities);    //LED string for debugging purposes
RGBcontrol string1(2,3,4,nrOfIntensities);
RGBcontrol string2(7,8,9,nrOfIntensities);
RGBcontrol string3(10,11,12,nrOfIntensities);

void setup()
{
  //Serial.begin(9600);
  //Random number generator is used during "CHANGING" state.  Otherwise the color changes are always the same.
  randomSeed(analogRead(0));
  //Start waiting for IR-messages
  sharpIR.startDecoding();
  pinMode(PS_ON_PIN,OUTPUT);
  pinMode(_5V_ON_PIN, OUTPUT);
}

void loop() 
{
  unsigned int uiCommand;        //IR command (10 bits)
  byte yAddress;                 //IR address (5 bits) 
  long lIRdata;                  //data received from IR remote
  static byte lState=SLEEPING;   //state of the state machine (sleeping, active or changing)
  byte intens;
  byte pin;
  static byte ledOnState=7;      //Holds the status of the different LED strings (ON or OFF)
  
  //Change state when user pressed button on the remote control
  if(sharpIR.SharpDecode(&yAddress, &uiCommand)==sharpIR.DECODED)
  {
    lIRdata=((long)yAddress)<<12 | uiCommand;
    //Serial.println(lIRdata,HEX);
    switch(lState)
    {
      case SLEEPING:
        //wake from sleep
        if(lIRdata!=BT_STANDBY)
        {          
          lState=ACTIVE;
        }
        break;
      case ACTIVE:
        if(lIRdata==BT_STANDBY)
        {          
          lState=SLEEPING;
          ledOnState=0;
        }
        if(lIRdata==BT_OK)
        {
          if(string1.isRandom()){
            string1.setFix();
          }else{
            string1.setRandom(TIMEOUT_MS);
          }
          if(string2.isRandom()){
            string2.setFix();
          }else{
            string2.setRandom(TIMEOUT_MS);
          }
          if(string3.isRandom()){
            string3.setFix();
          }else{
            string3.setRandom(TIMEOUT_MS);
          }
        }
        break;
    }//switch
    
    if(lIRdata==BT_STANDBY)
    {
      //User pushed standby
      ledOnState=0;
      //power off;
      digitalWrite(PS_ON_PIN,LOW);
      digitalWrite(_5V_ON_PIN,LOW);
    }
    else
    {
      //power on;
      digitalWrite(PS_ON_PIN,HIGH);
      digitalWrite(_5V_ON_PIN,HIGH);
    }
    if(lIRdata>=BT_1 && lIRdata<=BT_0)
    {
      //User pushed a numeric button: so set the predefined color
      string1.setColor(
        colormap[lIRdata-BT_1][0],
        colormap[lIRdata-BT_1][1],
        colormap[lIRdata-BT_1][2]);
      string2.setColor(
        colormap[lIRdata-BT_1][0],
        colormap[lIRdata-BT_1][1],
        colormap[lIRdata-BT_1][2]);
      string3.setColor(
        colormap[lIRdata-BT_1][0],
        colormap[lIRdata-BT_1][1],
        colormap[lIRdata-BT_1][2]);
    }
    if(lIRdata==BT_VOL_UP || lIRdata==BT_KEY_RIGHT){
      //increase intensity
      string1.increaseIntensity();
      string2.increaseIntensity();
      string3.increaseIntensity();
    }
    if(lIRdata==BT_VOL_DOWN || lIRdata==BT_KEY_LEFT){
      //decrease intensity
      string1.decreaseIntensity();
      string2.decreaseIntensity();
      string3.decreaseIntensity();
    }
    if(lIRdata==BT_P_UP){
      if(ledOnState<7){
        ledOnState++;
      }else{
        ledOnState=1;
      }
    }
    if(lIRdata==BT_P_DOWN){
      if(ledOnState>1){
        ledOnState--;
      }else{
        ledOnState=7;
      }
    }
    
  }//if valid IR remote code received
  
  //Process which strings must be on
  if((ledOnState & 1)!=0){
    string1.lightsOn();
  }else{
    string1.lightsOut();
  }
  if((ledOnState & 2)!=0){
    string2.lightsOn();
  }else{
    string2.lightsOut();
  }
  if((ledOnState & 4)!=0){
    string3.lightsOn();
  }else{
    string3.lightsOut();
  }
  
  string1.updateRandom();
  string2.updateRandom();
  string3.updateRandom();
 
  //Write the analog values to the IO pins.
  for(byte i=0;i<3;i++)
  {
   string1.getPinIntensity(i,&pin,&intens);
   analogWrite(pin, intensity[intens]);
   string2.getPinIntensity(i,&pin,&intens);
   analogWrite(pin, intensity[intens]);
   string3.getPinIntensity(i,&pin,&intens);
   analogWrite(pin, intensity[intens]);
  }
 
  
  
}//loop


















