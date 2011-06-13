/* Leduino
 * 
 * This programs receives commands from a Sharp remote control and uses that to control the colors and intensity of a string of LEDs.
 * Using the remote is simple = red standby button = all LEDs off
 *    Number 0 to 9 : set a predefined color (hard coded in the code below)
 *    Volume buttons: change intensity of the LEDs
 *    OK-button: use it to enable/disable automatic color changing mode.
 */

#include <SharpIR.h>

int RECV_PIN = 22;
int nPS_ON_PIN = 26;
int n5V_ON_PIN = 28;

//IR data constants - Buttons of remote control
const long BT_STANDBY=0x01116;
const long BT_1=0x1101;
const long BT_0=0x110A;
const long BT_UP=0x1114;
const long BT_DOWN=0x1115;
const long BT_OK=0x1152;

//Using exponential intensity changes, because the eye response is logarithmically. (Use linear changes for output values near 255 and you'll see what I mean).
const byte nrOfIntensities=40;
const byte intensity[nrOfIntensities]={
  0 ,  1,  2,  3,  4,  5,  6,  7,  8,  9,
  10, 11, 12, 13, 15, 17, 19, 21, 24, 26,
  30, 33, 37, 42, 47, 52, 58, 65, 73, 82,
  92,103,115,129,144,162,181,203,227,255};
  
const byte colormap[][3]={
  {0,0,39},{0,39,0},{0,39,39},{39,0,0},{39,0,39},{39,39,0},{39,39,39},{19,39,39},{39,19,39},{39,39,19}
};

//state constants
const byte SLEEPING=0;    //All LEDs off
const byte ACTIVE=1;      //LEDs are on and color is constant
const byte CHANGING=2;    //LEDs are on and color is changing

//Used LED pins on the Arduino
const byte Rpin=5;
const byte Gpin=6;
const byte Bpin=13;

//Initialize the IR-remote receiver
SharpIR sharpIR(RECV_PIN);

void setup()
{
  Serial.begin(9600);
  //Random number generator is used during "CHANGING" state.  Otherwise the color changes are always the same.
  randomSeed(analogRead(0));
  //Start waiting for IR-messages
  sharpIR.startDecoding();
  pinMode(nPS_ON_PIN,OUTPUT);
  pinMode(n5V_ON_PIN, OUTPUT);
}

void loop() 
{
  unsigned int uiCommand;        //IR command (10 bits)
  byte yAddress;                 //IR address (5 bits) 
  long lIRdata;                  //data received from IR remote
  static byte lState=SLEEPING;   //state of the state machine (sleeping, active or changing)
  static unsigned int colVal[3]; //current analog value for each of the three colors.

  //Change state when user pressed button on the remote control
  if(sharpIR.SharpDecode(&yAddress, &uiCommand)==sharpIR.DECODED)
  {
    lIRdata=((long)yAddress)<<12 | uiCommand;
    
    switch(lState){
      case SLEEPING:
        //wake from sleep
        if(lIRdata!=BT_STANDBY){          
          lState=ACTIVE;
        }
        break;
      case ACTIVE:
        if(lIRdata==BT_STANDBY){          
          lState=SLEEPING;
        }
        if(lIRdata==BT_OK){
          lState=CHANGING;
          randomIntensity(colVal,0); 
        }
        break;
      case CHANGING:
        if(lIRdata==BT_STANDBY){          
          lState=SLEEPING;
        }
        if(lIRdata==BT_OK){
          lState=ACTIVE;
        }
        break;
    }
    
    if(lIRdata==BT_STANDBY){
      //User pushed standby
      for(byte i=0;i<3;i++){
        colVal[i]=0;
      }
      //power off;
      digitalWrite(nPS_ON_PIN,LOW);
      digitalWrite(n5V_ON_PIN,LOW);
    }else{
      //power on;
      digitalWrite(nPS_ON_PIN,HIGH);
      digitalWrite(n5V_ON_PIN,HIGH);
    }
    if(lIRdata>=BT_1 && lIRdata<=BT_0){
      //User pushed a numeric button: so set the predefined color
      for(byte i=0;i<3;i++){
        colVal[i]=colormap[lIRdata-BT_1][i];
      }
    }
    if(lIRdata==BT_UP){
      for(byte i=0;i<3;i++){
        if(colVal[i] < nrOfIntensities-1){
          //increase intensity
          colVal[i]++;
        }
      }
    }
    if(lIRdata==BT_DOWN){
      for(byte i=0;i<3;i++){
        if(colVal[i] > 0){
          //increase intensity
          colVal[i]--;
        }
      }
    }
  }//if valid IR remote code received

  if(lState==CHANGING){
    //vary colors randomly
    randomIntensity(colVal,1); 
  }
  
  //Write the analog values to the IO pins.
  analogWrite(Rpin, intensity[colVal[0]]);
  analogWrite(Gpin, intensity[colVal[1]]);
  analogWrite(Bpin, intensity[colVal[2]]);  
  
  analogWrite(10, intensity[colVal[0]]);
  analogWrite(11, intensity[colVal[1]]);
  analogWrite(12, intensity[colVal[2]]);  

}

/* State machine that takes care of the random color changes.
 * The first stage defines a new desired color (R,G,B).  To avoid that the
 * intensity of the new color would be very different from the current color,
 * the total lightlevel of the three LEDs will remain unchanged.
 * In the second state, the color indices are increased, so that the intensity
 * of each LED approaches the desired index for that LED (R, G or B).
 * If the desired color is attained, then a new random color is chosen and the
 * process starts anew. 
 */
void randomIntensity(unsigned int curVal[], bool nReset){
  const unsigned long TIMEOUT_MS=1000;
  static byte state=0;
  static byte desired[3];
  static unsigned long startTime;
  unsigned int total;
  unsigned int intensity;
  if(nReset==0){
    state=0;
  }
  
  switch(state){
    case 0:
      //Set a new desired color
      total=curVal[0] + curVal[1] + curVal[2];
      for(byte i=0;i<2;i++){
        intensity=random(0,total);
        //set new desired value
        if(intensity<nrOfIntensities){
          desired[i]=intensity;
        }else{
          desired[i]=nrOfIntensities-1;
        }
        total-=desired[i];
      }
      desired[2]=total;
      state=1;
      break;
     case 1:
       //Bring the current color closer to the desired one.
       for(byte i=0;i<3;i++){
         if(curVal[i]>desired[i]){
           curVal[i]--;
         }
         if(curVal[i]<desired[i]){
           curVal[i]++;
         }
       }
       if((curVal[0]==desired[0]) && (curVal[1]==desired[1]) && (curVal[2]=desired[2])){
         //desired color is reached, set a new color
         state=0;
       }else{
         //Start the delay loop
         startTime=millis();
         state=2;
       }
       break;
     case 2:
       //Delay loop
       if(millis()-startTime > TIMEOUT_MS){
         state=1;
       }
     break;
  }
}

















