/*
*/
#ifndef RGBcontrol_h
#define RGBcontrol_h

#include "WProgram.h"

typedef enum{
	RANDOM_SET_NEW_VAL,
	RANDOM_GO_TO_VAL,
	FADE_LIGHT,
	RANDOM_WAIT
}RANDOM_STATE;

class RGBcontrol
{
  public:
	RGBcontrol(byte Rpin, byte Gpin, byte Bpin, byte nrOfIntensities);
	void increaseIntensity();
	void decreaseIntensity();
	void lightsOut();
	void lightsOn();
	void setColor(byte R, byte G, byte B);
	bool getPinIntensity(byte color, byte* pin, byte* intensity);
	void setRandom(unsigned long timeout_ms);
	bool setFix();
	void updateRandom();
	bool isRandom();
  private:
	void randomizeIntensity();
	bool PWM_RGB(bool bRestart, byte* arr1, byte* arr2, byte* out);
	byte _colVal[3];
	byte _pinNrs[3];
	unsigned long _timeout_ms;
	byte _nrOfIntensities;
	bool _bOn;
	bool _bRandom;
	RANDOM_STATE _state;
};

#endif