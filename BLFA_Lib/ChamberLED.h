/*
** ChamberLED.h
** Controls function of the Chamber LED
** Output: PWM on A4
** Input: ON/OFF, Duty Cycle
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 1/31/16
*/
#ifndef ChamberLED_h
#define ChamberLED_h

#define MAXDUTY 40
#define MINDUTY 0

#include "application.h"

class ChamberLED{
public:
	ChamberLED(uint8_t pin, uint8_t brightness);
  void setState(bool state);
  bool getState(void);
  void setBrightness(uint8_t brightness);
	uint16_t getBrightness(void);

private:
  void ON();
  void OFF();

	uint8_t _pin;
  bool _state;
  uint8_t _brightness;

};

#endif
