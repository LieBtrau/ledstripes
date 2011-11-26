#include "WProgram.h"
#include "RGBcontrol.h"

RGBcontrol::RGBcontrol(byte Rpin, byte Gpin, byte Bpin, byte nrOfIntensities){
	_pinNrs[0]=Rpin;
	_pinNrs[1]=Gpin;
	_pinNrs[2]=Bpin;
	_colVal[0]=20;
	_colVal[1]=20;
	_colVal[2]=20;
	_bOn=true;
	_nrOfIntensities=nrOfIntensities;
}

void RGBcontrol::setColor(byte R, byte G, byte B){
	if(_bOn){
		_bRandom=false;
		_colVal[0]=R;
		_colVal[1]=G;
		_colVal[2]=B;
	}
}

void RGBcontrol::updateRandom(){
	if(_bRandom){
		randomizeIntensity();
	}
}

bool RGBcontrol::getPinIntensity(byte color, byte* pin, byte* intensity){
	if(color>2){
		return false;
	}
	*pin=_pinNrs[color];
	if(!_bOn){
		*intensity=0;
	}else{
		*intensity=_colVal[color];
	}
	return true;
}

void RGBcontrol::increaseIntensity(){
	if(_bOn){
		for(byte i=0;i<3;i++){
			if(_colVal[i]<(_nrOfIntensities-1)){
				_colVal[i]++;
			}
		}
	}
}

void RGBcontrol::decreaseIntensity(){
	if(_bOn){
		for(byte i=0;i<3;i++){
			if(_colVal[i]>0){
				_colVal[i]--;
			}
		}
	}
}

void RGBcontrol::lightsOut(){
	_bOn=false;
}

void RGBcontrol::lightsOn(){
	_bOn=true;
}

bool RGBcontrol::isRandom(){
	return _bRandom;
}

bool RGBcontrol::setFix(){
	if(_bOn){
		_bRandom=false;
	}
}

void RGBcontrol::setRandom(unsigned long b){
	if(_bOn){
		_bRandom=true;
		_state=RANDOM_SET_NEW_VAL;
		_timeout_ms=b;
	}
}

/*Fade a light by PWM. First RGB-value is arr1, this gradually
 *	changes to arr2.
 *\param bRestart		set to true to start the fading cycle from the beginning
 *\param arr1			original RGB-value
 *\param arr2			the new RGB-value.  That's where it will fade to.
 *\param out			the current fading RGB-output value
 *\return				true when fading cycle ended, else false.
 */
bool RGBcontrol::PWM_RGB(bool bRestart, byte* arr1, byte* arr2, byte* out){
  const unsigned long TIMEOUT=1;
  const byte PWM_RESOLUTION=20;
  static byte state=0;
  static unsigned long endTime;
  static byte loopCounter;
  static byte repeat;

  if(bRestart){
    state=0;
  }

  switch(state){
  case 0:
    loopCounter=PWM_RESOLUTION;
    state=1;
    repeat=5;
    break;
  case 1:
    //First show old value
    for(byte i=0;i<3;i++){
      out[i]=arr1[i];
    }
    endTime=millis()+loopCounter*TIMEOUT;
    state=2;
    break;
  case 2:
    //Wait some time
    if(millis()>endTime){
      state=3;
    }
    break;
  case 3:
    //Show new value now
    for(byte i=0;i<3;i++){
      out[i]=arr2[i];
    }
    endTime=millis()+(PWM_RESOLUTION-loopCounter)*TIMEOUT;
    state=4;
    break;
  case 4:
    //Wait some time again
    if(millis()>endTime){
      if(repeat-->0){
        state=1;
      }
      else{
        if(loopCounter-->0){
          repeat=5;
          state=1;
        }
        else{
          state=5;
          return true;
        }
      }
//      Serial.print(repeat,DEC);
//      Serial.print(" ");
//      Serial.println(loopCounter,DEC);
    }
    break;
  case 5:
    break;
  }
  return false;
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
void RGBcontrol::randomizeIntensity(){
  static byte desired[3];
  static byte oldVal[3];
  static byte tempVal[3];
  int total;
  unsigned int intensity;
  static unsigned long startTime;
  unsigned long diffTime;
  int desTotal;
 
  switch(_state){
    case RANDOM_SET_NEW_VAL:
      //Set a new random desired color
      total=_colVal[0] + _colVal[1] + _colVal[2];
	  for(byte i=0;i<3;i++){
		  desired[i]=random(min(_nrOfIntensities-1,total));
			tempVal[i]=_colVal[i];
	  }
	  do{
		  for(byte i=0;i<3;i++){
			  desTotal=desired[0]+desired[1]+desired[2];
			  if(desTotal>total && desired[i]>0){
				desired[i]--;
			  }
			  if(desTotal<total && desired[i]<(_nrOfIntensities-1)){
				desired[i]++;
			  }
		  }
	  }while(total!=desired[0]+desired[1]+desired[2]);
      _state=RANDOM_GO_TO_VAL;
      break;
     case RANDOM_GO_TO_VAL:
       //Bring the current color closer to the desired one.
       for(byte i=0;i<3;i++){
	     //save current light value, so that we can show it mixed with the new value
		 oldVal[i]=tempVal[i];
         if(tempVal[i]>desired[i]){
           tempVal[i]--;
         }
         if(tempVal[i]<desired[i]){
           tempVal[i]++;
         }
       }
       if((tempVal[0]==desired[0]) && (tempVal[1]==desired[1]) && (tempVal[2]==desired[2])){
			//desired color is reached, set a new color
			_state=RANDOM_SET_NEW_VAL;
       }else{
			//start fading from the current level to the new tempVal-level.
			PWM_RGB(true,oldVal,tempVal,_colVal);
			_state=FADE_LIGHT;
       }
       break;
	 case FADE_LIGHT:
		if(PWM_RGB(false,oldVal,tempVal,_colVal)){
			//Start the delay loop
			startTime=millis();
			_state=RANDOM_WAIT;
		 }
		break;
     case RANDOM_WAIT:
       //Delay loop
       if(millis()-startTime > _timeout_ms){
         _state=RANDOM_GO_TO_VAL;
       }
	break;
  }
}



