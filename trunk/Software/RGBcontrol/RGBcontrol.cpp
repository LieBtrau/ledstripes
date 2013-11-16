//http://www.kasperkamperman.com/blog/arduino/arduino-programming-hsb-to-rgb/

#include <Arduino.h>
#include "RGBcontrol.h"

/* 
  dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
  Used in the getRGB function on saturation and brightness to make 'dimming' look more natural. 
  Exponential function used to create lightnessues below : 
  x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)   
*/
const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

RGBcontrol::RGBcontrol(byte Rpin, byte Gpin, byte Bpin){
	_pinNrs[0]=Rpin;
	_pinNrs[1]=Gpin;
	_pinNrs[2]=Bpin;
	_hue=180;
	_saturation=128;
	_lightness=128;
}

void RGBcontrol::updateLEDs(){
	// getRGB function stores RGB lightnessues in this array
	// use these lightnessues for the red, blue, green led. 
	int rgb_colors[3]; 
	static int fadeSpeed=1;
  getRGB(_hue,_saturation,_lightness,rgb_colors);   // converts HSB to RGB
      
  analogWrite(_pinNrs[0], rgb_colors[0]);            // red lightnessue in index 0 of rgb_colors array
  analogWrite(_pinNrs[1], rgb_colors[1]);            // green lightnessue in index 1 of rgb_colors array
  analogWrite(_pinNrs[2], rgb_colors[2]);            // blue lightnessue in index 2 of rgb_colors array

  // fade from 0 - 255 and back with a certain speed   
  _hue = _hue + fadeSpeed;         // change fadelightness by speed
  _hue = constrain(_hue, 0, 255);  // keep fadelightness between 0 and 255
  
  if(_hue==255 || _hue==0)         // change from up>down or down-up (negative/positive)
  { fadeSpeed = -fadeSpeed;  
  }  
}

void RGBcontrol::increaseHue()
{
	_hue=(_hue<359?_hue+1:359);
}

void RGBcontrol::decreaseHue()
{
	_hue=(_hue>0?_hue-1:0);
}

void RGBcontrol::increaseSaturation()
{
	_saturation=(_saturation<255?_saturation+1:255);
}

void RGBcontrol::decreaseSaturation()
{
	_saturation=(_saturation>0?_saturation-1:0);
}

void RGBcontrol::increaseIntensity()
{
	_lightness=(_lightness<255?_lightness+1:255);
}

void RGBcontrol::decreaseIntensity()
{
	_lightness=(_lightness>0?_lightness-1:0);
}

void RGBcontrol::lightsOut(){
	_lightness=0;
}

void RGBcontrol::lightsOn(){
	_lightness=128;
}

void RGBcontrol::getRGB(int hue, int sat, int lightness, int colors[3]) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/lightnessue and on saturation (inverted).
     This looks the most natural.      
  */
  
  lightness = dim_curve[lightness];
  sat = 255-dim_curve[255-sat];
  
  int r;
  int g;
  int b;
  int base;
  
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=lightness;
    colors[1]=lightness;
    colors[2]=lightness;  
  } else  { 
    
    base = ((255 - sat) * lightness)>>8;
  
    switch(hue/60) {
	case 0:
		r = lightness;
		g = (((lightness-base)*hue)/60)+base;
		b = base;
	break;
	
	case 1:
		r = (((lightness-base)*(60-(hue%60)))/60)+base;
		g = lightness;
		b = base;
	break;
	
	case 2:
		r = base;
		g = lightness;
		b = (((lightness-base)*(hue%60))/60)+base;
	break;
	
	case 3:
		r = base;
		g = (((lightness-base)*(60-(hue%60)))/60)+base;
		b = lightness;
	break;
	
	case 4:
		r = (((lightness-base)*(hue%60))/60)+base;
		g = base;
		b = lightness;
	break;
	
	case 5:
		r = lightness;
		g = base;
		b = (((lightness-base)*(60-(hue%60)))/60)+base;
	break;
    }
      
    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   

}

