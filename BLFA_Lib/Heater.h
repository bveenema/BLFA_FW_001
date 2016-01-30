/*
** Heater.h
** Controls low level functions of the heater
** Output:
** Input: Temperature, assumes unit is Celcius
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 12/23/15
*/

#ifndef Heater_h
#define Heater_h

#define MAXTEMP 3800
#define MINTEMP 0

#include "application.h"

class Heater{
public:
	Heater(uint8_t pin, uint16_t setpoint);
  void update(uint16_t temperature);
  void setTarget(uint16_t temperature);
	uint16_t getTarget(void);
  bool isON(void);
private:
  void ON();
  void OFF();

	uint8_t _pin;
  bool _isON;
  uint16_t _setpoint;

};

#endif
