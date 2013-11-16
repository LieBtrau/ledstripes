/*
*/
#ifndef RGBcontrol_h
#define RGBcontrol_h

typedef unsigned char byte;

typedef enum{
	RANDOM_SET_NEW_VAL,
	RANDOM_GO_TO_VAL,
	FADE_LIGHT,
	RANDOM_WAIT
}RANDOM_STATE;

class RGBcontrol
{
  public:
	RGBcontrol(byte Rpin, byte Gpin, byte Bpin);
	void updateLEDs();
	void increaseHue();
	void decreaseHue();
	void increaseIntensity();
	void decreaseIntensity();
	void increaseSaturation();
	void decreaseSaturation();
	void lightsOut();
	void lightsOn();
  private:
	void getRGB(int hue, int sat, int val, int colors[3]);
	int _hue;
	int _saturation;
	int _lightness;
	byte _pinNrs[3];
};

#endif
