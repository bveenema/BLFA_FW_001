/*
** Fridge.h
** Controls low level functions of the fridge
** Output: High or Low signal on D5
** Input: Temperature, assumes unit is Celcius
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 12/23/15
*/

#ifndef Fridge_h
#define Fridge_h

#define MAXTEMP 5000
#define MINTEMP 0
#define MINONTIME 360000 //6 minutes = 360000
#define MINOFFTIME 360000

#include "application.h"

class Fridge{
public:
	Fridge(uint8_t pin);
  void update(int16_t temperature);
	bool stop();
  void setTarget(int16_t temperature);
	uint16_t getTarget(void);
  bool isON(void);
private:
  void ON();
  void OFF();

	uint8_t _pin;
  bool _isON;
  int16_t _setpoint;
	uint32_t _lastOnTime, _lastOffTime;

};

#endif
